#pragma once

#include <furi.h>

#define ADC_BUFFER_SIZE 6

typedef struct {
    FuriThread* thread;
    uint16_t adc_buf[ADC_BUFFER_SIZE];
    float adc_converted_data[ADC_BUFFER_SIZE];
} FlipperECUAdcWorker;

typedef enum {
    FlipperECUAdcWorkerEventStop = (1 << 0),
    FlipperECUAdcWorkerEventDmaDone = (1 << 1),
    FlipperECUAdcWorkerEventDmaTest = (1 << 2),
    FlipperECUAdcWorkerEventAll = FlipperECUAdcWorkerEventStop | FlipperECUAdcWorkerEventDmaDone |
                                  FlipperECUAdcWorkerEventDmaTest
} FlipperECUAdcWorkerEvent;

FlipperECUAdcWorker* flipper_ecu_adc_worker_alloc(void);
void flipper_ecu_adc_worker_start(FlipperECUAdcWorker* worker);
void flipper_ecu_adc_worker_free(FlipperECUAdcWorker* worker);
void flipper_ecu_adc_worker_send_stop(FlipperECUAdcWorker* worker);
void flipper_ecu_adc_worker_await_stop(FlipperECUAdcWorker* worker);
