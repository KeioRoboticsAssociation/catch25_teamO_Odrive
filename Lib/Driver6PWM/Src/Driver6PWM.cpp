#include "Driver6PWM.hpp"

// コンストラクタ
Driver6PWM::Driver6PWM(
    TIM_HandleTypeDef* _timAH, uint32_t _chAH,
    TIM_HandleTypeDef* _timAL, uint32_t _chAL,
    TIM_HandleTypeDef* _timBH, uint32_t _chBH,
    TIM_HandleTypeDef* _timBL, uint32_t _chBL,
    TIM_HandleTypeDef* _timCH, uint32_t _chCH,
    TIM_HandleTypeDef* _timCL, uint32_t _chCL,
    float _Vbus
)
{
    this->timAH = _timAH;
    this->timAH_Channel = _chAH;
    this->timAL = _timAL;
    this->timAL_Channel = _chAL;
    this->timBH = _timBH;
    this->timBH_Channel = _chBH;
    this->timBL = _timBL;
    this->timBL_Channel = _chBL;
    this->timCH = _timCH;
    this->timCH_Channel = _chCH;
    this->timCL = _timCL;
    this->timCL_Channel = _chCL;
    this->Vbus = _Vbus;
}

// PWMの初期化
void Driver6PWM::init() {
    HAL_TIM_PWM_Start(timAH, timAH_Channel); // AH
    HAL_TIM_PWM_Start(timAL, timAL_Channel); // AL
    HAL_TIM_PWM_Start(timBH, timBH_Channel); // BH
    HAL_TIM_PWM_Start(timBL, timBL_Channel); // BL
    HAL_TIM_PWM_Start(timCH, timCH_Channel); // CH
    HAL_TIM_PWM_Start(timCL, timCL_Channel); // CL
}

// PWMの有効化
void Driver6PWM::enable() {
    // DRV8301 の EN_GATE を High に設定するGPIO操作を追加しても良い
    // main.cppで行う
}

// PWMの設定
// 三相インバータに対して、入力された電圧指令値をPWMduty比に変換して出力する関数

// Ua, Ub, Uc: 各相の目標電圧 [V] (0~Vbusの範囲を想定)
// ARR: PWMの周期を決める大麻のオートリロード値（＝1周期あたりのカウント数）
// 例: 20kHzでVbus=24Vなら、ARR=4200 (84MHz/20kHz=4200)
// __HAL_TIM_SET_COMPARE: HALマクロ。タイマのキャプチャコンペアレジスタ（CCR）に値を書き込み、デューティ比を決める
void Driver6PWM::setPwm(float Ua, float Ub, float Uc) {
    // ARRはAHのタイマーから取得（全チャネル同一周期前提）
    uint32_t ARR = __HAL_TIM_GET_AUTORELOAD(timAH); // PWMの周期設定値　20kHz

    // 電圧をデューティ比に変換するラムダ関数
    /*
    (1) ラムダ式とは？
    C++ では「無名関数（名前を持たない関数オブジェクト）」を作れる構文を ラムダ式 と呼びます。
    書式：   [capture](引数) -> 戻り値型 {
                // 関数の中身
            }
    ここでの [&] は「外部の変数を参照渡しでキャプチャ」する意味です。
    つまり、外側にある Vbus や ARR をそのまま使えます。

    (2) このラムダの役割
    ・U = 電圧指令値を PWM デューティ比（CCRの値）に変換。
    ・範囲外（0以下 or Vbus以上）の値はクリップ。
    ・U / Vbus がデューティ比（0.0～1.0）に相当し、それを ARR でスケーリングして整数に変換。
    例：
    Vbus = 24 V, ARR = 1000, U = 12 V の場合
    → U / Vbus = 0.5
    → duty = 0.5 * 1000 = 500 (50%デューティ)。

    (3) なぜラムダにしたのか？
    もしラムダを使わなかった場合：
    uint32_t toDuty(float U) {
        if(U < 0) U = 0;
        if(U > Vbus) U = Vbus;
        return static_cast<uint32_t>((U / Vbus) * ARR);
    }
    でも良いですが、
    関数をファイルの外に切り出す必要がなく、ローカル専用で使うのに便利。
    [&] で外の変数をキャプチャできるため、Vbus や ARR を引数に渡さなくて済む。
    なので「この関数の中でだけ使う変換処理」をシンプルに書くのにラムダが適しています。
    */
    auto toDuty = [&](float U) -> uint32_t {
        if(U < 0) U = 0;
        if(U > Vbus) U = Vbus;
        return static_cast<uint32_t>((U / Vbus) * ARR);
    };

    uint32_t dutyA = toDuty(Ua);
    uint32_t dutyB = toDuty(Ub);
    uint32_t dutyC = toDuty(Uc);

    /*
    これは「補完PWM（Complementary PWM）」を実現するためで、
    AH が ON のとき AL は OFF
    AH が OFF のとき AL は ON
    というように必ず対になるようにしています。
    これによりハーフブリッジで同時導通を避け、モータを正しく駆動します。
    */
    // Set PWM duty cycles
    // Phase A
    __HAL_TIM_SET_COMPARE(timAH, timAH_Channel, dutyA); // AH
    __HAL_TIM_SET_COMPARE(timAL, timAL_Channel, ARR - dutyA); // AL
    // Phase B
    __HAL_TIM_SET_COMPARE(timBH, timBH_Channel, dutyB); // BH
    __HAL_TIM_SET_COMPARE(timBL, timBL_Channel, ARR - dutyB); // BL
    // Phase C
    __HAL_TIM_SET_COMPARE(timCH, timCH_Channel, dutyC); // CH
    __HAL_TIM_SET_COMPARE(timCL, timCL_Channel, ARR - dutyC); // CL
}