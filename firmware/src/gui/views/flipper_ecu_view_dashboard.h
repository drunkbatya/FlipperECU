#pragma once

#include <gui/view.h>
#include <stdint.h>

typedef struct FlipperECUDashboardView FlipperECUDashboardView;

FlipperECUDashboardView* flipper_ecu_view_dashboard_alloc(void);
void flipper_ecu_view_dashboard_free(FlipperECUDashboardView* view_dashboard);
void flipper_ecu_view_dashboard_set_rpm(FlipperECUDashboardView* view_dashboard, uint32_t rpm);
View* flipper_ecu_view_dashboard_get_view(FlipperECUDashboardView* view_dashboard);
void flipper_ecu_view_dashboard_view_update(FlipperECUDashboardView* view_dashboard);
