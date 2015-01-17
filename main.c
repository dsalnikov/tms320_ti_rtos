/*
 *  ======== main.c ========
 */
#include "DSP2802x_Device.h"
//#define xdc__strict

//#include <xdc/std.h>

#include <xdc/runtime/Error.h>
#include <xdc/runtime/System.h>

#include <ti/sysbios/BIOS.h>

#include <ti/sysbios/knl/Task.h>



/*
 *  ======== taskFxn ========
 */
Void taskFxn(UArg a0, UArg a1)
{
	while(1)
	{
		GpioDataRegs.GPATOGGLE.bit.GPIO1 = 1;
		Task_sleep(1000);
	}
}

/*
 *  ======== main ========
 */
Int main()
{ 
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
