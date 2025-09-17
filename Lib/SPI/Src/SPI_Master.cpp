#include "SPI_Master.hpp"
#include <stdio.h>

Drv8301::Drv8301(SPI_HandleTypeDef* _hspi, GPIO_TypeDef* _csPort, uint16_t _csPin)
{
    this->hspi = _hspi;
    this->csPort = _csPort;
    this->csPin = _csPin;
    this->uart = nullptr; // UARTハンドラは後で設定

}

uint16_t Drv8301::readReg(DRV8301_Reg reg) {
    //  bit　 1  2  3  4  5  6  7  8  9  10  11  12  13  14  15  16
    //  R=1/W=0  |--addr--|  |-----------data(11bit)-------------|
    // Fault=1/0 |--addr--|  |-----------data(11bit)-------------|
    uint16_t cmd = DRV8301_CMD_READ | ((reg & 0xF) << DRV8301_REG_SHIFT);
    uint16_t rx = spiTransfer(cmd);
    return rx & DRV8301_DATA_MASK;
}

void Drv8301::writeReg(DRV8301_Reg reg, uint16_t data) {
    // bit　 1  2  3  4  5  6  7  8  9  10  11  12  13  14  15  16
    // R=1/W=0  |--addr--|  |-----------data(11bit)-------------|
    uint16_t cmd = DRV8301_CMD_WRITE | ((reg & 0xF) << DRV8301_REG_SHIFT) | (data & DRV8301_DATA_MASK);
    spiTransfer(cmd);
}

uint16_t Drv8301::spiTransfer(uint16_t txData) {
    uint16_t rxData = 0;
    HAL_GPIO_WritePin(csPort, csPin, GPIO_PIN_RESET); // CS Low
    HAL_SPI_TransmitReceive(hspi, (uint8_t*)&txData, (uint8_t*)&rxData, 1, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(csPort, csPin, GPIO_PIN_SET); // CS High
    return rxData;
}

// ===== High-level API =====
void Drv8301::setGain(DRV8301_CSAGain gain) {
    uint16_t ctrl2 = readReg(REG_CONTROL2);
    ctrl2 &= ~CTRL2_GAIN_Msk;
    ctrl2 |= (gain << CTRL2_GAIN_Pos);
    writeReg(REG_CONTROL2, ctrl2);

    if (uart) {
        char buf[64];
        snprintf(buf, sizeof(buf), "[DRV8301] Gain set to %d (reg=0x%04X)\r\n", gain, ctrl2);
        uart->UART_Print(buf);
    }
}

void Drv8301::resetGateDriver() {
    uint16_t ctrl1 = readReg(REG_CONTROL1);
    ctrl1 |= CTRL1_GATE_RESET_Msk;
    writeReg(REG_CONTROL1, ctrl1);

    if (uart) {
        uart->UART_Print("[DRV8301] Gate driver reset\r\n");
    }
}

void Drv8301::printStatus() {
    uint16_t s1 = readReg(REG_STATUS1);
    uint16_t s2 = readReg(REG_STATUS2);

    if (uart) {
        char buf[128];
        snprintf(buf, sizeof(buf), "[DRV8301] STATUS1=0x%04X STATUS2=0x%04X\r\n", s1, s2);
        uart->UART_Print(buf);
    }
}
