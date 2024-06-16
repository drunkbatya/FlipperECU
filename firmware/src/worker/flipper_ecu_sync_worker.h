#pragma once

#include <stdint.h>
#include <furi.h>
#include "../flipper_ecu_settings.h"
#include "../flipper_ecu_gpio.h"

#define GPIO_EVENTS_MAX_PER_CHANNEL 4

typedef struct {
    bool pin_state;
    const GpioPin* gpio_pin;
    uint32_t compare_value;
    uint32_t next_compare_value;
} GPIOTimerEvent;

typedef struct {
    GPIOTimerEvent queue_oc_1[GPIO_EVENTS_MAX_PER_CHANNEL];
    GPIOTimerEvent queue_oc_2[GPIO_EVENTS_MAX_PER_CHANNEL];
    GPIOTimerEvent queue_oc_3[GPIO_EVENTS_MAX_PER_CHANNEL];
    GPIOTimerEvent queue_oc_4[GPIO_EVENTS_MAX_PER_CHANNEL];

    GPIOTimerEvent* queue_head_oc_1;
    GPIOTimerEvent* queue_head_oc_2;
    GPIOTimerEvent* queue_head_oc_3;
    GPIOTimerEvent* queue_head_oc_4;

    GPIOTimerEvent* queue_tail_oc_1;
    GPIOTimerEvent* queue_tail_oc_2;
    GPIOTimerEvent* queue_tail_oc_3;
    GPIOTimerEvent* queue_tail_oc_4;
} GPIOTimerQueue;

typedef struct {
    FuriThread* thread;
    FlipperECUSettings* settings;
    GPIOTimerQueue qpio_timer_queue;
    uint32_t current_period;
    uint32_t previous_period;
    uint32_t temp;
    uint32_t ckps_timer_overflows;
    bool synced;
} FlipperECUSyncWorker;

typedef enum {
    FlipperECUSyncWorkerEventStop = (1 << 0),
    FlipperECUSyncWorkerEventCkpPulse = (1 << 1),
    FlipperECUSyncWorkerEventPredictionDone = (1 << 2),
    FlipperECUSyncWorkerEventAll = FlipperECUSyncWorkerEventStop |
                                   FlipperECUSyncWorkerEventCkpPulse |
                                   FlipperECUSyncWorkerEventPredictionDone
} FlipperECUSyncWorkerEvent;

FlipperECUSyncWorker* flipper_ecu_sync_worker_alloc(FlipperECUSettings* settings);
void flipper_ecu_sync_worker_send_stop(FlipperECUSyncWorker* worker);
void flipper_ecu_sync_worker_await_stop(FlipperECUSyncWorker* worker);
void flipper_ecu_sync_worker_start(FlipperECUSyncWorker* worker);
void flipper_ecu_sync_worker_free(FlipperECUSyncWorker* worker);
uint32_t flipper_ecu_sync_worker_get_rpm(FlipperECUSyncWorker* worker);
