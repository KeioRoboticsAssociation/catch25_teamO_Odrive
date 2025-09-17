#include "encoder.hpp"
#include <cmath>
#include <cstdio>
#include <cstring>

Encoder::Encoder(TIM_HandleTypeDef *htim, int16_t resolution, UART_Handler* uart) {
    this->htim = htim;
    this->resolution = resolution;
    this->uart_=uart;
    this->count = 0;
    this->prev_count = 0;
    this->rotations = 0;
    this->degrees = 0;
    this->radians = 0;
    this->angularVelocity = 0;
    this->prev_degrees = 0;
    /*
    htim → CubeMX で作った TIM ハンドル
    resolution → 1 回転あたりのカウント数（PPR × 4）
    count → 累積カウント（オーバーフロー補正込み）
    prev_count → 前回カウント値を保存（差分計算に使用）
    */
}

void Encoder::start() {
    HAL_TIM_Encoder_Start(htim, TIM_CHANNEL_ALL);
    if(htim->Init.Period != 0xFFFF){
        // エラーを起こす
        Error_Handler();
    }
    /*
    TIM をエンコーダモードで起動
    16bit タイマで最大カウント 65535 を想定しているので、それ以外ならエラー
    TIM1 の ARR = 0xFFFF（65535）でない場合、想定外動作のため Error_Handler() を呼ぶ
    */
}

void Encoder::update(float dt) {
    int32_t current_count = (int16_t)__HAL_TIM_GET_COUNTER(htim);
    int32_t diff = current_count - prev_count;
    if (diff >  32767) diff -= 65536; // アンダーフロー補正
    if (diff < -32768) diff += 65536; // オーバーフロー補正
    prev_count = current_count;
    count += diff;
    /*
    TIM1 の 16bit カウンタ値を読み込み current_count に保存
    diff = current_count - prev_count で 前回からの増減 を計算
    これでオーバーフロー/アンダーフローも自然に処理できる
    count += diff で 累積カウント を更新
    prev_count = current_count で次回差分計算用に保存
    ✅ この方式の利点：
    16bit タイマのオーバーフロー（0→65535）やアンダーフロー（0→65535の逆）も補正できる
    累積回転数を整数で保持できる
    */

    // 計算して int 型にキャスト
    float tmp_rot = (float)count / (float)resolution; // 累積カウントを resolution で割る → 回転数（[rev]）
    rotations = (int)tmp_rot;
    degrees   = (int)(tmp_rot * 360.0f); // 回転数 × 360 → 角度 [°]
    radians   = (int)(tmp_rot * 2.0f * M_PI); // 回転数 × 2π → 角度 [rad]

    // 角速度計算 [deg/s]
    angularVelocity = (float)diff * (360.0f / (float)resolution) / dt;
}

int32_t Encoder::getRawCount() {
    return count;
}

// 回転数
int Encoder::getRotations() {
    return rotations;
}

// 角度
int Encoder::getDegrees() {
    return degrees;
}

// ラジアン
int Encoder::getRadians() {
    return radians;
}

int Encoder::getAngularVelocity() {
    return angularVelocity;
}

void Encoder::printEncoder(){
    char buffer[120];
    snprintf(buffer, sizeof(buffer),
             "Count: %ld, Angle: %d deg, Rotations: %d, Velocity: %d deg/s\r\n",
             getRawCount(), getDegrees(), getRotations(), getAngularVelocity());
    uart_->UART_Print(buffer);
}
