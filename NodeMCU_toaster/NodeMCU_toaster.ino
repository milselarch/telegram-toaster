#include <Regexp.h>

/*******************************************************************
 *  this is a basic example how to program a Telegram Bot          *
 *  using TelegramBOT library on ESP8266                           *
 *                                                                 *
 *  Open a conversation with the bot, it will echo your messages   *
 *  https://web.telegram.org/#/im?p=@EchoBot_bot                   *                                                                 
 *                                                                 *
 *  written by Giacarlo Bacchio                                    *
 *******************************************************************/
 
#include <LiquidCrystal.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

// Initialize Wifi connection to the router
const char ssid[] = "Singtel7002-FBEF";
const char password[] = "0013499375";

void setup() {
	pinMode(D7, OUTPUT);
	Serial.begin(115200);
	delay(3000);

	// attempt to connect to Wifi network:
	Serial.print("Connecting Wifi: ");
	Serial.println(ssid);
	
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


