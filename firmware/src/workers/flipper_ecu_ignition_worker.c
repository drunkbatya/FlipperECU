#include "flipper_ecu_ignition_worker.h"

typedef struct {
    uint8_t test;
} FlipperECUIgnitionWorker;

void flipper_ecu_ignition_worker_thread_send_stop(FuriThread* thread) {
    furi_thread_flags_set(furi_thread_get_id(thread), FlipperECUIgnitionWorkerEventStop);
}

int32_t flipper_ecu_ignition_worker_thread(void* arg) {
    UNUSED(arg);
    FlipperECUIgnitionWorker* app = malloc(sizeof(FlipperECUIgnitionWorker));
    uint32_t events;
    FURI_LOG_I("FlipperECUIgnitionWorker", "thread started");
    while(1) {
        events = furi_thread_flags_wait(
            FlipperECUIgnitionWorkerEventAll, FuriFlagWaitAny, FuriWaitForever);
        if(events & FlipperECUIgnitionWorkerEventStop) {
            break;
        }
    }
    FURI_LOG_I("FlipperECUIgnitionWorker", "thread stopped");
    free(app);
    return 0;
}
