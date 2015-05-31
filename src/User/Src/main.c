/**
  ******************************************************************************
  * @file    USB_Device/CDC_Standalone/Src/main.c
  * @author  MCD Application Team
  * @version V1.2.1
  * @date    13-March-2015
  * @brief   USB device CDC application main file
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2015 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
USBD_HandleTypeDef  USBD_Device;
extern UART_HandleTypeDef UartHandle;
/* Private function prototypes -----------------------------------------------*/
static void SystemClock_Config(void);
static void Toggle_Leds(void);
/* In stdio.h file is everything related to output stream */
#include "stdio.h"
#define __STDC_HOSTED__ 1
/* We need to implement own __FILE struct */
/* FILE struct is used from __FILE */
struct __FILE {
    int dummy;
};

/* You need this if you want use printf */
/* Struct FILE is implemented in stdio.h */
FILE __stdout;


int fputc(int ch, FILE *f)
{
	 HAL_UART_Transmit_DMA(&UartHandle, &ch, 1);
     return (ch);
}
/* Private functions ---------------------------------------------------------*/ 

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
  /* STM32F4xx HAL library initialization:
       - Configure the Flash prefetch, instruction and Data caches
       - Configure the Systick to generate an interrupt each 1 msec
       - Set NVIC Group Priority to 4
       - Global MSP (MCU Support Package) initialization
     */
  HAL_Init();
  
  /* Configure the system clock to 168 MHz */
  SystemClock_Config();
  
  /* Configure LED1, LED2, LED3 and LED4 */
  BSP_LED_Init(LED4);
  BSP_LED_Init(LED3);
  BSP_LED_Init(LED5);
  BSP_LED_Init(LED6);
  
  //BSP_LED_Toggle(LED4);
  /* Init Device Library */
  USBD_Init(&USBD_Device, &VCP_Desc, 0);
  
  /* Add Supported Class */
  USBD_RegisterClass(&USBD_Device, USBD_APCN_CLASS);
  
  /* Add CDC Interface Class */
  USBD_APCN_RegisterInterface(&USBD_Device, &USBD_APCN_fops);
  
  /* Start Device Process */
  USBD_Start(&USBD_Device);
  
  /* Run Application (Interrupt mode) */
  printf("USART1 Stream\n");
  while (1)
  {
    Toggle_Leds();
  }
}

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow : 
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 168000000
  *            HCLK(Hz)                       = 168000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 4
  *            APB2 Prescaler                 = 2
  *            HSE Frequency(Hz)              = 25000000
  *            PLL_M                          = 25
  *            PLL_N                          = 336
  *            PLL_P                          = 2
  *            PLL_Q                          = 7
  *            VDD(V)                         = 3.3
  *            Main regulator output voltage  = Scale1 mode
  *            Flash Latency(WS)              = 5
  * @param  None
  * @retval None
  */
static void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

  /* Enable Power Control clock */
  __HAL_RCC_PWR_CLK_ENABLE();

  /* The voltage scaling allows optimizing the power consumption when the device is 
     clocked below the maximum system frequency, to update the voltage scaling value 
     regarding system frequency refer to product datasheet.  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /* Configure RCC Oscillators: All parameters can be changed according to user�s needs */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  HAL_RCC_OscConfig (&RCC_OscInitStruct);
  
  /* RCC Clocks: All parameters can be changed according to user�s needs */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_SYSCLK |RCC_CLOCKTYPE_HCLK |RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;  
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;  
  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5);
  
  /* STM32F405x/407x/415x/417eeeex Revision Z devices: prefetch is supported  */
  if (HAL_GetREVID() == 0x1001)
  {
    /* Enable the Flash prefetch */
    __HAL_FLASH_PREFETCH_BUFFER_ENABLE();
  }
}

/**
  * @brief  Toggles LEDs.
  * @param  None
  * @retval None
  */
static void Toggle_Leds(void)
{
	uint8_t togglecounter = 0x00;
	/* Configure LEDs to be managed by GPIO */
	BSP_LED_Init(LED4);
	BSP_LED_Init(LED3);
	BSP_LED_Init(LED5);
	BSP_LED_Init(LED6);

	/* SysTick end of count event each 10ms */
	SystemCoreClock = HAL_RCC_GetHCLKFreq();
	SysTick_Config(SystemCoreClock / 100);

	/* Turn OFF all LEDs */
	BSP_LED_Off(LED4);
	BSP_LED_Off(LED3);
	BSP_LED_Off(LED5);
	BSP_LED_Off(LED6);

	/* Toggle LED4 */
	HAL_Delay(20);
	BSP_LED_Toggle(LED4);
	HAL_Delay(10);
	/* Toggle LED4 */
	BSP_LED_Toggle(LED3);
	HAL_Delay(10);
	/* Toggle LED4 */
	BSP_LED_Toggle(LED5);
	HAL_Delay(10);
	/* Toggle LED4 */
	BSP_LED_Toggle(LED6);
	HAL_Delay(10);
	togglecounter++;
	if (togglecounter == 0x10) {
		togglecounter = 0x00;
		while (togglecounter < 0x10) {
			//BSP_LED_Toggle(LED4);
			BSP_LED_Toggle(LED3);
			BSP_LED_Toggle(LED5);
			BSP_LED_Toggle(LED6);
			HAL_Delay(10);
			togglecounter++;
		}
		togglecounter = 0x00;
	}
}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
