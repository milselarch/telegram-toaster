#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>

#include <LiquidCrystal_I2C.h>

#include <DNSServer.h>         // Local DNS Server used for redirecting all requests to the configuration portal
#include <ESP8266WebServer.h>    // Local WebServer used to serve the configuration portal
#include <WiFiManager.h>         // https://github.com/tzapu/WiFiManager WiFi Configuration Magic

#include "NTPhelper.h"
#include "toast_status.h"
#include "requests.h"
#include "toaster.h"

LiquidCrystal_I2C lcd(0x27, 16, 2);
WiFiManager wifiManager;

ToastState state;
TimeKeeper timeTracker;
Requester requester;
Toaster toaster;


void lcdWrite(String line1, String line2) {
    lcd.setCursor(0, 0);
    lcd.print(line1 + "                      ");
    lcd.setCursor(0, 1);
    lcd.print(line2 + "                      ");
}
void lcdWrite(String line) {
    lcdWrite(line, "");
}

void setup() {
	pinMode(D7, OUTPUT);
    
    lcd.init();
    lcdWrite("TOASTER BOOT");
    delay(500);
    
	Serial.begin(115200);
	// attempt to connect to Wifi network:
    Serial.print("Connecting Wifi: ");
    wifiManager.autoConnect("AutoConnectAP");

    lcdWrite("WIFI INIT");
    delay(1000);
    
	Serial.println("");
	Serial.println("WiFi connected");
	Serial.println("IP address: ");
	IPAddress ip = WiFi.localIP();
	Serial.println(ip);
 
    lcdWrite("IP ADDRESS", ip.toString());
    delay(500);

    timeTracker.setOffset(8*3600); //UTC+8 singapore
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

    time_t timeNow = now();
    
    if (state.hasPendingToast(timeNow)) {
        Serial.println("secs now: " + String(timeNow));
        
        if (state.isToasting(timeNow)) {
            lcdWrite(
                state.getProgress(timeNow) + " toasted!",
                String(state.sinceStart(timeNow)) + "s/"
                + String(state.tillEnd(timeNow)) + "s"
                );
            toaster.toast();

        } else {
            Serial.println("not toasting as of now"); //state.reset();
            time_t wait = state.tillStart(timeNow);
            time_t duration = state.duration();
        
            lcdWrite(
                String(duration/3600) + "h"
                + String((duration%3600)/60) + "m"
                + String(duration%60) + "s"
                + " toast:",
                
                "in " +
                String(wait/3600/24) + "d " 
                + String((wait%(3600*24))/3600) + "h"
                + String((wait%(3600))/60) + "m"
                + String(wait%60) + "s"
                );
        }
         
    } else {
        lcdWrite(
            "not toasting",
            String(day(timeNow)) + " " 
            + timeTracker.monthName(month(timeNow)) + " "
            + String(hour(timeNow)) + ":" 
            + String(minute(timeNow)) + ":" 
            + String(second(timeNow))
            );
                
        toaster.untoast();
    }

    timeTracker.sync();
    delay(100);
}


