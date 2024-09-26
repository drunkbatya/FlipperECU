#pragma once

#include "flipper_ecu_adc_worker.h"

#define ADC_BUFFER_SIZE 8

struct FlipperECUAdcWorker {
    FuriThread* thread;
    FlipperECUApp* ecu_app;
    uint16_t adc_buf[ADC_BUFFER_SIZE];
    double adc_converted_data[ADC_BUFFER_SIZE];
    double adc_converted_data_full[ADC_BUFFER_SIZE];
    bool first_measurement_done;
};

typedef enum {
    FlipperECUAdcWorkerEventStop = (1 << 0),
    FlipperECUAdcWorkerEventDmaDone = (1 << 1),
    FlipperECUAdcWorkerEventDmaTest = (1 << 2),
    FlipperECUAdcWorkerEventAll = FlipperECUAdcWorkerEventStop | FlipperECUAdcWorkerEventDmaDone |
                                  FlipperECUAdcWorkerEventDmaTest
} FlipperECUAdcWorkerEvent;
