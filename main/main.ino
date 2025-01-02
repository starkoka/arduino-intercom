#include <WiFiS3.h>
#include <WiFiSSLClient.h>
#include <R4HttpClient.h>

#include "main.h"

void sendPostRequest();
WiFiSSLClient client;
R4HttpClient http;

#define SEND_STRING "<@" YOUR_DISCORD_USERID "> ドアの呼び出しが鳴りました！"
#define PAYLOAD "{\"content\":\"" SEND_STRING "\"}"

void setup() {
  Serial.begin(115200);
  WiFi.begin(SSID, PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
  }
  pinMode(10,INPUT);
}

int cnt = 0;

void loop(){
  delay(1000);
  cnt++;
  if(digitalRead(10)){
    Serial.print(cnt);
    sendPostRequest();
  }
  else{  
    Serial.println(cnt);
  }
}

void sendPostRequest() {
  http.begin(client, YOOUR_DISCORD_WEBHOOKURL, 443);

  http.addHeader("Content-Type: application/json");
  String payload = PAYLOAD;
  Serial.println(" : sended1");
  http.POST(payload);
  http.close();
}

