from PIL import Image
import pytesseract
from os import listdir
from os.path import isfile, join
import cv2 as cv
import matplotlib.pyplot as plt 
from skimage.filters import *
from skimage import data,img_as_ubyte
import numpy as np
file_list=[]
file_list_tiff =[]
data_list =[]
pytesseract.pytesseract.tesseract_cmd = r'C:\Program Files\Tesseract-OCR\tesseract.exe'

def convert_tiff_all():
    mypath = "img/"
    # file_list = [f for f in listdir(mypath) if isfile(join(mypath, f))]
    for f in listdir(mypath):
        if isfile(join(mypath, f)):
            file_list.append(f)
    tobe_removed =""
    for file in file_list:
        if "pilot" in file:
            tobe_removed=file
        print(file)
    file_list.remove(tobe_removed)
    print("\nAfter effect:")
    for file in file_list:
        print(file)
        im = Image.open(mypath+file)
        im.save("tiff/"+file.split('.')[0]+".tif")
        file_list_tiff.append("tiff/"+file.split('.')[0]+".tif")
    
def list_png():
    mypath = "img/"
    # file_list = [f for f in listdir(mypath) if isfile(join(mypath, f))]
    for f in listdir(mypath):
        if isfile(join(mypath, f)):
            file_list.append(mypath+f)
    tobe_removed =""
    for file in file_list:
        if "pilot" in file:
            tobe_removed=file
        print(file)
    file_list.remove(tobe_removed)


def list_img():
    mypath = "tiff/"
    # file_list_tiff = [f for f in listdir(mypath) if isfile(join(mypath, f))]
    for f in listdir(mypath):
        if isfile(join(mypath, f)):
            file_list_tiff.append(mypath+f)
    print("\nAfter effect:")
    for file in file_list_tiff:
        print(file)

def process():
    fig,axes = plt.subplots(ncols=3, figsize=(8, 2.5))
    ax = axes.ravel()  
    for file in file_list:
        data = cv.imread(file)
        # data= np.asarray(data,dtype="int32")
        # thresh = threshold_yen(data)
        # binary = data>thresh
        gray = cv.cvtColor(data, cv.COLOR_RGB2GRAY)
        gray,img_bin = cv.threshold(gray,128,255,cv.THRESH_BINARY | cv.THRESH_OTSU)
        gray = cv.bitwise_not(gray)
        kernel = np.ones((2, 1), np.uint8)
        img = cv.erode(img_bin, kernel, iterations=1)
        img = cv.dilate(img, kernel, iterations=1)
        # level2= cv.cvtColor(img_bin, cv.COLOR_RGB2GRAY)
        # level2_bin = cv.threshold(level2,128,255,cv.THRESH_BINARY | cv.THRESH_OTSU)
        # print(type(data))
        # binary = img_as_ubyte(binary)
        print("Processing "+str(file))
        result = pytesseract.image_to_string(img)
        print(result)
        cv.imshow(result,img)
        cv.waitKey(0)
        # ax[0] = plt.subplot(1, 3, 1)
        # ax[1] = plt.subplot(1, 3, 2)
        # ax[2] = plt.subplot(1, 3, 3, sharex=ax[0], sharey=ax[0])

        # ax[0].imshow(data, cmap=plt.cm.gray)
        # ax[0].set_title('Original')
        # ax[0].axis('off')

        # ax[1].hist(data.ravel(), bins=256)
        # ax[1].set_title('Histogram')
        # ax[1].axvline(thresh, color='r')

        # ax[2].imshow(binary, cmap=plt.cm.gray)
        # ax[2].set_title('Thresholded')
        # ax[2].axis('off')
        # plt.show()

        
