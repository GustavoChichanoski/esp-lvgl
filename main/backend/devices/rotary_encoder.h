#ifndef __ENCODER_H__
#define __ENCODER_H__

#ifdef __cplusplus
extern "C" {
#endif

#define ENCODER_ROTATE_CLOCKWISE      (0)
#define ENCODER_ROTATE_ANTI_CLOCKWISE (1)

#include "configs/project_types.h"

Encoder* get_encoder();
void update_encoder_position(void* arg);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif