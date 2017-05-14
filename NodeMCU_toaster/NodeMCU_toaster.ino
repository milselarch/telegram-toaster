#include <Servo.h>

#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <ESP8266TelegramBOT.h>

#include <DNSServer.h>         // Local DNS Server used for redirecting all requests to the configuration portal
#include <ESP8266WebServer.h>    // Local WebServer used to serve the configuration portal
#include <WiFiManager.h>         // https://github.com/tzapu/WiFiManager WiFi Configuration Magic

const int toastDegree = 0;
const int untoastDegree = 180;

WiFiManager wifiManager;
Servo myservo;

void setup() {
	pinMode(D7, OUTPUT);
	Serial.begin(115200);
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

    // configure traged server and url
    //http.begin("https://192.168.1.12/test.html", "7a 9c f4 db 40 d3 62 5a 6e 21 bc 5c cc 66 c8 3e a1 45 59 38"); //HTTPS
    http.begin("http://192.168.1.150:8080/"); //HTTP

    // start connection and send HTTP header
    int httpCode = http.GET();
	Serial.printf("[HTTP] GET... code: %d\n", httpCode);
	
    // httpCode will be negative on error
    if(httpCode > 0) {
        // HTTP header has been send and Server response header has been handled

        // file found at server
        if (httpCode == HTTP_CODE_OK) {
            String payload = http.getString();
            Serial.println(payload);
        }
    } else {
        Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }

    http.end();	

    delay(2000);
}


