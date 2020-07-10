#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <stdio.h>
#include <time.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "esp32/rom/ets_sys.h"
#include "rc.h"
#include "esp_log.h"
#include "mirf.h"
#define CSN 26
#define CE 25


#define _BV(x) (1<<(x))

NRF24_t dev;
uint8_t buf;
uint8_t txid[]={177,9,156};
uint8_t freq_hopping[4][16]={
 {0x27, 0x1B, 0x39, 0x28, 0x24, 0x22, 0x2E, 0x36, 0x19, 0x21, 0x29, 0x14, 0x1E, 0x12, 0x2D, 0x18 }, 
 { 0x2E, 0x33, 0x25, 0x38, 0x19, 0x12, 0x18, 0x16, 0x2A, 0x1C, 0x1F, 0x37, 0x2F, 0x23, 0x34, 0x10 },
 { 0x11, 0x1A, 0x35, 0x24, 0x28, 0x18, 0x25, 0x2A, 0x32, 0x2C, 0x14, 0x27, 0x36, 0x34, 0x1C, 0x17 }, 
 { 0x22, 0x27, 0x17, 0x39, 0x34, 0x28, 0x2B, 0x1D, 0x18, 0x2A, 0x21, 0x38, 0x10, 0x26, 0x20, 0x1F }
};

uint8_t rf_channels[16];
uint8_t rf_ch_num=0;
uint8_t flags=0xc0;
bool packet_sent=false;

 
uint8_t* rx_tx_addr=(uint8_t *)("\x66\x88\x68\x68\x68");
uint8_t* rx_p1_addr=(uint8_t *)("\x88\x66\x86\x86\x86");




void setTXId(){
	uint16_t summ = txid[0] + txid[1] + txid[2];
	uint8_t* sel_list=freq_hopping[summ & 0x03];
	uint16_t increment = (summ & 0x1e) >> 2;
	for(int i=0;i<16;i++){
		uint16_t val = sel_list[i] + increment;
		rf_channels[i] =  (val & 0x0f) ? val: val - 3;
	}
}

uint16_t sum(uint8_t* arr)
{

	uint16_t sum=0;
	for(int i=0;i<16;i++)
		sum+=*arr++;

	return sum;
}

void command(uint8_t throttle, uint8_t yaw, uint8_t pitch, uint8_t roll, uint8_t flags){
  
  uint8_t com[16];
  if (flags == 0xc0){
   
	    com[0] = 0x00;
	    com[1] = 0x00;
	    com[2] = 0x00;
	    com[3] = 0x00;
	    com[4] = 0x00;
	    com[5] = 0x00;
	    com[6] = 0x00;
	}
  else{
   
	    com[0] = throttle;
	    com[1] = yaw;
	    com[2] = pitch;
	    com[3] = 0;
	   
	    com[4] = roll; 
	    com[5] = 0x40; 
	    com[6] = 0x40; 
	}
  
   
  com[7] = txid[0];
  com[8] = txid[1];
  com[9] = txid[2];
   
  com[10] = 0x00;
  com[11] = 0x00;
  com[12] = 0x00;
  com[13] = 0x00;
  
  com[14] = flags;
  uint16_t summ= sum(com);

  com[15] = summ;
  
  
  uint8_t rf_ch = rf_channels[rf_ch_num >> 1];
  rf_ch_num = (rf_ch_num + 1) & 0x1F;
  
  Nrf24_configRegister(&dev,RF_CH,  rf_ch);
   
  Nrf24_flushTx(&dev);
 
  Nrf24_send(&dev,com);
  
   ets_delay_us(15);

}

void bindrc(){
  for(int i=0;i<50;i++){
    command(0,0,0,0,0xc0);
     ets_delay_us(4000);
   
}

}

void process(uint8_t steer,uint8_t going,uint8_t trim){
    
      
      command(0,steer,going,trim,0);
       ets_delay_us(4000);
}
  
void begin()
{


   
   spi_master_init(&dev,  CE,CSN);
   Nrf24_configRegister(&dev,CONFIG, _BV(EN_CRC) | _BV(CRCO));
   Nrf24_configRegister(&dev,EN_AA, 0x00);
   Nrf24_configRegister(&dev,EN_RXADDR, 0x3F);
   Nrf24_configRegister(&dev,SETUP_AW, 0x03);
   Nrf24_configRegister(&dev,SETUP_RETR, 0xFF);
   Nrf24_configRegister(&dev,RF_CH, 0x08);
   Nrf24_configRegister(&dev,DYNPD, 0);
   Nrf24_configRegister(&dev,RF_SETUP, 0x7);
   Nrf24_configRegister(&dev,STATUS, 0x70);
   Nrf24_configRegister(&dev,RX_ADDR_P2, 0xC3);
   Nrf24_configRegister(&dev,RX_ADDR_P3, 0xC4);
   Nrf24_configRegister(&dev,RX_ADDR_P4, 0xC5);
   Nrf24_configRegister(&dev,RX_ADDR_P5, 0xC6);
   Nrf24_configRegister(&dev,RX_PW_P0, 0x10);
   Nrf24_configRegister(&dev,RX_PW_P1, 0x10);
   Nrf24_configRegister(&dev,RX_PW_P2, 0x10);
   Nrf24_configRegister(&dev,RX_PW_P3, 0x10);
   Nrf24_configRegister(&dev,RX_PW_P4, 0x10);
   Nrf24_configRegister(&dev,RX_PW_P5, 0x10);
   Nrf24_configRegister(&dev,FIFO_STATUS, 0x00);
   Nrf24_writeRegister(&dev,RX_ADDR_P0, rx_tx_addr,5);
   Nrf24_writeRegister(&dev,RX_ADDR_P1, rx_p1_addr,5);
   Nrf24_writeRegister(&dev,TX_ADDR, rx_tx_addr,5);
   Nrf24_flushTx(&dev);
   Nrf24_configRegister(&dev,CONFIG, _BV(EN_CRC) | _BV(CRCO) | _BV(PWR_UP));
   Nrf24_configRegister(&dev,STATUS, RX_DR | TX_DS | MAX_RT);
   Nrf24_ceHi(&dev);
   // int val;
   // Nrf24_readRegister(&dev,EN_RXADDR, &val,1);
   // printf("%d\n",val );

    
 


}



void setup(){

 setTXId();
 begin();
 
}
