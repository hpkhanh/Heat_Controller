/*-------------------------------------------------------------
  Wifi MQTT - header file
  ESP32 DEVKIT V1
  (c) Can Tho University - 2019 
  version 1.00 - 24/08/2019
---------------------------------------------------------------
 * ESP-IDF version: 3.2
 * Compiler version: 5.2.0
 * Arduino components version: latest
--------------------------------------------------------------*/
#ifndef __ESP32_MQTT_H
#define __ESP32_MQTT_H
#include <WiFi.h> //for esp32
#include "src/Adafruit_MQTT_Mod/Adafruit_MQTT.h"
#include "src/Adafruit_MQTT_Mod/Adafruit_MQTT_Client.h"
#include "ESP32_NVS.h"
#include "ESP32_PID.h"
#include "ESP32_relay.h"
#include "ESP32_PWM_Pump.h"
#include "ESP32_UART.h"
#include "debugConfig.h"
//#include "config.h"

// ------ Public constants ------------------------------------

#define WLAN_SSID     ""
#define WLAN_PASS     ""

#if (COMPANY_NAME == FECTUM)
#define WIFI_PREFIX                           "FEC_"
#elif (COMPANY_NAME == SOLESTA)
#define WIFI_PREFIX                           "SOL_"
#else
#define WIFI_PREFIX                           "POM_"
#endif

#define WIFI_PREFIX_LENGTH                    4

/******** MQTT Client Setup **********/
// #define SERVER        "io.adafruit.com"                   //change it to your server
// #define PORT          1883                                // use 8883 for SSL - change it to your port
// #define USERNAME      "heatcontroller2019"                //change it to your username
// #define PASS          "def9f50546334d019a3c58a4a908d1e7"  //change it to your password 36374516bd4246869ce62deac0924c15

#define SERVER        "mqtt.vlwsystemen.nl"                      //change it to your server
#define PORT          8883                                // use 8883 for SSL - change it to your port
#define USERNAME      "clie"                //change it to your username
#define PASS          "5eecd4c981fc4981a616244561b4a73d"  //change it to your password 36374516bd4246869ce62deac0924c15
#define CLIENT_ID     "HC01"

#define LWT_TOPIC     "heatcontroller2019/feeds/availability" //change it to your last will topic
#define LWT_PAYLOAD   "offline"
#define LWT_RETAIN    1     //The broker will store the last message as the last known good value if this flag is on
#define RETAIN        true  //The broker will store the last message as the last known good value
#define NO_RETAIN     false //The broker will not store the last message as the last known good value
#define PUB_RETRIES   10
#define PUB_WAIT      700
//MQTT_CONN_KEEPALIVE is defined inside the Adafruit_MQTT.h (default 5 minutes) --> need to change to 15 min (900)
//CLEAN_SESSION is defined as TRUE as always clean the session

#define KP_FEED         "heatcontroller2019/feeds/kp"   //command feed, change it to yours
#define KI_FEED         "heatcontroller2019/feeds/ki"   //command feed, change it to yours
#define KD_FEED         "heatcontroller2019/feeds/kd"   //command feed, change it to yours
#define TEMP1_FEED      "heatcontroller2019/feeds/temp01"   //command feed, change it to yours
#define TEMP2_FEED      "heatcontroller2019/feeds/temp02"   //command feed, change it to yours
#define TEMP3_FEED      "heatcontroller2019/feeds/temp03"   //command feed, change it to yours
#define TEMP4_FEED      "heatcontroller2019/feeds/temp04"   //command feed, change it to yours
#define PUMP1PWM_FEED   "heatcontroller2019/feeds/pump01pwm"   //command feed, change it to yours
#define PUMP2PWM_FEED   "heatcontroller2019/feeds/pump02pwm"   //command feed, change it to yours
#define RELAY01_FEED    "heatcontroller2019/feeds/relay01"   //command feed, change it to yours
#define RELAY02_FEED    "heatcontroller2019/feeds/relay02"   //command feed, change it to yours
#define RELAY03_FEED    "heatcontroller2019/feeds/relay03"   //command feed, change it to yours
#define FLOWSENS_01_FEED   "heatcontroller2019/feeds/flowsen-01"

#define PING_WAIT   55000
#define PING_TIMES  10
#define CONFIRM_PRESENT_WAIT            86400000 // one full day

#define CHIP_ID_HEX_STRING_LENGTH       13U
#define WIFI_SSID_STRING_LENGTH         (CHIP_ID_HEX_STRING_LENGTH + WIFI_PREFIX_LENGTH)
#define MQTT_MESSAGE_STRING_MAX_SIZE         255 
#define MQTT_MESSAGE_STRING_MAX_APPEND       250
#define MQTT_MESSAGE_VALUE_MAX_SIZE          12
#define MQTT_CLIENT_ID_STRING_MAX_SIZE       24U

// ------ Public function prototypes --------------------------
/**
Initialize Wifi and MQTT
**/
void MQTT_init();
/**
Keep the MQTT connection even if no data was sent to the broker
**/
void MQTT_maintain();
/**
MQTT subscribe task. wait for signal from the server
**/
void MQTT_subscribe();
/**
Publish the Kp value to the server
**/
void MQTT_Kp_pub(float);
/**
Publish the Ki value to the server
**/
void MQTT_Ki_pub(float);
/**
Publish the Kd value to the server
**/
void MQTT_Kd_pub(float);
/**
Publish the T1 value to the server
**/
void MQTT_T1_pub(int);
/**
Publish the T2 value to the server
**/
void MQTT_T2_pub(int);
/**
Publish the T3 value to the server
**/
void MQTT_T3_pub(int);
/**
Publish the T4 value to the server
**/
void MQTT_T4_pub(int);
/**
Publish the pwm freq of Pump1 to the server
**/
void MQTT_Pump1pwm_pub(float);
/**
Publish the pwm freq of Pump2 to the server
**/
void MQTT_Pump2pwm_pub(float);
/**
Publish the value of flow sensor 01 to the server
**/
void MQTT_FlowSen_01_pub(float flowSenVal);

/**
Handle the looping function for Web Server
**/
void MQTT_loopHandle();

/**
Initialize the subscription to MQTT server
**/
void MQTT_subscribeInit();

/**
publish the wanted value to the broker (self-created QOS1)-- make sure the packet made it to the broker
**/
void publishNow(Adafruit_MQTT_Publish,const char*,bool,const char*,const char*);
void publishStringNow(Adafruit_MQTT_Publish,char*,bool,const char*,const char*);
void publishNow(Adafruit_MQTT_Publish,int, bool,const char*,const char*);
void publishNow(Adafruit_MQTT_Publish,float, bool,const char*,const char*);
// ------ Public variable -------------------------------------
extern SemaphoreHandle_t baton; //declared in core0 cpp

#endif // __ESP32_MQTT_H
