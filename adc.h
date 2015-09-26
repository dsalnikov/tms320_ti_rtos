/*
 * adc.h
 *
 *  Created on: 14 марта 2015 г.
 *      Author: lamazavr
 */

#ifndef ADC_H_
#define ADC_H_

#include <xdc/cfg/global.h>


void init_adc();

void adc_isr(UArg arg);

void InitAdc(void);


void measure_high_freq();
void calculate_rms();


#endif /* ADC_H_ */
