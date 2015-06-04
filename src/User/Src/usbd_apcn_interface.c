/**
  ******************************************************************************
  * @file    USB_Device/APCN_Standalone/Src/usbd_APCN_interface.c
  * @author  MCD Application Team
  * @version V1.2.1
  * @date    13-March-2015
  * @brief   Source file for USBD CDC interface
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
#include <string.h>
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define APP_RX_DATA_SIZE  2048
#define APP_TX_DATA_SIZE  2048
#define APP_EP1_DATA_SIZE  1024
#define APP_EP2_DATA_SIZE  1024

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
USBD_APCN_LineCodingTypeDef LineCoding =
  {
    115200, /* baud rate*/
    0x00,   /* stop bits-1*/
    0x00,   /* parity - none*/
    0x08    /* nb. of bits 8*/
  };

uint8_t UserRxBuffer[APP_RX_DATA_SIZE];/* Received Data over USB are stored in this buffer */
uint8_t UserTxBuffer[APP_TX_DATA_SIZE];/* Received Data over UART (CDC interface) are stored in this buffer */
uint8_t UserEP1TxBuffer[APP_EP1_DATA_SIZE];//EP1Buffer
uint8_t UserEP1RxBuffer[APP_EP1_DATA_SIZE];//EP1Buffer
uint32_t UserEP1TxBufPtrIn = 0;
uint8_t UserEP2TxBuffer[APP_EP2_DATA_SIZE];//EP1Buffer
uint8_t UserEP2RxBuffer[APP_EP2_DATA_SIZE];//EP1Buffer
uint32_t UserEP2TxBufPtrIn = 0;
uint32_t BuffLength;
uint32_t UserTxBufPtrIn = 0;/* Increment this pointer or roll it back to
                               start address when data are received over USART */
uint32_t UserTxBufPtrOut = 0; /* Increment this pointer or roll it back to
                                 start address when data are sent over USB */

/* UART handler declaration */
UART_HandleTypeDef UartHandle;
/* TIM handler declaration */
TIM_HandleTypeDef  TimHandle;
/* USB handler declaration */
extern USBD_HandleTypeDef  USBD_Device;

/* Private function prototypes -----------------------------------------------*/
static int8_t APCN_Itf_Init(void);
static int8_t APCN_Itf_DeInit(void);
static int8_t APCN_Itf_Control(uint8_t cmd, uint8_t* pbuf, uint16_t length);
static int8_t APCN_Itf_Receive(uint8_t ep,uint8_t* pbuf, uint16_t *Len);

static void Error_Handler(void);
static void ComPort_Config(void);
static void TIM_Config(void);
int VCP_write(const uint8_t *pBuffer, int size,int ep);
USBD_APCN_ItfTypeDef USBD_APCN_fops =
{
  APCN_Itf_Init,
  APCN_Itf_DeInit,
  APCN_Itf_Control,
  APCN_Itf_Receive
};

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  APCN_Itf_Init
  *         Initializes the CDC media low layer
  * @param  None
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t APCN_Itf_Init(void)
{
  /*##-1- Configure the UART peripheral ######################################*/
  /* Put the USART peripheral in the Asynchronous mode (UART Mode) */
  /* USART configured as follow:
      - Word Length = 8 Bits
      - Stop Bit    = One Stop bit
      - Parity      = No parity
      - BaudRate    = 115200 baud
      - Hardware flow control disabled (RTS and CTS signals) */
  UartHandle.Instance          = USARTx;
  UartHandle.Init.BaudRate     = 115200;
  UartHandle.Init.WordLength   = UART_WORDLENGTH_8B;
  UartHandle.Init.StopBits     = UART_STOPBITS_1;
  UartHandle.Init.Parity       = UART_PARITY_NONE;
  UartHandle.Init.HwFlowCtl    = UART_HWCONTROL_NONE;
  UartHandle.Init.Mode         = UART_MODE_TX_RX;
  UartHandle.Init.OverSampling = UART_OVERSAMPLING_16;
    
  if(HAL_UART_Init(&UartHandle) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }
  
  /*##-2- Put UART peripheral in IT reception process ########################*/
  /* Any data received will be stored in "UserTxBuffer" buffer  */
  if(HAL_UART_Receive_IT(&UartHandle, (uint8_t *)UserTxBuffer, 1) != HAL_OK)
  {
    /* Transfer error in reception process */
    Error_Handler();
  }
  
  /*##-3- Configure the TIM Base generation  #################################*/
  TIM_Config();
  
  /*##-4- Start the TIM Base generation in interrupt mode ####################*/
  /* Start Channel1 */
  if(HAL_TIM_Base_Start_IT(&TimHandle) != HAL_OK)
  {
    /* Starting Error */
    Error_Handler();
  }
  
  /*##-5- Set Application Buffers ############################################*/
  USBD_APCN_SetEP1TxBuffer(&USBD_Device, UserEP1TxBuffer, 0);
  USBD_APCN_SetEP1RxBuffer(&USBD_Device, UserEP1RxBuffer);
  USBD_APCN_SetEP2TxBuffer(&USBD_Device, UserEP2TxBuffer, 0);
  USBD_APCN_SetEP2RxBuffer(&USBD_Device, UserEP2RxBuffer);
  
  return (USBD_OK);
}

/**
  * @brief  APCN_Itf_DeInit
  *         DeInitializes the CDC media low layer
  * @param  None
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t APCN_Itf_DeInit(void)
{
  /* DeInitialize the UART peripheral */
  if(HAL_UART_DeInit(&UartHandle) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }
  return (USBD_OK);
}

/**
  * @brief  APCN_Itf_Control
  *         Manage the CDC class requests
  * @param  Cmd: Command code            
  * @param  Buf: Buffer containing command data (request parameters)
  * @param  Len: Number of data to be sent (in bytes)
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t APCN_Itf_Control (uint8_t cmd, uint8_t* pbuf, uint16_t length)
{ 
  switch (cmd)
  {
  case APCN_SEND_ENCAPSULATED_COMMAND:
    /* Add your code here */
    break;

  case APCN_GET_ENCAPSULATED_RESPONSE:
    /* Add your code here */
    break;

  case APCN_SET_COMM_FEATURE:
    /* Add your code here */
    break;

  case APCN_GET_COMM_FEATURE:
    /* Add your code here */
    break;

  case APCN_CLEAR_COMM_FEATURE:
    /* Add your code here */
    break;

  case APCN_SET_LINE_CODING:
    LineCoding.bitrate    = (uint32_t)(pbuf[0] | (pbuf[1] << 8) |\
                            (pbuf[2] << 16) | (pbuf[3] << 24));
    LineCoding.format     = pbuf[4];
    LineCoding.paritytype = pbuf[5];
    LineCoding.datatype   = pbuf[6];
    
    /* Set the new configuration */
    ComPort_Config();
    break;

  case APCN_GET_LINE_CODING:
    pbuf[0] = (uint8_t)(LineCoding.bitrate);
    pbuf[1] = (uint8_t)(LineCoding.bitrate >> 8);
    pbuf[2] = (uint8_t)(LineCoding.bitrate >> 16);
    pbuf[3] = (uint8_t)(LineCoding.bitrate >> 24);
    pbuf[4] = LineCoding.format;
    pbuf[5] = LineCoding.paritytype;
    pbuf[6] = LineCoding.datatype;     
    break;

  case APCN_SET_CONTROL_LINE_STATE:
    /* Add your code here */
    break;

  case APCN_SEND_BREAK:
     /* Add your code here */
    break;    
    
  default:
    break;
  }
  
  return (USBD_OK);
}

/*
 * @liushiwei
 * 鐎规碍妞傞崳銊ユ礀鐠嬶拷
 * 娴犲抖serTxBuffer娑擃厼褰囬崙杞拌閸欙絽褰傞弶銉ф畱閺佺増宓侀敍灞藉絺闁礁鍩孶SB 缁旑垰褰涢妴锟� */
/**
  * @brief  TIM period elapsed callback
  * @param  htim: TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{

  if(UserEP1TxBufPtrIn!=0){
//	  USBD_UsrLog("HAL_TIM  UserEP1 %s\n",UserEP1TxBuffer);
  	  USBD_APCN_SetEP1TxBuffer(&USBD_Device, (uint8_t*)&UserEP1TxBuffer[0], UserEP1TxBufPtrIn);
  		if (USBD_APCN_TransmitPacket(&USBD_Device,APCN_OUT_EP1) == USBD_OK) {
  			USBD_UsrLog("HAL_TIM  UserEP1 send message OK!\n");
  		}else{
  			USBD_UsrLog("HAL_TIM  UserEP1 send message ERROR!\n");
  		}
  		UserEP1TxBufPtrIn = 0;
//	  VCP_write((uint8_t*)&UserEP1TxBuffer[0], UserEP1TxBufPtrIn,APCN_OUT_EP1);
//	  UserEP1TxBufPtrIn = 0;
  	 }
  	 if(UserEP2TxBufPtrIn!=0){
//  		USBD_UsrLog("HAL_TIM  UserEP2 %s\n",UserEP2TxBuffer);
  		  USBD_APCN_SetEP2TxBuffer(&USBD_Device, (uint8_t*)&UserEP2TxBuffer[0], UserEP2TxBufPtrIn);
  			if (USBD_APCN_TransmitPacket(&USBD_Device,APCN_OUT_EP2) == USBD_OK) {
  				USBD_UsrLog("HAL_TIM  UserEP2 send message OK!\n");
  			}else{
  				USBD_UsrLog("HAL_TIM  UserEP2 send message ERROR!\n");
  			}
  			UserEP2TxBufPtrIn = 0;
//  		VCP_write((uint8_t*)&UserEP2TxBuffer[0], UserEP2TxBufPtrIn,APCN_OUT_EP2);
//  		UserEP2TxBufPtrIn = 0;
  	 }
}

/**
  * @brief  Rx Transfer completed callback
  * @param  huart: UART handle
  * @retval None
  */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  /* Increment Index for buffer writing */
  UserTxBufPtrIn++;
  
  /* To avoid buffer overflow */
  if(UserTxBufPtrIn == APP_RX_DATA_SIZE)
  {
    UserTxBufPtrIn = 0;
  }
  
  /* Start another reception: provide the buffer pointer with offset and the buffer size */
  HAL_UART_Receive_IT(huart, (uint8_t *)(UserTxBuffer + UserTxBufPtrIn), 1);
}

/**
  * @brief  APCN_Itf_DataRx
  *         Data received over USB OUT endpoint are sent over CDC interface 
  *         through this function.
  * @param  Buf: Buffer of data to be transmitted
  * @param  Len: Number of data received (in bytes)
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t APCN_Itf_Receive(uint8_t ep,uint8_t* Buf, uint16_t *Len)
{
//  HAL_PCD_EP_Receive(&USBD_Device,
//	                       APCN_IN_EP1,
//						   Buf, *Len);

  HAL_UART_Transmit_DMA(&UartHandle, Buf, *Len);
  //USBD_UsrLog("Receive ep = %u",ep);
  if(ep==APCN_OUT_EP1){
	  memcpy(&UserEP1TxBuffer,Buf,*Len);
	  UserEP1TxBufPtrIn = *Len;
  }
  if(ep==APCN_OUT_EP2){
  	  memcpy(&UserEP2TxBuffer,Buf,*Len);
  	  UserEP2TxBufPtrIn = *Len;
  }

  return (USBD_OK);
}

/**
  * @brief  Tx Transfer completed callback
  * @param  huart: UART handle
  * @retval None
  */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
  /* Initiate next USB packet transfer once UART completes transfer (transmitting data over Tx line) */
  USBD_APCN_ReceivePacket(&USBD_Device);
}

/**
  * @brief  ComPort_Config
  *         Configure the COM Port with the parameters received from host.
  * @param  None.
  * @retval None
  * @note   When a configuration is not supported, a default value is used.
  */
static void ComPort_Config(void)
{
  if(HAL_UART_DeInit(&UartHandle) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }
  
  /* set the Stop bit */
  switch (LineCoding.format)
  {
  case 0:
    UartHandle.Init.StopBits = UART_STOPBITS_1;
    break;
  case 2:
    UartHandle.Init.StopBits = UART_STOPBITS_2;
    break;
  default :
    UartHandle.Init.StopBits = UART_STOPBITS_1;
    break;
  }
  
  /* set the parity bit*/
  switch (LineCoding.paritytype)
  {
  case 0:
    UartHandle.Init.Parity = UART_PARITY_NONE;
    break;
  case 1:
    UartHandle.Init.Parity = UART_PARITY_ODD;
    break;
  case 2:
    UartHandle.Init.Parity = UART_PARITY_EVEN;
    break;
  default :
    UartHandle.Init.Parity = UART_PARITY_NONE;
    break;
  }
  
  /*set the data type : only 8bits and 9bits is supported */
  switch (LineCoding.datatype)
  {
  case 0x07:
    /* With this configuration a parity (Even or Odd) must be set */
    UartHandle.Init.WordLength = UART_WORDLENGTH_8B;
    break;
  case 0x08:
    if(UartHandle.Init.Parity == UART_PARITY_NONE)
    {
      UartHandle.Init.WordLength = UART_WORDLENGTH_8B;
    }
    else 
    {
      UartHandle.Init.WordLength = UART_WORDLENGTH_9B;
    }
    
    break;
  default :
    UartHandle.Init.WordLength = UART_WORDLENGTH_8B;
    break;
  }
  
  UartHandle.Init.BaudRate     = LineCoding.bitrate;
  UartHandle.Init.HwFlowCtl    = UART_HWCONTROL_NONE;
  UartHandle.Init.Mode         = UART_MODE_TX_RX;
  UartHandle.Init.OverSampling = UART_OVERSAMPLING_16;
  
  if(HAL_UART_Init(&UartHandle) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }

  /* Start reception: provide the buffer pointer with offset and the buffer size */
  HAL_UART_Receive_IT(&UartHandle, (uint8_t *)(UserTxBuffer + UserTxBufPtrIn), 1);
}

/**
 * @liushiwei
 *
 *
 */
/**
  * @brief  TIM_Config: Configure TIMx timer
  * @param  None.
  * @retval None
  */
static void TIM_Config(void)
{  
  /* Set TIMx instance */
  TimHandle.Instance = TIMx;
  
  /* Initialize TIM3 peripheral as follow:
       + Period = 10000 - 1
       + Prescaler = ((SystemCoreClock/2)/10000) - 1
       + ClockDivision = 0
       + Counter direction = Up
  */
  TimHandle.Init.Period = (APCN_POLLING_INTERVAL*1000) - 1;
  TimHandle.Init.Prescaler = 84-1;
  TimHandle.Init.ClockDivision = 0;
  TimHandle.Init.CounterMode = TIM_COUNTERMODE_UP;
  if(HAL_TIM_Base_Init(&TimHandle) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }
}

/**
  * @brief  UART error callbacks
  * @param  UartHandle: UART handle
  * @retval None
  */
void HAL_UART_ErrorCallback(UART_HandleTypeDef *UartHandle)
{
  /* Transfer error occured in reception and/or transmission process */
  Error_Handler();
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
static void Error_Handler(void)
{
  /* Add your own code here */
}

int VCP_write(const uint8_t *pBuffer, int size,int ep)
{
    if (size > APCN_DATA_FS_OUT_PACKET_SIZE)
    {
        int offset;
        for (offset = 0; offset < size; offset++)
        {
            int todo = MIN(APCN_DATA_FS_OUT_PACKET_SIZE,
                           size - offset);
            int done = VCP_write(pBuffer + offset, todo,ep);
            if (done != todo)
                return offset + done;
        }

        return size;
    }

    USBD_APCN_HandleTypeDef *pCDC =
            (USBD_APCN_HandleTypeDef *)USBD_Device.pClassData;
	if (ep == APCN_OUT_EP1) {
		while (pCDC->Tx1State) {
		} //Wait for previous transfer

		USBD_APCN_SetEP1TxBuffer(&USBD_Device, (uint8_t *) pBuffer, size);
		if (USBD_APCN_TransmitPacket(&USBD_Device,ep) != USBD_OK)
			return 0;

		while (pCDC->Tx1State) {
		} //Wait until transfer is done
		return size;
	} else {
		while (pCDC->Tx2State) {
		} //Wait for previous transfer

		USBD_APCN_SetEP2TxBuffer(&USBD_Device, (uint8_t *) pBuffer, size);
		if (USBD_APCN_TransmitPacket(&USBD_Device,ep) != USBD_OK)
			return 0;

		while (pCDC->Tx2State) {
		} //Wait until transfer is done
		return size;
	}

}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
