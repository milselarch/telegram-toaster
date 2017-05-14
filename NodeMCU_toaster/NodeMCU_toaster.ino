#include <Servo.h>

#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>

#include <DNSServer.h>         // Local DNS Server used for redirecting all requests to the configuration portal
#include <ESP8266WebServer.h>    // Local WebServer used to serve the configuration portal
#include <WiFiManager.h>         // https://github.com/tzapu/WiFiManager WiFi Configuration Magic

#include <LiquidCrystal_I2C.h>

const int toastDegree = 0;
const int untoastDegree = 180;

LiquidCrystal_I2C lcd(0x27, 16, 2);
WiFiManager wifiManager;
Servo myservo;

void setup() {
	pinMode(D7, OUTPUT);
	Serial.begin(115200);
    // Servo is HS-5945MG, pulse width range is different
    myservo.attach(D3, 900, 2100);
	delay(3000);

	// attempt to connect to Wifi network:
    Serial.print("Connecting Wifi: ");
    wifiManager.autoConnect("AutoConnectAP");
	
	Serial.println("");
	Serial.println("WiFi connected");
	Serial.println("IP address: ");
	IPAddress ip = WiFi.localIP();
	Serial.println(ip);
}


void loop () {
	HTTPClient http;
    http.begin("http://192.168.1.151:8080/"); //HTTP

    // start connection and send HTTP header
    int httpCode = http.GET();
	Serial.printf("[HTTP] GET... code: %d\n", httpCode);
	
    // httpCode will be negative on error
    if (httpCode > 0) {
        // HTTP header has been send and Server response header has been handled

        // file found at server
        if (httpCode == HTTP_CODE_OK) {
            String payload = http.getString();

            if (payload == "on") {
                myservo.write(toastDegree);
            } else if (payload == "off") {
                myservo.write(untoastDegree);              
            }
        }
    } else {
        Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }

    http.end();	

    delay(2000);
}


