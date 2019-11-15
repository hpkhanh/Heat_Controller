/*------------------------------------------------------------*-
  STATE FLOW - functions file
  ESP32 DEVKIT V1
  (c) Can Tho University - 2019
  version 1.00 - 23/10/2019
---------------------------------------------------------------
 * ESP-IDF version: 3.2
 * Compiler version: 5.2.0
 * Arduino components version: latest
--------------------------------------------------------------*/
#ifndef  __ESP32_STATEFLOW_CPP
#define  __ESP32_STATEFLOW_CPP
#include "ESP32_stateFlow.h"

// ------ Private constants -----------------------------------
#define STARTUP_STATE STATE_INIT
typedef enum {
  STATE_INIT,
  STATE_READY_1,
  STATE_READY_2,
  STATE_STARTUP,
  STATE_STARTUP_PUMP2,
  STATE_TRANSITION,
  STATE_RUN_1,
  STATE_RUN_2,

  STATE_ERROR = 9999
} SystemState;
// ------ Private function prototypes -------------------------

// ------ Private variables -----------------------------------
SystemState CurrentState;
uint8_t stateChanged = true;
uint32_t lastMillis=0;
// ------ PUBLIC variable definitions -------------------------
SemaphoreHandle_t baton; //baton to make a core waiting for another core when needed
//--------------------------------------------------------------
// FUNCTION DEFINITIONS
//--------------------------------------------------------------
void System_init() {
  vSemaphoreCreateBinary(baton); //initialize binary semaphore //baton = xSemaphoreCreateBinary(); //this works too but not as good as the current use

  CurrentState = STARTUP_STATE;
  UART_init();
  ADC_init();
  PID_init();
  pump1_init();
  pump2_init();
  relay_init();
  UART_masterReady();
  core0_init(); //must stand above MQTT init

  
  xSemaphoreTake(baton, portMAX_DELAY); // ( TickType_t ) and portTICK_PERIOD_MS is also available , view: http://esp32.info/docs/esp_idf/html/d1/d19/group__xSemaphoreTake.html 
  xSemaphoreGive(baton);
}//end System_init
//------------------------------------------------------------
void mainRoutine() {
  char Smes[50];
 //################################# STATE CHECKING ##################################
  switch (CurrentState) {
   // ================================== STATE A ============================
    case  STATE_INIT: {
      if (stateChanged)
      {
        stateChanged = false;
        snprintf(Smes,50,"Z|%d| Initial State",CurrentState);
        S_PRINTLN(Smes);

      }
      pump1_OFF(); //PWM_1=0
      pump2_OFF(); //PWM_2=0
      relay01(OFF); //Valve=0

      if (tempSen01_read()>60) { // T_collector >60
        lastMillis = millis();
        CurrentState = STATE_READY_1; // STATE_B
        stateChanged = true;
        break;
      }//end if

      break;
    }//end CASE_A
   // ================================== STATE B ============================
    case  STATE_READY_1: {
      if (stateChanged)
      {
        stateChanged = false;
        snprintf(Smes,50,"Z|%d| Ready 01 State",CurrentState);
        S_PRINTLN(Smes);
      }
      pump1_OFF(); //PWM_1=0
      pump2_OFF(); //PWM_2=0
      relay01(OFF); //Valve=0
      
      if ((tempSen02_read()<80)&& //T_Buffer1 < 80
          (tempSen03_read()<50)&& //T_Buffer2 < 50
          (tempSen03_read()<tempSen01_read())) { //T_Buffer2<T_collector
            if ((millis()-lastMillis)>5000) { //after 5secs
              lastMillis = millis();
              CurrentState = STATE_STARTUP; // STATE_C
              stateChanged = true;
              break;
            }//end if
      } else {lastMillis = millis();}

      if (tempSen01_read()<50) { // T_collector <50
        CurrentState = STATE_INIT; // STATE_A
        stateChanged = true;
        break;
      }//end if

      if (tempSen04_read()<21) { // T_h <21
        lastMillis = millis();
        CurrentState = STATE_READY_2; // STATE_B1
        stateChanged = true;
        break;
      }//end if

      break;
    }//end CASE_B
   // ================================== STATE B1 ============================
    case  STATE_READY_2: {
      if (stateChanged)
      {
        stateChanged = false;
        snprintf(Smes,50,"Z|%d| Ready 02 State",CurrentState);
        S_PRINTLN(Smes);
      }
      pump1_OFF(); //PWM_1=0
      pump2_OFF(); //PWM_2=0
      relay01(OFF); //Valve=0

      if (tempSen01_read()<50) { // T_collector <50
        CurrentState = STATE_INIT; // STATE_A
        break;
      }//end if

      if (tempSen04_read()>21) { // T_h >21
        lastMillis = millis();
        CurrentState = STATE_READY_1; // STATE_B
        break;
      }//end if
      
      if ((tempSen02_read()<80)&& //T_Buffer1 < 80
          (tempSen03_read()<50)&& //T_Buffer2 < 50
          (tempSen03_read()<tempSen01_read())) { //T_Buffer2<T_collector
            if ((millis()-lastMillis)>5000) { //after 5secs
              lastMillis = millis();
              CurrentState = STATE_STARTUP_PUMP2; // STATE_C1
              break;
            }//end if
      } else {lastMillis = millis();}

      break;
    }//end CASE_B1
   // ================================== STATE C ============================
    case  STATE_STARTUP: {
      if (stateChanged)
      {
        stateChanged = false;
        snprintf(Smes,50,"Z|%d| Startup State",CurrentState);
        S_PRINTLN(Smes);
      }
      pump1_maxspeed(); //PWM_1=1
      pump2_OFF(); //PWM_2=0
      relay01(OFF); //Valve=0
      
      if ((millis()-lastMillis)>30000) { // after 30s 
        CurrentState = STATE_RUN_1; //STATE_D
        stateChanged = true;
        break;
      }//end if

      break;
    }//end CASE_C
   // ================================== STATE C1 ============================
    case  STATE_STARTUP_PUMP2: {
      if (stateChanged)
      {
        stateChanged = false;
        snprintf(Smes,50,"Z|%d| Startup Pump 02 State",CurrentState);
        S_PRINTLN(Smes);
      }
      pump1_maxspeed(); //PWM_1=1
      pump2_maxspeed(); //PWM_2=1
      relay01(ON); //Valve=1
      
      if ((millis()-lastMillis)>30000) { // after 30s
        CurrentState = STATE_RUN_2; //STATE_D1
        stateChanged = true;
        break;
      }//end if

      break;
    }//end CASE_C1
   // ================================== STATE C2 ============================
    case  STATE_TRANSITION: {
      if (stateChanged)
      {
        stateChanged = false;
        snprintf(Smes,50,"Z|%d| Transition State",CurrentState);
        S_PRINTLN(Smes);
      }
      pump2_maxspeed(); //PWM_2=1
      relay01(OFF); //Valve=0
      //-------------USE TEMP 1 TO CALCULATE THE PID----------------
      float temp = PIDcal(NVS_read_T1(),tempSen01_read());
      //-------------------------------------------------
      if (temp<0) {pump1_slower(-temp);} //PWM_1 = PWM1_in
      else        {pump1_faster(temp);} //PWM_1 = PWM1_in

      if ((millis()-lastMillis)>30000) { // after 30s
        CurrentState = STATE_RUN_2; //STATE_D1
        stateChanged = true;
        break;
      }//end if
      
      break;
    }//end CASE_C2
   // ================================== STATE D ============================
    case  STATE_RUN_1: {
      if (stateChanged)
      {
        stateChanged = false;
        snprintf(Smes,50,"Z|%d| Run 01 State",CurrentState);
        S_PRINTLN(Smes);
      }
      pump2_OFF(); //PWM_2=0
      relay01(OFF); //Valve=0
      //-------------USE TEMP 1 TO CALCULATE THE PID----------------
      float temp = PIDcal(NVS_read_T1(),tempSen01_read());
      //-------------------------------------------------
      if (temp<0) {pump1_slower(-temp);} //PWM_1 = PWM1_in
      else        {pump1_faster(temp);} //PWM_1 = PWM1_in
      
      if ((tempSen02_read()>80)|| //T_Buffer1 > 80
          (tempSen03_read()>50)|| //T_Buffer2 > 50
          (tempSen03_read()>tempSen01_read())) { //T_Buffer2>T_collector
            CurrentState = STATE_READY_1; // STATE_B
            stateChanged = true;
            break;
      }//end if

      if (tempSen01_read()<50) { // T_collector <50
        CurrentState = STATE_INIT; // STATE_A
        stateChanged = true;
        break;
      }//end if

      if (tempSen04_read()<21) { // T_h <21
        lastMillis = millis();
        CurrentState = STATE_TRANSITION; // STATE_C2
        stateChanged = true;
        break;
      }//end if

      break;
    }//end CASE_D
   // ================================== STATE D1 ============================
    case  STATE_RUN_2: {
      if (stateChanged)
      {
        stateChanged = false;
        snprintf(Smes,50,"Z|%d| Run 02 State",CurrentState);
        S_PRINTLN(Smes);
      }
      relay01(ON); //Valve=1
      //-------------USE TEMP 1 TO CALCULATE THE PID----------------
      float temp1 = PIDcal(NVS_read_T1(),tempSen01_read());
      //-------------------------------------------------
      if (temp1<0) {pump1_slower(-temp1);} //PWM_1 = PWM1_in
      else         {pump1_faster(temp1);} //PWM_1 = PWM1_in
      //-------------USE TEMP 1 TO CALCULATE THE PID----------------
      float temp2 = PIDcal(NVS_read_T1(),tempSen01_read());
      //-------------------------------------------------
      if (temp2<0) {pump2_slower(-temp2);} //PWM_2 = PWM2_in
      else         {pump2_faster(temp2);} //PWM_2 = PWM2_in
      
      if (tempSen04_read()<21) { // T_h <21
        CurrentState = STATE_RUN_1; // STATE_D
        stateChanged = true;
        break;
      }//end if

      if ((tempSen02_read()>80)|| //T_Buffer1 > 80
          (tempSen03_read()>50)|| //T_Buffer2 > 50
          (tempSen03_read()>tempSen01_read())) { //T_Buffer2>T_collector
            lastMillis = millis();
            CurrentState = STATE_READY_2; // STATE_B1
            stateChanged = true;
            break;
      }//end if

      if (tempSen01_read()<50) { // T_collector <50
        CurrentState = STATE_INIT; // STATE_A
        stateChanged = true;
        break;
      }//end if

      break;
    }//end CASE_D1
   // ================================== STATE_ERROR============================
    case STATE_ERROR: {
      S_PRINTLN("System ERROR!");
      
      break;
    }//end STATE_ERROR
  }//end switch
//###################################################################################
}//end mainRoutine

#endif // __ESP32_STATEFLOW_CPP
    
