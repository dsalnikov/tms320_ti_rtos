/*
 * scalar.h
 *
 *  Created on: 21 сент. 2015 г.
 *      Author: salnikov
 */

#ifndef SCALAR_H_
#define SCALAR_H_

#include "svgen_dq.h"
#include "system.h"
#include "pwm.h"

#define K_MOD_SPACEPWM      1.4142135623730950488016887242097F

extern SVGENDQ svgen_dq;

void ScalarControl(void);
void rate_generator();
void uf_characteristics();

#endif /* SCALAR_H_ */
