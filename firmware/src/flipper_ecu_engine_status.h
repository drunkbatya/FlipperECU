#pragma once

typedef struct {
    uint16_t rpm;
    int16_t ign_angle;
    bool synced;
    double inj_time;
    double maf_value;
    double maf_adc;
} FlipperECUEngineStatus;
