#pragma once

float flipper_ecu_sync_worker_air_temp_get_t_charge_coefficient(float rpm, float tps);
float flipper_ecu_sync_worker_air_temp_get_t_charge(
    float water_temp,
    float air_temp,
    float rpm,
    float tps);
