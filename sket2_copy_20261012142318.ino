#include <avr/io.h>
#include <avr/interrupt.h>
#define LEDPIN 13

void setup() {
    pinMode(LEDPIN, OUTPUT);
    cli();                          // отключить глобальные прерывания
    TCCR1A = 0;
    TCCR1B = 0;
    OCR1A = 7811;                   // переключение каждые 0.5 с → 1 Гц
    TCCR1B |= (1 << WGM12);         // стс
    TCCR1B |= (1 << CS10);          // делитель 1024
    TCCR1B |= (1 << CS12);
    TIMSK1 |= (1 << OCIE1A);        // прерывание по совпадению
    sei();                          // включить глобальные прерывания
}

void loop() {
    // пусто — таймер работает сам
}

ISR(TIMER1_COMPA_vect) {
    digitalWrite(LEDPIN, !digitalRead(LEDPIN));   // мигаем
}