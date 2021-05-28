/******************************************************************************
* Copyright (c) 2019 - 2020 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/


#ifndef XPM_DEVICE_IDLE_H_
#define XPM_DEVICE_IDLE_H_

#include "xparameters.h"
#include "xpm_node.h"
#include "xpm_device.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct XPmDevice_SoftResetInfo {
	u32 DeviceId;
	int (*IdleHook)(u16 DeviceId, u32 BaseAddress);	/**< Hook function for idling */
	u16 IdleHookArgs;
} XPmDevice_SoftResetInfo;

/* Define the XILPM device macro based on canonical defination. */
/* XILPM_USB_0 */
#if (defined(XPAR_XUSBPSU_0_DEVICE_ID) && \
	(XPAR_XUSBPSU_0_BASEADDR == 0xFE200000U))
#define XILPM_USB_0 XPAR_XUSBPSU_0_DEVICE_ID
#endif

/* XILPM_QSPI_0 */
#if (defined(XPAR_XQSPIPSU_0_DEVICE_ID) && \
	(XPAR_XQSPIPSU_0_BASEADDR == 0xF1030000U))
#define XILPM_QSPI_0 XPAR_XQSPIPSU_0_DEVICE_ID
#endif

/* XILPM_OSPI_0 */
#if (defined(XPAR_XOSPIPSV_0_DEVICE_ID) && \
	(XPAR_XOSPIPSV_0_BASEADDR == 0xF1010000U))
#define XILPM_OSPI_0 XPAR_XOSPIPSV_0_DEVICE_ID
#endif

/* XILPM_SD_0 and XILPM_SD_1 */
#if (defined(XPAR_XSDPS_0_DEVICE_ID))
	#if (XPAR_XSDPS_0_BASEADDR == 0xF1040000U)
		#define XILPM_SD_0 XPAR_XSDPS_0_DEVICE_ID
	#elif (XPAR_XSDPS_0_BASEADDR == 0xF1050000U)
		#define XILPM_SD_1 XPAR_XSDPS_0_DEVICE_ID
	#endif
#endif
#if (defined(XPAR_XSDPS_1_DEVICE_ID))
	#if (XPAR_XSDPS_1_BASEADDR == 0xF1040000U)
		#define XILPM_SD_0 XPAR_XSDPS_1_DEVICE_ID
	#elif (XPAR_XSDPS_1_BASEADDR == 0xF1050000U)
		#define XILPM_SD_1 XPAR_XSDPS_1_DEVICE_ID
	#endif
#endif

/* XILPM_ETH_0 and XILPM_ETH_1 */
#if (defined(XPAR_XEMACPS_0_DEVICE_ID))
	#if (XPAR_XEMACPS_0_BASEADDR == 0xFF0C0000U)
		#define XILPM_ETH_0 XPAR_XEMACPS_0_DEVICE_ID
	#elif (XPAR_XEMACPS_0_BASEADDR == 0xFF0D0000U)
		#define XILPM_ETH_1 XPAR_XEMACPS_0_DEVICE_ID
	#endif
#endif
#if (defined(XPAR_XEMACPS_1_DEVICE_ID))
	#if (XPAR_XEMACPS_1_BASEADDR == 0xFF0C0000U)
		#define XILPM_ETH_0 XPAR_XEMACPS_1_DEVICE_ID
	#elif (XPAR_XEMACPS_1_BASEADDR == 0xFF0D0000U)
		#define XILPM_ETH_1 XPAR_XEMACPS_1_DEVICE_ID
	#endif
#endif

/* XILPM_ZDMA_0 to XILPM_ZDMA_7 */
#if (defined(XPAR_XZDMA_0_DEVICE_ID))
	#if (XPAR_XZDMA_0_BASEADDR == 0xFFA80000U)
		#define XILPM_ZDMA_0 XPAR_XZDMA_0_DEVICE_ID
	#elif (XPAR_XZDMA_0_BASEADDR == 0xFFA90000UU)
		#define XILPM_ZDMA_1 XPAR_XZDMA_0_DEVICE_ID
	#elif (XPAR_XZDMA_0_BASEADDR == 0xFFAA0000UU)
		#define XILPM_ZDMA_2 XPAR_XZDMA_0_DEVICE_ID
	#elif (XPAR_XZDMA_0_BASEADDR == 0xFFAB0000UU)
		#define XILPM_ZDMA_3 XPAR_XZDMA_0_DEVICE_ID
	#elif (XPAR_XZDMA_0_BASEADDR == 0xFFAC0000UU)
		#define XILPM_ZDMA_4 XPAR_XZDMA_0_DEVICE_ID
	#elif (XPAR_XZDMA_0_BASEADDR == 0xFFAD0000UU)
		#define XILPM_ZDMA_5 XPAR_XZDMA_0_DEVICE_ID
	#elif (XPAR_XZDMA_0_BASEADDR == 0xFFAE0000UU)
		#define XILPM_ZDMA_6 XPAR_XZDMA_0_DEVICE_ID
	#elif (XPAR_XZDMA_0_BASEADDR == 0xFFAF0000UU)
		#define XILPM_ZDMA_7 XPAR_XZDMA_0_DEVICE_ID
	#endif
#endif
#if (defined(XPAR_XZDMA_1_DEVICE_ID))
	#if (XPAR_XZDMA_1_BASEADDR == 0xFFA80000U)
		#define XILPM_ZDMA_0 XPAR_XZDMA_1_DEVICE_ID
	#elif (XPAR_XZDMA_1_BASEADDR == 0xFFA90000U)
		#define XILPM_ZDMA_1 XPAR_XZDMA_1_DEVICE_ID
	#elif (XPAR_XZDMA_1_BASEADDR == 0xFFAA0000U)
		#define XILPM_ZDMA_2 XPAR_XZDMA_1_DEVICE_ID
	#elif (XPAR_XZDMA_1_BASEADDR == 0xFFAB0000U)
		#define XILPM_ZDMA_3 XPAR_XZDMA_1_DEVICE_ID
	#elif (XPAR_XZDMA_1_BASEADDR == 0xFFAC0000U)
		#define XILPM_ZDMA_4 XPAR_XZDMA_1_DEVICE_ID
	#elif (XPAR_XZDMA_1_BASEADDR == 0xFFAD0000U)
		#define XILPM_ZDMA_5 XPAR_XZDMA_1_DEVICE_ID
	#elif (XPAR_XZDMA_1_BASEADDR == 0xFFAE0000U)
		#define XILPM_ZDMA_6 XPAR_XZDMA_1_DEVICE_ID
	#elif (XPAR_XZDMA_1_BASEADDR == 0xFFAF0000U)
		#define XILPM_ZDMA_7 XPAR_XZDMA_1_DEVICE_ID
	#endif
#endif
#if (defined(XPAR_XZDMA_2_DEVICE_ID))
	#if (XPAR_XZDMA_2_BASEADDR == 0xFFA80000U)
		#define XILPM_ZDMA_0 XPAR_XZDMA_2_DEVICE_ID
	#elif (XPAR_XZDMA_2_BASEADDR == 0xFFA90000U)
		#define XILPM_ZDMA_1 XPAR_XZDMA_2_DEVICE_ID
	#elif (XPAR_XZDMA_2_BASEADDR == 0xFFAA0000U)
		#define XILPM_ZDMA_2 XPAR_XZDMA_2_DEVICE_ID
	#elif (XPAR_XZDMA_2_BASEADDR == 0xFFAB0000U)
		#define XILPM_ZDMA_3 XPAR_XZDMA_2_DEVICE_ID
	#elif (XPAR_XZDMA_2_BASEADDR == 0xFFAC0000U)
		#define XILPM_ZDMA_4 XPAR_XZDMA_2_DEVICE_ID
	#elif (XPAR_XZDMA_2_BASEADDR == 0xFFAD0000U)
		#define XILPM_ZDMA_5 XPAR_XZDMA_2_DEVICE_ID
	#elif (XPAR_XZDMA_2_BASEADDR == 0xFFAE0000U)
		#define XILPM_ZDMA_6 XPAR_XZDMA_2_DEVICE_ID
	#elif (XPAR_XZDMA_2_BASEADDR == 0xFFAF0000U)
		#define XILPM_ZDMA_7 XPAR_XZDMA_2_DEVICE_ID
	#endif
#endif
#if (defined(XPAR_XZDMA_3_DEVICE_ID))
	#if (XPAR_XZDMA_3_BASEADDR == 0xFFA80000U)
		#define XILPM_ZDMA_0 XPAR_XZDMA_3_DEVICE_ID
	#elif (XPAR_XZDMA_3_BASEADDR == 0xFFA90000U)
		#define XILPM_ZDMA_1 XPAR_XZDMA_3_DEVICE_ID
	#elif (XPAR_XZDMA_3_BASEADDR == 0xFFAA0000U)
		#define XILPM_ZDMA_2 XPAR_XZDMA_3_DEVICE_ID
	#elif (XPAR_XZDMA_3_BASEADDR == 0xFFAB0000U)
		#define XILPM_ZDMA_3 XPAR_XZDMA_3_DEVICE_ID
	#elif (XPAR_XZDMA_3_BASEADDR == 0xFFAC0000U)
		#define XILPM_ZDMA_4 XPAR_XZDMA_3_DEVICE_ID
	#elif (XPAR_XZDMA_3_BASEADDR == 0xFFAD0000U)
		#define XILPM_ZDMA_5 XPAR_XZDMA_3_DEVICE_ID
	#elif (XPAR_XZDMA_3_BASEADDR == 0xFFAE0000U)
		#define XILPM_ZDMA_6 XPAR_XZDMA_3_DEVICE_ID
	#elif (XPAR_XZDMA_3_BASEADDR == 0xFFAF0000U)
		#define XILPM_ZDMA_7 XPAR_XZDMA_3_DEVICE_ID
	#endif
#endif
#if (defined(XPAR_XZDMA_4_DEVICE_ID))
	#if (XPAR_XZDMA_4_BASEADDR == 0xFFA80000U)
		#define XILPM_ZDMA_0 XPAR_XZDMA_4_DEVICE_ID
	#elif (XPAR_XZDMA_4_BASEADDR == 0xFFA90000U)
		#define XILPM_ZDMA_1 XPAR_XZDMA_4_DEVICE_ID
	#elif (XPAR_XZDMA_4_BASEADDR == 0xFFAA0000U)
		#define XILPM_ZDMA_2 XPAR_XZDMA_4_DEVICE_ID
	#elif (XPAR_XZDMA_4_BASEADDR == 0xFFAB0000U)
		#define XILPM_ZDMA_3 XPAR_XZDMA_4_DEVICE_ID
	#elif (XPAR_XZDMA_4_BASEADDR == 0xFFAC0000U)
		#define XILPM_ZDMA_4 XPAR_XZDMA_4_DEVICE_ID
	#elif (XPAR_XZDMA_4_BASEADDR == 0xFFAD0000U)
		#define XILPM_ZDMA_5 XPAR_XZDMA_4_DEVICE_ID
	#elif (XPAR_XZDMA_4_BASEADDR == 0xFFAE0000U)
		#define XILPM_ZDMA_6 XPAR_XZDMA_4_DEVICE_ID
	#elif (XPAR_XZDMA_4_BASEADDR == 0xFFAF0000U)
		#define XILPM_ZDMA_7 XPAR_XZDMA_4_DEVICE_ID
	#endif
#endif
#if (defined(XPAR_XZDMA_5_DEVICE_ID))
	#if (XPAR_XZDMA_5_BASEADDR == 0xFFA80000U)
		#define XILPM_ZDMA_0 XPAR_XZDMA_5_DEVICE_ID
	#elif (XPAR_XZDMA_5_BASEADDR == 0xFFA90000U)
		#define XILPM_ZDMA_1 XPAR_XZDMA_5_DEVICE_ID
	#elif (XPAR_XZDMA_5_BASEADDR == 0xFFAA0000U)
		#define XILPM_ZDMA_2 XPAR_XZDMA_5_DEVICE_ID
	#elif (XPAR_XZDMA_5_BASEADDR == 0xFFAB0000U)
		#define XILPM_ZDMA_3 XPAR_XZDMA_5_DEVICE_ID
	#elif (XPAR_XZDMA_5_BASEADDR == 0xFFAC0000U)
		#define XILPM_ZDMA_4 XPAR_XZDMA_5_DEVICE_ID
	#elif (XPAR_XZDMA_5_BASEADDR == 0xFFAD0000U)
		#define XILPM_ZDMA_5 XPAR_XZDMA_5_DEVICE_ID
	#elif (XPAR_XZDMA_5_BASEADDR == 0xFFAE0000U)
		#define XILPM_ZDMA_6 XPAR_XZDMA_5_DEVICE_ID
	#elif (XPAR_XZDMA_5_BASEADDR == 0xFFAF0000U)
		#define XILPM_ZDMA_7 XPAR_XZDMA_5_DEVICE_ID
	#endif
#endif
#if (defined(XPAR_XZDMA_6_DEVICE_ID))
	#if (XPAR_XZDMA_6_BASEADDR == 0xFFA80000U)
		#define XILPM_ZDMA_0 XPAR_XZDMA_6_DEVICE_ID
	#elif (XPAR_XZDMA_6_BASEADDR == 0xFFA90000U)
		#define XILPM_ZDMA_1 XPAR_XZDMA_6_DEVICE_ID
	#elif (XPAR_XZDMA_6_BASEADDR == 0xFFAA0000U)
		#define XILPM_ZDMA_2 XPAR_XZDMA_6_DEVICE_ID
	#elif (XPAR_XZDMA_6_BASEADDR == 0xFFAB0000U)
		#define XILPM_ZDMA_3 XPAR_XZDMA_6_DEVICE_ID
	#elif (XPAR_XZDMA_6_BASEADDR == 0xFFAC0000U)
		#define XILPM_ZDMA_4 XPAR_XZDMA_6_DEVICE_ID
	#elif (XPAR_XZDMA_6_BASEADDR == 0xFFAD0000U)
		#define XILPM_ZDMA_5 XPAR_XZDMA_6_DEVICE_ID
	#elif (XPAR_XZDMA_6_BASEADDR == 0xFFAE0000U)
		#define XILPM_ZDMA_6 XPAR_XZDMA_6_DEVICE_ID
	#elif (XPAR_XZDMA_6_BASEADDR == 0xFFAF0000U)
		#define XILPM_ZDMA_7 XPAR_XZDMA_6_DEVICE_ID
	#endif
#endif
#if (defined(XPAR_XZDMA_7_DEVICE_ID))
	#if (XPAR_XZDMA_7_BASEADDR == 0xFFA80000U)
		#define XILPM_ZDMA_0 XPAR_XZDMA_7_DEVICE_ID
	#elif (XPAR_XZDMA_7_BASEADDR == 0xFFA90000U)
		#define XILPM_ZDMA_1 XPAR_XZDMA_7_DEVICE_ID
	#elif (XPAR_XZDMA_7_BASEADDR == 0xFFAA0000U)
		#define XILPM_ZDMA_2 XPAR_XZDMA_7_DEVICE_ID
	#elif (XPAR_XZDMA_7_BASEADDR == 0xFFAB0000U)
		#define XILPM_ZDMA_3 XPAR_XZDMA_7_DEVICE_ID
	#elif (XPAR_XZDMA_7_BASEADDR == 0xFFAC0000U)
		#define XILPM_ZDMA_4 XPAR_XZDMA_7_DEVICE_ID
	#elif (XPAR_XZDMA_7_BASEADDR == 0xFFAD0000U)
		#define XILPM_ZDMA_5 XPAR_XZDMA_7_DEVICE_ID
	#elif (XPAR_XZDMA_7_BASEADDR == 0xFFAE0000U)
		#define XILPM_ZDMA_6 XPAR_XZDMA_7_DEVICE_ID
	#elif (XPAR_XZDMA_7_BASEADDR == 0xFFAF0000U)
		#define XILPM_ZDMA_7 XPAR_XZDMA_7_DEVICE_ID
	#endif
#endif

#if defined(XILPM_QSPI_0)
#include "xqspipsu.h"
int NodeQspiIdle(u16 DeviceId, u32 BaseAddress);
#endif

#if defined(XILPM_OSPI_0)
#include "xospipsv.h"
int NodeOspiIdle(u16 DeviceId, u32 BaseAddress);
#endif

#if defined(XILPM_SD_0) || defined(XILPM_SD_1)
#include "xsdps.h"
int NodeSdioIdle(u16 DeviceId, u32 BaseAddress);
#endif

#if defined(XILPM_USB_0)
#include "xusbpsu.h"
int NodeUsbIdle(u16 DeviceId, u32 BaseAddress);
#endif

#if defined(XILPM_ETH_0) || defined(XILPM_ETH_1)
#include "xemacps_hw.h"
int NodeGemIdle(u16 DeviceId, u32 BaseAddress);
#endif

#if (defined(XILPM_ZDMA_0) || \
	defined(XILPM_ZDMA_1) || \
	defined(XILPM_ZDMA_2) || \
	defined(XILPM_ZDMA_3) || \
	defined(XILPM_ZDMA_4) || \
	defined(XILPM_ZDMA_5) || \
	defined(XILPM_ZDMA_6) || \
	defined(XILPM_ZDMA_7))
#include "xzdma_hw.h"
int NodeZdmaIdle(u16 DeviceId, u32 BaseAddr);
#endif

int XPmDevice_SoftResetIdle(const XPm_Device *Device, const u32 IdleReq);

#ifdef __cplusplus
}
#endif

#endif /* XPM_DEVICE_IDLE_H_ */
