/*
 * system.h
 *
 *  Created on: 14 марта 2015 г.
 *      Author: lamazavr
 */

#ifndef SYSTEM_H_
#define SYSTEM_H_

#include "DSP2802x_Device.h"

typedef struct {
	Uint16 PhaseA;
	Uint16 PhaseB;
	Uint16 PhaseC;
} ThreePhase_t;

typedef struct {
	Uint16 FreqSetting;
	Uint16 VoltageSetting;
	Uint16 Status;
	Uint16 LastFault;
	Uint16 InputVoltage;
	ThreePhase_t current;
	ThreePhase_t voltage;
} System_t;

extern System_t system;

#endif /* SYSTEM_H_ */
