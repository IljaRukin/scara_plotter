#include "wifi_setup.h"

#define WIFI_SSID "your_ssid"
#define WIFI_PW "your_pw"
#define LED 2

void do_something(String data){
  //turn LED on/off
  if (data=="LED_ON"){
    digitalWrite(LED,LOW);
  }
  if (data=="LED_OFF"){
    digitalWrite(LED,HIGH);
  }
  return;
}

void setup(){
  Serial.begin(9600);
  start(WIFI_SSID,WIFI_PW);
  pinMode(LED,OUTPUT);
}

void loop(){
  //get request
  wait4req = true;
  do{check4req();} while(wait4req == true);

  send2client("ack");

  //process request
  String data;
  bool process = false;
  while(client.available()>0){
    if (process){
      data = client.readStringUntil('&');
      int addr_start = data.indexOf('=');
      data = data.substring(addr_start + 1);
      Serial.println(data);
      do_something(data);
    }
    else{
      data = client.readStringUntil('\r');
      //Serial.println(data);
      if (data.indexOf("Connection: close")>0){
      //if (data == "\nConnection: close"){
        Serial.println("data: ");
        process = true;
      }
    }
  }
}
