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
    
    void reset() {
        this->startTime = -1;
        this->endTime = -1;
    }
};
