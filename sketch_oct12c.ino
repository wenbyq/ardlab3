#include <avr/io.h>
#include <avr/interrupt.h>

#define LEDPIN 13
#define BUTTON 2

volatile boolean running = false;
volatile boolean blinkPhase = true;
volatile unsigned int secondsCount = 0;

void setup() {
    Serial.begin(9600);
    pinMode(LEDPIN, OUTPUT);
    pinMode(BUTTON, INPUT_PULLUP);
    Serial.println("Нажмите кнопку...");

    cli();

    // Timer1 — мигание 1 Гц
    TCCR1A = 0;
    TCCR1B = 0;
    OCR1A = 7811;                  // 0.5 с → 1 Гц
    TCCR1B |= (1 << WGM12);
    TCCR1B |= (1 << CS10) | (1 << CS12);  // делитель 1024

    // Timer2 — переполнение ~16 мс
    TCCR2A = 0;
    TCCR2B = 0;
    TCCR2B |= (1 << CS20) | (1 << CS21) | (1 << CS22);  // делитель 1024
    TIMSK2 |= (1 << TOIE2);

    attachInterrupt(0, buttonISR, FALLING);
    sei();
}

void loop() {
}

void buttonISR() {
    if (!running) {
        // --- Запуск ---
        running = true;
        blinkPhase = true;
        secondsCount = 0;
        digitalWrite(LEDPIN, LOW);
        TCNT1 = 0;
        TCNT2 = 0;
        TIMSK1 |= (1 << OCIE1A);       // пуск Timer1
        Serial.println("Запуск. Мигание 1 Гц.");
    } else {
        // --- Стоп ---
        running = false;
        digitalWrite(LEDPIN, LOW);
        TIMSK1 &= ~(1 << OCIE1A);      // стоп Timer1
        TCNT1 = 0;
        TCNT2 = 0;
        secondsCount = 0;
        Serial.println("Стоп. Цикл прерван.");
    }
}

// Timer1 — мигание 1 Гц
ISR(TIMER1_COMPA_vect) {
    if (running && blinkPhase) {
        digitalWrite(LEDPIN, !digitalRead(LEDPIN));
    }
}

// Timer2 — счёт секунд, переключение фаз
ISR(TIMER2_OVF_vect) {
    static unsigned int ovfCount = 0;
    if (running) {
        ovfCount++;
        if (ovfCount >= 61) {              // ~1 секунда
            ovfCount = 0;
            secondsCount++;
            Serial.print("Секунда: ");
            Serial.print(secondsCount);
            Serial.print(" — фаза: ");
            Serial.println(blinkPhase ? "мигание" : "пауза");

            if (secondsCount >= 5) {
                secondsCount = 0;
                if (blinkPhase) {
                    blinkPhase = false;
                    digitalWrite(LEDPIN, LOW);
                    Serial.println("Пауза 5 секунд.");
                } else {
                    blinkPhase = true;
                    TCNT1 = 0;
                    Serial.println("Мигание 5 секунд.");
                }
            }
        }
    }
}