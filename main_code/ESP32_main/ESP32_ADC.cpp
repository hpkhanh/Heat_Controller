/*-------------------------------------------
  ADC Converter - function file
  ESP32 DEVKIT V1
  (c) Can Tho University - 2019 
  version 1.00 - 14/08/2019
--------------------------------------------*/ 
/*
  AnalogRead Serial
  https://randomnerdtutorials.com/esp32-adc-analog-read-arduino-ide/
  ADC_0db: sets no attenuation (1V input = ADC reading of 1088).
  ADC_2_5db: sets an attenuation of 1.34 (1V input = ADC reading of 2086).
  ADC_6db: sets an attenuation of 1.5 (1V input = ADC reading of 2975).
  ADC_11db: sets an attenuation of 3.6 (1V input = ADC reading of 3959).
  https://www.esp32.com/viewtopic.php?f=19&t=2881&start=10#p13739
--------------------------------------------------------------
 * ESP-IDF version: 3.2
 * Compiler version: 5.2.0
 * Arduino components version: latest
--------------------------------------------------------------

Stable time: ~25 second.
*/
#ifndef ESP32_ADC
#define ESP32_ADC
#include "ESP32_ADC.h"
#include "Arduino.h"
#include <Adafruit_MAX31865.h>
#include <ESP32AnalogRead.h>
#include <Wire.h>
#include "debugConfig.h"

// ------ Private constants -----------------------------------
ESP32AnalogRead adc;
Adafruit_MAX31865 adc_temp_01 = Adafruit_MAX31865(TEMP_CS_PIN_01);
Adafruit_MAX31865 adc_temp_02 = Adafruit_MAX31865(TEMP_CS_PIN_02);
Adafruit_MAX31865 adc_temp_03 = Adafruit_MAX31865(TEMP_CS_PIN_03);
Adafruit_MAX31865 adc_temp_04 = Adafruit_MAX31865(TEMP_CS_PIN_04);

// ------ Private function prototypes -------------------------
/**
Read value of the ADC 
**/
int ADC_read(int,int,int);
#if (MAX31865_DEBUG)
void ADC_max31865_resistance_debug(Adafruit_MAX31865 & max_id);
#endif
uint16_t ADC_read_temp_default(Adafruit_MAX31865 & max_id);

// ------ Private variables -----------------------------------
SimpleKalmanFilter filter(E_MEA, E_EST, QUE); //For flow sensors, we don't have one so we cannot test it
SimpleKalmanFilter filter1(E_MEA, E_EST, QUE); //For T1
SimpleKalmanFilter filter2(E_MEA, E_EST, QUE); //For T2
SimpleKalmanFilter filter3(E_MEA, E_EST, QUE); //For T3
SimpleKalmanFilter filter4(E_MEA, E_EST, QUE); //For T4
// ------ PUBLIC variable definitions -------------------------

//--------------------------------------------------------------
// FUNCTION DEFINITIONS
//--------------------------------------------------------------
void ADC_init() 
{
  adc_temp_01.begin(MAX31865_3WIRE);  
  adc_temp_02.begin(MAX31865_3WIRE);
  adc_temp_03.begin(MAX31865_3WIRE);
  adc_temp_04.begin(MAX31865_3WIRE);
}//end ADC_init
//------------------------------------------
int ADC_read(int ADCpin, int lowVal, int maxVal)
{
  // read the input on the corresponding analog pin:
  //if recieved signal is less than 4mA, than the sensor is off or is having some errors
  //if recieved signal is equal or more than 4mA (~360mV ~398 ADC value), than the sensor is working probaly
  //int senVal = analogRead(ADCpin);
  //int senVal = analogRead(ADCpin);
  adc.attach(ADCpin);
  int senVal = adc.readMiliVolts();
  //------------------------------Kalman filter applied:
  int es_senVal = filter.updateEstimate(senVal); // first layer
  for (int a=1;a<FILTER_LAYER; a++) {        // next layers (if possible)
    es_senVal = filter.updateEstimate(es_senVal);   
  }//end for
  //------------------------------Kalman filter done
  if (es_senVal<200) //if sensor is off or error
    {return SENSOR_ERROR;} //use 200 for compensating for noises.
  int calculatedVal = map(es_senVal,0,4096,lowVal,maxVal); //map es_senVal from 0-2047 to lowVal-maxVal
  return calculatedVal; //return the calculated value
}//end ADC_read
//------------------------------------------

#if (MAX31865_DEBUG)
void ADC_max31865_resistance_debug(Adafruit_MAX31865 & max_id)
{
  uint16_t raw_resistance = max_id.readRTD();
  Serial.print("Raw Resistance Input:");
  Serial.println(raw_resistance);
  float resistance = raw_resistance * RREF / 32768;
  Serial.print("Calculated Resistance:");
  Serial.println(resistance);
}
#endif

uint16_t ADC_read_temp_default(Adafruit_MAX31865 & max_id)
{
  uint16_t raw_rtd = max_id.readRTD();
  float es_senVal = max_id.temperature(RNOMINAL, RREF); 
#if (USE_KALMAN_FILTER)
  //------------------------------Kalman filter applied:
  es_senVal = filter1.updateEstimate(es_senVal); // first layer
  for (int a=1;a<FILTER_LAYER; a++) {        // next layers (if possible)
    es_senVal = filter1.updateEstimate(es_senVal);   
  }//end for
  //------------------------------Kalman filter done
#endif
  uint8_t fault = max_id.readFault();
  int temp;
  if ((raw_rtd > RTD_THRESHOLD_LOW) && (raw_rtd < RTD_THRESHOLD_HIGH) && ((fault & RTD_ALL_FAULT_MASK) == 0))
  {
    temp = es_senVal;
  }
  else
  {
    temp = DEFAULT_TEMP_01;
  }
  
  return temp;
}

// T1: Collector Temperature
int tempSen01_read() {
  adc_temp_01.begin(MAX31865_3WIRE);
  return ADC_read_temp_default(adc_temp_01);
}//end tempSen01_read
//------------------------------------------
//T2: Buffer below temperature
int tempSen02_read() {
  adc_temp_02.begin(MAX31865_3WIRE);
  return ADC_read_temp_default(adc_temp_02);
}//end tempSen02_read
//------------------------------------------
//T3: Buffer top temperature
int tempSen03_read() {
  adc_temp_03.begin(MAX31865_3WIRE);
  return ADC_read_temp_default(adc_temp_03);
}//end tempSen03_read
//------------------------------------------
//T4: Temperature of the cooled down water from the radiator (warming up the house). 
int tempSen04_read() {
  adc_temp_04.begin(MAX31865_3WIRE);
  return ADC_read_temp_default(adc_temp_04);
}//end tempSen04_read
//------------------------------------------
#endif //__ESP32_ADC_CPP
