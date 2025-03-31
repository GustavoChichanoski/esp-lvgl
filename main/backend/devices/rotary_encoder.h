#ifndef __ENCODER_H__
#define __ENCODER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "configs/project_types.h"

#define ENCODER_ROTATE_CLOCKWISE      (0)
#define ENCODER_ROTATE_ANTI_CLOCKWISE (1)

Encoder* encoderGet();
void encoderUpdatePosition(void* arg);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif