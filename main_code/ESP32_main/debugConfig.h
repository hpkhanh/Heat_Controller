/*------------------------------------------------------------*-
  Debug Configuration file
  (c) Can Tho University 2019
  version 2.00 - 09/02/2019
---------------------------------------------------------------
 * ESP-IDF version: 3.2
 * Compiler version: 5.2.0
 * Arduino components version: newest
--------------------------------------------------------------*/
#ifndef __DEBUG_CONFIG_H
#define __DEBUG_CONFIG_H



//#define SYSTEM_DEBUG
#define SERIAL_OUTPUT  1

#define FECTUM                0
#define SOLESTA               1
// #define COMPANY_NAME    FECTUM
#define COMPANY_NAME          SOLESTA

#define USE_KALMAN_FILTER     0

#define MAX31865_DEBUG        0

#define VERSION               "RC 00.06"

#ifdef SYSTEM_DEBUG // When this is active, every log will be execute.
  #ifndef SERIAL_OUTPUT
    #define SERIAL_OUTPUT
  #endif
  #ifndef WIFI_CHANGEABLE
    #define WIFI_CHANGEABLE
  #endif
  #ifndef SERIAL_BEGIN
    #define SERIAL_BEGIN()      {Serial.begin(115200);}
  #endif
  #define D_PRINT(...)       {Serial.print(__VA_ARGS__);}
  #define D_PRINTLN(...)     {Serial.println(__VA_ARGS__);}
  #define D_PRINTF(...)      {Serial.printf(__VA_ARGS__);}
#else
  #define D_PRINT(...)   {}
  #define D_PRINTLN(...) {}
  #define D_PRINTF(...)  {}
#endif

#ifdef WIFI_CHANGEABLE
  #ifndef SERIAL_BEGIN
    #define SERIAL_BEGIN()      {Serial.begin(115200);}
  #endif
#endif

#ifdef SERIAL_OUTPUT
  #ifndef SERIAL_BEGIN
    #define SERIAL_BEGIN()      {Serial.begin(115200);}
  #endif
  #define S_PRINT(...)       {Serial.print(__VA_ARGS__);}
  #define S_PRINTLN(...)     {Serial.println(__VA_ARGS__);}
  #define S_PRINTF(...)      {Serial.printf(__VA_ARGS__);}
#else
  #ifndef SERIAL_BEGIN
    #define SERIAL_BEGIN()  {}
  #endif
  #define S_PRINT(...)   {}
  #define S_PRINTLN(...) {}
  #define S_PRINTF(...)  {}
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif //_DEBUG_CONFIG_H
