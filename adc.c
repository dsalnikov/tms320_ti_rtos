/*
 * adc.c
 *
 *  Created on: 14 марта 2015 г.
 *      Author: lamazavr
 */
#include "DSP2802x_Device.h"
#include "adc.h"
#include "system.h"
#include "iqmath/include/IQmathLib.h"
#include "pwm.h"

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

#define ADC_OFFSET 2048
#define ADC_MAX_VALUE (4095/2)

inline _iq get_adc_data(Uint16 value, _iq base)
{
	return _IQmpy(_IQ((value - ADC_OFFSET)/ADC_MAX_VALUE), base);
}

void adc_isr(UArg arg)
{
	//TODO: create some filter here
	system.current.PhaseA = get_adc_data(AdcResult.ADCRESULT0, system.bases.current);
	system.current.PhaseB = get_adc_data(AdcResult.ADCRESULT1, system.bases.current);
	system.current.PhaseC = get_adc_data(AdcResult.ADCRESULT2, system.bases.current);

	system.voltage.PhaseA = get_adc_data(AdcResult.ADCRESULT3, system.bases.voltage);
	system.voltage.PhaseB = get_adc_data(AdcResult.ADCRESULT4, system.bases.voltage);
	system.voltage.PhaseC = get_adc_data(AdcResult.ADCRESULT5, system.bases.voltage);

	system.Udc = get_adc_data(AdcResult.ADCRESULT6, system.bases.voltage);

	AdcRegs.ADCINTFLGCLR.bit.ADCINT1 = 1;	  //Clear ADCINT1 flag reinitialize for next SOC
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;   // Acknowledge interrupt to PIE
}

void measure_high_freq()
{
	system.rms_accumulation.PhaseA += system.current.PhaseA * system.current.PhaseA;
	system.rms_accumulation.PhaseB += system.current.PhaseB * system.current.PhaseB;
	system.rms_accumulation.PhaseC += system.current.PhaseC * system.current.PhaseC;

	system.rms_samples_number++;

	if (pwm.period_out)
	{
		system.rms_copy = system.rms_accumulation;
		system.rms_samples_number_copy = system.rms_samples_number;
		pwm.period_out = 0;
		pwm.en_rms_calc = 1;

		system.rms_samples_number = 0;
		system.rms_accumulation.PhaseA = 0;
		system.rms_accumulation.PhaseB = 0;
		system.rms_accumulation.PhaseC = 0;

		Semaphore_post(calculate_rms_sem);
	}
}

void calculate_rms()
{
	_iq val;
	Uint32 tmp;

	if (pwm.en_rms_calc && system.rms_samples_number_copy > 0)
	{
		tmp = sqrt(system.rms_copy.PhaseA / system.rms_samples_number_copy);
		system.rms_current.PhaseA = get_adc_data(tmp, system.bases.current);
		tmp = sqrt(system.rms_copy.PhaseB / system.rms_samples_number_copy);
		system.rms_current.PhaseB = get_adc_data(tmp, system.bases.current);
		tmp = sqrt(system.rms_copy.PhaseC / system.rms_samples_number_copy);
		system.rms_current.PhaseC = get_adc_data(tmp, system.bases.current);
	}
}

