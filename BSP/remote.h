#ifndef __REMOTE_H
#define __REMOTE_H

#include "main.h"

typedef enum {
    CMD_NONE = 0, CMD_FORWARD, CMD_BACKWARD, CMD_TURN_LEFT, CMD_TURN_RIGHT,
    CMD_SPIN_LEFT, CMD_SPIN_RIGHT, CMD_STOP, CMD_MODE_TRACK, CMD_MODE_AVOID,
    CMD_UNKNOWN
} Remote_Command_t;

void Remote_Init(void);
Remote_Command_t Remote_Get_Command(void);
void Remote_UART_RxCallback(uint8_t rx_data);

#endif /* __REMOTE_H */