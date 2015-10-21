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
#include "ioregs.h"
#include "alarms.h"

#define RUDDER_MINIMUM		-400
#define RUDDER_OFFSET		100

int				req_rudder;
volatile int	cur_rudder;

/*
 *
 */
void
rudderinit()
{
	req_rudder = 0;
	cur_rudder = 0;
}

/*
 * Set the rudder angle according to a set value. The value is in the
 * range of -45 to +45 degrees of rudder position.
 */
void
rudder_adjust(char angle)
{
	static char saved_angle;

	if (saved_angle == angle)
		return;
	saved_angle = angle;
	if (angle > 40)
		angle = 40;
	else if (angle < -40)
		angle = -40;
	req_rudder = RUDDER_OFFSET + angle * 20 / 9;
	printf("ReqR:%d\n", req_rudder);
}

/*
 * Generate a rudder servo step if the rudder isn't where we want it.
 * Called by the timer interrupt service routine so be careful what
 * you do, here.
 */
void
rudder_setpos()
{
	int n;

	if (req_rudder == 0) {
		if (_rudder_home()) {
			if (cur_rudder < RUDDER_MINIMUM)
				alarm_state |= (1 << RUDDER_NOZERO);
			else
				_rudder_dec();
		} else
			cur_rudder = 0;
	} else if ((n = (req_rudder - cur_rudder)) != 0) {
		if (n > 0)
			_rudder_inc();
		else
			_rudder_dec();
	}
}

/*
 * Perform a rudder calibration operation.
 */
void
rudder_calibrate()
{
	printf("Rcal\n");
	/* ::FIXME:: */
}
