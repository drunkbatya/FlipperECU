#pragma once

#include "flipper_ecu_sync_worker.h"

#define GPIO_EVENTS_MAX_PER_CHANNEL 8

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

struct FlipperECUSyncWorker {
    FuriThread* thread;
    FlipperECUApp* ecu_app;
    FlipperECUEngineConfig engine_config;
    FlipperECUEngineSettings* engine_settings;
    FlipperECUEngineStatus* engine_status;
    GPIOTimerQueue qpio_timer_queue;
    uint32_t current_period;
    uint32_t previous_period;
    uint32_t temp;
    uint32_t ckps_timer_overflows;
    bool synced;
};

typedef enum {
    FlipperECUSyncWorkerEventStop = (1 << 0),
    FlipperECUSyncWorkerEventEngineStopped = (1 << 1),
    FlipperECUSyncWorkerEventEngineRunningAgain = (1 << 2),
    FlipperECUSyncWorkerEventIgnitionSwitchedOn = (1 << 3),
    FlipperECUSyncWorkerEventAll =
        FlipperECUSyncWorkerEventStop | FlipperECUSyncWorkerEventEngineStopped |
        FlipperECUSyncWorkerEventEngineRunningAgain | FlipperECUSyncWorkerEventIgnitionSwitchedOn
} FlipperECUSyncWorkerEvent;
