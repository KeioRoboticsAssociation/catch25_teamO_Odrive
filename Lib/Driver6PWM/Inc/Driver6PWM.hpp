#ifndef DRIVER6PWM_HPP
#define DRIVER6PWM_HPP

#pragma once

#include "main.h"
#include <string.h>
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_tim.h"
#include <stm32f446xx.h>
#include "tim.h"
#include <cmath>

class Driver6PWM
{
    private:
        // 6つのPWMチャンネルを保持
        TIM_HandleTypeDef* timAH;
        uint32_t timAH_Channel;
        TIM_HandleTypeDef* timAL;
        uint32_t timAL_Channel;
        TIM_HandleTypeDef* timBH;
        uint32_t timBH_Channel;
        TIM_HandleTypeDef* timBL;
        uint32_t timBL_Channel;
        TIM_HandleTypeDef* timCH;
        uint32_t timCH_Channel;
        TIM_HandleTypeDef* timCL;
        uint32_t timCL_Channel;

        float Vbus;   // DCバス電圧[V]　例: 24V 動作（ODrive 56V基板でも可変）
    public:
        Driver6PWM(
            TIM_HandleTypeDef* _timAH, uint32_t _chAH,
            TIM_HandleTypeDef* _timAL, uint32_t _chAL,
            TIM_HandleTypeDef* _timBH, uint32_t _chBH,
            TIM_HandleTypeDef* _timBL, uint32_t _chBL,
            TIM_HandleTypeDef* _timCH, uint32_t _chCH,
            TIM_HandleTypeDef* _timCL, uint32_t _chCL,
            float _Vbus = 24.0f
        );
        
        void init();
        void enable();
        void setPwm(float Ua, float Ub, float Uc);

};
#endif // DRIVER6PWM_HPP