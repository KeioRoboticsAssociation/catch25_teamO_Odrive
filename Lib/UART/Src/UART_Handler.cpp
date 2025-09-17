#include "UART_Handler.hpp"


UART_Handler::UART_Handler(UART_HandleTypeDef *huart)
{
    this->huart = huart;
}

void UART_Handler::UART_Print(const char *msg)
{
    // HAL_UART_Transmit関数を使用してUARTで文字列を送信
    HAL_UART_Transmit(huart, (uint8_t *) msg, strlen(msg), HAL_MAX_DELAY);
    // 第１項：
    // 第２項：msgのポインタをuint8_t型にキャスト
    // 第３項：msgの長さを取得
    // 第４項：送信が完了するまで待機
}