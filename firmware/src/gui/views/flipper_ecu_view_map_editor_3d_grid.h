
#pragma once

#include <gui/view.h>
#include <stdint.h>

#include "../../flipper_ecu_map.h"

typedef struct FlipperECUMapEditor3DGridView FlipperECUMapEditor3DGridView;

FlipperECUMapEditor3DGridView* flipper_ecu_view_map_editor_3d_grid_alloc(void);
void flipper_ecu_view_map_editor_3d_grid_free(
    FlipperECUMapEditor3DGridView* view_map_editor_3d_grid);
void flipper_ecu_view_map_editor_3d_grid_set_rpm(
    FlipperECUMapEditor3DGridView* view_map_editor_3d_grid,
    uint32_t rpm);
View* flipper_ecu_view_map_editor_3d_grid_get_view(
    FlipperECUMapEditor3DGridView* view_map_editor_3d_grid);
void flipper_ecu_view_map_editor_3d_grid_view_update(
    FlipperECUMapEditor3DGridView* view_map_editor_3d_grid);
void flipper_ecu_view_map_editor_3d_grid_load_map(
    FlipperECUMapEditor3DGridView* view_map_editor_3d_grid,
    FlipperECUMap* map);
void flipper_ecu_view_map_editor_3d_grid_reset(
    FlipperECUMapEditor3DGridView* view_map_editor_3d_grid);
