/******************************************************************************
* Copyright (c) 2020 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
 *****************************************************************************/

 /*****************************************************************************/
 /**
 *
 * @file xilpuf_enc_dec_data_example.c
 *
 * This file illustrates encryption and decryption of user data using PUF KEY.
 * The key can be generated using either PUF registration or PUF on demand
 * regeneration.
 *
 * <pre>
 * MODIFICATION HISTORY:
 *
 * Ver   Who   Date     Changes
 * ----- ---  -------- -------------------------------------------------------
 * 1.0   har  01/30/20 Initial release
 * 1.1   har  01/30/20 Updated file version to 1.1 to sync with library version
 *       har  03/08/20 Added function to print array
 *                     Corrected endianness of PUF helper data
 * 1.2   har  07/03/20 Corrected the length of PUF ID passed in XPuf_ShowData
 *       am   08/14/20 Replacing function prototype and local status variable
 *  				   from u32 and s32 to int.
 *
 * @note
 *
 * User configurable parameters for PUF
 *------------------------------------------------------------------------------
 * #define XPUF_DATA				"000000000000000000000000"
 * Data to be encrypted by PUF KEY should be provided in string format.
 *
 * #define XPUF_DATA_LEN_IN_BYTES		(0U)
 * Length of data to be encrypted should be provided in bytes.
 *
 * #define XPUF_IV				"000000000000000000000000"
 * IV should be provided in string format. It should be 24 characters long, valid
 * characters are 0-9, a-f, A-F. Any other character is considered as invalid
 * string. The value mentioned here will be converted to hex buffer. It is used
 * with the AES-GCM cryptographic hardware in order to encrypt user data.
 *
 * #define XPUF_KEY_GENERATE_OPTION		(XPUF_REGISTRATION)
 *							(or)
 *						(XPUF_REGEN_ON_DEMAND)
 * PUF helper data can be generated by PUF registration or PUF on-demand
 * regeneration. The user can configure XPUF_KEY_GENERATE_OPTION as either
 * XPUF_REGISTRATION or XPUF_REGEN_ON_DEMAND to select the mode of PUF operation
 * to generate helper data
 *
 * #define XPUF_READ_HD_OPTION			(XPUF_READ_FROM_RAM)
 *							(or)
 *						(XPUF_READ_FROM_EFUSE_CACHE)
 * This selects the location from where the helper data must be read by the
 * application. This option must be configured if XPUF_KEY_GENERATE_OPTION
 * is configured as XPUF_REGEN_ON_DEMAND.
 *
 * #define XPUF_CHASH				(0x00000000)
 * The length of CHASH should be 24 bits. It is valid only for PUF regeneration
 * and invalid for PUF registration. CHASH value should be supplied if
 * XPUF_READ_HD_OPTION is configured as XPUF_READ_FROM_RAM.
 *
 * #define XPUF_AUX				(0x00000000)
 * The length of AUX should be 32 bits. It is valid only for PUF regeneration
 * and invalid for PUF registration. AUX value should be supplied if
 * XPUF_READ_HD_OPTION is configured as XPUF_READ_FROM_RAM.
 *
 * #define XPUF_SYN_DATA_ADDRESS		(0x00000000)
 * Address of syndrome data should be supplied if XPUF_READ_HD_OPTION is
 * configured as XPUF_READ_FROM_RAM.
 *
 *****************************************************************************/
/***************************** Include Files *********************************/
#include "xpuf.h"
#include "xsecure_aes.h"
#include "xil_util.h"
#include "xil_mem.h"

/************************** Constant Definitions ****************************/
/* User configurable parameters start*/
#define XPUF_DATA 				"000000000000000000000000000000"
#define XPUF_DATA_LEN_IN_BYTES			(0U)
						/* Data length in Bytes */
#define XPUF_IV					"000000000000000000000000"

#define XPUF_KEY_GENERATE_OPTION		(XPUF_REGISTRATION)

#if (XPUF_KEY_GENERATE_OPTION == XPUF_REGISTRATION)
#define XPUF_WRITE_HD_OPTION 			(XPUF_WRITE_ON_UART)
#endif

#if (XPUF_KEY_GENERATE_OPTION == XPUF_REGEN_ON_DEMAND)
#define XPUF_READ_HD_OPTION			(XPUF_READ_FROM_RAM)
#define XPUF_CHASH				(0x00000000U)
#define XPUF_AUX				(0x00000000U)
#define XPUF_SYN_DATA_ADDRESS			(0x00000000U)
#endif
/*User configurable parameters end */

#define XPUF_PMCDMA_DEVICEID			PMCDMA_0_DEVICE_ID
#define XPUF_IV_LEN_IN_BYTES			(12U)
						/* IV Length in bytes */
#define XPUF_DATA_LEN_IN_BITS			(XPUF_DATA_LEN_IN_BYTES * 8U)
						/* Data length in Bits */
#define XPUF_IV_LEN_IN_BITS			(XPUF_IV_LEN_IN_BYTES * 8U)
						/* IV length in Bits */
#define XPUF_GCM_TAG_SIZE			(XSECURE_SECURE_GCM_TAG_SIZE)
						/* GCM tag Length in bytes */
#define XPUF_HD_LEN_IN_WORDS			(386U)
#define XPUF_ID_LEN_IN_BYTES			(XPUF_ID_LEN_IN_WORDS * XPUF_WORD_LENGTH)
#define XPUF_DEBUG_INFO				(1U)

/************************** Type Definitions *********************************/
static XPuf_Data PufData;

static u8 Iv[XPUF_IV_LEN_IN_BYTES];
#if defined (__GNUC__)
static u8 Data[XPUF_DATA_LEN_IN_BYTES]__attribute__ ((aligned (64)))
				__attribute__ ((section (".data.Data")));
static u8 DecData[XPUF_DATA_LEN_IN_BYTES]__attribute__ ((aligned (64)))
				__attribute__ ((section (".data.DecData")));
static u8 EncData[XPUF_DATA_LEN_IN_BYTES]__attribute__ ((aligned (64)))
				__attribute__ ((section (".data.EncData")));
static u8 GcmTag[XPUF_GCM_TAG_SIZE]__attribute__ ((aligned (64)))
				__attribute__ ((section (".data.GcmTag")));
#elif defined (__ICCARM__)
#pragma data_alignment = 64
static u8 Data[XPUF_DATA_LEN_IN_BYTES];
#pragma data_alignment = 64
static u8 DecData[XPUF_DATA_LEN_IN_BYTES];
#pragma data_alignment = 64
static u8 EncData[XPUF_DATA_LEN_IN_BYTES];
#pragma data_alignment = 64
static u8 GcmTag[XPUF_GCM_TAG_SIZE];
#endif

/************************** Function Prototypes ******************************/
static int XPuf_GenerateKey(void);
static int XPuf_VerifyDataEncDec(void);
static void XPuf_ShowData(const u8* Data, u32 Len);

/************************** Function Definitions *****************************/
int main(void)
{
	int Status = XST_FAILURE;

	/* Generate PUF KEY
	 */
	Status = XPuf_GenerateKey();
	if (Status == XST_SUCCESS) {
		xPuf_printf(XPUF_DEBUG_INFO, "\r\n Successfully generated "
					"PUF KEY %x\r\n", Status);
	}
	else {
		xPuf_printf(XPUF_DEBUG_INFO, "\r\n PUF KEY generation failed %x\r\n",
					Status);
		goto END;
	}

	/* Encryption using PUF KEY followed by decryption and then comparing
	 * decrypted data with original data */
	Status = XPuf_VerifyDataEncDec();
	if (Status == XST_SUCCESS) {
		xPuf_printf(XPUF_DEBUG_INFO, "\r\nSuccessfully encrypted and "
				"decrypted user data %x\r\n", Status);
	}
	else {
		xPuf_printf(XPUF_DEBUG_INFO, "\r\nEncryption/Decryption failed"
				"%x\r\n", Status);
	}

	if (Status == XST_SUCCESS) {
		xPuf_printf(XPUF_DEBUG_INFO, "Successfully ran Xilpuf enc dec "
			"data example\r\n");
	}

END:
	return Status;
}

/******************************************************************************/
/**
 *
 * @brief	This function generates PUF KEY by PUF registration or PUF on demand
 * 			regeneration as per the user provided inputs.
 *
 * @param	None.
 *
 * @return
 *		- XST_SUCCESS - If PUF_KEY generation was successful.
 * 		- XPUF_ERROR_INVALID_PARAM              - PufData is NULL.
 *		- XPUF_ERROR_INVALID_SYNDROME_MODE      - Incorrect Registration mode.
 *		- XPUF_ERROR_SYNDROME_WORD_WAIT_TIMEOUT - Timeout occurred while waiting
 *												  for PUF Syndrome data.
 *		- XPUF_ERROR_SYNDROME_DATA_OVERFLOW    - Syndrome data overflow reported
 *												 by PUF controller or more than
 *												 required data is provided by
 *												 PUF controller.
 *	    - XPUF_ERROR_SYNDROME_DATA_UNDERFLOW   - Number of syndrome data words
 *												 are less than expected number
 * 												 of words.
 *		- XPUF_ERROR_INVALID_REGENERATION_TYPE - Selection of invalid
 *			 									 regeneration type.
 *		- XPUF_ERROR_CHASH_NOT_PROGRAMMED      - Helper data not provided.
 *		- XPUF_ERROR_PUF_STATUS_DONE_TIMEOUT   - Timeout before Status was done.
 *
 *		- XST_FAILURE 						   - if PUF KEY generation failed.
 *
 ******************************************************************************/
static int XPuf_GenerateKey(void)
{
	int Status = XST_FAILURE;
#if (XPUF_KEY_GENERATE_OPTION == XPUF_REGISTRATION)
	u32 PUF_HelperData[XPUF_HD_LEN_IN_WORDS] = {0U};
#endif

	PufData.ShutterValue = XPUF_SHUTTER_VALUE;
	PufData.RegMode = XPUF_SYNDROME_MODE_4K;

	PufData.PufOperation = XPUF_KEY_GENERATE_OPTION;
#if (XPUF_KEY_GENERATE_OPTION == XPUF_REGISTRATION)
	Status = XPuf_Registration(&PufData);
	if (Status != XST_SUCCESS) {
		goto END;
	}
	xPuf_printf(XPUF_DEBUG_INFO, "Provided PUF helper "
				"on UART\r\n");
	xPuf_printf(XPUF_DEBUG_INFO, "PUF Helper data Start\r\n");
	Xil_MemCpy(PUF_HelperData, PufData.SyndromeData,
		XPUF_4K_PUF_SYN_LEN_IN_WORDS * sizeof(u32));
	PUF_HelperData[XPUF_HD_LEN_IN_WORDS-2] = PufData.Chash;
	PUF_HelperData[XPUF_HD_LEN_IN_WORDS-1] = PufData.Aux;
	XPuf_ShowData((u8*)PUF_HelperData, XPUF_HD_LEN_IN_WORDS * sizeof(u32));
	xPuf_printf(XPUF_DEBUG_INFO,"Chash: %02x", PufData.Chash);
	xPuf_printf(XPUF_DEBUG_INFO,"\r\n");
	xPuf_printf(XPUF_DEBUG_INFO,"Aux: %02x", PufData.Aux);
	xPuf_printf(XPUF_DEBUG_INFO, "\r\n");
	xPuf_printf(XPUF_DEBUG_INFO, "PUF Helper data End\r\n");
	xPuf_printf(XPUF_DEBUG_INFO, "PUF ID : ");
	XPuf_ShowData((u8*)PufData.PufID, XPUF_ID_LEN_IN_BYTES);

#elif (XPUF_KEY_GENERATE_OPTION == XPUF_REGEN_ON_DEMAND)
	PufData.ReadOption = XPUF_READ_HD_OPTION;
	if (PufData.ReadOption == XPUF_READ_FROM_RAM) {
		PufData.Chash = XPUF_CHASH;
		PufData.Aux = XPUF_AUX;
		PufData.SyndromeAddr = XPUF_SYN_DATA_ADDRESS;
		xPuf_printf(XPUF_DEBUG_INFO, "Reading helper"
			"data from provided address\r\n");
	}
	else if (PufData.ReadOption == XPUF_READ_FROM_EFUSE_CACHE) {
		xPuf_printf(XPUF_DEBUG_INFO, "Reading helper data "
			"from eFUSE\r\n");
	}
	else {
		xPuf_printf(XPUF_DEBUG_INFO, "Invalid read option for "
			"reading helper data\r\n");
	}
	Status = XPuf_Regeneration(&PufData);
	if (Status != XST_SUCCESS) {
		xPuf_printf(XPUF_DEBUG_INFO, "Puf Regeneration failed"
			"with error:%x\r\n", Status);
		goto END;
	}
	xPuf_printf(XPUF_DEBUG_INFO, "PUF ID : ");
	XPuf_ShowData((u8*)PufData.PufID, XPUF_ID_LEN_IN_BYTES);
#else
	#error "Invalid option selected for generating PUF KEY. Only Puf \
registration and on demand regeneration are allowed"
#endif

END:
	return Status;
}

/******************************************************************************/
/**
 *
 * @brief	This function encrypts the data with PUF key and IV and decrypts
 * 			the encrypted data also checks whether GCM tag is matched or not
 * 			and compares the decrypted data with the original data provided.
 *
 * @param	None.
 *
 * @return
 *		- XST_SUCCESS - If encryption was successful.
 *  	- XNVM_EFUSE_ERR_INVALID_PARAM - On Invalid Parameter.
 *		- XST_FAILURE 				   - On failure of AES Encrypt
 *										 Initialization, AES Encrypt data,
 *										 format AES key and AES decrypt
 *										 Initialization and data.
 *
 ******************************************************************************/
static int XPuf_VerifyDataEncDec(void)
{
	int Status = XST_FAILURE;
	XPmcDma_Config *Config;
	u32 Index;
	XPmcDma PmcDmaInstance;
	XSecure_Aes SecureAes;

	Xil_DCacheDisable();

	if (Xil_Strnlen(XPUF_IV, (XPUF_IV_LEN_IN_BYTES * 2U)) ==
				(XPUF_IV_LEN_IN_BYTES * 2U)) {
		Status = Xil_ConvertStringToHexBE((const char *)(XPUF_IV), Iv,
						XPUF_IV_LEN_IN_BITS);
		if (Status != XST_SUCCESS) {
			xPuf_printf(XPUF_DEBUG_INFO, "String Conversion error"
				"(IV):%08x !!!\r\n", Status);
			goto END;
		}
	}
	else {
		xPuf_printf(XPUF_DEBUG_INFO, "Provided IV length is wrong\r\n");
		goto END;
	}

	if (Xil_Strnlen(XPUF_DATA, (XPUF_DATA_LEN_IN_BYTES * 2U)) ==
				(XPUF_DATA_LEN_IN_BYTES * 2U)) {
		Status = Xil_ConvertStringToHexBE(
			(const char *) (XPUF_DATA),
			Data, XPUF_DATA_LEN_IN_BITS);
		if (Status != XST_SUCCESS) {
			xPuf_printf(XPUF_DEBUG_INFO, "String Conversion error"
			"(Data):%08x !!!\r\n", Status);
			goto END;
		}
	}
	else {
		Status = XST_FAILURE;
		xPuf_printf(XPUF_DEBUG_INFO, "Provided data length is wrong\r\n");
		goto END;
	}

	/* Initialize PMC DMA driver */
	Config = XPmcDma_LookupConfig(XPUF_PMCDMA_DEVICEID);
	if (Config == NULL) {
		goto END;
	}
	Status = XPmcDma_CfgInitialize(&PmcDmaInstance, Config,
			Config->BaseAddress);
	if (Status != XST_SUCCESS) {
		goto END;
	}

	/* Initialize the Aes driver so that it's ready to use */
	XSecure_AesInitialize(&SecureAes, &PmcDmaInstance);

	xPuf_printf(XPUF_DEBUG_INFO, "Data to be encrypted: \n\r");
	XPuf_ShowData((u8*)Data, XPUF_DATA_LEN_IN_BYTES);

	xPuf_printf(XPUF_DEBUG_INFO, "IV: \n\r");
	XPuf_ShowData((u8*)Iv, XPUF_IV_LEN_IN_BYTES);

	/* Encryption of Data */
	Status = XSecure_AesEncryptInit(&SecureAes, XSECURE_AES_PUF_KEY,
				XSECURE_AES_KEY_SIZE_256, (u64)Iv);
	if (Status != XST_SUCCESS) {
		xPuf_printf(XPUF_DEBUG_INFO, " Aes encrypt init failed %x\n\r"
			, Status);
		goto END;
	}

	Status = XSecure_AesEncryptData(&SecureAes, (u64)Data, (u64)EncData,
			XPUF_DATA_LEN_IN_BYTES, (u64)GcmTag);
	if (Status != XST_SUCCESS) {
		xPuf_printf(XPUF_DEBUG_INFO, " Data encryption failed %x\n\r"
			, Status);
		goto END;
	}

	xPuf_printf(XPUF_DEBUG_INFO, "\r\nEncrypted data: \n\r");
	XPuf_ShowData((u8*)EncData, XPUF_DATA_LEN_IN_BYTES);

	xPuf_printf(XPUF_DEBUG_INFO, "GCM tag: \n\r");
	XPuf_ShowData((u8*)GcmTag, XPUF_GCM_TAG_SIZE);

	/* Initialize the Aes driver so that it's ready to use */
	XSecure_AesInitialize(&SecureAes, &PmcDmaInstance);

	/* Decryption of Data */
	Status = XSecure_AesDecryptInit(&SecureAes, XSECURE_AES_PUF_KEY,
			XSECURE_AES_KEY_SIZE_256, (u64)Iv);
	if (Status != XST_SUCCESS) {
		xPuf_printf(XPUF_DEBUG_INFO, "Error in decrypt init %x\n\r"
					, Status);
		goto END;
	}

	Status = XSecure_AesDecryptData(&SecureAes, (u64)EncData, (u64)DecData,
			XPUF_DATA_LEN_IN_BYTES, (u64)GcmTag);
	if (Status != XST_SUCCESS) {
		xPuf_printf(XPUF_DEBUG_INFO, "Data encryption failed %x\n\r"
			, Status);
		goto END;
	}

	xPuf_printf(XPUF_DEBUG_INFO, "\r\nDecrypted data \n\r ");
	XPuf_ShowData((u8*)DecData, XPUF_DATA_LEN_IN_BYTES);

	/* Comparison of Decrypted Data with original data */
	for(Index = 0U; Index < XPUF_DATA_LEN_IN_BYTES; Index++) {
		if (Data[Index] != DecData[Index]) {
			xPuf_printf(XPUF_DEBUG_INFO, "Failure during comparison "
				"of the data\n\r");
			Status = XST_FAILURE;
		}
	}
END:
	return Status;
}

/******************************************************************************/
/**
 *
 * @brief	This function prints the data array.
 *
 * @param	Data - Pointer to the data to be printed.
 * @param	Len  - Length of the data in bytes.
 *
 * @return	None.
 *
 ******************************************************************************/
static void XPuf_ShowData(const u8* Data, u32 Len)
{
	u32 Index;

	for (Index = 0U; Index < Len; Index++) {
		xPuf_printf(XPUF_DEBUG_INFO, "%02x", Data[Index]);
	}
	xPuf_printf(XPUF_DEBUG_INFO, "\r\n");
}
