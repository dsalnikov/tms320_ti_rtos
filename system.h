/*
 * system.h
 *
 *  Created on: 14 марта 2015 г.
 *      Author: lamazavr
 */

#ifndef SYSTEM_H_
#define SYSTEM_H_

#include "DSP2802x_Device.h"
#include "iqmath/include/IQmathLib.h"

typedef struct {
	Uint16 PhaseA;
	Uint16 PhaseB;
	Uint16 PhaseC;
} ThreePhase_t;

typedef struct {
	_iq PhaseA;
	_iq PhaseB;
	_iq PhaseC;
} IQThreePhase_t;

typedef struct {
	int32 PhaseA;
	int32 PhaseB;
	int32 PhaseC;
} Int32ThreePhase_t;

typedef struct {
	Uint16 voltage;
	Uint16 current;
	Uint16 frequency;
} Bases_t;

#define UF_CHARACT_LEN 2
typedef struct {
	_iq U[UF_CHARACT_LEN];
	_iq F[UF_CHARACT_LEN];
} UFCharacteristics_t;

typedef enum {
	init_state = 0,
	error_state,
	ready_state,
	running_state,
} System_state_t;

typedef enum {
	idle_command = 0,
	start_command = 1,
	stop_command = 2,
} System_command_t;

typedef struct {
	Bases_t bases;
	//задание по частоте и напряжению
	_iq Fref;
	_iq Uref;
	//текущее задание
	_iq Fout;
	_iq Uout;

	//время разгона
	_iq Tacc;

	System_command_t Command;
	Uint16 Status;
	Uint16 LastFault;
	_iq Udc;
	IQThreePhase_t current;
	IQThreePhase_t voltage;

	Int32ThreePhase_t rms_accumulation;
	Int32ThreePhase_t rms_copy;
	Uint16 rms_samples_number;
	Uint16 rms_samples_number_copy;

	ThreePhase_t rms_current;

	Uint16 pwm_on;

	UFCharacteristics_t uf;

	System_state_t state;
} System_t;

extern System_t system;




void system_fsm();

#endif /* SYSTEM_H_ */
