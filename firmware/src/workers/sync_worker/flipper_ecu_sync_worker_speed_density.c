#include "flipper_ecu_sync_worker_speed_density.h"
#include "flipper_ecu_sync_worker_air_temp.h"

/**
 * Derived via:
 * (8.31 J K mol^-1)  <- ideal gas constant R
 * /
 * (28.97g mol^-1)    <- molar mass of air
 * = 0.28705 J*K/g
 */
#define AIR_R (double)0.28705f

#define KELV (double)273.15f
#define convertCelsiusToKelvin(tempC) ((tempC) + KELV)

static double ideal_gas_law(double volume, double pressure, double temperature) {
    return volume * pressure / (AIR_R * temperature);
}

inline static double flipper_ecu_sync_worker_speed_density_get_air_mass_per_cycle(
    FlipperECUEngineSettings* engine_settings,
    double ve,
    double manifold_pressure,
    double temperature) {
    double cycleAir =
        ve * ideal_gas_law(engine_settings->engine_displacement, manifold_pressure, temperature);
    return cycleAir / engine_settings->cylinders_count;
}

double flipper_ecu_sync_worker_speed_density_get_inj_time(FlipperECUSyncWorker* worker) {
    FlipperECUAdcWorker* adc_worker = flipper_ecu_app_get_adc_worker(worker->ecu_app);
    const double manifold_pressure = flipper_ecu_adc_worker_get_value_map_full(adc_worker);
    worker->engine_status->manifold_pressure = manifold_pressure;
    const double water_temp = flipper_ecu_adc_worker_get_value_water_temp_full(adc_worker);
    const double air_temp = flipper_ecu_adc_worker_get_value_air_temp_full(adc_worker);
    const double rpm = worker->engine_status->rpm;
    const double tps = flipper_ecu_adc_worker_get_value_tps_full(adc_worker);
    double temperature =
        flipper_ecu_sync_worker_air_temp_get_t_charge(water_temp, air_temp, rpm, tps);
    worker->engine_status->t_charge = temperature;
    temperature = convertCelsiusToKelvin(temperature);
    const double ve = flipper_ecu_map_interpolate_3d(
                          worker->engine_settings->maps[VE], rpm, manifold_pressure * (double)10) /
                      (double)1000;
    worker->engine_status->ve = ve;
    const double afr = 14.7;

    const double air_mass = flipper_ecu_sync_worker_speed_density_get_air_mass_per_cycle(
        worker->engine_settings, ve, manifold_pressure, temperature);
    worker->engine_status->air_mass = air_mass;
    double inj_time = air_mass / afr / ((double)1 / worker->engine_settings->inj_flow);

    return inj_time;
}
