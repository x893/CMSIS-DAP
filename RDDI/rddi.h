/*
 * rddi.h - Core header for RDDI
 * Copyright (C) 2007 ARM Limited. All rights reserved.
 */

#ifndef RDDI_H
#define RDDI_H

/**  \file

     \brief This file describes the core functions of the RDDI module along with 
     the necessary definitions.
*/

#ifdef WIN32
  #ifdef _RDDI_EXPORT
    #ifdef __cplusplus
      #define RDDI extern "C" __declspec(dllexport)
    #else
      #define RDDI __declspec(dllexport)
    #endif
  #elif defined(_RDDI_LOADLIBRARY)
    #define RDDILL_GetProcAddress(dll, func)(func *)GetProcAddress(dll, #func);
    #ifdef __cplusplus
      #define RDDI extern "C" typedef
    #else
      #define RDDI typedef
    #endif
  #else
    #ifdef __cplusplus
      #define RDDI extern "C" __declspec(dllimport)
    #else
      #define RDDI __declspec(dllimport)
    #endif
  #endif
#else
  #ifdef __cplusplus
    #define RDDI extern "C" 
  #else
    #define RDDI
  #endif 
#endif
/**
    \def RDDI 
    \brief Macro used to define the DLL entry point function calls
*/

/**
   \brief unsigned 8 bit integer.
 */
#ifndef uint8
typedef unsigned char  uint8;
#endif
/**
   \brief unsigned 16 bit integer.
 */
#ifndef uint16
typedef unsigned short uint16;
#endif
/**
   \brief unsigned 32 bit integer.
 */
#ifndef uint32
typedef unsigned int  uint32;
#endif
/*
 * Error codes
 *
 * Every time an error code is added here, we need a descriptive string in 
 * rddi_error.cpp
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS
/* 0x00XX - debug errors */
#define RDDI_SUCCESS                            0x0000
#define RDDI_BADARG                             0x0001
#define RDDI_INVHANDLE                          0x0002
#define RDDI_FAILED                             0x0003
#define RDDI_TOOMANYCONNECTIONS                 0x0004
#define RDDI_BUFFER_OVERFLOW                    0x0005
/*      REMOVED                                 0x0006 */
#define RDDI_BROWSE_FAILED                      0x000C
#define RDDI_INTERNAL_ERROR                     0x000D
#define RDDI_PARSE_FAILED                       0x000E
#define RDDI_OUTOFMEM                           0x0043
#define RDDI_EXTERNAL_CMD_FAILED                0x006E
#define RDDI_LICENSE_FAILED                     0x0070
#define RDDI_LICENSE_WARNING                    0x8070
#define RDDI_MAX_ERR                            0xFFFF
#endif


/*
 * Log levels
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS
#define RDDI_LOGLEVEL_FATAL   0
#define RDDI_LOGLEVEL_ERROR   1
#define RDDI_LOGLEVEL_WARNING 2
#define RDDI_LOGLEVEL_INFO    3
#define RDDI_LOGLEVEL_DEBUG   4
#define RDDI_LOGLEVEL_TRACE   5
#endif


/**
   \brief Declares the type used to hold an RDDI Handle. See RDDI_Open().
 */
typedef int RDDIHandle;

/**
   \brief Special RDDIHandle value used to indicate an invalid handle
 */
#define INV_HANDLE 0xFFFF

#if DOXYGEN_ACTIVE
/**
    \brief Open a connection to the RDDI interface

    \param[out] pHandle Pointer to a handle. This function returns a unique 
    handle which identifies the current session and must be used in each 
    following call to the RDDI.
    
    \param[in] pDetails Supplementary connection details. If this is not needed
    it may be passed as NULL.

    <B>Example usage</B> - open a connection to RDDI, obtain the version 
    information, cause an error and obtain the error details, and then close 
    the connection.

    \dontinclude init.c
    \skip pDetails
    \until }
    \until }
    \until }
    \until }
    \skip RDDI_Close
    \until }
 */
#endif
RDDI int RDDI_Open(
		RDDIHandle *pHandle,
		const void *pDetails);

#if DOXYGEN_ACTIVE
/**
    \brief Close a connection to the RDDI interface

    \param[in] handle The handle identifying the current RDDI session. This 
    handle is invalidated by this method and may not be used after the call to 
    RDDI_Close() is completed.

    For example usage see RDDI_Open().
*/
#endif
RDDI int RDDI_Close(
		RDDIHandle handle);

#if DOXYGEN_ACTIVE
/**
    \brief Get the RDDI version and build number

    \param[in] handle Session handle previously created by RDDI_Open()

    \param[out] pMajor The RDDI major version number

    \param[out] pMinor The RDDI minor version number

    \param[out] pBuild The RDDI build number

    \param[out] pDescription A client-supplied buffer to receive a version 
    descriptive string

    \param[in] descriptionLen The length of the client-supplied descriptive 
    buffer in characters. Note that this includes the terminating NULL, and so 
    is one more than the number of characters that can be returned from RDDI. 
    If necessary, the descriptive string is truncated to fit in the buffer and 
    the error RDDI_BUFFER_OVERFLOW is returned. The suggested minimum size for 
    this buffer is 250 characters.

    If the description is not needed then pDescription may be passed as NULL or 
    descriptionLen as zero, and pDescription will be ignored without error.

    For example usage see RDDI_Open().
 */
#endif
RDDI int RDDI_GetVersion(
		RDDIHandle handle,
		int *pMajor,
		int *pMinor,
		int *pBuild,
		char *pDescription, size_t descriptionLen);

#if DOXYGEN_ACTIVE
/** 
    \brief Get the last error returned by RDDI. 

    This is the only RDDI method that does not update the last recorded error, 
    even if it fails

    \param[out] pError If non-null this receives the last error produced by RDDI 
    for the current session handle

    \param[out] pDetails Client supplied buffer to receive a textual description 
    of the error complete with any available details.

    \param[in] detailsLen The length of the client-supplied descriptive buffer 
    in characters. Note that this includes the terminating NULL, and so is one 
    more than the number of characters that can be returned from RDDI. If 
    necessary, the descriptive string is truncated to fit in the buffer and the 
    error RDDI_BUFFER_OVERFLOW is returned. The suggested minimum size for this 
    buffer is 250 characters.

    The textual description can be considered optional: if pDetails is passed as NULL 
    or detailsLen is zero, pDetails does not receive the textual description and 
    RDDI_SUCCESS is returned. However if pError is also NULL, RDDI_BADARG is returned.

    For example usage see RDDI_Open().
 */
#endif
RDDI int RDDI_GetLastError(
		int *pError,
		char *pDetails, size_t detailsLen);


#ifdef __cplusplus
extern "C" {
#endif
    
#if DOXYGEN_ACTIVE
/**
    \brief RDDI log callback function definition
    
    The RDDI logging mechanism expects a pointer to a function that matches
    this definition.  This function is then called whenever a log event
    occurs.  Log messages have a level of importance as defined below.

    RDDI_LOGLEVEL_FATAL   = 0
    RDDI_LOGLEVEL_ERROR   = 1
    RDDI_LOGLEVEL_WARNING = 2
    RDDI_LOGLEVEL_INFO    = 3
    RDDI_LOGLEVEL_DEBUG   = 4
    RDDI_LOGLEVEL_TRACE   = 5

    \param [in] context Client specific context value

    \param [in] msg Null terminated string containing a plain text log message

    \param [in] logLevel The level of the log message
 */
#endif
typedef void (*RDDILogCallback)(void* context, const char* const msg, const int logLevel);
#ifdef __cplusplus
}
#endif

#if DOXYGEN_ACTIVE
/** 
    \brief Register a log callback with RDDI 

    The RDDI logging mechanism uses the supplied function pointer to
    inform the client software that a logging event has occurred.

    \param[in] handle  Session handle previously created by RDDI_Open()

    \param[in] pfn Pointer to a function of type RDDILogCallback that will be
    called when a log event occurs

    \param[in] context Client supplied context variable that will be returned
    in the RDDILogCallback function. If context is unimportant (e.g. only one 
    client handling callbacks) then this may be passed as NULL.

    \param[in] maxLogLevel Maximum level of log messages to produce e.g.
    RDDI_LOGLEVEL_WARNING will produce FATAL and WARNING messages, but not
    INFO, DEBUG and TRACE
 */
#endif
RDDI void RDDI_SetLogCallback(
		RDDIHandle handle,
		RDDILogCallback pfn,
		void* context,
		int maxLogLevel);

#if DOXYGEN_ACTIVE
/** 
\mainpage DOCTAG_name DOCTAG_version vDOCTAG_buildnum

\section rddioverview RDDI Overview

RDDI provides a C interface for connecting to debug agents e.g. ARM &reg; RealView &reg; ICE unit.
The interface is implemented as a Windows DLL or Linux shared library.

\section rddiinstall RDDI Installation

RDDI is delivered as a .zip (or a .tar.gz) file.

The intention is that you can take the .zip file and unzip its contents into a
directory which is convenient for your development environment.

Within the .zip file you will see the following directory tree structure: 

\verbatim 
rddi 
rddi\docs
rddi\include
rddi\rvi\lib
rddi\rvi\examples
rddi\rvi\redistributables
\endverbatim

Other editions of RDDI have the same directory structure, but will contain 
extra files as required.

\subsection rddidocs Documentation 

Documentation for RDDI is provided in HTML files generated by Doxygen. Just 
open the \c rddi\docs\html\index.html file in your browser.

\subsection rddiinclude include files

These are the C include files you will need to include when creating your own 
programs which use RDDI.

\subsection rddilib lib files

These are the library files you will need to link against when creating your 
own programs which use RDDI (Windows only).

\subsection rddiexamples Example files

These are code examples which show you how to use RDDI. Microsoft VS2003 and 
VS2005 project files are provided along with a linux makefile to show you how 
to build the example program. Note that the example program is written assuming
the target system contains a single ARM920T &tm; processor. Before the example program can 
be run you will need to create a suitable RealView ICE configuration file (.rvc).
This can be done by using the RVConfig program to edit a .rvc file and save it
into the same location as the rddi-device-example program executable. The example
program is then run by providing it with 2 parameters:
- The configuration file to use
- The name of the log file to generate (e.g. rddi.log)

e.g. rddi-device-example rvi.rvc rddi.log

Note that for linux systems you will need to set the library search path to
include the executable directory. This can be done by setting the 
LD_LIBRARY_PATH environment variable to point to the current directory.

e.g. export LD_LIBRARY_PATH=./

You are free to modify and use/copy any of the example code within your own 
applications.

\subsection rddiredistributables Redistributable files

The redistributables directory contains all the files you are permitted to 
redistribute with your RDDI based applications. Some of these files are 
licensed to you by ARM &reg; and some use 3rd party licenses. 

The following files are licensed by ARM:

\if DOCS_V2_0
    \if RDDI_DEBUG
\verbatim
librddi-debug-cadi.so.2
rddi-debug-cadi_2.dll
librddi-debug-coredump.so.2
rddi-debug-coredump_2.dll
librddi-debug-gdb.so.2
rddi-debug-gdb_2.dll
librddi-debug-rvi.so.2
rddi-debug-rvi_2.dll
libcadiclient.so
libxml_stl_gcc_3_4.so.1
xml_stl2.dll
\endverbatim
    \endif
\endif

The filenames starting with \c boost_ or \c libboost_ are licensed under the 
Boost license. 
The XERCES XML parser (\c libxerces-c.so,\c xerces-c_2_5_0.dll) are licensed under 
the Apache 1.1 License.

\subsection rddiinstallationnotes Installation Notes

\li The memory interface at the DLL has been designed to be clean - ie 
ownership of memory does not pass between the DLL and client. This means that 
release versions of the DLL can be used with either debug or release 
build of the client application without fear of causing heap corruption.

    <b>Further Topics</b>
    \li \ref DummyFunctionIndex
    \li \ref rddifunctions 
    \li \ref rddireturns
    \li \ref StandardRegs
    \li \ref CP15Regs
    \li \ref ArmCSDPTemplate
*/

/** \page rddifunctions RDDI Functional Overview

The RDDI interface is split into several broad functional area, each of which is 
described in a different header file:

\li rddi.h - functions prefixed RDDI_ and their supporting definitions, for 
connecting to and disconnecting from RDDI itself and obtaining version and
error information.

\li rddi_browse.h - functions prefixed with Browse_ and their supporting 
definitions, for browsing the network and USB for available RVI units.

\li rddi_configinfo.h - functions prefixed with ConfigInfo_ and their 
supporting definitions, for managing and parsing RVI configuration files 
(typically created by RVConfig).

\li rddi_jtag.h - functions prefixed with JTAG and their supporting 
definitions, for low-level JTAG control of devices via ARM RealView hardware.

\li rddi_event.h - definitions for the async. events which can be delivered
to an RDDI client application.

\li rddi_debug.h - functions prefixed with Debug_ and their supporting 
definitions, for run control and debugging of devices via ARM RealView ICE 
hardware.

\li rddi_dap.h - functions prefixed with DAP_ and their supporting 
definitions, for run control and debugging of devices via ARM's Debug Access Port.

Each of the functional groups is described in some detail elsewhere in 
this documentation - follow the links above. 
*/

/** \page rddireturns RDDI Error Returns

All RDDI functions return an integer. A zero return value 
(RDDI_SUCCESS) indicates that the function succeeded, any other return value 
indicates that errors occurred. The currently available return values are as 
follows:
 
\li <B>RDDI_SUCCESS</B> Success - no errors.
\li <B>RDDI_BADARG</B> The client passed a bad argument to an RDDI function - 
typically a NULL pointer, a zero-length buffer or a zero device ID. RDDI fails 
the function immediately with this code and does not carry out any other 
actions.
\li <B>RDDI_INVHANDLE</B> The session handle passed to the RDDI function was 
invalid - typically the handle has already been invalidated with RDDI_Close() 
or has not been obtained using RDDI_Open().
\li <B>RDDI_FAILED</B> Non-specific failure.
\li <B>RDDI_TOOMANYCONNECTIONS</B> Only returned by RDDI_Open().  
Some RDDI implementations support a maximum of 10 concurrent connections, 
although others support many more. If all available connections are in use
the RDDI_Open() will fail with this error.
\li <B>RDDI_BUFFER_OVERFLOW</B> Many RDDI functions pass back large quantities 
of information, and to facilitate this the client passes pre-allocated buffers 
into the RDDI function. This ensures that the memory interface is kept clean 
and avoids the need for ownership of memory to pass from RDDI to the client. 
However, if the supplied buffer is not big enough to accommodate the information
from RDDI then this error code is returned. Some functions will leave the 
buffer empty and some will truncate their information to fit the buffer - check 
the individual function documentation for details.
\li <B>RDDI_BROWSE_FAILED</B> Browse_Start() or Browse_Search() failed, 
normally because of network connectivity issues.
\li <B>RDDI_INTERNAL_ERROR</B> An unspecified internal error occurred.
\li <B>RDDI_PARSE_FAILED</B> ConfigInfo_OpenFile() failed to parse the 
specified configuration file. The client should make sure that the
file is not corrupt and that its contents are correct for the RDDI 
implementation being used.
*/



/** 
\page StandardRegs Standard Register Map
\section StdRegOview Overview
For ARM &reg; architecture devices, the register map is divided into 32 blocks. 
Block zero is further subdivided into 8 blocks of 256 registers each:

<table>
<tr bgcolor="#CCCCCCCC"><th>Block</th> <th>Subblock</th><th>Range</th><th>Description</th></tr>
<tr bgcolor="#EEEEEEEE"><td>0</td><td>0</td><td>0x0000-0x00FF</td><td>Core registers</td></tr>
<tr bgcolor="#EEEEEEEE"><td> </td><td>1</td><td>0x0100-0x01FF</td><td>EICE registers</td></tr>
<tr bgcolor="#EEEEEEEE"><td> </td><td>2</td><td>0x0200-0x02FF</td><td>ETM registers</td></tr>
<tr bgcolor="#EEEEEEEE"><td> </td><td>3</td><td>0x0300-0x03FF</td><td>FPA registers</td></tr>
<tr bgcolor="#EEEEEEEE"><td> </td><td>4-7</td><td>0x0400-0x07FF</td><td>Reserved for future standard blocks</td></tr>
<tr bgcolor="#EEEEEEEE"><td>1</td><td> </td><td>0x0800-0x0FFF</td><td>MCRR/MRRC extension space (CP 0-7)</td></tr>
<tr bgcolor="#EEEEEEEE"><td>2</td><td> </td><td>0x1000-0x17FF</td><td>MCRR/MRRC extension space (CP 8-15)</td></tr>
<tr bgcolor="#EEEEEEEE"><td>3</td><td> </td><td>0x1800-0x1FFF</td><td>Coresight: Any AP, registers 0x00-0x1C</td></tr>
<tr bgcolor="#EEEEEEEE"><td>4</td><td>0</td><td>0x2000-0x20FF</td><td>Coresight: DPACC + APACC registers</td></tr>
<tr bgcolor="#EEEEEEEE"><td> </td><td>1-3</td><td>0x2100-0x23FF</td><td>Unused/Reserverd for other coresight regs.</td></tr>
<tr bgcolor="#EEEEEEEE"><td> </td><td>4-7</td><td>0x2400-0x27FF</td><td>Coresight: Component Registers</td></tr>
<tr bgcolor="#EEEEEEEE"><td>5</td><td> </td><td>0x2800-0x2FFF</td><td>CP15 v7 arch. register space (see \ref RegB4 )</td></tr>
<tr bgcolor="#EEEEEEEE"><td>6-7</td><td> </td><td>0x3000-0x3FFF</td><td>Reserved for future functionality</td></tr>
<tr bgcolor="#EEEEEEEE"><td>8-12</td><td> </td><td>0x4000-0x77FF</td><td>May be used for vehicle-defined registers</td></tr>
<tr bgcolor="#EEEEEEEE"><td>13</td><td> </td><td>0x7000-0x73FF</td><td>TrustZone CP15 secure world registers (see \ref cp15tz )</td></tr>
<tr bgcolor="#EEEEEEEE"><td>14</td><td> </td><td>0x7400-0x77FF</td><td>TrustZone CP15 non-secure world registers (see \ref cp15tz )</td></tr>
<tr bgcolor="#EEEEEEEE"><td>15</td><td> </td><td>0x7800-0x7FFF</td><td>CP15 extended register space (see \ref cp15_15 )</td></tr>
<tr bgcolor="#EEEEEEEE"><td>16</td><td> </td><td>0x8000-0x87FF</td><td>CP0 registers</td></tr>
<tr bgcolor="#EEEEEEEE"><td>17</td><td> </td><td>0x8800-0x8FFF</td><td>CP1 registers</td></tr>
<tr bgcolor="#EEEEEEEE"><td align="center">...</td><td> </td><td align="center">...</td><td><B>...</B></td></tr>
<tr bgcolor="#EEEEEEEE"><td>31</td><td> </td><td>0xF800-0xFFFF</td><td>CP15 registers</td></tr>
</table>

\section RegCaps Register Capabilities
The vehicle supplies a list of supported register numbers (for each device) in 
the RVM_CAP_REGMAP capability block. For ARM &reg; architecture devices, the following
rules should be followed when deciding which registers to list in the capability 
block:
<ul>
<li>Where a block has a fixed number of specifically-defined registers, the 
vehicle must ONLY claim support for the range of registers that are currently 
defined. For example in the core register block (block 0, sub block 0), a 
vehicle would typically claim support for register numbers 0-43, (0-46 for 
TrustZone &reg; processors). It must NOT claim support for the entire block (registers
 0-255), because that includes registers that are currently undefined.
<li>Where a block has a general register encoding scheme, the vehicle should 
claim support for all the registers covered by the encoding scheme, even if 
that covers registers which do not currently exist in any devices. This is 
desirable because the vehicle will typically be able to successfully access any 
registers that fit into the encoding scheme, including new registers that are 
added in the future. So for example on a device that includes a CP15, the 
vehicle can claim support for registers 0xF800-0xFFFF and 0x7C80-0x7FFF, even 
though this range includes only a few valid registers for that device.
<li>Where a block has one or more encoding schemes that do not completely fill 
the block, the vehicle must not claim support in areas of the block which are 
currently undefined. For example, on a device that has a VFP, the vehicle would 
typically claim support for registers 0xD000-0xD01F, 0xD700-0xD71F and 
0xD800-0xD81F. Claiming support for any other registers in the range 
0xD000-0xDFFF would be invalid.
</ul>

\section RegBlock0 Block 0 (Core Registers)

Block zero contains the core registers, along with registers for standard blocks
that are not implemented using the general coprocessor interface.

\subsection B0CoreRegs Core Registers
Core registers are encoded in Sub-Block 0 of this block.
<table>
<tr><td>15</td> <td>14</td><td>13</td><td>12</td><td>11</td> <td>10</td><td>9</td><td>8</td> <td>7</td><td>6</td> <td>5</td><td>4</td><td>3</td><td>2</td><td>1</td><td>0</td></tr>
<tr><td bgcolor="#CCCCCCCC">0</td><td bgcolor="#CCCCCCCC" align="center" colspan="5">Block 0</td> <td bgcolor="#CCCCCCCC" align="center" colspan="3">Sub block 0</td> <td bgcolor="#CCCCCCCC" align="center" colspan="2">0</td> <td bgcolor="#EEEEEEEE" align="center" colspan="6">Core Register ID</td></tr> 
</table>
<EM>Core Register encoding</EM>

The table below defines the valid register IDs for this block.
<table>
<tr bgcolor="#CCCCCCCC"><th>Register ID</th><th>Name</th><th>Description</th></tr>
<tr bgcolor="#EEEEEEEE"><td>0</td><td>CPSR</td><td>Current status register</td></tr>
<tr bgcolor="#EEEEEEEE"><td>1-15</td><td>R0-R14</td><td>Current register bank</td></tr>
<tr bgcolor="#EEEEEEEE"><td>16</td><td>PC (R15)</td><td>Program counter</td></tr>
<tr bgcolor="#EEEEEEEE"><td>17</td><td>SPSR_FIQ</td><td>FIQ mode saved status register</td></tr>
<tr bgcolor="#EEEEEEEE"><td>18-24</td><td>R8_FIQ-R14_FIQ</td><td>FIQ mode register bank</td></tr>
<tr bgcolor="#EEEEEEEE"><td>25</td><td>SPSR_SVC</td><td>SVC mode saved status register</td></tr>
<tr bgcolor="#EEEEEEEE"><td>26, 27</td><td>R13_SVC,R14_SVC</td><td>SVC mode register bank</td></tr>
<tr bgcolor="#EEEEEEEE"><td>28</td><td>SPSR_ABT</td><td>ABT mode saved status register</td></tr>
<tr bgcolor="#EEEEEEEE"><td>29, 30</td><td>R13_ABT,R14_ABT</td><td>ABT mode register bank</td></tr>
<tr bgcolor="#EEEEEEEE"><td>31</td><td>SPSR_IRQ</td><td>IRQ mode saved status register</td></tr>
<tr bgcolor="#EEEEEEEE"><td>32, 33</td><td>R13_IRQ,R14_IRQ</td><td>IRQ mode register bank</td></tr>
<tr bgcolor="#EEEEEEEE"><td>34</td><td>SPSR_UND</td><td>UND mode saved status register</td></tr>
<tr bgcolor="#EEEEEEEE"><td>35, 36</td><td>R13_UND,R14_UND</td><td>UND mode register bank</td></tr>
<tr bgcolor="#EEEEEEEE"><td>37-43</td><td>R8_USR-R14_USR</td><td>USR mode register bank</td></tr>
<tr bgcolor="#EEEEEEEE"><td>44</td><td>SPSR_MON</td><td>MON mode saved status register</td></tr>
<tr bgcolor="#EEEEEEEE"><td>45, 46</td><td>R13_MON,R14_MON</td><td>MON mode register bank</td></tr>
</table>

\subsection M3CoreRegs Cortex-M Core Registers
The Cortex-M family of cores use a different set of register IDs to other ARM cores.

<table>
<tr bgcolor="#CCCCCCCC"><th>Register ID</th><th>Name</th><th>Description</th></tr>
<tr bgcolor="#EEEEEEEE"><td>0-12</td><td>R0-R12</td><td>Current register bank</td></tr>
<tr bgcolor="#EEEEEEEE"><td>13</td><td>SP (R13)</td><td>Stack Pointer Register</td></tr>
<tr bgcolor="#EEEEEEEE"><td>14</td><td>LR (R14)</td><td>Link Register</td></tr>
<tr bgcolor="#EEEEEEEE"><td>15</td><td>PC (R15)</td><td>Program Counter</td></tr>
<tr bgcolor="#EEEEEEEE"><td>16</td><td>xPSR</td><td>special-purpose Program Status Register</td></tr>
<tr bgcolor="#EEEEEEEE"><td>17</td><td>SP_main</td><td>Main Stack Register</td></tr>
<tr bgcolor="#EEEEEEEE"><td>18</td><td>SP_process</td><td>Process Stack Register</td></tr>
<tr bgcolor="#EEEEEEEE"><td>21</td><td>PRIMASK</td><td>Exception Mask Register</td></tr>
<tr bgcolor="#EEEEEEEE"><td>22</td><td>BASEPRI</td><td>Base Priority Mask Register</td></tr>
<tr bgcolor="#EEEEEEEE"><td>22</td><td>FAULTMASK</td><td>Fault Mask Register</td></tr>
<tr bgcolor="#EEEEEEEE"><td>23</td><td>CONTROL</td><td>special-purpose Control Register</td></tr>

\subsection RegEmICE Embedded ICE Registers
For ARM &reg; processors that contain an EmbeddedICE &reg; macrocell (ARM7 and ARM9), the 
registers of the EICE unit can be exposed in this block. This allows a client 
direct access to the debug hardware for the purposes of programming watchpoint 
units, transferring DCC data and so on.

<table>
<tr><td>15</td><td>14</td><td>13</td><td>12</td><td>11</td> <td>10</td><td>9</td><td>8</td> <td>7</td><td>6</td><td>5</td> <td>4</td><td>3</td><td>2</td><td>1</td><td>0</td></tr>
<tr><td bgcolor="#CCCCCCCC" align="center" colspan="5">Block 0</td> <td bgcolor="#CCCCCCCC" align="center" colspan="3">Sub block 1</td> <td bgcolor="#CCCCCCCC" align="center" colspan="3">0</td> <td bgcolor="#EEEEEEEE" align="center" colspan="5">Address</td></tr> 
</table>
<EM>Embedded ICE register encoding</EM>

Bits 0-4 of the register number are used directly as the address bits in scan 
chain 2 to select particular EICE registers. The register numbers are therefore:

<table>
<tr bgcolor="#CCCCCCCC"><th>Number</th><th>Register</th></tr>
<tr bgcolor="#EEEEEEEE"><td>0x0100</td><td>Debug control</td></tr>
<tr bgcolor="#EEEEEEEE"><td>0x0101</td><td>Debug status</td></tr>
<tr bgcolor="#EEEEEEEE"><td>0x0102</td><td>Vector catch control (not all EICE versions)</td></tr>
<tr bgcolor="#EEEEEEEE"><td>0x0104</td><td>Debug comms control</td></tr>
<tr bgcolor="#EEEEEEEE"><td>0x0105</td><td>Debug comms data</td></tr>
<tr bgcolor="#EEEEEEEE"><td>0x0108</td><td>Watchpoint 0 address value</td></tr>
<tr bgcolor="#EEEEEEEE"><td>0x0109</td><td>Watchpoint 0 address mask</td></tr>
<tr bgcolor="#EEEEEEEE"><td>0x010A</td><td>Watchpoint 0 data value</td></tr>
<tr bgcolor="#EEEEEEEE"><td>0x010B</td><td>Watchpoint 0 data mask</td></tr>
<tr bgcolor="#EEEEEEEE"><td>0x010C</td><td>Watchpoint 0 control value</td></tr>
<tr bgcolor="#EEEEEEEE"><td>0x010D</td><td>Watchpoint 0 control mask</td></tr>
<tr bgcolor="#EEEEEEEE"><td>0x0110</td><td>Watchpoint 1 address value</td></tr>
<tr bgcolor="#EEEEEEEE"><td>0x0111</td><td>Watchpoint 1 address mask</td></tr>
<tr bgcolor="#EEEEEEEE"><td>0x0112</td><td>Watchpoint 1 data value</td></tr>
<tr bgcolor="#EEEEEEEE"><td>0x0113</td><td>Watchpoint 1 data mask</td></tr>
<tr bgcolor="#EEEEEEEE"><td>0x0114</td><td>Watchpoint 1 control value</td></tr>
<tr bgcolor="#EEEEEEEE"><td>0x0115</td><td>Watchpoint 1 control mask</td></tr>
</table>

\subsection RegETM ETM Registers
For ARM processors that contain an Embedded Trace Macrocell &tm;, the registers of 
the ETM &tm; can be exposed in this block. Bits 0-6 of the register number are
used directly as the address bits in scan chain 6 to select particular ETM 
registers. Refer to the ETM technical reference manual for a complete list of 
the available registers for the various ETM architectures.

<table>
<tr><td>15</td><td>14</td><td>13</td><td>12</td><td>11</td> <td>10</td><td>9</td><td>8</td> <td>7</td><td>6</td><td>5</td> <td>4</td><td>3</td><td>2</td><td>1</td><td>0</td></tr>
<tr><td bgcolor="#CCCCCCCC" align="center" colspan="5">Block 0</td> <td bgcolor="#CCCCCCCC" align="center" colspan="3">Sub block 2</td> <td bgcolor="#CCCCCCCC" align="center" colspan="1">0</td> <td bgcolor="#EEEEEEEE" align="center" colspan="7">Address</td></tr> 
</table>
<EM>ETM register encoding</EM>

Here are some examples of ETM register numbers:
<table>
<tr bgcolor="#CCCCCCCC"><th>Number</th><th>Register</th></tr>
<tr bgcolor="#EEEEEEEE"><td>0x0200</td><td>ETM control register</td></tr>
<tr bgcolor="#EEEEEEEE"><td>0x0201</td><td>ETM conmfiguration code register</td></tr>
<tr bgcolor="#EEEEEEEE"><td>0x0210</td><td>Address comparator 0 value register</td></tr>
</table>

\subsection RegFPA FPA Registers
For processors that have an attached FPA (floating point accelerator) 
coprocessor, its registers should be exposed in sub block 3 of block 0. The FPA 
is accessed through CP1 and CP2, but it does not use MRC instructions to 
transfer floating point registers and is a legacy device, so fits better into a
'special case' sub block.

For the eight floating point registers in the FPA, the following register 
encoding is used. The registers are only available in the internal 80-bit, 3 
word extended precision format, and therefore take up 3 sequential register 
numbers (with one undefined register number between each):

<table>
<tr><td>15</td><td>14</td><td>13</td><td>12</td><td>11</td> <td>10</td><td>9</td><td>8</td> <td>7</td><td>6</td> <td>5</td> <td>4</td><td>3</td><td>2</td> <td>1</td><td>0</td></tr>
<tr><td bgcolor="#CCCCCCCC" align="center" colspan="5">Block 0</td> <td bgcolor="#CCCCCCCC" align="center" colspan="3">Sub block 3</td> <td bgcolor="#CCCCCCCC" align="center" colspan="2">0</td> <td bgcolor="#CCCCCCCC" align="center" colspan="1">0</td><td bgcolor="#EEEEEEEE" align="center" colspan="3">Fn</td><td bgcolor="#EEEEEEEE" align="center" colspan="2">Word</td></tr> 
</table>
<EM>FPA floating point register encoding</EM>

So for example, F4 is available in these register numbers:
<table>
<tr bgcolor="#CCCCCCCC"><th>Number</th><th>Register</th></tr>
<tr bgcolor="#EEEEEEEE"><td>0x0310</td><td>F4 first word (sign & exponent)</td></tr>
<tr bgcolor="#EEEEEEEE"><td>0x0311</td><td>F4 second word (fraction most significant part)</td></tr>
<tr bgcolor="#EEEEEEEE"><td>0x0312</td><td>F4 third word (fraction least significant part)</td></tr>
</table>

For the FPCR and FPSR, the following encoding is used:
<table>
<tr><td>15</td><td>14</td><td>13</td><td>12</td><td>11</td> <td>10</td><td>9</td><td>8</td> <td>7</td><td>6</td> <td>5</td> <td>4</td><td>3</td> <td>2</td><td>1</td><td>0</td></tr>
<tr><td bgcolor="#CCCCCCCC" align="center" colspan="5">Block 0</td> <td bgcolor="#CCCCCCCC" align="center" colspan="3">Sub block 3</td> <td bgcolor="#CCCCCCCC" align="center" colspan="2">0</td> <td bgcolor="#CCCCCCCC" align="center" colspan="1">1</td><td bgcolor="#CCCCCCCC" align="center" colspan="2">0</td><td bgcolor="#EEEEEEEE" align="center" colspan="3">Op1</td></tr> 
</table>
<EM>FPA system register encoding</EM>

So the register numbers are:
<table>
<tr bgcolor="#CCCCCCCC"><th>Number</th><th>Register</th></tr>
<tr bgcolor="#EEEEEEEE"><td>0x032A</td><td>FPCR</td></tr>
<tr bgcolor="#EEEEEEEE"><td>0x0329</td><td>FPSR</td></tr>

\section RegB1to2 Blocks 1 to 2 (MCRR / MRRC Registers)
Blocks 1 and 2 hold the registers relating to the MCRR and MRRC instructions 
that are available in ARM &reg; architecture v5TE and above. These instructions 
transfer 64-bits of data, and have the format:
\verbatim
MCRR <coproc>, <opcode>, <Rd>, <Rn>, <CRm>
MRRC <coproc>, <opcode>, <Rd>, <Rn>, <CRm>
\endverbatim
The registers encoded in these blocks have a size of 64-bits. The upper 32-bits
of the register correspond to Rn in the instruction. The lower 32-bits of the 
register correspond to Rd in the instruction.

The register number encodes the co-processor number (split across the two 
blocks), the opcode, and the value of CRm.

Co-processors 0 to 7 use block 1:
<table>
<tr><td>15</td><td>14</td><td>13</td><td>12</td><td>11</td> <td>10</td><td>9</td><td>8</td> <td>7</td><td>6</td><td>5</td><td>4</td> <td>3</td><td>2</td><td>1</td><td>0</td></tr>
<tr> <td bgcolor="#CCCCCCCC">0</td><td bgcolor="#CCCCCCCC">0</td><td bgcolor="#CCCCCCCC">0</td><td bgcolor="#CCCCCCCC">0</td><td bgcolor="#CCCCCCCC">1</td><td bgcolor="#EEEEEEEE" align="center" colspan="3">Cp num</td><td bgcolor="#EEEEEEEE" align="center" colspan="4">Opcode</td><td bgcolor="#EEEEEEEE" align="center" colspan="4">CRm</td> </tr> 
</table>

Co-processors 8 to 15 use block 2:
<table>
<tr><td>15</td><td>14</td><td>13</td><td>12</td><td>11</td> <td>10</td><td>9</td><td>8</td> <td>7</td><td>6</td><td>5</td><td>4</td> <td>3</td><td>2</td><td>1</td><td>0</td></tr>
<tr> <td bgcolor="#CCCCCCCC">0</td><td bgcolor="#CCCCCCCC">0</td><td bgcolor="#CCCCCCCC">0</td><td bgcolor="#CCCCCCCC">1</td><td bgcolor="#CCCCCCCC">0</td><td bgcolor="#EEEEEEEE" align="center" colspan="3">Cp num - 8</td><td bgcolor="#EEEEEEEE" align="center" colspan="4">Op</td><td bgcolor="#EEEEEEEE" align="center" colspan="4">CRm</td> </tr> 
</table>

\section RegB3and4 Blocks 3 and 4 (sub-block 0) - Coresight Registers
<table>
<tr><td>15</td><td>14</td><td>13</td><td>12</td><td>11</td> <td>10</td><td>9</td><td>8</td><td>7</td><td>6</td><td>5</td><td>4</td><td>3</td> <td>2</td><td>1</td><td>0</td></tr>
<tr> <td bgcolor="#CCCCCCCC" align="center" colspan="5">Block 3</td><td bgcolor="#EEEEEEEE" align="center" colspan="8">AP Selector</td><td bgcolor="#EEEEEEEE" align="center" colspan="3">AP Reg Addr</td></tr>
</table>

Block 3 allows access to the first eight 32 bit registers of any of the 256 AP 
blocks on the DAP. The AP selector is the MS byte of the AP address. The AP 
register address bits are the 3 most significant bits of the 5 bit address range
0x00 to 0x1C. This allows the first 8 registers of the selected AP to be 
accessed, allowing a single call to select the correct AP address and access the
register. These will tend to be the most frequently used in the case of AHB-AP, 
APB_AP and JTAG-AP blocks.

<table>
<tr><td>15</td><td>14</td><td>13</td><td>12</td><td>11</td> <td>10</td><td>9</td><td>8</td> <td>7</td><td>6</td><td>5</td><td>4</td> <td>3</td><td>2</td><td>1</td><td>0</td></tr>
<tr> <td bgcolor="#CCCCCCCC" align="center" colspan="5">Block 4</td> <td bgcolor="#CCCCCCCC" align="center" colspan="3">Sub block 0</td> <td bgcolor="#EEEEEEEE">DP</td> <td bgcolor="#CCCCCCCC">0</td> <td bgcolor="#CCCCCCCC">0</td> <td bgcolor="#CCCCCCCC">0</td> <td bgcolor="#CCCCCCCC">0</td> <td bgcolor="#CCCCCCCC">0</td> <td bgcolor="#EEEEEEEE" align="center" colspan="2">Address</td> </tr>
</table>

Block 4, sub block 0 allows access of any DPACC or APACC coresight &tm; register. 
Both DPACC and APACC have a block of four 32 bit registers selected by a 2 bit 
address, corresponding to the "Address" field above. The DP bit
selects DPACC (DP = 1) or APACC (DP = 0). Combinations of DPACC and APACC 
register accesses can access any register on any AP on the DAP.


\section RegB4 Block 4 (sub-blocks 4-7) - Coresight Component Registers

Coresight &tm; components expose a set of 32 bit memory mapped registers in a 4k 
byte block (or multiple 4k blocks). This gives a total of 1024 registers in a 
block. Block 4, sub blocks 4-7 cover this register range.

<table>
<tr><td>15</td><td>14</td><td>13</td><td>12</td><td>11</td> <td>10</td><td>9</td><td>8</td> <td>7</td><td>6</td><td>5</td><td>4</td> <td>3</td><td>2</td><td>1</td><td>0</td></tr>
<tr> <td bgcolor="#CCCCCCCC" align="center" colspan="5">Block 4</td> <td bgcolor="#CCCCCCCC" align="center" colspan="1">1</td> <td bgcolor="#EEEEEEEE" align="center" colspan="10">Coresight Component Register Number</td> </tr> 
</table>
The register number is the memory mapped offset divided by 4. These register IDs
will be recognised when connected to coresight component implementations (such 
as ETB &tm;, CTI etc.), rather than a processor based coresight implementations
(A8, R4), 
which will respond normally to processor based register IDs. Writes to none existent 
registers in the range will be ignored. Reads will always return zero.
The above encoding is sufficient for coresight components with a single 4k block
of registers. For components with multiple 4k blocks, then a register ID is 
provided to select the 4k block to be used. [0x2004]. (This encoding is an 
unused value from the sub-block 0 coresight range.) 

<table>
<tr><td>15</td><td>14</td><td>13</td><td>12</td><td>11</td> <td>10</td><td>9</td><td>8</td> <td>7</td><td>6</td><td>5</td><td>4</td> <td>3</td><td>2</td><td>1</td><td>0</td></tr>
<tr> <td bgcolor="#CCCCCCCC" align="center" colspan="5">Block 4</td> <td bgcolor="#CCCCCCCC" align="center" colspan="3">Sub block 0</td> <td bgcolor="#CCCCCCCC">0</td> <td bgcolor="#CCCCCCCC">0</td> <td bgcolor="#CCCCCCCC">0</td> <td bgcolor="#CCCCCCCC">0</td> <td bgcolor="#CCCCCCCC">0</td> <td bgcolor="#CCCCCCCC">1</td> <td bgcolor="#CCCCCCCC">0</td> <td bgcolor="#CCCCCCCC">0</td> </tr>
</table>

Once a 4k block is selected all subsequent accesses using the encoding above 
will be to the selected block. Block selects will be a zero based index, thus 
the block select will be zero for a component with only one 4k block. By 
default, in components with more than one 4k block, the block select will be 
set to the block containing the coresight management registers - the coresight 
architecture specification defines this has having the highest address in 
memory.  This will therefore be at the highest possible block select number for
the component, hence reading the block select value before any writes occur will
obtain the maximum permitted value. The maximum number of 4k blocks for a 
component is 16384, giving indexes from 0 to 16383.

\section RegB5 CP15 Architecture v7 extended register space

The v7 architecture processors, such as Cortex &tm; A8, have CP15 registers which do not 
fit into the encoding scheme outlined below. The encodings defined in sections 
\ref cp15 and \ref cp15_15 assume that either Op1 is 0, or CRn is 15 
respectively. The encoding in this block is for the cases in v7 architecture 
which do not fit into these two categories. 

<table>
<tr><td>15</td><td>14</td><td>13</td><td>12</td><td>11</td> <td>10</td><td>9</td><td>8</td><td>7</td><td>6</td><td>5</td><td>4</td><td>3</td> <td>2</td><td>1</td><td>0</td></tr>
<tr> <td bgcolor="#CCCCCCCC" align="center" colspan="5">Block 5</td><td bgcolor="#EEEEEEEE" align="center" colspan="3">Op2</td> <td bgcolor="#EEEEEEEE" align="center" colspan="2">Op1-1</td> <td bgcolor="#EEEEEEEE" align="center" colspan="2">CRm</td> <td bgcolor="#EEEEEEEE" align="center" colspan="4">CRn</td> </tr>
</table>

This encoding supports any Op2 and CRn values. The "Op1-1" field represents Op1
values of 1 to 4, the field value being the Op1 value minus 1. CRm is limited 
to the values 0 to 3. These encodings cover the CP15 registers used by the 
Cortex A8 processor, which do not fit into the other encoding schemes, and should be 
used only for such register encodings.

\section RegB6 Blocks 6 and 7 (Expansion Area)
Blocks 6 to 7 in the ARM register map are reserved for future standard blocks. 
Vehicles may not expose registers in this range. Blocks 8 to 14 are reserved 
for private use by the vehicle and may be used to expose non-standard registers
supported on a particular target that do not have a defined location elsewhere 
in the register map.

*/



/** 
    \page CP15Regs Coprocessor Registers

    The entire upper half of the register map is given over to the registers of
    the 16 ARM &reg; coprocessors. The block for each coprocessor contains 2048 
    registers, or 11 selection bits. 
    An MRC instruction allows 14 bits for selecting a particular coprocessor 
    register, not counting additional variations for MRC2 and MRRC instructions 
    in architecture v5.  No current coprocessors use all of the possible 
    variations, but since use of the various encoding fields varies widely 
    between coprocessors, it is not possible to use one consistent encoding for 
    the RV-MSG register numbers. The following sections describe the encoding 
    chosen for each of the current coprocessors.

\section cp15 CP15
    CP15 is the system control coprocessor, providing access to caches, the MMU,
    the protection unit, tightly coupled memory and other features of some ARM 
    variants. The ARM Architecture Reference Manual states that the Op1 field 
    of CP15 register transfer instructions should always be zero. This leaves 
    11 bits that can vary between registers, which fits neatly into the register
    map, allowing vehicles to construct the necessary MRC instructions directly 
    from the register number:
<br>

<table>
<tr><td>15</td> <td>14</td><td>13</td><td>12</td><td>11</td><td>10</td><td>9</td><td>8</td><td>7</td><td>6</td><td>5</td><td>4</td> <td>3</td><td>2</td><td>1</td><td>0</td></tr>
<tr><td bgcolor="#CCCCCCCC">1</td><td bgcolor="#CCCCCCCC" align="center" colspan="4">CP15</td> <td bgcolor="#EEEEEEEE" align="center" colspan="3">Op2</td> <td bgcolor="#EEEEEEEE" align="center" colspan="4">CRm</td> <td bgcolor="#EEEEEEEE" align="center" colspan="4">CRn</td></tr>
</table>
<EM>CP15 Register encoding</EM>

<br>
This encoding is capable of describing almost all of the CP15 registers present
in current ARM variants. Refer to the technical reference manuals for 
descriptions of the particular registers and the values of CRn, CRm and Op2 
used to access them. 
<br>
Here are some examples:
<table>
<tr bgcolor="#CCCCCCCC"><th>Number</th><th>Register</th></tr>
<tr bgcolor="#EEEEEEEE"><td>0xF800</td><td>Main ID register (any processor with a CP15)</td></tr>
<tr bgcolor="#EEEEEEEE"><td>0xF802</td><td>TTBR, the translation table base register, for processors 
with a full MMU (920T, 926EJ-S etc)</td></tr>
<tr bgcolor="#EEEEEEEE"><td>0xF967</td><td>D cache line invalidate - write the modified virtual address 
of the line to invalidate. For processors with a separate D cache (940T, 946E-S, 
1020T etc)</td></tr>
</table>

\subsection cp15tz CP15 TrustZone extension
There are requirements to access some of the CP15 registers for the Other World. 
This is why some CP15 registers have separate IDs in Block 13 and 14. These IDs 
refer to the same registers but in the required world or the banked versions of 
the registers in the required world.

<table>
<tr><td>15</td> <td>14</td><td>13</td><td>12</td><td>11</td><td>10</td><td>9</td><td>8</td><td>7</td><td>6</td><td>5</td><td>4</td> <td>3</td><td>2</td><td>1</td><td>0</td><td align="center"><B>World</B></td></tr>
<tr><td bgcolor="#CCCCCCCC">0</td><td bgcolor="#CCCCCCCC" align="center" colspan="4">13</td> <td bgcolor="#EEEEEEEE" align="center" colspan="3">Op2</td> <td bgcolor="#EEEEEEEE" align="center" colspan="4">CRm</td> <td bgcolor="#EEEEEEEE" align="center" colspan="4">CRn</td> <td bgcolor="#EEEEEEEE" align="center" colspan="1"><B>Secure</B></td> </tr> 
<tr><td bgcolor="#CCCCCCCC">0</td><td bgcolor="#CCCCCCCC" align="center" colspan="4">14</td> <td bgcolor="#EEEEEEEE" align="center" colspan="3">Op2</td> <td bgcolor="#EEEEEEEE" align="center" colspan="4">CRm</td> <td bgcolor="#EEEEEEEE" align="center" colspan="4">CRn</td> <td bgcolor="#EEEEEEEE" align="center" colspan="1"><B>NonSecure</B></td> </tr> 
</table>
<EM>CP15 TrustZone &tm; registers encoding</EM>

Where the same registers are specified in the original CP15 block, the Current 
World register values are accessed.

\section cp15_15 CP15 register 15

There are some ARM variants that implement CP15 registers that do not fit into 
this encoding scheme (they have values of Op1 other than zero). These extra 
registers are always part of CP15 register 15 (CRn = 15), which is 
implementation defined in the architecture reference manual. They include test 
registers, cache modification registers, the trace control register, and power 
management registers.
In order to preserve an easy mapping between register numbers and MRC 
instruction encoding, extended CP15 r15 registers have their own special 
encoding in block 15 of the register map:
<table>
<tr><td>15</td> <td>14</td><td>13</td><td>12</td><td>11</td><td>10</td><td>9</td><td>8</td><td>7</td><td>6</td><td>5</td><td>4</td> <td>3</td><td>2</td><td>1</td><td>0</td></tr>
<tr><td bgcolor="#CCCCCCCC">0</td><td bgcolor="#CCCCCCCC" align="center" colspan="4">15</td> <td bgcolor="#CCCCCCCC" align="center" colspan="1">1</td> <td bgcolor="#EEEEEEEE" align="center" colspan="3">Op1 (!= 0)</td> <td bgcolor="#EEEEEEEE" align="center" colspan="3">Op2</td> <td bgcolor="#EEEEEEEE" align="center" colspan="4">CRm</td> </tr> 
</table>
<EM>Extended CP15 register 15 encoding</EM>

Note that this extended encoding should ONLY be used for registers which require
an Op1 field that is NOT zero. For CP15 r15 registers with Op1 = 0, use the 
standard CP15 register encoding. Note that less than half of register block 15 
is used by this encoding scheme. The rest of the block is reserved for future 
expansion. Here are some examples of extended CP15 registers:

<table>
<tr bgcolor="#CCCCCCCC"><th>Number</th><th>Register</th></tr>
<tr bgcolor="#EEEEEEEE"><td>0x7C81</td><td>Trace Control register (946E-S, 926E-S)</td></tr>
<tr bgcolor="#EEEEEEEE"><td>0x7D66</td><td>D cache CAM write (920T only)</td></tr>
<tr bgcolor="#EEEEEEEE"><td>0x7C91</td><td>BIST control register (966E-S only)</td></tr> 
</table>

\section cp_vfp VFP and SIMD (CP10 and CP11)
The registers of the ARM &reg; vector floating point unit are accessed through CP10 
and CP11. For single precision floating point registers (S0-S31), the register 
number is encoded directly using bits 0-4. In order to construct the necessary 
FMRS instruction, an emulator would have to split this field up into the Fn 
field and the N bit (see the VFP technical reference manual for more details): 

<table>
<tr><td>15</td> <td>14</td><td>13</td><td>12</td><td>11</td><td>10</td><td>9</td><td>8</td><td>7</td><td>6</td><td>5</td><td>4</td> <td>3</td><td>2</td><td>1</td><td>0</td></tr>
<tr><td bgcolor="#CCCCCCCC">1</td><td bgcolor="#CCCCCCCC" align="center" colspan="4">CP10</td> <td bgcolor="#CCCCCCCC" align="center" colspan="3">0</td> <td bgcolor="#CCCCCCCC" align="center" colspan="3">0</td> <td bgcolor="#EEEEEEEE" align="center" colspan="5">Sn</td> </tr> 
</table>
<EM>VFP register encoding form 1: Single precision floating point registers</EM>

For double precision floating point registers (D0-D15) in VFPv2 and below, 
(D0-D31) in VFPv3, the register number is encoded in bits 1-5. A double 
precision register is 64 bits long, so takes up two register locations. To 
ensure these locations are consecutive, Bit 0 (the H bit) is used to determines 
which half of the register is required (0 for low, 1 for high). In order to 
construct the necessary FMRDH or FMRDL instruction, an emulator would have to 
use the H bit as the lowest bit of the opcode field:
<table>
<tr><td>15</td> <td>14</td><td>13</td><td>12</td><td>11</td><td>10</td><td>9</td><td>8</td><td>7</td><td>6</td><td>5</td><td>4</td> <td>3</td><td>2</td><td>1</td><td>0</td></tr>
<tr><td bgcolor="#CCCCCCCC">1</td><td bgcolor="#CCCCCCCC" align="center" colspan="4">CP11</td> <td bgcolor="#CCCCCCCC" align="center" colspan="3">0</td> <td bgcolor="#CCCCCCCC" align="center" colspan="2">0</td> <td bgcolor="#EEEEEEEE" align="center" colspan="5">Dn</td> <td bgcolor="#EEEEEEEE" align="center" colspan="1">H</td> </tr> 
</table>
<EM>VFP register encoding form 2: Double precision floating point registers</EM>

Where an emulator gets a request for the lower half of the double precision 
register, and a register count of two, this represents a request for a complete 
double precision register. The emulator may use the FMRRD instruction for 
efficiency in this case.
SIMD extensions use the same register set as VFPv3, although they may present a 
view of quad word registers. Clients access the SIMD registers using the 
encodings above.  The access of a quad word register would consist of requesting 
the encoding for the lower half of the least significant double precision 
register with a register count of four, to return the complete 128 bit register.

For VFP system registers, the register number is encoded directly using bits 1-4. 
Bit 0 is zero, for consistency with the other two encodings. Bits 8-10 are set  
to the value 7, to reflect the opcode encoding used in the corresponding FMRX 
instructions:
<table>
<tr><td>15</td> <td>14</td><td>13</td><td>12</td><td>11</td><td>10</td><td>9</td><td>8</td><td>7</td><td>6</td><td>5</td><td>4</td> <td>3</td><td>2</td><td>1</td><td>0</td></tr>
<tr><td bgcolor="#CCCCCCCC">1</td><td bgcolor="#CCCCCCCC" align="center" colspan="4">CP10</td> <td bgcolor="#CCCCCCCC" align="center" colspan="3">7</td> <td bgcolor="#CCCCCCCC" align="center" colspan="3">0</td> <td bgcolor="#EEEEEEEE" align="center" colspan="4">reg</td> <td bgcolor="#CCCCCCCC" align="center" colspan="1">0</td> </tr> 
</table>
<EM>VFP register encoding form 3: System registers</EM>

So the specific register numbers for the defined system registers in VFPv1, 
VFPv2 and VFPv3 are:

<table>
<tr bgcolor="#CCCCCCCC"><th>Number</th><th>VFP version</th><th>Register</th></tr>
<tr bgcolor="#EEEEEEEE"><td>0xD700</td><td>v1, v2, v3</td><td>FPSID</td></tr>
<tr bgcolor="#EEEEEEEE"><td>0xD702</td><td>v1, v2, v3</td><td>FPSCR</td></tr>
<tr bgcolor="#EEEEEEEE"><td>0xD704 - 0xD70A</td><td>All</td><td>Reserved</td></tr>
<tr bgcolor="#EEEEEEEE"><td>0xD70C</td><td>v3 only</td><td>MVFR1</td></tr>
<tr bgcolor="#EEEEEEEE"><td>0xD70E</td><td>v3 only</td><td>MVFR0</td></tr>
<tr bgcolor="#EEEEEEEE"><td>0xD710</td><td>v1, v2, v3</td><td>FPEXC</td></tr>
<tr bgcolor="#EEEEEEEE"><td>0xD712 - 0xD71E</td><td>All</td><td>Sub-Architecture Defined.</td></tr>
</table>

\section cp14 CP14
CP14 is used to control debug on ARM10 and architecture v6 processors. There are
128 potential registers encoded using bits from the CRm and Op2 fields of the 
MCR and MRC instructions. Op1 is always zero. The register numbers for CP14 
debug are encoded like this:
<table>
<tr><td>15</td> <td>14</td><td>13</td><td>12</td><td>11</td><td>10</td><td>9</td><td>8</td><td>7</td><td>6</td><td>5</td><td>4</td> <td>3</td><td>2</td><td>1</td><td>0</td></tr>
<tr><td bgcolor="#CCCCCCCC">1</td><td bgcolor="#CCCCCCCC" align="center" colspan="4">CP14</td> <td bgcolor="#CCCCCCCC" align="center" colspan="3">0</td> <td bgcolor="#CCCCCCCC" align="center" colspan="1">0</td> <td bgcolor="#EEEEEEEE" align="center" colspan="3">Op2</td> <td bgcolor="#EEEEEEEE" align="center" colspan="4">CRm</td> </tr> 
</table>
<EM>CP14 register encoding form 1: Debug</EM>

The register numbers of specific debug registers are therefore:
<table>
<tr bgcolor="#CCCCCCCC"><th>Number</th><th>Register</th></tr>
<tr bgcolor="#EEEEEEEE"><td>0xF000</td><td>DIDR</td></tr>
<tr bgcolor="#EEEEEEEE"><td>0xF001</td><td>DSCR</td></tr>
<tr bgcolor="#EEEEEEEE"><td>0xF005</td><td>DTR</td></tr>
<tr bgcolor="#EEEEEEEE"><td>0xF007</td><td>VTR</td></tr>
<tr bgcolor="#EEEEEEEE"><td>0xF04x</td><td>BVRx</td></tr>
<tr bgcolor="#EEEEEEEE"><td>0xF05x</td><td>BCRx</td></tr>
<tr bgcolor="#EEEEEEEE"><td>0xF06x</td><td>WVRx</td></tr>
<tr bgcolor="#EEEEEEEE"><td>0xF07x</td><td>WCRx</td></tr>
</table>

Note that for an emulator vehicle, some of these registers may be available 
through dedicated scan chains (for architecture v6 this includes most of the 
CP14 registers). The vehicle should expose these registers in its register map 
if it wants to allow the client direct access to them.
<br>
For processors with Jazelle &reg; (Java acceleration) extensions, CP14 is also used 
to configure the Jazelle extensions. Register numbers are encoded in the CRn 
field, with Op1 set to the value 7. The corresponding register numbers are 
encoded like this:
<table>
<tr><td>15</td> <td>14</td><td>13</td><td>12</td><td>11</td><td>10</td><td>9</td><td>8</td><td>7</td><td>6</td><td>5</td><td>4</td> <td>3</td><td>2</td><td>1</td><td>0</td></tr>
<tr><td bgcolor="#CCCCCCCC">1</td><td bgcolor="#CCCCCCCC" align="center" colspan="4">CP14</td> <td bgcolor="#CCCCCCCC" align="center" colspan="3">7</td> <td bgcolor="#CCCCCCCC" align="center" colspan="4">0</td><td bgcolor="#EEEEEEEE" align="center" colspan="4">CRn</td> </tr>
</table>
<EM>CP14 register encoding form 2: Jazelle config</EM>

For processors supporting the Thumb-2EE (Jazelle-X) extensions, CP14 is used to 
configure this feature. Register numbers are again in the CRn field with Op1 
set to the value 6.
<table> 
<tr><td>15</td> <td>14</td><td>13</td><td>12</td><td>11</td><td>10</td><td>9</td><td>8</td><td>7</td><td>6</td><td>5</td><td>4</td> <td>3</td><td>2</td><td>1</td><td>0</td></tr>
<tr><td bgcolor="#CCCCCCCC">1</td><td bgcolor="#CCCCCCCC" align="center" colspan="4">CP14</td> <td bgcolor="#CCCCCCCC" align="center" colspan="3">6</td> <td bgcolor="#CCCCCCCC" align="center" colspan="4">0</td><td bgcolor="#EEEEEEEE" align="center" colspan="4">CRn</td>
</tr> </table>
<EM>CP14 register encoding form 3: Thumb-2EE (Jazelle-X) config</EM>

*/

/**
    \page ArmCSDPTemplate The ARMCS-DP Template
    \section CSDP The ARMCS-DP Template
    This template is used to directly access a Coresight &tm; DAP on a JTAG 
    scanchain.  Some utilities and clients will use this DAP template although 
    debuggers connected to coresight cores, such as the Cortex &tm; A8, will use
    core specific templates. This template gives the lowest possible coresight 
    register access to a coresight system with very little in the way of 
    functional optimisation.

    \subsection CSDPMsgInfo Function support
    The template supports a reduced set of functionality compared to a normal 
    core template. No support for the execution functions - other than the 
    reset, breakpoint, and channel - is provided. 
    Register operations support the set of coresight register IDs only - see 
    sections \ref RegB3and4 and \ref RegB4 .
    The "page" and "rule" parameters allow memory access functions to use the 
    AHB-AP or APB-AP in a coresight system. If these are not used then the AP 
    specified by the "MEMORY_ACCESS_AP" configuration item will be used.

*/
#endif

/** \page DummyFunctionIndex Function Index
This page will be replaced by the real index during document generation
*/

#endif /* def RDDI_H */


/* end of file rddi.h */

