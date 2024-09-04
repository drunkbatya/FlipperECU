#pragma once

#include <gui/view.h>
#include <stdint.h>

#include "../../flipper_ecu_map.h"

typedef struct FlipperECUMapEditorView FlipperECUMapEditorView;

FlipperECUMapEditorView* flipper_ecu_view_map_editor_alloc(void);
void flipper_ecu_view_map_editor_free(FlipperECUMapEditorView* view_map_editor);
void flipper_ecu_view_map_editor_set_rpm(FlipperECUMapEditorView* view_map_editor, uint32_t rpm);
View* flipper_ecu_view_map_editor_get_view(FlipperECUMapEditorView* view_map_editor);
void flipper_ecu_view_map_editor_view_update(FlipperECUMapEditorView* view_map_editor);
void flipper_ecu_view_map_editor_load_map(
    FlipperECUMapEditorView* view_map_editor,
    FlipperECUMap* map);
void flipper_ecu_view_map_editor_reset(FlipperECUMapEditorView* view_map_editor);
