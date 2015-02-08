/*
 * pwm.c
 *
 *  Created on: 17 авг. 2014 г.
 *      Author: lamazavr
 */

#include "pwm.h"

void pwm_gpio_init()
{
	EALLOW;

	// включаем подтяжку
	GpioCtrlRegs.GPAPUD.bit.GPIO0 = 1;
	GpioCtrlRegs.GPAPUD.bit.GPIO1 = 1;
	GpioCtrlRegs.GPAPUD.bit.GPIO2 = 1;
	GpioCtrlRegs.GPAPUD.bit.GPIO3 = 1;
	GpioCtrlRegs.GPAPUD.bit.GPIO4 = 1;
	GpioCtrlRegs.GPAPUD.bit.GPIO5 = 1;

	// Настраиваем мультиплексоры PWM
	GpioCtrlRegs.GPAMUX1.bit.GPIO0 = 1;  // epwm1a
	GpioCtrlRegs.GPAMUX1.bit.GPIO1 = 1;	 // epwm1b
	GpioCtrlRegs.GPAMUX1.bit.GPIO2 = 1;	 // epwm2a
	GpioCtrlRegs.GPAMUX1.bit.GPIO3 = 1;	 // epwm2b
	GpioCtrlRegs.GPAMUX1.bit.GPIO4 = 1;	 // epwm3a
	GpioCtrlRegs.GPAMUX1.bit.GPIO5 = 1;  // epwm3b

	EDIS;
}

void pwm_init()
{
	// настраиваем GPIO для ШИМ
	pwm_gpio_init();

	pwm1_init();
	pwm2_init();
	pwm3_init();

	//TODO: configure as HWI
	//pwm1_interrupt_init();

}

void pwm1_init()
{
	// Setup TBCLK
	EPwm1Regs.TBPRD = 0x00FF;           // Set timer period 801 TBCLKs
	EPwm1Regs.TBPHS.all = 0x0000;           // Phase is 0
	EPwm1Regs.TBCTR = 0x0000;                      // Clear counter

	// Set Compare values
	EPwm1Regs.CMPA.half.CMPA = 0;     // Set compare A value
	//EPwm1Regs.CMPB = 200;               // Set Compare B value

	// настраиваем синхронный шим
	EPwm1Regs.TBCTL.bit.CTRMODE = TB_COUNT_UP; // считаем вверх/вниз
	EPwm1Regs.TBCTL.bit.PHSEN = TB_DISABLE;        // выключаем фазовый сдвиг
	EPwm1Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1;       // делитель 1
	EPwm1Regs.TBCTL.bit.CLKDIV = TB_DIV1;

	// Setup shadowing
	EPwm1Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;
	EPwm1Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;  // Load on Zero

	// Set actions
	EPwm1Regs.AQCTLA.bit.ZRO = AQ_SET;
	EPwm1Regs.AQCTLA.bit.CAU = AQ_CLEAR;

}

void pwm2_init()
{
	// Setup TBCLK
	EPwm2Regs.TBPRD = 0x00FF;           // Set timer period 801 TBCLKs
	EPwm2Regs.TBPHS.all = 0x0000;           // Phase is 0
	EPwm2Regs.TBCTR = 0x0000;                      // Clear counter

	// Set Compare values
	EPwm2Regs.CMPA.half.CMPA = 0;     // Set compare A value
	//EPwm1Regs.CMPB = 200;               // Set Compare B value

	// настраиваем синхронный шим
	EPwm2Regs.TBCTL.bit.CTRMODE = TB_COUNT_UP; // считаем вверх/вниз
	EPwm2Regs.TBCTL.bit.PHSEN = TB_DISABLE;        // выключаем фазовый сдвиг
	EPwm2Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1;       // делитель 1
	EPwm2Regs.TBCTL.bit.CLKDIV = TB_DIV1;

	// Setup shadowing
	EPwm2Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;
	EPwm2Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;  // Load on Zero

	// Set actions
	EPwm2Regs.AQCTLA.bit.ZRO = AQ_SET;
	EPwm2Regs.AQCTLA.bit.CAU = AQ_CLEAR;
}

void pwm3_init()
{
	// Setup TBCLK
	EPwm3Regs.TBPRD = 0x00FF;           // Set timer period 801 TBCLKs
	EPwm3Regs.TBPHS.all = 0x0000;           // Phase is 0
	EPwm3Regs.TBCTR = 0x0000;                      // Clear counter

	// Set Compare values
	EPwm3Regs.CMPA.half.CMPA = 0;     // Set compare A value
	//EPwm1Regs.CMPB = 200;               // Set Compare B value

	// настраиваем синхронный шим
	EPwm3Regs.TBCTL.bit.CTRMODE = TB_COUNT_UP; // считаем вверх/вниз
	EPwm3Regs.TBCTL.bit.PHSEN = TB_DISABLE;        // выключаем фазовый сдвиг
	EPwm3Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1;       // делитель 1
	EPwm3Regs.TBCTL.bit.CLKDIV = TB_DIV1;

	// Setup shadowing
	EPwm3Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;
	EPwm3Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;  // Load on Zero

	// Set actions
	EPwm3Regs.AQCTLA.bit.ZRO = AQ_SET;
	EPwm3Regs.AQCTLA.bit.CAU = AQ_CLEAR;
}

#if 0
void pwm1_interrupt_init()
{
	EALLOW;  // This is needed to write to EALLOW protected registers
	PieVectTable.EPWM1_INT = &epwm1_timer_isr;
	EDIS;

	IER |= M_INT3;

	PieCtrlRegs.PIEIER3.bit.INTx1 = 1;

	// настраиваем прерывание по пояалению в счетчике 0
	EPwm1Regs.ETSEL.bit.INTSEL = ET_CTR_ZERO;
	EPwm1Regs.ETSEL.bit.INTEN = 1; 		// разрешаем прерывание
	EPwm1Regs.ETPS.bit.INTPRD = ET_1ST;	// генерируем прерывание каждый раз

	//для прерывания АЦП
	EPwm1Regs.ETSEL.bit.SOCAEN	= 1;		// Enable SOC on A group
	EPwm1Regs.ETSEL.bit.SOCASEL	= 4;		// Select SOC from from CPMA on upcount
	EPwm1Regs.ETPS.bit.SOCAPRD 	= 1;		// Generate pulse on 1st event
}
#endif

