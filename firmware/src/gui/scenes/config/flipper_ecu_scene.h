#pragma once

#include <gui/scene_manager.h>

// Generate scene id and total number
#define ADD_SCENE(prefix, name, id) FlipperECUScene##id,
typedef enum {
#include "flipper_ecu_scene_config.h"
    FlipperECUSceneNum,
} FlipperECUScene;
#undef ADD_SCENE

extern const SceneManagerHandlers flipper_ecu_scene_handlers;

// Generate scene on_enter handlers declaration
#define ADD_SCENE(prefix, name, id) void prefix##_scene_##name##_on_enter(void*);
#include "flipper_ecu_scene_config.h"
#undef ADD_SCENE

// Generate scene on_event handlers declaration
#define ADD_SCENE(prefix, name, id) \
    bool prefix##_scene_##name##_on_event(void* context, SceneManagerEvent event);
#include "flipper_ecu_scene_config.h"
#undef ADD_SCENE

// Generate scene on_exit handlers declaration
#define ADD_SCENE(prefix, name, id) void prefix##_scene_##name##_on_exit(void* context);
#include "flipper_ecu_scene_config.h"
#undef ADD_SCENE