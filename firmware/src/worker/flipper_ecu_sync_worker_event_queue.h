#pragma once

#define GPIO_QUEUE_ADD(channel, comp, pin, state)                                    \
    if(worker->qpio_timer_queue.queue_head_oc_##channel !=                           \
       worker->qpio_timer_queue.queue_tail_oc_##channel) {                           \
        worker->qpio_timer_queue.queue_head_oc_##channel->next_compare_value = comp; \
    } else {                                                                         \
        worker->qpio_timer_queue.queue_head_oc_##channel->next_compare_value = 0;    \
        LL_TIM_OC_SetCompareCH##channel(GPIO_TIMER, comp);                           \
    }                                                                                \
    worker->qpio_timer_queue.queue_head_oc_##channel->pin_state = state;             \
    worker->qpio_timer_queue.queue_head_oc_##channel->gpio_pin = pin;                \
    worker->qpio_timer_queue.queue_head_oc_##channel += 1

#define GPIO_QUEUE_GET(channel)                       \
    worker->qpio_timer_queue.queue_tail_oc_##channel; \
    worker->qpio_timer_queue.queue_tail_oc_##channel += 1

#define GPIO_QUEUE_IS_EMPTY(channel)                     \
    (worker->qpio_timer_queue.queue_head_oc_##channel == \
     worker->qpio_timer_queue.queue_tail_oc_##channel)

#define GPIO_QUEUE_IS_FULLY_EMPTY()                                                \
    (GPIO_QUEUE_IS_EMPTY(1) && GPIO_QUEUE_IS_EMPTY(2) && GPIO_QUEUE_IS_EMPTY(3) && \
     GPIO_QUEUE_IS_EMPTY(4))

#define GPIO_QUEUE_RESET()                                                          \
    worker->qpio_timer_queue.queue_head_oc_1 = worker->qpio_timer_queue.queue_oc_1; \
    worker->qpio_timer_queue.queue_tail_oc_1 = worker->qpio_timer_queue.queue_oc_1; \
    worker->qpio_timer_queue.queue_head_oc_2 = worker->qpio_timer_queue.queue_oc_2; \
    worker->qpio_timer_queue.queue_tail_oc_2 = worker->qpio_timer_queue.queue_oc_2; \
    worker->qpio_timer_queue.queue_head_oc_3 = worker->qpio_timer_queue.queue_oc_3; \
    worker->qpio_timer_queue.queue_tail_oc_3 = worker->qpio_timer_queue.queue_oc_3; \
    worker->qpio_timer_queue.queue_head_oc_4 = worker->qpio_timer_queue.queue_oc_4; \
    worker->qpio_timer_queue.queue_tail_oc_4 = worker->qpio_timer_queue.queue_oc_4
