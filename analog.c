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

uchar_t	admux;
uchar_t	adcsra;

uchar_t		power_state;
uint_t		battery_critical;
uint_t		battery_low;
uint_t		battery_high;
uint_t		solar_high;
uint_t		voltage[4];

/*
 *
 */
void
analoginit()
{
	admux = (ADMUX_REFS0|ADMUX_ADLAR);
	adcsra = (ADCSRA_ADEN|ADCSRA_ADPS2|ADCSRA_ADPS1|ADCSRA_ADPS0);
	*(uchar_t *)ADMUX = admux;
	*(uchar_t *)ADCSRA = adcsra;
	/*
	 * Set the power status until we know more...
	 */
	power_state = POWER_FAIL;
}

/*
 *
 */
void
analog_eeprom_data()
{
	/*
	 * Pull these from the EEPROM, if it's been programmed. Otherwise set
	 * very high/low defaults so we don't create spurious alarms.
	 */
	if (eeprom_ok) {
		battery_critical = eeprom_rdint(EEPROM_BATT_CRITICAL);
		battery_low = eeprom_rdint(EEPROM_BATT_LOW);
		battery_high = eeprom_rdint(EEPROM_BATT_HIGH);
		solar_high = eeprom_rdint(EEPROM_SOLAR_HIGH);
	} else {
		battery_critical = battery_low = 0;
		battery_high = solar_high = 0x3ff;
	}
}

/*
 *
 */
void
voltage_read()
{
	int i;

	for (i = 0; i < 4; i++)
		voltage[i] = analog_read(i);
	/*
	 * Raise the alarm, if necessary.
	 */
	if (voltage[0] < battery_critical) {
		power_state = POWER_FAIL;
		alarm(VBATT_CRITICAL);
	} else if (voltage[0] < battery_low) {
		power_state = POWER_LOW;
		alarm(VBATT_UNDERVOLTAGE);
	} else {
		power_state = POWER_GOOD;
		if (voltage[0] > battery_high)
			alarm(VBATT_OVERVOLTAGE);
	}
	if (voltage[3] > solar_high)
		alarm(VSOLAR_OVERVOLTAGE);
	/*
	 * If power is bad, turn off unnecessary systems.
	 * We're in trouble, here. We have absolutely no power reserves.
	 * Shut off everything and reduce power consumption until things
	 * get better - if they do... :(
	 */
	if (power_state == POWER_FAIL) {
		_buzzer(0);
		_navlight(0);
		shutdown(240);
		mother_timeout();
	}
}

/*
 *
 */
void
report_voltages()
{
	reportv('v', 4, voltage);
}
