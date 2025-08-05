#ifndef __MOTOR_H
#define __MOTOR_H

#include "main.h"

// ����˶�����ö��
typedef enum {
    MOTOR_FORWARD = 0,
    MOTOR_BACKWARD,
    MOTOR_STOP
} Motor_Direction_t;

// --- ��ʼ�� ---
void Motor_Init(void);

// --- �ײ���� (���������ĸ�����) ---
void Motor_FL_Set(Motor_Direction_t direction, uint16_t speed); // ǰ�� Front-Left
void Motor_FR_Set(Motor_Direction_t direction, uint16_t speed); // ǰ�� Front-Right
void Motor_BL_Set(Motor_Direction_t direction, uint16_t speed); // ���� Back-Left
void Motor_BR_Set(Motor_Direction_t direction, uint16_t speed); // ���� Back-Right

// --- �߼���װ (����С�����嶯��) ---
void Car_Forward(uint16_t speed);
void Car_Backward(uint16_t speed);
void Car_Turn_Left(uint16_t speed);
void Car_Turn_Right(uint16_t speed);
void Car_Spin_Left(uint16_t speed);
void Car_Spin_Right(uint16_t speed);
void Car_Stop(void);

#endif /* __MOTOR_H */