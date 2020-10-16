// Bernard Morelos Jr.
// ESP8266 Receiver and fish feeder
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Servo.h>

Servo myservo;

// Define WiFi Credentials
const char *ssid = "ESPap";
const char *password = "thereisnospoon";

// Open port 80
ESP8266WebServer server(80);

// Function to feed the fish with a servo
void servoTurn(int degz) {
    int pos = 0;
    
    // Turn forward
    for (pos = 0; pos <= degz; pos += 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
       myservo.write(pos);       // tell servo to go to position in variable 'pos'
       delay(15);                // waits 15ms for the servo to reach the position
     }
    delay (250);

    // Turn back
    for (pos = degz; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
      myservo.write(pos);         // tell servo to go to position in variable 'pos'
      delay(15);                 // waits 15ms for the servo to reach the position
    }

    return;
}

// Function to handle packet
void handleSentVar() { // this is the variable sent from the client
  if (server.hasArg("sensor_reading")) {
    int readingInt = server.arg("sensor_reading").toInt();
    Serial.println(readingInt);
    server.send(200, "text/html", "Data received");
    servoTurn(90);
  }
  else {
    Serial.println("null");
    delay(500);
  }
}


void setup() {
  delay(1000);
  
  Serial.begin(115200);
  Serial.println("Serial printed");
  
  delay(10);
  
  WiFi.softAP(ssid, password);
  IPAddress myIP = WiFi.softAPIP();

  server.on("/data/", HTTP_GET, handleSentVar); 

// when the server receives a request with /data/ in the string then run the handleSentVar function
  server.begin();
  Serial.println("Server Started");

  myservo.attach(2);
}

// Loop by just handling the client.
void loop() {
  server.handleClient();
  //Serial.println(0);
}

