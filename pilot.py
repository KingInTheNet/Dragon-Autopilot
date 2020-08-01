import time
from grabber import screendGrab,grab_section
from movement import *
import win32gui,win32con
from get_input import list_img,process,convert_tiff_all,list_png
from util import *

def main():
    # input_array={"pos":pos_coordinates,"roll":roll_coordinates,"pitch":pitch_coordinates,"yaw":yaw_coordinates,"range:"range_coordinates,"rate":rate_coordinates}
    while(True):
        im = screendGrab()
        for index in input_array:
            grab_section(im,input_array[index],index)
    # hwndMain = win32gui.FindWindow(None, "SpaceX - ISS Docking Simulator - Cốc Cốc")
    # print(hwndMain)
    # hwndChild = win32gui.GetWindow(hwndMain, win32con.GW_CHILD)
    # print(hwndChild)
    # win32gui.SetForegroundWindow(hwndChild)
    # i=10
    # while(i>0):
    #     translate_left()
    #     translate_right()
    #     translate_up()
    #     translate_up()
    #     translate_up()
    #     translate_right()
    #     translate_forward()
    #     translate_forward()
    #     translate_forward()
    #     time.sleep(.100)
    #     i-=1
    # convert_tiff_all()
    # list_img()
<<<<<<< HEAD
    list_png()
    process()
=======
    # process()
>>>>>>> 89d9df7... input stream formation

if __name__=='__main__':
    main()