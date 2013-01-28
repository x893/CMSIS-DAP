/*
 * RDDI_DAP_IF.h:  ULINK to RDDI-DAP Interface definitions
 *
 * Copyright (c) 2012, ARM Limited. All Rights Reserved.
 */

#ifndef _RDDI_DAP_IF_H_
#define _RDDI_DAP_IF_H_


// Error Codes
#define RDDI_DAP_ERROR          0x2000  // RDDI-DAP Error
#define RDDI_DAP_ERROR_NO_DLL   0x2001  // CMSIS_DAP.DLL missing
#define RDDI_DAP_ERROR_INTERNAL 0x2002  // Internal DLL Error
#define RDDI_DAP_ERROR_POWER    0x2003  // Device could not be powered up
#define RDDI_DAP_ERROR_DEBUG    0x2004  // Cannot enter Debug Mode
#define RDDI_DAP_ERROR_MEMORY   0x2005  // Cannot access Memory
#define RDDI_DAP_ERROR_INUSE    0x2006  // Debug Port in use
#define RDDI_DAP_ERROR_SWJ      0x2007  // SWD/JTAG Communication Error


// Configuration string
//
//   "Key=Value;" (multiple keys are separated using semi-colons)
//
//    Key         Value     Default   Comment
//    --------------------------------------------------------------------------
//    Port        JTAG|SW      None   Selects Debug Port (JTAG or SW)
//    SWJ         Y|N             N   Enable/Disable SWJ switch sequence
//    Clock       Number    1000000   Set maximum Debug Clock in Hz
//    Turnaround  1..4            1   Turnaround period (SWD only)
//    DataPhase   Y|N             N   Force Data also on WAIT/FAULT (SWD only)
//    IdleCycles  0..255          0   Number of extra idle cycles after each transfer (SWD only)
//    WaitRetry   0..65535      100   Number of transfer retries after WAIT response
//    
//   Examples:
//     "Port=SW;SWJ=Y;Clock=1000000;"
//     "Port=JTAG;SWJ=N;Clock=0;"

// Target strings (control and monitor target signals)
//
//   Signals:
//     "sys_reset"      : overall system reset
//     "sys_power"      : overall system power
//     "core_reset_n"   : core n reset state
//     "core_power_n"   : core n power state 
//     "core_reset_all" : all core reset states - response will list each individual state
//     "core_power_all" : all core power states - response will list each individual state
//     "run_led"        : target running LED
//
//   Commands for signals:
//     "on"             : set the signal on or asserted
//     "off"            : set the signal off or de-asserted
//     "read"           : return the current state of the signal
//     "signal_avail"   : return a list of available signals
//
//   Responses for signals:
//     "on"             : the signal is on or asserted
//     "off"            : the signal is off or de-asserted
//     "latch_on"       : the signal has been asserted since the last read of the signal (may be combined with current state)
//     "unknown"        : the signal name not recognised
//     "err"            : the signal cannot be driven to the requested value
//
//   Multiple Commands and responses are separated using semi-colons.
//
//   Examples:
//     Command "signal_avail"
//     Response "sys_reset;sys_power"
// 
//     Command "sys_reset.on;sys_power.read"
//     Response "sys_reset.on;sys_power.on"
// 
//     Command "sys_reset.off"
//     Response "sys_reset.off"


// Variables
extern  BOOL RDDI_DAP_IF;         // RDDI-DAP Interface Active Flag

// Functions
extern  int  RDDI_DAP_Init        (void);
extern  int  RDDI_DAP_UnInit      (void);

extern  int  RDDI_DAP_Detect      (int *num);
extern  int  RDDI_DAP_Vendor      (int  idx, char *str, int len);
extern  int  RDDI_DAP_Product     (int  idx, char *str, int len);
extern  int  RDDI_DAP_SerialNo    (int  idx, char *str, int len);
extern  int  RDDI_DAP_FWVersion   (int  idx, char *str, int len);

extern  int  RDDI_DAP_Configure   (int idx, char *cfg);
extern  int  RDDI_DAP_Connect     (void);
extern  int  RDDI_DAP_Disconnect  (void);

extern  int  RDDI_DAP_List        (DWORD *id, int *cnt, int max);

extern  int  RDDI_DAP_Abort       (DWORD  val);

extern  int  RDDI_DAP_ReadDP      (BYTE   adr,  DWORD *val);
extern  int  RDDI_DAP_WriteDP     (BYTE   adr,  DWORD  val);
extern  int  RDDI_DAP_ReadAP      (BYTE   adr,  DWORD *val);
extern  int  RDDI_DAP_WriteAP     (BYTE   adr,  DWORD  val);

extern  int  RDDI_DAP_ReadD32     (DWORD  adr,  DWORD *val);
extern  int  RDDI_DAP_ReadD16     (DWORD  adr,  WORD  *val);
extern  int  RDDI_DAP_ReadD8      (DWORD  adr,  BYTE  *val);
extern  int  RDDI_DAP_WriteD32    (DWORD  adr,  DWORD  val);
extern  int  RDDI_DAP_WriteD16    (DWORD  adr,  WORD   val);
extern  int  RDDI_DAP_WriteD8     (DWORD  adr,  BYTE   val);

extern  int  RDDI_DAP_ReadBlock   (DWORD  adr,  BYTE *pB, DWORD nMany);
extern  int  RDDI_DAP_WriteBlock  (DWORD  adr,  BYTE *pB, DWORD nMany);
extern  int  RDDI_DAP_VerifyBlock (DWORD  adr,  BYTE *pB, DWORD nMany);

extern  int  RDDI_DAP_ReadARMMem  (DWORD *nAdr, BYTE *pB, DWORD nMany);
extern  int  RDDI_DAP_WriteARMMem (DWORD *nAdr, BYTE *pB, DWORD nMany);
extern  int  RDDI_DAP_VerifyARMMem(DWORD *nAdr, BYTE *pB, DWORD nMany);

extern  int  RDDI_DAP_GetARMRegs  (RgARMCM *regs, RgARMFPU *rfpu, U64 mask);
extern  int  RDDI_DAP_SetARMRegs  (RgARMCM *regs, RgARMFPU *rfpu, U64 mask);

extern  int  RDDI_DAP_SysCallExec (RgARMCM *regs);
extern  int  RDDI_DAP_SysCallRes  (DWORD   *rval);

extern  int  RDDI_DAP_Target      (char *cmd_str, char *resp_str, const int resp_len);

extern  int  RDDI_DAP_DebugInit   (void);


#endif  // _RDDI_DAP_IF_H_
