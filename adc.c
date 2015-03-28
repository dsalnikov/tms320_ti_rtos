/*
 * adc.c
 *
 *  Created on: 14 марта 2015 г.
 *      Author: lamazavr
 */
#include "DSP2802x_Device.h"
#include "adc.h"
#include "system.h"

//TODO: remove this after parameters set/get functions implementation
extern System_t system;

void init_adc()
{
	InitAdc();

	PieCtrlRegs.PIEIER1.bit.INTx1 = 1;	// Enable INT 1.1 in the PIE
	IER |= M_INT1; 						// Enable CPU Interrupt 1
	EINT;          						// Enable Global interrupt INTM
	ERTM;          						// Enable Global realtime interrupt DBGM

	// Configure ADC
	EALLOW;
	AdcRegs.ADCCTL1.bit.INTPULSEPOS	= 1;	//ADCINT1 trips after AdcResults latch
	AdcRegs.INTSEL1N2.bit.INT1E     = 1;	//Enabled ADCINT1
	AdcRegs.INTSEL1N2.bit.INT1CONT  = 0;	//Disable ADCINT1 Continuous mode
	AdcRegs.INTSEL1N2.bit.INT1SEL	= 1;	//setup EOC1 to trigger ADCINT1 to fire

	//current measurements sources
	// Ia_FB - B1
	// Ib_FB - B3
	// Ic_FB - B7
	AdcRegs.ADCSOC0CTL.bit.CHSEL 	= 0x09;
	AdcRegs.ADCSOC1CTL.bit.CHSEL 	= 0x0B;
	AdcRegs.ADCSOC2CTL.bit.CHSEL 	= 0x0F;

	//ADCTRIG5 Ц ePWM1,ADCSOCA
	AdcRegs.ADCSOC0CTL.bit.TRIGSEL 	= 5;
	AdcRegs.ADCSOC1CTL.bit.TRIGSEL 	= 5;
	AdcRegs.ADCSOC2CTL.bit.TRIGSEL 	= 5;

	//voltage measurements sources and triggers for them
	// Va_FB - A3
	// Vb_FB - A1
	// Vc_FB - A0
	AdcRegs.ADCSOC3CTL.bit.CHSEL = 0x03;
	AdcRegs.ADCSOC4CTL.bit.CHSEL = 0x01;
	AdcRegs.ADCSOC5CTL.bit.CHSEL = 0x00;

	AdcRegs.ADCSOC3CTL.bit.TRIGSEL 	= 5;
	AdcRegs.ADCSOC4CTL.bit.TRIGSEL 	= 5;
	AdcRegs.ADCSOC5CTL.bit.TRIGSEL 	= 5;

	// DC_FB - A7
	AdcRegs.ADCSOC6CTL.bit.CHSEL = 0x07;
	AdcRegs.ADCSOC6CTL.bit.TRIGSEL 	= 5;

	//set SOCs S/H Window to 7 ADC Clock Cycles, (6 ACQPS plus 1)
	AdcRegs.ADCSOC0CTL.bit.ACQPS = 6;
	AdcRegs.ADCSOC1CTL.bit.ACQPS = 6;
	AdcRegs.ADCSOC2CTL.bit.ACQPS = 6;
	AdcRegs.ADCSOC3CTL.bit.ACQPS = 6;
	AdcRegs.ADCSOC4CTL.bit.ACQPS = 6;
	AdcRegs.ADCSOC5CTL.bit.ACQPS = 6;
	AdcRegs.ADCSOC6CTL.bit.ACQPS = 6;

	EDIS;

}

#pragma CODE_SECTION(adc_isr, "ramfuncs");
void adc_isr(UArg arg)
{
	//TODO: create some filter here
	system.current.PhaseA = AdcResult.ADCRESULT0;
	system.current.PhaseB = AdcResult.ADCRESULT1;
	system.current.PhaseC = AdcResult.ADCRESULT2;

	system.voltage.PhaseA = AdcResult.ADCRESULT3;
	system.voltage.PhaseB = AdcResult.ADCRESULT4;
	system.voltage.PhaseC = AdcResult.ADCRESULT5;

	system.InputVoltage = AdcResult.ADCRESULT6;

	AdcRegs.ADCINTFLGCLR.bit.ADCINT1 = 1;		//Clear ADCINT1 flag reinitialize for next SOC
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;   // Acknowledge interrupt to PIE
}

