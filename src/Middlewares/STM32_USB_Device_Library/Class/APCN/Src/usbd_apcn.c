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

static uint8_t  *USBD_APCN_GetCfgHSDesc (uint16_t *length);

static uint8_t  *USBD_APCN_GetCfgFSDesc (uint16_t *length);

static uint8_t  *USBD_APCN_GetCfgOtherDesc (uint16_t *length);

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
  NULL,
  USBD_APCN_EP0_RxReady,
  USBD_APCN_DataIn,
  USBD_APCN_DataOut,
  NULL,
  NULL,
  NULL,
  USBD_APCN_GetCfgHSDesc,
  USBD_APCN_GetCfgFSDesc,
  USBD_APCN_GetCfgOtherDesc,
  USBD_APCN_GetDeviceQualifierDesc,
};

#if defined ( __ICCARM__ ) /*!< IAR Compiler */
  #pragma data_alignment=4
#endif
/* USB APCN device Configuration Descriptor */
static uint8_t USBD_APCN_CfgHSDesc[USB_APCN_CONFIG_DESC_SIZ] =
{
  0x09, /* bLength: Configuation Descriptor size */
  USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION, /* bDescriptorType: Configuration */
  USB_APCN_CONFIG_DESC_SIZ,
  /* wTotalLength: Bytes returned */
  0x00,
  0x02,         /*bNumInterfaces: 1 interface*/
  0x01,         /*bConfigurationValue: Configuration value*/
  0x00,         /*iConfiguration: Index of string descriptor describing the configuration*/
  0xC0,         /*bmAttributes: bus powered and Supports Remote Wakeup */
  0x32,         /*MaxPower 100 mA: this current is used for detecting Vbus*/

  /*Interface Descriptor */
    0x09,   /* bLength: Interface Descriptor size */
    USB_DESC_TYPE_INTERFACE,  /* bDescriptorType: Interface */
    /* Interface descriptor type */
    0x00,   /* bInterfaceNumber: Number of Interface */
    0x00,   /* bAlternateSetting: Alternate setting */
    0x01,   /* bNumEndpoints: One endpoints used */
    0x02,   /* bInterfaceClass: Communication Interface Class */
    0x02,   /* bInterfaceSubClass: Abstract Control Model */
    0x01,   /* bInterfaceProtocol: Common AT commands */
    0x00,   /* iInterface: */

	/*Header Functional Descriptor*/
	  0x05,   /* bLength: Endpoint Descriptor size */
	  0x24,   /* bDescriptorType: CS_INTERFACE */
	  0x00,   /* bDescriptorSubtype: Header Func Desc */
	  0x10,   /* bcdCDC: spec release number */
	  0x01,

	  /*Call Management Functional Descriptor*/
	  0x05,   /* bFunctionLength */
	  0x24,   /* bDescriptorType: CS_INTERFACE */
	  0x01,   /* bDescriptorSubtype: Call Management Func Desc */
	  0x00,   /* bmCapabilities: D0+D1 */
	  0x01,   /* bDataInterface: 1 */

	  /*ACM Functional Descriptor*/
	  0x04,   /* bFunctionLength */
	  0x24,   /* bDescriptorType: CS_INTERFACE */
	  0x02,   /* bDescriptorSubtype: Abstract Control Management desc */
	  0x02,   /* bmCapabilities */

	  /*Union Functional Descriptor*/
	  0x05,   /* bFunctionLength */
	  0x24,   /* bDescriptorType: CS_INTERFACE */
	  0x06,   /* bDescriptorSubtype: Union func desc */
	  0x00,   /* bMasterInterface: Communication class interface */
	  0x01,   /* bSlaveInterface0: Data Class Interface */

	  /*Endpoint 2 Descriptor*/
	  0x07,                           /* bLength: Endpoint Descriptor size */
	  USB_DESC_TYPE_ENDPOINT,   /* bDescriptorType: Endpoint */
	  APCN_CMD_EP,                     /* bEndpointAddress */
	  0x03,                           /* bmAttributes: Interrupt */
	  LOBYTE(APCN_CMD_PACKET_SIZE),     /* wMaxPacketSize: */
	  HIBYTE(APCN_CMD_PACKET_SIZE),
	  0x10,                           /* bInterval: */
	  /*---------------------------------------------------------------------------*/

  /*Interface Descriptor */
    0x09,   /* bLength: Interface Descriptor size */
    USB_DESC_TYPE_INTERFACE,  /* bDescriptorType: Interface */
    /* Interface descriptor type */
    0x01,   /* bInterfaceNumber: Number of Interface */
    0x00,   /* bAlternateSetting: Alternate setting */
    0x04,   /* bNumEndpoints: One endpoints used */
    0x0A,   /* bInterfaceClass: Communication Interface Class */
    0x00,   /* bInterfaceSubClass: Abstract Control Model */
    0x00,   /* bInterfaceProtocol: Common AT commands */
    0x00,   /* iInterface: */
  /**********  Descriptor of APCN interface 0 Alternate setting 0 **************/

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
	LOBYTE(APCN_DATA_HS_MAX_PACKET_SIZE), /* wMaxPacketSize: */
	HIBYTE(APCN_DATA_HS_MAX_PACKET_SIZE),
	0x00 /* bInterval: ignore for Bulk transfer */

};

/* USB APCN device Configuration Descriptor */
static uint8_t USBD_APCN_CfgFSDesc[USB_APCN_CONFIG_DESC_SIZ] =
{
  0x09, /* bLength: Configuation Descriptor size */
  USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION, /* bDescriptorType: Configuration */
  USB_APCN_CONFIG_DESC_SIZ,
  /* wTotalLength: Bytes returned */
  0x00,
  0x02,         /*bNumInterfaces: 1 interface*/
  0x01,         /*bConfigurationValue: Configuration value*/
  0x00,         /*iConfiguration: Index of string descriptor describing the configuration*/
  0xC0,         /*bmAttributes: bus powered and Supports Remote Wakeup */
  0x32,         /*MaxPower 100 mA: this current is used for detecting Vbus*/

  /*Interface Descriptor */
    0x09,   /* bLength: Interface Descriptor size */
    USB_DESC_TYPE_INTERFACE,  /* bDescriptorType: Interface */
    /* Interface descriptor type */
    0x00,   /* bInterfaceNumber: Number of Interface */
    0x00,   /* bAlternateSetting: Alternate setting */
    0x01,   /* bNumEndpoints: One endpoints used */
    0x02,   /* bInterfaceClass: Communication Interface Class */
    0x02,   /* bInterfaceSubClass: Abstract Control Model */
    0x01,   /* bInterfaceProtocol: Common AT commands */
    0x00,   /* iInterface: */

	/*Header Functional Descriptor*/
	  0x05,   /* bLength: Endpoint Descriptor size */
	  0x24,   /* bDescriptorType: CS_INTERFACE */
	  0x00,   /* bDescriptorSubtype: Header Func Desc */
	  0x10,   /* bcdCDC: spec release number */
	  0x01,

	  /*Call Management Functional Descriptor*/
	  0x05,   /* bFunctionLength */
	  0x24,   /* bDescriptorType: CS_INTERFACE */
	  0x01,   /* bDescriptorSubtype: Call Management Func Desc */
	  0x00,   /* bmCapabilities: D0+D1 */
	  0x01,   /* bDataInterface: 1 */

	  /*ACM Functional Descriptor*/
	  0x04,   /* bFunctionLength */
	  0x24,   /* bDescriptorType: CS_INTERFACE */
	  0x02,   /* bDescriptorSubtype: Abstract Control Management desc */
	  0x02,   /* bmCapabilities */

	  /*Union Functional Descriptor*/
	  0x05,   /* bFunctionLength */
	  0x24,   /* bDescriptorType: CS_INTERFACE */
	  0x06,   /* bDescriptorSubtype: Union func desc */
	  0x00,   /* bMasterInterface: Communication class interface */
	  0x01,   /* bSlaveInterface0: Data Class Interface */

	  /*Endpoint 2 Descriptor*/
	  0x07,                           /* bLength: Endpoint Descriptor size */
	  USB_DESC_TYPE_ENDPOINT,   /* bDescriptorType: Endpoint */
	  APCN_CMD_EP,                     /* bEndpointAddress */
	  0x03,                           /* bmAttributes: Interrupt */
	  LOBYTE(APCN_CMD_PACKET_SIZE),     /* wMaxPacketSize: */
	  HIBYTE(APCN_CMD_PACKET_SIZE),
	  0x10,                           /* bInterval: */
	  /*---------------------------------------------------------------------------*/

  /*Interface Descriptor */
    0x09,   /* bLength: Interface Descriptor size */
    USB_DESC_TYPE_INTERFACE,  /* bDescriptorType: Interface */
    /* Interface descriptor type */
    0x01,   /* bInterfaceNumber: Number of Interface */
    0x00,   /* bAlternateSetting: Alternate setting */
    0x04,   /* bNumEndpoints: One endpoints used */
    0x0A,   /* bInterfaceClass: Communication Interface Class */
    0x00,   /* bInterfaceSubClass: Abstract Control Model */
    0x00,   /* bInterfaceProtocol: Common AT commands */
    0x00,   /* iInterface: */
  /**********  Descriptor of APCN interface 0 Alternate setting 0 **************/

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
/* USB APCN device Configuration Descriptor */
static uint8_t USBD_APCN_CfgOtherDesc[USB_APCN_CONFIG_DESC_SIZ] =
{
  0x09, /* bLength: Configuation Descriptor size */
  USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION, /* bDescriptorType: Configuration */
  USB_APCN_CONFIG_DESC_SIZ,
  /* wTotalLength: Bytes returned */
  0x00,
  0x02,         /*bNumInterfaces: 1 interface*/
  0x01,         /*bConfigurationValue: Configuration value*/
  0x00,         /*iConfiguration: Index of string descriptor describing the configuration*/
  0xC0,         /*bmAttributes: bus powered and Supports Remote Wakeup */
  0x32,         /*MaxPower 100 mA: this current is used for detecting Vbus*/

  /*Interface Descriptor */
    0x09,   /* bLength: Interface Descriptor size */
    USB_DESC_TYPE_INTERFACE,  /* bDescriptorType: Interface */
    /* Interface descriptor type */
    0x00,   /* bInterfaceNumber: Number of Interface */
    0x00,   /* bAlternateSetting: Alternate setting */
    0x01,   /* bNumEndpoints: One endpoints used */
    0x02,   /* bInterfaceClass: Communication Interface Class */
    0x02,   /* bInterfaceSubClass: Abstract Control Model */
    0x01,   /* bInterfaceProtocol: Common AT commands */
    0x00,   /* iInterface: */

	/*Header Functional Descriptor*/
	  0x05,   /* bLength: Endpoint Descriptor size */
	  0x24,   /* bDescriptorType: CS_INTERFACE */
	  0x00,   /* bDescriptorSubtype: Header Func Desc */
	  0x10,   /* bcdCDC: spec release number */
	  0x01,

	  /*Call Management Functional Descriptor*/
	  0x05,   /* bFunctionLength */
	  0x24,   /* bDescriptorType: CS_INTERFACE */
	  0x01,   /* bDescriptorSubtype: Call Management Func Desc */
	  0x00,   /* bmCapabilities: D0+D1 */
	  0x01,   /* bDataInterface: 1 */

	  /*ACM Functional Descriptor*/
	  0x04,   /* bFunctionLength */
	  0x24,   /* bDescriptorType: CS_INTERFACE */
	  0x02,   /* bDescriptorSubtype: Abstract Control Management desc */
	  0x02,   /* bmCapabilities */

	  /*Union Functional Descriptor*/
	  0x05,   /* bFunctionLength */
	  0x24,   /* bDescriptorType: CS_INTERFACE */
	  0x06,   /* bDescriptorSubtype: Union func desc */
	  0x00,   /* bMasterInterface: Communication class interface */
	  0x01,   /* bSlaveInterface0: Data Class Interface */

	  /*Endpoint 2 Descriptor*/
	  0x07,                           /* bLength: Endpoint Descriptor size */
	  USB_DESC_TYPE_ENDPOINT,   /* bDescriptorType: Endpoint */
	  APCN_CMD_EP,                     /* bEndpointAddress */
	  0x03,                           /* bmAttributes: Interrupt */
	  LOBYTE(APCN_CMD_PACKET_SIZE),     /* wMaxPacketSize: */
	  HIBYTE(APCN_CMD_PACKET_SIZE),
	  0xFF,                           /* bInterval: */
	  /*---------------------------------------------------------------------------*/

  /*Interface Descriptor */
    0x09,   /* bLength: Interface Descriptor size */
    USB_DESC_TYPE_INTERFACE,  /* bDescriptorType: Interface */
    /* Interface descriptor type */
    0x01,   /* bInterfaceNumber: Number of Interface */
    0x00,   /* bAlternateSetting: Alternate setting */
    0x04,   /* bNumEndpoints: One endpoints used */
    0x0A,   /* bInterfaceClass: Communication Interface Class */
    0x00,   /* bInterfaceSubClass: Abstract Control Model */
    0x00,   /* bInterfaceProtocol: Common AT commands */
    0x00,   /* iInterface: */
  /**********  Descriptor of APCN interface 0 Alternate setting 0 **************/

	/*Endpoint OUT Descriptor*/
	0x07, /* bLength: Endpoint Descriptor size */
	USB_DESC_TYPE_ENDPOINT, /* bDescriptorType: Endpoint */
	APCN_OUT_EP1, /* bEndpointAddress */
	0x02, /* bmAttributes: Bulk */
	0x40, /* wMaxPacketSize: */
	0x00,
	0x00, /* bInterval: ignore for Bulk transfer */

	/*Endpoint IN Descriptor*/
	0x07, /* bLength: Endpoint Descriptor size */
	USB_DESC_TYPE_ENDPOINT, /* bDescriptorType: Endpoint */
	APCN_IN_EP1, /* bEndpointAddress */
	0x02, /* bmAttributes: Bulk */
	0x40, /* wMaxPacketSize: */
	0x00,
	0x00, /* bInterval: ignore for Bulk transfer */

	/*Endpoint OUT Descriptor*/
	0x07, /* bLength: Endpoint Descriptor size */
	USB_DESC_TYPE_ENDPOINT, /* bDescriptorType: Endpoint */
	APCN_OUT_EP2, /* bEndpointAddress */
	0x02, /* bmAttributes: Bulk */
	0x40, /* wMaxPacketSize: */
	0x00,
	0x00, /* bInterval: ignore for Bulk transfer */

	/*Endpoint IN Descriptor*/
	0x07, /* bLength: Endpoint Descriptor size */
	USB_DESC_TYPE_ENDPOINT, /* bDescriptorType: Endpoint */
	APCN_IN_EP2, /* bEndpointAddress */
	0x02, /* bmAttributes: Bulk */
	0x40, /* wMaxPacketSize: */
	0x00,
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
	USBD_APCN_HandleTypeDef *hcdc;
	if(pdev->dev_speed == USBD_SPEED_HIGH  )
	{
		/* Open EP IN */
		USBD_LL_OpenEP(pdev,
		APCN_IN_EP1,
		USBD_EP_TYPE_BULK,
		APCN_DATA_HS_IN_PACKET_SIZE);

		/* Open EP OUT */
		USBD_LL_OpenEP(pdev,
		APCN_OUT_EP1,
		USBD_EP_TYPE_BULK,
		APCN_DATA_HS_OUT_PACKET_SIZE);
		/* Open EP IN */
		USBD_LL_OpenEP(pdev,
		APCN_IN_EP2,
		USBD_EP_TYPE_BULK,
		APCN_DATA_HS_IN_PACKET_SIZE);

		/* Open EP OUT */
		USBD_LL_OpenEP(pdev,
		APCN_OUT_EP2,
		USBD_EP_TYPE_BULK,
		APCN_DATA_HS_OUT_PACKET_SIZE);
	}else{
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
	}


	USBD_LL_OpenEP(pdev,
	APCN_CMD_EP,
	USBD_EP_TYPE_INTR,
	APCN_CMD_PACKET_SIZE);

	pdev->pClassData = USBD_malloc(sizeof (USBD_APCN_HandleTypeDef));

	if (pdev->pClassData == NULL) {
		ret = 1;
	} else {
		hcdc = (USBD_APCN_HandleTypeDef*) pdev->pClassData;

		/* Init  physical Interface components */
		((USBD_APCN_ItfTypeDef *) pdev->pUserData)->Init();

		/* Init Xfer states */
		hcdc->Tx1State = 0;
		hcdc->Rx1State = 0;
		hcdc->Tx2State = 0;
		hcdc->Rx2State = 0;
		if (pdev->dev_speed == USBD_SPEED_HIGH) {
			/* Prepare Out endpoint to receive next packet */
			USBD_LL_PrepareReceive(pdev,
			APCN_OUT_EP1, hcdc->EP1RxBuffer,
			APCN_DATA_HS_OUT_PACKET_SIZE);
			USBD_LL_PrepareReceive(pdev,
			APCN_OUT_EP2, hcdc->EP2RxBuffer,
			APCN_DATA_HS_OUT_PACKET_SIZE);
		} else {
			/* Prepare Out endpoint to receive next packet */
			USBD_LL_PrepareReceive(pdev,
			APCN_OUT_EP1, hcdc->EP1RxBuffer,
			APCN_DATA_FS_OUT_PACKET_SIZE);
			USBD_LL_PrepareReceive(pdev,
			APCN_OUT_EP2, hcdc->EP2RxBuffer,
			APCN_DATA_FS_OUT_PACKET_SIZE);
		}
	}


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

	/* Open EP IN */
	USBD_LL_CloseEP(pdev,
	APCN_IN_EP1);

	/* Open EP OUT */
	USBD_LL_CloseEP(pdev,
	APCN_OUT_EP1);

	USBD_LL_CloseEP(pdev,
	APCN_IN_EP2);

	/* Open EP OUT */
	USBD_LL_CloseEP(pdev,
	APCN_OUT_EP2);

	/* Open Command IN EP */
	  USBD_LL_CloseEP(pdev,
	              APCN_CMD_EP);

	/* DeInit  physical Interface components */
	if (pdev->pClassData != NULL) {
		((USBD_APCN_ItfTypeDef *) pdev->pUserData)->DeInit();
		USBD_free(pdev->pClassData);
		pdev->pClassData = NULL;
	}



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
	USBD_APCN_HandleTypeDef   *hcdc = (USBD_APCN_HandleTypeDef*) pdev->pClassData;
	  static uint8_t ifalt = 0;

	  switch (req->bmRequest & USB_REQ_TYPE_MASK)
	  {
	  case USB_REQ_TYPE_CLASS :
	    if (req->wLength)
	    {
	      if (req->bmRequest & 0x80)
	      {
	        ((USBD_APCN_ItfTypeDef *)pdev->pUserData)->Control(req->bRequest,
	                                                          (uint8_t *)hcdc->data,
	                                                          req->wLength);
	          USBD_CtlSendData (pdev,
	                            (uint8_t *)hcdc->data,
	                            req->wLength);
	      }
	      else
	      {
	        hcdc->CmdOpCode = req->bRequest;
	        hcdc->CmdLength = req->wLength;

	        USBD_CtlPrepareRx (pdev,
	                           (uint8_t *)hcdc->data,
	                           req->wLength);
	      }

	    }
	    else
	    {
	      ((USBD_APCN_ItfTypeDef *)pdev->pUserData)->Control(req->bRequest,
	                                                        (uint8_t*)req,
	                                                        0);
	    }
	    break;

	  case USB_REQ_TYPE_STANDARD:
	    switch (req->bRequest)
	    {
	    case USB_REQ_GET_INTERFACE :
	      USBD_CtlSendData (pdev,
	                        &ifalt,
	                        1);
	      break;

	    case USB_REQ_SET_INTERFACE :
	      break;
	    }

	  default:
	    break;
	  }
  return USBD_OK;
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
	USBD_APCN_HandleTypeDef   *hcdc = (USBD_APCN_HandleTypeDef*) pdev->pClassData;

	  if(pdev->pClassData != NULL)
	  {
		  if(epnum == APCN_OUT_EP1){
			  hcdc->Tx1State = 0;
		  }
		  if(epnum == APCN_OUT_EP2){
		  	  hcdc->Tx2State = 0;
		  }

	    return USBD_OK;
	  }
	  else
	  {
	    return USBD_FAIL;
	  }
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
	USBD_ErrLog("USBD_APCN_DataOut");
	USBD_APCN_HandleTypeDef *hcdc = (USBD_APCN_HandleTypeDef*) pdev->pClassData;

	/* Get the received data length */
	if(epnum == APCN_OUT_EP1){
		hcdc->EP1RxLength = USBD_LL_GetRxDataSize(pdev, epnum);
	}else if(epnum == APCN_OUT_EP2){
		hcdc->EP2RxLength = USBD_LL_GetRxDataSize(pdev, epnum);
	}


	/* USB data will be immediately processed, this allow next USB traffic being
	 NAKed till the end of the application Xfer */
	if (pdev->pClassData != NULL) {
		if(epnum == APCN_OUT_EP1){
		((USBD_APCN_ItfTypeDef *) pdev->pUserData)->Receive(epnum,hcdc->EP1RxBuffer,
				&hcdc->EP1RxLength);
		}else if(epnum == APCN_OUT_EP2){
			((USBD_APCN_ItfTypeDef *) pdev->pUserData)->Receive(epnum,hcdc->EP2RxBuffer,
							&hcdc->EP2RxLength);
		}

		return USBD_OK;
	} else {
		return USBD_FAIL;
	}
}


/**
  * @brief  USBD_APCN_EP0_RxReady
  *         handle EP0 Rx Ready event
  * @param  pdev: device instance
  * @retval status
  */
static uint8_t  USBD_APCN_EP0_RxReady (USBD_HandleTypeDef *pdev)
{
 USBD_APCN_HandleTypeDef   *hcdc = (USBD_APCN_HandleTypeDef*) pdev->pClassData;
  
  if((pdev->pUserData != NULL) && (hcdc->CmdOpCode != 0xFF))
  {
    ((USBD_APCN_ItfTypeDef *)pdev->pUserData)->Control(hcdc->CmdOpCode,
                                                      (uint8_t *)hcdc->data,
                                                      hcdc->CmdLength);
      hcdc->CmdOpCode = 0xFF; 
      
  }

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
  * @brief  USBD_APCN_GetCfgDesc
  *         return configuration descriptor
  * @param  length : pointer data length
  * @retval pointer to descriptor buffer
  */
static uint8_t  *USBD_APCN_GetCfgHSDesc (uint16_t *length)
{
  *length = sizeof (USBD_APCN_CfgHSDesc);
  return USBD_APCN_CfgHSDesc;
}

/**
  * @brief  USBD_APCN_GetCfgDesc
  *         return configuration descriptor
  * @param  length : pointer data length
  * @retval pointer to descriptor buffer
  */
static uint8_t  *USBD_APCN_GetCfgFSDesc (uint16_t *length)
{
  *length = sizeof (USBD_APCN_CfgFSDesc);
  return USBD_APCN_CfgFSDesc;
}

/**
  * @brief  USBD_APCN_GetCfgDesc
  *         return configuration descriptor
  * @param  length : pointer data length
  * @retval pointer to descriptor buffer
  */
static uint8_t  *USBD_APCN_GetCfgOtherDesc (uint16_t *length)
{
  *length = sizeof (USBD_APCN_CfgOtherDesc);
  return USBD_APCN_CfgOtherDesc;
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
* @brief  USBD_APCN_RegisterInterface
  * @param  pdev: device instance
  * @param  fops: CD  Interface callback
  * @retval status
  */
uint8_t  USBD_APCN_RegisterInterface  (USBD_HandleTypeDef   *pdev,
                                      USBD_APCN_ItfTypeDef *fops)
{
  uint8_t  ret = USBD_FAIL;

  if(fops != NULL)
  {
    pdev->pUserData= fops;
    ret = USBD_OK;
  }

  return ret;
}




/**
  * @brief  USBD_APCN_SetTxBuffer
  * @param  pdev: device instance
  * @param  pbuff: Tx Buffer
  * @retval status
  */
uint8_t  USBD_APCN_SetEP1TxBuffer  (USBD_HandleTypeDef   *pdev,
                                uint8_t  *pbuff,
                                uint16_t length)
{
  USBD_APCN_HandleTypeDef   *hcdc = (USBD_APCN_HandleTypeDef*) pdev->pClassData;

  hcdc->EP1TxBuffer = pbuff;
  hcdc->EP1TxLength = length;

  return USBD_OK;
}


/**
  * @brief  USBD_APCN_SetRxBuffer
  * @param  pdev: device instance
  * @param  pbuff: Rx Buffer
  * @retval status
  */
uint8_t  USBD_APCN_SetEP1RxBuffer  (USBD_HandleTypeDef   *pdev,
                                   uint8_t  *pbuff)
{
  USBD_APCN_HandleTypeDef   *hcdc = (USBD_APCN_HandleTypeDef*) pdev->pClassData;

  hcdc->EP1RxBuffer = pbuff;

  return USBD_OK;
}

/**
  * @brief  USBD_APCN_SetTxBuffer
  * @param  pdev: device instance
  * @param  pbuff: Tx Buffer
  * @retval status
  */
uint8_t  USBD_APCN_SetEP2TxBuffer  (USBD_HandleTypeDef   *pdev,
                                uint8_t  *pbuff,
                                uint16_t length)
{
  USBD_APCN_HandleTypeDef   *hcdc = (USBD_APCN_HandleTypeDef*) pdev->pClassData;

  hcdc->EP2TxBuffer = pbuff;
  hcdc->EP2TxLength = length;

  return USBD_OK;
}


/**
  * @brief  USBD_APCN_SetRxBuffer
  * @param  pdev: device instance
  * @param  pbuff: Rx Buffer
  * @retval status
  */
uint8_t  USBD_APCN_SetEP2RxBuffer  (USBD_HandleTypeDef   *pdev,
                                   uint8_t  *pbuff)
{
  USBD_APCN_HandleTypeDef   *hcdc = (USBD_APCN_HandleTypeDef*) pdev->pClassData;

  hcdc->EP2RxBuffer = pbuff;

  return USBD_OK;
}

/**
  * @brief  USBD_APCN_DataOut
  *         Data received on non-control Out endpoint
  * @param  pdev: device instance
  * @param  epnum: endpoint number
  * @retval status
  */
uint8_t  USBD_APCN_TransmitPacket(USBD_HandleTypeDef *pdev,uint8_t ep)
{
  USBD_APCN_HandleTypeDef   *hcdc = (USBD_APCN_HandleTypeDef*) pdev->pClassData;

  if(pdev->pClassData != NULL)
  {
	  if(ep == APCN_OUT_EP1){
		  if(hcdc->Tx1State == 0)
		      {
		        /* Tx Transfer in progress */
		        hcdc->Tx1State = 1;

		        /* Transmit next packet */


		        USBD_LL_Transmit(pdev,
		                         APCN_IN_EP1,
		                         hcdc->EP1TxBuffer,
		                         hcdc->EP1TxLength);

		        return USBD_OK;
		      }
		      else
		      {
		        return USBD_BUSY;
		      }
	  }
	  if(ep == APCN_OUT_EP2){
		  if(hcdc->Tx2State == 0)
		  {
				/* Tx Transfer in progress */
				hcdc->Tx2State = 1;

				/* Transmit next packet */

				USBD_LL_Transmit(pdev,
				APCN_IN_EP2, hcdc->EP2TxBuffer, hcdc->EP2TxLength);
				return USBD_OK;
			} else {
				return USBD_BUSY;
			}
	  }
	  return USBD_FAIL;

  }
  else
  {
    return USBD_FAIL;
  }
}


/**
  * @brief  USBD_APCN_ReceivePacket
  *         prepare OUT Endpoint for reception
  * @param  pdev: device instance
  * @retval status
  */
uint8_t  USBD_APCN_ReceivePacket(USBD_HandleTypeDef *pdev)
{
  USBD_APCN_HandleTypeDef   *hcdc = (USBD_APCN_HandleTypeDef*) pdev->pClassData;

  /* Suspend or Resume USB Out process */
  if(pdev->pClassData != NULL)
  {
    if(pdev->dev_speed == USBD_SPEED_HIGH  )
    {
      /* Prepare Out endpoint to receive next packet */
      USBD_LL_PrepareReceive(pdev,
                             APCN_OUT_EP1,
                             hcdc->EP1RxBuffer,
                             APCN_DATA_HS_OUT_PACKET_SIZE);
      USBD_LL_PrepareReceive(pdev,
                                   APCN_OUT_EP2,
                                   hcdc->EP2RxBuffer,
                                   APCN_DATA_HS_OUT_PACKET_SIZE);
    }
    else
    {
      /* Prepare Out endpoint to receive next packet */
		USBD_LL_PrepareReceive(pdev,
			APCN_OUT_EP1, hcdc->EP1RxBuffer,
			APCN_DATA_FS_OUT_PACKET_SIZE);
		USBD_LL_PrepareReceive(pdev,
			APCN_OUT_EP2, hcdc->EP2RxBuffer,
			APCN_DATA_FS_OUT_PACKET_SIZE);
    }
    return USBD_OK;
  }
  else
  {
    return USBD_FAIL;
  }
}

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
