#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include <stdio.h>

const int BTN_PIN_R = 28;
const int BTN_PIN_G = 26;

const int LED_PIN_R = 4;
const int LED_PIN_G = 6;

volatile int btn_red_apertado = 0;
volatile int btn_green_apertado = 0;

volatile int timer_red_disparou = 0;
volatile int timer_green_disparou = 0;

void btn_callback(uint gpio, uint32_t events) {
    if (events == 0x4) {
        if (gpio == BTN_PIN_R)
            btn_red_apertado = 1;
        else if (gpio == BTN_PIN_G)
            btn_green_apertado = 1;
    }
}

bool timer_red_callback(repeating_timer_t *rt) { // rt é um ponteiro para uma estrutura do tipo repeating_timer_t. Essa estrutura contém informações sobre o timer que foi configurado, como o intervalo de tempo, o estado atual, etc.
    timer_red_disparou = 1;
    return true; // keep repeating
}

bool timer_green_callback(repeating_timer_t *rt) { // rt é um ponteiro para uma estrutura do tipo repeating_timer_t. Essa estrutura contém informações sobre o timer que foi configurado, como o intervalo de tempo, o estado atual, etc.
    timer_green_disparou = 1;
    return true; // keep repeating
}

int main() {
    gpio_init(LED_PIN_R);
    gpio_set_dir(LED_PIN_R, GPIO_OUT);

    gpio_init(LED_PIN_G);
    gpio_set_dir(LED_PIN_G, GPIO_OUT);

    gpio_init(BTN_PIN_R);
    gpio_set_dir(BTN_PIN_R, GPIO_IN);
    gpio_pull_up(BTN_PIN_R);

    gpio_init(BTN_PIN_G);
    gpio_set_dir(BTN_PIN_G, GPIO_IN);
    gpio_pull_up(BTN_PIN_G);

    gpio_set_irq_enabled_with_callback(BTN_PIN_R, GPIO_IRQ_EDGE_FALL, true,
                                       &btn_callback);
    gpio_set_irq_enabled(BTN_PIN_G, GPIO_IRQ_EDGE_FALL, true);

    int timer_red_hz = 2; //Frequencia, quantas vezes por segundo o timer deve disparar? Como queremos 500 ms ent em 1 segundo são duas vezes
    int timer_green_hz = 4;
    int timer_red_rodando = 0;
    int timer_green_rodando = 0;

    repeating_timer_t timer_red; 
    repeating_timer_t timer_green; 

    while (true) {

        if (btn_red_apertado) {
            btn_red_apertado = 0;
            if (!timer_red_rodando){// se timer nao esta rodando vamos add ele
                if (!add_repeating_timer_us(1000000 / timer_red_hz, // Aqui estamos tentando add o timer. A função do timer retorna false se der erro ao add, ent aqui vamos verificar isso
                    timer_red_callback,                             // e ja tratar com um print de erro caso não de certo de add.
                    NULL, 
                    &timer_red)){
                        printf("Failed to add timer red\n");
                    } else {
                        timer_red_rodando = 1; // Se entrou aqui é pq o timer foi adicionado corretamente e podemos considerar que esta rodando, mas quem vai 
                        printf("Hello Timer red\n"); // ativar ele efetivamente é o evento do hardware
                    }
            } else {
                cancel_repeating_timer(&timer_red);
                timer_red_rodando = 0;
                gpio_put(LED_PIN_R, 0);
                printf("Timer red cancelado\n");
            }
        }
        if (timer_red_disparou) {
            timer_red_disparou = 0;
            gpio_put(LED_PIN_R, !gpio_get(LED_PIN_R)); // Como o timer esta configurado para disparar periodicamente, o callback dele seta a flag timer_disparou repetidamente
        }

        if (btn_green_apertado) {
            btn_green_apertado = 0;
            if (!timer_green_rodando){// se timer nao esta rodando vamos add ele
                if (!add_repeating_timer_us(1000000 / timer_green_hz, // Aqui estamos tentando add o timer. A função do timer retorna false se der erro ao add, ent aqui vamos verificar isso
                    timer_green_callback,                             // e ja tratar com um print de erro caso não de certo de add.
                    NULL, 
                    &timer_green)){
                        printf("Failed to add timer green\n");
                    } else {
                        timer_green_rodando = 1; // Se entrou aqui é pq o timer foi adicionado corretamente e podemos considerar que esta rodando, mas quem vai 
                        printf("Hello Timer green\n"); // ativar ele efetivamente é o evento do hardware
                    }
            } else {
                cancel_repeating_timer(&timer_green);
                timer_green_rodando = 0;
                gpio_put(LED_PIN_G, 0);
                printf("Timer green cancelado\n");
            }
        }
        if (timer_green_disparou) {
            timer_green_disparou = 0;
            gpio_put(LED_PIN_G, !gpio_get(LED_PIN_G)); // Como o timer esta configurado para disparar periodicamente, o callback dele seta a flag timer_disparou repetidamente
        }
    }
}
