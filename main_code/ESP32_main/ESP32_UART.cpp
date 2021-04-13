/*-------------------------------------------
  UART - functions file
  ESP32 DEVKIT V1
  (c) Can Tho University 2019 
  version 1.10 - 26/08/2019
----------------------------------------------
 * All test function used with UART to USB has
 * to be in "No line ending" mode
 --------------------------------------------*/ 
#ifndef  __ESP32_UART_CPP
#define  __ESP32_UART_CPP
#include "ESP32_UART.h"
#include "ESP32_FlowSensor.h"

// ------ Private constants -----------------------------------

// ------ Private function prototypes -------------------------

// ------ Private variables -----------------------------------
uint32_t uartLastMillis;
// ------ PUBLIC variable definitions -------------------------

//--------------------------------------------------------------
// FUNCTION DEFINITIONS
//--------------------------------------------------------------
void UART_init()
{
  Serial.begin(57600);
  uartLastMillis = millis();
}
//------------------------------------------------------------
void UART_masterReady() {
  Serial.print('K');
  Serial.print(AUTHORIZED_KEY);
  // bool not_authorized = true;
  // uint32_t lastmillis = millis();
//  while (not_authorized) { //while not authorized yet
//    if ((millis()-lastmillis)>3000) { //only happend once every 3s
//        Serial.print('K');
//        Serial.print(AUTHORIZED_KEY);
//        lastmillis = millis();
//    }//end if
//    while (Serial.available()){
//      if (Serial.read()=='U') {
//        String buff = Serial.readString();
//        if (buff=="pdated!") {
//          not_authorized=false;
//        }//end if
//      }//end if
//    }//end while
//  }//end while
  char Smes[60];
  snprintf(Smes,60,"S|%f|%f|%f|%f|%f|%d|%d|%d|%d",NVS_read_Kp(),NVS_read_Ki(),NVS_read_Kd(),NVS_read_F1(),NVS_read_F2(),NVS_read_T1(),NVS_read_T2(),NVS_read_T3(),NVS_read_T4());
  Serial.println(Smes);
  // lastmillis = millis();
//  while (1) {
//    if ((millis()-lastmillis)>3000) { //only happend once every 3s
//        Serial.println(Smes); //resend if no confirmation was received
//        lastmillis = millis();
//    }//end if
//    while (Serial.available()){
//      if (Serial.read()=='U') {
//        String buff = Serial.readString();
//        if (buff=="pdated!") {
//          return;
//        }//end if
//      }//end if
//    }//end while
//  }//end while
}//end UART_masterReady
//------------------------------------------------------------
 void UART_sendToSlave(int timeInterval) {//command: T|T1|T2|T3|T4_F|F1|F2
  if ((millis()-uartLastMillis)>timeInterval*1000) {
    uartLastMillis = millis();
    char Smes[60];
    snprintf(Smes,60,"T|%d|%d|%d|%d_F|%f_PWM|%f",tempSen01_read(),tempSen02_read(),tempSen03_read(),tempSen04_read(),FlowSensor_get_flow_polling(),pump1pwm_read());
    Serial.println(Smes);
  }//end if
}// end UART_sendToSlave
//------------------------------------------------------------
 void UART_PIDsendToSlave() {//command: P|Kp|Ki|Kd
  char Smes[60];
  snprintf(Smes,60,"P|%f|%f|%f",NVS_read_Kp(),NVS_read_Ki(),NVS_read_Kd());
  Serial.println(Smes);
}// end UART_sendToSlave
//------------------------------------------------------------
void UART_getFromSlave() {// P|Kp|Ki|Kd or T|T1|T2|T3|T4 or F|F1|F2
  if( Serial.available()) {//if something appear in the serial monitor
    char charBuf = Serial.read();
    if (charBuf=='P') {
      uint16_t p1,p2,p3;
      String bKp="",bKi="",bKd="",rec="";
      rec=Serial.readString();
      p1=rec.indexOf("|",0); //search for initial signal
      if (p1>=0) { //has the correct signal
        //pid parameters
        p1=rec.indexOf("|",0);    //position of Kp
        p2=rec.indexOf("|",p1+1); //position of Ki
        p3=rec.indexOf("|",p2+1); //position of Kd
        bKp=rec.substring(p1+1,p2);//get the string out
        bKi=rec.substring(p2+1,p3);//get the string out
        bKd=rec.substring(p3+1);//get the string out
        NVS_PID_write(bKp.toFloat(),bKi.toFloat(),bKd.toFloat());
        MQTT_Kp_pub(NVS_read_Kp());delay(300);
        MQTT_Ki_pub(NVS_read_Ki());delay(300);
        MQTT_Kd_pub(NVS_read_Kd());
        D_PRINTLN(F("PID saved!"));
      } else {
         D_PRINTLN(F("Not recognized command!"));
      }// end if else
    } else if (charBuf=='T') {
      uint16_t t1,t2,t3,t4;
      String bT1="",bT2="",bT3="",bT4="",rec="";
      rec=Serial.readString();
      t1=rec.indexOf("|",0); //search for initial signal
      if (t1>=0) { //has the correct signal
        //set temperature values
        t1=rec.indexOf("|",0);    //position of temp1
        t2=rec.indexOf("|",t1+1); //position of temp2
        t3=rec.indexOf("|",t2+1); //position of temp3
        t4=rec.indexOf("|",t3+1); //position of temp4
        bT1=rec.substring(t1+1,t2);//get the string out
        bT2=rec.substring(t2+1,t3);//get the string out
        bT3=rec.substring(t3+1,t4);//get the string out
        bT4=rec.substring(t4+1);   //get the string out
        NVS_Temp_write(bT1.toInt(),bT2.toInt(),bT3.toInt(),bT4.toInt());
        D_PRINTLN(F("temp saved!"));
        
      } else {
         D_PRINTLN(F("Not recognized command!"));
      }// end if else
    } else if (charBuf=='F') {
      uint16_t f1,f2;
      String bF1="",bF2="", rec="";
      rec=Serial.readString();
      f1=rec.indexOf("|",0); //search for initial signal
      if (f1>=0) { //has the correct signal
        //set flow values
        f1=rec.indexOf("|",0);    //position of flow1
        f2=rec.indexOf("|",f1+1); //position of flow2
        bF1=rec.substring(f1+1,f2);//get the string out
        bF2=rec.substring(f2+1);//get the string out
        NVS_Flow_write(bF1.toFloat(),bF2.toFloat());
        D_PRINTLN(F("flow saved!"));
      } else {
         D_PRINTLN(F("Not recognized command!"));
      }// end if else 
    } else {
      D_PRINTLN(F("Not recognized command!"));
    }// end if else
  }//end if
}//end UART_getFromSlave
//--------------------------------
#endif // __ESP32_UART_CPP
