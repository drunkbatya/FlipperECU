#include "flipper_ecu_scene.h"

// Generate scene on_enter handlers array
#define ADD_SCENE(prefix, name, id) prefix##_scene_##name##_on_enter,
void (*const flipper_ecu_on_enter_handlers[])(void*) = {
#include "flipper_ecu_scene_config.h"
};
#undef ADD_SCENE

// Generate scene on_event handlers array
#define ADD_SCENE(prefix, name, id) prefix##_scene_##name##_on_event,
bool (*const flipper_ecu_on_event_handlers[])(void* context, SceneManagerEvent event) = {
#include "flipper_ecu_scene_config.h"
};
#undef ADD_SCENE

// Generate scene on_exit handlers array
#define ADD_SCENE(prefix, name, id) prefix##_scene_##name##_on_exit,
void (*const flipper_ecu_on_exit_handlers[])(void* context) = {
#include "flipper_ecu_scene_config.h"
};
#undef ADD_SCENE

// Initialize scene handlers configuration structure
const SceneManagerHandlers flipper_ecu_scene_handlers = {
    .on_enter_handlers = flipper_ecu_on_enter_handlers,
    .on_event_handlers = flipper_ecu_on_event_handlers,
    .on_exit_handlers = flipper_ecu_on_exit_handlers,
    .scene_num = FlipperECUSceneNum,
};
