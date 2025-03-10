#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include <stdio.h>

const int BTN_PIN_R = 28;
const int LED_PIN_R = 4;

volatile int timer_disparou = 0;
volatile int btn_apertado = 0;



void btn_callback(uint gpio, uint32_t events) {
    if (events == 0x4) { // fall edge
        btn_apertado = 1;
    } else if (events == 0x8) { // rise edge
    }
}

bool timer_0_callback(repeating_timer_t *rt) { // rt é um ponteiro para uma estrutura do tipo repeating_timer_t. Essa estrutura contém informações sobre o timer que foi configurado, como o intervalo de tempo, o estado atual, etc.
    timer_disparou = 1;
    return true; // keep repeating
}

int main() {
    stdio_init_all();
    gpio_init(LED_PIN_R);
    gpio_set_dir(LED_PIN_R, GPIO_OUT);
    gpio_put(LED_PIN_R, 0);

    gpio_init(BTN_PIN_R);
    gpio_set_dir(BTN_PIN_R, GPIO_IN);
    gpio_pull_up(BTN_PIN_R);

    gpio_set_irq_enabled_with_callback(BTN_PIN_R, GPIO_IRQ_EDGE_FALL, true,
                                       &btn_callback);
    
    int timer_0_hz = 2; //Frequencia, quantas vezes por segundo o timer deve disparar? Como queremos 500 ms ent em 1 segundo são duas vezes
    int timer_rodando = 0;
    repeating_timer_t timer_0; //Temos que colocar essa variavel porque precisamos referenciar fisicamente nosso timer e passar para a função de 
                              // add o timer que ela tem que colocar os dados do timer no endereço dessa variavel 'timer_0' 

    while (true) {

        if (btn_apertado) {
            btn_apertado = 0;
            if (!timer_rodando){// se timer nao esta rodando vamos add ele
                if (!add_repeating_timer_us(1000000 / timer_0_hz, // Aqui estamos tentando add o timer. A função do timer retorna false se der erro ao add, ent aqui vamos verificar isso
                    timer_0_callback,                             // e ja tratar com um print de erro caso não de certo de add.
                    NULL, 
                    &timer_0)){
                        printf("Failed to add timer\n");
                    } else {
                        timer_rodando = 1; // Se entrou aqui é pq o timer foi adicionado corretamente e podemos considerar que esta rodando, mas quem vai 
                        printf("Hello Timer 0\n"); // ativar ele efetivamente é o evento do hardware
                    }
            } else {
                cancel_repeating_timer(&timer_0);
                timer_rodando = 0;
                gpio_put(LED_PIN_R, 0);
                printf("Timer cancelado\n");
            }
        }
        if (timer_disparou) {
            timer_disparou = 0;
            gpio_put(LED_PIN_R, !gpio_get(LED_PIN_R)); // Como o timer esta configurado para disparar periodicamente, o callback dele seta a flag timer_disparou repetidamente
        }
        sleep_ms(50);
    }
}
