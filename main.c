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

Uint16 LedsState = 0;
Uint16 TestPrm = 0;

/*
 *  ======== taskFxn ========
 */
Void taskFxn(UArg a0, UArg a1)
{
	while(1)
	{
	    Uart_send_msg("Helo world!");


		if (LedsState & 0x01)
			GpioDataRegs.GPACLEAR.bit.GPIO0 = 1;
		else
			GpioDataRegs.GPASET.bit.GPIO0 = 1;

		if (LedsState & 0x02)
			GpioDataRegs.GPACLEAR.bit.GPIO1 = 1;
		else
			GpioDataRegs.GPASET.bit.GPIO1 = 1;

		if (LedsState & 0x04)
			GpioDataRegs.GPACLEAR.bit.GPIO2 = 1;
		else
			GpioDataRegs.GPASET.bit.GPIO2 = 1;

		if (LedsState & 0x08)
			GpioDataRegs.GPACLEAR.bit.GPIO3 = 1;
		else
			GpioDataRegs.GPASET.bit.GPIO3 = 1;

		Task_sleep(1000);
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
	//InitPieVectTable();

	EINT;

    Uart_init();


	// настройка выводов со светодиодами
	EALLOW; // разрешаем доступ к защищенным регистрам
	GpioCtrlRegs.GPAMUX1.bit.GPIO0 = 0;
	GpioCtrlRegs.GPAMUX1.bit.GPIO1 = 0;
	GpioCtrlRegs.GPAMUX1.bit.GPIO2 = 0;
	GpioCtrlRegs.GPAMUX1.bit.GPIO3 = 0;

    GpioCtrlRegs.GPADIR.all = 0xFFFFFFFF;   // Все как выход
    EDIS; // запрещаем доступ к регистрам

    BIOS_start();    /* does not return */
    return(0);
}
