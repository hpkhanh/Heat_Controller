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
#include "ESP32_FlowSensor.h"
#include "debugConfig.h"
#include <nvs.h>

// ------ Private constants -----------------------------------
#if (COMPANY_NAME == FECTUM)
#define STARTUP_STATE STATE_INIT_1
#elif (COMPANY_NAME == SOLESTA)
#define STARTUP_STATE STATE_INIT
#endif

typedef enum {
#if (COMPANY_NAME == FECTUM)
  STATE_INIT_1,
  STATE_INIT_2,
  STATE_READY_1,
  STATE_READY_2,
  STATE_STARTUP_1,
  STATE_STARTUP_2,
  STATE_RUN_1,
  STATE_RUN_2,
  STATE_PAUSE_1,
  STATE_PAUSE_2,
#elif (COMPANY_NAME == SOLESTA)
  STATE_INIT,
  STATE_READY,
  STATE_STARTUP,
  STATE_RUN,
  STATE_PAUSE,
#endif

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
  nvs_flash_init();
  UART_init();
  ADC_init();
  PID_init();
  pump1_init();
  relay_init();
  FlowSensor_init();
  UART_masterReady();
  core0_init(); //must stand above MQTT init
  MQTT_init();

  
  xSemaphoreTake(baton, portMAX_DELAY); // ( TickType_t ) and portTICK_PERIOD_MS is also available , view: http://esp32.info/docs/esp_idf/html/d1/d19/group__xSemaphoreTake.html 
  xSemaphoreGive(baton);

}//end System_init
//------------------------------------------------------------
void mainRoutine() {
  char Smes[50];
 //################################# STATE CHECKING ##################################
  switch (CurrentState) {
   // ================================== STATE A ============================
#if (COMPANY_NAME == FECTUM)
    case  STATE_INIT_1: {
    if (stateChanged)
      {
      stateChanged = false;
      snprintf(Smes,50,"Z|%d| STATE_INIT_1",CurrentState);
      S_PRINTLN(Smes);

      }
    //pump1_OFF(); //PWM_1=0
    Pump_output(0);
    relay01(OFF); //Valve=0
    //lastMillis = millis();

    if (tempSen01_read()<60)
    {
      //Serial.println(millis()); //prints time since program started
      //Serial.println(lastMillis); //prints time since program started
      
      if (tempSen02_read() < tempSen04_read())
      {
        if ((millis()-lastMillis)>10000)
        {
          lastMillis = millis();
          CurrentState = STATE_INIT_2; // STATE_B
          stateChanged = true;
          break;  
        }         
      }
      else {lastMillis = millis();}     
     }//end if
     
    else if (tempSen01_read()>60) 
    { // T_collector >60
      //Serial.println(millis()); //prints time since program started
      //Serial.println(lastMillis); //prints time since program started
      if (tempSen02_read() >= tempSen04_read())
      {
        if((millis()-lastMillis)>1000*30)
        {
          lastMillis = millis();
          CurrentState = STATE_READY_1; // STATE_B
          stateChanged = true;
          break;
        }       
      }
      if (tempSen02_read() < tempSen04_read())
      {
       if((millis()-lastMillis)>1000*30)
        {
          lastMillis = millis();
          CurrentState = STATE_READY_2; // STATE_B
          stateChanged = true;
          break;
        }      
      }
      
    }//end if
      
    //else {lastMillis = millis();}

      break;
    }//end CASE_A
  
  // ================================== STATE A1 ============================
    case  STATE_INIT_2: {
    if (stateChanged)
    {
      stateChanged = false;
      snprintf(Smes,50,"Z|%d| STATE_INIT_2",CurrentState);
      S_PRINTLN(Smes);

    }
    Pump_output(0);
    //pump1_OFF(); //PWM_1=0
    relay01(ON); //Valve=0
      
    if (tempSen01_read()<60)
    {
      //Serial.println(millis()); //prints time since program started
      //Serial.println(lastMillis); //prints time since program started
      
      if (tempSen02_read() >= tempSen04_read())
      {
        if ((millis()-lastMillis)>10000)
        {
          lastMillis = millis();
          CurrentState = STATE_INIT_1; // STATE_B
          stateChanged = true;
          break;  
        }         
      }
      else {lastMillis = millis();}
      
     }//end if
   

      else if (tempSen01_read()>60) 
      { // T_collector >60
        //Serial.println(millis()); //prints time since program started
        //Serial.println(lastMillis); //prints time since program started
        if (tempSen02_read() >= tempSen04_read())
        {
          if((millis()-lastMillis)>10000)
          {
            lastMillis = millis();
            CurrentState = STATE_READY_1; // STATE_B
            stateChanged = true;
            break;
          }       
        }
        if (tempSen02_read() < tempSen04_read())
        {
         if((millis()-lastMillis)>10000)
          {
            lastMillis = millis();
            CurrentState = STATE_READY_2; // STATE_B
            stateChanged = true;
            break;
          }      
        }
      
    }//end if
      
    //else {lastMillis = millis();}

      break;
    }//end CASE_A1
  
   // ================================== STATE B ============================
    case  STATE_READY_1: {
    
    if (stateChanged)
    {
      stateChanged = false;
      snprintf(Smes,50,"Z|%d| Ready 01 State",CurrentState);
      S_PRINTLN(Smes);
    }
      Pump_output(0);
      //pump1_OFF(); //PWM_1=0
      relay01(OFF); //Valve=0

    if (tempSen01_read()<50) 
    { // T_collector <50
      if (tempSen02_read() >= tempSen04_read())
      {
        CurrentState = STATE_INIT_1; // STATE_A
        stateChanged = true;
        break;
      }
      else if (tempSen02_read() < tempSen04_read())
      {
        CurrentState = STATE_INIT_2; // STATE_A
        stateChanged = true;
        break;
      }   
    }//end if    
       
    if ((tempSen02_read()<85) && tempSen01_read()>=50)
    { //Serial.println(millis()); //prints time since program started
      //Serial.println(lastMillis); //prints time since program started 
      if (tempSen02_read() >= tempSen04_read())
      { //after 5secs
        if ((millis()-lastMillis)>10000)  //back to 10000
        {
          lastMillis = millis();
          CurrentState = STATE_STARTUP_1; // STATE_C
          stateChanged = true;
          break;
        }         
      }
      else if (tempSen02_read() < tempSen04_read())
      { //after 5secs
        if ((millis()-lastMillis)>10000)  //back to 10000
        {
          lastMillis = millis();
          CurrentState = STATE_STARTUP_2; // STATE_C
          stateChanged = true;
          break;
          }
         
      }//end if
      //else {lastMillis = millis();}
    } 

    if ((tempSen02_read()>=85)&& tempSen01_read()>=50)
    { 
      if (tempSen02_read() < tempSen04_read())
      { //after 5secs
        if((millis()-lastMillis)>10000)
        {
          lastMillis = millis();
          CurrentState = STATE_READY_2; // STATE_C
          stateChanged = true;
          break;
        }
       
      }//end if
      else {lastMillis = millis();}
    }    
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
    
      Pump_output(0);
      //pump1_OFF(); //PWM_1=0
      relay01(ON); //Valve=0

     if (tempSen01_read()<50) 
    { // T_collector <50
      if (tempSen02_read() >= tempSen04_read())
      {
        CurrentState = STATE_INIT_1; // STATE_A
        stateChanged = true;
        break;
      }
      else if (tempSen02_read() < tempSen04_read())
      {
        CurrentState = STATE_INIT_2; // STATE_A
        stateChanged = true;
        break;
      }   
    }//end if    
  
    if ((tempSen02_read()<85) && tempSen01_read()>=50)
    { //Serial.println(millis()); //prints time since program started
      //Serial.println(lastMillis); //prints time since program started 
      if (tempSen02_read() >= tempSen04_read())
      { //after 5secs
        if ((millis()-lastMillis)>10000) //back to 10000
        {
          lastMillis = millis();
          CurrentState = STATE_STARTUP_1; // STATE_C
          stateChanged = true;
          break;
        }         
      }
      else if (tempSen02_read() < tempSen04_read())
      { //after 5secs
        if ((millis()-lastMillis)>10000)  //back to 10000
        {
          lastMillis = millis();
          CurrentState = STATE_STARTUP_2; // STATE_C
          stateChanged = true;
          break;
          }
         
      }//end if
      //else {lastMillis = millis();}
    } 

    if ((tempSen02_read()>=85)&& tempSen01_read()>=50)
    { 
      if (tempSen02_read() >= tempSen04_read())
      { //after 5secs
        if((millis()-lastMillis)>10000)
        {
          lastMillis = millis();
          CurrentState = STATE_READY_1; // STATE_C
          stateChanged = true;
          break;
        }
       
      }//end if
      else {lastMillis = millis();}
    }    
      break;
    }//end CASE_B1

   // ================================== STATE C ============================
    case  STATE_STARTUP_1: {
      if (stateChanged)
      {
        stateChanged = false;
        snprintf(Smes,50,"Z|%d| Startup State 1",CurrentState);
        S_PRINTLN(Smes);
      }
      Pump_output(1);
      //pump1_maxspeed(); //PWM_1=1
      relay01(OFF); //Valve=0
      
//      if ((millis()-lastMillis)>10000 && (millis()-lastMillis)<=15000) 
//      { // after 10s 
//        pump1_OFF(); //PWM_1=0
//        relay01(OFF); //Valve=0
//      }
//      if ((millis()-lastMillis)>15000 && (millis()-lastMillis)<=45000)
//      {
//        pump1_maxspeed(); //PWM_1=1
//        relay01(OFF); //Valve=0
//      }

      if ((millis()-lastMillis)>45000)
      {
        lastMillis = millis();
        CurrentState = STATE_RUN_1; //STATE_D
        stateChanged = true;
        break;
        }  //end
      break;
    }//end CASE_C
  
  // ================================== STATE C1 ============================
    case  STATE_STARTUP_2: {
    if (stateChanged)
    {
      stateChanged = false;
      snprintf(Smes,50,"Z|%d| Startup State 2",CurrentState);
      S_PRINTLN(Smes);
    }
      Pump_output(1);
      //pump1_maxspeed(); //PWM_1=1
      relay01(ON); //Valve=0
      
//    if ((millis()-lastMillis)>10000 && (millis()-lastMillis)<=15000) 
//    { // after 10s 
//      pump1_OFF(); //PWM_1=0
//      relay01(ON); //Valve=0
//    }
//    if ((millis()-lastMillis)>15000 && (millis()-lastMillis)<=45000)
//    {
//      pump1_maxspeed(); //PWM_1=1
//      relay01(ON); //Valve=0
//      }

    if ((millis()-lastMillis)>45000)
    {
      lastMillis = millis();
      CurrentState = STATE_RUN_2; //STATE_D
      stateChanged = true;
      break;
      }  //end
    break;
    }//end CASE_C1

   // ================================== STATE D ============================
    case  STATE_RUN_1: {
    if (stateChanged)
      {
      stateChanged = false;
      snprintf(Smes,50,"Z|%d| Run 01 State",CurrentState);
      S_PRINTLN(Smes);
      }
      
    relay01(OFF); //Valve=0
      //-------------NEED TO BE FIX HERE!----------------
    int SP = 150;
    float temp = PIDcal(SP,tempSen01_read()); //NVS_read_T1()
      //-------------------------------------------------
    Pump_output(temp);
    
    //---------------------//----------------------
    if(tempSen01_read()>50 && tempSen02_read()<=85 && tempSen02_read() >= tempSen04_read())
    {
      lastMillis = millis();
    }
      
    else if(tempSen01_read()>50 && tempSen02_read()<=85 && tempSen02_read() < tempSen04_read())
    {
        if ((millis()-lastMillis)>10000)
        { //after 10 secs
          lastMillis = millis();
          CurrentState = STATE_RUN_2; // STATE_C
          stateChanged = true;
          break;
        }      
      //else {lastMillis = millis();}
    }      
    else if ((tempSen02_read()>85)&& tempSen02_read() >= tempSen04_read())
    { 
      if ((millis()-lastMillis)>10000)
      { //after 5secs
        lastMillis = millis();
        CurrentState = STATE_PAUSE_1; // STATE_C
        stateChanged = true;
        break;
      }//end if
    }
    else if ((tempSen02_read()>85)&& tempSen02_read() < tempSen04_read())
    { 
      if ((millis()-lastMillis)>10000)
      { //after 5secs
        lastMillis = millis();
        CurrentState = STATE_PAUSE_2; // STATE_C
        stateChanged = true;
        break;
      }//end if
    }          
    else if ((tempSen01_read()<=50)&& tempSen02_read() >= tempSen04_read()) 
    { // T_collector <50
      if ((millis()-lastMillis)>10000)
      {
        lastMillis = millis();
        CurrentState = STATE_PAUSE_1; // STATE_A
        stateChanged = true;
        break;
      }
    }

    else if ((tempSen01_read()<=50)&& tempSen02_read() < tempSen04_read()) 
    { // T_collector <50
      if ((millis()-lastMillis)>10000)
      {
        lastMillis = millis();
        CurrentState = STATE_PAUSE_2; // STATE_A
        stateChanged = true;
        break;
      }
    }   
    //else {lastMillis = millis();}
      
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
    
      relay01(ON); //Valve=0
      //-------------NEED TO BE FIX HERE!----------------
    
      int SP = 150;
      float temp = PIDcal(SP,tempSen01_read()); //NVS_read_T1()
    
      //-------------------------------------------------
    
      Pump_output(temp);
    if(tempSen01_read()>50 && tempSen02_read()<=85 && tempSen02_read() < tempSen04_read())
    {
      lastMillis = millis();
    }
      
    else if(tempSen01_read()>50 && tempSen02_read()<=85 && tempSen02_read() >= tempSen04_read())
    {
        if ((millis()-lastMillis)>10000)
        { //after 10 secs
          lastMillis = millis();
          CurrentState = STATE_RUN_1; // STATE_C
          stateChanged = true;
          break;
        }      
      //else {lastMillis = millis();}
    }      
    else if ((tempSen02_read()>85)&& tempSen02_read() >= tempSen04_read())
    { 
      if ((millis()-lastMillis)>10000)
      { //after 5secs
        lastMillis = millis();
        CurrentState = STATE_PAUSE_1; // STATE_C
        stateChanged = true;
        break;
      }//end if
    }
    else if ((tempSen02_read()>85)&& tempSen02_read() < tempSen04_read())
    { 
      if ((millis()-lastMillis)>10000)
      { //after 5secs
        lastMillis = millis();
        CurrentState = STATE_PAUSE_2; // STATE_C
        stateChanged = true;
        break;
      }//end if
    }          
    else if ((tempSen01_read()<=50)&& tempSen02_read() >= tempSen04_read()) 
    { // T_collector <50
      if ((millis()-lastMillis)>10000)
      {
        lastMillis = millis();
        CurrentState = STATE_PAUSE_1; // STATE_A
        stateChanged = true;
        break;
      }
    }

    else if ((tempSen01_read()<=50)&& tempSen02_read() < tempSen04_read()) 
    { // T_collector <50
      if ((millis()-lastMillis)>10000)
      {
        lastMillis = millis();
        CurrentState = STATE_PAUSE_2; // STATE_A
        stateChanged = true;
        break;
      }
    }   
    //else {lastMillis = millis();}

      break;
    }//end CASE_D1
    
    // ================================== STATE E ============================
    case  STATE_PAUSE_1: {
      if (stateChanged)
      {
        stateChanged = false;
        snprintf(Smes,50,"Z|%d| Pause State 1",CurrentState);
        S_PRINTLN(Smes);
      }
      Pump_output(0);
      //pump1_OFF(); //PWM_1=0
      relay01(OFF); //Valve=0

      if ((millis()-lastMillis)>60000*8)
      {
        lastMillis = millis();
        CurrentState = STATE_READY_1; //STATE_D
        stateChanged = true;
        break;
        }  //end
    
      break;
    }//end CASE_E

    // ================================== STATE E1 ============================
    case  STATE_PAUSE_2: {
      if (stateChanged)
      {
        stateChanged = false;
        snprintf(Smes,50,"Z|%d| Pause State 2",CurrentState);
        S_PRINTLN(Smes);
      }
      Pump_output(0);
      //pump1_OFF(); //PWM_1=0
      relay01(ON); //Valve=1

      if ((millis()-lastMillis)>60000*8)
      {
        lastMillis = millis();
        CurrentState = STATE_READY_2; //STATE_D
        stateChanged = true;
        break;
        }  //end
    
      break;
    }//end CASE_E
#elif (COMPANY_NAME == SOLESTA)
    case  STATE_INIT: {
      if (stateChanged)
      {
        stateChanged = false;
        snprintf(Smes,50,"Z|%d| Initial State",CurrentState);
        S_PRINTLN(Smes);
      }
    //pump1_OFF(); //PWM_1=0
    Pump_output(0);
    relay01(OFF); //Valve=0
    //lastMillis = millis();
    
      if (tempSen01_read()>60) 
      { // T_collector >60
        if((millis()-lastMillis)>1000*30)
        {
          lastMillis = millis();
          CurrentState = STATE_READY; // STATE_B
          stateChanged = true;
          break;
        }        
      }//end if
      
      else {lastMillis = millis();}

    break;
    }//end CASE_A
  
   // ================================== STATE B ============================
    case  STATE_READY: {
    
      if (stateChanged)
      {
        stateChanged = false;
        snprintf(Smes,50,"Z|%d| Ready 01 State",CurrentState);
        S_PRINTLN(Smes);
      }
    
      //pump1_OFF(); //PWM_1=0
      Pump_output(0);
      relay01(OFF); //Valve=0

      if (tempSen01_read()<50) 
      { // T_collector <50
          lastMillis = millis();    
          CurrentState = STATE_INIT; // STATE_A
          stateChanged = true;
          break;     
      }//end if    
          
      if ((tempSen02_read()<80) && tempSen01_read()>=50)
      { //Serial.println(millis()); //prints time since program started
        //Serial.println(lastMillis); //prints time since program started 
        if ((millis()-lastMillis)>10000)  //back to 10000
          {
            lastMillis = millis();
            CurrentState = STATE_STARTUP; // STATE_C
            stateChanged = true;
            break;
          }             
      } 

      else {lastMillis = millis();}
  
    break;
    }//end CASE_B

   // ================================== STATE C ============================
    case  STATE_STARTUP: {
      if (stateChanged)
      {
        stateChanged = false;
        snprintf(Smes,50,"Z|%d| Startup State",CurrentState);
        S_PRINTLN(Smes);
      }
      //pump1_maxspeed(); //PWM_1=1
      Pump_output(1);
      relay01(OFF); //Valve=0
        
      if ((millis()-lastMillis)>30000)
      {
        lastMillis = millis();
        CurrentState = STATE_RUN; //STATE_D
        stateChanged = true;
        break;
        }  //end
      break;
    }//end CASE_C
  
   // ================================== STATE D ============================
    case  STATE_RUN: {
      if (stateChanged)
      {
        stateChanged = false;
        snprintf(Smes,50,"Z|%d| Run 01 State",CurrentState);
        S_PRINTLN(Smes);
      }
      
      relay01(OFF); //Valve=0
      //-------------NEED TO BE FIX HERE!----------------
      int SP = 150;
      float temp = PIDcal(SP,tempSen01_read()); //NVS_read_T1()
      //-------------------------------------------------
      Pump_output(temp);
    
      //---------------------//----------------------
      if(tempSen01_read()>50 && tempSen02_read()<=80 && tempSen02_read() <= tempSen01_read())
      {
        lastMillis = millis();
      }
      
      else if (tempSen02_read()>80 || tempSen02_read() > tempSen01_read() || tempSen01_read()<=50)
      { 
        if ((millis()-lastMillis)>10000)
        { //after 5secs
          lastMillis = millis();
          CurrentState = STATE_PAUSE; // STATE_C
          stateChanged = true;
          break;
        }//end if
      }

    //else {lastMillis = millis();}
      
      break;
    }//end CASE_D
 // ================================== STATE E ============================
    case  STATE_PAUSE: {
      if (stateChanged)
      {
        stateChanged = false;
        snprintf(Smes,50,"Z|%d| Pause State",CurrentState);
        S_PRINTLN(Smes);
      }
      //pump1_OFF(); //PWM_1=0
      Pump_output(0);
      relay01(OFF); //Valve=0

      if ((millis()-lastMillis)>60000*4)
      {
        lastMillis = millis();
        CurrentState = STATE_READY; //STATE_D
        stateChanged = true;
        break;
        }  //end
    
      break;
    }//end CASE_E
#endif   // COMPANY_NAME
  }//end switch
//###################################################################################
}//end mainRoutine

#endif // __ESP32_STATEFLOW_CPP