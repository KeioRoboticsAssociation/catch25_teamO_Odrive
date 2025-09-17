#include "main.hpp"
#include "CurrentSense.hpp"
#include "LED.hpp"
#include "SPI_Master.hpp"
#include "UART_Handler.hpp"
#include "Driver6PWM.hpp"
#include "encoder.hpp"
#include "cstdio"

// ==== 外部ハンドル ====
extern UART_HandleTypeDef huart2; // UART2ハンドル
extern UART_HandleTypeDef huart5; // UART5ハンドル:RS485
extern SPI_HandleTypeDef hspi1; // SPI1ハンドル
extern TIM_HandleTypeDef htim1; // encoder用TIM1ハンドル
extern TIM_HandleTypeDef htim2; // PWM用TIM2ハンドル
extern TIM_HandleTypeDef htim3; // PWM用TIM3ハンドル
extern TIM_HandleTypeDef htim10; // 100ms割り込み用TIM10ハンドル
extern ADC_HandleTypeDef hadc1; // ADC1ハンドル

// ==== オブジェクト ====
LED led1(GPIOC, GPIO_PIN_2); // PC2に接続されたLED
UART_Handler uart(&huart2);
Drv8301 drv(&hspi1, GPIOA, GPIO_PIN_4); // CS=PA4
CurrentSense cs(&hadc1); // Rshunt=0.1Ω, Gain=0.1V/A
Driver6PWM pwm(
    &htim2, TIM_CHANNEL_1, // AH = TIM2_CH1 (PA15)
    &htim2, TIM_CHANNEL_2, // AL = TIM2_CH2 (PB3)
    &htim2, TIM_CHANNEL_3, // BH = TIM2_CH3 (PB10)
    &htim3, TIM_CHANNEL_3, // BL = TIM3_CH3 (PB0)
    &htim3, TIM_CHANNEL_1, // CH = TIM3_CH1 (PA6)
    &htim3, TIM_CHANNEL_2, // CL = TIM3_CH2 (PA7)
    24.0f                  // Vbus=24V
);
// AMT102-V PPR=2048 → x4モード → resolution=8192
Encoder encoder(&htim1, 8192, &uart); // TIM1をエンコーダ用に使用

// === ピン定義 ===
// EN_GATE: PC3
// DC_CAL : PB1
// nFAULT : PC1 (割り込み入力)

// ==== 割り込み関数 ====
// ---- nFAULT割り込み ----
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
    if (GPIO_Pin == GPIO_PIN_1) { // nFAULT=PC1割り込み
        // LEDで可視化
        HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5); // 通常のnucleoのPA5のLEDをトグル
        led1.toggle();         // エラー時LED点灯
        
        // UARTに通知
        uart.UART_Print("DRV8301 nFAULT triggered!\r\n");

        drv.printStatus(); // DRV8301の状態をUARTに出力
    }
}
//---- TIM割り込み ----
void HAL_User_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM10) { // 100ms周期割り込み
        const float dt = 0.1f; // 100ms周期
        encoder.update(dt);
        encoder.printEncoder(); // エンコーダ情報を周期的に送信
    }
}

// ==== setupとloop関数 ====
void setup()
{
    // ===== 1. DC_CALでオフセットキャリブレーション =====
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET); // DC_CAL=High
    HAL_Delay(10); // 安定化待ち

    // ADC開始
    cs.start();
    // オフセットキャリブレーション実行
    cs.calibrate(); // CurrentSenseでオフセット記録

    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET); // DC_CAL=Low


    // ===== 2. DRV8301初期化 =====
    drv.setGain(GAIN_40);       // ゲイン設定
    drv.resetGateDriver();      // ゲートリセット
    drv.printStatus();          // UARTに状態を出力

    // ===== 3. EN_GATE 有効化 =====
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3, GPIO_PIN_SET); // EN_GATE=High

    // ===== 4. PWM開始 =====
    pwm.init();

    // ===== 5. その他開始 =====
    encoder.start(); // エンコーダスタート
    HAL_TIM_Base_Start_IT(&htim10); // 10ms周期割り込みスタート(TIM10)
    led1.off();
    uart.UART_Print("DRV8301 Test Start\r\n");

    HAL_Delay(10);
}

void loop()
{
    led1.on();
    // drv.printStatus(); // DRV8301の状態を周期的に送信

    // // 例: 正弦波PWMで回す
    // static float t = 0.0f;
    // float Ua = 12.0f + 12.0f * sinf(t);
    // float Ub = 12.0f + 12.0f * sinf(t - 2.094f); // -120°
    // float Uc = 12.0f + 12.0f * sinf(t + 2.094f); // +120°
    // pwm.setPwm(Ua, Ub, Uc);

    // t += 0.01f;
    // HAL_Delay(1);

    // led1.toggle();
    // HAL_Delay(500); // 500ms待機
    // uart.UART_Print("Hello, KIRATO4.0!\n");

}