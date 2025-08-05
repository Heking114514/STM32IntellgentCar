#include "track.h"
#include "usart.h" 
#include <string.h>
#include <stdio.h>

// TCRT5000: 黑线(高电平), 白地(低电平)
#define ON_LINE_LEVEL   GPIO_PIN_SET
#define OFF_LINE_LEVEL  GPIO_PIN_RESET

Track_Status_t Track_Get_Status(void) {
    GPIO_PinState val_left   = HAL_GPIO_ReadPin(TRACK_L_GPIO_Port, TRACK_L_Pin);
    GPIO_PinState val_middle = HAL_GPIO_ReadPin(TRACK_M_GPIO_Port, TRACK_M_Pin);
    GPIO_PinState val_right  = HAL_GPIO_ReadPin(TRACK_R_GPIO_Port, TRACK_R_Pin);

    // 状态判断 (L, M, R) -> (0=白, 1=黑)
    if (val_left==OFF_LINE_LEVEL && val_middle==ON_LINE_LEVEL && val_right==OFF_LINE_LEVEL) { // 010
        return TRACK_STATE_ON_LINE;
    } else if (val_left==ON_LINE_LEVEL && val_middle==ON_LINE_LEVEL && val_right==OFF_LINE_LEVEL) { // 110
        return TRACK_STATE_DEVIATE_RIGHT;
    } else if (val_left==ON_LINE_LEVEL && val_middle==OFF_LINE_LEVEL && val_right==OFF_LINE_LEVEL) { // 100
        return TRACK_STATE_DEVIATE_RIGHT;
    } else if (val_left==OFF_LINE_LEVEL && val_middle==ON_LINE_LEVEL && val_right==ON_LINE_LEVEL) { // 011
        return TRACK_STATE_DEVIATE_LEFT;
    } else if (val_left==OFF_LINE_LEVEL && val_middle==OFF_LINE_LEVEL && val_right==ON_LINE_LEVEL) { // 001
        return TRACK_STATE_DEVIATE_LEFT;
    } else if (val_left==ON_LINE_LEVEL && val_middle==ON_LINE_LEVEL && val_right==ON_LINE_LEVEL) { // 111
        return TRACK_STATE_ALL_ON_LINE;
    } else if (val_left==OFF_LINE_LEVEL && val_middle==OFF_LINE_LEVEL && val_right==OFF_LINE_LEVEL) { // 000
        return TRACK_STATE_OFF_LINE;
    }
    
    return TRACK_STATE_UNKNOWN;
}

void Track_Print_Raw_Value(void) {
    char buffer[50];
    GPIO_PinState val_left   = HAL_GPIO_ReadPin(TRACK_L_GPIO_Port, TRACK_L_Pin);
    GPIO_PinState val_middle = HAL_GPIO_ReadPin(TRACK_M_GPIO_Port, TRACK_M_Pin);
    GPIO_PinState val_right  = HAL_GPIO_ReadPin(TRACK_R_GPIO_Port, TRACK_R_Pin);
    
    sprintf(buffer, "Track (L-M-R): %d-%d-%d\r\n", val_left, val_middle, val_right);
    HAL_UART_Transmit(&huart1, (uint8_t*)buffer, strlen(buffer), 100);
}