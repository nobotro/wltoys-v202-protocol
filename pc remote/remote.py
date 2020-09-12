from pynput.keyboard import Key, Listener
import socket
import time

import serial



prev_steering_key=None




PREV_KEY=None

ser = serial.Serial('/dev/ttyUSB0',115200, bytesize=8, parity=serial.PARITY_NONE, stopbits=1,timeout=1)
def on_press(key):
     
    global PREV_KEY,prev_steering_key
    # PREV_KEY='sgsg'
    key = str(key)
    if key == "'w'":
        if key == PREV_KEY:
            return
        else:
            PREV_KEY = key
        ser.write((str(key)+'\n').encode())
        print(key)
        return
    if key == "'s'":
        if key == PREV_KEY:
            return
        else:
            PREV_KEY = key
        
        ser.write((str(key)+'\n').encode())
        print(key)
        return
    if key == "'d'":
        if key == PREV_KEY:
            return
        else:
            PREV_KEY = key
        prev_steering_key="'d'"
        ser.write((str(key)+'\n').encode())
        print(key)
        return
    if key == "'a'":
        if key == PREV_KEY:
            return
        else:
            PREV_KEY = key
        prev_steering_key="'a'"
        ser.write((str(key)+'\n').encode())
        print(key)
        return
    if key == "'t'":
        if key == PREV_KEY:
            return
        else:
            PREV_KEY = key
        ser.write((str(key)+'\n').encode())
        print(key)
        return
    if key == "'p'":
        if key == PREV_KEY:
            return
        else:
            PREV_KEY = key
        ser.write((str(key)+'\n').encode())
        print(key)
        return
    if key == "Key.up":
        ser.write((str(key)+'\n').encode())
        print(key)
        return
    if key == "Key.down":
        ser.write((str(key)+'\n').encode())
        print(key)
        return
    if key == "Key.left":
        ser.write((str(key)+'\n').encode())
        print(key)
        return
    if key == "Key.right":
        ser.write((str(key)+'\n').encode())
        print(key)
        return

def on_release(key):
    global PREV_KEY,prev_steering_key
    #PREV_KEY='sgsg'

    key = '!' + str(key)
    
    if key == "!'w'":
        if key == PREV_KEY:
            return
        else:
            PREV_KEY = key
        ser.write((str(key)+'\n').encode())
        print(key)
        return
    if key == "!'s'":
        if key == PREV_KEY:
            return
        else:
            PREV_KEY = key
        ser.write((str(key)+'\n').encode())
        print(key)
        return
    if key == "!'d'":
        if key == PREV_KEY:
            return
        else:
            PREV_KEY = key
        if prev_steering_key=="'d'":
            ser.write((str(key)+'\n').encode())
            print(key)
        return
    if key == "!'a'":
        if key == PREV_KEY:
            return
        else:
            PREV_KEY = key
        if prev_steering_key=="'a'":
            ser.write((str(key)+'\n').encode())
            print(key)
        return
    if key == "!'t'":
        if key == PREV_KEY:
            return
        else:
            PREV_KEY = key
        ser.write((str(key)+'\n').encode())
        print(key)
        return
    if key == "!Key.esc":
        print('Exiting...')
        return

with Listener(on_release=on_release,
        on_press=on_press
        ) as listener:
    listener.join()
