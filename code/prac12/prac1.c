#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>

#define LED_R   3
#define SW_R    6

int main(void) {
    if(wiringPiSetup() == -1) {
        puts("Error");
        exit(1);
    }

    pinMode(SW_R, INPUT);
    pinMode(LED_R, OUTPUT);
    digitalWrite(LED_R, 0);

    while(1) {
        if(digitalRead(SW_R) == 0) {
            digitalWrite(LED_R, 1);
        }
        digitalWrite(LED_R, 0);
    }

    return 0;
}