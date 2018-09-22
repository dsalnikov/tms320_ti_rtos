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
#include "math.h"

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

const long adc_bias = 2048;


#pragma CODE_SECTION( adc_isr, "ramfuncs")
void adc_isr(UArg arg)
{
	GpioDataRegs.GPASET.bit.GPIO0 = 1;

	system.adc_current.PhaseA = (long)AdcResult.ADCRESULT0 - adc_bias;
    system.adc_current.PhaseB = (long)AdcResult.ADCRESULT1 - adc_bias;
    system.adc_current.PhaseC = (long)AdcResult.ADCRESULT2 - adc_bias;

    system.adc_voltage.PhaseA = (long)AdcResult.ADCRESULT3 - adc_bias;
    system.adc_voltage.PhaseB = (long)AdcResult.ADCRESULT4 - adc_bias;
    system.adc_voltage.PhaseC = (long)AdcResult.ADCRESULT5 - adc_bias;

	//TODO: create some filter here
	system.current.PhaseA = _IQ15mpy(system.adc_current.PhaseA<<4, _IQ15(3.));
	system.current.PhaseB = _IQ15mpy(system.adc_current.PhaseB<<4, _IQ15(3.));
	system.current.PhaseC = _IQ15mpy(system.adc_current.PhaseC<<4, _IQ15(3.));

	system.voltage.PhaseA = _IQ15mpy(system.adc_voltage.PhaseA<<4, _IQ15(3.));
	system.voltage.PhaseB = _IQ15mpy(system.adc_voltage.PhaseB<<4, _IQ15(3.));
	system.voltage.PhaseC = _IQ15mpy(system.adc_voltage.PhaseC<<4, _IQ15(3.));

//	system.Udc = conv_adc_val_to_iq15(AdcResult.ADCRESULT6, _IQ15(3.));

	AdcRegs.ADCINTFLGCLR.bit.ADCINT1 = 1;	  //Clear ADCINT1 flag reinitialize for next SOC
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;   // Acknowledge interrupt to PIE

	GpioDataRegs.GPACLEAR.bit.GPIO0 = 1;
}

void measure_high_freq()
{
	system.rms_accumulation.PhaseA += system.adc_current.PhaseA * system.adc_current.PhaseA;
	system.rms_accumulation.PhaseB += system.adc_current.PhaseB * system.adc_current.PhaseB;
	system.rms_accumulation.PhaseC += system.adc_current.PhaseC * system.adc_current.PhaseC;

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
	long tmp;

	if (pwm.en_rms_calc && system.rms_samples_number_copy > 0)
	{
		tmp = sqrt(system.rms_copy.PhaseA / system.rms_samples_number_copy);
		system.rms_current.PhaseA = _IQ15mpy((long)tmp<<4, _IQ15(3.));

        tmp = sqrt(system.rms_copy.PhaseB / system.rms_samples_number_copy);
        system.rms_current.PhaseB = _IQ15mpy((long)tmp<<4, _IQ15(3.));

        tmp = sqrt(system.rms_copy.PhaseC / system.rms_samples_number_copy);
        system.rms_current.PhaseC= _IQ15mpy((long)tmp<<4, _IQ15(3.));
	}
}

