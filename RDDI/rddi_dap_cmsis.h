/*
 * rddi_dap_cmsis.h - Debug header for CMSIS-DAP
 * Copyright (C) 2012 ARM Limited. All rights reserved.
 */

#ifndef RDDI_DAP_CMSIS_H
#define RDDI_DAP_CMSIS_H


// RDDI.h defines the dll exports etc.
#include "rddi.h"


// Identifiers (DAP_Identify)
#define RDDI_CMSIS_DAP_ID_VENDOR        1
#define RDDI_CMSIS_DAP_ID_PRODUCT       2
#define RDDI_CMSIS_DAP_ID_SER_NUM       3
#define RDDI_CMSIS_DAP_ID_FW_VER        4
#define RDDI_CMSIS_DAP_ID_DEVICE_VENDOR 5
#define RDDI_CMSIS_DAP_ID_DEVICE_NAME   6


/*!
 * 
 * Detect DAP interfaces. 
 *
 * Must be present in all levels of implementations
 * 
 * @param[in] handle Implementation specific opaque pointer.  
 * @param[out] noOfIFs will be filled with the number of detected interfaces.
 *
 * @return RDDI_SUCCESS on success, other on fail
 */
RDDI int CMSIS_DAP_Detect(const RDDIHandle handle, int *noOfIFs);

/*!
 * 
 * Identify specified DAP interface.  Must be called after DAP_Detect and provides specified
 * string identifiers.
 *
 * Must be present in all levels of implementations
 * 
 * @param[in] handle Implementation specific opaque pointer.  
 * @param[in] ifNo interface number (0 .. noOfIFs-1)
 * @param[in] idNo identifier number (RDDI_CMSIS_DAP_ID_xxx)
 * @param[out] str a buffer that will be filled with the indentification string
 * @param[in] len the length of the str buffer in bytes 
 *
 * @return RDDI_SUCCESS on success, other on fail
 */
RDDI int CMSIS_DAP_Identify(const RDDIHandle handle, int ifNo, int idNo, char *str, const int len);


/*!
 * Configure the interface with the settings it needs, e.g. "Port=SW;Clock=1000000;SWJ=Y"
 *
 * @param[in] handle Implementation specific opaque pointer.  
 * @param[in] ifNo interface number (0 .. noOfIFs-1)
 * @param[in] str CMSIS_DAP configuration string
 *
 * @return RDDI_SUCCESS on success, other on fail
 */
RDDI int CMSIS_DAP_ConfigureInterface(const RDDIHandle handle, int ifNo, char *str);

/*!
 * Inspect the connected CMSIS_DAP interface to get the no of DAPs accessible via it
 *
 * @param[in] handle Implementation specific opaque pointer.  
 * @param[out] noOfDAPs the number of DAPs accessible on this interface
 *
 * @return RDDI_SUCCESS on success, other on fail
 */
RDDI int CMSIS_DAP_DetectNumberOfDAPs(const RDDIHandle handle, int *noOfDAPs);

/*!
 * Inspect the connected CMSIS_DAP interface to the get the DAP ID for each DAP accessible via this interface
 *
 * @param[in] handle opaque pointer - obtained from DAP_Open call
 * @param[out] DAP_ID_Array will be filled with the IDs for the DAPs in the configure/connected system.  Must be big enough to hold the list.
 * Size should be at leat the number returned by DAP_GetNumberOfDAPs * sizeof(int).
 * @param[in] sizeOfArray size of the DAP_ID_Array parameter in bytes
 *
 * @return RDDI_SUCCESS on success, other on fail
 */
RDDI int CMSIS_DAP_DetectDAPIDList(const RDDIHandle handle, int *DAP_ID_Array, size_t sizeOfArray);

/*!
 * Execute CMSIS_DAP protocol commands on connected interface
 *
 * @param[in] handle opaque pointer - obtained from DAP_Open call
 * @param[in] num number of commands
 * @param[in] request array of command requests
 * @param[in] req_len array of command requests length
 * @param[out] response array of command responses
 * @param[out] resp_len array of command responses length
 *
 * @return RDDI_SUCCESS on success, other on fail
 */
RDDI int CMSIS_DAP_Commands(const RDDIHandle handle, int num, unsigned char **request, int *req_len, unsigned char **response, int *resp_len);


#endif
