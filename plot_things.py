# function that take object or 1D array or 2D array of objects, where object is either an image or a thing to plot
# and also takes two values as dimensions of the figure
# and returns a figure with subplots of the objects
# > the 2D array may be made of list that differ in size
# > object to show is either an image or a list of points, both represented as ndarrays, but while the image is 3D, the list of points is 1D
# > grouping is done by creating python lists
def plot_things(thing, sizeX=8, sizeY=8, hideAxes=True, verbose=False):
    fig = None
    axes = None
    #check if thing is a list or a 2D array
    if isinstance(thing, list):
        #check if thing is a 2D array
        if isinstance(thing[0], list):
            # --- 2D PROCESSING ---
            if verbose:
                print("Autodetecting 2D array")
            #search for the longest list in the 2D array
            dimX = max([len(i) for i in thing])
            dimY = len(thing)
            print("dimX=",dimX,"dimY=",dimY)
            #create a figure with the right dimensions
            fig, axes = plt.subplots(dimY, dimX, figsize=(sizeX, sizeY))
            matrix=thing
            for j,row in enumerate(matrix):
                for i,thingy in enumerate(row):
                    dims=thingy.shape
                    print("j=",j,"i=",i,"dims=",dims)
                    if len(dims)==3: # this is color image
                        axes[j,i].imshow(thingy)
                    elif len(dims)==2: # this is grayscale image
                        axes[j,i].imshow(thingy, cmap='gray')
                    elif len(dims)==1: # this is a list of points
                        axes[j,i].plot(thingy)
                    else:
                        print("Error: the object is not an image or a thing to plot, dims=",dims)
        else:
            # --- 1D PROCESSING ---
            if verbose:
                print("Autodetecting 1D array")
            fig, axes = plt.subplots(1, len(thing), figsize=(sizeX, sizeY))
            for i,thingy in enumerate(thing):
                dims=thingy.shape
                print("i=",i,"dims=",dims)
                if len(dims)==3: # this is color image
                    axes[i].imshow(thingy)
                elif len(dims)==2: # this is grayscale image
                    axes[i].imshow(thingy, cmap='gray')
                elif len(dims)==1: # this is a list of points
                    axes[i].plot(thingy)
                else:
                    print("Error: the object is not an image or a thing to plot, dims=",dims)
    elif isinstance(thing, np.ndarray):
        # --- SIMPLE OBJECT PROCESSING ---
        if verbose:
            print("Autodetecting simple object")
        fig, axes = plt.subplots(1, 1, figsize=(sizeX, sizeY))
        dims=thing.shape
        print("dims=",dims)
        if len(dims)==3: # this is color image
            axes.imshow(thing)
        elif len(dims)==2: # this is grayscale image
            axes.imshow(thing, cmap='gray')
        elif len(dims)==1: # this is a list of points
            axes.plot(thing)
        else:
            print("Error: the object is not an image or a thing to plot, dims=",dims)
    else:
        print("Error: the object is not an image or a thing to plot")
    
    if fig is None or axes is None:
        return

    if hideAxes:
        if not isinstance(axes, np.ndarray):
            axes.axis('off')
        else: 
            for ax in axes.flatten():
                ax.axis('off')

# function that takes tests the function plot_things
def test_plot_things():
    #test image by creating new 2x2 image, with 4 different colors, then upscaling it to 100x100
    testImg = np.zeros((2,2,3), np.uint8)
    testImg[0,0] = [255,0,0]
    testImg[0,1] = [0,255,0]
    testImg[1,0] = [0,0,255]
    testImg[1,1] = [255,255,255]
    #test image
    testImg = cv2.resize(testImg, (128,128))
    #test list
    list1 = np.array([x%7+x%3 for x in range(20)])
    #test 1D array with 2 images and 1 list
    array1d = [testImg, list1, testImg]
    #test 2D, 4x3 array
    array2d = [[testImg, list1, testImg], [list1, testImg], [testImg, list1, testImg, list1]]
    #check if the function works
    plot_things(testImg,2,2)
    plot_things(list1,2,2)
    plot_things(array1d,8,2)
    plot_things(array2d,15,8)

import cv2
import matplotlib.pyplot as plt
import numpy as np
#test_plot_things()