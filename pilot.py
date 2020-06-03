import time
from grabber import screendGrab
from movement import *
import win32gui,win32con

def main():
    # screendGrab()
    hwndMain = win32gui.FindWindow(None, "SpaceX - ISS Docking Simulator - Cốc Cốc")
    print(hwndMain)
    hwndChild = win32gui.GetWindow(hwndMain, win32con.GW_CHILD)
    print(hwndChild)
    win32gui.SetForegroundWindow(hwndChild)
    i=10
    while(i>0):
        translate_left()
        translate_right()
        translate_up()
        translate_up()
        translate_up()
        translate_right()
        translate_forward()
        translate_forward()
        translate_forward()
        time.sleep(.100)
        i-=1

if __name__=='__main__':
    main()