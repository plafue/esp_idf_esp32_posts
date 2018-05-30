/*
Code is adapted from 

https://github.com/nkolban/esp32-snippets/blob/master/hardware/servos/servoSweep.c

*/

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/ledc.h"
#include "sdkconfig.h"
#include "esp_log.h"

static char tag[] = "servo1";

void sweepServo_task(void *ignore) {
	int bitSize         = 15;
	int minValue        = 500;  // micro seconds (uS)
	int maxValue        = 2500; // micro seconds (uS)
	int sweepDuration   = 1500; // milliseconds (ms)
	int duty            = (1<<bitSize) * minValue / 20000 ;//1638
	int direction       = 1; // 1 = up, -1 = down
	int valueChangeRate = 20; // msecs

	ESP_LOGD(tag, ">> task_servo1");
	ledc_timer_config_t timer_conf;
	timer_conf.bit_num    = LEDC_TIMER_15_BIT;
	timer_conf.freq_hz    = 50;
	timer_conf.speed_mode = LEDC_HIGH_SPEED_MODE;
	timer_conf.timer_num  = LEDC_TIMER_0;
	ledc_timer_config(&timer_conf);

	ledc_channel_config_t ledc_conf;
	ledc_conf.channel    = LEDC_CHANNEL_0;
	ledc_conf.duty       = duty;
	ledc_conf.gpio_num   = 16;
	ledc_conf.intr_type  = LEDC_INTR_DISABLE;
	ledc_conf.speed_mode = LEDC_HIGH_SPEED_MODE;
	ledc_conf.timer_sel  = LEDC_TIMER_0;
	ledc_channel_config(&ledc_conf);

	int changesPerSweep = sweepDuration / valueChangeRate;// 1500/20 -> 75
	int changeDelta = (maxValue-minValue) / changesPerSweep;// 2000/75 -> 26
	int i;
	ESP_LOGD(tag, "sweepDuration: %d seconds", sweepDuration);
	ESP_LOGD(tag, "changesPerSweep: %d", changesPerSweep);
	ESP_LOGD(tag, "changeDelta: %d", changeDelta);
	ESP_LOGD(tag, "valueChangeRate: %d", valueChangeRate);
	while(1) {
		for (i=0; i<changesPerSweep; i++) {
			if (direction > 0) {
				duty += changeDelta;
			} else {
				duty -= changeDelta;
			}
			ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, duty);
			ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0);
			vTaskDelay(valueChangeRate/portTICK_PERIOD_MS);
		}
		direction = -direction;
		ESP_LOGD(tag, "Direction now %d", direction);
	} // End loop forever

	vTaskDelete(NULL);
}

void app_main()
{	
    
        xTaskCreate(&sweepServo_task,"sweepServo_task",2048,NULL,5,NULL);
        printf("servo sweep task  started\n");
}
