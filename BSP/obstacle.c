#include "obstacle.h"
#include "tim.h"

// --- 模块内部全局变量 ---
typedef enum {
    US_STATE_IDLE, 
    US_STATE_TRIGGERED, 
    US_STATE_MEASURING,
    US_STATE_DONE, 
    US_STATE_TIMEOUT
} Ultrasonic_State_t;

static volatile Ultrasonic_State_t g_us_state = US_STATE_IDLE;
static volatile uint32_t g_tim_capture_val1 = 0;
static volatile uint32_t g_tim_capture_val2 = 0;
static volatile uint32_t g_measurement_start_tick = 0;
static float g_last_distance_cm = -1.0f;

#define ULTRASONIC_TIMEOUT_MS 60

// --- 舵机部分 ---
void Servo_SetAngle(uint8_t angle) {
    if (angle > 180) angle = 180;
    float pulse = 50.0f + (angle / 180.0f) * 200.0f;
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, (uint16_t)pulse);
}

// --- 初始化 ---
void Obstacle_Init(void) {
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
    Servo_SetAngle(SERVO_POS_CENTER);
    HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_1);
    __HAL_TIM_SET_CAPTUREPOLARITY(&htim2, TIM_CHANNEL_1, TIM_INPUTCHANNELPOLARITY_RISING);
}

// --- 底层非阻塞函数 (不对外暴露) ---
static bool ultrasonic_start(void)
{
    // 只有在空闲、完成或超时状态下才能开始新的测量
    if (g_us_state != US_STATE_IDLE && g_us_state != US_STATE_DONE && g_us_state != US_STATE_TIMEOUT) {
        return false;
    }
    
    // 发送Trig信号
    HAL_GPIO_WritePin(TRIG_GPIO_Port, TRIG_Pin, GPIO_PIN_SET);
    for(volatile int i=0; i<150; i++); // >10us延时
    HAL_GPIO_WritePin(TRIG_GPIO_Port, TRIG_Pin, GPIO_PIN_RESET);
    
    // 更新状态和时间戳
    g_us_state = US_STATE_TRIGGERED;
    g_measurement_start_tick = HAL_GetTick();
    
    // (重新)使能中断
    __HAL_TIM_ENABLE_IT(&htim2, TIM_IT_CC1);
    return true;
}

// --- 中断回调处理 ---
void Obstacle_TIM_IC_Callback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance != TIM2) return;
    
    // 在处理中断时，暂时关闭中断，防止重入
    __HAL_TIM_DISABLE_IT(&htim2, TIM_IT_CC1);

    if (g_us_state == US_STATE_TRIGGERED) { // 捕获到上升沿
        g_tim_capture_val1 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
        __HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_1, TIM_INPUTCHANNELPOLARITY_FALLING);
        g_us_state = US_STATE_MEASURING;
        __HAL_TIM_ENABLE_IT(&htim2, TIM_IT_CC1); // 重新使能，准备捕获下降沿
    } 
    else if (g_us_state == US_STATE_MEASURING) { // 捕获到下降沿
        g_tim_capture_val2 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
        
        uint32_t high_level_us = (g_tim_capture_val2 >= g_tim_capture_val1) ? 
                                (g_tim_capture_val2 - g_tim_capture_val1) : 
                                ((0xFFFF - g_tim_capture_val1) + g_tim_capture_val2 + 1);
                                
        g_last_distance_cm = high_level_us * 0.017f;
        g_us_state = US_STATE_DONE;
        
        // 测量完成后，重置为上升沿捕获，为下一次做准备
        __HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_1, TIM_INPUTCHANNELPOLARITY_RISING);
    }
}


// --- 重新实现的、供main.c调用的高级阻塞函数 ---

/**
 * @brief  获取一次超声波测距结果 (阻塞式)
 */
float Obstacle_GetDistance(void)
{
    // 1. 启动一次测量
    if (!ultrasonic_start()) {
        return -1.0f; // 如果当前正忙，返回错误
    }

    // 2. 等待结果 (带超时)
    uint32_t start_wait_tick = HAL_GetTick();
    while(g_us_state != US_STATE_DONE)
    {
        // 检查硬件超时
        if ((g_us_state == US_STATE_TRIGGERED || g_us_state == US_STATE_MEASURING) && 
            (HAL_GetTick() - g_measurement_start_tick > ULTRASONIC_TIMEOUT_MS))
        {
            __HAL_TIM_DISABLE_IT(&htim2, TIM_IT_CC1);
            g_us_state = US_STATE_IDLE; // 超时后直接变为空闲
            return -1.0f; // 返回无效值
        }
        
        // 增加一个软件层面的超时，防止因未知中断问题卡死
        if (HAL_GetTick() - start_wait_tick > ULTRASONIC_TIMEOUT_MS + 10)
        {
             g_us_state = US_STATE_IDLE;
             return -1.0f;
        }
    }

    // 3. 返回结果并重置状态
    float distance = g_last_distance_cm;
    g_us_state = US_STATE_IDLE;
    return distance;
}

/**
 * @brief  扫描前方三个方向的距离 (阻塞式)
 */
void Obstacle_Scan(float* dist_left, float* dist_center, float* dist_right)
{
    Servo_SetAngle(SERVO_POS_RIGHT);
    HAL_Delay(400); // 等待舵机转动稳定
    *dist_right = Obstacle_GetDistance();
    
    Servo_SetAngle(SERVO_POS_CENTER);
    HAL_Delay(500);
    *dist_center = Obstacle_GetDistance();
    
    Servo_SetAngle(SERVO_POS_LEFT);
    HAL_Delay(500);
    *dist_left = Obstacle_GetDistance();
    
    // 扫描完毕后，舵机归中
    Servo_SetAngle(SERVO_POS_CENTER);
    HAL_Delay(400);
}