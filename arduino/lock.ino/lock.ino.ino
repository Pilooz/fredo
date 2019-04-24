#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <Servo.h>

#define SERVO_PIN 2
#define GND_PIN 14
const char* ssid = "Pilouphone";
const char* password = "3r4sm369!";

ESP8266WebServer server(80);
Servo servoLock;

const int led = 13;

void handleRoot() {
  String page = "<html><body><ul><li><a href='/on'>Open the lock</a></li><li><a href='/off'>Close the lock</a></li></ul></body></html>";
  digitalWrite(led, 1);
  server.send(200, "text/html", page);
  digitalWrite(led, 0);
}


void openLock() {
  Serial.println("Opening the lock");
  //for (int pos = 0; pos <= 180; pos += 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    servoLock.attach(SERVO_PIN);
    servoLock.write(180);              // tell servo to go to position in variable 'pos'
    delay(1000);
    servoLock.detach();
  //}
  server.send(200, "text/plain", "Opening the lock");
};

void closeLock() {
  Serial.println("Closing the lock");
  //for (int pos = 180; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
  servoLock.attach(SERVO_PIN);
    servoLock.write(0);              // tell servo to go to position in variable 'pos'
    delay(1000);                       // waits 15ms for the servo to reach the position
  servoLock.detach();
  //}
  server.send(200, "text/plain", "Closing the lock");
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

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);

  server.on("/on", openLock);

  server.on("/off", closeLock);

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");

  servoLock.attach(SERVO_PIN);
  Serial.print("servo attached to pin #");
  Serial.println(SERVO_PIN);

}

void loop(void) {
  server.handleClient();
}
