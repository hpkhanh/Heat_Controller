/*------------------------------------------------------------*-
  PWM pump controller - functions file
  ESP32 DEVKIT V1
  (c) Can Tho University 2019
  version 1.00 - 14/08/2019
---------------------------------------------------------------
 * ESP-IDF version: 3.2
 * Compiler version: 5.2.0
 * Arduino components version: latest
--------------------------------------------------------------*/
#ifndef __ESP32_PWM_PUMP_H
#define __ESP32_PWM_PUMP_H
#include <WiFi.h> //for esp32
#include "debugConfig.h"
//#include "config.h"

// #define SERIAL_OUTPUT
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////MODE CONFIGS/////////////////////////////////////////////////////////////////////
//--------------------------------------------------------------
// CHOOSING PUMP
//--------------------------------------------------------------
#define GRUNDFOS_UPM3_PWM //if using this pump, you must choose ONE profile below, slash out the others if you dont want to invite errors.
  //#define PROFILE_A_HEATING
#define PROFILE_C_SOLAR
//#define YONOS_PARA_ST_15/13_PWM2
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// ------ Public constants ------------------------------------
//--------------------------------------------------------------
// PUMP PARAMETERS
//--------------------------------------------------------------
//========================= GRUNDFOS UPM3 PWM PUMP =============
#ifdef GRUNDFOS_UPM3_PWM
  #define PWM_FREQ  1000  //The square-wave PWM signal is designed for a 100 to 4,000 Hz frequency range.
  #ifdef PROFILE_A_HEATING
    #define MAX_SPEED           0.05*PWM_RESOLUTION   // < 10%, so 5% will be enough
    #define MIN_SPEED           0.87*PWM_RESOLUTION   // >84% / <91% --> choose 87%
    #define VAR_MAXSPEED        0.82*PWM_RESOLUTION   // <=84% --> choose 82%
    #define VAR_MINSPEED        0.12*PWM_RESOLUTION   // >10% --> choose 12%
    //hysteresis area lies in between 91% to 95%
    #define OFF_MODE            0.97*PWM_RESOLUTION   // >95% and <100% --> choose 97% (Standby mode: Off)
  #endif
  #ifdef PROFILE_C_SOLAR
    #define MAX_SPEED           0.95*PWM_RESOLUTION   // >90% and <= 100%, so 95% will be enough
    #define MIN_SPEED           0.10*PWM_RESOLUTION   // >8% / <15% --> choose 13%
    #define VAR_MAXSPEED        0.90*PWM_RESOLUTION   // <=90% --> choose 88%
    #define VAR_MINSPEED        0.17*PWM_RESOLUTION   // >15% --> choose 17%
    //hysteresis area lies in between 5% to 8%
    #define OFF_MODE            0.00*PWM_RESOLUTION   // <5% --> choose 3% (Standby mode: Off)
  #endif
  //PWM feedback signal
  #define PUMP_STANDBY          0.95
  #define PUMP_BLOCK_ERROR      0.90
  #define PUMP_ELECTRICAL_ERROR 0.85
  #define PUMP_WARNING          0.75
  #define PUMP_NORMAL           0.10
  //Input cycle
  #define PUMP_CYCLE   13333 //us
  
#endif
//========================= YONOS PARA ST 15/13 PWM2 =============
#ifdef YONOS_PARA_ST_15_13_PWM2
  #define PWM_FREQ  1000  //Signal frequency: 100 Hz-5000 Hz (1000 Hz nominal).
  #define MAX_SPEED           0.97*PWM_RESOLUTION   // >95% --> choose 97%
  #define MIN_SPEED           0.13*PWM_RESOLUTION   // >7% and <15% pump runs at minimum speed (operation) and from 12% to 15% pump runs at minimum speed (start up) --> choose 13%
  #define VAR_MAXSPEED        0.94*PWM_RESOLUTION   // <=95% --> choose 94%
  #define VAR_MINSPEED        0.16*PWM_RESOLUTION   // >15% --> choose 16%
  #define OFF_MODE            0.05*PWM_RESOLUTION   // <7% --> choose 5% (Pump stops (standby))
#endif
//===============================================================
#define NO_PWM_INPUT  9999
//--------------------------------------------------------------
// PUMP PARAMETERS
//--------------------------------------------------------------
#define PUMP1_OUT_PIN 25
#define PUMP1_IN_PIN  26
#define PUMP2_OUT_PIN 27
#define PUMP2_IN_PIN  14

//PWM Channels - max 16 independent channels
#define PWM_CHANNEL_1 0
#define PWM_CHANNEL_2 1
#define PWM_CHANNEL_3 2
#define PWM_CHANNEL_4 3

#define PWM_RES 13 // 13 bit resolution = 2^13-1 = 8191 propotion
#define PWM_RESOLUTION  8191 // 2^13-1
// ------ Public function prototypes --------------------------
/**
Initialize Pump1
**/
void pump1_init();
/**
Initialize Pump2
**/
void pump2_init();
/**
Read pwm freq of pump1
**/
float pump1pwm_read();
/**
Read pwm freq of pump2
**/
float pump2pwm_read();
/**
Pump1 current Status
**/
void pump1_status();
/**
Pump2 current Status
**/
void pump2_status();
/**
Put Pump1 in maxspeed
**/
void pump1_maxspeed();
/**
Put Pump2 in maxspeed
**/
void pump2_maxspeed();
/**
Put Pump1 in minspeed
**/
void pump1_minspeed();
/**
Put Pump2 in minspeed
**/
void pump2_minspeed();
/**
Make Pump1 faster (in % but not exceed the limit percentage of the pump)
**/
void pump1_faster(float);
/**
Make Pump2 faster (in % but not exceed the limit percentage of the pump)
**/
void pump2_faster(float);
/**
Make Pump1 slower (in % but not exceed the limit percentage of the pump)
**/
void pump1_slower(float);
/**
Make Pump2 slower (in % but not exceed the limit percentage of the pump)
**/
void pump2_slower(float);
/**
Change pwm frequency of pump1 directly through internet
**/
void pump1_wifiChange(float);
/**
Change pwm frequency of pump2 directly through internet
**/
void pump2_wifiChange(float);
/**
Turn OFF Pump1
**/
void pump1_OFF();
/**
Turn OFF Pump2
**/
void pump2_OFF();

void Pump_output(float);
// ------ Public variable -------------------------------------

#endif //__ESP32_PWM_PUMP_H
