#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include <stdio.h>

const int BTN_PIN_R = 28;
const int LED_PIN_R = 4;

volatile int g_timer_0 = 0;
volatile int flag_f_r = 0;



void btn_callback(uint gpio, uint32_t events) {
    if (events == 0x4) { // fall edge
        flag_f_r = 1;
    } else if (events == 0x8) { // rise edge
    }
}

bool timer_0_callback(repeating_timer_t *rt) {
    g_timer_0 = 1;
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
    
    int timer_0_hz = 2;
    int timer_rodando = 0;
    repeating_timer_t timer_0;

    while (true) {

        if (flag_f_r) {
            flag_f_r = 0;
            if (!timer_rodando){
                if (!add_repeating_timer_us(1000000 / timer_0_hz, 
                    timer_0_callback,
                    NULL, 
                    &timer_0)){
                        printf("Failed to add timer\n");
                    } else {
                        timer_rodando = 1;
                        printf("Hello Timer 0\n");
                    }
            } else {
                cancel_repeating_timer(&timer_0);
                timer_rodando = 0;
                gpio_put(LED_PIN_R, 0);
                printf("Timer cancelado\n");
            }
        }
        if (g_timer_0) {
            g_timer_0 = 0;
            gpio_put(LED_PIN_R, !gpio_get(LED_PIN_R));
        }
        sleep_ms(50);
    }
}
