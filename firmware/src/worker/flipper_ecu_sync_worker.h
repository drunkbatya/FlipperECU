#pragma once

#include <stdint.h>
#include <furi.h>
#include "../flipper_ecu_gpio.h"

#include "../flipper_ecu_engine_config.h"

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

    uint8_t queue_head_oc_1;
    uint8_t queue_head_oc_2;
    uint8_t queue_head_oc_3;
    uint8_t queue_head_oc_4;

    uint8_t queue_tail_oc_1;
    uint8_t queue_tail_oc_2;
    uint8_t queue_tail_oc_3;
    uint8_t queue_tail_oc_4;
} GPIOTimerQueue;

typedef struct {
    FuriThread* thread;
    FlipperECUEngineConfig engine_config;
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

FlipperECUSyncWorker* flipper_ecu_sync_worker_alloc(void);
void flipper_ecu_sync_worker_send_stop(FlipperECUSyncWorker* worker);
void flipper_ecu_sync_worker_await_stop(FlipperECUSyncWorker* worker);
void flipper_ecu_sync_worker_start(FlipperECUSyncWorker* worker);
void flipper_ecu_sync_worker_free(FlipperECUSyncWorker* worker);
uint32_t flipper_ecu_sync_worker_get_rpm(FlipperECUSyncWorker* worker);
const FlipperECUEngineConfig* flipper_ecu_sync_worker_get_config(FlipperECUSyncWorker* worker);
void flipper_ecu_sync_worker_update_config(
    FlipperECUSyncWorker* worker,
    const FlipperECUEngineConfig* config);
