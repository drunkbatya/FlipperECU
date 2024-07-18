#pragma once

#include <gui/view.h>
#include <stdint.h>

#include "../../flipper_ecu_engine_status.h"

typedef struct FlipperECUDashboardView FlipperECUDashboardView;

FlipperECUDashboardView*
    flipper_ecu_view_dashboard_alloc(const FlipperECUEngineStatus* engine_status);
void flipper_ecu_view_dashboard_free(FlipperECUDashboardView* view_dashboard);
View* flipper_ecu_view_dashboard_get_view(FlipperECUDashboardView* view_dashboard);
void flipper_ecu_view_dashboard_view_update(FlipperECUDashboardView* view_dashboard);
