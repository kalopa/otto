/*
 * Copyright (c) 2013, Kalopa Research Limited.  All rights
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
 * All TWI (I2C) I/O happens here.
 */
#include <stdio.h>
#include <avr.h>

#include "otto.h"
#include "ioregs.h"

#define LINACC_READ	0x31
#define LINACC_WRITE	0x30
#define COMPASS_READ	0x3d
#define COMPASS_WRITE	0x3c

volatile uchar_t	*twcr;
volatile uchar_t	*twsr;
volatile uchar_t	*twdr;

int	twi_sendcmd(uchar_t, uchar_t);

/*
 *
 */
void
twiinit()
{
	/*
	 * Configure the I2C registers...
	 * We're operating at 20KHz, which means given a clock
	 * frequency of 16MHz, the divisor is 49 and the
	 * prescaler is 16.
	 */
	twcr = (uchar_t *)TWCR;
	twsr = (uchar_t *)TWSR;
	twdr = (uchar_t *)TWDR;
	*(uchar_t *)TWBR = 49;
	*twcr = TWCR_TWEN;
	*twsr = TWSR_TWPS1;
}
/*
 *
 */
void
twi_read()
{
#if 0
	int i;
	uchar_t status;

	/* ::FIXME:: */
	status = *twsr & 0xf8;
	if (twi_sendcmd(TWCR_TWSTA, 0x08) < 0)
		return;
	*twdr = COMPASS_WRITE;
	if (twi_sendcmd(0, 0x18) < 0)
		return;
	*twdr = 0;	/* Sub-address */
	if (twi_sendcmd(0, 0x28) < 0)
		return;
	/*
	 * Now do the register read.
	 */
	if (twi_sendcmd(TWCR_TWSTA, 0x10) < 0)
		return;
	*twdr = COMPASS_READ;
	for (i = 0; i < 12; i++) {
		*twcr = (TWCR_TWINT|TWCR_TWEA|TWCR_TWEN);
		while ((*twcr & TWCR_TWINT) == 0)
			;
		status = *twsr & 0xf8;
	}
	/*
	 * Stop...
	 */
	*twcr = (TWCR_TWINT|TWCR_TWSTO|TWCR_TWEN);
	putchar('!');
#endif
}

/*
 *
 */
int
twi_sendcmd(uchar_t cmd, uchar_t resp)
{
	uchar_t status;

	/* ::FIXME:: */
	*twcr = (cmd|TWCR_TWINT|TWCR_TWEN);
	while ((*twcr & TWCR_TWINT) == 0)
		;
	status = *twsr & 0xf8;
	if (status != resp) {
		*twcr = (TWCR_TWINT|TWCR_TWSTO|TWCR_TWEN);
		return(-1);
	}
	return(0);
}
