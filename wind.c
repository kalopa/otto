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

uchar_t		admux;
uchar_t		adcsra;

uchar_t					desired_twa;
volatile uchar_t		actual_twa;

/*
 *
 */
void
windinit()
{
	admux = (ADMUX_REFS0|ADMUX_ADLAR);
	adcsra = (ADCSRA_ADEN|ADCSRA_ADPS2|ADCSRA_ADPS1|ADCSRA_ADPS0);
	*(uchar_t *)ADMUX = admux;
	*(uchar_t *)ADCSRA = adcsra;
	desired_twa = 0;
	actual_twa = 0;
}

/*
 * Read the two wind instrument potentiometers and compute the true
 * wind angle from this. We're only interested in a 256-degree circle.
 */
void
wind_read()
{
	/* ::FIXME:: */
	actual_twa = 60;
}

/*
 * We don't set the sails to the required or the actual TWA. We adjust
 * for a theoretical point between the two. This is to avoid two
 * issues. Firstly, if we are close-hauled but we really want to sail
 * downwind, the boat won't bear away if we're fully sheeted-in. So
 * we can't use the actual TWA. At the same time, if we let the sails
 * fully out then we may lose so much speed that we can't steer the boat
 * to the required course so the required TWA doesn't help either. We
 * split the difference. We set the sails half-way between the actual
 * and required TWA.
 */
uchar_t
effective_twa()
{
	int eff_twa;

	eff_twa = ((short )desired_twa + (short )actual_twa) / 2;
	return((uchar_t )eff_twa & 0xff);
}

/*
 * Perform a wind calibration operation.
 */
void
wind_calibrate()
{
	printf("Commencing wind calibration...\n");
	/* ::FIXME:: */
}