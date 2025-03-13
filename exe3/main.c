#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include <stdio.h>

const int BTN_PIN_R = 28;
const int LED_PIN_R = 4;

volatile int btn_apertado = 0;
volatile int btn_solto = 0;

absolute_time_t press_time;

void btn_callback(uint gpio, uint32_t events) {
    if (events == 0x4) { // fall edge
        if (gpio == BTN_PIN_R){
            btn_apertado = 1;
            press_time = get_absolute_time();
        }
    } else if (events == 0x8) { // rise edge
        if (gpio == BTN_PIN_R){
            btn_solto = 1;
        }
    }
}

int main() {
    stdio_init_all();

    gpio_init(LED_PIN_R);
    gpio_set_dir(LED_PIN_R, GPIO_OUT);

    gpio_init(BTN_PIN_R);
    gpio_set_dir(BTN_PIN_R, GPIO_IN);
    gpio_pull_up(BTN_PIN_R);

    gpio_set_irq_enabled_with_callback(
        BTN_PIN_R, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true, &btn_callback);

    while (true) {

        if (btn_solto) {
            btn_solto = 0;

            if (btn_apertado) {
                int tempo_atual = get_absolute_time();
                int diferenca = tempo_atual - press_time;
                
                if (diferenca >= 500000) {
                    gpio_put(LED_PIN_R, !gpio_get(LED_PIN_R));
                }
                btn_apertado = 0;
            }
        }
        sleep_ms(10);
    }
}
