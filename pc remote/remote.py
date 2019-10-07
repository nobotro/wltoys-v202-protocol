from pynput.keyboard import Key, Listener
import socket
import time





prev_steering_key=None
#UDP_IP = "127.0.0.1"
UDP_IP = "192.168.4.1"
UDP_PORT = 1327
PREV_KEY=None

def on_press(key):
     
    global PREV_KEY,prev_steering_key
    # PREV_KEY='sgsg'
    key = str(key)
    if key == "'w'":
        if key == PREV_KEY:
            return
        else:
            PREV_KEY = key
        sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        sock.sendto(str(key).encode(), (UDP_IP, UDP_PORT))
        print(key)
        return
    if key == "'s'":
        if key == PREV_KEY:
            return
        else:
            PREV_KEY = key
        
        sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        sock.sendto(key.encode(), (UDP_IP, UDP_PORT))
        print(key)
        return
    if key == "'d'":
        if key == PREV_KEY:
            return
        else:
            PREV_KEY = key
        prev_steering_key="'d'"
        sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        sock.sendto(key.encode(), (UDP_IP, UDP_PORT))
        print(key)
        return
    if key == "'a'":
        if key == PREV_KEY:
            return
        else:
            PREV_KEY = key
        prev_steering_key="'a'"
        sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        sock.sendto(key.encode(), (UDP_IP, UDP_PORT))
        print(key)
        return
    if key == "'t'":
        if key == PREV_KEY:
            return
        else:
            PREV_KEY = key
        sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        sock.sendto(key.encode(), (UDP_IP, UDP_PORT))
        print(key)
        return
    if key == "'p'":
        if key == PREV_KEY:
            return
        else:
            PREV_KEY = key
        sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        sock.sendto(key.encode(), (UDP_IP, UDP_PORT))
        print(key)
        return
    if key == "Key.up":
        sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        sock.sendto(key.encode(), (UDP_IP, UDP_PORT))
        print(key)
        return
    if key == "Key.down":
        sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        sock.sendto(key.encode(), (UDP_IP, UDP_PORT))
        print(key)
        return
    if key == "Key.left":
        sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        sock.sendto(key.encode(), (UDP_IP, UDP_PORT))
        print(key)
        return
    if key == "Key.right":
        sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        sock.sendto(key.encode(), (UDP_IP, UDP_PORT))
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
        sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        sock.sendto(key.encode(), (UDP_IP, UDP_PORT))
        print(key)
        return
    if key == "!'s'":
        if key == PREV_KEY:
            return
        else:
            PREV_KEY = key
        sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        sock.sendto(key.encode(), (UDP_IP, UDP_PORT))
        print(key)
        return
    if key == "!'d'":
        if key == PREV_KEY:
            return
        else:
            PREV_KEY = key
        if prev_steering_key=="'d'":
            sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
            sock.sendto(key.encode(), (UDP_IP, UDP_PORT))
            print(key)
        return
    if key == "!'a'":
        if key == PREV_KEY:
            return
        else:
            PREV_KEY = key
        if prev_steering_key=="'a'":
            sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
            sock.sendto(key.encode(), (UDP_IP, UDP_PORT))
            print(key)
        return
    if key == "!'t'":
        if key == PREV_KEY:
            return
        else:
            PREV_KEY = key
        sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        sock.sendto(key.encode(), (UDP_IP, UDP_PORT))
        print(key)
        return
    if key == "!Key.esc":
        print('Exiting...')
        return

with Listener(on_release=on_release,
        on_press=on_press
        ) as listener:
    listener.join()
