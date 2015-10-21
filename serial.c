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
 * All serial I/O happens here.
 */
#include <stdio.h>
#include <avr.h>
#include <ctype.h>

#include "otto.h"
#include "ioregs.h"

#define BAUDRATE	103			/* 9600 baud @ 16Mhz */

uchar_t		serial_state = 0;
const char	*hello_mother = "PLUGH!";

/*
 *
 */
void
serialinit()
{
	/*
	 * Set baud rate.
	 */
	*(uchar_t *)UCSRC = 0;
	*(uchar_t *)UBRRL = BAUDRATE;
	/*
	 * Configure the USART.
	 */
	*(uchar_t *)UCSRB = (UCSRB_RXCIE|UCSRB_UDRIE|UCSRB_RXEN|UCSRB_TXEN);
	*(uchar_t *)UCSRC = (UCSRC_URSEL|UCSRC_UCSZ1|UCSRC_UCSZ0);
}

/*
 *
 */
void
run_command(char cmd, int val)
{
	switch (cmd) {
		case 'A':
			/* Clear all known alarms */
			clear_alarms();
			break;

		case 'a':
			/* Report alarm status */
			report1('a', alarm_state);
			break;

		case 'B':
			/* Request orderly shutdown for 'n' minutes */
			shutdown(val, 0);
			break;

		case 'b':
			/* Return seconds since last Otto reboot */
			report_boot_time();
			break;

		case 'C':
			/* Set desired compass heading */
			desired_mch = val;

		case 'c':
			/* Report actual compass heading */
			report1('c', actual_mch);
			break;

		case 'd':
			/* Return the debug value */
			reportv('d', 4, debug_vals);
			break;

		case 'e':
			/* Return most recent PID error */
			report1('e', pid_error);
			break;

		case 'L':
			/* Set navigation light on/off */
			_navlight(val);
			break;

		case 'M':
			/* Enable/disable the mission switch */
			switch_enable = (val != 0);

		case 'm':
			/* Query the mission switch status */
			report1('m', mission_switch);
			mission_switch = 0;
			break;

		case 'N':
			/* Set the navigation mode */
			if (eeprom_ok)
				nav_mode = val;

		case 'n':
			/* Query the navigation mode */
			report1('n', nav_mode);
			break;

		case 'O':
			/* Write the value to EEPROM */
			eeprom_write(val);
			break;

		case 'o':
			/* Read the value from the EEPROM */
			report1('o', eeprom_read());
			break;

		case 'P':
			/* Set the EEPROM current address */
			eeprom_setaddr(val);
			break;

		case 'p':
			/* Return the EEPROM current address */
			report1('p', eeprom_getaddr());
			break;

		case 'Q':
			/*
			 * ::FIXME::
			 * Debugging way to set the actual TWA
			 */
			actual_twa = val;
			break;

		case 'R':
			/* Set the required rudder angle */
			rudder_adjust(val);

		case 'r':
			/* Query the current rudder position */
			report1('r', cur_rudder);
			break;

		case 'S':
			/* Set the required sail angle */
			sail_adjust(val);

		case 's':
			/* Query the current sail angle */
			report1('s', cur_sail);
			break;

		case 'T':
			/* Set the desired TWA */
			desired_twa = val;

		case 't':
			/* Query the actual TWA */
			report1('t', actual_twa);
			break;

		case 'v':
			/* Report all (last) voltage readings */
			report_voltages();
			break;

		case 'W':
			/* Watchdog - keep the computer alive a bit longer */
			mother_timer = val;
			break;

		case 'X':
			/* Perform a calibration suite */
			calibrate(val);
			break;

		case 'Z':
			/*
			 * This is not for the faint-hearted...
			 * Reset ourselves and start again.
			 */
			if (val == 999)
				_reset();
			break;

		default:
			printf("?E\n");
			break;
	}
}

/*
 * Report back one argument
 */
void
report1(uchar_t cmd, uint_t val)
{
	uint_t vals[2];

	vals[0] = val;
	reportv(cmd, 1, vals);
}

/*
 *
 */
void
reportv(uchar_t cmd, int nargs, uint_t args[])
{
	int i;
	putchar('>');
	putchar (cmd);
	for (i = 0; i < nargs; i++)
		printf(".%d", args[i]);
	putchar('\n');
}

/*
 *
 */
void
do_cmd_char()
{
	char ch;
	static uchar_t cmd = 0;
	static uint_t val = 0;


	/*
	 * Get the character - ignore it if we weren't expecting it.
	 */
	if ((ch = getchar()) == 0)
		return;
	/*
	 * Are we waiting for Mother to say hello?
	 */
	if (mother_state == MOTHER_BOOTING) {
#if 0
		if (hello_mother[serial_state] == ch) {
			serial_state++;
			if (hello_mother[serial_state] == '\0') {
				/*
				 * Yay! The onboard computer is up and
				 * running. Mark it as such, and wait
				 * for the next CR/LF.
				 */
				putchar('!');
				mother_state = MOTHER_UP;
				mother_timer = 300;
				serial_state = 3;
			}
		} else
			serial_state = 0;
#else
		mother_state = MOTHER_UP;
		mother_timer = 3600;
#endif
		return;
	}
	/*
	 * If we're not up then we don't care about the serial data.
	 */
	if (mother_state != MOTHER_UP)
		return;
	if (ch == '\r' || ch == '\n') {
		if (serial_state == 1)
			run_command(cmd, val);
		else if (serial_state == 2)
			run_command(0, 0);
		serial_state = 0;
	} else if (serial_state == 1 && isdigit(ch))
		val = val * 10 + ch - '0';
	else {
		if (serial_state != 0)
			serial_state = 2;
		else {
			cmd = ch;
			val = 0;
			serial_state = 1;
		}
	}
}
