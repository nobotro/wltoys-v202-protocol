import machine
import socket
import utime
import network
import rc
import time

from machine import UART

steer_key=None
gear=1

do_steer=False
#steer_directio ,[0-127] steer right, [128-255] steer left
#speed_direction ,[0-127] going forward, [128-255] going backward
steering_step=30

steering_trim=124

always_exec_keys=['Key.up','Key.down','Key.left','Key.right']

state=[0,0] #[speed_value,steer_value,steer trim]
def get_next_steer():
  global steer_key,do_steer,state,gear,steering_step
  
  if state[1]<128:
    state[1]=state[1]+steering_step if state[1]+steering_step<128 else 127
  else:
    state[1]=state[1]+steering_step if state[1]+steering_step<256 else 255
 

def driver():

  global steer_key,do_steer,state,gear,steering_trim,always_exec_keys

  machine.freq(160000000)
  rc.setup()
  rc.bind()
 
  
  
  uart = UART(2, 115200)  
  uart.init(115200, bits=8, parity=None, stop=1,timeout=0)
  prev_data=None
  while True:

    #do v202 stuffs in loop
    rc.process(state[1],state[0],steering_trim)
    #if steer key pressed ,steering step by step
    if do_steer:
      get_next_steer()

    data=None
      
    data=uart.readline()
    if data:
      data=data.decode()[:-1]
    else:continue
    if data not in always_exec_keys:
      
      if data==prev_data:
         continue
      else:
        prev_data=data
 
    
        
    
    key = data
    if key==steer_key:
      continue
    elif key == "'w'":
        state[0]=30*gear

    elif key == "'s'":
        state[0]=128+(30*gear)

    elif key == "'d'":
         steer_key='d' 
         state[1]=0
         do_steer=True
          
    elif key == "'a'":
         steer_key='a'
         state[1]=128
         do_steer=True

    elif key == "!'w'":
      state[0]=0
        
    elif key == "!'s'":
      state[0]=0
         
    elif key == "!'d'":
      if steer_key=='d':
        do_steer=False
        state[1]=0

    elif key == "!'a'":
      if steer_key=='a':
        do_steer=False
        state[1]=0
        
    elif key == "Key.up":

        if gear<4:
          gear+=1

          #to change gear in runtime
          if state[0]!=0:
            state[0]+=30
        

    elif key == "Key.down":
        if gear>1:
          gear-=1

          #to change gear in runtime
          if state[0]!=0:
            state[0]-=30
    elif key == "Key.left":

        if steering_trim>=10:

         steering_trim-=2
         

    elif key == "Key.right":

        if steering_trim<=245:
         steering_trim+=2
         

        














