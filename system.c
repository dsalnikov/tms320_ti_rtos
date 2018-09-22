/*
 * system.c
 *
 *  Created on: 14 марта 2015 г.
 *      Author: lamazavr
 */

#include "system.h"
#include "pwm.h"

System_t system;

void system_fsm()
{
	switch (system.state)
	{
		case init_state:
			system.Command = no_command;
			pwm_off();
			system.state = ready_state;
		break;


		case ready_state:
			if (system.Command == start_command)
			{
				pwm_on();
				system.state = running_state;
			}
			system.Command = no_command;
		break;


		case running_state:
			if (system.Command == stop_command)
			{
				pwm_off();
				system.state = init_state;
			}
			system.Command = no_command;
		break;


		case fail_state:
		default:
		    if (system.Command == reset_command)
		    {
		        system.state = init_state;
		    }

		break;
	}
}
