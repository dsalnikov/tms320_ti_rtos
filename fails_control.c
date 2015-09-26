/*
 * fails_control.c
 *
 *  Created on: 22 сент. 2015 г.
 *      Author: salnikov
 */

#include "fails_control.h"


void fails_control()
{
	Uint16 num_fail = 0;



	if (system.pwm_on && num_fail != 0)
	{
		system.LastFault = num_fail;
	}
}
