/*------------------------------------------------------------*-
  PID - header file
  ESP32 DEVKIT V1
  (c) Can Tho University 2019
  version 1.00 - 14/08/2019
---------------------------------------------------------------
 * ESP-IDF version: 3.2
 * Compiler version: 5.2.0
 * Arduino components version: latest
--------------------------------------------------------------*/
#ifndef __ESP32_PID_H
#define __ESP32_PID_H
#include <WiFi.h>
#include "ESP32_NVS.h"
// ------ Public constants ------------------------------------

// ------ Public function prototypes --------------------------
/**
PID initialize
*/
void PID_init();
/**
Flow control PID initialize
*/
void F_PID_init();
/**
Read Kp value
*/
float PID_Kp_read();
/**
Read Ki value
*/
float PID_Ki_read();
/**
Read Kd value
*/
float PID_Kd_read();
/**
Fix Kp value
*/
void PID_Kp_write(float);
/**
Fix Kp value
*/
void F_PID_Kp_write(float);
/**
Fix Ki value
*/
void PID_Ki_write(float);
/**
Fix Kd value
*/
void PID_Kd_write(float);
/**
PID calculation, return value from 0.01 to 1.
*/
float PIDcal(int,int);
/**
PID calculation, return value from 0.01 to 1.
*/
float F_PIDcal(int,int);

// ------ Public variable -------------------------------------

#endif //__ESP32_PID_H
