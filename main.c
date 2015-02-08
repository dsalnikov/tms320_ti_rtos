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

Uint16 LedsState = 0;
Uint16 TestPrm = 0;

/*
 *  ======== taskFxn ========
 */
Void taskFxn(UArg a0, UArg a1)
{
	while(1)
	{

		Task_sleep(100);
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

    Uart_init();
    pwm_init();

    BIOS_start();    /* does not return */
    return(0);
}
