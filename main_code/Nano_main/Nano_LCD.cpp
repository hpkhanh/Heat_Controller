/*------------------------------------------------------------*-
  LCD - functions file
  ARDUINO NANO
  (c) Can Tho University 2019
  version 1.20 - 05/08/2019
 --------------------------------------------------------------
 * Have to change the ADC in every power supply if buttons do not work
 * Also need to turn the LCD potentiometer if the LCD is too dark
 -------------------------------------------------------------- */
#ifndef  __NANO_LCD_CPP 
#define  __NANO_LCD_CPP
#include "Nano_LCD.h"
#include <string.h>

// ------ Private constants -----------------------------------
#define LCD_ADRESS 	0x27
#define LCD_WIDTH 	16
#define LCD_HEIGHT	2
#define BUTTON_PIN	A1
#define	ERROR		9999
typedef enum {
	BUT_LEFT=1,
	BUT_DOWN,
	BUT_CONFIRM,
	BUT_RIGHT,
	BUT_UP
} ButtonState;
typedef enum {
	MAIN_STATE,
	PID_STATE,
  SHOW_STATE,
	TEMP_STATE,
	FLOW_STATE,
  SET_TEMP,
  SET_FLOW,
} LCDstate;

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

#define CONFIRM_MAX 350
#define CONFIRM_MIN 170
#define UP_MAX      900 
#define UP_MIN      650 
#define DOWN_MAX    150
#define DOWN_MIN    60
#define LEFT_MAX    50
#define LEFT_MIN    0
#define RIGHT_MAX   550
#define RIGHT_MIN   350
// ------ Private function prototypes -------------------------
/**
Read the buttons
*/
int buttonRead();
/**
Main user interface for the system
*/
void LCD_menu();
/**
User interface for the PID calibration
*/
void PIDdisplay_editor();
/**
 *User interface for the temperature calibration
 */
void TempEditor();
/**
 *User interface for the flow calibration
 */
void FlowEditor();
/**
User interface for the temperature monitoring
*/
void LCD_temp();
/**
User interface for the water flow monitoring
*/
void LCD_flow();

/**
User interface for showing the current state
*/
void LCD_showState();
// ------ Private variables -----------------------------------
int currentState=MAIN_STATE;
bool PIDchange=false;
int LCDpointer=1;
uint16_t Temp[4]={0,0,0,0};// an array that saves temperature T1 to T4 correspondingly
uint16_t sTemp[4]={0,0,0,0};// an array that saves the user-set temperature T1 to T4 correspondingly
float flow[2]={0,0};// an array that saves flow sensor values correspondingly
float sFlow[2]={0,0};// an array that saves the user-set flow values correspondingly
float PID[3]={0,0,0};// an array that saves kp,ki,kd correspondingly
uint16_t programState = 0; // Store state flow state value
// ------ PUBLIC variable definitions -------------------------
LiquidCrystal_I2C lcd(LCD_ADRESS, LCD_WIDTH, LCD_HEIGHT);
//--------------------------------------------------------------
// FUNCTION DEFINITIONS
//--------------------------------------------------------------
void LCD_init() 
{
	currentState=MAIN_STATE;
	lcd.init();
	lcd.backlight();
  lcd.clear();
  lcd.setCursor(0,0); lcd.print("SYSTEM STARTING");
  lcd.setCursor(7,1); lcd.print("...");
}//end LCD_init
//--------------------------------
void LCD_display() {
	switch (currentState) {
	  case MAIN_STATE: 	{LCD_menu();break;}
	  case PID_STATE: 	{PIDdisplay_editor();break;}
	  case TEMP_STATE: 	{LCD_temp();break;}
	  case FLOW_STATE: 	{LCD_flow();break;}
    case SET_TEMP:    {TempEditor(); break;}
    case SET_FLOW:    {FlowEditor(); break;}
    case SHOW_STATE:  {LCD_showState(); break;}
	  case ERROR: 		  {break;}
	}//end switch
}//end LCD_display
//--------------------------------
int buttonRead()
{
	int adc_buttons=analogRead(BUTTON_PIN);
	delay(150); //debounce
	if(adc_buttons<LEFT_MAX) //button 1 --have to change this in every power supply if buttons do not work
	{
		return BUT_LEFT;
	}
	if(adc_buttons>DOWN_MIN & adc_buttons<DOWN_MAX) //button 2 --have to change this in every power supply if buttons do not work
	{
		return BUT_DOWN;
	}
	if(adc_buttons>CONFIRM_MIN& adc_buttons<CONFIRM_MAX) //button 3 --have to change this in every power supply if buttons do not work
  {
		return BUT_CONFIRM;
	}
	if(adc_buttons>RIGHT_MIN& adc_buttons<RIGHT_MAX) //button 4 --have to change this in every power supply if buttons do not work
  { 
		return BUT_RIGHT;
  }
	if (adc_buttons >=UP_MIN & adc_buttons<UP_MAX)  //button 5 --have to change this in every power supply if buttons do not work
	{
		return BUT_UP;
  }
	return ERROR; //error
}//end buttonRead
//--------------------------------
void LCD_showState()
{
  String state_LCD;
  switch (programState)
  {
    case STATE_INIT:
      state_LCD = "INIT      ";
      break;
    case STATE_READY_1:
      state_LCD = "READY 01  ";
      break;
    case STATE_READY_2:
      state_LCD = "READY 02  ";
      break;
    case STATE_RUN_1:
      state_LCD = "RUN 01    ";
      break;    
    case STATE_RUN_2:
      state_LCD = "RUN 02    ";
      break;    
    case STATE_STARTUP:
      state_LCD = "STARTUP   ";
      break;  
    case STATE_STARTUP_PUMP2:
      state_LCD = "SU PUMP 02";
      break;  
    case STATE_TRANSITION:
      state_LCD = "TRANSITION";
      break;      
    case STATE_ERROR:
      state_LCD = "ERROR     ";
      break;      
  }

lcd.setCursor(1,0); lcd.print(state_LCD);

switch (buttonRead()) 
{
case BUT_UP: 
case BUT_DOWN: 
case BUT_RIGHT:
case BUT_LEFT:
  break;
case BUT_CONFIRM: 
{
  currentState=MAIN_STATE;LCDpointer=1;lcd.clear();return;
}
case ERROR:     {break;}
}//end switch
return;
}

void LCD_menu()
{
 //------------------------------Display user interface------------------
	lcd.setCursor(1,0); lcd.print("PID ");
//  lcd.setCursor(1,1); lcd.print(analogRead(BUTTON_PIN));lcd.print("    ");  //<-- this line used for determine the ADC value when power suppy changes
//Serial.println(analogRead(BUTTON_PIN));
  lcd.setCursor(1,1); lcd.print("STAT");
	lcd.setCursor(6,0); lcd.print("TEMP");
	lcd.setCursor(6,1); lcd.print("FLOW");
  lcd.setCursor(11,0);lcd.print("sTEMP");
  lcd.setCursor(11,1);lcd.print("sFLOW");
  
	if (LCDpointer==1) {
		lcd.setCursor(0,0);   lcd.print(">");
    lcd.setCursor(0,1);   lcd.print(" ");
		lcd.setCursor(5,0);   lcd.print(" ");
    lcd.setCursor(5,1);   lcd.print(" ");
		lcd.setCursor(10,0);  lcd.print(" ");
    lcd.setCursor(10,1);  lcd.print(" ");
  } else if(LCDpointer==2) {
    lcd.setCursor(0,0);   lcd.print(" ");
    lcd.setCursor(0,1);   lcd.print(">");
    lcd.setCursor(5,0);   lcd.print(" ");
    lcd.setCursor(5,1);   lcd.print(" ");
    lcd.setCursor(10,0);  lcd.print(" ");
    lcd.setCursor(10,1);  lcd.print(" ");
	} else if(LCDpointer==3) {
    lcd.setCursor(0,0);   lcd.print(" ");
    lcd.setCursor(0,1);   lcd.print(" ");
    lcd.setCursor(5,0);   lcd.print(">");
    lcd.setCursor(5,1);   lcd.print(" ");
    lcd.setCursor(10,0);  lcd.print(" ");
    lcd.setCursor(10,1);  lcd.print(" ");
	} else if(LCDpointer==4) {
    lcd.setCursor(0,0);   lcd.print(" ");
    lcd.setCursor(0,1);   lcd.print(" ");
    lcd.setCursor(5,0);   lcd.print(" ");
    lcd.setCursor(5,1);   lcd.print(">");
    lcd.setCursor(10,0);  lcd.print(" ");
    lcd.setCursor(10,1);  lcd.print(" ");
	} else if(LCDpointer==5)  {
    lcd.setCursor(0,0);   lcd.print(" ");
    lcd.setCursor(0,1);   lcd.print(" ");
    lcd.setCursor(5,0);   lcd.print(" ");
    lcd.setCursor(5,1);   lcd.print(" ");
    lcd.setCursor(10,0);  lcd.print(">");
    lcd.setCursor(10,1);  lcd.print(" ");
  } else if(LCDpointer==6)  {
    lcd.setCursor(0,0);   lcd.print(" ");
    lcd.setCursor(0,1);   lcd.print(" ");
    lcd.setCursor(5,0);   lcd.print(" ");
    lcd.setCursor(5,1);   lcd.print(" ");
    lcd.setCursor(10,0);  lcd.print(" ");
    lcd.setCursor(10,1);  lcd.print(">");
  }//end if else
 //----------------------------------------------------------------------
 
	switch (buttonRead()) {
	  case BUT_UP: 
	  {
      if(LCDpointer<2)  LCDpointer=6;
      else              LCDpointer--;
		  break;
	  }
	  case BUT_DOWN: 
	  {
      if(LCDpointer>5)  LCDpointer=1;
      else              LCDpointer++;
		  break;
	  }
	  case BUT_RIGHT: {break;}
	  case BUT_LEFT:  {break;}
	  case BUT_CONFIRM: 
	  {
		  switch (LCDpointer) {
		    case 1: {currentState=PID_STATE;LCDpointer=1;lcd.clear();return;}
        case 2:	{currentState=SHOW_STATE;LCDpointer=1;lcd.clear();return;}
		    case 3:	{currentState=TEMP_STATE;LCDpointer=1;lcd.clear();return;}
		    case 4:	{currentState=FLOW_STATE;LCDpointer=1;lcd.clear();return;}
        case 5: {currentState=SET_TEMP; LCDpointer=1;lcd.clear();return;}
        case 6: {currentState=SET_FLOW; LCDpointer=1;lcd.clear();return;}
		  }//end switch case
	  }
	  case ERROR:     {break;}
	}//end switch
	return;
}//end LCD_menu
//--------------------------------
void PIDdisplay_editor()
{
	//------------------------------Display user interface------------------
  lcd.setCursor(1,0);lcd.print("Kp:");
  lcd.setCursor(4,0);lcd.print(PID[0]);
  lcd.setCursor(9,0);lcd.print("Ki:");
  lcd.setCursor(12,0);lcd.print(PID[1]);
  lcd.setCursor(1,1);lcd.print("Kd:");
  lcd.setCursor(4,1);lcd.print(PID[2]); 
	
	if  (LCDpointer==1) {
		lcd.setCursor(0,0); lcd.print(">");
		lcd.setCursor(8,0); lcd.print(" ");
		lcd.setCursor(0,1); lcd.print(" ");
	} else if(LCDpointer==2)  {
		lcd.setCursor(0,0); lcd.print(" ");
		lcd.setCursor(8,0); lcd.print(">");
		lcd.setCursor(0,1); lcd.print(" ");
	} else if(LCDpointer==3)  {
		lcd.setCursor(0,0); lcd.print(" ");
		lcd.setCursor(8,0); lcd.print(" ");
		lcd.setCursor(0,1); lcd.print(">");
	}//end if
  //----------------------------------------------------------------------

	switch (buttonRead()) {
	  case BUT_UP: 
    {
      if(LCDpointer<2)  LCDpointer=3;
      else              LCDpointer--;
      break;
    }
    case BUT_DOWN: 
	  {
		  if(LCDpointer>2)   LCDpointer=1;
      else              LCDpointer++;
      break;
	  }
	  case BUT_RIGHT: 
	  {
		  PID[LCDpointer-1]+=0.1;
      break;
	  }
	  case BUT_LEFT: 
	  {
		  PID[LCDpointer-1]-=(PID[LCDpointer-1]<=0)?(0):(0.1);
      break;
	  }
	  case BUT_CONFIRM: 
	  {
      PIDsendToMaster(PID); //send new PID vals to master
		  currentState=MAIN_STATE;
      LCDpointer=1;lcd.clear();
		  return;//back
	  }
	  case ERROR:
	  {
		  break;
	  }
	}//end switch
	return;
}//PIDdisplay_editor
//----------------------------
void TempEditor()
{
  //------------------------------Display user interface------------------
  lcd.setCursor(1,0);  lcd.print("T1:");
  lcd.setCursor(4,0);  lcd.print(31);//sTemp[0]
  lcd.setCursor(9,0);  lcd.print("T3:");
  lcd.setCursor(12,0); lcd.print(35);//sTemp[2]
  lcd.setCursor(1,1);  lcd.print("T2:");
  lcd.setCursor(4,1);  lcd.print(33);//sTemp[1]
  lcd.setCursor(9,1);  lcd.print("T4:");
  lcd.setCursor(12,1); lcd.print(30);//
  if (sTemp[0]<10) {lcd.setCursor(5,0); lcd.print(" ");} 
  if (sTemp[2]<10) {lcd.setCursor(13,0); lcd.print(" ");}
  if (sTemp[1]<10) {lcd.setCursor(5,1); lcd.print(" ");} 
  if (sTemp[3]<10) {lcd.setCursor(13,1); lcd.print(" ");}
  if (sTemp[0]<100) {lcd.setCursor(6,0); lcd.print(" ");}
  if (sTemp[2]<100) {lcd.setCursor(14,0); lcd.print(" ");}
  if (sTemp[1]<100) {lcd.setCursor(6,1); lcd.print(" ");}
  if (sTemp[3]<100) {lcd.setCursor(14,1); lcd.print(" ");} 
  if (LCDpointer==1)  {
    lcd.setCursor(0,0); lcd.print(">");
    lcd.setCursor(8,0); lcd.print(" ");
    lcd.setCursor(0,1); lcd.print(" ");
    lcd.setCursor(8,1); lcd.print(" ");
  } else if(LCDpointer==2) {
    lcd.setCursor(0,0); lcd.print(" ");
    lcd.setCursor(8,0); lcd.print(" ");
    lcd.setCursor(0,1); lcd.print(">");
    lcd.setCursor(8,1); lcd.print(" ");
  } else if(LCDpointer==3) {
    lcd.setCursor(0,0); lcd.print(" ");
    lcd.setCursor(8,0); lcd.print(">");
    lcd.setCursor(0,1); lcd.print(" ");
    lcd.setCursor(8,1); lcd.print(" ");
  } else if(LCDpointer==4) {
    lcd.setCursor(0,0); lcd.print(" ");
    lcd.setCursor(8,0); lcd.print(" ");
    lcd.setCursor(0,1); lcd.print(" ");
    lcd.setCursor(8,1); lcd.print(">");
  }//end if else
  if ( LCDpointer>4) LCDpointer=1; 
  //----------------------------------------------------------------------
  
 switch (buttonRead()){
    case BUT_UP:  
    {
      if (LCDpointer<2) LCDpointer=4;
      else              LCDpointer--;
      break;
    }
    case BUT_DOWN:  
    {
      if(LCDpointer>3)  LCDpointer=1;
      else              LCDpointer++;
      break;
    }
    case BUT_RIGHT: 
    {
      sTemp[LCDpointer-1]+=(sTemp[LCDpointer-1]>=500)?(0):(1);
      break;
    }
    case BUT_LEFT:  
    {
      sTemp[LCDpointer-1]-=(sTemp[LCDpointer-1]<=0)?(0):(1);
      break;
    }
    case BUT_CONFIRM: 
    { 
      sTempSendToMaster(sTemp); //send the set temperature to master
      currentState=MAIN_STATE;
      LCDpointer=1;lcd.clear();
      return;
    }
    case ERROR: 
    {
      break;
    }
  }//end switch
  return;
}//end TempEditor
//----------------------------  
void FlowEditor()
{
  lcd.setCursor(1,0); lcd.print("FlowPulse1:");
  lcd.setCursor(12,0); lcd.print(sFlow[0]);
  lcd.setCursor(1,1); lcd.print("FlowPulse2:");
  lcd.setCursor(12,1); lcd.print(sFlow[1]);
  if (sFlow[0]<10)   {lcd.setCursor(13,0); lcd.print(" ");}
  if (sFlow[1]<10)   {lcd.setCursor(13,1); lcd.print(" ");}
  if (sFlow[0]<100)  {lcd.setCursor(14,0); lcd.print(" ");}
  if (sFlow[1]<100)  {lcd.setCursor(14,1); lcd.print(" ");}  
  if(LCDpointer==1) {
   lcd.setCursor(0,0); lcd.print(">");
   lcd.setCursor(0,1); lcd.print(" ");
  } else if( LCDpointer==2) {
   lcd.setCursor(0,0); lcd.print(" ");
   lcd.setCursor(0,1); lcd.print(">");
  }//end if else
  if (LCDpointer>2) LCDpointer=1;
  
  switch ( buttonRead())
  {
    case BUT_UP:  
    {
      if(LCDpointer>1)    LCDpointer=1;
      else                LCDpointer++;
      break;
    }  
    case BUT_DOWN:  
    {
      if(LCDpointer<2)    LCDpointer=2;
      else                LCDpointer--;
      break;
    }
    case BUT_RIGHT: 
    {
      sFlow[LCDpointer-1]+=(sFlow[LCDpointer-1]>=500)?(0):(1);
      break;
    } 
    case BUT_LEFT:  
    {
      sFlow[LCDpointer-1]-=(sFlow[LCDpointer-1]<=0)?(0):(1);
      break;
    }
    case BUT_CONFIRM: 
    {
      sFlowSendToMaster(sFlow); //send the set flow to master
      currentState= MAIN_STATE;
      LCDpointer=1;lcd.clear();
      return;
    }
    case ERROR: 
    {
      break;
    }
  }// end switch  
  return;
}// end FlowEditor
//----------------------------
void LCD_flow()
{ 
  //------------------------------Display user interface------------------
  lcd.setCursor(0,0); lcd.print("FlowPulse1:");
  lcd.setCursor(11,0); lcd.print(flow[0]);
  lcd.setCursor(14,0); lcd.print("L");
  lcd.setCursor(0,1); lcd.print("FlowPulse2:");
  lcd.setCursor(11,1); lcd.print(flow[1]);
  lcd.setCursor(14,1); lcd.print("L");
  if(flow[0]<10)  {lcd.setCursor(12,0); lcd.print(" ");}
  if(flow[1]<10)  {lcd.setCursor(12,1); lcd.print(" ");}
  if(flow[0]<100) {lcd.setCursor(13,0); lcd.print(" ");}
  if(flow[1]<100) {lcd.setCursor(13,1); lcd.print(" ");}
  //----------------------------------------------------------------------
 
  switch (buttonRead()) {
    case BUT_LEFT:  {break;}
    case BUT_RIGHT: {break;}
    case BUT_UP:    {break;}
    case BUT_DOWN:  {break;}
    case BUT_CONFIRM: 
    {
      currentState=MAIN_STATE;
      LCDpointer=1;lcd.clear();
      return;//back
    }
    case ERROR:     {break;}
  }//end switch
  return;
}// end LCD_flow()
//----------------------------
void LCD_temp()
{
  //------------------------------Display user interface------------------
  lcd.setCursor(0,0); lcd.print("T1:");
  lcd.setCursor(3,0); lcd.print(31);//Temp[0]
  lcd.setCursor(9,0); lcd.print("T3:");
  lcd.setCursor(12,0); lcd.print(33);//Temp[2]
  lcd.setCursor(0,1); lcd.print("T2:");
  lcd.setCursor(3,1); lcd.print(35);//Temp[1]
  lcd.setCursor(9,1); lcd.print("T4:");
  lcd.setCursor(12,1); lcd.print(30);//
  
  if (Temp[0]<10) {lcd.setCursor(4,0); lcd.print("    ");} 
  if (Temp[2]<10) {lcd.setCursor(13,0); lcd.print("    ");}
  if (Temp[1]<10) {lcd.setCursor(4,1); lcd.print("    ");} 
  if (Temp[3]<10) {lcd.setCursor(13,1); lcd.print("    ");}
  if (Temp[0]<100) {lcd.setCursor(5,0); lcd.print(" ");}
  if (Temp[2]<100) {lcd.setCursor(14,0); lcd.print(" ");}
  if (Temp[1]<100) {lcd.setCursor(5,1); lcd.print(" ");}
  if (Temp[3]<100) {lcd.setCursor(14,1); lcd.print(" ");} 
  //----------------------------------------------------------------------
 
  switch (buttonRead()) {
    case BUT_LEFT:  {break;}
    case BUT_RIGHT: {break;}
    case BUT_UP:    {break;}
    case BUT_DOWN:  {break;}
    case BUT_CONFIRM: 
    {
      currentState=MAIN_STATE;
      LCDpointer=1;lcd.clear();
      return;//back
    }
    case ERROR:     {break;}
  }//end switch
  return;
}// end LCD_temp
//----------------------------
void changeVal(uint16_t mt1,uint16_t mt2,uint16_t mt3,uint16_t mt4, float mf1, float mf2) //Change Value temp and flow to display on the LCD
{
  Temp[0]=mt1;
  Temp[1]=mt2;
  Temp[2]=mt3;
  Temp[3]=mt4;
  flow[0]=mf1;
  flow[1]=mf2;
}// end changeVal
//----------------------------
void changeSetVal(float* vPID,uint16_t* vTemp,float* vFlow) {
  *PID = *vPID;         //Kp
  *(PID+1) = *(vPID+1); //Ki
  *(PID+2) = *(vPID+2); //Kd
  *sTemp = *vTemp;          //T1
  *(sTemp+1) = *(vTemp+1);  //T2
  *(sTemp+2) = *(vTemp+2);  //T3
  *(sTemp+3) = *(vTemp+3);  //T4
  *sFlow = *vFlow;          //F1
  *(sFlow+1) = *(vFlow+1);  //F2
}//end changeSetVal
//----------------------------
void changeSetVal(float* vPID) {
  *PID = *vPID;         //Kp
  *(PID+1) = *(vPID+1); //Ki
  *(PID+2) = *(vPID+2); //Kd
}//end changeSetVal
//----------------------------

void LCD_changeState(uint16_t state)
{
  programState = state;
}
#endif //__NANO_LCD_CPP
