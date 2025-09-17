#ifndef ENCODER_HPP
#define ENCODER_HPP

#pragma once

#include "main.h"
#include <string.h>
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_tim.h"
#include <stm32f446xx.h>
#include <cstdint>
#include "tim.h"
#include "UART_Handler.hpp"

// エンコーダの基本的な角度や回転数を扱う

class Encoder {
    private:
        TIM_HandleTypeDef *htim; // 使用するTIM
        uint16_t channel; // 将来的にZ相やPWM読取拡張したい場合用
        int16_t resolution; // 1回転あたりのカウント数

        int32_t count; // 現在のカウント値キャッシュ
        int16_t prev_count;  // 速度演算用に前回値を保存する領域

        int rotations;
        int degrees;
        int radians;
        int angularVelocity; // 角速度[度/秒]
        int prev_degrees; // 速度演算用に前回値を保存する領域

        UART_Handler* uart_;
    public:
        Encoder(TIM_HandleTypeDef *htim, int16_t resolution, UART_Handler* uart);
        /*
        htim → CubeMXで生成された TIM のハンドルを受け取る
        resolution → 1回転あたりの カウント数（PPR×4） を受け取る
        例: AMT102-V PPR=2048 → 8192
        */ 

        void start();
        void update(float dt); // 毎ループで呼ぶ, Δt[秒]を渡して更新

        // カウント取得
        int32_t getRawCount(); // 生のカウント値
        int getRotations(); // 回転数[回転]
        int getDegrees(); // 角度[°]
        int getRadians(); // 角度[rad]
        // TIMカウンタから現在値を取得し、resolution を基準に回転数や角度に変換。

        int getAngularVelocity(); // 角速度[度/秒]
        // 速度は update() 内で計算される。dt[秒]を正しく渡すこと。

        //UART出力
        void printEncoder();
};

#endif // ENCODER_HPP