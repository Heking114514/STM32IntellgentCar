#ifndef __DELAY_TIMER_H
#define __DELAY_TIMER_H

#include "main.h"
#include <stdbool.h>

// ����һ�������ʱ���ṹ��
typedef struct {
    uint32_t start_tick; // ��ʱ������ʱ��ʱ���
    uint32_t interval;   // ��ʱ���趨�ļ��ʱ�� (ms)
} Delay_Timer_t;

/**
 * @brief  ����(������)һ�������ʱ��
 * @param  timer: ָ��Ҫ���õĶ�ʱ���ṹ���ָ��
 * @param  interval_ms: ��ʱ����ʱ���� (��λ: ����)
 * @retval None
 */
void Delay_Timer_Set(Delay_Timer_t* timer, uint32_t interval_ms);

/**
 * @brief  ���һ�������ʱ���Ƿ��Ѿ�����
 * @note   ����һ�����������������������ؽ����
 *         �����ʱ�����ڣ��˺������Զ����ö�ʱ����Ϊ��һ�μ�ʱ��׼����
 * @param  timer: ָ��Ҫ���Ķ�ʱ���ṹ���ָ��
 * @retval bool: ���ʱ���ѵ��򷵻�true�����򷵻�false
 */
bool Delay_Timer_Is_Expired(Delay_Timer_t* timer);

#endif /* __DELAY_TIMER_H */