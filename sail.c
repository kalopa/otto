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

#define SAIL_MINIMUM	-2000

int				req_sail;
volatile int	cur_sail;

/*
 *
 */
void
sailinit()
{
	req_sail = 0;
	cur_sail = 0;
}

/*
 * Determine the correct sail trim, given a TWA. Express the required
 * sail angle on a scale of 0 (fully sheeted in) to 100 (fully sheeted
 * out).
 */
void
sail_adjust(uchar_t twa)
{
	static uchar_t saved_twa = 0;

	if (saved_twa == twa)
		return;
	saved_twa = twa;
	if (twa > 128)
		twa = -twa;
	if (twa <= 32)
		req_sail = 0;
	else {
		twa -= 32;
		req_sail = twa * 25 / 24;
	}
	printf("ReqS:%d\n", req_sail);
}

/*
 * Generate a sail servo step if the sail isn't where we want it.
 * Called by the timer interrupt service routine so be careful what
 * you do, here.
 */
void
sail_setpos()
{
	int n;

	if (req_sail == 0) {
		if (_sail_home()) {
			if (cur_sail < SAIL_MINIMUM)
				alarm_state |= (1 << SAIL_NOZERO);
			else
				_sail_dec();
		} else
			cur_sail = 0;
	} else if ((n = (req_sail - cur_sail)) != 0) {
		if (n > 0)
			_sail_inc();
		else
			_sail_dec();
	}
}

/*
 * Perform a sail calibration operation. 
 */
void
sail_calibrate()
{
	printf("Scal\n");
	/* ::FIXME:: */
}
