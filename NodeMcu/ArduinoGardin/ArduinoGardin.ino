#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ArduinoJson.h>
#include <WebSocketsClient.h>

#include <Hash.h>
DynamicJsonDocument doc(2048);
ESP8266WiFiMulti WiFiMulti;
String state = "up";
WebSocketsClient webSocket;
#define USE_SERIAL Serial

int A = 14;
int B = 12;
int C = 13;
int D = 5;
long del = 20000;
int stap = 1;

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
	switch(type) {
		case WStype_DISCONNECTED:
			USE_SERIAL.printf("[WSc] Disconnected!\n");
			break;
		case WStype_CONNECTED: {
			USE_SERIAL.printf("[WSc] Connected to url: %s\n", payload);

			// send message to server when Connected
			webSocket.sendTXT("{\"type\":\"server_connect\"}");
		}
			break;
		case WStype_TEXT:
      onMessage(payload);
      // USE_SERIAL.printf("[WSc] get text: %s\n", payload);
			// send message to server
			// webSocket.sendTXT("message here");
			break;
		case WStype_BIN:
			USE_SERIAL.printf("[WSc] get binary length: %u\n", length);
			hexdump(payload, length);
  
			// send data to server
			// webSocket.sendBIN(payload, length);
			break;
        case WStype_PING:
            // pong will be send automatically
            USE_SERIAL.printf("[WSc] get ping\n");
            break;
        case WStype_PONG:
            // answer to a ping we send
            USE_SERIAL.printf("[WSc] get pong\n");
            break;
    }

}

void onMessage(uint8_t * data)

{  
   DeserializationError errorTEST = deserializeJson(doc, data); //Gör om inkommande JSON sträng till Array
   if (errorTEST) { // Error check när JSON-string omvandlas
        USE_SERIAL.print(F("deserializeJson() failed: "));
        USE_SERIAL.println(errorTEST.f_str());
   }
   String data_to_send;
   String type = doc["type"].as<char*>(); //Hämtar "type": "type" delen av JSON,
   String msg = doc["msg"].as<char*>(); // Hämtar "msg" : "msg" delen av JSON
   
   if(type == "client_connect"){ //ifall "type" : "Client_connect" skickas statusen av gardinen ut till clienterna
        doc["type"] = "send_state";
        doc["msg"] = state;
        serializeJson(doc, data_to_send);
        webSocket.sendTXT(data_to_send);
   }else if(type == "client_action"){ // När clienten skickar att de ska flytta gardinen
        if(msg == "up"){
          //MOVING THE BLINDS UP 
          moveBlind();
          state = "up";
          doc["type"] = "send_state";
          doc["msg"] = state;
          //delay(1000);
          serializeJson(doc, data_to_send);
          webSocket.sendTXT(data_to_send);
        }else if(msg == "down"){
          //MOVING THE BLINDS DOWN
          moveBlind();
          state = "down";
          doc["type"] = "send_state";
          doc["msg"] = state;
          //delay(1000);
          serializeJson(doc, data_to_send);
          webSocket.sendTXT(data_to_send);
        }else{ // Error hantering
          doc["type"] = "error";
          doc["msg"] = "error: Couln't move blinds";
          //delay(1000);
          serializeJson(doc, data_to_send);
          webSocket.sendTXT(data_to_send);
        }
   }else{ // Error hantering
      doc["type"] = "error";
      doc["msg"] = "type not supported";
      serializeJsonPretty(doc, Serial);
   }
   
   //webSocket.sendTXT("{\"type\":\"send_state\", \"msg\": \", doc["type"] ,\"}");

}

void setup() {
  pinMode(A, OUTPUT);     
  pinMode(B, OUTPUT);     
  pinMode(C, OUTPUT);     
  pinMode(D, OUTPUT);
	// USE_SERIAL.begin(921600);
	USE_SERIAL.begin(115200);

	//Serial.setDebugOutput(true);
	USE_SERIAL.setDebugOutput(true);

	USE_SERIAL.println();
	USE_SERIAL.println();
	USE_SERIAL.println();

	for(uint8_t t = 4; t > 0; t--) {
		USE_SERIAL.printf("[SETUP] BOOT WAIT %d...\n", t);
		USE_SERIAL.flush();
		delay(1000);
	}

	WiFiMulti.addAP("tekniklabbet", "tekniklabbet");

	//WiFi.disconnect();
	while(WiFiMulti.run() != WL_CONNECTED) {
		delay(100);
	}

	// server address, port and URL
	webSocket.begin("192.168.30.231", 8080, "/", "");

	// event handler
	webSocket.onEvent(webSocketEvent);

	// use HTTP Basic Authorization this is optional remove if not needed
	//webSocket.setAuthorization("user", "Password");

	// try ever 5000 again if connection has failed
	webSocket.setReconnectInterval(5000);
  
  // start heartbeat (optional)
  // ping server every 15000 ms
  // expect pong from server within 3000 ms
  // consider connection disconnected if pong is not received 2 times
  //webSocket.enableHeartbeat(15000, 3000, 2);

}

void moveBlind(){
  for (int i=0; i<=500; i++){
    een(); 
    twee();
    drie();
    vier();
  }
  motorOff();
}

void een(){
  digitalWrite(A, HIGH);   
  digitalWrite(B, HIGH);   
  digitalWrite(C, HIGH);   
  digitalWrite(D, HIGH); 
  Serial.println("een");  
  delayMicroseconds(del);
}
void twee(){
  digitalWrite(A, LOW);   
  digitalWrite(B, HIGH);   
  digitalWrite(C, LOW);   
  digitalWrite(D, HIGH); 
   Serial.println("twee");    
  delayMicroseconds(del);
}
void drie(){
  digitalWrite(A, HIGH);   
  digitalWrite(B, LOW);   
  digitalWrite(C, LOW);   
  digitalWrite(D, HIGH);   
   Serial.println("drie");  
  delayMicroseconds(del);
}
void vier(){
  digitalWrite(A, HIGH);   
  digitalWrite(B, LOW);   
  digitalWrite(C, HIGH);   
  digitalWrite(D, LOW); 
  Serial.println("vier");  
  delayMicroseconds(del);
}
void motorOff(){
  digitalWrite(A, LOW);   
  digitalWrite(B, LOW);   
  digitalWrite(C, LOW);   
  digitalWrite(D, LOW);   
} 

void loop() {
	webSocket.loop();
}