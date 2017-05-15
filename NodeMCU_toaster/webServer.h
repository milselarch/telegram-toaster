#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

ESP8266WebServer server(80);

class WebServer() {
    void start() {
        if (MDNS.begin("esp8266")) {
            Serial.println("MDNS responder started");
        }
        
        server.on("/", this.handleRoot);
        server.on("/inline", []() {
            server.send(200, "text/plain", "this works as well");
        });
        
        server.onNotFound(this.handleNotFound);
        
        server.begin();
        Serial.println("HTTP server started");
    }

    void handleRequests() {
        server.handleClient();
    } 

    void handleRoot() {
        String message = "hello from esp8266!\n\n";
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
        
        server.send(200, "text/plain", message);
    }
    
    void handleNotFound(){
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
    }
}


