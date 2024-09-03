#include "flipper_ecu_adc_worker.h"

#include <furi_hal_bus.h>
#include <furi_hal_adc.h>
#include <furi_hal_interrupt.h>
#include <furi_hal_cortex.h>

#include <stm32wbxx_ll_adc.h>
#include <stm32wbxx_ll_dma.h>
#include <stm32wbxx_ll_dmamux.h>
#include <stm32wbxx_ll_system.h>

#include "../../flipper_ecu_resources.h"

#define TAG "FlipperECUAdcWorker"

static void flipper_ecu_adc_worker_dma_callback(void* context) {
    FlipperECUAdcWorker* worker = context;

    if(LL_DMA_IsActiveFlag_TC1(DMA2)) {
        LL_DMA_ClearFlag_TC1(DMA2);

        furi_thread_flags_set(furi_thread_get_id(worker->thread), FlipperECUAdcWorkerEventDmaDone);
    }
}

static void flipper_ecu_adc_worker_dma_init(FlipperECUAdcWorker* worker) {
    LL_DMA_InitTypeDef init_struct = {0};
    init_struct.PeriphOrM2MSrcAddress = LL_ADC_DMA_GetRegAddr(ADC1, LL_ADC_DMA_REG_REGULAR_DATA);
    init_struct.MemoryOrM2MDstAddress = (uint32_t) & (worker->adc_buf[0]);
    init_struct.Direction = LL_DMA_DIRECTION_PERIPH_TO_MEMORY;
    init_struct.Mode = LL_DMA_MODE_CIRCULAR;
    init_struct.PeriphOrM2MSrcIncMode = LL_DMA_PERIPH_NOINCREMENT;
    init_struct.MemoryOrM2MDstIncMode = LL_DMA_MEMORY_INCREMENT;
    init_struct.PeriphOrM2MSrcDataSize = LL_DMA_PDATAALIGN_HALFWORD;
    init_struct.MemoryOrM2MDstDataSize = LL_DMA_MDATAALIGN_HALFWORD;
    init_struct.NbData = gpio_adc_pins_size;
    init_struct.PeriphRequest = LL_DMAMUX_REQ_ADC1;
    init_struct.Priority = LL_DMA_PRIORITY_HIGH;
    furi_check(LL_DMA_Init(DMA2, LL_DMA_CHANNEL_1, &init_struct) == SUCCESS);

    furi_hal_interrupt_set_isr(
        FuriHalInterruptIdDma2Ch1, flipper_ecu_adc_worker_dma_callback, worker);

    //LL_DMA_EnableIT_HT(DMA2, LL_DMA_CHANNEL_1);
    LL_DMA_EnableIT_TC(DMA2, LL_DMA_CHANNEL_1);

    LL_DMA_EnableChannel(DMA2, LL_DMA_CHANNEL_1);
    FURI_LOG_I(TAG, "DMA configured");
}

static void flipper_ecu_adc_worker_dma_deinit(FlipperECUAdcWorker* worker) {
    UNUSED(worker);
    furi_hal_interrupt_set_isr(FuriHalInterruptIdDma2Ch1, NULL, NULL);
    LL_DMA_DeInit(DMA2, LL_DMA_CHANNEL_1);
}

static void flipper_ecu_adc_worker_adc_pins_init(FlipperECUAdcWorker* worker) {
    UNUSED(worker);
    for(uint8_t index = 0; index < gpio_adc_pins_size; index++) {
        furi_hal_gpio_init(gpio_adc_pins[index].pin, GpioModeAnalog, GpioPullNo, GpioSpeedLow);
        LL_ADC_SetChannelSamplingTime(
            ADC1, gpio_adc_pins[index].channel, LL_ADC_SAMPLINGTIME_247CYCLES_5);
        LL_ADC_REG_SetSequencerRanks(
            ADC1, gpio_adc_pins[index].rank, gpio_adc_pins[index].channel);
        LL_ADC_SetChannelSingleDiff(ADC1, gpio_adc_pins[index].channel, LL_ADC_SINGLE_ENDED);
    }
}

static void flipper_ecu_adc_worker_adc_init(FlipperECUAdcWorker* worker) {
    FuriHalCortexTimer timer;
    if(furi_hal_bus_is_enabled(FuriHalBusADC)) furi_hal_bus_disable(FuriHalBusADC);
    LL_VREFBUF_SetVoltageScaling(LL_VREFBUF_VOLTAGE_SCALE0);
    LL_VREFBUF_SetTrimming(LL_VREFBUF_SC0_GetCalibration() & 0x3FU);

    LL_VREFBUF_Enable();
    LL_VREFBUF_DisableHIZ();

    timer = furi_hal_cortex_timer_get(500000); // 500ms to stabilize VREF

    while(!LL_VREFBUF_IsVREFReady()) {
        furi_check(!furi_hal_cortex_timer_is_expired(timer), "VREF fail");
    };

    furi_hal_bus_enable(FuriHalBusADC);

    LL_ADC_CommonInitTypeDef common_init_struct = {0};
    common_init_struct.CommonClock = LL_ADC_CLOCK_SYNC_PCLK_DIV1;
    furi_check(LL_ADC_CommonInit(ADC1_COMMON, &common_init_struct) == SUCCESS);

    LL_ADC_InitTypeDef init_struct = {0};
    init_struct.Resolution = LL_ADC_RESOLUTION_12B;
    init_struct.DataAlignment = LL_ADC_DATA_ALIGN_RIGHT;
    init_struct.LowPowerMode = LL_ADC_LP_MODE_NONE;
    furi_check(LL_ADC_Init(ADC1, &init_struct) == SUCCESS);

    LL_ADC_REG_InitTypeDef reg_init_struct = {0};
    reg_init_struct.TriggerSource = LL_ADC_REG_TRIG_SOFTWARE;
    reg_init_struct.SequencerLength = LL_ADC_REG_SEQ_SCAN_ENABLE_6RANKS;
    reg_init_struct.SequencerDiscont = LL_ADC_REG_SEQ_DISCONT_DISABLE;
    reg_init_struct.ContinuousMode = LL_ADC_REG_CONV_SINGLE;
    reg_init_struct.DMATransfer = LL_ADC_REG_DMA_TRANSFER_UNLIMITED;
    reg_init_struct.Overrun = LL_ADC_REG_OVR_DATA_OVERWRITTEN;
    furi_check(LL_ADC_REG_Init(ADC1, &reg_init_struct) == SUCCESS);

    LL_ADC_SetOverSamplingScope(ADC1, LL_ADC_OVS_GRP_REGULAR_CONTINUED);
    LL_ADC_ConfigOverSamplingRatioShift(ADC1, LL_ADC_OVS_RATIO_64, LL_ADC_OVS_SHIFT_RIGHT_6);

    flipper_ecu_adc_worker_adc_pins_init(worker); // must be in this place

    LL_ADC_DisableDeepPowerDown(ADC1);
    LL_ADC_EnableInternalRegulator(ADC1);

    timer = furi_hal_cortex_timer_get(LL_ADC_DELAY_INTERNAL_REGUL_STAB_US);
    while(!furi_hal_cortex_timer_is_expired(timer))
        ;

    // Run ADC self calibration
    LL_ADC_StartCalibration(ADC1, LL_ADC_SINGLE_ENDED);
    // Poll for ADC effectively calibrated
    while(LL_ADC_IsCalibrationOnGoing(ADC1) != 0)
        ;
    // Delay between ADC end of calibration and ADC enable
    const size_t end = DWT->CYCCNT + LL_ADC_DELAY_CALIB_ENABLE_ADC_CYCLES;
    while(DWT->CYCCNT < end)
        ;

    // Enable ADC
    LL_ADC_ClearFlag_ADRDY(ADC1);
    LL_ADC_Enable(ADC1);
    while(!LL_ADC_IsActiveFlag_ADRDY(ADC1))
        ;

    LL_ADC_REG_StartConversion(ADC1);
    FURI_LOG_I(TAG, "ADC configured");
}

static void flipper_ecu_adc_worker_adc_deinit(FlipperECUAdcWorker* worker) {
    UNUSED(worker);
    if(furi_hal_bus_is_enabled(FuriHalBusADC)) furi_hal_bus_disable(FuriHalBusADC);
    LL_VREFBUF_Disable();
    LL_VREFBUF_EnableHIZ();
}

static void flipper_ecu_adc_worker_convert_data_to_voltage(FlipperECUAdcWorker* worker) {
    for(uint8_t index = 0; index < gpio_adc_pins_size; index++) {
        worker->adc_converted_data[index] = (float)__LL_ADC_CALC_DATA_TO_VOLTAGE(
            2048, worker->adc_buf[index], LL_ADC_RESOLUTION_12B);
    }
}

static int32_t flipper_ecu_adc_worker_thread(void* arg) {
    FlipperECUAdcWorker* worker = arg;
    UNUSED(worker);
    UNUSED(flipper_ecu_adc_worker_convert_data_to_voltage);
    uint32_t events;
    FuriString* fstr = furi_string_alloc();
    FURI_LOG_I(TAG, "thread started");
    while(1) {
        events =
            furi_thread_flags_wait(FlipperECUAdcWorkerEventAll, FuriFlagWaitAny, FuriWaitForever);
        if(events & FlipperECUAdcWorkerEventStop) {
            break;
        }
        if(events & FlipperECUAdcWorkerEventDmaDone) {
            //flipper_ecu_adc_worker_convert_data_to_voltage(worker);
            //FURI_LOG_I(TAG, "DMA done");
            //furi_string_printf(
            //    fstr,
            //    "buf[0]: %4.0f, buf[1]: %4.0f, buf[2]: %4.0f",
            //    (double)worker->adc_converted_data[0],
            //    (double)worker->adc_converted_data[1],
            //    (double)worker->adc_converted_data[2]);
            //FURI_LOG_I(TAG, furi_string_get_cstr(fstr));
            //furi_string_printf(
            //    fstr,
            //    "buf[3]: %4.0f, buf[4]: %4.0f, buf[5]: %4.0f",
            //    (double)worker->adc_converted_data[3],
            //    (double)worker->adc_converted_data[4],
            //    (double)worker->adc_converted_data[5]);
            //FURI_LOG_I(TAG, furi_string_get_cstr(fstr));
            //LL_ADC_REG_StartConversion(ADC1);
        }
        furi_delay_tick(10);
    }
    furi_string_free(fstr);
    FURI_LOG_I(TAG, "thread stopped");
    return 0;
}

FlipperECUAdcWorker* flipper_ecu_adc_worker_alloc(void) {
    FlipperECUAdcWorker* worker = malloc(sizeof(FlipperECUAdcWorker));
    worker->thread = furi_thread_alloc_ex(TAG, 1024, flipper_ecu_adc_worker_thread, worker);
    return worker;
}
void flipper_ecu_adc_worker_start(FlipperECUAdcWorker* worker) {
    furi_thread_start(worker->thread);
    furi_delay_tick(10);
    flipper_ecu_adc_worker_dma_init(worker);
    flipper_ecu_adc_worker_adc_init(worker);
}

void flipper_ecu_adc_worker_free(FlipperECUAdcWorker* worker) {
    furi_thread_free(worker->thread);
    free(worker);
}

void flipper_ecu_adc_worker_send_stop(FlipperECUAdcWorker* worker) {
    // there is no gpio deinit function because analog mode is a system default state
    flipper_ecu_adc_worker_dma_deinit(worker);
    flipper_ecu_adc_worker_adc_deinit(worker);
    furi_thread_flags_set(furi_thread_get_id(worker->thread), FlipperECUAdcWorkerEventStop);
}

void flipper_ecu_adc_worker_await_stop(FlipperECUAdcWorker* worker) {
    furi_thread_join(worker->thread);
}
