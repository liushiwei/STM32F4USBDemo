/**
  ******************************************************************************
  * @file    usbd_template.c
  * @author  MCD Application Team
  * @version V2.4.0
  * @date    28-February-2015
  * @brief   This file provides the HID core functions.
  *
  * @verbatim
  *
  *          ===================================================================
  *                                APCN Class  Description
  *          ===================================================================
  *
  *
  *
  *
  *
  *
  * @note     In HS mode and when the DMA is used, all variables and data structures
  *           dealing with the DMA during the transaction process should be 32-bit aligned.
  *
  *
  *  @endverbatim
  *
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

/* Includes ------------------------------------------------------------------*/
#include "usbd_desc.h"
#include "usbd_ctlreq.h"
#include "../Inc/usbd_apcn.h"


/** @addtogroup STM32_USB_DEVICE_LIBRARY
  * @{
  */


/** @defgroup USBD_APCN
  * @brief usbd core module
  * @{
  */

/** @defgroup USBD_APCN_Private_TypesDefinitions
  * @{
  */
/**
  * @}
  */


/** @defgroup USBD_APCN_Private_Defines
  * @{
  */

/**
  * @}
  */


/** @defgroup USBD_APCN_Private_Macros
  * @{
  */

/**
  * @}
  */




/** @defgroup USBD_APCN_Private_FunctionPrototypes
  * @{
  */


static uint8_t  USBD_APCN_Init (USBD_HandleTypeDef *pdev,
                               uint8_t cfgidx);

static uint8_t  USBD_APCN_DeInit (USBD_HandleTypeDef *pdev,
                                 uint8_t cfgidx);

static uint8_t  USBD_APCN_Setup (USBD_HandleTypeDef *pdev,
                                USBD_SetupReqTypedef *req);

static uint8_t  *USBD_APCN_GetCfgDesc (uint16_t *length);

static uint8_t  *USBD_APCN_GetDeviceQualifierDesc (uint16_t *length);

static uint8_t  USBD_APCN_DataIn (USBD_HandleTypeDef *pdev, uint8_t epnum);

static uint8_t  USBD_APCN_DataOut (USBD_HandleTypeDef *pdev, uint8_t epnum);

static uint8_t  USBD_APCN_EP0_RxReady (USBD_HandleTypeDef *pdev);

static uint8_t  USBD_APCN_EP0_TxReady (USBD_HandleTypeDef *pdev);

static uint8_t  USBD_APCN_SOF (USBD_HandleTypeDef *pdev);

static uint8_t  USBD_APCN_IsoINIncomplete (USBD_HandleTypeDef *pdev, uint8_t epnum);

static uint8_t  USBD_APCN_IsoOutIncomplete (USBD_HandleTypeDef *pdev, uint8_t epnum);

/**
  * @}
  */

/** @defgroup USBD_APCN_Private_Variables
  * @{
  */

USBD_ClassTypeDef  USBD_APCN_ClassDriver =
{
  USBD_APCN_Init,
  USBD_APCN_DeInit,
  USBD_APCN_Setup,
  USBD_APCN_EP0_TxReady,
  USBD_APCN_EP0_RxReady,
  USBD_APCN_DataIn,
  USBD_APCN_DataOut,
  USBD_APCN_SOF,
  USBD_APCN_IsoINIncomplete,
  USBD_APCN_IsoOutIncomplete,
  USBD_APCN_GetCfgDesc,
  USBD_APCN_GetCfgDesc,
  USBD_APCN_GetCfgDesc,
  USBD_APCN_GetDeviceQualifierDesc,
};

#if defined ( __ICCARM__ ) /*!< IAR Compiler */
  #pragma data_alignment=4
#endif
/* USB APCN device Configuration Descriptor */
static uint8_t USBD_APCN_CfgDesc[USB_APCN_CONFIG_DESC_SIZ] =
{
  0x09, /* bLength: Configuation Descriptor size */
  USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION, /* bDescriptorType: Configuration */
  USB_APCN_CONFIG_DESC_SIZ,
  /* wTotalLength: Bytes returned */
  0x00,
  0x01,         /*bNumInterfaces: 1 interface*/
  0x01,         /*bConfigurationValue: Configuration value*/
  0x02,         /*iConfiguration: Index of string descriptor describing the configuration*/
  0xC0,         /*bmAttributes: bus powered and Supports Remote Wakeup */
  0x32,         /*MaxPower 100 mA: this current is used for detecting Vbus*/
  /* 09 */

  /*Interface Descriptor */
    0x09,   /* bLength: Interface Descriptor size */
    USB_DESC_TYPE_INTERFACE,  /* bDescriptorType: Interface */
    /* Interface descriptor type */
    0x00,   /* bInterfaceNumber: Number of Interface */
    0x00,   /* bAlternateSetting: Alternate setting */
    0x05,   /* bNumEndpoints: One endpoints used */
    0x02,   /* bInterfaceClass: Communication Interface Class */
    0x02,   /* bInterfaceSubClass: Abstract Control Model */
    0x01,   /* bInterfaceProtocol: Common AT commands */
    0x00,   /* iInterface: */
  /**********  Descriptor of APCN interface 0 Alternate setting 0 **************/
	/*Endpoint 2 Descriptor*/
	0x07, /* bLength: Endpoint Descriptor size */
	USB_DESC_TYPE_ENDPOINT, /* bDescriptorType: Endpoint */
	APCN_CMD_EP, /* bEndpointAddress */
	0x03, /* bmAttributes: Interrupt */
	LOBYTE(APCN_CMD_PACKET_SIZE), /* wMaxPacketSize: */
	HIBYTE(APCN_CMD_PACKET_SIZE),
	0x10, /* bInterval: */

	/*Endpoint OUT Descriptor*/
	0x07, /* bLength: Endpoint Descriptor size */
	USB_DESC_TYPE_ENDPOINT, /* bDescriptorType: Endpoint */
	APCN_OUT_EP1, /* bEndpointAddress */
	0x02, /* bmAttributes: Bulk */
	LOBYTE(APCN_DATA_FS_MAX_PACKET_SIZE), /* wMaxPacketSize: */
	HIBYTE(APCN_DATA_FS_MAX_PACKET_SIZE),
	0x00, /* bInterval: ignore for Bulk transfer */

	/*Endpoint IN Descriptor*/
	0x07, /* bLength: Endpoint Descriptor size */
	USB_DESC_TYPE_ENDPOINT, /* bDescriptorType: Endpoint */
	APCN_IN_EP1, /* bEndpointAddress */
	0x02, /* bmAttributes: Bulk */
	LOBYTE(APCN_DATA_FS_MAX_PACKET_SIZE), /* wMaxPacketSize: */
	HIBYTE(APCN_DATA_FS_MAX_PACKET_SIZE),
	0x00, /* bInterval: ignore for Bulk transfer */

	/*Endpoint OUT Descriptor*/
	0x07, /* bLength: Endpoint Descriptor size */
	USB_DESC_TYPE_ENDPOINT, /* bDescriptorType: Endpoint */
	APCN_OUT_EP2, /* bEndpointAddress */
	0x02, /* bmAttributes: Bulk */
	LOBYTE(APCN_DATA_FS_MAX_PACKET_SIZE), /* wMaxPacketSize: */
	HIBYTE(APCN_DATA_FS_MAX_PACKET_SIZE),
	0x00, /* bInterval: ignore for Bulk transfer */

	/*Endpoint IN Descriptor*/
	0x07, /* bLength: Endpoint Descriptor size */
	USB_DESC_TYPE_ENDPOINT, /* bDescriptorType: Endpoint */
	APCN_IN_EP2, /* bEndpointAddress */
	0x02, /* bmAttributes: Bulk */
	LOBYTE(APCN_DATA_FS_MAX_PACKET_SIZE), /* wMaxPacketSize: */
	HIBYTE(APCN_DATA_FS_MAX_PACKET_SIZE),
	0x00 /* bInterval: ignore for Bulk transfer */

};

#if defined ( __ICCARM__ ) /*!< IAR Compiler */
  #pragma data_alignment=4
#endif
/* USB Standard Device Descriptor */
static uint8_t USBD_APCN_DeviceQualifierDesc[USB_LEN_DEV_QUALIFIER_DESC] =
{
  USB_LEN_DEV_QUALIFIER_DESC,
  USB_DESC_TYPE_DEVICE_QUALIFIER,
  0x00,
  0x02,
  0x00,
  0x00,
  0x00,
  0x40,
  0x01,
  0x00,
};

/**
  * @}
  */

/** @defgroup USBD_APCN_Private_Functions
  * @{
  */

/**
  * @brief  USBD_APCN_Init
  *         Initialize the APCN interface
  * @param  pdev: device instance
  * @param  cfgidx: Configuration index
  * @retval status
  */
static uint8_t  USBD_APCN_Init (USBD_HandleTypeDef *pdev,
                               uint8_t cfgidx)
{
  uint8_t ret = 0;


  return ret;
}

/**
  * @brief  USBD_APCN_Init
  *         DeInitialize the APCN layer
  * @param  pdev: device instance
  * @param  cfgidx: Configuration index
  * @retval status
  */
static uint8_t  USBD_APCN_DeInit (USBD_HandleTypeDef *pdev,
                                 uint8_t cfgidx)
{
	uint8_t ret = 0;
	USBD_APCN_HandleTypeDef   *hcdc;
	/* Open EP IN */
	USBD_LL_OpenEP(pdev,
	APCN_IN_EP1,
	USBD_EP_TYPE_BULK,
	APCN_DATA_FS_IN_PACKET_SIZE);

	/* Open EP OUT */
	USBD_LL_OpenEP(pdev,
	APCN_OUT_EP1,
	USBD_EP_TYPE_BULK,
	APCN_DATA_FS_OUT_PACKET_SIZE);
	/* Open EP IN */
	USBD_LL_OpenEP(pdev,
	APCN_IN_EP2,
	USBD_EP_TYPE_BULK,
	APCN_DATA_FS_IN_PACKET_SIZE);

	/* Open EP OUT */
	USBD_LL_OpenEP(pdev,
	APCN_OUT_EP2,
	USBD_EP_TYPE_BULK,
	APCN_DATA_FS_OUT_PACKET_SIZE);

	USBD_LL_OpenEP(pdev,
	APCN_CMD_EP,
	USBD_EP_TYPE_INTR,
	APCN_CMD_PACKET_SIZE);


  return USBD_OK;
}

/**
  * @brief  USBD_APCN_Setup
  *         Handle the APCN specific requests
  * @param  pdev: instance
  * @param  req: usb requests
  * @retval status
  */
static uint8_t  USBD_APCN_Setup (USBD_HandleTypeDef *pdev,
                                USBD_SetupReqTypedef *req)
{

  switch (req->bmRequest & USB_REQ_TYPE_MASK)
  {
  case USB_REQ_TYPE_CLASS :
    switch (req->bRequest)
    {

    default:
      USBD_CtlError (pdev, req);
      return USBD_FAIL;
    }
    break;

  case USB_REQ_TYPE_STANDARD:
    switch (req->bRequest)
    {

    default:
      USBD_CtlError (pdev, req);
      return USBD_FAIL;
    }
  }
  return USBD_OK;
}


/**
  * @brief  USBD_APCN_GetCfgDesc
  *         return configuration descriptor
  * @param  length : pointer data length
  * @retval pointer to descriptor buffer
  */
static uint8_t  *USBD_APCN_GetCfgDesc (uint16_t *length)
{
  *length = sizeof (USBD_APCN_CfgDesc);
  return USBD_APCN_CfgDesc;
}

/**
* @brief  DeviceQualifierDescriptor
*         return Device Qualifier descriptor
* @param  length : pointer data length
* @retval pointer to descriptor buffer
*/
uint8_t  *USBD_APCN_DeviceQualifierDescriptor (uint16_t *length)
{
  *length = sizeof (USBD_APCN_DeviceQualifierDesc);
  return USBD_APCN_DeviceQualifierDesc;
}


/**
  * @brief  USBD_APCN_DataIn
  *         handle data IN Stage
  * @param  pdev: device instance
  * @param  epnum: endpoint index
  * @retval status
  */
static uint8_t  USBD_APCN_DataIn (USBD_HandleTypeDef *pdev,
                              uint8_t epnum)
{

  return USBD_OK;
}

/**
  * @brief  USBD_APCN_EP0_RxReady
  *         handle EP0 Rx Ready event
  * @param  pdev: device instance
  * @retval status
  */
static uint8_t  USBD_APCN_EP0_RxReady (USBD_HandleTypeDef *pdev)
{

  return USBD_OK;
}
/**
  * @brief  USBD_APCN_EP0_TxReady
  *         handle EP0 TRx Ready event
  * @param  pdev: device instance
  * @retval status
  */
static uint8_t  USBD_APCN_EP0_TxReady (USBD_HandleTypeDef *pdev)
{

  return USBD_OK;
}
/**
  * @brief  USBD_APCN_SOF
  *         handle SOF event
  * @param  pdev: device instance
  * @retval status
  */
static uint8_t  USBD_APCN_SOF (USBD_HandleTypeDef *pdev)
{

  return USBD_OK;
}
/**
  * @brief  USBD_APCN_IsoINIncomplete
  *         handle data ISO IN Incomplete event
  * @param  pdev: device instance
  * @param  epnum: endpoint index
  * @retval status
  */
static uint8_t  USBD_APCN_IsoINIncomplete (USBD_HandleTypeDef *pdev, uint8_t epnum)
{

  return USBD_OK;
}
/**
  * @brief  USBD_APCN_IsoOutIncomplete
  *         handle data ISO OUT Incomplete event
  * @param  pdev: device instance
  * @param  epnum: endpoint index
  * @retval status
  */
static uint8_t  USBD_APCN_IsoOutIncomplete (USBD_HandleTypeDef *pdev, uint8_t epnum)
{

  return USBD_OK;
}
/**
  * @brief  USBD_APCN_DataOut
  *         handle data OUT Stage
  * @param  pdev: device instance
  * @param  epnum: endpoint index
  * @retval status
  */
static uint8_t  USBD_APCN_DataOut (USBD_HandleTypeDef *pdev,
                              uint8_t epnum)
{

  return USBD_OK;
}

/**
* @brief  DeviceQualifierDescriptor
*         return Device Qualifier descriptor
* @param  length : pointer data length
* @retval pointer to descriptor buffer
*/
uint8_t  *USBD_APCN_GetDeviceQualifierDesc (uint16_t *length)
{
  *length = sizeof (USBD_APCN_DeviceQualifierDesc);
  return USBD_APCN_DeviceQualifierDesc;
}

/**
  * @}
  */


/**
  * @}
  */


/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
