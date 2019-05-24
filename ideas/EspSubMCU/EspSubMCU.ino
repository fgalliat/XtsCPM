/**
 * Esp8266 - ESP12 as SubMCU replacement
 * 
 * Xtase - fgalliat @May 2019
 */

// NodeMCU led GPIO16
// ESP led GPIO2
#define LED 2
bool ledState = false;

void led(bool state) {
    digitalWrite(LED, state ? HIGH : LOW);
    ledState = state;
}


void setup() {
    pinMode(LED, OUTPUT);
    led(false);

    Serial.begin(115200);
    Serial.println("ESP is ON");
}

void loop() {
    led( !ledState );
    Serial.print(".");
    delay(500);
}