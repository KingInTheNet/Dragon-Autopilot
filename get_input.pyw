from PIL import Image
import pytesseract
from os import listdir
from os.path import isfile, join
import cv2
file_list=[]
file_list_tiff =[]
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
    for file in file_list_tiff:
        print("Processing "+str(file))
        print(pytesseract.image_to_string(Image.open(file)))
