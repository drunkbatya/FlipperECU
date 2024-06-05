#pragma once

typedef struct FlipperECUGui FlipperECUGui;

FlipperECUGui* flipper_ecu_gui_alloc(void* main_app);
void flipper_ecu_gui_free(FlipperECUGui* app);
void flipper_ecu_gui_start(FlipperECUGui* app);
void flipper_ecu_gui_await_stop(FlipperECUGui* app);
