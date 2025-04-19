#pragma once

typedef struct FlipperECUGui FlipperECUGui;

#include "../flipper_ecu_app_i.h"

// need to alloc gui thread in main thread for proper event loop work
// because of a lot of `furi_check(instance->thread_id == furi_thread_get_current_id());`
// in `flipperzero-firmware/furi/core/event_loop.c`
int32_t flipper_ecu_gui_thread(void* arg);
