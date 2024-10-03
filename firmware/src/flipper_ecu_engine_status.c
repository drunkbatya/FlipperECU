#include "flipper_ecu_engine_status.h"

static const char* const mode_names[] = {
    [EngineModeIgnitionOff] = "Ignition off",
    [EngineModeStopped] = "Stopped",
    [EngineModeCranking] = "Cranking",
    [EngineModeIdle] = "Idle",
    [EngineModeWorking] = "Working"};

const char* flipper_ecu_engine_status_get_mode_name(const FlipperECUEngineStatus* status) {
    return mode_names[status->mode];
}
