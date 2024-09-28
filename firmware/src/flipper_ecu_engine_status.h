#pragma once

typedef struct {
    uint16_t rpm;
    int16_t ign_angle;
    bool synced;
    double inj_time;
    double inj_dead_time;
    uint32_t sync_processing_time;
    uint32_t first_queue_event_ticks;
} FlipperECUEngineStatus;
