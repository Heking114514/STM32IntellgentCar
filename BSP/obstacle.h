#ifndef __OBSTACLE_H
#define __OBSTACLE_H

#include "main.h"
#include <stdbool.h>

// 舵机角度定义
#define SERVO_POS_LEFT       170
#define SERVO_POS_CENTER     90
#define SERVO_POS_RIGHT      10

// --- 初始化 ---
void Obstacle_Init(void);

// --- 舵机控制 ---
void Servo_SetAngle(uint8_t angle);

// --- 封装后的高级接口 (供main.c调用) ---

/**
 * @brief  获取一次超声波测距结果 (阻塞式)
 * @note   此函数内部会启动一次非阻塞测量并等待其完成或超时。
 * @param  None
 * @retval 测得的距离 (单位: cm)。如果测量超时或失败，返回 -1.0f。
 */
float Obstacle_GetDistance(void);

/**
 * @brief  扫描前方三个方向的距离 (阻塞式)
 * @param  dist_left:   用于存储左侧距离的指针
 * @param  dist_center: 用于存储中间距离的指针
 * @param  dist_right:  用于存储右侧距离的指针
 */
void Obstacle_Scan(float* dist_left, float* dist_center, float* dist_right);


// --- 供内部和中断使用的函数 (通常不需要被外部文件调用) ---
void Obstacle_TIM_IC_Callback(TIM_HandleTypeDef *htim);

#endif /* __OBSTACLE_H */