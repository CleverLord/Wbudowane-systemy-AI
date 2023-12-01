from typing import Any, Tuple, Callable
import time
import numpy as np
import cv2

# ################################################################################################################################
# Common
# ################################################################################################################################

def get_distinct_colors(n_hues: int=10, n_sats: int=1, n_brigths: int=1) -> list:
  """Get list of distinctive colors.

  Args:
      n_hues (int, optional): Number of hues to choose from. Hues are equally distributed along full range. Defaults to 10.
      n_sats (int, optional): Number of saturation levels to choose from. Levels are equally distributed along full range. Defaults to 1.
      n_brigths (int, optional): Number of brightness levels to choose from. Levels are equally distributed along full range. Defaults to 1.

  Returns:
      list: List of distinctive colors in RGB format.
  """
  hues    = np.linspace(0, 179, n_hues)
  sats    = np.linspace(0, 255, n_sats+1)[1:]
  brights = np.linspace(0, 255, n_brigths+1)[1:]
  color_list = np.expand_dims(np.array([[h,s,v] for h in hues for s in sats for v in brights]), axis=0)
  color_list = cv2.cvtColor(color_list.astype(np.uint8), code=cv2.COLOR_HSV2BGR)[0, :, :]
  color_list = [(int(c[0]), int(c[1]), int(c[2])) for c in color_list]
  return color_list

def draw_stats(img, infer_time, app_time) -> None:
  """Draw timing stats on the image.

  Args:
      img (_type_): Input image to draw on.
      infer_time (_type_): Inference time.
      app_time (_type_): Time of the application, measured from acquiring the image from camera to getting results from model.
  """
  text = f'INFER time: {infer_time:10.6f}s | APP time: {app_time:10.6f}s | APP FPS: {1.0/app_time:6.2f}'
  text_size, baseline = cv2.getTextSize(text=text, fontFace=cv2.FONT_HERSHEY_SIMPLEX, fontScale=0.4, thickness=1)
  offset = text_size[1]+baseline+4
  cv2.rectangle(img=img, pt1=(0,0), pt2=(text_size[0]+20,offset), color=(0,0,0), thickness=-1)
  cv2.putText(img=img, text=text, org=(10,offset-baseline-1), fontFace=cv2.FONT_HERSHEY_SIMPLEX, fontScale=0.4, color=(255,255,255), thickness=1)

# ################################################################################################################################
# Object detection
# ################################################################################################################################

def draw_box_on_image(img: np.ndarray, p1: Tuple, p2: Tuple, color: Tuple, text: str=None, is_normalized=True) -> None:
  """Draw single bounding-box on the image

  Args:
      img (np.ndarray): Input image to draw on.
      p1 (Tuple): Uppermost left-most vertex of the B-Box.
      p2 (Tuple): Lowermost right-most vertex of the B-Box.
      color (Tuple): Color of the edges, in RGB format.
      text (str, optional): Text to display above the B-Box. Defaults to None.
      is_normalized (bool, optional): Use this flag in case the `p1` and `p2` points are not normalized to [0,1] range. Defaults to True.
  """
  im_height, im_width, _ = img.shape
  if is_normalized:
    p1 = (int(max(p1[0]*im_width, 0)), int(max(p1[1]*im_height, 0)))
    p2 = (int(min(p2[0]*im_width, im_width)), int(min(p2[1]*im_height, im_height)))
  else:
    p1 = (int(max(p1[0], 0)), int(max(p1[1], 0)))
    p2 = (int(min(p2[0], im_width)), int(min(p2[1], im_height)))
  cv2.rectangle(img=img, pt1=p1, pt2=p2, color=color, thickness=2)
  if text is not None:
    text_size, baseline = cv2.getTextSize(text=text, fontFace=cv2.FONT_HERSHEY_SIMPLEX, fontScale=0.4, thickness=1)
    if p1[1]-(text_size[1]+baseline+2) < 0: draw_text_above = -1
    else: draw_text_above = 1
    ptx1 = (p1[0], p1[1]-draw_text_above*(text_size[1]+baseline+2))
    ptx2 = (p1[0]+(text_size[0]+2), p1[1])
    porg = (p1[0]+1, p1[1]-baseline-1)
    cv2.rectangle(img=img, pt1=ptx1, pt2=ptx2, color=color, thickness=-2)
    box_lumin = (0.2126*color[0] + 0.7152*color[1] + 0.0722*color[2])
    text_color = (0,0,0) if box_lumin > 127 else (255,255,255)
    cv2.putText(img=img, text=text, org=porg, fontFace=cv2.FONT_HERSHEY_SIMPLEX, fontScale=0.4, color=text_color, thickness=1)

def draw_boxes(img: np.ndarray, boxes: np.ndarray, class_idxs: np.ndarray, scores: np.ndarray, labels: list, colors: list, max_boxes: int=1, min_score: int=0.0) -> None:
  """Draw bounding-boxes on the image

  Args:
      img (np.ndarray): Input image to draw on.
      boxes (np.ndarray): List of B-Boxes, specifically, their vertexes.
      class_idxs (np.ndarray): List of class indexes used to iterate over labels.
      scores (np.ndarray): Prediction scores for each class.
      labels (list): Labels for each class.
      colors (list): Colors list used to draw B-Boxes.
      max_boxes (int, optional): Max number of best detections to diaplay. Defaults to 1.
      min_score (int, optional): Min score a detection should have to be displayed. Defaults to 0.0.
  """
  best_detections = np.argsort(scores)[::-1]
  for i in range(min(boxes.shape[0], max_boxes)):
    ni = best_detections[i]
    if scores[ni] >= min_score:
      ymin, xmin, ymax, xmax = tuple(boxes[ni])
      text = f'{labels[int(class_idxs[ni])]}: {100*scores[ni]:0.2f}%'
      color = colors[int(class_idxs[ni]) % len(colors)]
      draw_box_on_image(img=img, p1=(xmin,ymin), p2=(xmax,ymax), color=color, text=text)

# ################################################################################################################################
# Image classification
# ################################################################################################################################

def draw_classification_results(img: np.ndarray, class_idxs: np.ndarray, scores: np.ndarray, labels: list, max_classes: int=1, min_score: float=0.0) -> None:
  """Draw classification results on the image

  Args:
      img (np.ndarray): nput image to draw on.
      class_idxs (np.ndarray): List of class indexes used to iterate over labels.
      scores (np.ndarray): Prediction scores for each class.
      labels (list): Labels for each class.
      max_classes (int, optional): Max number of best detections to diaplay. Defaults to 1.
      min_score (float, optional): Min score a detection should have to be displayed. Defaults to 0.0.
  """
  best_detections = np.argsort(scores)[::-1]
  text2display = []
  for i in range(min(scores.shape[0], max_classes)):
    ni = best_detections[i]
    if scores[ni] >= min_score:
      text = f'{labels[int(class_idxs[ni])]}: {100*scores[ni]:0.2f}%'
      text2display.append(text)
  text_lines = len(text2display)
  if text_lines > 0:
    im_height = img.shape[0]
    for i in range(0,text_lines):
      textSize, baseline = cv2.getTextSize(text=text2display[i], fontFace=cv2.FONT_HERSHEY_SIMPLEX, fontScale=0.4, thickness=1)
      offsetY = textSize[1]+baseline+2
      ptx1 = (0, im_height-offsetY*(i+2))
      ptx2 = (textSize[0]+4, im_height-offsetY*(i+1))
      cv2.rectangle(img=img, pt1=ptx1, pt2=ptx2, color=(0,0,0), thickness=-2)
      text = text2display[i]
      porg = (2, im_height-offsetY*(i+1)-baseline-1)
      cv2.putText(img=img, text=text, org=porg, fontFace=cv2.FONT_HERSHEY_SIMPLEX, fontScale=0.4, color=(255,255,255), thickness=1)

# ################################################################################################################################
# Model execution
# ################################################################################################################################

def run_program(exec_net: Any, c_func: Callable, camera_idx: int=0, max_disp: int=1, min_score: float=0.0) -> None:
  """Application main function, takes image from camera, perform inference on provided model and displays results

  Args:
      exec_net (Any): ANN model that performs classification.
      c_func (Callable): Function handle to one that prepares input image, run the model and measure inferenc time.
        `c_func` has to have following interface:
        `c_func(exec_net: Any, img: np.ndarray, max_classes: int, min_score: float) -> float`
        
        where:
        - `exec_net` is ANN model that performs classification;
        - `img` is an input image;
        - `max_classes` is max number of best detections to diaplay. Defaults to 1;
        - `min_score` is min score a detection should have to be displayed. Defaults to 0.0;
        - Function output is inference time in seconds as type `float`.
      
      camera_idx (int, optional): Camera index the application will conect to. Defaults to 0.
      max_disp (int, optional): Max number of best detections to diaplay. Defaults to 1.
      min_score (float, optional): Min score a detection should have to be displayed. Defaults to 0.0.
  """
  cam = cv2.VideoCapture(camera_idx)
  infer_time, elapsed = (0, 0)
  counter, infer_mean, app_mean, fps_mean = (0, 0, 0, 0)
  try:
    while True:
      t0 = time.time()
      ret, frame = cam.read()
      if ret:
        infer_time = c_func(exec_net, frame, max_disp, min_score)
        elapsed = time.time() - t0
        draw_stats(img=frame, infer_time=infer_time, app_time=elapsed)
        counter += 1
        if counter <= 1:
          infer_mean = infer_time
          app_mean   = elapsed
          fps_mean   = 1/elapsed
        else:
          infer_mean = (infer_mean+infer_time/counter) * counter/(counter+1)
          app_mean   = (app_mean+elapsed/counter) * counter/(counter+1)
          fps_mean   = (fps_mean+1/elapsed/counter) * counter/(counter+1)
        cv2.imshow('press \'q\' to exit program', frame)
        if cv2.waitKey(1) == ord('q'):
          break
      else:
        print('[ ERROR ] Could not retrieve image from camera - quiting program')
        break
  finally:
    cv2.destroyAllWindows()
    cam.release()
    print(f'[ INFO ] Mean stats:\nINFER time: {infer_mean:10.6f}s | APP time: {app_mean:10.6f}s | APP FPS: {fps_mean:6.2f}')

if __name__ == '__main__':
  pass