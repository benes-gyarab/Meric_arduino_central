/****************************************************************************************************************************
  Based on and modified from Gil Maimon's ArduinoWebsockets library https://github.com/gilmaimon/ArduinoWebsockets
  to support STM32F/L/H/G/WB/MP1, nRF52 and SAMD21/SAMD51 boards besides ESP8266 and ESP32
  The library provides simple and easy interface for websockets (Client and Server).
  Built by Khoi Hoang https://github.com/khoih-prog/Websockets2_Generic
  Licensed under MIT license
  Version: 1.0.7

*****************************************************************************************************************************/

//ESP8266 klient, zasílající aktualní vlhkost s ID senzoru serveru.
//Vždy zašle data a pak do LIGHT SLEEP modu dokud není probuzen

#include <WebSockets2_Generic.h>
#include <ESP8266WiFi.h>
#include <WiFiManager.h>  //https://github.com/tzapu/WiFiManager
#define FPM_SLEEP_MAX_TIME           0xFFFFFFF
#define LIGHT_WAKE_PIN D5
#define websockets_server_host "wss://young-stream-00076.herokuapp.com/sensor"

using namespace websockets2_generic;
int soil_pin = A0;

void onMessageCallback(WebsocketsMessage message) 
{
  Serial.print("Got Message: ");
  Serial.println(message.data());
}

void onEventsCallback(WebsocketsEvent event, String data) 
{
  if (event == WebsocketsEvent::ConnectionOpened) 
  {
    Serial.println("Connnection Opened");
  } 
  else if (event == WebsocketsEvent::ConnectionClosed) 
  {
    Serial.println("Connnection Closed");
  } 
  else if (event == WebsocketsEvent::GotPing) 
  {
    Serial.println("Got a Ping!");
  } 
  else if (event == WebsocketsEvent::GotPong) 
  {
    Serial.println("Got a Pong!");
  }
}

WebsocketsClient client;

void setup() 
{
  pinMode(4, OUTPUT);
  pinMode(14, INPUT);

}
void callback() {
  Serial1.println("Callback");
  Serial.flush();
}

void loop() 
{
  
  Serial.begin(115200);
  WiFiManager wifiManager;
  wifiManager.autoConnect("MoistureSensor", "vKN*5ZGMkX");
  
  Serial.print("Connected to Wifi, Connecting to WebSockets Server @");
  Serial.println(websockets_server_host);
  
  // run callback when messages are received
  client.onMessage(onMessageCallback);

  // run callback when events are occuring
  client.onEvent(onEventsCallback);

  // Connect to server
  client.connect(websockets_server_host);
  digitalWrite(4, HIGH); //zapnutí napájení senzoru
  delay(1000);
  
  int moisture = analogRead(soil_pin);

  String sensorID = "1";  // Zde by bylo možné implementovat přepínání mezi senzory a posílání dat s různými ID
  String data = sensorID + ":" +  moisture;
  
  client.send(data);
  digitalWrite(4, LOW); //zapnutí napájení senzoru

  //  https://www.mischianti.org/2019/11/21/wemos-d1-mini-esp8266-the-three-type-of-sleep-mode-to-manage-energy-savings-part-4/
  Serial.println("Enter light sleep mode");
      uint32_t sleep_time_in_ms = 900000; //15 minut
      wifi_set_opmode(NULL_MODE);
      wifi_fpm_set_sleep_type(LIGHT_SLEEP_T);
      wifi_fpm_open();
      wifi_fpm_set_wakeup_cb(callback);
      wifi_fpm_do_sleep(sleep_time_in_ms *1000 );
      delay(sleep_time_in_ms + 1);


}
