#include <Servo.h>

Servo myservo;

const unsigned int SHDN_PIN = D4;
const unsigned int SERV_PIN = D3;

const int toastDegree = 0;
const int untoastDegree = 180;

int prevDegree = -1;

class Toaster {
public:
    Toaster() {
        pinMode(SHDN_PIN, OUTPUT);
        // Servo is HS-5945MG, pulse width range is different
        myservo.attach(SERV_PIN, 900, 2100);
    }

    void toast() {
        prevDegree = toastDegree;
        myservo.write(toastDegree);
        digitalWrite(SHDN_PIN, HIGH);
    }

    void untoast() {
        if (untoastDegree != prevDegree) {
            prevDegree = untoastDegree;
            this->turnThenOff(untoastDegree);
        }
    }

private:
    void turnThenOff(int degree) {
        Serial.println(degree);
        
        myservo.write(degree);
        digitalWrite(SHDN_PIN, HIGH);
        delay(3000);
        digitalWrite(SHDN_PIN, LOW);

        Serial.println("done");
    }
};

