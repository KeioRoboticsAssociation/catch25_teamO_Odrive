#ifndef SPI_MASTER_HPP
#define SPI_MASTER_HPP

#pragma once

#include "main.h"
#include <string.h>
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_spi.h"
#include <stm32f446xx.h>
#include "spi.h"
#include "LED.hpp"
#include "UART_Handler.hpp"

// ========== SPI Command format ==========
// [15]   : 1=Read, 0=Write
// [14:11]: Register Address (4bit)
// [10:0] : Data (11bit)

// コマンド定義
/*  ?U : 符号なし整数定数にするためにUを付与
    << : 左シフト演算子
    1U << 15 : 1を15ビット左にシフト（0x8000）
    0U << 15 : 0を15ビット左にシフト（0x0000）
    11bitデータマスク : 0x07FF (11ビット分の1を立てたもの)
*/
#define DRV8301_CMD_READ   (1U << 15)
#define DRV8301_CMD_WRITE  (0U << 15)
#define DRV8301_REG_SHIFT  11
#define DRV8301_DATA_MASK  0x07FF // 11bit data mask

// ========== Register Map ==========
enum DRV8301_Reg {
    REG_STATUS1  = 0x00,
    REG_STATUS2  = 0x01,
    REG_CONTROL1 = 0x02,
    REG_CONTROL2 = 0x03
};

// ===== Control Register 1 Fields =====
// (bit positions in the 11-bit DATA part)

// Gate driver peak current setting (bits[1:0])
#define CTRL1_GATE_CURRENT_Pos   0
#define CTRL1_GATE_CURRENT_Msk   (0x03 << CTRL1_GATE_CURRENT_Pos)

// Gate driver reset (bit[2])
#define CTRL1_GATE_RESET_Pos     2
#define CTRL1_GATE_RESET_Msk     (0x01 << CTRL1_GATE_RESET_Pos)

// PWM mode (bit[3]) → 0: 6-PWM, 1: 3-PWM
#define CTRL1_PWM_MODE_Pos       3
#define CTRL1_PWM_MODE_Msk       (0x01 << CTRL1_PWM_MODE_Pos)

// Overcurrent mode (bits[5:4])
#define CTRL1_OC_MODE_Pos        4
#define CTRL1_OC_MODE_Msk        (0x03 << CTRL1_OC_MODE_Pos)

// Overcurrent adjust set (bits[9:6])
#define CTRL1_OC_ADJ_SET_Pos     6
#define CTRL1_OC_ADJ_SET_Msk     (0x0F << CTRL1_OC_ADJ_SET_Pos)

// ===== Control Register 2 Fields(11 bit data) =====
// Overcurrent protection action (bits[1:0])
#define CTRL2_OCP_ACT_Pos        0
#define CTRL2_OCP_ACT_Msk        (0x03 << CTRL2_OCP_ACT_Pos)

// Current shunt amplifier gain (bits[10:9])
#define CTRL2_GAIN_Pos           9
#define CTRL2_GAIN_Msk           (0x03 << CTRL2_GAIN_Pos)

// Gain selection
/*
    enum : 列挙型の定義
*/
enum DRV8301_CSAGain {
    GAIN_10 = 0, // 10V/V
    GAIN_20 = 1, // 20V/V
    GAIN_40 = 2, // 40V/V
    GAIN_80 = 3  // 80V/V
};

class Drv8301 {
public:
    Drv8301(SPI_HandleTypeDef* _hspi, GPIO_TypeDef* _csPort, uint16_t _csPin);

    // 低レベル API（生レジスタ read/write）
    /*
        なぜvoidでなくuint16_tを返すのか？
        readReg()はレジスタの内容を読み取るため、読み取った値(=16bitのレジスタデータ)を返す必要がある。
        voidにすると、読み出した値をどこにも渡せなくなる。
        反対に、
        writeReg()はレジスタに値を書き込むため、戻り値は不要である。
    */
    uint16_t readReg(DRV8301_Reg reg); // 「レジスタの内容を読む」関数。
    void writeReg(DRV8301_Reg reg, uint16_t data);

    // 高レベルAPI（使いやすい抽象化された関数）
    void setGain(DRV8301_CSAGain gain); // CSAゲイン設定。ADCの分解能を考慮して選択
    void resetGateDriver(); // ゲートドライバリセット
    void printStatus();  // sutatusレジスタをUARTに出力

private:
    SPI_HandleTypeDef* hspi;
    GPIO_TypeDef* csPort;
    uint16_t csPin;

    uint16_t spiTransfer(uint16_t txData); // spiTransfer() が CS制御つきの1トランザクションを担当

     UART_Handler* uart;
};
#endif // SPI_MASTER_HPP