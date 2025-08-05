#ifndef __OBSTACLE_H
#define __OBSTACLE_H

#include "main.h"
#include <stdbool.h>

// ����Ƕȶ���
#define SERVO_POS_LEFT       170
#define SERVO_POS_CENTER     90
#define SERVO_POS_RIGHT      10

// --- ��ʼ�� ---
void Obstacle_Init(void);

// --- ������� ---
void Servo_SetAngle(uint8_t angle);

// --- ��װ��ĸ߼��ӿ� (��main.c����) ---

/**
 * @brief  ��ȡһ�γ���������� (����ʽ)
 * @note   �˺����ڲ�������һ�η������������ȴ�����ɻ�ʱ��
 * @param  None
 * @retval ��õľ��� (��λ: cm)�����������ʱ��ʧ�ܣ����� -1.0f��
 */
float Obstacle_GetDistance(void);

/**
 * @brief  ɨ��ǰ����������ľ��� (����ʽ)
 * @param  dist_left:   ���ڴ洢�������ָ��
 * @param  dist_center: ���ڴ洢�м�����ָ��
 * @param  dist_right:  ���ڴ洢�Ҳ�����ָ��
 */
void Obstacle_Scan(float* dist_left, float* dist_center, float* dist_right);


// --- ���ڲ����ж�ʹ�õĺ��� (ͨ������Ҫ���ⲿ�ļ�����) ---
void Obstacle_TIM_IC_Callback(TIM_HandleTypeDef *htim);

#endif /* __OBSTACLE_H */