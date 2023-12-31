// #include "Arduino.h"
#include <TensorFlowLite_ESP32.h>

#include "tensorflow/lite/c/common.h"
#include "tensorflow/lite/micro/micro_error_reporter.h"

#include "p_det_model.h"
#include "model_settings.h"

#include "tensorflow/lite/micro/all_ops_resolver.h"
#include "tensorflow/lite/micro/micro_error_reporter.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/schema/schema_generated.h"

#include <esp_heap_caps.h>

#include "esp_camera.h"

#include "img_converters.h"
#include "Free_Fonts.h"

#include "soc/soc.h" // Disable brownout problems
#include "soc/rtc_cntl_reg.h" // Disable brownout problems


#include "tst_img.h"

#define CAMERA_MODEL_AI_THINKER // Has PSRAM
#define VERBOSE false

#include "camera_pins.h"
#include "downsample.h"
// #include "tft_stuff.h"

unsigned long tm_to_disp;

// #include <fb_gfx.h>
const char* kCategoryLabels[kCategoryCount] = {
    "A", "B", "C", "D", "E", "F", "G", "H", "I", "K", "L", "M",
                        "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y"
};


camera_fb_t * fb = NULL;

//tflite stuff
tflite::ErrorReporter* error_reporter = nullptr;
const tflite::Model* model = nullptr;
tflite::MicroInterpreter* interpreter = nullptr;
TfLiteTensor* input = nullptr;

// In order to use optimized tensorflow lite kernels, a signed int8_t quantized
// model is preferred over the legacy unsigned model format. This means that
// throughout this project, input images must be converted from unisgned to
// signed format. The easiest and quickest way to convert from unsigned to
// signed 8-bit integers is to subtract 128 from the unsigned value to get a
// signed value.

#ifdef CONFIG_IDF_TARGET_ESP32S3
constexpr int scratchBufSize = 39 * 1024;
#else
constexpr int scratchBufSize = 0;
#endif
// An area of memory to use for input, output, and intermediate arrays.
constexpr int kTensorArenaSize = 81 * 1024 + scratchBufSize; //81 * 1024 + scratchBufSize;
static uint8_t *tensor_arena;//[kTensorArenaSize]; // Maybe we should move this to external


void init_camera(){
  if(VERBOSE) Serial.println("Initializing Camera");
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_RGB565;//PIXFORMAT_GRAYSCALE;//PIXFORMAT_JPEG;//PIXFORMAT_RGB565;// 
  config.fb_location = CAMERA_FB_IN_PSRAM;
  config.frame_size = FRAMESIZE_96X96;//FRAMESIZE_QVGA;//FRAMESIZE_96X96;//
  
  config.jpeg_quality = 12;
  config.fb_count = 2;

  // camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    delay(1000);
    ESP.restart();
  }
  if(VERBOSE) Serial.println("Camera Initialized");
}


void setup() {
  
  // put your setup code here, to run once:
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);//disable brownout detector

  Serial.begin(115200);
  if(VERBOSE) Serial.println("Hello World");
  init_camera();
  
  dstImage = (uint16_t *) malloc(DST_WIDTH * DST_HEIGHT*2);
  if(VERBOSE) Serial.println("dstImage allocated");
  delay(200);
  static tflite::MicroErrorReporter micro_error_reporter;
  error_reporter = &micro_error_reporter;
  if(VERBOSE) Serial.println("error_reporter initialized");

  model = tflite::GetModel(ASL_1_lite_tflite);
  if (model->version() != TFLITE_SCHEMA_VERSION) {
    TF_LITE_REPORT_ERROR(error_reporter,
                         "Model provided is schema version %d not equal "
                         "to supported version %d.",
                         model->version(), TFLITE_SCHEMA_VERSION);
    return;
  }
  Serial.println("model loaded");
  if (tensor_arena == NULL) {
    tensor_arena =  (uint8_t *) heap_caps_malloc(kTensorArenaSize, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
  }
  Serial.println("tensor_arena allocated");
  if (tensor_arena == NULL) {
    printf("Couldn't allocate memory of %d bytes\n", kTensorArenaSize);
    return;
  }

  // Pull in only the operation implementations we need.
  // This relies on a complete list of all the ops needed by this graph.
  // An easier approach is to just use the AllOpsResolver, but this will
  // incur some penalty in code space for op implementations that are not
  // needed by this graph.
  //
  
  // tflite::AllOpsResolver resolver;
  /*
  static tflite::MicroInterpreter static_interpreter(
      model, resolver, tensor_arena, kTensorArenaSize, error_reporter);
      //
      */
  // NOLINTNEXTLINE(runtime-global-variables)
  //
  if(VERBOSE) Serial.println("micro_op_resolver initializing");
  static tflite::MicroMutableOpResolver<9> micro_op_resolver;
  //micro_op_resolver.AddAveragePool2D();
  micro_op_resolver.AddMaxPool2D();
  //micro_op_resolver.AddReshape();
  micro_op_resolver.AddFullyConnected();
  micro_op_resolver.AddConv2D();
  micro_op_resolver.AddDepthwiseConv2D();
  // micro_op_resolver.AddReshape();
  micro_op_resolver.AddSoftmax();
  micro_op_resolver.AddQuantize();
  micro_op_resolver.AddDequantize();
  if(VERBOSE) Serial.println("micro_op_resolver initialized");
  
  

  // Build an interpreter to run the model with.
  // NOLINTNEXTLINE(runtime-global-variables)
  if(VERBOSE) Serial.println("static_interpreter initializing");
  static tflite::MicroInterpreter static_interpreter(
      model, micro_op_resolver, tensor_arena, kTensorArenaSize, error_reporter);
  
      ////
  
  if(VERBOSE) Serial.println("static_interpreter initialized");
  interpreter = &static_interpreter;
  if(VERBOSE) Serial.println("interpreter initialized");

  // Allocate memory from the tensor_arena for the model's tensors.
  TfLiteStatus allocate_status = interpreter->AllocateTensors();
  if (allocate_status != kTfLiteOk) {
    TF_LITE_REPORT_ERROR(error_reporter, "AllocateTensors() failed");
    return;
  }
  if(VERBOSE) Serial.println("tensors allocated");

  // Get information about the memory area to use for the model's input.
  input = interpreter->input(0);
  pinMode(4, OUTPUT);
  if(VERBOSE) Serial.println("input initialized");
}

void loop() {
  // put your main code here, to run repeatedly:
  if(VERBOSE) Serial.println("loop");
  for (int i = 0; i<1; i++){
  fb = esp_camera_fb_get();
    if (!fb) {
      Serial.println("Camera capture failed");
    }
    if(fb){
      esp_camera_fb_return(fb);
      fb=NULL;
    }
  delay(1);
  }
  if(VERBOSE) Serial.println("camera capture");

  fb = esp_camera_fb_get();
    if (!fb) {
      Serial.println("Camera capture failed");
    }
  uint16_t * tmp = (uint16_t *) fb->buf;
    if(VERBOSE) Serial.println("downsampling");
    downsampleImage((uint16_t *) fb->buf, fb->width, fb->height);
    if(VERBOSE) Serial.println("downsampled");

    for (int y = 0; y < DST_HEIGHT; y++) {
      for (int x = 0; x < DST_WIDTH; x++) {
        tmp[y*(fb->width) + x] = (uint16_t) dstImage[y*DST_WIDTH +x];
      }
    }
    if(VERBOSE) Serial.println("upsampling");
    // upsample((uint16_t *) fb->buf);
    delay(15);
    if(VERBOSE) Serial.println("upsampled");

    if(fb){
      esp_camera_fb_return(fb);
      fb = NULL;
    }
    if(VERBOSE) Serial.println("fb returned");
    delay(15);
  
    //digitalWrite(4, LOW);
    if(VERBOSE) Serial.println("invoking");
    int8_t * image_data = input->data.int8;

    for (int i = 0; i < kNumRows; i++) {
      for (int j = 0; j < kNumCols; j++) {
        uint16_t pixel = ((uint16_t *) (dstImage))[i * kNumCols + j];

        // for inference
        uint8_t hb = pixel & 0xFF;
        uint8_t lb = pixel >> 8;
        uint8_t r = (lb & 0x1F) << 3;
        uint8_t g = ((hb & 0x07) << 5) | ((lb & 0xE0) >> 3);
        uint8_t b = (hb & 0xF8);

        /**
        * Gamma corected rgb to greyscale formula: Y = 0.299R + 0.587G + 0.114B
        * for effiency we use some tricks on this + quantize to [-128, 127]
        */
        int8_t grey_pixel = ((305 * r + 600 * g + 119 * b) >> 10) - 128;

        // int8_t grey_pixel = (int8_t) ((int) tst_img1[i * kNumCols + j]-128);

        image_data[i * kNumCols + j] = grey_pixel;

        // float grey_pixel = (float) ((305 * r + 600 * g + 119 * b) >> 10);// - 128;
        // grey_pixel=grey_pixel/255.0;

        // image_data[i * kNumCols + j] = grey_pixel;
      }
    }
    if(VERBOSE) Serial.println("image_data set");
  if (kTfLiteOk != interpreter->Invoke()) {
    TF_LITE_REPORT_ERROR(error_reporter, "Invoke failed.");
  }
  if(VERBOSE) Serial.println("invoked");
  TfLiteTensor* output = interpreter->output(0);
  if(VERBOSE) Serial.println("output set");

  // Process the inference results.
  // int8_t max_confidence = -127;
  int idx = 0;
  int idx2 = 0;
  uint8_t max_confidence = output->data.uint8[idx];
  uint8_t cur_confidence = 0;
  float max_tmp = -10000.0;
  for(int i = 0; i < kCategoryCount; i++){
    float tmp=output->data.f[i];
    // Serial.println(String(tmp));
    
    cur_confidence = output->data.uint8[i];
    
    if(max_confidence < cur_confidence){
      idx2=idx;
      idx = i;
      max_confidence = cur_confidence;
    }
    if (tmp > max_tmp){
      // idx = i;
      max_tmp = tmp;
    }
  }
  Serial.println("Letter: ");
  Serial.print(kCategoryLabels[idx]);
  Serial.print(" Confidence: ");
  Serial.println(max_confidence);
  //delay(2000);
  Serial.println("displaying");
  delay(10);

}
