#ifndef __MOTOR_H
#define __MOTOR_H

#include "main.h"

// 电机运动方向枚举
typedef enum {
    MOTOR_FORWARD = 0,
    MOTOR_BACKWARD,
    MOTOR_STOP
} Motor_Direction_t;

// --- 初始化 ---
void Motor_Init(void);

// --- 底层控制 (独立控制四个轮子) ---
void Motor_FL_Set(Motor_Direction_t direction, uint16_t speed); // 前左 Front-Left
void Motor_FR_Set(Motor_Direction_t direction, uint16_t speed); // 前右 Front-Right
void Motor_BL_Set(Motor_Direction_t direction, uint16_t speed); // 后左 Back-Left
void Motor_BR_Set(Motor_Direction_t direction, uint16_t speed); // 后右 Back-Right

// --- 高级封装 (控制小车整体动作) ---
void Car_Forward(uint16_t speed);
void Car_Backward(uint16_t speed);
void Car_Turn_Left(uint16_t speed);
void Car_Turn_Right(uint16_t speed);
void Car_Spin_Left(uint16_t speed);
void Car_Spin_Right(uint16_t speed);
void Car_Stop(void);

#endif /* __MOTOR_H */