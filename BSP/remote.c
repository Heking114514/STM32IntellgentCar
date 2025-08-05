#include "remote.h"
#include "usart.h"

static uint8_t g_rx_buffer;
volatile static Remote_Command_t g_remote_cmd = CMD_NONE;

void Remote_Init(void) {
    HAL_UART_Receive_IT(&huart1, &g_rx_buffer, 1);
}

Remote_Command_t Remote_Get_Command(void) {
    Remote_Command_t cmd_to_return = g_remote_cmd;
    if (g_remote_cmd != CMD_NONE) {
        g_remote_cmd = CMD_NONE;
    }
    return cmd_to_return;
}

void Remote_UART_RxCallback(uint8_t rx_data) {
    switch(rx_data) {
        case 'F': case 'f': g_remote_cmd = CMD_FORWARD; break;
        case 'B': case 'b': g_remote_cmd = CMD_BACKWARD; break;
        case 'L': case 'l': g_remote_cmd = CMD_TURN_LEFT; break;
        case 'R': case 'r': g_remote_cmd = CMD_TURN_RIGHT; break;
        case 'G': case 'g': g_remote_cmd = CMD_SPIN_LEFT; break;
        case 'H': case 'h': g_remote_cmd = CMD_SPIN_RIGHT; break;
        case 'S': case 's': g_remote_cmd = CMD_STOP; break;
        case '1': g_remote_cmd = CMD_MODE_TRACK; break;
        case '2': g_remote_cmd = CMD_MODE_AVOID; break;
        default: g_remote_cmd = CMD_UNKNOWN; break;
    }
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == USART1) {
        Remote_UART_RxCallback(g_rx_buffer);
        HAL_UART_Receive_IT(&huart1, &g_rx_buffer, 1);
    }
}