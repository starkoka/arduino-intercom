#include <WiFiS3.h>
#include <WiFiSSLClient.h>
#include <R4HttpClient.h>
#include "Arduino_LED_Matrix.h"
#include "FspTimer.h"
#include "main.h"
#include "seacret.h"


WiFiSSLClient client;
WiFiServer server(80);
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
  server.begin();
}


void loop() {
  WiFiClient client = server.available();   // listen for incoming clients
  sendWebhook();
  if (client) {                             // if you get a client,
    Serial.println("new client");           // print a message out the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      sendWebhook();
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out to the serial monitor
        if (c == '\n') {                    // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();

            // the content of the HTTP response follows the header:
            String now = "STANDBY";
            if(matrixState == _STATE_OK_){
              now = "OK";
            }
            else if(matrixState == _STATE_DONT_ENTRY_){
              now = "Dont't entry";
            }
            else if(matrixState == _STATE_NG_){
              now = "NG";
            }
            else if(matrixState == _STATE_WAIT_){
              now = "WAIT"; 
            }

            client.print("<div style=\"font-family: Arial, sans-serif; display: flex; flex-direction: column; align-items: center; justify-content: center; min-height: 100vh; margin: 0; background: linear-gradient(135deg, #ece9e6, #ffffff);\">");
            client.print("<h1 style=\"font-size: 2rem; margin-bottom: 20px;\">Status Selector</h1>");
            client.print("<div id=\"currentStatus\" style=\"margin-bottom: 20px; padding: 10px 20px; font-size: 1.2rem; color: white; background-color: #333; border-radius: 5px; box-shadow: 0 4px 6px rgba(0, 0, 0, 0.1);\">Current Status : " + now + "</div>");
            client.print("<div style=\"display: flex; gap: 10px;\">");
            client.print("<a href=\"/O\" style=\"display: inline-block; text-decoration: none; padding: 15px 25px; font-size: 1rem; color: white; background-color: #28a745; border-radius: 5px; box-shadow: 0 4px 6px rgba(0, 0, 0, 0.1); transition: opacity 0.3s ease;\">OK</a>");
            client.print("<a href=\"/D\" style=\"display: inline-block; text-decoration: none; padding: 15px 25px; font-size: 1rem; color: white; background-color: #007bff; border-radius: 5px; box-shadow: 0 4px 6px rgba(0, 0, 0, 0.1); transition: opacity 0.3s ease;\">Don't entry</a>");
            client.print("<a href=\"/G\" style=\"display: inline-block; text-decoration: none; padding: 15px 25px; font-size: 1rem; color: white; background-color: #dc3545; border-radius: 5px; box-shadow: 0 4px 6px rgba(0, 0, 0, 0.1); transition: opacity 0.3s ease;\">NG</a>");
            client.print("<a href=\"/W\" style=\"display: inline-block; text-decoration: none; padding: 15px 25px; font-size: 1rem; color: white; background-color: #ffc107; border-radius: 5px; box-shadow: 0 4px 6px rgba(0, 0, 0, 0.1); transition: opacity 0.3s ease;\">Wait</a>");
            client.print("<a href=\"/C\" style=\"display: inline-block; text-decoration: none; padding: 15px 25px; font-size: 1rem; color: white; background-color: #6c757d; border-radius: 5px; box-shadow: 0 4px 6px rgba(0, 0, 0, 0.1); transition: opacity 0.3s ease;\">Clear</a>");
            client.print("</div>");
            client.print("</div>");



            
            // The HTTP response ends with another blank line:
            client.println();
            // break out of the while loop:
            break;
          } else {    // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }

        // Check to see if the client request was "GET /H" or "GET /L":
        if (currentLine.endsWith("GET /O")) {
          matrixState = _STATE_OK_;             // GET /H turns the LED on
        }
        if (currentLine.endsWith("GET /D")) {
          matrixState = _STATE_DONT_ENTRY_;               // GET /L turns the LED off
        }
        if (currentLine.endsWith("GET /G")) {
          matrixState = _STATE_NG_;              // GET /L turns the LED off
        }
        if (currentLine.endsWith("GET /W")) {
          matrixState = _STATE_WAIT_;              // GET /L turns the LED off
        }
        if (currentLine.endsWith("GET /C")) {
          matrixState = _STATE_STANDBY_;              // GET /L turns the LED off
        }
      }
      
    }
    // close the connection:
    client.stop();
    Serial.println("client disconnected");
  }
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
    case _STATE_STANDBY_:
      matrix.loadFrame(animation[0]);
      break;

    case _STATE_POSTTING_:
      matrix.loadFrame(animation[((ck-beforeCK)/5)%4 + 5]);
      break;

    case _STATE_RINGING_:   
      matrix.loadFrame(animation[((ck-beforeCK)/5)%4 + 1]);
      if(now == 0){
        tone(SPEAKER_PIN,660,1000);
      }
      else if(now == 10){
        tone(SPEAKER_PIN,550,1000);
      }
      else if(now > 30){
        matrixState = _STATE_STANDBY_;
      }
      break;

    default:
      if(now > 3000){
        matrixState = _STATE_STANDBY_;
      }
      else{
        matrix.loadFrame(animation[matrixState+6]);
      }
  }

  beforeState = matrixState;
}

#define SEND_STRING "<@" YOUR_DISCORD_USERID "> ドアの呼び出しが鳴りました！"
#define PAYLOAD "{\"content\":\"" SEND_STRING "\"}"

void sendWebhook(){
  static bool flag = false;
  bool read = digitalRead(SWITCH_PIN);
  if(read && !flag){
    matrixState = _STATE_POSTTING_;
    http.begin(client, YOOUR_DISCORD_WEBHOOKURL, 443);
    http.addHeader("Content-Type: application/json");
    String payload1 = "{\"content\":\"<@";
    String payload2 = YOUR_DISCORD_USERID;
    String payload3 = "> ドアの呼び出しが鳴りました！[応答はこちら](http://";
    String payload4 = WiFi.localIP().toString();
    String payload5 = ")\"}";
    http.sendRequest("POST", payload1 + payload2 + payload3 + payload4 + payload5, true);
    http.close();
    matrixState = _STATE_RINGING_;
    flag = true;
  }
  else{
    flag = read;
  }
}