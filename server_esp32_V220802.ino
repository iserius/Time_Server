
//aktifin SPIFFS uploader js
// websocket server for controlling timer in client using RF HC12
// board address https://docs.heltec.cn/download/package_heltec_esp32_index.json
// https://dl.espressif.com/dl/package_esp32_index.json, http://arduino.esp8266.com/stable/package_esp8266com_index.json


#include <WiFi.h>
#include <SPIFFS.h>
#include <ESPAsyncWebServer.h>
#include <WebSocketsServer.h>
#include <HardwareSerial.h>
#include <WiFiAP.h>

// Set your Static IP address
//IPAddress local_IP(192, 168, 100, 111);
IPAddress local_IP(10, 10, 10, 200);
//IPAddress gateway(192, 168, 100, 200);
IPAddress gateway(10, 10, 10, 11);
IPAddress subnet(255, 255, 255, 0);
IPAddress primaryDNS(8, 8, 8, 8);   //optional
IPAddress secondaryDNS(8, 8, 4, 4); //optional
//IPAddress primaryDNS(192, 168, 1, 1);   //optional
//IPAddress secondaryDNS(8, 8, 4, 4); //optional


HardwareSerial MySerial(2);
 
// Constants
//const char *ssid = "RiPiTeR";
//const char *password =  "";
//const char *ssid = "iSerius_Time";
//const char *password =  "rahasia22aja";
const char *ssid = "iSerius_TailQuarter";
const char *password =  "alkat2113";

const char *msg_toggle_led = "tL";
const char *msg_get_led = "gL";
const int dns_port = 53;
const int http_port = 80;
const int ws_port = 1337;
const int led_pin = LED_BUILTIN;
String bt = "";
const byte SET = 4;
// Globals
AsyncWebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(1337);
char msg_buf[10];
int led_state = 0;
/***********************************************************
 * Functions
 */
void bacaserial(){
     if (MySerial.available() > 0)
  {
    bt = MySerial.readString();
   // MySerial.print("receive:");
   // MySerial.println(bt);
   // Serial.print("received from - hc12 and send to websocket:");
    Serial.println(bt);
     webSocket.sendTXT(0, bt);
    bt="";
    MySerial.flush();
    
   }  
 }
 
// Callback: receiving any WebSocket message
void onWebSocketEvent(uint8_t client_num, WStype_t type,uint8_t * payload, size_t length) {


  
  // Figure out the type of WebSocket event
  switch(type) {
 
    // Client has disconnected
    case WStype_DISCONNECTED:
      Serial.printf("[%u] Disconnected!\n", client_num);
      break;
 
    // New client has connected
    case WStype_CONNECTED:
      {
        IPAddress ip = webSocket.remoteIP(client_num);
        Serial.printf("[%u] Connection from ", client_num);
        Serial.println(ip.toString());
      }
      break;
 
    // Handle text messages from client
    case WStype_TEXT:
 
      // Print out raw message
     // Serial.printf("[%u] Received text: %s\n", client_num, payload);
      //webSocket.sendTXT(client_num, payload);
      Serial.print("received from websocket and sent to HC12 : ");   
      //array payload to string  
      if(client_num==0){  // hanya web pertama yang bisa kirim ke HC12
       for(int i=0; i < length; i++) {
          Serial.print((char) payload[i]);
          MySerial.print((char) payload[i]);
            }
      
           
 
    Serial.println();
      //MySerial.println((char *)payload);
   //   break;
       if ( strcmp((char *)payload, "FU1") == 0 ) {
        FU1();
        Serial.print("set FU1 mode .. ");
       // MySerial.print("FU1");
        }
       if ( strcmp((char *)payload, "FU3") == 0 ) { 
        FU3();
        Serial.print("set FU3 mode .. ");
       // MySerial.print("FU3");
        }
        
      // Toggle LED
      //if ( strcmp((char *)payload, "toggleLED") == 0 ) {
      if ( strcmp((char *)payload, "tL") == 0 ) {
        led_state = led_state ? 0 : 1;
        Serial.printf("Toggling LED to %u\n", led_state);
        digitalWrite(led_pin, led_state);
 
      // Report the state of the LED
      //} else if ( strcmp((char *)payload, "getLEDState") == 0 ) {
      } else if ( strcmp((char *)payload, "gL") == 0 ) {
        sprintf(msg_buf, "%d", led_state);
        Serial.printf("Sending to [%u]: %s\n", client_num, msg_buf);
        webSocket.sendTXT(client_num, msg_buf);

       } else {
       // Serial.println("[%u] Message not recognized");
      }
      break;
      }  // end if only first web browser

      
    // For everything else: do nothing
    case WStype_BIN:
    case WStype_ERROR:
    case WStype_FRAGMENT_TEXT_START:
    case WStype_FRAGMENT_BIN_START:
    case WStype_FRAGMENT:
    case WStype_FRAGMENT_FIN:
    default:
      break;
  }

}
 
// Callback: send homepage
void onIndexRequest(AsyncWebServerRequest *request) {
  IPAddress remote_ip = request->client()->remoteIP();
  Serial.println("[" + remote_ip.toString() +
                  "] HTTP GET request of " + request->url());
  request->send(SPIFFS, "/index.html", "text/html");
}
 
// Callback: send style sheet
void onCSSRequest(AsyncWebServerRequest *request) {
  IPAddress remote_ip = request->client()->remoteIP();
  Serial.println("[" + remote_ip.toString() +
                  "] HTTP GET request of " + request->url());
  request->send(SPIFFS, "/style.css", "text/css");
}
// Callback: send style sheet
void onBootStrapRequest(AsyncWebServerRequest *request) {
  IPAddress remote_ip = request->client()->remoteIP();
  Serial.println("[" + remote_ip.toString() +
                  "] HTTP GET request of " + request->url());
  request->send(SPIFFS, "/bootstrap.min.css", "text/css");
}


 
// Callback: send 404 if requested file does not exist
void onPageNotFound(AsyncWebServerRequest *request) {
  IPAddress remote_ip = request->client()->remoteIP();
  Serial.println("[" + remote_ip.toString() +
                  "] HTTP GET request of " + request->url());
  request->send(404, "text/plain", "Not found");
}


/***********************************************************
 * Main
 */
void FU3()
{
 digitalWrite(SET, LOW);         // Enter command mode
 delay(60);   
 MySerial.println("AT+FU3");     // set FU1
 delay(60);                 
 digitalWrite(SET, HIGH);        // close command mode
 delay(200); 
}
void FU1()
{ 
  delay(200); 
 digitalWrite(SET, LOW);         // Enter command mode
 delay(60);   
 MySerial.println("AT+FU1");     // set FU1
 delay(60);                 
 digitalWrite(SET, HIGH);        // close command mode
 delay(200); 
}
 
void setup() {
  // Init LED and turn off
  pinMode(led_pin, OUTPUT);
  digitalWrite(led_pin, LOW);
  pinMode (SET, OUTPUT);
  
  // Start Serial port
  Serial.begin(115200);
 // MySerial.begin(9600, SERIAL_8N1, 12, 13);
  MySerial.begin(9600, SERIAL_8N1, 17, 16);

 FU3 (); // set normal FU1 mode

 
  // Make sure we can read the file system
  if( !SPIFFS.begin()){
    Serial.println("Error mounting SPIFFS");
    while(1);
  }
 
  // Start access point
 // WiFi.softAP(ssid, password);

   // Start client
 
    // Configures static IP address
  if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
    Serial.println("STA Failed to configure");
  }

   // Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(750);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();



  // Print our IP address
  Serial.println();
  Serial.println("iSerius Technology 2022");
  Serial.println("Timer Server");
  Serial.println();
  Serial.println("AP running");
 // Serial.print("My IP address: ");
 // Serial.println(WiFi.softAPIP());
 
  // On HTTP request for root, provide index.html file
  server.on("/", HTTP_GET, onIndexRequest);
 
  // On HTTP request for style sheet, provide style.css
  server.on("/style.css", HTTP_GET, onCSSRequest);

  server.on("/bootstrap.min.css", HTTP_GET, onBootStrapRequest);
 
  // Handle requests for pages that do not exist
  server.onNotFound(onPageNotFound);
 
  // Start web server
  server.begin();
 
  // Start WebSocket server and assign callback
  webSocket.begin();
  webSocket.onEvent(onWebSocketEvent);

}
 
void loop() {
    bacaserial();
  // Look for and handle WebSocket data
  webSocket.loop();


}
