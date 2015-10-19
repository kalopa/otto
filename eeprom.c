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
 * This file provides mechanisms for accessing configuration data
 * from the system EEPROM.  It also provides a mechanism for updating
 * the EEPROM values via the serial port.
 */
#include <stdio.h>
#include <avr.h>

#include "otto.h"

uchar_t		eeprom_ok;
uint_t		eeprom_addr = 0;

/*
 *
 */
void
eeprom_data_test()
{
	eeprom_ok = (eeprom_rdint(EEPROM_MAGIC_ADDR) == EEPROM_MAGIC_CODE);
}

/*
 * Copy a block of data from the EEPROM to RAM.
 */
void
eeprom_copy(int srcaddr, void *dst, int len)
{
	char *dp = (char *)dst;

	while (len--)
		*dp++ = _rdeeprom(srcaddr++);

}

/*
 * Read an int (16 bits) from the EEPROM.
 */
uint_t
eeprom_rdint(int addr)
{
        uint_t val;

        val  = _rdeeprom(addr);
        val |= _rdeeprom(addr + 1) << 8;
        return(val);
}

/*
 * Write an int (16 bits) to the EEPROM.
 */
void
eeprom_wrint(int addr, uint_t val)
{
        _wreeprom(addr, val & 0xff);
        _wreeprom(addr + 1, (val >> 8) & 0xff);
}

/*
 *
 */
uint_t
eeprom_getaddr()
{
	return(eeprom_addr);
}

/*
 *
 */
void
eeprom_setaddr(uint_t val)
{
	eeprom_addr = val;
}

/*
 *
 */
uint_t
eeprom_read()
{
	return(_rdeeprom(eeprom_addr++));
}

/*
 *
 */
void
eeprom_write(uint_t val)
{
	_wreeprom(eeprom_addr++, val & 0xff);
}
