/*------------------------------------------------------------*-
  PID - functions file
  ESP32 DEVKIT V1
  (c) Can Tho University 2019
  version 1.00 - 14/08/2019
---------------------------------------------------------------
 * ESP-IDF version: 3.2
 * Compiler version: 5.2.0
 * Arduino components version: latest
--------------------------------------------------------------*/
#ifndef __ESP32_PID_CPP
#define __ESP32_PID_CPP
#include "ESP32_PID.h"

// ------ Private constants -----------------------------------

// ------ Private function prototypes -------------------------

// ------ Private variables -----------------------------------
float previous_error;
float f_previous_error;
uint32_t lastTime;
//PID constants:
float kp,ki,kd;
//PID constants:
float F_kp,F_ki,F_kd;
// ------ PUBLIC variable definitions -------------------------

//--------------------------------------------------------------
// FUNCTION DEFINITIONS
//--------------------------------------------------------------
void PID_init() {
  kp = 30;// 17 NVS_read_Kp();
  ki = NVS_read_Ki();
  kd = NVS_read_Kd();
  lastTime = millis();
  previous_error = 0;
}//end PID_init
//------------------------------------
void F_PID_init() {
  F_kp = 17;//NVS_read_Kp();
  //ki = NVS_read_Ki();
  //kd = NVS_read_Kd();
  lastTime = millis();
  f_previous_error = 0;
}//end PID_init
//------------------------------------
float PID_Kp_read() {
  return kp;
}//end PID_Kp_read
//------------------------------------
float PID_Ki_read() {
  return ki;
}//end PID_Ki_read
//------------------------------------
float PID_Kd_read() {
  return kd;
}//end PID_Kd_read
//------------------------------------
void PID_Kp_write(float PIDval) {
  kp = PIDval;
  NVS_PID_write(kp,ki,kd);
}//end PID_Kp_write
//------------------------------------  Remember to add function to NVS
void F_PID_Kp_write(float F_PIDval) {
  F_kp = F_PIDval;
  //NVS_F_PID_write(F_kp,F_ki,F_kd);
}//end PID_Kp_write
//------------------------------------
void PID_Ki_write(float PIDval) {
  ki = PIDval;
  NVS_PID_write(kp,ki,kd);
}//end PID_Ki_write
//------------------------------------
void PID_Kd_write(float PIDval) {
  kd = PIDval;
  NVS_PID_write(kp,ki,kd);
}//end PID_Kd_write
//------------------------------------
float F_PIDcal(int F_setVal,int F_realVal) {
  float elapsedTime, timePrev;
  float F_PID_value = 0;
  float F_PID_p = 0;    
  //float PID_i = 0;
  //float PID_d = 0;
  //We calculate the error between the setpoint and the real value
  float F_PID_error = F_setVal - F_realVal;
  //Calculate the P value
  F_PID_p = F_kp / F_PID_error;
  //Calculate the I value in a range on +-3
  //if(-3 < PID_error <3) {PID_i = PID_i + (ki * PID_error);}

  //For derivative we need real time to calculate speed change rate
  timePrev = lastTime; // the previous time is stored before the actual time read
  lastTime = millis(); // actual time read
  elapsedTime = (lastTime - timePrev) / 1000; 
  //Now we can calculate the D calue
  //PID_d = kd*((PID_error - previous_error)/elapsedTime);
  //Final total PID value is the sum of P + I + D
  F_PID_value = F_PID_p; //+ PID_i + PID_d;
  
  //We define PID range between -1000 and 1000
  if(F_PID_value < 0){F_PID_value = 1;}
  if (F_PID_value > 1){F_PID_value = 1;}
  //Remember to store the previous error for next loop.
  f_previous_error = F_PID_error;  

  return (F_PID_value); //to make return value goes from -0.1 to 0.1
}//end F_PID_cal
//------------------------------------
float PIDcal(int setVal,int realVal) {
  float elapsedTime, timePrev;
  float PID_value = 0;
  float PID_p = 0;    
  //float PID_i = 0;
  //float PID_d = 0;
  //We calculate the error between the setpoint and the real value
  float PID_error = setVal - realVal;
  //Calculate the P value
  if(PID_error < 1){PID_error = 1;}
  
  PID_p = kp / PID_error;
  //Calculate the I value in a range on +-3
  //if(-3 < PID_error <3) {PID_i = PID_i + (ki * PID_error);}

  //For derivative we need real time to calculate speed change rate
  timePrev = lastTime; // the previous time is stored before the actual time read
  lastTime = millis(); // actual time read
  elapsedTime = (lastTime - timePrev) / 1000; 
  //Now we can calculate the D calue
  //PID_d = kd*((PID_error - previous_error)/elapsedTime);
  //Final total PID value is the sum of P + I + D
  PID_value = PID_p; //+ PID_i + PID_d;
  
  //We define PID range between -1000 and 1000
  if(PID_value < 0){PID_value = 1;}
  if (PID_value > 1){PID_value = 1;}
  //Remember to store the previous error for next loop.
  previous_error = PID_error;  

  return (PID_value); //to make return value goes from -0.1 to 0.1
}//end PID_cal

#endif //__ESP32_PID_CPP
