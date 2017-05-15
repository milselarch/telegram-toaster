#include <Servo.h>

#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>

#include <DNSServer.h>         // Local DNS Server used for redirecting all requests to the configuration portal
#include <ESP8266WebServer.h>    // Local WebServer used to serve the configuration portal
#include <WiFiManager.h>         // https://github.com/tzapu/WiFiManager WiFi Configuration Magic

#include "NTPhelper.h"
#include "toast_status.h"
#include "requests.h"

#include <LiquidCrystal_I2C.h>

const int toastDegree = 0;
const int untoastDegree = 180;

ToastState state;
TimeKeeper timeTracker;
Requester requester;

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

    //timeTracker.setOffset(8*3600);  //UTC+8
    while (!timeTracker.sync()) { delay(1000); }
}


void loop () {
    if (requester.timedRequest()) {
        String payload = requester.response;
        
    	if (payload == "off") {
            state.reset();
        } else {
            state.extract(payload);
        }
    }

    if (state.hasPendingToast()) {
        time_t timeNow = now();
        Serial.println("secs now: " + String(timeNow));
        
        if (state.isToasting(timeNow)) {
            myservo.write(toastDegree);
            Serial.println("secs since start: " + String(state.sinceStart(timeNow)));
        } else {
            Serial.println("not toasting as of now"); //state.reset();    
        }
         
    } else {
        myservo.write(untoastDegree);
    }

    timeTracker.sync();
    delay(500);
}


