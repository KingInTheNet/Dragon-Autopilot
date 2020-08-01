from PIL import ImageGrab
import os
import time

def screendGrab():
    box =()
    im = ImageGrab.grab()
    # im.save(os.getcwd()+'\\pilot__.png','PNG')
    return im
def grab_section(im, coordinate_set,dir):
    print(coordinate_set)
    section = im.crop(coordinate_set)
    print(os.getcwd()+'\\'+ str(dir)+'\\pilot__'+str(dir)+' '+ str(int(time.time()))+'.png')
    section.save(os.getcwd()+'\\'+ str(dir)+'\\pilot__'+str(dir)+' '+ str(int(time.time()))+'.png','PNG')


def main():
    screendGrab()

if __name__=='__main__':
    main()