/*

Udp NTP Client

Get the time from a Network Time Protocol (NTP) time server
Demonstrates use of UDP sendPacket and ReceivePacket
For more on NTP time servers and the messages needed to communicate with them,
see http://en.wikipedia.org/wiki/Network_Time_Protocol

created 4 Sep 2010
by Michael Margolis
modified 9 Apr 2012
by Tom Igoe
updated for the ESP8266 12 Apr 2015
by Ivan Grokhotkov

This code is in the public domain.

*/

#include <Time.h>
#include <TimeLib.h>

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

unsigned int localPort = 2390;      // local port to listen for UDP packets
/* Don't hardwire the IP address or we won't get the benefits of the pool.
*  Lookup the IP address for the host name instead */
//IPAddress timeServer(129, 6, 15, 28); // time.nist.gov NTP server

IPAddress timeServerIP; // time.nist.gov NTP server address
const char* ntpServerName = "time.nist.gov";
const int NTP_PACKET_SIZE = 48; // NTP time stamp is in the first 48 bytes of the message
byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets
// A UDP instance to let us send and receive packets over UDP
WiFiUDP udp;

class NTPhelper {
private:
    unsigned long lastNtpRequest = -1;
    unsigned long intervalMs = 3600 * 1000;

public:
    NTPhelper() {
        udp.begin(localPort);
        //Serial.print("Local port: ");
        //Serial.println(udp.localPort());
    }

    void setInterval(float seconds) {
        if (seconds >= 4) {
            this->intervalMs = long(seconds * 1000);
        }
    }

    bool canSendNtpRequest() {
        unsigned long timeNow = millis();
        //Serial.println(String(this->lastNtpRequest) + " | " + String(timeNow));

        if (this->lastNtpRequest == -1) {
            return true;
        } else if (this->lastNtpRequest > timeNow) {
            this->lastNtpRequest = 0;
            //Serial.println("NTP CANNOT");
            return false;
        } else if (timeNow - this->lastNtpRequest < this->intervalMs) {
            //Serial.println("NTP CANNOT");
            return false;
        } else {
            Serial.println("NTP CAN");
            return true;
        }
    }

    unsigned long getEpochTime() {
        if (this->canSendNtpRequest() == false) {
            //Serial.println("SEND NTP DISALLOWED");
            return 0;
        }

        //get a random server from the pool
        WiFi.hostByName(ntpServerName, timeServerIP);
        this->sendNTPpacket(timeServerIP); // send an NTP packet to a time server
        // wait to see if a reply is available
        delay(1000);

        int cb = udp.parsePacket();
        if (!cb) {
            //Serial.println("no packet yet");
            return 0;
        } else {
            //Serial.print("packet received, length=");
            //Serial.println(cb);
            // We've received a packet, read the data from it
            udp.read(packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer
            this->lastNtpRequest = millis();

            //the timestamp starts at byte 40 of the received packet and is four bytes,
            //or two words, long. First, esxtract the two words:

            unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
            unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
            // combine the four bytes (two words) into a long integer
            // this is NTP time (seconds since Jan 1 1900):
            unsigned long secsSince1900 = highWord << 16 | lowWord;
            //Serial.print("Seconds since Jan 1 1900 = ");
            //Serial.println(secsSince1900);

            // now convert NTP time into everyday time:
            //Serial.print("Unix time = ");
            // Unix time starts on Jan 1 1970. In seconds, that's 2208988800:
            const unsigned long seventyYears = 2208988800UL;
            // subtract seventy years:
            unsigned long epoch = secsSince1900 - seventyYears;
            // print Unix time:
            //Serial.println(epoch);

            // print the hour, minute and second:
            //Serial.print("The UTC time is ");       // UTC is the time at Greenwich Meridian (GMT)
            //Serial.print((epoch % 86400L) / 3600); // print the hour (86400 equals secs per day)
            //Serial.print(':');
        
            return epoch;
        }
    }

    // send an NTP request to the time server at the given address
    unsigned long sendNTPpacket(IPAddress& address) {
        Serial.println("sending NTP packet...");
        // set all bytes in the buffer to 0
        memset(packetBuffer, 0, NTP_PACKET_SIZE);
        // Initialize values needed to form NTP request
        // (see URL above for details on the packets)
        packetBuffer[0] = 0b11100011;   // LI, Version, Mode
        packetBuffer[1] = 0;     // Stratum, or type of clock
        packetBuffer[2] = 6;     // Polling Interval
        packetBuffer[3] = 0xEC;  // Peer Clock Precision
        // 8 bytes of zero for Root Delay & Root Dispersion
        packetBuffer[12] = 49;
        packetBuffer[13] = 0x4E;
        packetBuffer[14] = 49;
        packetBuffer[15] = 52;

        // all NTP fields have been given values, now
        // you can send a packet requesting a timestamp:
        udp.beginPacket(address, 123); //NTP requests are to port 123
        udp.write(packetBuffer, NTP_PACKET_SIZE);
        udp.endPacket();
    }
};

class TimeKeeper : NTPhelper {
private:
    unsigned int offset = 0;

public:
    void setOffset(unsigned int offset) {
        this->offset = offset;
    }

    bool sync() {
        unsigned long epoch = this->getEpochTime();
        if (epoch != 0) {
            setTime(epoch + this->offset);
            return true;
        }
        
        return false;
    }

    time_t getDayEpoch(const time_t t) {
        return t - hour(t)*3600 - minute(t)*60 - second(t); 
    }

    void printTime() {
        time_t t = now();
        Serial.println("year: " + String(year(t)));
        Serial.println("month: " + String(month(t)));
        Serial.println("day: " + String(day(t)));
        Serial.println("hour: " + String(hour(t)));
        Serial.println("minute: " + String(minute(t)));
        Serial.println("second: " + String(second(t)));
    }

    String monthName(const unsigned int monthIndex) {
        String monthNames[] = {
            "Jan", "Feb", "Mar", "Apr", "May", "Jun",
            "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
        };
        
        return monthNames[monthIndex-1];
    }
};


