/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define nFAULT_Pin GPIO_PIN_1
#define nFAULT_GPIO_Port GPIOC
#define nFAULT_EXTI_IRQn EXTI1_IRQn
#define LED_Pin GPIO_PIN_2
#define LED_GPIO_Port GPIOC
#define EN_GATE_Pin GPIO_PIN_3
#define EN_GATE_GPIO_Port GPIOC
#define SPI__NSS_CS__Pin GPIO_PIN_4
#define SPI__NSS_CS__GPIO_Port GPIOA
#define phB_l_Pin GPIO_PIN_0
#define phB_l_GPIO_Port GPIOB
#define DC_CAL_Pin GPIO_PIN_1
#define DC_CAL_GPIO_Port GPIOB
#define phB_h_Pin GPIO_PIN_10
#define phB_h_GPIO_Port GPIOB
#define phC_h_Pin GPIO_PIN_6
#define phC_h_GPIO_Port GPIOC
#define phC_l_Pin GPIO_PIN_7
#define phC_l_GPIO_Port GPIOC
#define phA_h_Pin GPIO_PIN_15
#define phA_h_GPIO_Port GPIOA
#define UART5_TX_Pin GPIO_PIN_12
#define UART5_TX_GPIO_Port GPIOC
#define UART5_RX_Pin GPIO_PIN_2
#define UART5_RX_GPIO_Port GPIOD
#define phA_l_Pin GPIO_PIN_3
#define phA_l_GPIO_Port GPIOB
#define Button_Pin GPIO_PIN_6
#define Button_GPIO_Port GPIOB
#define Button_EXTI_IRQn EXTI9_5_IRQn

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
