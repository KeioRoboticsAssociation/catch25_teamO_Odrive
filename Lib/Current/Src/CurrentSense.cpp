#include "CurrentSense.hpp"

CurrentSense::CurrentSense(ADC_HandleTypeDef* _hadc){
    this->hadc = _hadc;
}

void CurrentSense::init() {
    // CubeMX で ADC1 (Injected Conv) を設定済み

    // ここでは初期化処理を記述する場所だけど、今回は CubeMX 側で Injected Conversion を設定しているので、コードは空。
    // Injected Conversion = 特定イベント（例：PWM トリガ）で自動的に変換される ADC モード。
    // → FOC 制御のように「PWM のスイッチングタイミングに合わせて電流を読む」のに必須。
}

void CurrentSense::start() {
    HAL_ADCEx_InjectedStart_IT(hadc);
    /*
    Injected Conversion を 割り込み付きで開始。
    ADC が変換完了すると HAL_ADCEx_InjectedConvCpltCallback() が呼ばれる。
    そこから updateFromADC() を呼び出す想定。
    */
}

// オフセットキャリブレーション処理
void CurrentSense::calibrate() {
    // DC_CAL ピンが High のときに呼び出す想定
    // ここではオフセットキャリブレーションの処理を記述する。
    // 例えば、ADC の読み取り値の平均を計算し、その値をオフセットとして保存するなど。
    HAL_Delay(10); // 安定化待ち
    uint16_t rawIa = HAL_ADCEx_InjectedGetValue(hadc, ADC_INJECTED_RANK_1);
    uint16_t rawIc = HAL_ADCEx_InjectedGetValue(hadc, ADC_INJECTED_RANK_2);
    // Injected Rank 1,2 に Ia, Ic が入っている想定
    offsetIa = (rawIa / adc_resolution) * adc_ref_voltage;
    offsetIc = (rawIc / adc_resolution) * adc_ref_voltage;

    calibrated = true; // キャリブレーション完了フラグ
}
void CurrentSense::updateFromADC(ADC_HandleTypeDef* _hadc) {
    if(_hadc->Instance == hadc->Instance) { 
        // 割り込みが来ても「どの ADC からか」を確認。ここでは ADC1 のときだけ処理。

        uint16_t rawIa = HAL_ADCEx_InjectedGetValue(_hadc, ADC_INJECTED_RANK_1);
        uint16_t rawIc = HAL_ADCEx_InjectedGetValue(_hadc, ADC_INJECTED_RANK_2);
        // Injected Rank 1,2 に Ia, Ic が入っている想定
        // シャント抵抗の電圧を ADC で読む。
        // rawIa, rawIc は 0 ～ 4095 の範囲。rawIb は計算で求める。
        // 今回は 2 相分 (Ia, Ic) を直接測定。残りの Ib は「キルヒホッフの法則」から計算する。

        // ADC → 電圧 [V]
        float vIa = (rawIa / adc_resolution) * adc_ref_voltage;
        float vIc = (rawIc / adc_resolution) * adc_ref_voltage;
        // adc_resolution = 4096 (12bit ADC)
        // adc_ref_voltage = 3.3V
        // なので raw = 2048 のとき ≈ 1.65V になる。

        // オフセット補正
        if (calibrated) {
            vIa -= offsetIa;
            vIc -= offsetIc;
        } else {
            vIa -= adc_ref_voltage / 2.0f; // 校正前は理論値で仮置き
            vIc -= adc_ref_voltage / 2.0f;
            // DRV8301 内部のアンプは 無電流時に Vref/2（約1.65V）を出力。
            // 実際の電流はこのオフセットからの差分として表れる。
        }

        
        // 電圧 [V] → 電流 [A]  (I = V / (Rshunt * Gain))
        // ΔV = R × ΔI
        // ΔI = Ia - 0
        // ΔV = Vmeas - Voffset
        Ia = vIa / (shunt_resistor * opamp_gain);
        Ic = vIc / (shunt_resistor * opamp_gain);
        Ib = -(Ia + Ic);
    }
}
