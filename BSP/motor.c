#include "motor.h"
#include "main.h" // 强制包含main.h以获取所有宏和类型定义
#include "tim.h"

void Motor_Init(void)
{
    // 启动所有电机PWM通道
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1); // FL
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2); // FR
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_3); // BL
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_4); // BR
    Car_Stop();
}

/**
 * @brief  控制左侧所有轮子(前左+后左)的运动
 */
void Motor_Left_Set(Motor_Direction_t direction, uint16_t speed)
{
    if (speed > 1000) speed = 1000;

    // 设置左侧轮子PWM速度
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, speed); // 前左轮 (FL)
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3, speed); // 后左轮 (BL)

    // 设置方向
    switch(direction)
    {
        case MOTOR_FORWARD:
            // 前左轮: AIN1=RESET, AIN2=SET
            HAL_GPIO_WritePin(MOTOR_AIN1_GPIO_Port, MOTOR_AIN1_Pin, GPIO_PIN_SET);
            HAL_GPIO_WritePin(MOTOR_AIN2_GPIO_Port, MOTOR_AIN2_Pin, GPIO_PIN_RESET);
            // 后左轮: CIN1=SET, CIN2=RESET
            HAL_GPIO_WritePin(MOTOR_CIN1_GPIO_Port, MOTOR_CIN1_Pin, GPIO_PIN_SET);
            HAL_GPIO_WritePin(MOTOR_CIN2_GPIO_Port, MOTOR_CIN2_Pin, GPIO_PIN_RESET);
            break;
        case MOTOR_BACKWARD:
            // 前左轮: AIN1=SET, AIN2=RESET
            HAL_GPIO_WritePin(MOTOR_AIN1_GPIO_Port, MOTOR_AIN1_Pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(MOTOR_AIN2_GPIO_Port, MOTOR_AIN2_Pin, GPIO_PIN_SET);
            // 后左轮: CIN1=RESET, CIN2=SET
            HAL_GPIO_WritePin(MOTOR_CIN1_GPIO_Port, MOTOR_CIN1_Pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(MOTOR_CIN2_GPIO_Port, MOTOR_CIN2_Pin, GPIO_PIN_SET);
            break;
        case MOTOR_STOP:
            HAL_GPIO_WritePin(MOTOR_AIN1_GPIO_Port, MOTOR_AIN1_Pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(MOTOR_AIN2_GPIO_Port, MOTOR_AIN2_Pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(MOTOR_CIN1_GPIO_Port, MOTOR_CIN1_Pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(MOTOR_CIN2_GPIO_Port, MOTOR_CIN2_Pin, GPIO_PIN_RESET);
            break;
    }
}

/**
 * @brief  控制右侧所有轮子(前右+后右)的运动
 */
void Motor_Right_Set(Motor_Direction_t direction, uint16_t speed)
{
    if (speed > 1000) speed = 1000;
    
    // 设置右侧轮子PWM速度
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, speed); // 前右轮 (FR)
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_4, speed); // 后右轮 (BR)

    switch(direction)
    {
        case MOTOR_FORWARD:
            // 前右轮: BIN1=SET, BIN2=RESET
            HAL_GPIO_WritePin(MOTOR_BIN1_GPIO_Port, MOTOR_BIN1_Pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(MOTOR_BIN2_GPIO_Port, MOTOR_BIN2_Pin, GPIO_PIN_SET);
            // 后右轮: DIN1=SET, DIN2=RESET
            HAL_GPIO_WritePin(MOTOR_DIN1_GPIO_Port, MOTOR_DIN1_Pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(MOTOR_DIN2_GPIO_Port, MOTOR_DIN2_Pin, GPIO_PIN_SET);
            break;
        case MOTOR_BACKWARD:
            // 前右轮: BIN1=RESET, BIN2=SET
            HAL_GPIO_WritePin(MOTOR_BIN1_GPIO_Port, MOTOR_BIN1_Pin, GPIO_PIN_SET);
            HAL_GPIO_WritePin(MOTOR_BIN2_GPIO_Port, MOTOR_BIN2_Pin, GPIO_PIN_RESET);
            // 后右轮: DIN1=RESET, DIN2=SET
            HAL_GPIO_WritePin(MOTOR_DIN1_GPIO_Port, MOTOR_DIN1_Pin, GPIO_PIN_SET);
            HAL_GPIO_WritePin(MOTOR_DIN2_GPIO_Port, MOTOR_DIN2_Pin, GPIO_PIN_RESET);
            break;
        case MOTOR_STOP:
            HAL_GPIO_WritePin(MOTOR_BIN1_GPIO_Port, MOTOR_BIN1_Pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(MOTOR_BIN2_GPIO_Port, MOTOR_BIN2_Pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(MOTOR_DIN1_GPIO_Port, MOTOR_DIN1_Pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(MOTOR_DIN2_GPIO_Port, MOTOR_DIN2_Pin, GPIO_PIN_RESET);
            break;
    }
}

// --- 高级封装函数 ---
void Car_Forward(uint16_t speed)
{
    Motor_Left_Set(MOTOR_FORWARD, speed);
    Motor_Right_Set(MOTOR_FORWARD, speed);
}

void Car_Backward(uint16_t speed)
{
    Motor_Left_Set(MOTOR_BACKWARD, speed);
    Motor_Right_Set(MOTOR_BACKWARD, speed);
}

void Car_Stop(void)
{
    Motor_Left_Set(MOTOR_STOP, 0);
    Motor_Right_Set(MOTOR_STOP, 0);
}

void Car_Spin_Left(uint16_t speed)
{
    Motor_Left_Set(MOTOR_BACKWARD, speed);
    Motor_Right_Set(MOTOR_FORWARD, speed);
}

void Car_Spin_Right(uint16_t speed)
{
    Motor_Left_Set(MOTOR_FORWARD, speed);
    Motor_Right_Set(MOTOR_BACKWARD, speed);
}

void Car_Turn_Left(uint16_t speed)
{
    Motor_Left_Set(MOTOR_FORWARD, speed / 2);
    Motor_Right_Set(MOTOR_FORWARD, speed);
}

void Car_Turn_Right(uint16_t speed)
{
    Motor_Left_Set(MOTOR_FORWARD, speed);
    Motor_Right_Set(MOTOR_FORWARD, speed / 2);
}