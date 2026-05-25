#include "main.h"
#include "state.h"
#include <string.h>

extern int state;
extern UART_HandleTypeDef huart1;

static void State_Idle(void)
{
    static uint8_t  debouncing = 0;
    static uint32_t press_tick = 0;
    if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_1) == GPIO_PIN_RESET)
    {
        // 按键按下
        if (!debouncing) {
            press_tick = HAL_GetTick();   // 记录首次检测到按下的时刻
                debouncing = 1;
            }
        else if (HAL_GetTick() - press_tick >= 500)
            {
                // 长按确认：从第一次检测到按下到现在已超过 500ms
                state = state_long;
                debouncing = 0;
            }
            // else: 还在消抖或等待中，啥也不做，下一轮再来
        }
        else
        {
            // 按键松开
            if (debouncing)
            {
                uint32_t elapsed = HAL_GetTick() - press_tick;

                // 只有消抖通过（≥ 10ms）才视为有效短按
                if (elapsed >= 10 && elapsed < 500)
                {
                    state = state_short;
                }
                // 否则 (< 10ms) 是抖动，直接忽略

                debouncing = 0;
            }
        }
}

static void State_ShortPress(void)
{
    //开关led
    HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
    //切换到空闲状态
    state = state_idle;
}

static void State_LongPress(void)
{
    //打印命令列表
    char *cmd_list[] = {"idle\r\n", "short\r\n", "long\r\n"};
    for (uint8_t i = 0; i < 3; i++) {
        HAL_UART_Transmit(&huart1, (uint8_t *)cmd_list[i], strlen(cmd_list[i]), 100);
    }
    //切换到空闲状态
    state = state_idle;
}


static const State State_table[] = {
    {state_idle, State_Idle},
    {state_short, State_ShortPress},
    {state_long, State_LongPress}
};

static const uint8_t State_Count = sizeof(State_table) / sizeof(State);

void State_Process(int state) {
    for (uint8_t i = 0; i < State_Count; i++) {
        if (State_table[i].state == state) {
            State_table[i].handler();
            return;
        }
    }
}

