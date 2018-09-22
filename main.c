/*
 *  ======== main.c ========
 */
#include "DSP2802x_Device.h"
#include "DSP2802x_GlobalPrototypes.h"

#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Semaphore.h>

#include <xdc/cfg/global.h>

#include "Uart.h"
#include "pwm.h"
#include "adc.h"


/*
 *  ======== main_task_function ========
 */
Void main_task_function(UArg a0, UArg a1)
{

    pwm_init();

	while(1)
	{
	    if (Semaphore_pend(calculate_rms_sem, 100))
	    {
	        calculate_rms();
	    }
//	    else
//	    {
//	        Task_sleep(100);
//	    }
	}
}

extern Uint16 RamfuncsLoadStart;
extern Uint16 RamfuncsLoadEnd;
extern Uint16 RamfuncsRunStart;

/*
 *  ======== main ========
 */
Int main()
{ 
	MemCopy(&RamfuncsLoadStart, &RamfuncsLoadEnd, &RamfuncsRunStart);
	InitSysCtrl();

	DINT;

	InitPieCtrl();
	IER = 0x0000;
	IFR = 0x0000;
	EINT;

    init_uart();
    init_adc();

    BIOS_start();    /* does not return */
    return(0);
}
