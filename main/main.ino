#include <WiFiS3.h>
#include <WiFiSSLClient.h>
#include <R4HttpClient.h>
#include <TimedAction.h>
#include "main.h"

void sendPostRequest();
void speaker();
WiFiSSLClient client;
R4HttpClient http;

#define SEND_STRING "<@" YOUR_DISCORD_USERID "> ドアの呼び出しが鳴りました！"
#define PAYLOAD "{\"content\":\"" SEND_STRING "\"}"
#define DISCORD_WEBHOOKPORT 443
#define SWITCH_PIN 10
#define SPEAKER_PIN 12

TimedAction speakerAction = TimedAction(1, speaker);
TimedAction webhookAction = TimedAction(1, sendPostRequest);

void setup() {
  Serial.begin(115200);
  WiFi.begin(SSID, PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
  }
  pinMode(10,INPUT);
}

void loop(){
  speakerAction.check();
  webhookAction.check();
}

void sendPostRequest() {
  static bool flag = false;
  bool read = digitalRead(SWITCH_PIN);
  if(read && !flag){
    http.begin(client, YOOUR_DISCORD_WEBHOOKURL, 443);
    http.addHeader("Content-Type: application/json");
    String payload = PAYLOAD;
    http.sendRequest("POST", payload, true);
    http.close();
    flag = true;
  }
  else{
    flag = read;
  }
}

void speaker(){
  static bool flag = false;
  bool read = digitalRead(SWITCH_PIN);
  if(read && !flag){
    tone(SPEAKER_PIN,660,1000); // ピン
    delay(1000);
    tone(SPEAKER_PIN,550,1000); // ポーン
    delay(2000);
    flag = true;
  }
  else{
    flag = read;
  }
}