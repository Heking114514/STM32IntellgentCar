#ifndef __DELAY_TIMER_H
#define __DELAY_TIMER_H

#include "main.h"
#include <stdbool.h>

// 定义一个软件定时器结构体
typedef struct {
    uint32_t start_tick; // 定时器启动时的时间戳
    uint32_t interval;   // 定时器设定的间隔时间 (ms)
} Delay_Timer_t;

/**
 * @brief  设置(或重置)一个软件定时器
 * @param  timer: 指向要设置的定时器结构体的指针
 * @param  interval_ms: 定时器的时间间隔 (单位: 毫秒)
 * @retval None
 */
void Delay_Timer_Set(Delay_Timer_t* timer, uint32_t interval_ms);

/**
 * @brief  检查一个软件定时器是否已经到期
 * @note   这是一个非阻塞函数，会立即返回结果。
 *         如果定时器到期，此函数会自动重置定时器，为下一次计时做准备。
 * @param  timer: 指向要检查的定时器结构体的指针
 * @retval bool: 如果时间已到则返回true，否则返回false
 */
bool Delay_Timer_Is_Expired(Delay_Timer_t* timer);

#endif /* __DELAY_TIMER_H */