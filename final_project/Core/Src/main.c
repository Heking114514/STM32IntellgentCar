/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "motor.h"
#include "obstacle.h"
#include "track.h"
#include "remote.h"
#include <stdio.h>  
#include <string.h> 
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
// 定义小车的工作模式
typedef enum {
    MODE_REMOTE, // 手动遥控模式
    MODE_AVOID,  // 纯避障模式
    MODE_TRACK   // 纯循迹模式
} Car_Mode_t;

// 定义自动模式下的运行状态 (用于避障模式的启停)
typedef enum {
    AUTO_STATE_STOP,
    AUTO_STATE_RUN
} Auto_State_t;

// 定义避障过程的详细状态
typedef enum {
    AVOID_STATE_IDLE,       // 0. 空闲/前进状态
    AVOID_STATE_STOPPING,   // 1. 发现障碍物，正在停车
    AVOID_STATE_SCANNING,   // 2. 正在扫描
    AVOID_STATE_DECIDING,   // 3. 正在决策
    AVOID_STATE_TURNING,    // 4. 正在转向
    AVOID_STATE_BACKING,    // 5. 正在后退
    AVOID_STATE_RECHECK     // 6. 转向/后退后，重新检查
} Avoid_State_t;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define OBSTACLE_DISTANCE_THRESHOLD  15.0f  // 避障触发距离 (cm)
#define DISTANCE_SEND_INTERVAL_MS    500 
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

// 定义全局变量来存储当前的工作模式和状态
volatile Car_Mode_t g_car_mode = MODE_REMOTE;
volatile Auto_State_t g_auto_state = AUTO_STATE_STOP;
static uint32_t last_distance_send_tick = 0; // +++ ADD: 用于记录上次发送时间的变量

// 避障状态机相关全局变量
volatile Avoid_State_t g_avoid_state = AVOID_STATE_IDLE;
uint32_t g_state_timer = 0; // 用于状态计时的软件定时器
float g_dist_left, g_dist_center, g_dist_right; // 存储扫描结果
static int8_t g_last_avoid_turn = 1; // +++ ADD: 用于记录上一次避障转向 (1=右, -1=左)
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void Handle_Mode_Switch(Remote_Command_t command);
void Perform_Remote_Control(Remote_Command_t command);
void Perform_Avoid_Mode(Remote_Command_t command);
void Perform_Track_Mode(void);
void Send_Distance_Data(float distance); // +++ ADD: 声明新的数据发送函数
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM1_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
  
  Motor_Init();
  Obstacle_Init();
  Remote_Init();
  Car_Stop();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    Remote_Command_t command = Remote_Get_Command();
    // 1. 模式切换逻辑 (最高优先级)
    Handle_Mode_Switch(command);
	

    // 2. 根据当前模式执行对应的任务
    switch(g_car_mode)
    {
        case MODE_REMOTE:
            Perform_Remote_Control(command);
            break;
        case MODE_AVOID:
            Perform_Avoid_Mode(command);
            break;
        case MODE_TRACK:
            Perform_Track_Mode();
            break;
    }
   
    HAL_Delay(20);
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/**
  * @brief  处理模式切换的逻辑
  */
void Handle_Mode_Switch(Remote_Command_t command)
{
    // 收到 '1' 指令 -> 切换到循迹模式
    if (command == CMD_MODE_TRACK) {
        if (g_car_mode != MODE_TRACK) {
            g_car_mode = MODE_TRACK;
            g_auto_state = AUTO_STATE_RUN; // 循迹模式默认启动
            g_avoid_state = AVOID_STATE_IDLE; // <-- 增加
            Car_Stop();
            HAL_Delay(100);
        }
    }
    // 收到 '2' 指令 -> 切换到避障模式
    else if (command == CMD_MODE_AVOID) {
        if (g_car_mode != MODE_AVOID) {
            g_car_mode = MODE_AVOID;
            g_auto_state = AUTO_STATE_STOP; // 避障模式默认停止
            g_avoid_state = AVOID_STATE_IDLE; // 重置避障状态机
            Car_Stop();
            HAL_Delay(100);
        }
    }
    // 收到任何方向键 (L, R, G, H, B)，强制切回遥控模式
    else if ((command >= CMD_TURN_LEFT && command <= CMD_SPIN_RIGHT) || command == CMD_BACKWARD) {
        if (g_car_mode != MODE_REMOTE) {
            g_car_mode = MODE_REMOTE;
            g_auto_state = AUTO_STATE_STOP;
            Car_Stop();
        }
    }
}

/**
  * @brief  执行遥控模式的逻辑
  */
void Perform_Remote_Control(Remote_Command_t command)
{
    switch(command) {
        case CMD_FORWARD:    Car_Forward(900); break;
        case CMD_BACKWARD:   Car_Backward(700); break;
        case CMD_TURN_LEFT:  Car_Turn_Left(600); break;
        case CMD_TURN_RIGHT: Car_Turn_Right(600); break;
        case CMD_SPIN_LEFT:  Car_Spin_Left(600); break;
        case CMD_SPIN_RIGHT: Car_Spin_Right(600); break;
        case CMD_STOP:       Car_Stop(); break;
        default: break;
    }
}

/**
  * @brief  执行纯避障模式的逻辑 (非阻塞状态机 + 智能决策)
  * @param  command: 蓝牙指令
  */
void Perform_Avoid_Mode(Remote_Command_t command)
{
    // 1. 启停控制
    if (command == CMD_FORWARD) {
        if (g_auto_state == AUTO_STATE_STOP) {
            g_auto_state = AUTO_STATE_RUN;
            g_avoid_state = AVOID_STATE_IDLE;
        }
    } else if (command == CMD_STOP) {
        if (g_auto_state == AUTO_STATE_RUN) {
            g_auto_state = AUTO_STATE_STOP;
            g_avoid_state = AVOID_STATE_IDLE;
            Car_Stop();
        }
    }
    
    if (g_auto_state == AUTO_STATE_STOP) return;

    // --- 避障状态机核心 ---
    switch(g_avoid_state)
    {
        case AVOID_STATE_IDLE:
        {
            Car_Forward(600);
            float distance = Obstacle_GetDistance();
            
            if (HAL_GetTick() - last_distance_send_tick > DISTANCE_SEND_INTERVAL_MS) {
                Send_Distance_Data(distance);
                last_distance_send_tick = HAL_GetTick();
            }

            if (distance > 0 && distance < OBSTACLE_DISTANCE_THRESHOLD) {
                g_avoid_state = AVOID_STATE_STOPPING;
            }
            break;
        }

        case AVOID_STATE_STOPPING:
        {
            Car_Stop();
            g_state_timer = HAL_GetTick();
            g_avoid_state = AVOID_STATE_SCANNING;
            break;
        }

        case AVOID_STATE_SCANNING:
        {
            if (HAL_GetTick() - g_state_timer > 200) {
                Obstacle_Scan(&g_dist_left, &g_dist_center, &g_dist_right);
                g_avoid_state = AVOID_STATE_DECIDING;
            }
            break;
        }

        case AVOID_STATE_DECIDING:
        {
            _Bool is_right_safe = (g_dist_right > OBSTACLE_DISTANCE_THRESHOLD);
            _Bool is_left_safe = (g_dist_left > OBSTACLE_DISTANCE_THRESHOLD);

            if (is_right_safe && !is_left_safe) { // 仅右安全
                Car_Spin_Right(500);
                g_last_avoid_turn = 1;
                g_state_timer = HAL_GetTick();
                g_avoid_state = AVOID_STATE_TURNING;
            } 
            else if (!is_right_safe && is_left_safe) { // 仅左安全
                Car_Spin_Left(500);
                g_last_avoid_turn = -1;
                g_state_timer = HAL_GetTick();
                g_avoid_state = AVOID_STATE_TURNING;
            }
            else if (is_right_safe && is_left_safe) { // 两边都安全
                if (g_dist_right > g_dist_left) { // 选更宽的路
                    Car_Spin_Right(500);
                    g_last_avoid_turn = 1;
                } else {
                    Car_Spin_Left(500);
                    g_last_avoid_turn = -1;
                }
                g_state_timer = HAL_GetTick();
                g_avoid_state = AVOID_STATE_TURNING;
            } 
            else { // 两边都不安全
                Car_Backward(600);
                g_state_timer = HAL_GetTick();
                g_avoid_state = AVOID_STATE_BACKING;
            }
            break;
        }

        case AVOID_STATE_TURNING:
        {
            if (HAL_GetTick() - g_state_timer > 500) {
                Car_Stop();
                g_avoid_state = AVOID_STATE_RECHECK;
            }
            break;
        }
        
        case AVOID_STATE_BACKING:
        {
            if (HAL_GetTick() - g_state_timer > 2000) {
                // 后退完成后，根据"记忆"进行强制转向
                if (g_last_avoid_turn == 1) {
                    Car_Spin_Left(600); // 上次向右碰壁，这次强制向左
                    g_last_avoid_turn = -1;
                } else {
                    Car_Spin_Right(600); // 上次向左碰壁，这次强制向右
                    g_last_avoid_turn = 1;
                }
                g_state_timer = HAL_GetTick();
                g_avoid_state = AVOID_STATE_TURNING; // 进入转向状态
            }
            break;
        }

        case AVOID_STATE_RECHECK:
        {
            g_avoid_state = AVOID_STATE_IDLE;
            break;
        }
    }
}


/**
  * @brief  执行纯循迹模式的逻辑
  */
void Perform_Track_Mode(void)
{
    Track_Status_t track_status = Track_Get_Status();
    switch(track_status) {
        case TRACK_STATE_ON_LINE:       Car_Forward(700); break;
        case TRACK_STATE_DEVIATE_LEFT:  Car_Spin_Right(600); break;
        case TRACK_STATE_DEVIATE_RIGHT: Car_Spin_Left(600); break;
        case TRACK_STATE_ALL_ON_LINE:
        case TRACK_STATE_OFF_LINE:
        default:
            Car_Stop();
            break;
    }
}

/**
  * @brief  通过蓝牙串口发送距离数据 (使用HAL库函数)
  * @param  distance: 要发送的距离值
  * @retval None
  */
void Send_Distance_Data(float distance)
{
    char tx_buffer[32]; // 创建一个足够大的发送缓冲区
    int index = 0;

    // 1. 添加数据头 "D:"
    tx_buffer[index++] = 'D';
    tx_buffer[index++] = ':';

    // 2. 处理无效距离 (例如超时返回的-1.0)
    if (distance < 0) {
        tx_buffer[index++] = '-';
        tx_buffer[index++] = '1';
    } 
    else 
    {
        // 3. 将浮点数转换为整数部分和小数部分 (只保留一位小数)
        int integer_part = (int)distance;
        int fractional_part = (int)((distance - integer_part) * 10);

        // 4. 手动转换整数部分为字符串
        if (integer_part == 0) {
            tx_buffer[index++] = '0';
        } else {
            char temp_buffer[10];
            int i = 0;
            while (integer_part > 0) {
                temp_buffer[i++] = (integer_part % 10) + '0';
                integer_part /= 10;
            }
            // 反转数字字符串
            while (i > 0) {
                tx_buffer[index++] = temp_buffer[--i];
            }
        }
        
        // 5. 添加小数点
        tx_buffer[index++] = '.';

        // 6. 添加小数部分
        tx_buffer[index++] = fractional_part + '0';
    }

    // 7. 添加结束符 "\n"
    tx_buffer[index++] = '\n';

    // 8. 调用HAL库函数发送
    HAL_UART_Transmit(&huart1, (uint8_t*)tx_buffer, index, 100);
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
