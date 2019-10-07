
import sys
import ustruct as struct
import utime
from machine import Pin, SPI


from regs import *


_RX_POLL_DELAY = const(15)
buf = bytearray(1)

txid = [ 177,9, 156];

freq_hopping = [

 [ 0x27, 0x1B, 0x39, 0x28, 0x24, 0x22, 0x2E, 0x36, 0x19, 0x21, 0x29, 0x14, 0x1E, 0x12, 0x2D, 0x18 ], 

 [ 0x2E, 0x33, 0x25, 0x38, 0x19, 0x12, 0x18, 0x16, 0x2A, 0x1C, 0x1F, 0x37, 0x2F, 0x23, 0x34, 0x10 ],

 [ 0x11, 0x1A, 0x35, 0x24, 0x28, 0x18, 0x25, 0x2A, 0x32, 0x2C, 0x14, 0x27, 0x36, 0x34, 0x1C, 0x17 ], 

 [ 0x22, 0x27, 0x17, 0x39, 0x34, 0x28, 0x2B, 0x1D, 0x18, 0x2A, 0x21, 0x38, 0x10, 0x26, 0x20, 0x1F ] 


];

rf_channels=[0]*16#16 size

rf_ch_num=0


flags=0xc0

packet_sent=False
cfg = {'spi': -1, 'miso': 32, 'mosi': 33, 'sck': 25, 'csn': 26, 'ce': 27}

rx_tx_addr =bytearray(b'\x66\x88\x68\x68\x68')
rx_p1_addr =bytearray(b'\x88\x66\x86\x86\x86')


csn = Pin(cfg['csn'], mode=Pin.OUT, value=1)
ce = Pin(cfg['ce'], mode=Pin.OUT, value=0)
test=1
spi = SPI(-1, sck=Pin(cfg['sck']), mosi=Pin(cfg['mosi']), miso=Pin(cfg['miso']))
def _BV(bit):
  return 1<<bit
#+
def begin():

  spi.init( baudrate=4000000, polarity=0, phase=0)
  write_register(CONFIG, _BV(EN_CRC) | _BV(CRCO));
  write_register(EN_AA, 0x00);
  write_register(EN_RXADDR, 0x3F);
  write_register(SETUP_AW, 0x03);
  write_register(SETUP_RETR, 0xFF);
  write_register(RF_CH, 0x08);
  write_register(DYNPD, 0)
  write_register(RF_SETUP, 0x7);
  write_register(STATUS, 0x70);
  write_register(RX_ADDR_P2, 0xC3);
  write_register(RX_ADDR_P3, 0xC4);
  write_register(RX_ADDR_P4, 0xC5);
  write_register(RX_ADDR_P5, 0xC6);
  write_register(RX_PW_P0, 0x10);
  write_register(RX_PW_P1, 0x10);
  write_register(RX_PW_P2, 0x10);
  write_register(RX_PW_P3, 0x10);
  write_register(RX_PW_P4, 0x10);
  write_register(RX_PW_P5, 0x10);
  write_register(FIFO_STATUS, 0x00);
  write_register_bytes(RX_ADDR_P0, rx_tx_addr);
  write_register_bytes(RX_ADDR_P1, rx_p1_addr);
  write_register_bytes(TX_ADDR, rx_tx_addr);
  flush_tx()
  write_register(CONFIG, _BV(EN_CRC) | _BV(CRCO) | _BV(PWR_UP));
  write_register(STATUS, RX_DR | TX_DS | MAX_RT)
  ce(1)
   
 
 
#+
def setup():

 setTXId()
 begin()
 



#+
def setTXId():
	summ = txid[0] + txid[1] + txid[2]
	sel_list=freq_hopping[summ & 0x03]
	increment = (summ & 0x1e) >> 2
	for i in range(16):
		val = sel_list[i] + increment
		rf_channels[i] =  val if (val & 0x0f) else val - 3


def read_register(reg):
        csn(0)
        spi.readinto(buf, reg)
        spi.readinto(buf)
        csn(1)
        return buf[0]

#+
def write_register_bytes( reg, data):
        csn(0)
        spi.readinto(buf,W_REGISTER | ( REGISTER_MASK & reg ))
        spi.write(data)
        csn(1)

        return buf[0]

#+
def write_register( reg, value):
        csn(0)
        spi.readinto(buf,W_REGISTER | ( REGISTER_MASK & reg ))
        ret = buf[0]
        spi.readinto(buf, value)
        csn(1)
        return ret

#+
def flush_tx():
        csn(0)
        spi.readinto(buf, FLUSH_TX)
        csn(1)

def write_payload(comm):
    csn(0)
    spi.readinto(buf, W_TX_PAYLOAD)
    spi.write(comm)
    csn(1)
    
def bind():
  for i in range(50):
    command(0,0,0,0,0xc0)
    utime.sleep_ms(4)


def process(steer,going,trim):
    
      
      command(0,steer,going,trim,0);
      utime.sleep_ms(4)
      
       
  

  

def command(throttle, yaw, pitch, roll, flags):
  global rf_ch_num,packet_sent
  com=bytearray(16)
  if (flags == 0xc0):
   
	    com[0] = 0x00
	    com[1] = 0x00
	    com[2] = 0x00
	    com[3] = 0x00
	    com[4] = 0x00
	    com[5] = 0x00
	    com[6] = 0x00
  else:
   
	    com[0] = throttle
	    com[1] = yaw
	    com[2] = pitch
	    com[3] = 0
	   
	    com[4] = roll; 
	    com[5] = 0x40; 
	    com[6] = 0x40; 
  
   
  com[7] = txid[0];
  com[8] = txid[1];
  com[9] = txid[2];
   
  com[10] = 0x00;
  com[11] = 0x00;
  com[12] = 0x00;
  com[13] = 0x00;
  
  com[14] = flags;
  summ= sum(com);

  com[15] = summ;
  
  
  rf_ch = rf_channels[rf_ch_num >> 1];
  rf_ch_num = (rf_ch_num + 1) & 0x1F;
  
  write_register(RF_CH,  rf_ch);
   
  flush_tx();
 
  write_payload(com);
  
  utime.sleep_us(15)
  

    
    
