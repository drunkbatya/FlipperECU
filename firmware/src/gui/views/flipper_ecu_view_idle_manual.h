#pragma once

#include <gui/view.h>
#include <stdint.h>

#include "../../flipper_ecu_app.h"

#include "../../flipper_ecu_engine_status.h"

typedef struct FlipperECUIdleManualView FlipperECUIdleManualView;

FlipperECUIdleManualView* flipper_ecu_view_idle_manual_alloc(FlipperECUApp* ecu_app);
void flipper_ecu_view_idle_manual_free(FlipperECUIdleManualView* view_idle_manual);
View* flipper_ecu_view_idle_manual_get_view(FlipperECUIdleManualView* view_idle_manual);
void flipper_ecu_view_idle_manual_view_update(FlipperECUIdleManualView* view_idle_manual);
void flipper_ecu_view_idle_manual_reset(FlipperECUIdleManualView* view_idle_manual);
