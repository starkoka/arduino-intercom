#include <WiFiS3.h>
#include <WiFiSSLClient.h>
#include <R4HttpClient.h>
#include "Arduino_LED_Matrix.h"
#include "FspTimer.h"
#include "main.h"
#include "seacret.h"




WiFiSSLClient client;
R4HttpClient http;
ArduinoLEDMatrix matrix;
FspTimer _timer;

void setup() {
  Serial.begin(115200);
  matrix.begin();
  pinMode(SWITCH_PIN,INPUT);
  pinMode(SPEAKER_PIN,OUTPUT);

  uint8_t type;
  int8_t ch = FspTimer::get_available_timer(type);
  if (ch < 0) {
    return;
  }
  _timer.begin(TIMER_MODE_PERIODIC, type, ch, 10.0f, 50.0f, matrixTimer, nullptr);
  _timer.setup_overflow_irq();
  _timer.open();
  _timer.start();

  WiFi.begin(SSID, PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
  }
}


void loop(){
  sendWebhook();
}

void matrixTimer(timer_callback_args_t *arg){
  ck++;
  static int beforeState = 0;
  static unsigned long beforeCK = 0;

  if(beforeState != matrixState){
    beforeCK = ck;
  }
  unsigned long now = ck-beforeCK;

  switch(matrixState){
    case 1:
      matrix.loadFrame(animation[((ck-beforeCK)/5)%4 + 5]);
      break;
    case 2:   
      matrix.loadFrame(animation[((ck-beforeCK)/5)%4 + 1]);
      if(now == 0){
        tone(SPEAKER_PIN,660,1000);
      }
      else if(now == 10){
        tone(SPEAKER_PIN,660,2000);
      }
      else if(now > 30){
        matrixState = 0;
      }
      break;
    default:
      matrix.loadFrame(animation[0]);
  }

  beforeState = matrixState;
}

void sendWebhook(){
  static bool flag = false;
  bool read = digitalRead(SWITCH_PIN);
  if(read && !flag){
    matrixState = 1;
    http.begin(client, YOOUR_DISCORD_WEBHOOKURL, 443);
    http.addHeader("Content-Type: application/json");
    String payload = PAYLOAD;
    http.sendRequest("POST", payload, true);
    http.close();
    matrixState = 2;
    flag = true;
  }
  else{
    flag = read;
  }
}