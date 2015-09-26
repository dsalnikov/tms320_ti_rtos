/*
 *  Do not modify this file; it is automatically 
 *  generated and any modifications will be overwritten.
 *
 * @(#) xdc-A44
 */

#include <xdc/std.h>

#include <ti/sysbios/knl/Task.h>
extern const ti_sysbios_knl_Task_Handle task0;

#include <ti/sysbios/hal/Hwi.h>
extern const ti_sysbios_hal_Hwi_Handle uart_rx_hwi_handle;

#include <ti/sysbios/knl/Semaphore.h>
extern const ti_sysbios_knl_Semaphore_Handle modbus_sem;

#include <ti/sysbios/hal/Timer.h>
extern const ti_sysbios_hal_Timer_Handle modbus_timer;

#include <ti/sysbios/knl/Task.h>
extern const ti_sysbios_knl_Task_Handle drv8301_task_handle;

#include <ti/sysbios/hal/Hwi.h>
extern const ti_sysbios_hal_Hwi_Handle epwm1_hwi_handle;

#include <ti/sysbios/hal/Hwi.h>
extern const ti_sysbios_hal_Hwi_Handle adc_hwi_handle;

#include <ti/sysbios/knl/Semaphore.h>
extern const ti_sysbios_knl_Semaphore_Handle calculate_rms_sem;

extern int xdc_runtime_Startup__EXECFXN__C;

extern int xdc_runtime_Startup__RESETFXN__C;

#ifndef ti_sysbios_knl_Task__include
#ifndef __nested__
#define __nested__
#include <ti/sysbios/knl/Task.h>
#undef __nested__
#else
#include <ti/sysbios/knl/Task.h>
#endif
#endif

extern ti_sysbios_knl_Task_Struct TSK_idle;

