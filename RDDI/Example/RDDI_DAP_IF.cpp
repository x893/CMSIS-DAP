/* 
 * RDDI_DAP_IF.cpp: ULINK to RDDI-DAP Interface
 *
 * Copyright (c) 2012, ARM Limited. All Rights Reserved.
 */

#include "stdafx.h"

#define _RDDI_LOADLIBRARY
#include "..\CMSIS_DAP\rddi.h"
#include "..\CMSIS_DAP\rddi_dap.h"
#include "..\CMSIS_DAP\rddi_dap_cmsis.h"

#include "Debug.h"
#include "RDDI_DAP_IF.h"


// RDDI DLL function pointers
static RDDI_Open                         *rddi_Open;
static RDDI_Close                        *rddi_Close;
static RDDI_GetLastError                 *rddi_GetLastError;
static RDDI_SetLogCallback               *rddi_SetLogCallback;

// RDDI-DAP DLL Level 0 function pointers
static DAP_GetInterfaceVersion           *rddi_DAP_GetInterfaceVersion;
static DAP_Configure                     *rddi_DAP_Configure;
static DAP_Connect                       *rddi_DAP_Connect;
static DAP_Disconnect                    *rddi_DAP_Disconnect;
static DAP_GetSupportedOptimisationLevel *rddi_DAP_GetSupportedOptimisationLevel;
static DAP_GetNumberOfDAPs               *rddi_DAP_GetNumberOfDAPs;
static DAP_GetDAPIDList                  *rddi_DAP_GetDAPIDList;
static DAP_ReadReg                       *rddi_DAP_ReadReg;
static DAP_WriteReg                      *rddi_DAP_WriteReg;
static DAP_RegAccessBlock                *rddi_DAP_RegAccessBlock;
static DAP_RegWriteBlock                 *rddi_DAP_RegWriteBlock;
static DAP_RegReadBlock                  *rddi_DAP_RegReadBlock;
static DAP_RegWriteRepeat                *rddi_DAP_RegWriteRepeat;
static DAP_RegReadRepeat                 *rddi_DAP_RegReadRepeat;
static DAP_RegReadWaitForValue           *rddi_DAP_RegReadWaitForValue;
static DAP_Target                        *rddi_DAP_Target;

// RDDI-DAP DLL Level 1 function pointers
static DAP_DefineSequence                *rddi_DAP_DefineSequence;
static DAP_RunSequence                   *rddi_DAP_RunSequence;

// CMSIS-DAP DLL function pointers
static CMSIS_DAP_Detect                  *rddi_CMSIS_DAP_Detect;
static CMSIS_DAP_Identify                *rddi_CMSIS_DAP_Identify;
static CMSIS_DAP_ConfigureInterface      *rddi_CMSIS_DAP_ConfigureInterface;
static CMSIS_DAP_DetectNumberOfDAPs      *rddi_CMSIS_DAP_DetectNumberOfDAPs;
static CMSIS_DAP_DetectDAPIDList         *rddi_CMSIS_DAP_DetectDAPIDList;
static CMSIS_DAP_Commands                *rddi_CMSIS_DAP_Commands;


// Global variables
BOOL       RDDI_DAP_IF  = FALSE;        // RDDI-DAP Interface Active Flag
HINSTANCE  RDDI_DAP_DLL = NULL;         // RDDI-DAP DLL Handle
RDDIHandle rddiHandle;                  // RDDI Handle

RDDI_DAP_CONN_DETAILS rddiConnDetails;  // RDDI Connection Details


// Initialize RDDI-DAP Interface
//   return : 0 - Success, else Error Code
int RDDI_DAP_Init (void) {
  char name[MAX_PATH];
  int  status;

  rddiHandle = NULL;

  strcpy(name, MonConf.DriverPath);
  strcat(name, "CMSIS_DAP.dll");
  RDDI_DAP_DLL = LoadLibrary(name);
  if (RDDI_DAP_DLL == NULL) return (RDDI_DAP_ERROR_NO_DLL);

  rddi_Open                              = RDDILL_GetProcAddress(RDDI_DAP_DLL, RDDI_Open);
  rddi_Close                             = RDDILL_GetProcAddress(RDDI_DAP_DLL, RDDI_Close);
  rddi_GetLastError                      = RDDILL_GetProcAddress(RDDI_DAP_DLL, RDDI_GetLastError);
  rddi_SetLogCallback                    = RDDILL_GetProcAddress(RDDI_DAP_DLL, RDDI_SetLogCallback);

  rddi_DAP_GetInterfaceVersion           = RDDILL_GetProcAddress(RDDI_DAP_DLL, DAP_GetInterfaceVersion);
  rddi_DAP_Configure                     = RDDILL_GetProcAddress(RDDI_DAP_DLL, DAP_Configure);
  rddi_DAP_Connect                       = RDDILL_GetProcAddress(RDDI_DAP_DLL, DAP_Connect);
  rddi_DAP_Disconnect                    = RDDILL_GetProcAddress(RDDI_DAP_DLL, DAP_Disconnect);
  rddi_DAP_GetSupportedOptimisationLevel = RDDILL_GetProcAddress(RDDI_DAP_DLL, DAP_GetSupportedOptimisationLevel);
  rddi_DAP_GetNumberOfDAPs               = RDDILL_GetProcAddress(RDDI_DAP_DLL, DAP_GetNumberOfDAPs);
  rddi_DAP_GetDAPIDList                  = RDDILL_GetProcAddress(RDDI_DAP_DLL, DAP_GetDAPIDList);
  rddi_DAP_ReadReg                       = RDDILL_GetProcAddress(RDDI_DAP_DLL, DAP_ReadReg);
  rddi_DAP_WriteReg                      = RDDILL_GetProcAddress(RDDI_DAP_DLL, DAP_WriteReg);
  rddi_DAP_RegAccessBlock                = RDDILL_GetProcAddress(RDDI_DAP_DLL, DAP_RegAccessBlock);
  rddi_DAP_RegWriteBlock                 = RDDILL_GetProcAddress(RDDI_DAP_DLL, DAP_RegWriteBlock);
  rddi_DAP_RegReadBlock                  = RDDILL_GetProcAddress(RDDI_DAP_DLL, DAP_RegReadBlock);
  rddi_DAP_RegWriteRepeat                = RDDILL_GetProcAddress(RDDI_DAP_DLL, DAP_RegWriteRepeat);
  rddi_DAP_RegReadRepeat                 = RDDILL_GetProcAddress(RDDI_DAP_DLL, DAP_RegReadRepeat);
  rddi_DAP_RegReadWaitForValue           = RDDILL_GetProcAddress(RDDI_DAP_DLL, DAP_RegReadWaitForValue);
  rddi_DAP_Target                        = RDDILL_GetProcAddress(RDDI_DAP_DLL, DAP_Target);

  rddi_DAP_DefineSequence                = RDDILL_GetProcAddress(RDDI_DAP_DLL, DAP_DefineSequence);
  rddi_DAP_RunSequence                   = RDDILL_GetProcAddress(RDDI_DAP_DLL, DAP_RunSequence);

  rddi_CMSIS_DAP_Detect                  = RDDILL_GetProcAddress(RDDI_DAP_DLL, CMSIS_DAP_Detect);
  rddi_CMSIS_DAP_Identify                = RDDILL_GetProcAddress(RDDI_DAP_DLL, CMSIS_DAP_Identify);
  rddi_CMSIS_DAP_ConfigureInterface      = RDDILL_GetProcAddress(RDDI_DAP_DLL, CMSIS_DAP_ConfigureInterface);
  rddi_CMSIS_DAP_DetectNumberOfDAPs      = RDDILL_GetProcAddress(RDDI_DAP_DLL, CMSIS_DAP_DetectNumberOfDAPs);
  rddi_CMSIS_DAP_DetectDAPIDList         = RDDILL_GetProcAddress(RDDI_DAP_DLL, CMSIS_DAP_DetectDAPIDList);
  rddi_CMSIS_DAP_Commands                = RDDILL_GetProcAddress(RDDI_DAP_DLL, CMSIS_DAP_Commands);

  if (rddi_Open                              == NULL) goto error;
  if (rddi_Close                             == NULL) goto error;
  if (rddi_GetLastError                      == NULL) goto error;
  if (rddi_SetLogCallback                    == NULL) goto error;

  if (rddi_DAP_GetInterfaceVersion           == NULL) goto error;
  if (rddi_DAP_Configure                     == NULL) goto error;
  if (rddi_DAP_Connect                       == NULL) goto error;
  if (rddi_DAP_Disconnect                    == NULL) goto error;
  if (rddi_DAP_GetSupportedOptimisationLevel == NULL) goto error;
  if (rddi_DAP_GetNumberOfDAPs               == NULL) goto error;
  if (rddi_DAP_GetDAPIDList                  == NULL) goto error;
  if (rddi_DAP_ReadReg                       == NULL) goto error;
  if (rddi_DAP_WriteReg                      == NULL) goto error;
  if (rddi_DAP_RegAccessBlock                == NULL) goto error;
  if (rddi_DAP_RegWriteBlock                 == NULL) goto error;
  if (rddi_DAP_RegReadBlock                  == NULL) goto error;
  if (rddi_DAP_RegWriteRepeat                == NULL) goto error;
  if (rddi_DAP_RegReadRepeat                 == NULL) goto error;
  if (rddi_DAP_RegReadWaitForValue           == NULL) goto error;
  if (rddi_DAP_Target                        == NULL) goto error;

  if (rddi_DAP_DefineSequence                == NULL) goto error;
  if (rddi_DAP_RunSequence                   == NULL) goto error;

  if (rddi_CMSIS_DAP_Detect                  == NULL) goto error;
  if (rddi_CMSIS_DAP_Identify                == NULL) goto error;
  if (rddi_CMSIS_DAP_ConfigureInterface      == NULL) goto error;
  if (rddi_CMSIS_DAP_DetectNumberOfDAPs      == NULL) goto error;
  if (rddi_CMSIS_DAP_DetectDAPIDList         == NULL) goto error;
  if (rddi_CMSIS_DAP_Commands                == NULL) goto error;

  // RDDI Open
  status = rddi_Open(&rddiHandle, NULL);
  if (status) goto error;

  return (0);

error:
  FreeLibrary(RDDI_DAP_DLL);
  return (RDDI_DAP_ERROR);
}


// Uninitialize RDDI-DAP Interface
//   return : 0 - Success, else Error Code
int RDDI_DAP_UnInit (void) {

  if (rddiHandle) {
    rddi_Close(rddiHandle);
    rddiHandle = NULL;
  }
  if (RDDI_DAP_DLL != NULL) {
    FreeLibrary(RDDI_DAP_DLL);
    RDDI_DAP_DLL = NULL;
  }
  return (0);
}


// RDDI-DAP Detect Number of intefaces
//   num    : Pointer to Number
//   return : 0 - Success, else Error Code
int RDDI_DAP_Detect (int *num) {
  int status;

  if (rddiHandle == NULL) return (RDDI_DAP_ERROR_INTERNAL);

  status = rddi_CMSIS_DAP_Detect(rddiHandle, num);
  if (status) return (RDDI_DAP_ERROR);

  return (0);
}


// RDDI-DAP Get Vendor ID string
//   idx    : Zero based index of interface
//   str    : Buffer that will be filled with identification string
//   len    : Length of str buffer in bytes
//   return : 0 - Success, else Error Code
int RDDI_DAP_Vendor (int  idx, char *str, int len) {
  int status;

  if (rddiHandle == NULL) return (RDDI_DAP_ERROR_INTERNAL);

  status = rddi_CMSIS_DAP_Identify(rddiHandle, idx, RDDI_CMSIS_DAP_ID_VENDOR, str, len);
  if (status) return (RDDI_DAP_ERROR);

  return (0); 
}


// RDDI-DAP Get Product ID string
//   idx    : Zero based index of interface
//   str    : Buffer that will be filled with identification string
//   len    : Length of str buffer in bytes
//   return : 0 - Success, else Error Code
int RDDI_DAP_Product (int  idx, char *str, int len) {
  int status;

  if (rddiHandle == NULL) return (RDDI_DAP_ERROR_INTERNAL);

  status = rddi_CMSIS_DAP_Identify(rddiHandle, idx, RDDI_CMSIS_DAP_ID_PRODUCT, str, len);
  if (status) return (RDDI_DAP_ERROR);

  return (0); 
}


// RDDI-DAP Get Serial Number
//   idx    : Zero based index of interface
//   str    : Buffer that will be filled with identification string
//   len    : Length of str buffer in bytes
//   return : 0 - Success, else Error Code
int RDDI_DAP_SerialNo (int  idx, char *str, int len) {
  int status;

  if (rddiHandle == NULL) return (RDDI_DAP_ERROR_INTERNAL);

  status = rddi_CMSIS_DAP_Identify(rddiHandle, idx, RDDI_CMSIS_DAP_ID_SER_NUM, str, len);
  if (status) return (RDDI_DAP_ERROR);

  return (0); 
}


// RDDI-DAP Get Firmware Version
//   idx    : Zero based index of interface
//   str    : Buffer that will be filled with identification string
//   len    : Length of str buffer in bytes
//   return : 0 - Success, else Error Code
int RDDI_DAP_FWVersion (int  idx, char *str, int len) {
  int status;

  if (rddiHandle == NULL) return (RDDI_DAP_ERROR_INTERNAL);

  status = rddi_CMSIS_DAP_Identify(rddiHandle, idx, RDDI_CMSIS_DAP_ID_FW_VER, str, len);
  if (status) return (RDDI_DAP_ERROR);

  return (0); 
}


// RDDI-DAP Configure DAP interface
//   idx    : Zero based index of interface
//   cfg    : Configuration string (see header file for detailed description)
//   return : 0 - Success, else Error Code
int RDDI_DAP_Configure (int idx, char *cfg) {
  int status;

  if (rddiHandle == NULL) return (RDDI_DAP_ERROR_INTERNAL);

  status = rddi_CMSIS_DAP_ConfigureInterface(rddiHandle, idx, cfg);
  if (status) return (RDDI_DAP_ERROR);

  status = rddi_DAP_Configure(rddiHandle, NULL);
  if (status) return (RDDI_DAP_ERROR);

  return (0);
}


// RDDI-DAP Connect to configured DAP interface
//   return : 0 - Success, else Error Code
int RDDI_DAP_Connect (void) {
  int status;

  if (rddiHandle == NULL) return (RDDI_DAP_ERROR_INTERNAL);

  status = rddi_DAP_Connect(rddiHandle, &rddiConnDetails);
  if (status == RDDI_DAP_DAPINUSE) return (RDDI_DAP_ERROR_INUSE);
  if (status) return (RDDI_DAP_ERROR);

  return (0);
}


// RDDI-DAP Disconnect from configured/connected DAP interface
//   return : 0 - Success, else Error Code
int RDDI_DAP_Disconnect (void) {
  int status;

  if (rddiHandle == NULL) return (RDDI_DAP_ERROR_INTERNAL);

  status = rddi_DAP_Disconnect(rddiHandle);
  if (status) return (RDDI_DAP_ERROR);

  return (0);
}


// RDDI-DAP Get a list of DAP IDs for the configured/connected DAP interface
//   id     : ID array which will be filled with DAP IDs
//   cnt:   : Will be filled with number of DAPs detected
//   max    : Maximum number of DAPs to detect
//   return : 0 - Success, else Error Code
int RDDI_DAP_List (DWORD *id, int *cnt, int max) {
  int status;

  if (rddiHandle == NULL) return (RDDI_DAP_ERROR_INTERNAL);

  status = rddi_CMSIS_DAP_DetectNumberOfDAPs(rddiHandle, cnt);
  if (status != RDDI_SUCCESS) {
    if (status == RDDI_DAP_OPERATION_FAILED) {
      return (RDDI_DAP_ERROR_SWJ);
    }
    return (RDDI_DAP_ERROR);
  }
  if (status) return (RDDI_DAP_ERROR);

  if (*cnt > max) return (RDDI_DAP_ERROR);

  status = rddi_CMSIS_DAP_DetectDAPIDList(rddiHandle, (int *)id, max*sizeof(int));
  if (status) return (RDDI_DAP_ERROR);

  return (0);
}



// RDDI-DAP Write Abort Register
//   val    : Value
//   return : 0 - Success, else Error Code
int RDDI_DAP_Abort (DWORD val) {
  int status;

  if (rddiHandle == NULL) return (RDDI_DAP_ERROR_INTERNAL);

  // Write Abort Register
  status = rddi_DAP_WriteReg(rddiHandle, rddiDAP_ID, DAP_REG_DP_ABORT, val);
  if (status) return (RDDI_DAP_ERROR_DEBUG);

  return (0);
}


// RDDI-DAP Check Status of Register Access
//   status : Status of previous Register Access Operation
//   return : 0 - Success, else Error Code
static int RDDI_DAP_CheckStatus (int status) {

  if (status == RDDI_DAP_OPERATION_TIMEOUT) {
    status = RDDI_DAP_Abort(DAPABORT);
    if (status) return (status);
    return (RDDI_DAP_ERROR_MEMORY);
  }
  if (status == RDDI_DAP_DP_STICKY_ERR) {
    // Only for SW (not availalbe for JTAG)
    status = RDDI_DAP_Abort(STKERRCLR | WDERRCLR);
    if (status) return (status);
    return (RDDI_DAP_ERROR_MEMORY);
  }
  if (status) return (RDDI_DAP_ERROR);

  return (0);
}


// RDDI-DAP Read DP Register
//   adr    : Address
//   val    : Pointer to Value
//   return : 0 - Success, else Error Code
int RDDI_DAP_ReadDP (BYTE adr, DWORD *val) {
  int status;
  int abort = 0;

  if (rddiHandle == NULL) return (RDDI_DAP_ERROR_INTERNAL);

dp_access:
  // Read DP Register
  status = rddi_DAP_ReadReg(rddiHandle, rddiDAP_ID, DAP_REG_DP_0x0 + (adr>>2), (int *)val);
  status = RDDI_DAP_CheckStatus(status);
  if (status == RDDI_DAP_ERROR_MEMORY) {
    if (abort) return (RDDI_DAP_ERROR_DEBUG);
    abort = 1;
    goto dp_access;
  }
  if (status) return (status);

  return (0);
}


// RDDI-DAP Write DP Register
//   adr    : Address
//   val    : Value
//   return : 0 - Success, else Error Code
int RDDI_DAP_WriteDP (BYTE adr, DWORD val) {
  int status;
  int abort = 0;

  if (rddiHandle == NULL) return (RDDI_DAP_ERROR_INTERNAL);

dp_access:
  // Write DP Register
  status = rddi_DAP_WriteReg(rddiHandle, rddiDAP_ID, DAP_REG_DP_0x0 + (adr>>2), val);
  status = RDDI_DAP_CheckStatus(status);
  if (status == RDDI_DAP_ERROR_MEMORY) {
    if (abort) return (RDDI_DAP_ERROR_DEBUG);
    abort = 1;
    goto dp_access;
  }
  if (status) return (status);

  return (0);
}


// RDDI-DAP Read AP Register
//   adr    : Address
//   val    : Pointer to Value
//   return : 0 - Success, else Error Code
int RDDI_DAP_ReadAP (BYTE adr, DWORD *val) {
  int status;

  if (rddiHandle == NULL) return (RDDI_DAP_ERROR_INTERNAL);

  if ((adr ^ AP_Bank) & APBANKSEL) {
    status = RDDI_DAP_WriteDP(DP_SELECT, AP_Sel | (adr & APBANKSEL));
    if (status) return (status);
    AP_Bank = adr & APBANKSEL;
  }

  adr &= 0x0F;

  // Read AP Register
  status = rddi_DAP_ReadReg(rddiHandle, rddiDAP_ID, DAP_REG_AP_0x0 + (adr>>2), (int *)val);
  status = RDDI_DAP_CheckStatus(status);
  if (status) return (status);

  return (0);
}


// RDDI-DAP Write AP Register
//   adr    : Address
//   val    : Value
//   return : 0 - Success, else Error Code
int RDDI_DAP_WriteAP (BYTE adr, DWORD val) {
  int status;

  if (rddiHandle == NULL) return (RDDI_DAP_ERROR_INTERNAL);

  if ((adr ^ AP_Bank) & APBANKSEL) {
    status = RDDI_DAP_WriteDP(DP_SELECT, AP_Sel | (adr & APBANKSEL));
    if (status) return (status);
    AP_Bank = adr & APBANKSEL;
  }

  adr &= 0x0F;

  // Write AP Register
  status = rddi_DAP_WriteReg(rddiHandle, rddiDAP_ID, DAP_REG_AP_0x0 + (adr>>2), val);
  status = RDDI_DAP_CheckStatus(status);
  if (status) return (status);

  return (0);
}


// RDDI-DAP Read Data
//   adr    : Address
//   val    : Pointer to Value
//   return : 0 - Success, else Error Code
static int RDDI_DAP_ReadData (DWORD adr, DWORD *val) {
  int status;
  int regID[2];
  int regData[2];

  if (rddiHandle == NULL) return (RDDI_DAP_ERROR_INTERNAL);

  // TAR = adr
  regID[0] = DAP_AP_REG_TAR;
  regData[0] = adr;

  // DRW read
  regID[1] = DAP_AP_REG_DRW | DAP_REG_RnW;

  // R/W DAP Registers
  status = rddi_DAP_RegAccessBlock(rddiHandle, rddiDAP_ID, 2, regID, regData);
  status = RDDI_DAP_CheckStatus(status);
  if (status) return (status);

  *val = regData[1];

  return (0);
}

// RDDI-DAP Read 32-bit Data
//   adr    : Address
//   val    : Pointer to Value
//   return : 0 - Success, else Error Code
int RDDI_DAP_ReadD32 (DWORD adr, DWORD *val) {
  int status;

  if (AP_Bank != 0) {
    status = RDDI_DAP_WriteDP(DP_SELECT, AP_Sel | 0);
    if (status) return (status);
    AP_Bank = 0;
  }

  if ((CSW_Val & CSW_SIZE) != CSW_SIZE32) {
    CSW_Val &= ~CSW_SIZE;
    CSW_Val |=  CSW_SIZE32;
    status = RDDI_DAP_WriteAP(AP_CSW, CSW_Val);
    if (status) return (status);
  }

  status = RDDI_DAP_ReadData(adr, val);
  if (status) return (status);

  return (0);
}

// RDDI-DAP Read 16-bit Data
//   adr    : Address
//   val    : Pointer to Value
//   return : 0 - Success, else Error Code
int RDDI_DAP_ReadD16 (DWORD adr, WORD *val) {
  int   status;
  DWORD v;

  if (AP_Bank != 0) {
    status = RDDI_DAP_WriteDP(DP_SELECT, AP_Sel | 0);
    if (status) return (status);
    AP_Bank = 0;
  }

  if ((CSW_Val & CSW_SIZE) != CSW_SIZE16) {
    CSW_Val &= ~CSW_SIZE;
    CSW_Val |=  CSW_SIZE16;
    status = RDDI_DAP_WriteAP(AP_CSW, CSW_Val);
    if (status) return (status);
  }

  status = RDDI_DAP_ReadData(adr, &v);
  if (status) return (status);

  *val = (WORD)(v >> ((adr & 0x02) << 3));

  return (0);
}

// RDDI-DAP Read 8-bit Data
//   adr    : Address
//   val    : Pointer to Value
//   return : 0 - Success, else Error Code
int RDDI_DAP_ReadD8 (DWORD adr, BYTE *val) {
  int   status;
  DWORD v;

  if (AP_Bank != 0) {
    status = RDDI_DAP_WriteDP(DP_SELECT, AP_Sel | 0);
    if (status) return (status);
    AP_Bank = 0;
  }

  if ((CSW_Val & CSW_SIZE) != CSW_SIZE8) {
    CSW_Val &= ~CSW_SIZE;
    CSW_Val |=  CSW_SIZE8;
    status = RDDI_DAP_WriteAP(AP_CSW, CSW_Val);
    if (status) return (status);
  }

  status = RDDI_DAP_ReadData(adr, &v);
  if (status) return (status);

  *val = (BYTE)(v >> ((adr & 0x03) << 3));

  return (0);
}


// RDDI-DAP Write Data
//   adr    : Address
//   val    : Value
//   return : 0 - Success, else Error Code
static int RDDI_DAP_WriteData (DWORD adr, DWORD val) {
  int status;
  int regID[2];
  int regData[2];

  if (rddiHandle == NULL) return (RDDI_DAP_ERROR_INTERNAL);

  // TAR = adr
  regID[0] = DAP_AP_REG_TAR;
  regData[0] = adr;

  // DRW = val
  regID[1] = DAP_AP_REG_DRW;
  regData[1] = val;

  // R/W DAP Registers
  status = rddi_DAP_RegAccessBlock(rddiHandle, rddiDAP_ID, 2, regID, regData);
  status = RDDI_DAP_CheckStatus(status);
  if (status) return (status);

  return (0);
}

// RDDI-DAP Write 32-bit Data
//   adr    : Address
//   val    : Value
//   return : 0 - Success, else Error Code
int RDDI_DAP_WriteD32 (DWORD adr, DWORD val) {
  int status;

  if (AP_Bank != 0) {
    status = RDDI_DAP_WriteDP(DP_SELECT, AP_Sel | 0);
    if (status) return (status);
    AP_Bank = 0;
  }

  if ((CSW_Val & CSW_SIZE) != CSW_SIZE32) {
    CSW_Val &= ~CSW_SIZE;
    CSW_Val |=  CSW_SIZE32;
    status = RDDI_DAP_WriteAP(AP_CSW, CSW_Val);
    if (status) return (status);
  }

  status = RDDI_DAP_WriteData(adr, val);

  return (status);
}

// RDDI-DAP Write 16-bit Data
//   adr    : Address
//   val    : Value
//   return : 0 - Success, else Error Code
int RDDI_DAP_WriteD16 (DWORD adr, WORD val) {
  int status;

  if (AP_Bank != 0) {
    status = RDDI_DAP_WriteDP(DP_SELECT, AP_Sel | 0);
    if (status) return (status);
    AP_Bank = 0;
  }

  if ((CSW_Val & CSW_SIZE) != CSW_SIZE16) {
    CSW_Val &= ~CSW_SIZE;
    CSW_Val |=  CSW_SIZE16;
    status = RDDI_DAP_WriteAP(AP_CSW, CSW_Val);
    if (status) return (status);
  }

  status = RDDI_DAP_WriteData(adr, (DWORD)val << ((adr & 0x02) << 3));

  return (status);
}

// RDDI-DAP Write 8-bit Data
//   adr    : Address
//   val    : Value
//   return : 0 - Success, else Error Code
int RDDI_DAP_WriteD8 (DWORD adr, BYTE val) {
  int status;

  if (AP_Bank != 0) {
    status = RDDI_DAP_WriteDP(DP_SELECT, AP_Sel | 0);
    if (status) return (status);
    AP_Bank = 0;
  }

  if ((CSW_Val & CSW_SIZE) != CSW_SIZE8) {
    CSW_Val &= ~CSW_SIZE;
    CSW_Val |=  CSW_SIZE8;
    status = RDDI_DAP_WriteAP(AP_CSW, CSW_Val);
    if (status) return (status);
  }

  status = RDDI_DAP_WriteData(adr, (DWORD)val << ((adr & 0x03) << 3));

  return (status);
}


// RDDI-DAP Check/clear Sticky Error Flag
//   dp_stat : Status read from DP Status Register
//   return  : 0 - Success, else Error Code
static int RDDI_DAP_CheckStickyError (DWORD dp_stat) {
  int status;

  if (MonConf.Opt & PORT_SW) {
    // SW Port
    if (dp_stat & (STICKYERR | WDATAERR)) {
      status = RDDI_DAP_Abort(STKERRCLR | WDERRCLR);
      if (status) return (status);
      return (RDDI_DAP_ERROR_MEMORY);
    }
  } else {
    // JTAG Port
    if (dp_stat & STICKYERR) {
      status = RDDI_DAP_WriteDP(DP_CTRL_STAT, dp_stat);
      if (status) return (status);
      return (RDDI_DAP_ERROR_MEMORY);
    }
  }

  return (0);
}


// RDDI-DAP Read and check/clear Sticky Error Flag
//   return : 0 - Success, else Error Code
static int RDDI_DAP_StickyError (void) {
  int   status;
  DWORD dp_stat;

  status = RDDI_DAP_ReadDP(DP_CTRL_STAT, &dp_stat);
  if (status) return (status);

  status = RDDI_DAP_CheckStickyError(dp_stat);
  return (status);
}


// RDDI-DAP Read Data Block (32-bit Elements inside R/W Page Block)
//   adr    : Address
//   pB     : Pointer to Buffer
//   nMany  : Number of bytes
//   return : 0 - Success, else Error Code
int RDDI_DAP_ReadBlock (DWORD adr, BYTE *pB, DWORD nMany) {
  int status;

  if (rddiHandle == NULL) return (RDDI_DAP_ERROR_INTERNAL);

  if (nMany == 0)     return (RDDI_DAP_ERROR_INTERNAL);
  if (nMany & 0x03)   return (RDDI_DAP_ERROR_INTERNAL);
  if (nMany > RWPage) return (RDDI_DAP_ERROR_INTERNAL);

  if (AP_Bank != 0) {
    status = RDDI_DAP_WriteDP(DP_SELECT, AP_Sel | 0);
    if (status) return (status);
    AP_Bank = 0;
  }

  if ((CSW_Val & (CSW_SIZE | CSW_ADDRINC)) != (CSW_SIZE32 | CSW_SADDRINC)) {
    CSW_Val &= ~(CSW_SIZE   | CSW_ADDRINC);
    CSW_Val |=  (CSW_SIZE32 | CSW_SADDRINC);
    status = RDDI_DAP_WriteAP(AP_CSW, CSW_Val);
    if (status) return (status);
  }

  status = RDDI_DAP_WriteAP(AP_TAR, adr);
  if (status) return (status);

  // Multiple Read AP DRW
  status = rddi_DAP_RegReadRepeat(rddiHandle, rddiDAP_ID, nMany>>2, DAP_AP_REG_DRW, (int *)pB);
  status = RDDI_DAP_CheckStatus(status);
  if (status) return (status);

  status = RDDI_DAP_StickyError();
  if (status) return (status);

  return (0);
}


// RDDI-DAP Write Data Block (32-bit Elements inside R/W Page Block)
//   adr    : Address
//   pB     : Pointer to Buffer
//   nMany  : Number of bytes
//   return : 0 - Success, else Error Code
int RDDI_DAP_WriteBlock (DWORD adr, BYTE *pB, DWORD nMany) {
  int   status;

  if (rddiHandle == NULL) return (RDDI_DAP_ERROR_INTERNAL);

  if (nMany == 0)     return (RDDI_DAP_ERROR_INTERNAL);
  if (nMany & 0x03)   return (RDDI_DAP_ERROR_INTERNAL);
  if (nMany > RWPage) return (RDDI_DAP_ERROR_INTERNAL);

  if (AP_Bank != 0) {
    status = RDDI_DAP_WriteDP(DP_SELECT, AP_Sel | 0);
    if (status) return (status);
    AP_Bank = 0;
  }

  if ((CSW_Val & (CSW_SIZE | CSW_ADDRINC)) != (CSW_SIZE32 | CSW_SADDRINC)) {
    CSW_Val &= ~(CSW_SIZE   | CSW_ADDRINC);
    CSW_Val |=  (CSW_SIZE32 | CSW_SADDRINC);
    status = RDDI_DAP_WriteAP(AP_CSW, CSW_Val);
    if (status) return (status);
  }

  status = RDDI_DAP_WriteAP(AP_TAR, adr);
  if (status) return (status);

  // Multiple Write AP DRW
  status = rddi_DAP_RegWriteRepeat(rddiHandle, rddiDAP_ID, nMany>>2, DAP_AP_REG_DRW, (int *)pB);
  status = RDDI_DAP_CheckStatus(status);
  if (status) return (status);

  status = RDDI_DAP_StickyError();
  if (status) return (status);

  return (0);
}


// RDDI-DAP Verify Data Block (32-bit Elements inside R/W Page Block)
//   adr    : Address
//   pB     : Pointer to Buffer
//   nMany  : Number of bytes
//   return : 0 - Success, else Error Code or -1 on Verify Missmatch
int RDDI_DAP_VerifyBlock (DWORD adr, BYTE *pB, DWORD nMany) {
  int   status;
  int   flag;
  DWORD val;

  if (rddiHandle == NULL) return (RDDI_DAP_ERROR_INTERNAL);

  if (nMany == 0)     return (RDDI_DAP_ERROR_INTERNAL);
  if (nMany & 0x03)   return (RDDI_DAP_ERROR_INTERNAL);
  if (nMany > RWPage) return (RDDI_DAP_ERROR_INTERNAL);

  if (AP_Bank != 0) {
    status = RDDI_DAP_WriteDP(DP_SELECT, AP_Sel | 0);
    if (status) return (status);
    AP_Bank = 0;
  }

  if ((CSW_Val & (CSW_SIZE | CSW_ADDRINC)) != (CSW_SIZE32 | CSW_SADDRINC)) {
    CSW_Val &= ~(CSW_SIZE   | CSW_ADDRINC);
    CSW_Val |=  (CSW_SIZE32 | CSW_SADDRINC);
    status = RDDI_DAP_WriteAP(AP_CSW, CSW_Val);
    if (status) return (status);
  }

  status = RDDI_DAP_WriteAP(AP_TAR, adr);
  if (status) return (status);

  // Configure pushed compare
  status = RDDI_DAP_ReadDP(DP_CTRL_STAT, &val);
  if (status) return (status);
  val &= ~TRNMODE;
  val |=  TRNVERIFY | STICKYCMP;
  status = RDDI_DAP_WriteDP(DP_CTRL_STAT, val);
  if (status) return (status);

  flag = 0;
  status = rddi_DAP_RegWriteRepeat(rddiHandle, rddiDAP_ID, nMany>>2, DAP_AP_REG_DRW, (int *)pB);

  if (status == RDDI_DAP_OPERATION_TIMEOUT) {
    status = RDDI_DAP_Abort(DAPABORT);
    if (status) goto end;
    status = RDDI_DAP_ERROR_MEMORY;
    goto end;
  }
  if (status == RDDI_DAP_DP_STICKY_ERR) {
    // Only for SW (not availalbe for JTAG)
    status = RDDI_DAP_ReadDP(DP_CTRL_STAT, &val);
    if (status) goto end;
    status = RDDI_DAP_Abort(STKERRCLR | STICKYCMP | WDERRCLR);
    if (status) goto end;
    if (val & (STICKYERR | WDATAERR)) {
      status = RDDI_DAP_ERROR_MEMORY;
    }
    if (val & STICKYCMP) {
      flag = -1;  // Verify Mismatch (JTAG)
    }
    goto end;
  }
  if (status == RDDI_SUCCESS) {
    status = RDDI_DAP_ReadDP(DP_CTRL_STAT, &val);
    if (status) goto end;
    if (val & STICKYERR) {
      status = RDDI_DAP_ERROR_MEMORY;
    }
    if (val & STICKYCMP) {
      flag = -1;  // Verify Mismatch (JTAG)
    }
  } else {
    status = RDDI_DAP_ERROR;
  }

end:
  val &= ~TRNMODE;
  if (status) {
    RDDI_DAP_WriteDP(DP_CTRL_STAT, val);
    return (status);
  } else {
    status = RDDI_DAP_WriteDP(DP_CTRL_STAT, val);
    if (status) return (status);
  }

  return (flag);
}


// RDDI-DAP Read ARM Memory
//   nAdr   : Start Address (used to return error addresses)
//   pB     : Pointer to Buffer
//   nMany  : Number of bytes to Read
//   return : 0 - Success, else Error Code
int RDDI_DAP_ReadARMMem (DWORD *nAdr, BYTE *pB, DWORD nMany) {
  int   status;
  DWORD n;

  // Read 8-bit Data (8-bit Aligned)
  if ((*nAdr & 0x01) && nMany) {
    status = RDDI_DAP_ReadD8(*nAdr, pB);
    if (status) return (status);
    status = RDDI_DAP_StickyError();
    if (status) return (status);
    pB    += 1;
    *nAdr += 1;
    nMany -= 1;
  }

  // Read 16-bit Data (16-bit Aligned)
  if ((*nAdr & 0x02) && (nMany >= 2)) {
    status = RDDI_DAP_ReadD16(*nAdr, (WORD *)pB);
    if (status) return (status);
    status = RDDI_DAP_StickyError();
    if (status) return (status);
    pB    += 2;
    *nAdr += 2;
    nMany -= 2;
  }

  // Read Data Block (32-bit Aligned)
  while (nMany >= 4) {
    n = RWPage - (*nAdr & (RWPage - 1));
    if (nMany < n) n = nMany & 0xFFFFFFFC;
    status = RDDI_DAP_ReadBlock(*nAdr, pB, n);
    if (status == RDDI_DAP_ERROR_MEMORY) {
      // Slow Access
      while (n) {  
        status = RDDI_DAP_ReadD32(*nAdr, (DWORD *)pB);
        if (status) return (status);
        pB    += 4;
        *nAdr += 4;
        nMany -= 4;
        n     -= 4;
      }
      status = RDDI_DAP_StickyError();
      if (status) return (status);
      continue;
    }
    if (status) return (status);
    pB    += n;
    *nAdr += n;
    nMany -= n;
  }

  // Read 16-bit Data (16-bit Aligned)
  if (nMany >= 2) {
    status = RDDI_DAP_ReadD16(*nAdr, (WORD *)pB);
    if (status) return (status);
    status = RDDI_DAP_StickyError();
    if (status) return (status);
    pB    += 2;
    *nAdr += 2;
    nMany -= 2;
  }

  // Read 8-bit Data (8-bit Aligned)
  if (nMany) {
    status = RDDI_DAP_ReadD8(*nAdr, pB);
    if (status) return (status);
    status = RDDI_DAP_StickyError();
    if (status) return (status);
    pB    += 1;
    *nAdr += 1;
    nMany -= 1;
  }

  return (0);
}


// RDDI-DAP Write ARM Memory
//   nAdr   : Start Address (used to return error addresses)
//   pB     : Pointer to Buffer
//   nMany  : Number of bytes to Write
//   return value: error status
int RDDI_DAP_WriteARMMem (DWORD *nAdr, BYTE *pB, DWORD nMany) {
  int   status;
  DWORD n;

  // Write 8-bit Data (8-bit Aligned)
  if ((*nAdr & 0x01) && nMany) {
    status = RDDI_DAP_WriteD8(*nAdr, *pB);
    if (status) return (status);
    status = RDDI_DAP_StickyError();
    if (status) return (status);
    pB    += 1;
    *nAdr += 1;
    nMany -= 1;
  }

  // Write 16-bit Data (16-bit Aligned)
  if ((*nAdr & 0x02) && (nMany >= 2)) {
    status = RDDI_DAP_WriteD16(*nAdr, *((WORD *)pB));
    if (status) return (status);
    status = RDDI_DAP_StickyError();
    if (status) return (status);
    pB    += 2;
    *nAdr += 2;
    nMany -= 2;
  }

  // Write Data Block (32-bit Aligned)
  while (nMany >= 4) {
    n = RWPage - (*nAdr & (RWPage - 1));
    if (nMany < n) n = nMany & 0xFFFFFFFC;
    status = RDDI_DAP_WriteBlock(*nAdr, pB, n);
    if (status == RDDI_DAP_ERROR_MEMORY) {
      // Slow Access
      while (n) {  
        status = RDDI_DAP_WriteD32(*nAdr, *((DWORD *)pB));
        if (status) return (status);
        pB    += 4;
        *nAdr += 4;
        nMany -= 4;
        n     -= 4;
      }
      status = RDDI_DAP_StickyError();
      if (status) return (status);
      continue;
    }
    if (status) return (status);
    pB    += n;
    *nAdr += n;
    nMany -= n;
  }

  // Write 16-bit Data (16-bit Aligned)
  if (nMany >= 2) {
    status = RDDI_DAP_WriteD16(*nAdr, *((WORD *)pB));
    if (status) return (status);
    status = RDDI_DAP_StickyError();
    if (status) return (status);
    pB    += 2;
    *nAdr += 2;
    nMany -= 2;
  }

  // Write 8-bit Data (8-bit Aligned)
  if (nMany) {
    status = RDDI_DAP_WriteD8(*nAdr, *pB);
    if (status) return (status);
    status = RDDI_DAP_StickyError();
    if (status) return (status);
    pB    += 1;
    *nAdr += 1;
    nMany -= 1;
  }

  return (0);
}


// RDDI-DAP Verify ARM Memory
//   nAdr   : Start Address (used to return error addresses)
//   pB     : Pointer to Buffer
//   nMany  : Number of bytes to Verify
//   return value: error status
int RDDI_DAP_VerifyARMMem (DWORD *nAdr, BYTE *pB, DWORD nMany) {
  int   status;
  DWORD n;

  // Read 8-bit Data (8-bit Aligned)
  if ((*nAdr & 0x01) && nMany) {
    status = RDDI_DAP_ReadD8(*nAdr, pB);
    if (status) return (status);
    status = RDDI_DAP_StickyError();
    if (status) return (status);
    pB    += 1;
    *nAdr += 1;
    nMany -= 1;
  }

  // Read 16-bit Data (16-bit Aligned)
  if ((*nAdr & 0x02) && (nMany >= 2)) {
    status = RDDI_DAP_ReadD16(*nAdr, (WORD *)pB);
    if (status) return (status);
    status = RDDI_DAP_StickyError();
    if (status) return (status);
    pB    += 2;
    *nAdr += 2;
    nMany -= 2;
  }

  // Pushed Verify Data Block (32-bit Aligned)
  while (nMany >= 4) {
    n = RWPage - (*nAdr & (RWPage - 1));
    if (nMany < n) n = nMany & 0xFFFFFFFC;
    status = RDDI_DAP_VerifyBlock(*nAdr, pB, n);
    if (status == -1) {
      // Verify failed -> Detect 1st missmatch
      status = RDDI_DAP_ReadBlock(*nAdr, pB, n);
      if (status) return (status);
      return (0);
    }
    if (status == RDDI_DAP_ERROR_MEMORY) {
      // Slow Access
      while (n) {  
        status = RDDI_DAP_ReadD32(*nAdr, (DWORD *)pB);
        if (status) return (status);
        pB    += 4;
        *nAdr += 4;
        nMany -= 4;
        n     -= 4;
      }
      status = RDDI_DAP_StickyError();
      if (status) return (status);
      continue;
    }
    if (status) return (status);
    pB    += n;
    *nAdr += n;
    nMany -= n;
  }

  // Read 16-bit Data (16-bit Aligned)
  if (nMany >= 2) {
    status = RDDI_DAP_ReadD16(*nAdr, (WORD *)pB);
    if (status) return (status);
    status = RDDI_DAP_StickyError();
    if (status) return (status);
    pB    += 2;
    *nAdr += 2;
    nMany -= 2;
  }

  // Read 8-bit Data (8-bit Aligned)
  if (nMany) {
    status = RDDI_DAP_ReadD8(*nAdr, pB);
    if (status) return (status);
    status = RDDI_DAP_StickyError();
    if (status) return (status);
    pB    += 1;
    *nAdr += 1;
    nMany -= 1;
  }

  return (0);
}


// RDDI-DAP Get ARM Registers
//   regs   : Pointer to ARM Registers
//   rfpu   : Pointer to FPU Registers
//   mask   : Register Mask
//   return value: error status
int RDDI_DAP_GetARMRegs (RgARMCM *regs, RgARMFPU *rfpu, U64 mask) {
  int   status;
  int   regID  [3*64];
  int   regData[3*64];
  int   i, n, m;
  DWORD val;

  if (rddiHandle == NULL) return (RDDI_DAP_ERROR_INTERNAL);

  if (mask == 0) return (RDDI_DAP_ERROR_INTERNAL);

  // Match Retry = 100
  regID[0] = DAP_REG_MATCH_RETRY;
  regData[0] = 100;

  // Match Mask = 0x00010000
  regID[1] = DAP_REG_MATCH_MASK;
  regData[1] = 0x00010000;

  // TAR = DBG_Addr
  regID[2] = DAP_AP_REG_TAR;
  regData[2] = DBG_Addr;

  // SELECT = AP_Sel | 0x10
  regID[3] = DAP_DP_REG_APSEL;
  regData[3] = AP_Sel | 0x10;

  // R/W DAP Registers
  status = rddi_DAP_RegAccessBlock(rddiHandle, rddiDAP_ID, 4, regID, regData);
  status = RDDI_DAP_CheckStatus(status);
  if (status) return (status);

  AP_Bank = 0x10;

  // Prepare Register Access
  for (i = 0, n = 0; n < 64; n++) {
    if (mask & (1ULL << n)) {
      // Get register selector
      if (n < 21) {
        m = n;                  // Core Registers
      } else if (n >= 32) {
        m = 64 + (n - 32);      // FPU Sn
      } else if (n == 31) {
        m = 33;                 // FPU FPCSR
      } else {
        continue;
      }

      // Select register to read (write to DCRSR)
      regID  [i+0] = DAP_REG_AP_0x4;
      regData[i+0] = m;
      // Read and wait for register ready flag (read DHCSR.16)
      regID  [i+1] = DAP_REG_AP_0x0 | DAP_REG_RnW | DAP_REG_WaitForValue;
      regData[i+1] = 0x00010000;  // Value to Match
      // Read register value (read DCRDR)
      regID  [i+2] = DAP_REG_AP_0x8 | DAP_REG_RnW;

      i += 3;
    }
  }

  // R/W DAP Registers
  status = rddi_DAP_RegAccessBlock(rddiHandle, rddiDAP_ID, i, regID, regData);
  status = RDDI_DAP_CheckStatus(status);
  if (status) return (status);

  status = RDDI_DAP_StickyError();
  if (status) return (status);

  // Store register values
  for (i = 0, n = 0; n < 64; n++) {
    if (mask & (1ULL << n)) {
      val = regData[i+2];
      i  += 3;
      if ((n < 21) && regs) {
        *((DWORD *)regs + n) = val;
      } else if ((n >= 32) && rfpu) {
        *((DWORD *)rfpu + (n - 32)) = val;
      } else if ((n == 31) && rfpu) {
        rfpu->FPSCR = val;
      }
    }
  }

  return (0);
}


// RDDI-DAP Set ARM Registers
//   regs   : Pointer to ARM Registers
//   rfpu   : Pointer to FPU Registers
//   mask   : Register Mask
//   return value: error status
int RDDI_DAP_SetARMRegs (RgARMCM *regs, RgARMFPU *rfpu, U64 mask) {
  int   status;
  int   regID  [3*64];
  int   regData[3*64];
  int   i, n, m;
  DWORD val;

  if (rddiHandle == NULL) return (RDDI_DAP_ERROR_INTERNAL);

  if (mask == 0) return (RDDI_DAP_ERROR_INTERNAL);

  // Match Retry = 100
  regID[0] = DAP_REG_MATCH_RETRY;
  regData[0] = 100;

  // Match Mask = 0x00010000
  regID[1] = DAP_REG_MATCH_MASK;
  regData[1] = 0x00010000;

  // TAR = DBG_Addr
  regID[2] = DAP_AP_REG_TAR;
  regData[2] = DBG_Addr;

  // SELECT = AP_Sel | 0x10
  regID[3] = DAP_DP_REG_APSEL;
  regData[3] = AP_Sel | 0x10;

  // R/W DAP Registers
  status = rddi_DAP_RegAccessBlock(rddiHandle, rddiDAP_ID, 4, regID, regData);
  status = RDDI_DAP_CheckStatus(status);
  if (status) return (status);

  AP_Bank = 0x10;

  // Prepare Register Access
  for (i = 0, n = 0; n < 64; n++) {
    if (mask & (1ULL << n)) {
      // Get register selector and Load register value
      if ((n < 21) && regs) {
        m = 0x00010000 | n;                 // Core Registers
        val = *((DWORD *)regs + n);
      } else if ((n >= 32) && rfpu) {
        m = 0x00010000 | (64 + (n - 32));   // FPU Sn
        val = *((DWORD *)rfpu + (n - 32));
      } else if ((n == 31) && rfpu) {
        m = 0x00010000 | 33;                // FPU FPCSR
        val = rfpu->FPSCR;
      } else {
        continue;
      }

      // Write register value (write to DCRDR)
      regID  [i+0] = DAP_REG_AP_0x8;
      regData[i+0] = val;
      // Select register to write (write to DCRSR)
      regID  [i+1] = DAP_REG_AP_0x4;
      regData[i+1] = 0x00010000 | m;
      // Read and wait for register ready flag (read DHCSR.16)
      regID  [i+2] = DAP_REG_AP_0x0 | DAP_REG_RnW | DAP_REG_WaitForValue;
      regData[i+2] = 0x00010000;  // Value to Match

      i += 3;
    }
  }

  // R/W DAP Registers
  status = rddi_DAP_RegAccessBlock(rddiHandle, rddiDAP_ID, i, regID, regData);
  status = RDDI_DAP_CheckStatus(status);
  if (status) return (status);

  status = RDDI_DAP_StickyError();
  if (status) return (status);

  return (0);
}


// RDDI-DAP Execute System Call
//   regs   : Pointer to ARM Registers
//   return value: error status
int RDDI_DAP_SysCallExec (RgARMCM *regs) {
  int   status;
  int   regID  [3*16];
  int   regData[3*16];
  int   i, n;
  DWORD mask;

  if (rddiHandle == NULL) return (RDDI_DAP_ERROR_INTERNAL);

  // Match Retry = 100
  regID[0] = DAP_REG_MATCH_RETRY;
  regData[0] = 100;

  // Match Mask = 0x00010000
  regID[1] = DAP_REG_MATCH_MASK;
  regData[1] = 0x00010000;

  // TAR = DBG_Addr
  regID[2] = DAP_AP_REG_TAR;
  regData[2] = DBG_Addr;

  // SELECT = AP_Sel | 0x10
  regID[3] = DAP_DP_REG_APSEL;
  regData[3] = AP_Sel | 0x10;

  // R/W DAP Registers
  status = rddi_DAP_RegAccessBlock(rddiHandle, rddiDAP_ID, 4, regID, regData);
  status = RDDI_DAP_CheckStatus(status);
  if (status) return (status);

  AP_Bank = 0x10;

  // Register mask
  mask = (1UL <<  0) |          // R0 (A1)
         (1UL <<  1) |          // R1 (A2)
         (1UL <<  2) |          // R2 (A3)
         (1UL <<  3) |          // R3 (A4)
         (1UL <<  9) |          // R9 (SB)
         (1UL << 13) |          // R13(SP)
         (1UL << 14) |          // R14(LR)
         (1UL << 15) |          // R15(PC)
         (1UL << 16);           // xPSR

  // Prepare Register Access
  for (i = 0, n = 0; n <= 16; n++) {
    if (mask & (1UL << n)) {
      // Write register value (write to DCRDR)
      regID  [i+0] = DAP_REG_AP_0x8;
      regData[i+0] = *((DWORD *)regs + n);
      // Select register to write (write to DCRSR)
      regID  [i+1] = DAP_REG_AP_0x4;
      regData[i+1] = 0x00010000 | n;;
      // Read and wait for register ready flag (read DHCSR.16)
      regID  [i+2] = DAP_REG_AP_0x0 | DAP_REG_RnW | DAP_REG_WaitForValue;
      regData[i+2] = 0x00010000;  // Value to Match
      i += 3;
    }
  }

  // DHCSR = DBGKEY | C_DEBUGEN
  regID[i] = DAP_REG_AP_0x0;
  regData[i] = DBGKEY | C_DEBUGEN;
  i++;

  // DP_CTRL_STAT read
  regID[i] = DAP_REG_DP_0x4 | DAP_REG_RnW;

  // R/W DAP Registers
  status = rddi_DAP_RegAccessBlock(rddiHandle, rddiDAP_ID, i+1, regID, regData);
  status = RDDI_DAP_CheckStatus(status);
  if (status) return (status);

  status = RDDI_DAP_CheckStickyError(regData[i]);
  if (status) return (status);

  return (0);
}


// RDDI-DAP Read System Call Result
//   rval   : Pointer to Result Value
//   return value: error status
int RDDI_DAP_SysCallRes (DWORD *rval) {
  int   status;
  int   regID  [4];
  int   regData[4];

  if (rddiHandle == NULL) return (RDDI_DAP_ERROR_INTERNAL);

  // Match Retry = 100
  regID[0] = DAP_REG_MATCH_RETRY;
  regData[0] = 100;

  // Match Mask = 0x00010000
  regID[1] = DAP_REG_MATCH_MASK;
  regData[1] = 0x00010000;

  // TAR = DBG_Addr
  regID[2] = DAP_AP_REG_TAR;
  regData[2] = DBG_Addr;

  // SELECT = AP_Sel | 0x10
  regID[3] = DAP_DP_REG_APSEL;
  regData[3] = AP_Sel | 0x10;

  // R/W DAP Registers
  status = rddi_DAP_RegAccessBlock(rddiHandle, rddiDAP_ID, 4, regID, regData);
  status = RDDI_DAP_CheckStatus(status);
  if (status) return (status);

  AP_Bank = 0x10;

  // Select register R0 to read (write to DCRSR)
  regID  [0] = DAP_REG_AP_0x4;
  regData[0] = 0;
  // Read and wait for register ready flag (read DHCSR.16)
  regID  [1] = DAP_REG_AP_0x0 | DAP_REG_RnW | DAP_REG_WaitForValue;
  regData[1] = 0x00010000;  // Value to Match
  // Read register value (read DCRDR)
  regID  [2] = DAP_REG_AP_0x8 | DAP_REG_RnW;

  // DP_CTRL_STAT read
  regID  [3] = DAP_REG_DP_0x4 | DAP_REG_RnW;

  // R/W DAP Registers
  status = rddi_DAP_RegAccessBlock(rddiHandle, rddiDAP_ID, 4, regID, regData);
  status = RDDI_DAP_CheckStatus(status);
  if (status) return (status);

  status = RDDI_DAP_CheckStickyError(regData[3]);
  if (status) return (status);

  *rval = regData[2];

  return (0);
}


// RDDI-DAP Perform Target specific commands
//   cmd_str  : Command string
//   resp_str : Response string
//   resp_len : Length of resp_str buffer in bytes
//   return   : 0 - Success, else Error Code
int RDDI_DAP_Target (char *cmd_str, char *resp_str, const int resp_len) {
  int status;

  if (rddiHandle == NULL) return (RDDI_DAP_ERROR_INTERNAL);

  status = rddi_DAP_Target(rddiHandle, cmd_str, resp_str, resp_len);
  if (status) return (RDDI_DAP_ERROR);

  return (0); 
}


// RDDI-DAP Init Debugger
//   return value: error status
int RDDI_DAP_DebugInit (void) {
  int   status;
  DWORD tick;
  DWORD val;
  DWORD id;

  status = RDDI_DAP_WriteDP(DP_SELECT, 0x00000000);
  if (status) return (status);   

  status = RDDI_DAP_WriteDP(DP_CTRL_STAT, CDBGPWRUPREQ|CSYSPWRUPREQ);
  if (status) return (status);

  tick = GetTickCount();
  do {
    status = RDDI_DAP_ReadDP (DP_CTRL_STAT, &val);
    if (status) return (status);
    if ((val & (CDBGPWRUPACK|CSYSPWRUPACK)) == (CDBGPWRUPACK|CSYSPWRUPACK)) break;
  } while ((GetTickCount() - tick) < 1000);

  if ((val & (CDBGPWRUPACK|CSYSPWRUPACK)) != (CDBGPWRUPACK|CSYSPWRUPACK)) {
    return (RDDI_DAP_ERROR_POWER);  // Device could not be powered up
  }

  if (MonConf.Opt & PORT_SW) {
    status = RDDI_DAP_Abort(STKCMPCLR|STKERRCLR|WDERRCLR|ORUNERRCLR);
    if (status) return (status);
  }

  status = RDDI_DAP_WriteDP(DP_CTRL_STAT, CDBGPWRUPREQ|CSYSPWRUPREQ|CDBGRSTREQ);
  if (status) return (status);

  status = RDDI_DAP_WriteDP(DP_CTRL_STAT, CDBGPWRUPREQ|CSYSPWRUPREQ|
                                          ((MonConf.Opt & PORT_SW) ? 0 : STICKYERR|STICKYCMP|STICKYORUN)|
                                          TRNNORMAL|MASKLANE);
  if (status) return (status);

  return (0);
}
