/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file    usart.c
 * @brief   This file provides code for the configuration
 *          of the USART instances.
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2023 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "usart.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

UART_HandleTypeDef huart3;
UART_HandleTypeDef huart5;
UART_HandleTypeDef huart6;

/* USART3 init function */

void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 19200;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  huart3.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart3.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart3.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart3, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart3, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

}
/* USART5 init function */

void MX_USART5_UART_Init(void)
{

  /* USER CODE BEGIN USART5_Init 0 */

  /* USER CODE END USART5_Init 0 */

  /* USER CODE BEGIN USART5_Init 1 */

  /* USER CODE END USART5_Init 1 */
  huart5.Instance = USART5;
  huart5.Init.BaudRate = 115200;
  huart5.Init.WordLength = UART_WORDLENGTH_8B;
  huart5.Init.StopBits = UART_STOPBITS_1;
  huart5.Init.Parity = UART_PARITY_NONE;
  huart5.Init.Mode = UART_MODE_TX_RX;
  huart5.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart5.Init.OverSampling = UART_OVERSAMPLING_16;
  huart5.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart5.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart5.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart5) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART5_Init 2 */

  /* USER CODE END USART5_Init 2 */

}
/* USART6 init function */

void MX_USART6_UART_Init(void)
{

  /* USER CODE BEGIN USART6_Init 0 */

  /* USER CODE END USART6_Init 0 */

  /* USER CODE BEGIN USART6_Init 1 */

  /* USER CODE END USART6_Init 1 */
  huart6.Instance = USART6;
  huart6.Init.BaudRate = 115200;
  huart6.Init.WordLength = UART_WORDLENGTH_8B;
  huart6.Init.StopBits = UART_STOPBITS_1;
  huart6.Init.Parity = UART_PARITY_NONE;
  huart6.Init.Mode = UART_MODE_TX_RX;
  huart6.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart6.Init.OverSampling = UART_OVERSAMPLING_16;
  huart6.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart6.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart6.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart6) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART6_Init 2 */

  /* USER CODE END USART6_Init 2 */

}

void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};
  if(uartHandle->Instance==USART3)
  {
  /* USER CODE BEGIN USART3_MspInit 0 */

  /* USER CODE END USART3_MspInit 0 */

  /** Initializes the peripherals clocks
  */
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART3;
    PeriphClkInit.Usart3ClockSelection = RCC_USART3CLKSOURCE_PCLK1;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
    {
      Error_Handler();
    }

    /* USART3 clock enable */
    __HAL_RCC_USART3_CLK_ENABLE();

    __HAL_RCC_GPIOC_CLK_ENABLE();
    /**USART3 GPIO Configuration
    PC11     ------> USART3_RX
    PC10     ------> USART3_TX
    */
    GPIO_InitStruct.Pin = RS485_RX_Pin|RS485_TX_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF0_USART3;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    /* USART3 interrupt Init */
    HAL_NVIC_SetPriority(USART3_4_5_6_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USART3_4_5_6_IRQn);
  /* USER CODE BEGIN USART3_MspInit 1 */

  /* USER CODE END USART3_MspInit 1 */
  }
  else if(uartHandle->Instance==USART5)
  {
  /* USER CODE BEGIN USART5_MspInit 0 */

  /* USER CODE END USART5_MspInit 0 */

    /* USART5 clock enable */
    __HAL_RCC_USART5_CLK_ENABLE();

    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**USART5 GPIO Configuration
    PB0     ------> USART5_TX
    PB1     ------> USART5_RX
    */
    GPIO_InitStruct.Pin = OPTICAL_TX_Pin|OPTICAL_RX_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF8_USART5;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* USART5 interrupt Init */
    HAL_NVIC_SetPriority(USART3_4_5_6_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USART3_4_5_6_IRQn);
  /* USER CODE BEGIN USART5_MspInit 1 */

  /* USER CODE END USART5_MspInit 1 */
  }
  else if(uartHandle->Instance==USART6)
  {
  /* USER CODE BEGIN USART6_MspInit 0 */

  /* USER CODE END USART6_MspInit 0 */

    /* USART6 clock enable */
    __HAL_RCC_USART6_CLK_ENABLE();

    __HAL_RCC_GPIOC_CLK_ENABLE();
    /**USART6 GPIO Configuration
    PC0     ------> USART6_TX
    PC1     ------> USART6_RX
    */
    GPIO_InitStruct.Pin = GSM_TX_Pin|GSM_RX_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF4_USART6;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    /* USART6 interrupt Init */
    HAL_NVIC_SetPriority(USART3_4_5_6_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USART3_4_5_6_IRQn);
  /* USER CODE BEGIN USART6_MspInit 1 */

  /* USER CODE END USART6_MspInit 1 */
  }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{

  if(uartHandle->Instance==USART3)
  {
  /* USER CODE BEGIN USART3_MspDeInit 0 */

  /* USER CODE END USART3_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART3_CLK_DISABLE();

    /**USART3 GPIO Configuration
    PC11     ------> USART3_RX
    PC10     ------> USART3_TX
    */
    HAL_GPIO_DeInit(GPIOC, RS485_RX_Pin|RS485_TX_Pin);

    /* USART3 interrupt Deinit */
  /* USER CODE BEGIN USART3:USART3_4_5_6_IRQn disable */
		/**
		 * Uncomment the line below to disable the "USART3_4_5_6_IRQn" interrupt
		 * Be aware, disabling shared interrupt may affect other IPs
		 */
		/* HAL_NVIC_DisableIRQ(USART3_4_5_6_IRQn); */
  /* USER CODE END USART3:USART3_4_5_6_IRQn disable */

  /* USER CODE BEGIN USART3_MspDeInit 1 */
		/* USART3 interrupt Deinit */
		HAL_NVIC_DisableIRQ(USART3_4_5_6_IRQn);
  /* USER CODE END USART3_MspDeInit 1 */
  }
  else if(uartHandle->Instance==USART5)
  {
  /* USER CODE BEGIN USART5_MspDeInit 0 */

  /* USER CODE END USART5_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART5_CLK_DISABLE();

    /**USART5 GPIO Configuration
    PB0     ------> USART5_TX
    PB1     ------> USART5_RX
    */
    HAL_GPIO_DeInit(GPIOB, OPTICAL_TX_Pin|OPTICAL_RX_Pin);

    /* USART5 interrupt Deinit */
  /* USER CODE BEGIN USART5:USART3_4_5_6_IRQn disable */
		/**
		 * Uncomment the line below to disable the "USART3_4_5_6_IRQn" interrupt
		 * Be aware, disabling shared interrupt may affect other IPs
		 */
		/* HAL_NVIC_DisableIRQ(USART3_4_5_6_IRQn); */
  /* USER CODE END USART5:USART3_4_5_6_IRQn disable */

  /* USER CODE BEGIN USART5_MspDeInit 1 */
		HAL_NVIC_DisableIRQ(USART3_4_5_6_IRQn);
  /* USER CODE END USART5_MspDeInit 1 */
  }
  else if(uartHandle->Instance==USART6)
  {
  /* USER CODE BEGIN USART6_MspDeInit 0 */

  /* USER CODE END USART6_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART6_CLK_DISABLE();

    /**USART6 GPIO Configuration
    PC0     ------> USART6_TX
    PC1     ------> USART6_RX
    */
    HAL_GPIO_DeInit(GPIOC, GSM_TX_Pin|GSM_RX_Pin);

    /* USART6 interrupt Deinit */
  /* USER CODE BEGIN USART6:USART3_4_5_6_IRQn disable */
		/**
		 * Uncomment the line below to disable the "USART3_4_5_6_IRQn" interrupt
		 * Be aware, disabling shared interrupt may affect other IPs
		 */
		/* HAL_NVIC_DisableIRQ(USART3_4_5_6_IRQn); */
  /* USER CODE END USART6:USART3_4_5_6_IRQn disable */

  /* USER CODE BEGIN USART6_MspDeInit 1 */

  /* USER CODE END USART6_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
