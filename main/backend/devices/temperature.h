#ifndef __TEMPERATURE_H__
#define __TEMPERATURE_H__

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"


#define ADS1115_SPEED (100000)

#ifdef __cplusplus
extern "C" {
#endif

void temperature_task(void* args);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif