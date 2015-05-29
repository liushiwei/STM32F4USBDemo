/**
  ******************************************************************************
  * @file    usbd_template_core.h
  * @author  MCD Application Team
  * @version V2.4.0
  * @date    28-February-2015
  * @brief   Header file for the usbd_template_core.c file.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2015 STMicroelectronics</center></h2>
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

/* Define to prevent recursive inclusion -------------------------------------*/ 
#ifndef __USB_APCN_CORE_H
#define __USB_APCN_CORE_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include  "usbd_ioreq.h"

/** @addtogroup STM32_USB_DEVICE_LIBRARY
  * @{
  */
  
/** @defgroup USBD_APCN
  * @brief This file is the header file for usbd_template_core.c
  * @{
  */ 


/** @defgroup USBD_APCN_Exported_Defines
  * @{
  */ 
#define APCN_EPIN_ADDR                 0x81
#define APCN_EPIN_SIZE                 0x10

#define USB_APCN_CONFIG_DESC_SIZ       53

 /**
  * @liushiwei
  * define eps
  */
 #define APCN_IN_EP1                                   0x81  /* EP1 for data IN */
 #define APCN_OUT_EP1                                  0x01  /* EP1 for data OUT */
 #define APCN_IN_EP2                                  0x82  /* EP2 for APCN commands */
 #define APCN_OUT_EP2                                  0x02  /* EP1 for data OUT */
#define APCN_CMD_EP                                  0x83  /* EP1 for data OUT */

 /* APCN Endpoints parameters: you can fine tune these values depending on the needed baudrates and performance. */
 #define APCN_DATA_HS_MAX_PACKET_SIZE                 512  /* Endpoint IN & OUT Packet size */
 #define APCN_DATA_FS_MAX_PACKET_SIZE                 64  /* Endpoint IN & OUT Packet size */
 #define APCN_CMD_PACKET_SIZE                         8  /* Control Endpoint Packet size */

 #define USB_APCN_CONFIG_DESC_SIZ                     67
 #define USB_APCN_CUSTOM_CONFIG_DESC_SIZ              46
 #define APCN_DATA_HS_IN_PACKET_SIZE                  APCN_DATA_HS_MAX_PACKET_SIZE
 #define APCN_DATA_HS_OUT_PACKET_SIZE                 APCN_DATA_HS_MAX_PACKET_SIZE

 #define APCN_DATA_FS_IN_PACKET_SIZE                  APCN_DATA_FS_MAX_PACKET_SIZE
 #define APCN_DATA_FS_OUT_PACKET_SIZE                 APCN_DATA_FS_MAX_PACKET_SIZE
/**
  * @}
  */ 




 typedef struct _USBD_APCN_Itf
 {
   int8_t (* Init)          (void);
   int8_t (* DeInit)        (void);
   int8_t (* Control)       (uint8_t, uint8_t * , uint16_t);
   int8_t (* Receive)       (uint8_t *, uint32_t *);

 }USBD_APCN_ItfTypeDef;


 typedef struct
 {
   uint32_t data[APCN_DATA_HS_MAX_PACKET_SIZE/4];      /* Force 32bits alignment */
   uint8_t  CmdOpCode;
   uint8_t  CmdLength;
   uint8_t  *RxBuffer;
   uint8_t  *TxBuffer;
   uint32_t RxLength;
   uint32_t TxLength;

   __IO uint32_t TxState;
   __IO uint32_t RxState;
 }
 USBD_APCN_HandleTypeDef;


 /*---------------------------------------------------------------------*/
 /*  APCN definitions                                                    */
 /*---------------------------------------------------------------------*/
 #define APCN_SEND_ENCAPSULATED_COMMAND               0x00
 #define APCN_GET_ENCAPSULATED_RESPONSE               0x01
 #define APCN_SET_COMM_FEATURE                        0x02
 #define APCN_GET_COMM_FEATURE                        0x03
 #define APCN_CLEAR_COMM_FEATURE                      0x04
 #define APCN_SET_LINE_CODING                         0x20
 #define APCN_GET_LINE_CODING                         0x21
 #define APCN_SET_CONTROL_LINE_STATE                  0x22
 #define APCN_SEND_BREAK                              0x23

 /**
   * @}
   */
 typedef struct
 {
   uint32_t bitrate;
   uint8_t  format;
   uint8_t  paritytype;
   uint8_t  datatype;
 }USBD_APCN_LineCodingTypeDef;
/** @defgroup USBD_CORE_Exported_TypesDefinitions
  * @{
  */

/**
  * @}
  */ 



/** @defgroup USBD_CORE_Exported_Macros
  * @{
  */ 

/**
  * @}
  */ 

/** @defgroup USBD_CORE_Exported_Variables
  * @{
  */ 

extern USBD_ClassTypeDef  USBD_APCN_ClassDriver;
#define USBD_APCN_CLASS    &USBD_APCN_ClassDriver
/**
  * @}
  */ 

/** @defgroup USB_CORE_Exported_Functions
  * @{
  */ 
/**
  * @}
  */ 

#ifdef __cplusplus
}
#endif

#endif  /* __USB_APCN_CORE_H */
/**
  * @}
  */ 

/**
  * @}
  */ 
  
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
