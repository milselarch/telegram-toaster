class Requester {
    public:
    unsigned long lastRequest = -1;
    unsigned long timeOut = 5000;
    String response;
    
    bool request() {
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
                this->response = http.getString();
                return true; 
            }
            
        } else {
            Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
        }
    
        http.end();
        
        return false;    
    }

    bool timedRequest() {
        if (abs(millis() - this->lastRequest) > this->timeOut) {
            this->lastRequest = millis();
            return this->request();
        }

        return false;
    }
};


