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
 */
#include <stdio.h>
#include <avr.h>

#include "otto.h"
#include "alarms.h"

uint_t		alarm_state;
uint_t		alarm_state_copy;
uchar_t		buzzer_state;

/*
 *
 */
void
clear_alarms()
{
	alarm_state = alarm_state_copy = 0;
	_buzzer(buzzer_state = 0);
}

/*
 *
 */
void
alarm(int n)
{
	alarm_state |= (1 << (n & 0xf));
}

/*
 * Check if any alarms have fired since last time.
 */
void
alarm_check()
{
	/*
	 * If power is bad, turn off unnecessary systems.
	 * We're in trouble, here. We have absolutely no power reserves.
	 * Shut off everything and reduce power consumption until things
	 * get better - if they do... :(
	 */
	if (power_state == POWER_FAIL) {
		_buzzer(0);
		_navlight(0);
		shutdown(240, 1);
	}
	if (alarm_state != alarm_state_copy) {
		printf("NA:%o\n", alarm_state);
		if (power_state != POWER_FAIL) {
			if (buzzer_state == 0)
				buzzer_state = 8;
			bootup();
		}
	}
	alarm_state_copy = alarm_state;
}
