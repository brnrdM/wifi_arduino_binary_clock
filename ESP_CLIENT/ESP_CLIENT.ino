// Bernard Morelos Jr.
// ESP8266 Transmitter Module connected to the Arduino UNO
// Communicates with Arduino UNO using RX and TX (Serial Communication)
// Communicates with ESP8266 Host via WiFi TCP packets

#include <ESP8266WiFi.h>

// Initialize WiFi credentials
const char *ssid = "ESPap";
const char *password = "thereisnospoon";

void setup() {
    Serial.begin(115200);
    delay(10);

    // We start by connecting to a WiFi network
  
    /* Explicitly set the ESP8266 to be a WiFi-client, otherwise, it by default,
     would try to act as both a client and an access-point and could cause
     network-issues with your other WiFi-devices on your WiFi-network. */
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    // Connect to the Host
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
    }

    Serial.begin(115200);
}

void loop() {
    WiFiClient client;
    const char *host = "192.168.4.1";
    const int httpPort = 80;

    if (!client.connect(host, httpPort)) {
        //Serial.println("DEBUG: connection failed");
        return;
    }
      
    if (Serial.available()){
        if (Serial.find("+GET,")) {
            // Use WiFiClient class to create TCP connections
            // We now create a URI for the request
            String url = "/data/";
            url += "?sensor_reading=";
            url += 1;
      
            /* Serial.print("DEBUG: Requesting URL: ");
            Serial.println(url); */

            // This will send the request to the ESP server
            client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                                       "Host: " + host + "\r\n" +
                                     "Connection: close\r\n\r\n");
      	    /* DEBUG:
      	    Serial.println();
            Serial.println("Closing connection");
            Serial.println();
            Serial.println();
            Serial.println();
      	    */
        }
    }
    unsigned long timeout = millis();
  
    while (client.available() == 0) {
        if (millis() - timeout > 5000) {
          //Serial.println(">>> Client Timeout !");
          client.stop();
          return;
        }
    }

    delay (1000);
}
