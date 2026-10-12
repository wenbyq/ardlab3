#define PIN_LED 13

volatile boolean actionState = LOW;

void setup() {
    Serial.begin(9600);
    pinMode(PIN_LED, OUTPUT);
    pinMode(2, INPUT_PULLUP);
    
    Serial.println("Waiting for button press...");
    
    attachInterrupt(0, myEventListener, CHANGE);
}

void loop() {
}

void myEventListener() {
    actionState = !actionState;
    digitalWrite(PIN_LED, actionState);
    
    Serial.print("INTERRUPT! LED = ");
    Serial.println(actionState);
}