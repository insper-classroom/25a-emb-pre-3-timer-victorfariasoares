#include "hardware/rtc.h"
#include "pico/util/datetime.h"
#include "pico/stdlib.h"
#include <stdio.h>

const uint LED_PIN = 4;
volatile int fired = 0;

static void alarm_callback(void) {
    fired = 1;
}

int main() {
    stdio_init_all();

    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_put(LED_PIN, 0);

    datetime_t t = {
        .year  = 2024,
        .month = 2,
        .day   = 10,
        .dotw  = 0,  // 0 = domingo
        .hour  = 0,
        .min   = 0,
        .sec   = 0
    };
    rtc_init();
    rtc_set_datetime(&t);

    datetime_t alarm_time = t;
    alarm_time.sec += 5;
    if (alarm_time.sec >= 60) {
        alarm_time.sec -= 60;
        alarm_time.min += 1;
        if (alarm_time.min >= 60) {
            alarm_time.min -= 60;
            alarm_time.hour += 1;
            if (alarm_time.hour >= 24) {
                alarm_time.hour -= 24;
            }
        }
    }
    rtc_set_alarm(&alarm_time, alarm_callback);

    while (true) {
        if (fired) {
            fired = 0;
            gpio_put(LED_PIN, !gpio_get(LED_PIN));

            // pega tempo atual
            datetime_t now;
            rtc_get_datetime(&now);

            // Preparar o proximo na mesma logica
            datetime_t next_alarm = now;
            next_alarm.sec += 5;
            if (next_alarm.sec >= 60) {
                next_alarm.sec -= 60;
                next_alarm.min += 1;
                if (next_alarm.min >= 60) {
                    next_alarm.min -= 60;
                    next_alarm.hour += 1;
                    if (next_alarm.hour >= 24) {
                        next_alarm.hour -= 24;
                    }
                }
            }
            rtc_set_alarm(&next_alarm, alarm_callback);
        }
    }
}
