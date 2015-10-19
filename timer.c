/*
 * Copyright (c) 2007, Kalopa Research Limited.  All rights
 *  of the United States and/or the Republic of Ireland.
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
 * All the clock timer functionality is embedded in this file.
 */
#include <stdio.h>
#include <avr.h>

#include "otto.h"
#include "ioregs.h"

uchar_t				switch_state;
uchar_t				mission_switch;

volatile uchar_t	tick;
volatile ulong_t	boot_secs;
volatile uchar_t	have_second;

/*
 * Initialize the time of day clock system.
 */
void
clockinit()
{
	/*
	 * Initialize the time/date counters.
	 */
	tick = 200;
	boot_secs = 0;
	have_second = 0;
	switch_state = 0;
	mission_switch = 0;
	/*
	 * Set TIMER1 to free-run in NORMAL mode with a 1/64 divider
	 * for an I/O clock period of 4.000uS (16MHz crystal).  The
	 * divisor is then set for 1,250 to bring that down to 200 Hertz.
	 * Each clock hertz causes an OCF1 interrupt which we use to
	 * call the clocktick function.
	 */
	*(uint_t *)TCNT1 = 0;
	*(uint_t *)OCR1A = 1250;
	*(uchar_t *)TCCR1A = 0;
	*(uchar_t *)TCCR1B = (TCCR1B_WGM12|TCCR1B_CS11|TCCR1B_CS10);
	*(uchar_t *)TIMSK  = TIMSK_OCIE1A;
}

/*
 * Called from the interrupt service routine, every time a second
 * has elapsed.
 */
void
clocktick()
{
	uchar_t swstat;

	have_second = 1;
	boot_secs++;
	debug_vals[0]++;
	/*
	 * Anything to do for Mother?
	 */
	if (mother_timer > 0)
		mother_timer--;
	/*
	 * Sound the alarm.
	 */
	if (buzzer_state > 0)
		_buzzer(--buzzer_state & 01);
	/*
	 * Read the mission switch status.
	 */
	swstat = (_keysense() == 0)  ? 1 : 0;
	switch ((switch_state << 1) || swstat) {
		case 0: /* Switch OFF, state 0 */
		case 11: /* Switch ON, state 5 */
			/* Ignore the switch - nothing interesting */
			break;

		case 1: /* Switch ON, state 0 */
		case 3: /* Switch ON, state 1 */
		case 5: /* Switch ON, state 2 */
		case 7: /* Switch ON, state 3 */
		case 9: /* Switch ON, state 4 */
			/* Next state... */
			switch_state++;
			break;

		case 2: /* Switch OFF, state 1 */
		case 4: /* Switch OFF, state 2 */
			/* Switch is *1* and go to state 0 */
			mission_switch = 1;
			switch_state = 0;
			break;

		case 6: /* Switch OFF, state 3 */
		case 8: /* Switch OFF, state 4 */
			/* Switch is *2* and go to state 0 */
			mission_switch = 2;

		case 10: /* Switch OFF, state 5 */
			/* Switch held too long - ignore */
			switch_state = 0;
			break;
	}
}

/*
 *
 */
void
delay(int nsecs)
{
	while (nsecs-- > 0) {
		while (second_elapsed() == 0)
			;
		_watchdog();
	}
}

/*
 *
 */
uchar_t
second_elapsed()
{
	uchar_t ret = have_second;

	have_second = 0;
	return(ret);
}

/*
 * Report seconds since boot.
 */
void
report_boot_time()
{
	uint_t bsvals[2];

	bsvals[0] = (boot_secs >> 16) & 0xffff;
	bsvals[1] = boot_secs & 0xffff;
	reportv('b', 2, bsvals);
}
