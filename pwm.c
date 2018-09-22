/*
 * pwm.c
 *
 *  Created on: 17 авг. 2014 г.
 *      Author: lamazavr
 */

#include "pwm.h"
#include <math.h>
#include <xdc/cfg/global.h>
#include "system.h"
#include "scalar.h"
#include "fails_control.h"

pwm_t pwm;

#include "debug.h"

#pragma CODE_SECTION( epwm1_timer_isr, "ramfuncs")
void epwm1_timer_isr()
{
	//int32 tmp;

	system_fsm();
	rate_generator();
	scalar_control();

//	fails_control();

    debug_write(DEBUG_WR_ON_START, 100, svgen_dq.Ta);

//	tmp = (int32)(_IQtoIQ15(svgen_dq.Ta) * pwm.pwm_counter_max); //u
//	pwm.t.PhaseA = (tmp >> 16) + (pwm.pwm_counter_max >> 1);
//	EPwm1Regs.CMPA.half.CMPA = pwm.t.PhaseA;
//
//	tmp = (int32)(_IQtoIQ15(svgen_dq.Tb) * pwm.pwm_counter_max); //u
//	pwm.t.PhaseB = (tmp >> 16) + (pwm.pwm_counter_max >> 1);
//	EPwm2Regs.CMPA.half.CMPA = pwm.t.PhaseB;
//
//	tmp = (int32)(_IQtoIQ15(svgen_dq.Tc) * pwm.pwm_counter_max); //u
//	pwm.t.PhaseC = (tmp >> 16) + (pwm.pwm_counter_max >> 1);
//	EPwm3Regs.CMPA.half.CMPA = pwm.t.PhaseC;
//
//
	measure_high_freq();

	// Clear INT flag for this timer
	EPwm1Regs.ETCLR.bit.INT = 1;
	// Acknowledge this interrupt to receive more interrupts from group 3
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP3;
}

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
	GpioCtrlRegs.GPADIR.bit.GPIO0 = 1;
	GpioCtrlRegs.GPAMUX1.bit.GPIO0 = 0;//1;  // epwm1a
	GpioCtrlRegs.GPAMUX1.bit.GPIO1 = 1;	 // epwm1b
	GpioCtrlRegs.GPAMUX1.bit.GPIO2 = 1;	 // epwm2a
	GpioCtrlRegs.GPAMUX1.bit.GPIO3 = 1;	 // epwm2b
	GpioCtrlRegs.GPAMUX1.bit.GPIO4 = 1;	 // epwm3a
	GpioCtrlRegs.GPAMUX1.bit.GPIO5 = 1;  // epwm3b

	EDIS;
}

#define SYSTEM_FREQ_HZ 60000000
#define F_REF 50

#define PWM_FREQ 4000

void pwm_init()
{
	// настраиваем GPIO для ШИМ
	pwm_gpio_init();

	system.bases.current = 15;
	system.bases.voltage = 24;
	system.bases.frequency = 100;

	pwm.pwm_freq = PWM_FREQ;
	pwm.pwm_period = _IQ(1.0)/pwm.pwm_freq;
	pwm.pwm_counter_max = SYSTEM_FREQ_HZ / PWM_FREQ;

	pwm.angle = _IQ(0);
	pwm.angle_step = _IQmpy(_IQ(F_REF), pwm.pwm_period);

	system.Tacc = _IQ(10);
	system.Fref = _IQ(50);
	system.Uref = _IQ(20);

	system.uf.U[0] = 0;
	system.uf.U[1] = system.Uref;
	system.uf.F[0] = 0;
	system.uf.F[1] = system.Fref;

	system.Command = no_command;
	system.state = init_state;

	pwm1_init();
	pwm1_dead_band_configure();
	pwm2_init();
	pwm3_init();

	pwm_off();

	//TODO: configure as HWI
	pwm1_interrupt_init();

}

void pwm1_init()
{
	// Setup TBCLK
	EPwm1Regs.TBPRD = pwm.pwm_counter_max;		//15000 - 4kHz
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
	EPwm1Regs.TBCTL.bit.SYNCOSEL = TB_CTR_ZERO;

	// Setup shadowing
	EPwm1Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;
	EPwm1Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;  // Load on Zero

	// Set actions
	EPwm1Regs.AQCTLA.bit.ZRO = AQ_SET;
	EPwm1Regs.AQCTLA.bit.CAU = AQ_CLEAR;
}

void pwm1_dead_band_configure()
{
	EPwm1Regs.DBCTL.bit.IN_MODE = 0;
	EPwm1Regs.DBCTL.bit.POLSEL = 0x02; //active high complementary
	EPwm1Regs.DBCTL.bit.OUT_MODE = 3; //enabled for both

	//dead band - 0.1us
	EPwm1Regs.DBFED = 5;
	EPwm1Regs.DBRED = 5;
}

void pwm1_disable()
{
	EPwm1Regs.CMPA.half.CMPA = 0;
	EPwm1Regs.CMPB = 0;
	EPwm1Regs.DBCTL.bit.IN_MODE = 1;
}

void pwm2_init()
{
	// Setup TBCLK
	EPwm2Regs.TBPRD = pwm.pwm_counter_max;           // Set timer period 801 TBCLKs
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
	EPwm1Regs.TBCTL.bit.SYNCOSEL = TB_SYNC_IN;

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
	EPwm3Regs.TBPRD = pwm.pwm_counter_max;           // Set timer period 801 TBCLKs
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
	EPwm1Regs.TBCTL.bit.SYNCOSEL = TB_SYNC_IN;

	// Setup shadowing
	EPwm3Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;
	EPwm3Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;  // Load on Zero

	// Set actions
	EPwm3Regs.AQCTLA.bit.ZRO = AQ_SET;
	EPwm3Regs.AQCTLA.bit.CAU = AQ_CLEAR;
}


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

void pwm_it_enable()
{
	EPwm1Regs.ETSEL.bit.INTEN = 1;
}

void pwm_it_disable()
{
	EPwm1Regs.ETSEL.bit.INTEN = 0;
}

void pwm_on()
{
	EALLOW;
	// Включить все модули
	EPwm1Regs.TZCLR.all = 0xFF;
	EPwm2Regs.TZCLR.all = 0xFF;
	EPwm3Regs.TZCLR.all = 0xFF;
	EDIS;
    system.pwm_on = 1;
}

void pwm_off()
{
	EALLOW;
	// Выключить все модули
	EPwm1Regs.TZFRC.bit.OST = 1;
	EPwm2Regs.TZFRC.bit.OST = 1;
	EPwm3Regs.TZFRC.bit.OST = 1;
	EDIS;
    system.pwm_on = 0;
}
