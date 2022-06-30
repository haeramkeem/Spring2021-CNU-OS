#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#define WHITE   0
#define RED     1
#define YELLOW  2
#define GREEN   3
#define LED_R   3
#define LED_Y   2
#define LED_G   0
#define SW_R    6
#define SW_Y    5
#define SW_G    4
#define SW_W    27

void *solve_controller();
void *submit_controller();
int answer_is_wrong();
void set_thread();
int color_to_led();
void show_question();
void set_question();
void init();
void blink();
void wrong_blink();

int question[5] = {0,};
int answer[5] = {0,};
int answer_tail = 0;
int n = 1;
sem_t for_answer;

typedef struct {
    int color_code;
    int led;
    int sw;
} Color;

int main(void) {
    int status;
    pthread_t button_thread[4];
    Color color[3] = {
        {RED, LED_R, SW_R},
        {YELLOW, LED_Y, SW_Y},
        {GREEN, LED_G, SW_G}
    };

    init();
    blink();

    set_thread(&button_thread[WHITE], submit_controller, NULL);
    set_thread(&button_thread[RED], solve_controller, &color[RED - 1]);
    set_thread(&button_thread[YELLOW], solve_controller, &color[YELLOW - 1]);
    set_thread(&button_thread[GREEN], solve_controller, &color[GREEN - 1]);

    for(n = 1; n < 6; n++) {
        set_question();
        show_question();
        sem_wait(&for_answer);
    }

    pthread_join(button_thread[WHITE], (void*)&status);
    pthread_join(button_thread[RED], (void*)&status);
    pthread_join(button_thread[YELLOW], (void*)&status);
    pthread_join(button_thread[GREEN], (void*)&status);

    sem_destroy(&for_answer);

    if(n == 6) {
        blink();
    } else {
        wrong_blink();
    }

    return 0;
}

void *solve_controller(Color *color) {
    int status = 0;

    while(1) {
        if(n > 5) {
            return NULL;
        }
        if(digitalRead((*color).sw) == 0) {
            digitalWrite((*color).led, 1);
            delay(100);
            digitalWrite((*color).led, 0);
            status = 1;
        } else {
            if(status == 1 && answer_tail < n) {
                answer[answer_tail++] = (*color).color_code;
                status = 0;
            }
        }
    }
}

void *submit_controller() {
    int status = 0;

    while(1) {
        delay(100);
        if(n > 5) {
            return NULL;
        }
        if(digitalRead(SW_W) == 0) {
            status = 1;
        } else {
            if(status == 1) {
                if(answer_is_wrong()) {
                    n = 7;
                } else {
                    answer_tail = 0;
                    status = 0;
                }
                sem_post(&for_answer);
            }
        }
    }
}

int answer_is_wrong() {
    int i;
    for(i = 0; i < n; i++) {
        if(question[i] != answer[i]) {
            return 1;
        }
    }
    return 0;
}

void set_thread(pthread_t *pt, void* func, int *arg) {
    if(pthread_create(pt, NULL, func, arg) < 0) {
        perror("Thread create error : ");
        exit(1);
    }
}

int color_to_led(int color) {
    if(color == RED) {
        return LED_R;
    } else if(color == YELLOW) {
        return LED_Y;
    } else if(color == GREEN) {
        return LED_G;
    } else {
        puts("Convert Error");
        exit(1);
    }
}

void show_question() {
    int i;
    
    for(i = 0; i < n; i++) {
        delay(250);
        digitalWrite(color_to_led(question[i]), 1);
        delay(250);
        digitalWrite(color_to_led(question[i]), 0);
    }
}

void set_question() {
    int i;

    srand(time(NULL));
    for(i = 0; i < n; i++) {
        question[i] = (rand()%3 + 1);
    }
}

void init(void) {
    if(wiringPiSetup() == -1) {
        puts("Setup Fail");
        exit(1);
    }
    pinMode(SW_R, INPUT);
    pinMode(SW_Y, INPUT);
    pinMode(SW_G, INPUT);
    pinMode(SW_W, INPUT);
    pinMode(LED_R, OUTPUT);
    pinMode(LED_Y, OUTPUT);
    pinMode(LED_G, OUTPUT);

    digitalWrite(LED_R, 0);
    digitalWrite(LED_Y, 0);
    digitalWrite(LED_G, 0);

    sem_init(&for_answer, 0, 0);
}

void blink(void) {
    int i;

    for(i = 0; i < 3; i++) {
        digitalWrite(LED_R, 1);
        delay(250);
        digitalWrite(LED_R, 0);

        digitalWrite(LED_Y, 1);
        delay(250);
        digitalWrite(LED_Y, 0);

        digitalWrite(LED_G, 1);
        delay(250);
        digitalWrite(LED_G, 0);
    }
}

void wrong_blink() {
    int i;

    for(i = 0; i < 3; i++) {
        digitalWrite(LED_R, 1);
        digitalWrite(LED_Y, 1);
        digitalWrite(LED_G, 1);
        delay(250);
        digitalWrite(LED_R, 0);
        digitalWrite(LED_Y, 0);
        digitalWrite(LED_G, 0);
        delay(250);
    }
}