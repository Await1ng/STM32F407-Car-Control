#include "Angle.h"

float AngleX = 0.0f;  // 融合后的X轴角度
float AngleY = 0.0f;  // 融合后的Y轴角度
float AngleZ = 0.0f;  // 融合后的Z轴角度

int16_t GX_OFFSET = 0;  // 陀螺仪X轴零点校准值
int16_t GY_OFFSET = 0;  // 陀螺仪Y轴零点校准值
int16_t GZ_OFFSET = 0;  // 陀螺仪Z轴零点校准值

static float dt = 0.01f;  // 采样周期10ms
static float alpha = 0.03f;  // 互补滤波系数
static float angleAccX, angleAccY;
static float angleGyroX = 0, angleGyroY = 0, angleGyroZ = 0;
static int16_t AX, AY, AZ, GX, GY, GZ;

// 滑动平均滤波参数
#define FILTER_SIZE 5  // 滤波窗口大小
static float AngleX_Buffer[FILTER_SIZE] = {0};
static float AngleY_Buffer[FILTER_SIZE] = {0};
static float AngleZ_Buffer[FILTER_SIZE] = {0};
static uint8_t filter_index = 0;

/**
  * 函    数：角度初始化
  * 参    数：无
  * 返 回 值：无
  */
void Angle_Init(void)
{
    // 校准陀螺仪零点
    OLED_Printf(0, 0, OLED_8X16, "Calibrating...");
    OLED_Update();
    if (MPU6050_Calibrate(&GX_OFFSET, &GY_OFFSET, &GZ_OFFSET) != 0)
    {
        OLED_Printf(0, 0, OLED_8X16, "Calibration Failed");
        OLED_Update();
        delay_ms(1000);
        return;
    }
    OLED_Printf(0, 0, OLED_8X16, "Calibrated   ");
    OLED_Update();
    delay_ms(1000);
}

/**
  * 函    数：角度更新
  * 参    数：无
  * 返 回 值：无
  */
void Angle_Update(void)
{
    MPU6050_GetData(&AX, &AY, &AZ, &GX, &GY, &GZ);
    
    // 去除陀螺仪零点偏移
    GX -= GX_OFFSET;
    GY -= GY_OFFSET;
    GZ -= GZ_OFFSET;
    
    // 加速度计计算角度（弧度转角度）
    angleAccX = atan2((float)AY, sqrt((float)AX * AX + (float)AZ * AZ)) * 180.0f / 3.14159f;
    angleAccY = atan2((float)-AX, sqrt((float)AY * AY + (float)AZ * AZ)) * 180.0f / 3.14159f;
    
    // 陀螺仪计算角度（积分）
    angleGyroX += (float)GX / 32768.0f * 2000.0f * dt * 8.75f;
    angleGyroY += (float)GY / 32768.0f * 2000.0f * dt * 8.75f;
    angleGyroZ += (float)GZ / 32768.0f * 2000.0f * dt * 8.75f;
    
    // 互补滤波融合角度
    float tempX = alpha * angleAccX + (1 - alpha) * angleGyroX;
    float tempY = alpha * angleAccY + (1 - alpha) * angleGyroY;
    float tempZ = angleGyroZ;  // Z轴只有陀螺仪数据
    
    // 滑动平均滤波
    AngleX_Buffer[filter_index] = tempX;
    AngleY_Buffer[filter_index] = tempY;
    AngleZ_Buffer[filter_index] = tempZ;
    
    // 计算平均值
    float sumX = 0, sumY = 0, sumZ = 0;
    for (uint8_t i = 0; i < FILTER_SIZE; i++)
    {
        sumX += AngleX_Buffer[i];
        sumY += AngleY_Buffer[i];
        sumZ += AngleZ_Buffer[i];
    }
    
    AngleX = sumX / FILTER_SIZE;
    AngleY = sumY / FILTER_SIZE;
    AngleZ = sumZ / FILTER_SIZE;
    
    // 更新滤波索引
    filter_index = (filter_index + 1) % FILTER_SIZE;
}

