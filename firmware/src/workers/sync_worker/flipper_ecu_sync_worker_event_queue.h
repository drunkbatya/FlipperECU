#pragma once

#define GPIO_QUEUE_ADD(worker, channel, comp, pin, state)                                         \
    if(worker->qpio_timer_queue.queue_head_oc_##channel ==                                        \
       worker->qpio_timer_queue.queue_tail_oc_##channel) {                                        \
        worker->qpio_timer_queue                                                                  \
            .queue_oc_##channel[worker->qpio_timer_queue.queue_head_oc_##channel]                 \
            .next_compare_value = UINT32_MAX;                                                     \
        LL_TIM_OC_SetCompareCH##channel(GPIO_TIMER, comp);                                        \
    } else {                                                                                      \
        worker->qpio_timer_queue                                                                  \
            .queue_oc_##channel[worker->qpio_timer_queue.queue_head_oc_##channel]                 \
            .next_compare_value = UINT32_MAX;                                                     \
        worker->qpio_timer_queue                                                                  \
            .queue_oc_##channel[worker->qpio_timer_queue.queue_head_oc_##channel - 1]             \
            .next_compare_value = comp;                                                           \
    }                                                                                             \
    worker->qpio_timer_queue.queue_oc_##channel[worker->qpio_timer_queue.queue_head_oc_##channel] \
        .pin_state = state;                                                                       \
    worker->qpio_timer_queue.queue_oc_##channel[worker->qpio_timer_queue.queue_head_oc_##channel] \
        .gpio_pin = pin;                                                                          \
    worker->qpio_timer_queue.queue_head_oc_##channel += 1

#define GPIO_QUEUE_GET(worker, channel)                                         \
    &worker->qpio_timer_queue                                                   \
         .queue_oc_##channel[worker->qpio_timer_queue.queue_tail_oc_##channel]; \
    worker->qpio_timer_queue.queue_tail_oc_##channel += 1

#define GPIO_QUEUE_IS_EMPTY(worker, channel)             \
    (worker->qpio_timer_queue.queue_head_oc_##channel == \
     worker->qpio_timer_queue.queue_tail_oc_##channel)

#define GPIO_QUEUE_IS_FULLY_EMPTY(worker)                                \
    (GPIO_QUEUE_IS_EMPTY(worker, 1) && GPIO_QUEUE_IS_EMPTY(worker, 2) && \
     GPIO_QUEUE_IS_EMPTY(worker, 3) && GPIO_QUEUE_IS_EMPTY(worker, 4))

#define GPIO_QUEUE_RESET(worker)                  \
    worker->qpio_timer_queue.queue_head_oc_1 = 0; \
    worker->qpio_timer_queue.queue_tail_oc_1 = 0; \
    worker->qpio_timer_queue.queue_head_oc_2 = 0; \
    worker->qpio_timer_queue.queue_tail_oc_2 = 0; \
    worker->qpio_timer_queue.queue_head_oc_3 = 0; \
    worker->qpio_timer_queue.queue_tail_oc_3 = 0; \
    worker->qpio_timer_queue.queue_head_oc_4 = 0; \
    worker->qpio_timer_queue.queue_tail_oc_4 = 0
