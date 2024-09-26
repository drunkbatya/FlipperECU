#include <furi_hal_gpio.h>

#include "flipper_ecu_fuel_pump_worker_i.h"
#include "../../flipper_ecu_resources.h"

#define TAG "FlipperECUFuelPumpWorker"

static int32_t flipper_ecu_fuel_pump_worker_thread(void* arg) {
    FlipperECUFuelPumpWorker* worker = arg;
    uint32_t events;
    FURI_LOG_I(TAG, "thread started");
    while(1) {
        events = furi_thread_flags_wait(
            FlipperECUFuelPumpWorkerEventAll, FuriFlagWaitAny, FuriWaitForever);
        if(events & FlipperECUFuelPumpWorkerEventStop) {
            break;
        }
        if(events & FlipperECUFuelPumpWorkerEventIgnitionSwitchedOn) {
            furi_hal_gpio_write(gpio_mcu_fuel_pump_out, FUEL_PUMP_ON_LEVEL);
            furi_delay_ms(FUEL_PUMP_POWER_OFF_TIMEOUT_MS);
            if(!worker->engine_status->synced) {
                //furi_hal_gpio_write(gpio_mcu_fuel_pump_out, FUEL_PUMP_OFF_LEVEL);
            }
        }
        if(events & FlipperECUFuelPumpWorkerEventEngineRunning) {
            furi_hal_gpio_write(gpio_mcu_fuel_pump_out, FUEL_PUMP_ON_LEVEL);
        }
        if(events & FlipperECUFuelPumpWorkerEventEngineStopped) {
            //furi_hal_gpio_write(gpio_mcu_fuel_pump_out, FUEL_PUMP_OFF_LEVEL);
        }
        furi_delay_tick(10);
    }
    FURI_LOG_I(TAG, "thread stopped");
    return 0;
}

static void flipper_ecu_fuel_pump_worker_gpio_init(FlipperECUFuelPumpWorker* worker) {
    UNUSED(worker);
    furi_hal_gpio_init_ex(
        gpio_mcu_fuel_pump_out,
        GpioModeOutputPushPull,
        GpioPullDown,
        GpioSpeedVeryHigh,
        GpioAltFnUnused);
    furi_hal_gpio_write(gpio_mcu_fuel_pump_out, 0);
}

static void flipper_ecu_fuel_pump_worker_gpio_deinit(FlipperECUFuelPumpWorker* worker) {
    UNUSED(worker);
    furi_hal_gpio_init_simple(gpio_mcu_fuel_pump_out, GpioModeAnalog);
}

FlipperECUFuelPumpWorker*
    flipper_ecu_fuel_pump_worker_alloc(FlipperECUEngineStatus* engine_status) {
    FlipperECUFuelPumpWorker* worker = malloc(sizeof(FlipperECUFuelPumpWorker));
    worker->thread = furi_thread_alloc_ex(TAG, 1024, flipper_ecu_fuel_pump_worker_thread, worker);
    worker->engine_status = engine_status;
    flipper_ecu_fuel_pump_worker_gpio_init(worker);
    return worker;
}
void flipper_ecu_fuel_pump_worker_start(FlipperECUFuelPumpWorker* worker) {
    furi_thread_start(worker->thread);
    furi_delay_tick(10);
}

void flipper_ecu_fuel_pump_worker_free(FlipperECUFuelPumpWorker* worker) {
    flipper_ecu_fuel_pump_worker_gpio_deinit(worker);
    furi_thread_free(worker->thread);
    free(worker);
}

void flipper_ecu_fuel_pump_worker_send_stop(FlipperECUFuelPumpWorker* worker) {
    furi_thread_flags_set(furi_thread_get_id(worker->thread), FlipperECUFuelPumpWorkerEventStop);
}

void flipper_ecu_fuel_pump_worker_await_stop(FlipperECUFuelPumpWorker* worker) {
    furi_thread_join(worker->thread);
}

void flipper_ecu_fuel_pump_notify_ignition_switched_on(FlipperECUFuelPumpWorker* worker) {
    furi_thread_flags_set(
        furi_thread_get_id(worker->thread), FlipperECUFuelPumpWorkerEventIgnitionSwitchedOn);
}

void flipper_ecu_fuel_pump_notify_ignition_engine_stopped(FlipperECUFuelPumpWorker* worker) {
    furi_thread_flags_set(
        furi_thread_get_id(worker->thread), FlipperECUFuelPumpWorkerEventEngineStopped);
}

void flipper_ecu_fuel_pump_notify_ignition_engine_running(FlipperECUFuelPumpWorker* worker) {
    furi_thread_flags_set(
        furi_thread_get_id(worker->thread), FlipperECUFuelPumpWorkerEventEngineRunning);
}
