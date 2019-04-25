#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ESP8266HTTPClient.h>
#include <Servo.h>

#define SERVO_PIN 2
#define GND_PIN 14
//#define SERVER_IP "192.168.27.1"

const char* ssid = "fredo";
const char* password = "iotwifipass";

ESP8266WebServer server(80);
Servo servoLock;
String gateway = "";

const int led = 13;

void handleRoot() {
  String page = "<html><body><ul><li><a href='/on'>Open the lock</a></li><li><a href='/off'>Close the lock</a></li></ul></body></html>";
  digitalWrite(led, 1);
  server.send(200, "text/html", page);
  digitalWrite(led, 0);
}

// Sending a feedback to the server
void sendFeedback(String message) {
/*  
 WiFiClient client;
  if (!client.connect(gateway, 9000)) {
        return;
    }

    client.print(String("GET ") + "/feedbacklock?message=" + message + " HTTP/1.1\r\n" +
               "Host: " + gateway + "\r\n" + 
               "Connection: close\r\n\r\n");

    while(client.available()){
        String line = client.readStringUntil('\r');
        Serial.println(line);
    }
    */

  Serial.println("Sending a feedback to server...");
  HTTPClient http;    //Declare object of class HTTPClient

  //GET Data
  String getData = "?message=" + message ;  //Note "?" added at front
  String Link = "http://" + gateway + ":9000/feedbacklock" + getData;
  
  http.begin(Link);     //Specify request destination
  
  int httpCode = http.GET();            //Send the request
  String payload = http.getString();    //Get the response payload

  Serial.println(httpCode);   //Print HTTP return code
  Serial.println(payload);    //Print request response payload

  http.end();  //Close connection
  
}

void openLock() {
  Serial.println("Opening the lock");
  servoLock.attach(SERVO_PIN);
  //servoLock.write(180);              // tell servo to go to position in variable 'pos'
  //delay(1000);
  for (int pos = 0; pos <= 180; pos += 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    servoLock.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
  }
  servoLock.detach();
  sendFeedback("Lock has been opened.");
};

void closeLock() {
  Serial.println("Closing the lock");
  servoLock.attach(SERVO_PIN);

   for (int pos = 180; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
    servoLock.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
  }
  //servoLock.write(0);              // tell servo to go to position in variable 'pos'
  //delay(1000);                       // waits 15ms for the servo to reach the position
  servoLock.detach();
  sendFeedback("Lock has been closed.");
};

void handleNotFound() {
  digitalWrite(led, 1);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  digitalWrite(led, 0);
}

void setup(void) {
  pinMode(led, OUTPUT);
  digitalWrite(led, 0);

  // A ground pin for the servo
  pinMode(GND_PIN, OUTPUT);
  digitalWrite(GND_PIN, LOW);

  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  gateway = WiFi.gatewayIP().toString();

  Serial.print("Gateway IP address: ");
  Serial.println(gateway);

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);

  server.on("/on", []() {
    openLock();
    server.send(200, "text/plain", "Opening the lock");
  });

  server.on("/off", []() {
    closeLock();
    server.send(200, "text/plain", "Opening the lock");
  });

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");

  servoLock.attach(SERVO_PIN);
  Serial.print("servo attached to pin #");
  Serial.println(SERVO_PIN);

  closeLock();
  sendFeedback("Lock is ready ! ip = " + WiFi.localIP());
}

void loop(void) {
  server.handleClient();
}
