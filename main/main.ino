#include <WiFiS3.h>
#include <WiFiSSLClient.h>
#include <R4HttpClient.h>
#define _UNNEEDED_RESPONSE_

#include "main.h"

void sendPostRequest();
WiFiSSLClient client;
R4HttpClient http;

#define SEND_STRING "<@" YOUR_DISCORD_USERID "> ドアの呼び出しが鳴りました！"
#define PAYLOAD "{\"content\":\"" SEND_STRING "\"}"
#define DISCORD_WEBHOOKPORT 443


void setup() {
  Serial.begin(115200);
  WiFi.begin(SSID, PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
  }
  pinMode(10,INPUT);
}

bool flag = false;

void loop(){
  bool read = digitalRead(10);
  if(read && !flag){
    sendPostRequest();
    flag = true;
  }
  else{
    flag = read;
  }
  delay(500);
}


void sendPostRequest() {
  http.begin(client, YOOUR_DISCORD_WEBHOOKURL, 443);

  http.addHeader("Content-Type: application/json");
  String payload = PAYLOAD;
  http.sendRequest("POST", payload, true);
  http.close();
}

