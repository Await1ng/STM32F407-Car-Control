#ifndef __ANGLE_H
#define __ANGLE_H

#include "headfile.h"

extern float AngleX;  // 融合后的X轴角度
extern float AngleY;  // 融合后的Y轴角度
extern float AngleZ;  // 融合后的Z轴角度

extern int16_t GX_OFFSET;  // 陀螺仪X轴零点校准值
extern int16_t GY_OFFSET;  // 陀螺仪Y轴零点校准值
extern int16_t GZ_OFFSET;  // 陀螺仪Z轴零点校准值

void Angle_Init(void);
void Angle_Update(void);

#endif
