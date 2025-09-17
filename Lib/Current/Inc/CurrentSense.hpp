#ifndef CURRENTSENSE_HPP
#define CURRENTSENSE_HPP

#pragma once

#include "main.h"
#include <string.h>
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_adc.h"
#include <stm32f446xx.h>
#include "adc.h"

// シャント抵抗からモータ電流を計算する仕組み

class CurrentSense
{
    private:
        ADC_HandleTypeDef* hadc;
        float Ia = 0.0f;
        float Ib = 0.0f;
        float Ic = 0.0f;

         // オフセット補正用
        float offsetIa = 0.0f;
        float offsetIc = 0.0f;
        bool calibrated = false;
        
        // 電流換算スケール
        const float shunt_resistor = 0.0005f;   // 0.5 mΩ
        const float adc_ref_voltage = 3.3f;
        const float adc_resolution = 4096.0f;  // 12-bit
        const float opamp_gain = 40.0f;        //  例: DRV8301 CSA=40 V/V

    public:
        CurrentSense(ADC_HandleTypeDef* hadc);
        void init();
        void start();

        // オフセットキャリブレーション
        void calibrate();

        // 最新値取得
        inline float getIa() const { return Ia; }
        inline float getIb() const { return Ib; }
        inline float getIc() const { return Ic; }

        // 内部更新（ADC割り込みから呼ばれる）
        void updateFromADC(ADC_HandleTypeDef* hadc);
};
#endif // CURRENTSENSE_HPP

// inline float　とは？
// - コンパイラに対して、この関数をインライン展開するよう指示するキーワード。
// - 関数呼び出しのオーバーヘッドを削減し、パフォーマンスを向上させるために使用される。
// - ただし、関数が大きすぎる場合や複雑な場合、コンパイラはインライン展開を無視することがある。