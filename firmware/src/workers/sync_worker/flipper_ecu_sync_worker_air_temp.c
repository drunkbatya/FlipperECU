#include "flipper_ecu_sync_worker_air_temp.h"

#define rpmMin 500
#define rpmMax 8000

#define tps_min 0
#define tps_max 100

const float k_min_rpm_min_tp = 0.25;
const float k_max_rpm_min_tp = 0.25;
const float k_min_rpm_max_tp = 0.25;
const float k_max_rpm_max_tp = 0.9;

static float flipper_ecu_sync_worker_air_temp_interpolate(
    float key1,
    float value1,
    float key2,
    float value2,
    float key) {
    // a*key1 + b = y1
    // a*key2 + b = y2
    float a = (value1 - value2) / (key1 - key2);
    float b = value1 - a * key1;
    return a * key + b;
}

float flipper_ecu_sync_worker_air_temp_get_t_charge_coefficient(float rpm, float tps) {
    float rpm_min_k = flipper_ecu_sync_worker_air_temp_interpolate(
        tps_min, k_min_rpm_min_tp, tps_max, k_min_rpm_max_tp, tps);
    float rpm_max_k = flipper_ecu_sync_worker_air_temp_interpolate(
        tps_min, k_max_rpm_min_tp, tps_max, k_max_rpm_max_tp, tps);

    float t_charge_coff =
        flipper_ecu_sync_worker_air_temp_interpolate(rpmMin, rpm_min_k, rpmMax, rpm_max_k, rpm);
    return t_charge_coff;
}

float flipper_ecu_sync_worker_air_temp_get_t_charge(
    float water_temp,
    float air_temp,
    float rpm,
    float tps) {
    float t_charge_coff = flipper_ecu_sync_worker_air_temp_get_t_charge_coefficient(rpm, tps);

    // Interpolate between CLT and IAT:
    // 0.0 coefficient -> use CLT (full heat transfer)
    // 1.0 coefficient -> use IAT (no heat transfer)
    float t_charge = water_temp * (1 - t_charge_coff) + air_temp * t_charge_coff;
    return t_charge;
}
