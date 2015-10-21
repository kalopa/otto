/*
 * Copyright (c) 2007, Kalopa Research Limited.  All rights
 * reserved.  Unpublished rights reserved under the copyright laws
 * of the United States and/or the Republic of Ireland.
 *
 * The software contained herein is proprietary to and embodies the
 * confidential technology of Kalopa Research Limited.  Possession,
 * use, duplication or dissemination of the software and media is
 * authorized only pursuant to a valid written license from Kalopa
 * Research Limited.
 *
 * RESTRICTED RIGHTS LEGEND   Use, duplication, or disclosure by the
 * U.S.  Government is subject to restrictions as set forth in
 * Subparagraph (c)(1)(ii) of DFARS 252.227-7013, or in FAR 52.227-19,
 * as applicable.
 *
 * THIS SOFTWARE IS PROVIDED BY KALOPA RESEARCH LIMITED "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL KALOPA
 * RESEARCH LIMITED BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * ABSTRACT
 * This is the main code base.  Life begins here.  The locore.s
 * module will do the very basic initialization (stack, bss, etc)
 * and then call main.  After that, it's up to this file to initialize
 * all the hardware and get things moving.  It should never return.
 */
#include <stdio.h>
#include <avr.h>
#include <pid.h>

#include "otto.h"
#include "alarms.h"
#include "ioregs.h"

uchar_t		nav_mode;
uchar_t		switch_enable;
uchar_t		switch_copy;
uint_t		debug_vals[4];
int			pid_error;


struct	pid	wind_pid;
struct	pid	compass_pid;

/*
 *
 */
int
main()
{
	struct pid *pp = NULL;

	/*
	 * Do the initialization first...
	 */
	nav_mode = NAVMODE_SLEEP;
	switch_enable = 1;
	switch_copy = 0;
	debug_vals[0] = debug_vals[1] = debug_vals[2] = debug_vals[3] = 0;
	clear_alarms();
	clockinit();
	analoginit();
	serialinit();
	twiinit();
	windinit();
	compassinit();
	rudderinit();
	sailinit();
	/*
	 * Right. Let the good times roll...
	 */
	sei();
	(void )fdevopen(sio_putc, sio_getc);
	*(uchar_t *)MCUCSR = 0;
	printf("\nOtto!\n");
	calibrate(0);
	alarm(OTTO_RESTART);
	/*
	 * Start infinite loop.
	 */
	while (1) {
		/*
		 * Read the telemetry...
		 */
		wind_read();
		compass_read();
		voltage_read();
		/*
		 * Steer the boat, if need be...
		 */
		if (nav_mode == NAVMODE_AUTOWIND) {
			pp = &wind_pid;
			pid_error = (int )desired_twa - (int )actual_twa;
		} else if (nav_mode == NAVMODE_AUTOCOMPASS) {
			pp = &compass_pid;
			pid_error = (int )desired_mch - (int )actual_mch;
		} else {
			pp = NULL;
			pid_error = 0;
		}
		if (pid_error >= 128)
			pid_error -= 256;
		else if (pid_error <= -128)
			pid_error += 256;
		/*
		 * Recompute the rudder angle if there's an error
		 */
		if (nav_mode == NAVMODE_SLEEP) {
			/*
			 * Relax the stepper motors.
			 */
			 /* ::FIXME:: */
		} else {
			if (pp != NULL)
				rudder_adjust(pidcalc(pp, pid_error));
			sail_adjust(effective_twa());
		}
		/*
		 * Test the mission switch. If it's been flipped and the onboard
		 * compute hasn't told us to ignore that fact, then boot the
		 * main system. Also sound the buzzer. Save a copy so we don't
		 * keep doing this...
		 */
		if (mission_switch != switch_copy) {
			if (switch_enable)
				bootup();
			buzzer_state = mission_switch * 2;
			switch_copy = mission_switch;
		}
		/*
		 * Check for any alarms, and ping the watchdog so it doesn't bark
		 */
		alarm_check();
		_watchdog();
		/*
		 * Do we need to do anything with the onboard computer?
		 */
		if (mother_timer == 0)
			mother_timeout();
		debug_vals[1]++;
		/*
		 * Wait for a while before we try again...
		 */
		while (second_elapsed() == 0) {
			_watchdog();
			/*
			 * Check for serial I/O
			 */
			if (!sio_iqueue_empty())
				do_cmd_char();
		}
	}
}

/*
 * Perform a system calibration (system == 0 means ALL).
 */
void
calibrate(int system)
{
	int i;

	switch (system) {
		case 0:
			for (i = 1; i <= 5; i++)
				calibrate(i);
			break;

		case 1:
			/* EEPROM reload */
			eeprom_data_test();
			if (!eeprom_ok)
				break;
			analog_eeprom_data();
			eeprom_copy(EEPROM_WIND_PID, (void *)&wind_pid, sizeof(wind_pid));
			eeprom_copy(EEPROM_COMPASS_PID, (void *)&compass_pid, sizeof(compass_pid));
			break;

		case 2:
			rudder_calibrate();
			break;

		case 3:
			sail_calibrate();
			break;

		case 4:
			compass_calibrate();
			break;

		case 5:
			wind_calibrate();
			break;
	}
}
