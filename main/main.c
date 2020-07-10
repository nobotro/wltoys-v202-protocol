/* Hello World Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include <stdbool.h>
#include "lwip/err.h"
#include "lwip/sys.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include "rc.h"
#include "lwip/sockets.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"

static char *TAG = "wifi softAP";
char steer_key;
uint8_t gear=1;
bool do_steer=false;
uint8_t steering_step=30;
uint8_t steering_trim=124;
char always_exec_keys[4][20] ={"Key.up","Key.down","Key.left","Key.right"};
uint8_t state[2]={0,0};

void get_next_steer(){

  if (state[1]<128)
    state[1]= state[1]+steering_step<128 ? state[1]+steering_step : 127;
  else
    state[1]= state[1]+steering_step<256 ? state[1]+steering_step : 255;
 
}


void wifi_init_softap(void)
{
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_ap();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    

    wifi_config_t wifi_config = {
        .ap = {
            .ssid = "esp_car",
            .ssid_len = strlen("esp_car"),
            .channel = 0,
            .password = "",
            .max_connection = 1,
            .authmode = WIFI_AUTH_WPA_WPA2_PSK
        },
    };
    
        wifi_config.ap.authmode = WIFI_AUTH_OPEN;
    

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

  }


void v202_job(){
  setup();
  bindrc();
  while(1){
  process(state[1],state[0],steering_trim);
  if(do_steer)get_next_steer();
 
}
}


void driver(){

  
  int socket_fd;
  struct sockaddr_in ra;
  int recv_data;
  char data_buffer[20];
  socket_fd = socket(PF_INET, SOCK_DGRAM, 0);
struct timeval tv;
tv.tv_sec = 0;
tv.tv_usec = 1;
if (setsockopt(socket_fd, SOL_SOCKET, SO_RCVTIMEO,&tv,sizeof(tv)) < 0) {
    perror("Error");
}
    if (socket_fd < 0) {

        printf("socket call failed");
        exit(0);

    }
    ra.sin_family = AF_INET;

    ra.sin_addr.s_addr = htonl(INADDR_ANY);

    ra.sin_port = htons(1327);

    
    if (bind(socket_fd, (struct sockaddr *) &ra, sizeof(struct sockaddr_in))
            == -1) {

        close(socket_fd);
        exit(1);
    }
 
 

  char prev_data[20];
  while (1){

   
    recv_data = recvfrom(socket_fd, data_buffer, sizeof(data_buffer), 0,NULL, NULL);
    if (recv_data > 0) {

        data_buffer[recv_data] = '\0';
         bool is_always_executed=false;
        for(uint8_t i=0;i<4;i++)
        {

            if(!strcmp(data_buffer,always_exec_keys[i]))
                is_always_executed=true;

        }
        if(!is_always_executed && strcmp(data_buffer,prev_data)==0)
        {
            continue;

        }
        else 
            strncpy(prev_data, data_buffer,strlen(data_buffer));

    if (data_buffer[0]==steer_key) continue;
    else if(!strcmp(data_buffer,"'w'")){ 
        state[0]=30*gear;
     }

    else if(!strcmp(data_buffer,"'s'")){
        state[0]=128+(30*gear);
     }

    else if(!strcmp(data_buffer,"'d'")){
         steer_key='d';
         state[1]=0;
         do_steer=true;
     }
          
    else if(!strcmp(data_buffer,"'a'")){
         steer_key='a';
         state[1]=128;
         do_steer=true;
     }

    else if(!strcmp(data_buffer,"!'w'"))
      state[0]=0;
        
    else if(!strcmp(data_buffer,"!'s'"))
      state[0]=0;
         
    else if(!strcmp(data_buffer,"!'d'")){
      if (steer_key=='d'){
        do_steer=false;
        state[1]=0;
    }
}
    else if(!strcmp(data_buffer,"!'a'")){
      if (steer_key=='a'){
        do_steer=false;
        state[1]=0;
    }
}
        
    else if(!strcmp(data_buffer,"Key.up")){

        if (gear<4){
          gear+=1;
          if(state[0]!=0)state[0]+=30;
     }
    }
        

    else if(!strcmp(data_buffer,"Key.down")){
        if(gear>1){
          gear-=1;
          if(state[0]!=0)state[0]-=30;
     }
    }
     else if(!strcmp(data_buffer,"Key.left")){
        if(steering_trim>=10)steering_trim-=2;
    }
         

    else if(!strcmp(data_buffer,"Key.right")){
        if (steering_trim<=245)steering_trim+=2;
    }
}

 }
   


}
void app_main(void)
{
    //Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_LOGI(TAG, "ESP_WIFI_MODE_AP");
    wifi_init_softap();
   

   

      xTaskCreate(driver, "driver", 6000, NULL,
      configMAX_PRIORITIES - 1, NULL);
      xTaskCreate(v202_job, "v202", 6000, NULL,
      configMAX_PRIORITIES - 2, NULL);

    
    
}
