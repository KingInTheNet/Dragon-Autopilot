from PIL import ImageGrab
import os
import time

def screendGrab():
    box =()
    im = ImageGrab.grab()
    im.save(os.getcwd()+'\\pilot__'+str(int(time.time()))+'.png','PNG')

def main():
    screendGrab()

if __name__=='__main__':
    main()