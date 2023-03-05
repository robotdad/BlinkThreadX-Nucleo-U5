/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    app_threadx.c
  * @author  MCD Application Team
  * @brief   ThreadX applicative file
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
#include "app_threadx.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "main.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define THREAD_STACK_SIZE 1024
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
uint8_t red_thread_stack[THREAD_STACK_SIZE];
TX_THREAD red_thread_ptr;
uint8_t green_thread_stack[THREAD_STACK_SIZE];
TX_THREAD green_thread_ptr;
uint8_t blue_thread_stack[THREAD_STACK_SIZE];
TX_THREAD blue_thread_ptr;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */
VOID red_thread(ULONG initial_input);
VOID green_thread(ULONG initial_input);
VOID blue_thread(ULONG initial_input);
/* USER CODE END PFP */

/**
  * @brief  Application ThreadX Initialization.
  * @param memory_ptr: memory pointer
  * @retval int
  */
UINT App_ThreadX_Init(VOID *memory_ptr)
{
  UINT ret = TX_SUCCESS;
  TX_BYTE_POOL *byte_pool = (TX_BYTE_POOL*)memory_ptr;

   /* USER CODE BEGIN App_ThreadX_MEM_POOL */
  (void)byte_pool;
  /* USER CODE END App_ThreadX_MEM_POOL */
  tx_thread_create(&red_thread_ptr, "Red LED", red_thread, 0x1234, red_thread_stack, THREAD_STACK_SIZE, 15, 15, 1, TX_AUTO_START);
  tx_thread_create(&green_thread_ptr, "Green LED", green_thread, 0x1234, green_thread_stack, THREAD_STACK_SIZE, 15, 15, 1, TX_AUTO_START);
  tx_thread_create(&blue_thread_ptr, "Blue LED", blue_thread, 0x1234, blue_thread_stack, THREAD_STACK_SIZE, 15, 15, 1, TX_AUTO_START);
  /* USER CODE BEGIN App_ThreadX_Init */
  /* USER CODE END App_ThreadX_Init */

  return ret;
}

  /**
  * @brief  MX_ThreadX_Init
  * @param  None
  * @retval None
  */
void MX_ThreadX_Init(void)
{
  /* USER CODE BEGIN  Before_Kernel_Start */

  /* USER CODE END  Before_Kernel_Start */

  tx_kernel_enter();

  /* USER CODE BEGIN  Kernel_Start_Error */

  /* USER CODE END  Kernel_Start_Error */
}

/* USER CODE BEGIN 1 */
VOID red_thread(ULONG initial_input)
{
  while (1)
  {
    HAL_GPIO_TogglePin(LED_RED_GPIO_Port, LED_RED_Pin);
    tx_thread_sleep(50);
  }
}
VOID green_thread(ULONG initial_input)
{
  while (1)
  {
    HAL_GPIO_TogglePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin);
    tx_thread_sleep(50);
  }
}
VOID blue_thread(ULONG initial_input)
{
  while (1)
  {
    HAL_GPIO_TogglePin(LED_BLUE_GPIO_Port, LED_BLUE_Pin);
    tx_thread_sleep(50);
  }
}
/* USER CODE END 1 */
