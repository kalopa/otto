/*
 * Copyright (c) 2014, Kalopa Research.  All rights reserved.  This is free
 * software; you can redistribute it and/or modify it under the terms of the
 * GNU General Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * It is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this product; see the file COPYING.  If not, write to the Free
 * Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * THIS SOFTWARE IS PROVIDED BY KALOPA RESEARCH "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL KALOPA RESEARCH BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
 * USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * ABSTRACT
 * This is the control machine for Mother, the nanoBSD board. Functions
 * in here are used to boot up and shut down the system, depending on
 * the state.
 */
#include <stdio.h>
#include <avr.h>
#include <ctype.h>

#include "otto.h"

uchar_t		next_boot = 0;
uchar_t		mother_state = 0;
uint_t		mother_timer = 0;

void		mother_timeout();
void		run_command(char, int);

/*
 * Bring up the onboard computer.
 */
void
bootup()
{
	printf("BU\n");
	if (mother_state == MOTHER_DOWN)
		mother_timeout();
	else if (mother_timer < 120)
		mother_timer = 120;
}

/*
 * Orderly shutdown of the onboard computer. The passed-in parameter specifies
 * how many minutes until the next reboot. After this command is executed,
 * generally you have about 30 seconds to shut down. After that, the power
 * plug will be removed. This will automatically happen unless a 'W' command
 * has been issued. The 'W' command resets the countdown timer.
 */
void
shutdown(uchar_t next, uchar_t force)
{
	next_boot = next;
	printf("SD:%d\n", next);
	if (mother_state != MOTHER_DOWN) {
		if (force)
			mother_state = MOTHER_SHUTDOWN;
		mother_timeout();
	}
}

/*
 * Call this routine when the clock has run down. We can kick this timeout
 * when/if we need to.
 */
void
mother_timeout()
{
	printf("M.to: s%d\n", mother_state);
	switch (mother_state) {
	case MOTHER_DOWN:
		/*
		 * Time to boot up. We have 60 seconds to hear from the
		 * main computer, otherwise we've failed.
		 */
		if (power_state != POWER_FAIL) {
			printf("ON\n");
			_cpupwr(1);
			_setled(1);	/* ::FIXME:: */
			mother_state = MOTHER_BOOTING;
		}
		serial_state = 0;
		mother_timer = 60;
		break;

	case MOTHER_BOOTING:
		next_boot = 5;

	case MOTHER_SHUTDOWN:
		/*
		 * We have timed out while waiting for the CPU to talk to
		 * us or else we've run out of time. We'll wait either
		 * five minutes (timed out) or the indicated amount if
		 * an orderly shutdown.
		 */
		printf("OFF\n");
		_cpupwr(0);
		_setled(0);	/* ::FIXME:: */
		mother_state = MOTHER_DOWN;
		mother_timer = (uint_t )next_boot * 60;
		break;

	case MOTHER_UP:
		/*
		 * OK, time to think about shutting down. We've run out
		 * of time.
		 */
		mother_state = MOTHER_SHUTDOWN;
		mother_timer = 30;
		break;
	}
}
