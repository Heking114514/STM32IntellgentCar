#ifndef __TRACK_H
#define __TRACK_H

#include "main.h"

// ѭ��״̬ö��
typedef enum {
    TRACK_STATE_ON_LINE,
    TRACK_STATE_DEVIATE_LEFT,
    TRACK_STATE_DEVIATE_RIGHT,
    TRACK_STATE_ALL_ON_LINE,
    TRACK_STATE_OFF_LINE,
    TRACK_STATE_UNKNOWN
} Track_Status_t;

Track_Status_t Track_Get_Status(void);
void Track_Print_Raw_Value(void); // ���ڵ���

#endif /* __TRACK_H */