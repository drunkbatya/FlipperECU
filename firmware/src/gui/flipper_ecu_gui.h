#pragma once

typedef struct FlipperECUGui FlipperECUGui;

#include "../flipper_ecu_app_i.h"

FlipperECUGui* flipper_ecu_gui_alloc(FlipperECUApp* ecu_app);
void flipper_ecu_gui_free(FlipperECUGui* app);
void flipper_ecu_gui_start(FlipperECUGui* app);
void flipper_ecu_gui_await_stop(FlipperECUGui* app);
