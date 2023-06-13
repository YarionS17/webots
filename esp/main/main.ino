/*
 * WebSocketClient.ino
 *
 *  Created on: 24.05.2015
 *
 */

#include <Arduino.h>

#include <WiFi.h>
#include <WiFiMulti.h>
#include <WiFiClientSecure.h>


#include <WebSocketsClient.h>
#include <Arduino_JSON.h>

#define LEDUP 8
#define LEDDOWN 9
#define LEDLEFT 10
#define LEDRIGHT 11

//#define BUTTON



WiFiMulti WiFiMulti;
WebSocketsClient webSocket;
char naam[] = "MEGA RAT";
int lastX = 0;
int lastY = 0;



#define USE_SERIAL Serial1

void hexdump(const void *mem, uint32_t len, uint8_t cols = 16) {
	const uint8_t* src = (const uint8_t*) mem;
	USE_SERIAL.printf("\n[HEXDUMP] Address: 0x%08X len: 0x%X (%d)", (ptrdiff_t)src, len, len);
	for(uint32_t i = 0; i < len; i++) {
		if(i % cols == 0) {
			USE_SERIAL.printf("\n[0x%08X] 0x%08X: ", (ptrdiff_t)src, i);
		}
		USE_SERIAL.printf("%02X ", *src);
		src++;
	}
	USE_SERIAL.printf("\n");
}

void getCommand(JSONVar command){
  if(command.hasOwnProperty("type")) {
    Serial.println("type = ");
    Serial.print(".");
    Serial.print((const char*)command["type"]);
    Serial.print(".");


  }
  String testtest = "target_position";
  if(testtest == (const char*)command["type"]) {
    Serial.println("target_position = ");
    Serial.print((int)command["x"]);
    Serial.print((int)command["y"]);
    directionLED((int)command["x"],(int)command["y"]);


  }
}

void directionLED(int x, int y){
  if(x > lastX){
    //digitalWrite(LEDRIGHT, HIGH);

    //digitalWrite(LEDLEFT, LOW);
    //digitalWrite(LEDUP, LOW);
    //digitalWrite(LEDDOWN, LOW);
  }else if(x < lastX){
    //digitalWrite(LEDLEFT, HIGH);

    //digitalWrite(LEDRIGHT, LOW);
    //digitalWrite(LEDUP, LOW);
    //digitalWrite(LEDDOWN, LOW);
  }else if(y > lastY){
    //digitalWrite(LEDUP, HIGH);

    //digitalWrite(LEDRIGHT, LOW);
    //digitalWrite(LEDLEFT, LOW);
    //digitalWrite(LEDDOWN, LOW);
  }else if(y < lastY){
    //digitalWrite(LEDDOWN, HIGH);

    //digitalWrite(LEDRIGHT, LOW);
    //digitalWrite(LEDUP, LOW);
    //digitalWrite(LEDLEFT, LOW);
  }
  lastX = x;
  lastY = y;
}

void emergencyStop(){
  webSocket.sendTXT("{\"type\":\"stop\",\"robot_name\":\"MEGA RAT\"}");
}

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
  char newstring[length+1];
  for (int i = 0; i < length; i++) {
    newstring[i] = (char)payload[i];
  }
  JSONVar myObject = JSON.parse(newstring);
	switch(type) {
		case WStype_DISCONNECTED:
			Serial.println("[WSc] Disconnected!\n");
			break;
		case WStype_CONNECTED:
			Serial.println("[WSc] Connected to url: %s\n");

			// send message to server when Connected
			webSocket.sendTXT("{\"type\":\"connect\",\"fake\":false,\"robot_name\":\"MEGA RAT\"}");
			break;
		case WStype_TEXT:
			getCommand(myObject);



			// send message to server
			// webSocket.sendTXT("message here");
			break;
		case WStype_BIN:
			USE_SERIAL.printf("[WSc] get binary length: %u\n", length);
			hexdump(payload, length);

			// send data to server
			// webSocket.sendBIN(payload, length);
			break;
		case WStype_ERROR:			
		case WStype_FRAGMENT_TEXT_START:
		case WStype_FRAGMENT_BIN_START:
		case WStype_FRAGMENT:
		case WStype_FRAGMENT_FIN:
			break;
	}

}

void setup() {

	// USE_SERIAL.begin(921600);
	USE_SERIAL.begin(115200);
  Serial.begin(115200);
  Serial.println("Start");
	//Serial.setDebugOutput(true);
	USE_SERIAL.setDebugOutput(true);

	USE_SERIAL.println();
	USE_SERIAL.println();
	USE_SERIAL.println();

  //LED
  pinMode(LEDUP, OUTPUT);
  pinMode(LEDDOWN, OUTPUT);
  pinMode(LEDRIGHT, OUTPUT);
  pinMode(LEDLEFT, OUTPUT);

  //BUTTON
  //pinMode(BUTTON, INPUT);

	for(uint8_t t = 4; t > 0; t--) {
		USE_SERIAL.printf("[SETUP] BOOT WAIT %d...\n", t);
		USE_SERIAL.flush();
		delay(1000);
	}

	//WiFiMulti.addAP("Tesla IoT", "fsL6HgjN");
  WiFiMulti.addAP("H369A096CBF", "65F6EE559AED");

	//WiFi.disconnect();
	while(WiFiMulti.run() != WL_CONNECTED) {
		delay(100);
    Serial.print(".");

	}
  Serial.println("Connected to WiFi");
	// server address, port and URL
	webSocket.begin("192.168.1.190", 8001, "/");

	// event handler
	webSocket.onEvent(webSocketEvent);

	
	// try ever 5000 again if connection has failed
	webSocket.setReconnectInterval(5000);

}

void loop() {
	webSocket.loop();
  //if(digitalRead(BUTTON) == HIGH){
  //  emergencyStop();
  //}
}