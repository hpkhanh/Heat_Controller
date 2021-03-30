/*-------------------------------------------
  Wifi MQTT - function file
  ESP32 DEVKIT V1
  (c) Can Tho University - 2019 
  version 1.00 - 24/08/2019
-------------------------------------------- 
--------------------------------------------------------------
 * ESP-IDF version: 3.2
 * Compiler version: 5.2.0
 * Arduino components version: latest
--------------------------------------------------------------*/
#ifndef  __ESP32_MQTT_CPP 
#define  __ESP32_MQTT_CPP
#include "ESP32_MQTT.h"
#include <WiFiClientSecure.h>
#include "AutoConnect.h"
#include "ESP32_AutoConnect_Aux.h"
#include "ESP32_NVS.h"

// ------ Private constants -----------------------------------

// ------ Private function prototypes -------------------------
/**
Begin the Wifi 
**/
bool Wifi_begin();
/**
Connect to the MQTT broker 
**/
bool MQTT_connect();

/**
Callback function when the captive portal starts
**/
bool MQTT_portalStartCallback(IPAddress ip);

/**
Initialize the MQTT ID, WiFi SoftAP SSID and certificate
**/
void MQTT_IdInit();

/**
Publish an online message to the server
**/
void MQTT_appearOnline();

void publishStringIdPrefix(Adafruit_MQTT_Publish topicPub, char* MQTTmessage, const char* failed, const char* success);
void publishIntIdPrefix(Adafruit_MQTT_Publish topicPub, int MQTTmessage, const char* failed, const char* success);
void publishFloatIdPrefix(Adafruit_MQTT_Publish topicPub, float MQTTmessage, const char* failed, const char* success);

String saveMqttClientCallback(AutoConnectAux& aux, PageArgument& args);

// ------ Private variables -----------------------------------
AutoConnect portal;
WiFiClientSecure wifi_client; // Create an ESP32/ESP8266 WiFiClientSecure class to connect to the MQTT server.
const char* test_root_ca= \
      "-----BEGIN CERTIFICATE-----\n" \
      "MIIDSjCCAjKgAwIBAgIQRK+wgNajJ7qJMDmGLvhAazANBgkqhkiG9w0BAQUFADA/\n" \
      "MSQwIgYDVQQKExtEaWdpdGFsIFNpZ25hdHVyZSBUcnVzdCBDby4xFzAVBgNVBAMT\n" \
      "DkRTVCBSb290IENBIFgzMB4XDTAwMDkzMDIxMTIxOVoXDTIxMDkzMDE0MDExNVow\n" \
      "PzEkMCIGA1UEChMbRGlnaXRhbCBTaWduYXR1cmUgVHJ1c3QgQ28uMRcwFQYDVQQD\n" \
      "Ew5EU1QgUm9vdCBDQSBYMzCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEB\n" \
      "AN+v6ZdQCINXtMxiZfaQguzH0yxrMMpb7NnDfcdAwRgUi+DoM3ZJKuM/IUmTrE4O\n" \
      "rz5Iy2Xu/NMhD2XSKtkyj4zl93ewEnu1lcCJo6m67XMuegwGMoOifooUMM0RoOEq\n" \
      "OLl5CjH9UL2AZd+3UWODyOKIYepLYYHsUmu5ouJLGiifSKOeDNoJjj4XLh7dIN9b\n" \
      "xiqKqy69cK3FCxolkHRyxXtqqzTWMIn/5WgTe1QLyNau7Fqckh49ZLOMxt+/yUFw\n" \
      "7BZy1SbsOFU5Q9D8/RhcQPGX69Wam40dutolucbY38EVAjqr2m7xPi71XAicPNaD\n" \
      "aeQQmxkqtilX4+U9m5/wAl0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNV\n" \
      "HQ8BAf8EBAMCAQYwHQYDVR0OBBYEFMSnsaR7LHH62+FLkHX/xBVghYkQMA0GCSqG\n" \
      "SIb3DQEBBQUAA4IBAQCjGiybFwBcqR7uKGY3Or+Dxz9LwwmglSBd49lZRNI+DT69\n" \
      "ikugdB/OEIKcdBodfpga3csTS7MgROSR6cz8faXbauX+5v3gTt23ADq1cEmv8uXr\n" \
      "AvHRAosZy5Q6XkjEGB5YGV8eAlrwDPGxrancWYaLbumR9YbK+rlmM6pZW87ipxZz\n" \
      "R8srzJmwN0jP41ZL9c8PDHIyh8bwRLtTcm1D9SZImlJnt1ir/md2cXjbDaJWFBM5\n" \
      "JDGFoqgCWjBH4d1QB7wCCZAA62RjYJsWvIjJEubSfZGL+T0yjWW06XyxV3bqxbYo\n" \
      "Ob8VZRzI9neWagqNdwvYkQsEjgfbKbYK7p2CNTUQ\n" \
      "-----END CERTIFICATE-----\n";

bool subscribed = false;
char chipId[CHIP_ID_HEX_STRING_LENGTH];
char mqttClientId[MQTT_CLIENT_ID_STRING_MAX_SIZE];
char ssid[CHIP_ID_HEX_STRING_LENGTH+WIFI_PREFIX_LENGTH];

Adafruit_MQTT_Client mqtt(&wifi_client, SERVER, PORT, CLIENT_ID, USERNAME, PASS); // Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.;
//PUBLISH
//NEEDED TO BE QOS0, SO WE CAN USE OUR QOS1 FUNCTION
Adafruit_MQTT_Publish availability = Adafruit_MQTT_Publish(&mqtt,LWT_TOPIC,MQTT_QOS_0);
Adafruit_MQTT_Publish pub_kp = Adafruit_MQTT_Publish(&mqtt,KP_FEED,MQTT_QOS_0);
Adafruit_MQTT_Publish pub_ki = Adafruit_MQTT_Publish(&mqtt,KI_FEED,MQTT_QOS_0);
Adafruit_MQTT_Publish pub_kd = Adafruit_MQTT_Publish(&mqtt,KD_FEED,MQTT_QOS_0);
Adafruit_MQTT_Publish temp01 = Adafruit_MQTT_Publish(&mqtt,TEMP1_FEED,MQTT_QOS_0);
Adafruit_MQTT_Publish temp02 = Adafruit_MQTT_Publish(&mqtt,TEMP2_FEED,MQTT_QOS_0);
Adafruit_MQTT_Publish temp03 = Adafruit_MQTT_Publish(&mqtt,TEMP3_FEED,MQTT_QOS_0);
Adafruit_MQTT_Publish temp04 = Adafruit_MQTT_Publish(&mqtt,TEMP4_FEED,MQTT_QOS_0);
Adafruit_MQTT_Publish pub_pump1pwm = Adafruit_MQTT_Publish(&mqtt,PUMP1PWM_FEED,MQTT_QOS_0);
Adafruit_MQTT_Publish pub_pump2pwm = Adafruit_MQTT_Publish(&mqtt,PUMP2PWM_FEED,MQTT_QOS_0);
Adafruit_MQTT_Publish pub_relay01 = Adafruit_MQTT_Publish(&mqtt,RELAY01_FEED,MQTT_QOS_0);
Adafruit_MQTT_Publish pub_relay02 = Adafruit_MQTT_Publish(&mqtt,RELAY02_FEED,MQTT_QOS_0);
Adafruit_MQTT_Publish pub_relay03 = Adafruit_MQTT_Publish(&mqtt,RELAY03_FEED,MQTT_QOS_0);
Adafruit_MQTT_Publish pub_flowsen01 = Adafruit_MQTT_Publish(&mqtt,FLOWSENS_01_FEED,MQTT_QOS_0);
//SUBCRIBE
Adafruit_MQTT_Subscribe sub_kp = Adafruit_MQTT_Subscribe(&mqtt,KP_FEED,MQTT_QOS_1);
Adafruit_MQTT_Subscribe sub_ki = Adafruit_MQTT_Subscribe(&mqtt,KI_FEED,MQTT_QOS_1);
Adafruit_MQTT_Subscribe sub_kd = Adafruit_MQTT_Subscribe(&mqtt,KD_FEED,MQTT_QOS_1);
Adafruit_MQTT_Subscribe sub_pump1pwm = Adafruit_MQTT_Subscribe(&mqtt,PUMP1PWM_FEED,MQTT_QOS_1);
Adafruit_MQTT_Subscribe sub_pump2pwm = Adafruit_MQTT_Subscribe(&mqtt,PUMP2PWM_FEED,MQTT_QOS_1);
Adafruit_MQTT_Subscribe sub_relay01 = Adafruit_MQTT_Subscribe(&mqtt,RELAY01_FEED,MQTT_QOS_1);
Adafruit_MQTT_Subscribe sub_relay02 = Adafruit_MQTT_Subscribe(&mqtt,RELAY02_FEED,MQTT_QOS_1);
Adafruit_MQTT_Subscribe sub_relay03 = Adafruit_MQTT_Subscribe(&mqtt,RELAY03_FEED,MQTT_QOS_1);

uint32_t lastmillis = 0;
uint32_t lastShownOnline = 0;
// ------ PUBLIC variable definitions -------------------------

//--------------------------------------------------------------
// FUNCTION DEFINITIONS
//--------------------------------------------------------------
void MQTT_init() {
  if (Wifi_begin()) {
    MQTT_subscribeInit();
  }//end if
}//end MQTT_init

void MQTT_subscribeInit()
{
  if ((WiFi.status() == WL_CONNECTED) && (false == subscribed))
  {
    bool succeeded = true;
    char offlineMess[24];
    strncpy(offlineMess, chipId, CHIP_ID_HEX_STRING_LENGTH);
    strncat(offlineMess, (char*)" - Offline", CHIP_ID_HEX_STRING_LENGTH);
    succeeded &= mqtt.will (LWT_TOPIC, (char*)offlineMess, MQTT_QOS_1, LWT_RETAIN); //Set up last will
    succeeded &=mqtt.subscribe(&sub_kp); // Set up MQTT subscriptions.
    succeeded &=mqtt.subscribe(&sub_ki); // Set up MQTT subscriptions.
    succeeded &=mqtt.subscribe(&sub_kd); // Set up MQTT subscriptions.
    succeeded &=mqtt.subscribe(&sub_relay01); // Set up MQTT subscriptions.
    succeeded &=mqtt.subscribe(&sub_relay02); // Set up MQTT subscriptions.
    succeeded &=mqtt.subscribe(&sub_relay03); // Set up MQTT subscriptions.

    if (true == succeeded)
    {
      subscribed = true;
      if (MQTT_connect()) {
        MQTT_appearOnline();
        lastmillis = millis();
      }//end if
    }
  }
}

void MQTT_appearOnline()
{
  char onlineMess[24];
  strncpy(onlineMess, chipId, CHIP_ID_HEX_STRING_LENGTH);
  strncat(onlineMess, (char*)" - Online", CHIP_ID_HEX_STRING_LENGTH);
  lastShownOnline = millis();
  publishStringNow(availability, (char*)onlineMess, RETAIN,"Status Failed!","Status updated!");
  Serial.println(onlineMess);
}

//------------------------------------------
void MQTT_maintain() {//keep the MQTT connection
    if (MQTT_connect()) { //LED indicator integrated
      // Adjust PING time in seconds in Adafruit_MQTT.h:  
      //#define MQTT_CONN_KEEPALIVE 60 --> Default to 1 minutes.
      if (((millis()-lastmillis)>PING_WAIT)) {//wait for PING_WAIT msecond before ping the server to keep the mqtt connection alive
        char pingCount=0;
        while (! mqtt.ping()) {if(pingCount++>PING_TIMES) mqtt.disconnect();return;}// if ping PING_TIMES times and failed then disconnect
        lastmillis = millis(); //ping sucess
      }//end if
      
      uint32_t now = millis();
      uint32_t time_difference = 0;
      if (now >= lastShownOnline)
      {
        time_difference = now - lastShownOnline;
      }
      else 
      {
        time_difference = 0xFFFFFFFF - lastShownOnline + now + 1;
      }
      if (time_difference > CONFIRM_PRESENT_WAIT)
      {
        MQTT_appearOnline();
      }
    }//end if
}//end MQTT_maintain
//------------------------------------------
void MQTT_subscribe() {
    if (MQTT_connect()) {
      //SUBCRIBE:
      // this is our 'wait for incoming subscription packets' busy subloop. Max is 15 subs at a time, change at Adafruit_MQTT.h, #define MAXSUBSCRIPTIONS 15
      Adafruit_MQTT_Subscribe *subscription;                  
      while ((subscription = mqtt.readSubscription(5000))) {      //wait for signal
        if (subscription == &sub_kp) {                         // if something new is detected on this topic
          String sdata = (char*)sub_kp.lastread;   // Function to analize the string
          float sfloat = sdata.toFloat();
          if (sfloat!=NVS_read_Kp()) {
            PID_Kp_write(sfloat);
            UART_PIDsendToSlave();
          }//end if
          break;
        }//end if
        if (subscription == &sub_ki)  {                           // if something new is detected on this topic
          String sdata = (char*)sub_ki.lastread;   // Function to analize the string
          float sfloat = sdata.toFloat();
          if (sfloat!=NVS_read_Ki()) {
            PID_Ki_write(sfloat);
            UART_PIDsendToSlave();
          }//end if
          break;
        }//end if  
        if (subscription == &sub_kd)  {                           // if something new is detected on this topic
          String sdata = (char*)sub_kd.lastread;   // Function to analize the string
          float sfloat = sdata.toFloat();
          if (sfloat!=NVS_read_Kd()) {
            PID_Kd_write(sfloat);
            UART_PIDsendToSlave();
          }//end if
          break;
        }//end if  
        if (subscription == &sub_relay01)  {                           // if something new is detected on this topic
          String sdata = (char*)sub_relay01.lastread;   // Function to analize the string
          if (sdata=="ON") {
            relay01(ON);
            break;
          } else if (sdata=="OFF") {
            relay01(OFF);
            break;
          }//end if else
        }//end if
        if (subscription == &sub_relay02)  {                           // if something new is detected on this topic
          String sdata = (char*)sub_relay02.lastread;   // Function to analize the string
          if (sdata=="ON") {
            relay02(ON);
            break;
          } else if (sdata=="OFF") {
            relay02(OFF);
            break;
          }//end if else
        }//end if
        if (subscription == &sub_relay03)  {                           // if something new is detected on this topic
          String sdata = (char*)sub_relay03.lastread;   // Function to analize the string
          if (sdata=="ON") {
            relay03(ON);
            break;
          } else if (sdata=="OFF") {
            relay03(OFF);
            break;
          }//end if else
        }//end if
      }//end while
    }//end if
}//end MQTT_subscribe
//------------------------------------------
void publishNow(Adafruit_MQTT_Publish topicPub, const char* MQTTmessage, bool retained, const char* failed, const char* success) {//--------SELF CREATED QOS1 --make sure the packet made it to the broker
  char pub=1;
  while (!topicPub.publish(MQTTmessage)) 
  {
    Serial.println(F(failed));
    delay(PUB_WAIT);
    if (pub++>(PUB_RETRIES-1)) 
      break;
  } 
  if (pub<(PUB_RETRIES)) {Serial.println(F(success));}
}//end publishNow

void publishStringNow(Adafruit_MQTT_Publish topicPub, char* MQTTmessage, bool retained, const char* failed, const char* success) {//--------SELF CREATED QOS1 --make sure the packet made it to the broker
  char pub=1;
  while (!topicPub.publish((const char *)MQTTmessage)) 
  {
    Serial.println(F(failed));
    delay(PUB_WAIT);
    if (pub++>(PUB_RETRIES-1)) 
      break;
  } 
  if (pub<(PUB_RETRIES)) {Serial.println(F(success));}
}//end publishNow

void publishStringIdPrefix(Adafruit_MQTT_Publish topicPub, char* MQTTmessage, const char* failed, const char* success)
{
  char mess[30];
  strncpy(mess, chipId, CHIP_ID_HEX_STRING_LENGTH);
  strncat(mess, " - ", 3);
  strncat(mess, MQTTmessage, 12);
  publishStringNow(topicPub, mess , RETAIN, failed, success);
}

void publishIntIdPrefix(Adafruit_MQTT_Publish topicPub, int MQTTmessage, const char* failed, const char* success)
{
  char mess[30];
  char value[11];
  itoa(MQTTmessage, value, 10);
  strncpy(mess, chipId, CHIP_ID_HEX_STRING_LENGTH);
  strncat(mess, " - ", 3);
  strncat(mess, value, 11);
  publishStringNow(topicPub, mess , RETAIN, failed, success);
}

//------------------------------------------
void publishNow(Adafruit_MQTT_Publish topicPub,int MQTTmessage, bool retained, const char* failed, const char* success) {//--------SELF CREATED QOS1 --make sure the packet made it to the broker
  char pub=1;
  while (!topicPub.publish(MQTTmessage)) 
  {
    Serial.println(F(failed));
    delay(PUB_WAIT);
    if (pub++>(PUB_RETRIES-1))
      break;
  } 
  if (pub<(PUB_RETRIES)) {Serial.println(F(success));}
}//end publishNow

//------------------------------------------
void publishNow(Adafruit_MQTT_Publish topicPub,float MQTTmessage, bool retained, const char* failed, const char* success) {//--------SELF CREATED QOS1 --make sure the packet made it to the broker
  char pub=1;
  while (!topicPub.publish(MQTTmessage, 2)) 
  {
    Serial.println(F(failed));
    delay(PUB_WAIT);
    if (pub++>(PUB_RETRIES-1))
      break;
  } 
  if (pub<(PUB_RETRIES)) {Serial.println(F(success));}
}//end publishNow

void publishFloatIdPrefix(Adafruit_MQTT_Publish topicPub, float MQTTmessage, const char* failed, const char* success)
{
  char mess[30];
  char value[11];
  dtostrf(MQTTmessage, 10, 2, value);
  strncpy(mess, chipId, CHIP_ID_HEX_STRING_LENGTH);
  strncat(mess, " - ", 3);
  strncat(mess, value, 11);
  publishStringNow(topicPub, mess , RETAIN, failed, success);
}

//------------------------------------------
void MQTT_Kp_pub(float mqttKp) {
  publishFloatIdPrefix(pub_kp,mqttKp,"Kp Failed!","Kp updated!");
}//end MQTT_Kp_pub
//------------------------------------------
void MQTT_Ki_pub(float mqttKi) {
  publishFloatIdPrefix(pub_ki,mqttKi,"Ki Failed!","Ki updated!");
}//end MQTT_Ki_pub
//------------------------------------------
void MQTT_Kd_pub(float mqttKd) {
  publishFloatIdPrefix(pub_kd,mqttKd,"Kd Failed!","Kd updated!");
}//end MQTT_Kd_pub
//------------------------------------------
void MQTT_T1_pub(int mTemp1) {
  publishIntIdPrefix(temp01,mTemp1,"Temp01 Failed!","Temp01 updated!");
}//end MQTT_T1_pub
//------------------------------------------
void MQTT_T2_pub(int mTemp2) {
  publishIntIdPrefix(temp02,mTemp2,"Temp02 Failed!","Temp02 updated!");
}//end MQTT_T2_pub
//------------------------------------------
void MQTT_T3_pub(int mTemp3) {
  publishIntIdPrefix(temp03,mTemp3,"Temp03 Failed!","Temp03 updated!");
}//end MQTT_T3_pub
//------------------------------------------
void MQTT_T4_pub(int mTemp4) {
  publishIntIdPrefix(temp04,mTemp4,"Temp04 Failed!","Temp04 updated!");
}//end MQTT_T4_pub
//------------------------------------------
void MQTT_Pump1pwm_pub(float pwmVal) {
  publishIntIdPrefix(pub_pump1pwm,(int)(pwmVal*100),"Pump1 pwm Failed!","Pump1 pwm updated!");
}//end MQTT_Pump1pwm_pub
//------------------------------------------
void MQTT_Pump2pwm_pub(float pwmVal) {
  publishIntIdPrefix(pub_pump2pwm,(int)(pwmVal*100),"Pump2 pwm Failed!","Pump2 pwm updated!");
}//end MQTT_Pump2pwm_pub
//------------------------------------------
void MQTT_FlowSen_01_pub(float flowSenVal) {
  publishFloatIdPrefix(pub_flowsen01,flowSenVal,"Flow Sensor 01 Failed!","Flow Sensor 01 updated!");
}//end MQTT_FlowSen_01_pub
//------------------------------------------
void MQTT_relay01_pub(bool Rstate) {
  publishStringIdPrefix(pub_relay01,(char*)((Rstate==ON)?("ON"):("OFF")),"Relay01 Failed!","Relay01 updated!");
}//end MQTT_relay01_pub
//------------------------------------------
void MQTT_relay02_pub(bool Rstate) {
  publishStringIdPrefix(pub_relay02,(char*)((Rstate==ON)?("ON"):("OFF")),"Relay02 Failed!","Relay02 updated!");
}//end MQTT_relay02_pub
//------------------------------------------
void MQTT_relay03_pub(bool Rstate) {
  publishStringIdPrefix(pub_relay03,(char*)((Rstate==ON)?("ON"):("OFF")),"Relay03 Failed!","Relay03 updated!");
}//end MQTT_relay03_pub
//------------------------------------------
bool Wifi_begin() {
  MQTT_IdInit();
  Aux_updateSoftwareVersionText(String(VERSION));
  wifi_client.setCACert(test_root_ca);

  AutoConnectConfig  auto_config(ssid, "12345678");
  auto_config.autoReconnect = true;
  auto_config.reconnectInterval = 4; // Try to reconnect every 2 minutes
  auto_config.menuItems = AC_MENUITEM_CONFIGNEW|AC_MENUITEM_OPENSSIDS|AC_MENUITEM_DISCONNECT| AC_MENUITEM_RESET;
  auto_config.immediateStart = false;
  // auto_config.ticker = true;
  // auto_config.tickerPort = LED_BUILTIN;
  // auto_config.tickerOn = HIGH;

  portal.config(auto_config);
  portal.onDetect(MQTT_portalStartCallback);
  portal.join(Aux_getVersionReference());

  if (portal.begin()) 
  {
    Serial.println("Connected to " +WiFi.SSID() + " with IP: " + WiFi.localIP().toString());
    return true;
  }
  return false;
}//end wifi begin
//------------------------------------------
bool MQTT_connect() {// Ensure the connection to the MQTT server is alive (this will make the first connection and automatically reconnect when disconnected).
  if (WiFi.status() == WL_CONNECTED) { //if wifi is connected
    if (mqtt.connected()) {/*digitalWrite(LED_BUILTIN,HIGH);*/return true;} // Stop if already connected, turn on indicating LED

    //digitalWrite(LED_BUILTIN,LOW); //if not connected, indicate by a LED
    Serial.println(F("Connecting to MQTT... "));
    int8_t ret;
    int8_t waitTimes=0;
    while (((ret = mqtt.connect()) != 0)&&(waitTimes<4))  {         // connect will return 0 for connected
      waitTimes++;
      Serial.println(mqtt.connectErrorString(ret));
      Serial.println(F("Retrying MQTT connection in 5 seconds..."));
      mqtt.disconnect();
      delay(5000);  // wait 5 seconds
    }//end while
    if (waitTimes<3) {
      Serial.println(F("MQTT Connected!"));Serial.println();
      return true;
    } else {
      Serial.println(F("MQTT failed!")); Serial.println();
      return false;
    }//end if else
  }//end if
  return false;
}//end MQTT connect

void MQTT_loopHandle()
{
  portal.handleClient();
}

bool MQTT_portalStartCallback(IPAddress ip)
{
  Serial.print("Please connect to network ");
  Serial.print(ssid);  
  Serial.println(" with IP: " + WiFi.softAPIP().toString() + "/_ac");
  return true;
}

String saveMqttClientCallback(AutoConnectAux& aux, PageArgument& args)
{
  AutoConnectInput& clientIdInput = aux.getElement<AutoConnectInput>("clientId");
  String newClientId = clientIdInput.value;
  if (newClientId.length() > 0)
  {
    if (newClientId.length()+1 >= MQTT_CLIENT_ID_STRING_MAX_SIZE)
      newClientId.toCharArray(mqttClientId, MQTT_CLIENT_ID_STRING_MAX_SIZE);
    else
      newClientId.toCharArray(mqttClientId, newClientId.length()+1);
    Serial.print("New MQTT Client ID is: ");
    Serial.println(mqttClientId);
    mqtt.setClientId(mqttClientId);
    NVS_write_MqttClientId(mqttClientId);
  }

  AutoConnectText& newIdText = aux.getElement<AutoConnectText>("currentId");
  newIdText.value = String("Current MQTT Client ID is: ") + String(mqttClientId);
  return String("");
}

void MQTT_IdInit()
{
  uint64_t mac = ESP.getEfuseMac();

  /* Reverse the endianness of the MAC address */
  uint32_t mac_1 = (((mac&0xFF) << 24) | ((mac&0xFF00) << 8)
                  | ((mac&0xFF0000) >> 8) | ((mac&0xFF000000) >> 24)) & 0xFFFFFFFF;
  uint32_t mac_2 = (((mac&0xFF00000000) >> 24) | ((mac&0xFF0000000000) >> 40)) & 0xFFFF;

  /* Write MAC address value to Chip ID & WiFi SSID */
  snprintf(chipId, CHIP_ID_HEX_STRING_LENGTH, "%08X%04X", mac_1, mac_2);
  strncpy(ssid, (char*)WIFI_PREFIX, WIFI_PREFIX_LENGTH+1);
  strncat(ssid, chipId, CHIP_ID_HEX_STRING_LENGTH);

  strncpy(mqttClientId, ssid, CHIP_ID_HEX_STRING_LENGTH+WIFI_PREFIX_LENGTH);

  Serial.print("MQTT Client ID is: ");
  Serial.println(ssid);

  mqtt.setClientId(ssid);
}
//------------------------------------------
#endif //__ESP32_MQTT_CPP
