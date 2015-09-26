/*
 * pwm.h
 *
 *  Created on: 17 авг. 2014 г.
 *      Author: lamazavr
 */

#ifndef PWM_H_
#define PWM_H_

#include "DSP2802x_Device.h"
#include "DSP2802x_EPwm_defines.h"

#include "iqmath/include/IQmathLib.h"

#include "system.h"

//timer period on 60MHz for 4kHz PWM
#define PWM_TIMER_PRD 15000

/**
 * Настройка выводов GPIO0-5 как PWM
 */
void pwm_gpio_init();

void pwm_init();

void pwm1_init();
void pwm2_init();
void pwm3_init();

void pwm1_dead_band_configure();

static volatile Uint16 duty = 0;

void pwm_debug();

void pwm1_interrupt_init();

void pwm_it_enable();
void pwm_it_disable();

void pwm_on();
void pwm_off();

interrupt void epwm1_timer_isr();

typedef struct {
	Uint16 pwm_freq;
	_iq pwm_period;
	Uint16 pwm_counter_max;

	_iq angle_step;
	_iq angle;

	ThreePhase_t t;

	//прошел период выходного напряжения
	Uint16 period_out;
	//разрешение расчета rms
	Uint16 en_rms_calc;
} pwm_t;

extern pwm_t pwm;


#endif /* PWM_H_ */
