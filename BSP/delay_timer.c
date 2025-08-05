#include "delay_timer.h"

void Delay_Timer_Set(Delay_Timer_t* timer, uint32_t interval_ms)
{
    timer->start_tick = HAL_GetTick();
    timer->interval = interval_ms;
}

bool Delay_Timer_Is_Expired(Delay_Timer_t* timer)
{
    // 检查当前时间与开始时间的差值是否超过了设定的间隔
    if (HAL_GetTick() - timer->start_tick >= timer->interval)
    {
        // 时间已到，自动重置开始时间戳，为下一次计时做准备
        timer->start_tick = HAL_GetTick(); 
        return true; // 返回 true 表示到期
    }
    return false; // 返回 false 表示时间未到
}