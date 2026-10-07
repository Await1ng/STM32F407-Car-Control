#ifndef __ENCODER_H
#define __ENCODER_H

#include "headfile.h"

extern int16_t Encoder_L_Count;
extern int16_t Encoder_R_Count;
extern int16_t Encoder_L_Speed;
extern int16_t Encoder_R_Speed;



void Encoder_Init(void);
int16_t Encoder_L_Read(void);
int16_t Encoder_R_Read(void);
void Encoder_Clear(void);

#endif
