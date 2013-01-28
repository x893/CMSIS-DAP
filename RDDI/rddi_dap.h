/*
 * rddi_dap.h - Debug header for RDDI
 * Copyright (C) 2012 ARM Limited. All rights reserved.
 */

/*!
 * @file
 *  
 * @brief  This file describes the functionality of the RDDI_DAP module along with the necessary definitions.  The implementors of this Interface can choose to implement 
 * certain levels only.  Note that if the implementor implements a certain level then they must also implement the lower levels.  Each function in the interface is documented
 * with the level they should be present in.  The level of implementation is obtainable via the DAP_GetSupportedOptimisationLevel function.
 *
 * Timeout information is assumed to be provided in the Configure call and that the implementations will scale appropriately for compound operations
 *
 * The flow of calls is intended to be...
 *      -# RDDI_Open
 *      -# DAP_Configure
 *      -# DAP_GetNumberOfDAPs
 *      -# DAP_GetDAPIDList
 *      -# DAP_Connect
 *      -# DAP_GetSupportedOptimisationLevel
 *      -# DAP* Supported Level calls
 *      -# DAP_Disconnect
 *      -# RDDI_Close
 * 
 * It is acceptable to disconnect, reconfigure, and then connect again without closing the interface.
 *
 * Because DAP_ID is in every [appropriate] call, the implementor of this library is expected to do whatever
 * selection is necessary to route the requested operations to the specified DAP.
 *
 * It is envisaged that implementations of this interface could support all levels even if the hardware debug tool, or debug access mechanism, that is being controlled by this
 * implementation does not.  
 *
 * Level 1 enables the use of sequences that can perform multiple operations with program flow control.  It is envisaged that a macro language 
 * will be supplied to define such sequences in future and that implementations would be able to compile or optimize this language.  At present this macro language 
 * is not defined.
 *
 * This header file is not self-contained.  The following functions from rddi.h must also be implemented to provide a full interface:
 *
 * <ul>
 * <li>RDDI_Open</li>
 * <li>RDDI_Close</li>
 * <li>RDDI_GetLastError</li>
 * </ul>
 *
 * Example - How to read a Memory APs ROM table using only Level0 operations.
 *
\verbatim
    

	int ReadROMTable(RDDIHandle handle, int DAP_ID, int MemAPIndex)
	{
		// Select the AP and Bank 0xF (BASE reg specifies ROM Table address and is 3rd reg in in this bank)
		int APSelectVal = ( MemAPIndex << 24 ) | (0xF << 4);  
		DAP_WriteReg(handle, DAP_ID, DAP_DP_REG_APSEL, APSelectVal);	

		// read the BASE address for the ROM table
		int ROMTableBaseAddr;
		DAP_ReadReg(handle, DAP_ID, DAP_REG_AP_0x8, &ROMTableBaseAddr) // get ROM Table Base address
		if( (ROMTableBaseAddr & 1) == 0)
		{
			printf("No ROM table present for this memory AP");
			return -1;
		}

		if( (ROMTableBaseAddr & 2) == 0)
		{
			printf("ROM table present but it's format is unknown");
			return -1;
		}

		// select Bank 0 in the specified AP because we need access to the CSW, TAR and DRW
		APSelectVal = MemAPIndex << 24;
		DAP_WriteReg(handle, DAP_ID, DAP_DP_REG_APSEL, APSelectVal); 

		// Check CSW for errors
		int CSWvalue;
		DAP_ReadReg(handle, DAP_ID, DAP_REG_AP_0x0, &CSWvalue); 
		if(CheckCSWForErrors(CSWvalue)) 
		{
			printf("AP errors found - can't read ROM table");
			return -1;
		}

		// Write CSW with flags for the AP - Set the TAR to increment after each read so repeated reads can be issued to read the whole ROM table
		DAP_WriteReg(handle, DAP_ID, DAP_REG_AP_0x0, CSWvalue | (1 << 4));


		DAP_WriteReg(handle, DAP_ID, DAP_REG_AP_0x4, baseAddr) // write TAR with address for BASE
		int ROMTable[256];
		for(int i = 0; i < sizeof(ROMTable)/sizeof(int); ++i)
		{ 
			DAP_ReadReg(handle, DAP_ID, DAP_REG_AP_0xC, &value[i]); 
		}

		return 0;
	}

\endverbatim
 *
 */


#ifndef RDDI_DAP_H
#define RDDI_DAP_H

// RDDI.h defines the dll exports etc.
#include "rddi.h"

#ifdef __cplusplus  
extern "C" {
#endif
     
// Valid DAP Register IDs.
/* Register ID definitions
 *
 * <B><I>Format for register ID word.</I></B><BR>
 * IN:<BR><CODE><TABLE>
 * <TR><TD>Bits       <TD>[  31:17 ]<TD>[16 ]<TD>[   15:0    ]</TR>
 * <TR><TD>Description<TD>[Reserved]<TD>[RnW]<TD>[Register ID]</TR></TABLE></CODE><BR>
 *
 * <B>RnW :</B> Read operation = 1, Write operation = 0<BR>
 * <B>Register ID :</B> ID for the register to be accessed.<BR><BR>
 *
 * <B>RnW</B> may be set but is ignored in Read only / Write only functions.
 */

#define DAP_REG_DP_0x0          0   //!< ID to access DP reg 0, address 0x00
#define DAP_REG_DP_0x4          1   //!< ID to access DP reg 1, address 0x04
#define DAP_REG_DP_0x8          2   //!< ID to access DP reg 2, address 0x08
#define DAP_REG_DP_0xC          3   //!< ID to access DP reg 3, address 0x0C
#define DAP_REG_AP_0x0          4   //!< ID to access AP bank, reg 0, offset 0x00
#define DAP_REG_AP_0x4          5   //!< ID to access AP bank, reg 1, offset 0x04
#define DAP_REG_AP_0x8          6   //!< ID to access AP bank, reg 2, offset 0x08
#define DAP_REG_AP_0xC          7   //!< ID to access AP bank, reg 3, offset 0x0C
#define DAP_REG_DP_ABORT        8   //!< ID to write abort register - routed according to DAP architecture
#define DAP_REG_JTAG_IDCODE     9   //!< ID to read JTAG IDCODE - only valid JTAG or SWJ-DP targets.
#define DAP_REG_DAP_IDR        10   //!< ID to read IDR - return to 0x0 for Arch 0, read DP IDR for DAP arch 1 and 2

#define DAP_REG_MATCH_MASK      16  //!< ID to write virtual match mask (used as mask for read operation which waits for value match)
#define DAP_REG_MATCH_RETRY     17  //!< ID to write virtual match retry (used as retry count for read operation which waits for value match)

#define DAP_REG_RnW             0x10000 //!< Flag for read operation in register access call. Ignore flag in Read / Write specific calls
#define DAP_REG_WaitForValue    0x20000 //!< Flag for read operation which waits for value match  

// Useful DP register aliases
#define DAP_DP_REG_CSW          DAP_REG_DP_0x4
#define DAP_DP_REG_APSEL        DAP_REG_DP_0x8
#define DAP_DP_REG_RDBUFF       DAP_REG_DP_0xC

// Useful Mem AP register aliases 
// Note: correct bank must be set for these to work
#define DAP_AP_REG_CSW          DAP_REG_AP_0x0
#define DAP_AP_REG_TAR          DAP_REG_AP_0x4
#define DAP_AP_REG_DRW          DAP_REG_AP_0xC

#define DAP_AP_REG_APIDR        DAP_REG_AP_0xC
#define DAP_AP_REG_ROMADDR      DAP_REG_AP_0x8

#define MAKE_AP_SEL_ADDR(ap_idx, ap_bank_addr) (int)((ap_idx << 24) | ap_bank_addr)

#ifndef DOXYGEN_SHOULD_SKIP_THIS
#define RDDI_DAP_ERR_START                      0x1000
#endif
#define RDDI_DAP_MULTI_REG_CMD_ERR              (RDDI_DAP_ERR_START + 0)   //!< Error executing multiple register access command - at least one access failed.
#define RDDI_DAP_DAPINUSE                       (RDDI_DAP_ERR_START + 1)   //!< The operation on the DAP failed because it was already in use
#define RDDI_DAP_NOT_CONFIGURED                 (RDDI_DAP_ERR_START + 2)   //!< An attempt to use a function was made but the interface has not been configured.
#define RDDI_DAP_NOT_CONNECTED                  (RDDI_DAP_ERR_START + 3)   //!< An attempt to use a function was made but no conenction present
#define RDDI_DAP_CONFIG_CONNECTED               (RDDI_DAP_ERR_START + 4)   //!< An attempt to configure was made, but already connected.
#define RDDI_DAP_CONNECT_INUSE                  (RDDI_DAP_ERR_START + 5)   //!< An attempt to connect was made, but already connected.
#define RDDI_DAP_LEVEL1_NOT_IMPL                (RDDI_DAP_ERR_START + 6)   //!< An attempt to call function in unimplemented level 1
#define RDDI_DAP_LEVEL2_NOT_IMPL                (RDDI_DAP_ERR_START + 7)   //!< An attempt to call function in unimplemented level 2
#define RDDI_DAP_CONFIG_FAILED                  (RDDI_DAP_ERR_START + 8)   //!< Configuration of interface failed.
#define RDDI_DAP_CONFIG_FILE_NOT_FOUND          (RDDI_DAP_ERR_START + 9)   //!< Cannot find configuration file
#define RDDI_DAP_CONFIG_FILE_INVALID            (RDDI_DAP_ERR_START + 10)  //!< Configuration file invalid
#define RDDI_DAP_CONNECT_FAILED                 (RDDI_DAP_ERR_START + 11)  //!< Connection to target failed.
#define RDDI_DAP_CONNECTION_LOST                (RDDI_DAP_ERR_START + 12)  //!< Connection to target lost.
#define RDDI_DAP_OPERATION_FAILED               (RDDI_DAP_ERR_START + 13)  //!< requested DAP operation failed.
#define RDDI_DAP_OPERATION_TIMEOUT              (RDDI_DAP_ERR_START + 14)  //!< DAP operation timed out.
#define RDDI_DAP_DP_STICKY_ERR                  (RDDI_DAP_ERR_START + 15)  //!< DP sticky error flagged
#define RDDI_DAP_NCONT_STALL                    (RDDI_DAP_ERR_START + 16)  //!< NCont failed to clear
#define RDDI_DAP_BAD_REGISTER_ID                (RDDI_DAP_ERR_START + 17)  //!< Register ID not valid
#define RDDI_DAP_REGISTER_NOT_SUPPORTED         (RDDI_DAP_ERR_START + 18)  //!< Register ID not supported for this operation or architecture
#define RDDI_DAP_NO_MATCH                       (RDDI_DAP_ERR_START + 19)  //!< Match operation result no match after re-tries 
#define RDDI_DAP_MEMAP_UNSUPPORTED              (RDDI_DAP_ERR_START + 20)  //!< Memory access operation on unsupported / unknown AP type.
#define RDDI_TARGET_COMMAND_RESPONSE_TOO_SMALL  (RDDI_DAP_ERR_START + 21)  //!< Target command returned and error - see response string for details.
#define RDDI_TARGET_CMD_ERROR                   (RDDI_DAP_ERR_START + 22)  //!< Response buffer to small for target command.

#define RDDI_COMMUNICATION_ERROR                (RDDI_DAP_ERR_START + 23)  //!< Communication Error PC <-> Target



/*! Major version of this interface */
#define RDDI_DAP_MAJOR_VERSION 0
/*! Minor version of this interface */
#define RDDI_DAP_MINOR_VERSION 2

/*!
 * Structure for returning implementation and connection details to the client
 */
typedef struct _RDDI_DAP_CONN_DETAILS
{
    char implementorName[160];        //!< String that should be filled with the implementors name, e.g. ARM Ltd
    char connectionDescription[160];  //!< String that should be filled in with details of the connection, e.g. ARM LCP USB 
} RDDI_DAP_CONN_DETAILS;


#ifdef __cplusplus  
}
#endif


/*!
 * Get DAP interface version
 *
 * Must be present in all levels of implementations
 *
 * @param[in] handle Implementation specific opaque pointer.  
 * @param[out] version Version of the DAP interface.  Format is: bits[31:24] = major version, bits [23:16] = minor version, bits [15:0] = build.
 *
 * @return RDDI_SUCCESS on success, other on fail
 */

RDDI int DAP_GetInterfaceVersion(const RDDIHandle handle, int *version);
/*!
 * 
 * Configure the DAP interface - it is expected that the specific implementation will have
 * implementation specific configuration to do.  This function should be used to pass a file
 * containing any configuration information that is required.  Configuration is applied at Connection 
 * time and is torn down at disconnection time.  Configure calls are allowed while no 
 * active connection exists. 
 *
 * Must be present in all levels of implementations
 * 
 * @param[in] handle Implementation specific opaque pointer.  
 * @param[in] configFileName Name of the configuration file that configures the interface
 *
 * @return RDDI_SUCCESS on success, other on fail
 */
RDDI int DAP_Configure(const RDDIHandle handle, const char* configFileName);

/*!
 *  
 * Connect to the specified DAP in the system.
 *
 * Must be present in all levels of implementations
 *
 * This is the 1st point that any target communications should take place.  
 *
 * @param[in] handle opaque pointer - obtained from RDDI_Open call
 * @param[out] pConnDetails Pointer to a structure that will be filled with data about the configuration and the thing we have connected to.
 * 
 * @return RDDI_SUCCESS on success, other on fail 
 */
RDDI int DAP_Connect(const RDDIHandle handle, RDDI_DAP_CONN_DETAILS* pConnDetails);

/*!
 * Get the number of DAPs that are accessible via this interface.  Must be called after a successful DAP_Configure and DAP_Connect call.
 *
 * Must be present in all levels of implementations
 *
 * This function does not communicate with the target
 *
 * @param[in] handle opaque pointer - obtained from RDDI_Open call
 * @param[out] noOfDAPs will be filled with the number of DAPs connectable via the active connection.
 *
 * @return RDDI_SUCCESS on success, other on fail 
 */
RDDI int DAP_GetNumberOfDAPs(const RDDIHandle handle, int* noOfDAPs);


/*!
 * Get a list of DAP IDs for the configured/connected system.   Must be called after a successful DAP_Configure and DAP_Connect call.
 *
 * Must be present in all levels of implementations
 *
 * This function does not communicate with the target
 *
 * @param[in] handle opaque pointer - obtained from DAP_Open call
 * @param[out] DAP_ID_Array will be filled with the IDs for the DAPs in the configure/connected system.  Must be big enough to hold the list.
 * Size should be at leat the number returned by DAP_GetNumberOfDAPs * sizeof(int).
 * @param[in] sizeOfArray size of the DAP_ID_Array parameter in bytes
 *
 * @return RDDI_SUCCESS on success, other on fail 
 */
RDDI int DAP_GetDAPIDList(const RDDIHandle handle, int* DAP_ID_Array, size_t sizeOfArray);

/*!
 * Disconnect from this configuration.  After successful disconnect Configure can be called.
 *
 * Must be present in all levels of implementations
 *
 * @param[in] handle opaque pointer - obtained from DAP_Open call
 * 
 * @return RDDI_SUCCESS on success, other on fail 
 */
RDDI int DAP_Disconnect(const RDDIHandle handle);


/*!
 *
 * Get the level of optimisation supported by this interface.
 *
 * Must be present in all levels of implementations
 *
 * Only 2 levels of optimisation are possible (0 and 1) depending on the implementation.  Lower levels must be implemented: that 
 * is if level 1 is implemented then so is level 0.
 *
 * Level 0 offers no optimisations and implements only primitives.  
 * Level 1 supports macros 
 *
 * @param[in] handle opaque pointer - obtained from DAP_Open call
 * @param[out] level the level of optimisation supported by this interface
 *
 * @return An error code: RDDI_SUCCESS on success.
 */
RDDI int DAP_GetSupportedOptimisationLevel(const RDDIHandle handle, int* level);

/*! 
 * Read a value from a DAP DP register, or currently selected AP register.
 *
 * @param[in] handle opaque pointer - obtained from RDDI_Open call
 * @param[in] DAP_ID a unique identifier for the DAP to perform this operation on.  
 * This may be an index in a JTAG scan chain or a target selection ID for Multi-drop Serial Wire systems.  This identifier will have meaning to the implementation.
 * @param[in] regID ID for the specified register
 * @param[in] value The value to  to the specified register
 * 
 *
 * @return An error code.  RDDI_SUCCESS if successful, other code for errors.
 *                         RDDI_DAP_BAD_REGISTER_ID if invalid register ID.
 *                         RDDI_DAP_REGISTER_NOT_SUPPORTED if register not supported for this operation or DAP architecture.
 *      
 */
RDDI int DAP_ReadReg(const RDDIHandle handle, const int DAP_ID, const int regID, int *value);

/*! 
 * Write a value to a DAP DP register, or currently selected AP register.
 *
 * @param[in] handle opaque pointer - obtained from RDDI_Open call
 * @param[in] DAP_ID a unique identifier for the DAP to perform this operation on.  
 * This may be an index in a JTAG scan chain or a target selection ID for Multi-drop Serial Wire systems.  This identifier will have meaning to the implementation.
 * @param[in] regID ID for the specified register
 * @param[in] value The value read from the specified register
 * 
 *
 * @return An error code.  RDDI_SUCCESS if successful, other code for errors.
 *                         RDDI_DAP_BAD_REGISTER_ID if invalid register ID.
 *                         RDDI_DAP_REGISTER_NOT_SUPPORTED if register not supported for this operation or DAP architecture.
 *      
 */
RDDI int DAP_WriteReg(const RDDIHandle handle, const int DAP_ID, const int regID, const int value);
                                             

/*!
 * Access a number of registers, either reading or writing in sequence.<BR><BR>
 * <B><I>Format for register ID word.</I></B><BR>
 * IN:<BR><CODE><TABLE>
 * <TR><TD>Bits       <TD>[  31:17 ]<TD>[16 ]<TD>[   15:0    ]</TR>
 * <TR><TD>Description<TD>[Reserved]<TD>[RnW]<TD>[Register ID]</TR></TABLE></CODE><BR>
 *
 * <B>RnW :</B> Read operation = 1, Write operation = 0<BR>
 * <B>Register ID :</B> ID for the register to be accessed.<BR><BR>
 *
 * The function will return success if all accesses succeed, or a failure code if one or more fail. The 
 * caller can examine individual response codes to determine which register accesses failed.
 *
 * Must be present in all implementations
 *
 * @param[in] handle opaque pointer - obtained from DAP_Open call
 * @param[in] DAP_ID a unique identifier for the DAP to perform this operation on.  This may be an index in a JTAG scan chain or a target selection ID for Multi-drop Serial Wire systems.  This identifier will have meaning to the implementation.
 * @param[in] numRegs Number of registers being accessed.
 * @param[in] *regIDArray array of register ID and operation details.
 * @param[in,out] *dataArray Register data - either data for write, or space for read value.
 *
 * @return RDDI_SUCCESS - all registers accessed, RDDIDAPERR_MULTI_REG_CMD - at least one access failed.
 *
 * @note <B><I> Response Codes and Errors for Individual Accesses </I></B>
 *
 *  Error responses should not ordinarily be returned for errors accessing DP/AP registers that the system would flag - 
 *  i.e. those that would result in a sticky error, or error response in the AP Control Status word. The adaptor code
 *  implementation could be made configurable to allow error responses in these cases if desired.
 *
 *  Errors in attempting to write none writeable locations that would not generate an error in a real system;
 *  e.g. write to a locked register / read of none existent register - could also be implementation / configuration dependent. 
 *
 *  A sequence of accesses represents a sequence of debug operations, that will continue irrespective
 *  of errors occurring within a single debug operation. The implementation should allow the transactions to continue, and report further errors if
 *  these occur. 
 * 
 *  <I>Responses For Individual Accesses.</I>
 *  <BR><CODE><TABLE>
 *  <TR><TD>RDDIDAPERR_BAD_REGISTER_ID <TD>  Register ID not recognised </TR>
 *  <TR><TD>RDDIDAPERR_REGISTER_NOT_SUPPORTED<TD> Register ID not supported for the current architecture / configurations </TR>
 *  </TABLE></CODE><BR>
 * 
 */
RDDI int DAP_RegAccessBlock(const RDDIHandle handle, const int DAP_ID, const int numRegs, 
    const int *regIDArray, int *dataArray);


/*!
 * Write a block of registers. 
 *
 * Must be present in all implementations
 *
 * @param[in] handle opaque pointer - obtained from DAP_Open call
 * @param[in] DAP_ID a unique identifier for the DAP to perform this operation on.  
 * This may be an index in a JTAG scan chain or a target selection ID for Multi-drop Serial Wire systems.  This identifier will have meaning to the implementation.
 * @param[in] numRegs Number of registers being accessed.
 * @param[in] *regIDArray array of register IDs.
 * @param[in] *dataArray Location for data to write.
 *
 * @return DAPResponseCode  : RDDI_SUCCESS - all registers accessed, RDDIDAPERR_MULTI_REG_CMD - at least one access failed.
 */
RDDI int DAP_RegWriteBlock(const RDDIHandle handle, const int DAP_ID, const int numRegs, 
    const int *regIDArray, const int *dataArray);

/*!
 * Read a block of registers. 
 *
 * Must be present in all implementations
 *
 * @param[in] handle opaque pointer - obtained from DAP_Open call
 * @param[in] DAP_ID a unique identifier for the DAP to perform this operation on.  
 * This may be an index in a JTAG scan chain or a target selection ID for Multi-drop Serial Wire systems.  This identifier will have meaning to the implementation.
 * @param[in] numRegs Number of registers being accessed.
 * @param[in] *regIDArray array of register IDs.
 * @param[out] *dataArray Location for data read.
 *
 * @return DAPResponseCode  : RDDI_SUCCESS - all registers accessed, RDDIDAPERR_MULTI_REG_CMD - at least one access failed.
 */
RDDI int DAP_RegReadBlock(const RDDIHandle handle, const int DAP_ID, const int numRegs, 
    const int *regIDArray, int *dataArray);


/*!
 * Repeated write to a single register. 
 * Only one register ID is passed into the function. The response code in the ID is set to the first failure if one occurs.  Subsequent writes are abandoned if a failure occurs.
 *
 * Must be present in all implementations
 *
 * @param[in] handle opaque pointer - implementation specific
 * @param[in] DAP_ID a unique identifier for the DAP to perform this operation on.  This may be an index in a JTAG scan chain or a target selection ID for Multi-drop Serial Wire systems.  This identifier will have meaning to the implementation.
 * @param[in] numRepeats Number of writes to register.
 * @param[in] regID single register ID
 * @param[in] *dataArray Data to write to register.
 *
 * @return An Error Code. RDDI_SUCCESS - all registers accessed, RDDIDAPERR_MULTI_REG_CMD - at least one access failed.  RDDIDAPERR_BUFFER_TOO_SMALL - Not enough buffer supplied for count specified
 */
RDDI int DAP_RegWriteRepeat(const RDDIHandle handle, const int DAP_ID, const int numRepeats, 
    const int regID, const int *dataArray);

/*!
 * Repeated read of a single register. 
 * The response code array is filled for each read of the specified register
 *
 * Must be present in all implementations
 *
 * @param[in] handle opaque pointer - obtained from DAP_Open call
 * @param[in] DAP_ID a unique identifier for the DAP to perform this operation on.  This may be an index in a JTAG scan chain or a target selection ID for Multi-drop Serial Wire systems.  
 * This identifier will have meaning to the implementation.
 * @param[in] numRepeats Number of registers being accessed.
 * @param[in] regID single register ID.
 * @param[out] *dataArray Location for data read.
 *
 * @return int  : RDDI_SUCCESS - all registers accessed, RDDIDAPERR_MULTI_REG_CMD - at least one access failed, RDDIDAPERR_BUFFER_TOO_SMALL - Not enough buffer supplied for count specified
 */
RDDI int DAP_RegReadRepeat(const RDDIHandle handle, const int DAP_ID, const int numRepeats, 
    const int regID, int *dataArray);


/*!
 * Repeated read of a signal register testing for a required value.  
 * Will return immediately if an error occurs.
 * Will return early if expected value is read before numRepeats is expired.
 * 
 * Must be present in all implementations
 *
 * @param[in] handle opaque pointer - obtained from DAP_Open call
 * @param[in] DAP_ID a unique identifier for the DAP to perform this operation on.  
 * This may be an index in a JTAG scan chain or a target selection ID for Multi-drop Serial Wire systems.  This identifier will have meaning to the implementation.
 * @param[in] numRepeats Number of time to read the register and test its value.  
 * @param[in] regID single register ID.
 * @param[in] mask mask to be applied to value read from the register before testing against value
 * @param[in] requiredValue value to be tested for
 *
 * @return int  : RDDI_SUCCESS - register value matched, RDDIDAPERR_NO_MATCH if numRepeats expired without a match, other errors possible for specific fail cases.
 * 
 */
RDDI int DAP_RegReadWaitForValue(const RDDIHandle handle, const int DAP_ID, const int numRepeats, 
    const int regID, const int* mask, const int* requiredValue);

/*!
 * Perform target specific commands.
 *
 * This function is used to send target specific commands to the implementation.  
 *
 * At present this is only defined for controlling target signals that are not part of the DAP, but may be part of the protocol connection, 
 * and may need to be controlled by the debugger.
 *
 * e.g. nSRST is part of the JTAG connector that controls system reset if connected on the board.
 *
 * It is implementation and target dependent which, if any signals are available and supported.

 * These signals are strictly those which are not architecturally controlled or defined by Coresight.
 * i.e. the DBGPWRUPREQ and DBGPWRUPACK signals which are part of the DP Control-Status register do not 
 * appear here, and cannot be controlled in this function.
 * 
 * ARM debuggers will expect to control and monitor system reset if present, and monitor the powered state
 * of the board or individual cores.
 * 
 * ARM currently defines the following command set for correct operation with ARM debuggers.
 *
 * Signal names:-
 * sys_reset : overall system reset
 * sys_power : overall system power
 * core_reset_n : core n reset state
 * core_power_n : core n power state 
 * core_reset_all : all core reset states - response will list each individual state.
 * core_power_all : all core power states - response will list each individual state.
 *
 *
 * The following commands can operate on the signals:-
 * 
 * <ul>
 * <li>on  : set the signal on or asserted. (for a active low signal this means in the active low state).</li>
 * <li>off : set the signal off or de-asserted.</li>
 * <li>read : return the current state of the signal.</li>
 * <li>signal_avail : special command will return a list of available signals for this target and implementation.</li>
 * </ul>
 * 
 * The following responses can be obtained for signals
 * 
 * <ul>
 * <li>on  : the signal is on or asserted.</li>
 * <li>off : the signal is off or de-asserted.</li>
 * <li>latch_on : the signal has been asserted since the last read of the signal. This may be combined with current state.</li>
 * <li>unknown : the signal name not recognised.</li>
 * <li>err  : the signal cannot be driven to the requested value (e.g. power may be monitored, but it may not be controlled).</li>
 * </ul>
 * 
 * When signal commands are used to drive the signal, the signal will also be read to provide the response.
 * Signals and commands / response states are joined with the '.' character.
 * Multiple command / responses separated using semi-colons.
 *
 * e.g. Simple M class system:-
 * 
 *  Command "signal_avail"
 *  Response "sys_reset;sys_power"
 *
 *  Command "sys_reset.on;sys_power.read"
 *  Response "sys_reset.on;sys_power.on"
 *
 *  Command "sys_reset.off"
 *  Response "sys_reset.off"
 *
 * It is the responsibility of the implementation to perform the appropriate actions on the target.  For example a reset signal could actually require 
 * a number of different operations to perform a full system reset. The implementation must handle this and maintain the illusion that a system reset is controlled by the signal.
 * The Configure call could be used configure what actually needs to take place to reset the system.
 * 
 * @param[in] handle opaque pointer - obtained from DAP_Open call
 * @param[in] request_str a NULL terminated string containing the command(s) to perform
 * @param[out] resp_str a buffer that will be filled with the response string for the requested command(s)
 * @param[in] resp_len the length of the resp_str buffer in bytes 
 * 
 * @return int  : RDDI_SUCCESS on success, other on fail.  An RDDI_BUFFER_OVERFLOW error will occur if the size of the resp_str buffer (as specified by resp_len) is not sufficient to hold the response
 * codes.
 */
RDDI int DAP_Target(const RDDIHandle handle, const char *request_str, char *resp_str, const int resp_len);


/*!
 * Define an implementation specific sequence
 *
 * Implementation of this function is optional and indicated by level 1 support.
 *
 * @param[in] handle opaque pointer - obtained from DAP_Open call
 * @param[in] seqID an implementation specific ID that identifies a custom sequence
 * @param[in] seqDef pointer to a block of data that is the input for this sequence
 * 
 * @return int  : RDDI_SUCCESS on success, other on fail
 */
RDDI int DAP_DefineSequence(const RDDIHandle handle, const int seqID, void* seqDef);

/*!
 * Run an implementation specific sequence
 *
 * Implementation of this function is optional and indicated by level 1 support.
 *
 * @param[in] handle : opaque pointer - obtained from DAP_Open call
 * @param[in] seqID : an implementation specific ID that identifies a custom sequence
 * @param[in] seqInData : pointer to a block of data that is the input for this sequence
 * @param[out] seqOutData : pointer to a block of data that will be filled with the output for this sequence
 * 
 * @return int  : RDDI_SUCCESS on success, other on fail
 */
RDDI int DAP_RunSequence(const RDDIHandle handle, const int seqID, void* seqInData, void* seqOutData);


#endif
