class ToastState {
    private:
    unsigned int startTime;
    unsigned int endTime;

    public:
    ToastState() {
        this->reset();
    }
    
    void extract(String text) {
        String startString = "";
        String endString = "";
        int k;
    
        for (k = 0; text.charAt(k) != ' '; k++) {
            startString += text[k];
        }
    
        for (; k < text.length(); k++) {
            endString += text[k];
        }
    
        this->startTime = startString.toInt();
        this->endTime = endString.toInt();
        Serial.println(String(startTime)+" "+String(endTime));
    }

    time_t tillStart(time_t timeNow) {
        return this->startTime - timeNow;
    }

    time_t sinceStart(time_t timeNow) {
        return timeNow - this->startTime;
    }

    time_t tillEnd(time_t timeNow) {
        return this->endTime - timeNow;
    }

    time_t sinceEnd(time_t timeNow) {
        return timeNow - this->endTime;
    }

    bool isToasting(time_t timeNow) {
        return (this->endTime >= timeNow && timeNow >= this->startTime);
    }

    bool toastFinished(time_t timeNow) {
        return (timeNow > this->endTime);    
    }

    bool hasPendingToast() {
        return this->startTime != -1;
    }
    
    void reset() {
        this->startTime = -1;
        this->endTime = -1;
    }
};
