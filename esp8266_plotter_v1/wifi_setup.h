#include <ESP8266WiFi.h>
#include <WiFiClient.h>

WiFiServer server(80);
WiFiClient client;
bool wait4req = false;
String req;

void start(String ssid, String pass){
  //for static IP
  //IPAddress local_IP(192,168,0,5);
  //IPAddress gateway(192,168,0,1);
  //IPAddress subnet(255,255,255,0);

  //conect to Access Point
  Serial.println("");
  Serial.println("Setting up in station mode:");
  Serial.print("connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid.c_str(),pass.c_str());
  //WiFi.config(local_IP, gateway, subnet); //for static IP
  //----------
  //creating Access Point
  /*Serial.println("setting up Access Point:");
  Serial.println(WiFi.softAPConfig(local_IP, gateway, subnet));
  Serial.println(WiFi.softAP("ESP8266_01","acess_point_password"));
  Serial.println(WiFi.softAPIP());*/
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("success");
  Serial.println("");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.print("MAC address: ");
  Serial.println(WiFi.macAddress());
  //start TCP server
  server.begin();
  Serial.println("TCP server started");
}

void check4req(){
  client = server.available();
  if (client) {
    Serial.println("");
    Serial.println("NEW REQUEST:");
    //wait for client to send data
    while (client.connected() && !client.available()) {
      delay(1);
    }
    //got request, process in main
    wait4req = false;
  }
  return;
}

void send2client(String data){
  String s;
  s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
  s += data;
  client.print(s);
  client.flush();
  Serial.print("returned ");
  Serial.print(data);
  Serial.println(" to client");
  return;
}
