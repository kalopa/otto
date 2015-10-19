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
 * RESTRICTED RIGHTS LEGEND  Use, duplication, or disclosure by the
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
 * The main include file.
 */
#define REVISION				1

/*
 * EEPROM addresses
 */
#define EEPROM_MAGIC_ADDR		0
#define EEPROM_BATT_CRITICAL	2
#define EEPROM_BATT_LOW			4
#define EEPROM_BATT_HIGH		6
#define EEPROM_SOLAR_HIGH		8
#define EEPROM_WIND_PID			10
#define EEPROM_COMPASS_PID		28

#define EEPROM_MAGIC_CODE		0x5aa5

/*
 * From the Evernote note on mission state:
 *
 * Boat is in a SLEEP (0) state. Flip the MISSION switch once and it
 * goes into a TEST state. After the test is completed, it drops back
 * into a SLEEP state.
 *
 * Flipping the switch twice in one second (from a SLEEP state) will put
 * it into a MISSION PREFLIGHT state. This involves checking all the
 * systems, running the TEST suite successfully, verifying that there
 * is a route and that the navigation is all functioning and happy. Once
 * this has completed, a status (preflight-GO) message is sent to Mission
 * Status. It then moves to a MISSION WAIT status.  Externally, Mission
 * Status verifies the data sent, including the correct mission number,
 * and issues a MISSION-GO text (SMS and/or SLACK) message to the Launch
 * Operator.
 *
 * Flipping the switch twice in a MISSION WAIT status moves the boat to
 * a MISSION ACTIVE state after a ten second delay (to allow the Launch
 * Operator to get clear of the boat. The boat will then begin navigating
 * to its first waypoint.
 *
 * From MISSION ACTIVE, the boat can move to MISSION COMPLETED or MISSION
 * ABORTED depending on what happens.
 *
 * Either of these can be moved to MISSION_SLEEP if the mission switch
 * is flipped.
 */
#define MISSION_SLEEP		0
#define MISSION_TEST		1
#define MISSION_PREFLIGHT	2
#define MISSION_WAIT		3
#define MISSION_ACTIVE		4
#define MISSION_ABORTED		5
#define MISSION_COMPLETED	6

/*
 * The lower-level software operates in one of five modes. SLEEP means
 * that it has nothing to do, it should idle or disable the steppers
 * and relax.  REMOTE mode means the boat takes its direction from an RC
 * radio input. It can only be put in this mode by flipping the switch
 * four times in quick succession. All other modes are modified by the
 * onboard main computer.  In MANUAL mode, Otto takes direction from
 * the main computer. This is used, for example, when testing or sailing
 * via the WiFi link. AUTOWIND mode means that the PID controllers are
 * set based on True Wind Angle. AUTOCOMPASS means the boat is steered
 * (and the PID controllers are set) based on the compass reading.
 */
#define NAVMODE_SLEEP		0		/* Test the systems */
#define NAVMODE_REMOTE		1		/* Sail by remote control */
#define NAVMODE_MANUAL		2		/* Sail manually */
#define NAVMODE_AUTOWIND	3		/* Sail to the wind */
#define NAVMODE_AUTOCOMPASS	4		/* Sail to the compass */

/*
 * Power status.
 */
#define POWER_FAIL			0		/* Battery dead - shut down */
#define POWER_LOW			1		/* Battery low - don't boot */
#define POWER_GOOD			2		/* Battery good */

/*
 * Status of the onboard computer.
 */
#define MOTHER_DOWN			0
#define MOTHER_BOOTING		1
#define MOTHER_UP			2
#define MOTHER_SHUTDOWN		3

/*
 * Desired-TWA is the true wind angle we've been asked to sail to. Desired-MCH
 * is the desired magnetic compass heading. Actual-TWA and Actual-MCH are from
 * the readings we've taken.  We use 'nav_state' to remember what we're
 * supposed to be doing, and 'alarm' is a bit-mask of faults.
 */
extern uchar_t	eeprom_ok;

extern uchar_t	desired_twa;
extern uchar_t	desired_mch;

extern uchar_t	power_status;
extern uchar_t	buzzer_state;
extern uchar_t	nav_mode;
extern uchar_t	mission_switch;
extern uchar_t	switch_enable;

extern uchar_t	power_state;
extern uchar_t	mother_state;
extern uchar_t	serial_state;
extern uint_t	alarm_state;

extern int		pid_error;
extern uint_t	debug_vals[];

extern uint_t	mother_timer;
extern uchar_t	next_boot;

extern volatile int			cur_rudder;
extern volatile int			cur_sail;
extern volatile uchar_t		actual_twa;
extern volatile uchar_t		actual_mch;

/*
 * Prototypes...
 */
void	calibrate(int);
void	rudder_calibrate();
void	sail_calibrate();
void	compass_calibrate();
void	wind_calibrate();
void	analog_eeprom_data();
void	clear_alarms();
void	clockinit();
void	analoginit();
void	serialinit();
void	twiinit();
void	windinit();
void	compassinit();
void	rudderinit();
void	sailinit();
void	eeprom_data_test();
void	eeprom_copy(int, void *, int);
uint_t	eeprom_rdint(int);
void	eeprom_wrint(int, uint_t);
uint_t	eeprom_getaddr();
void	eeprom_setaddr(uint_t);
uint_t	eeprom_read();
void	eeprom_write(uint_t);

void	alarm_check();
void	alarm(int);
void	clocktick();
void	wind_read();
void	compass_read();
void	voltage_read();
uchar_t	effective_twa();
void	rudder_adjust(char);
void	sail_adjust(uchar_t);
void	rudder_setpos();
void	sail_setpos();
void	do_cmd_char();
void	delay(int);
uchar_t	second_elapsed();

void	bootup();
void	shutdown(uchar_t);
void	mother_timeout();

void	report1(uchar_t, uint_t);
void	reportv(uchar_t, int, uint_t[]);
void	report_boot_time();
void	report_voltages();

/*
 * These are implemented in assembly language in locore.s
 */
void	_watchdog();
void	_enaint();
int		_disint();
void	_setint(uchar_t);
void	_sleep();
void	_reset();

uchar_t	_rudder_home();
void	_rudder_inc();
void	_rudder_dec();
uchar_t	_sail_home();
void	_sail_inc();
void	_sail_dec();
void	_servo_reset();

uchar_t _keysense();
void	_buzzer(uchar_t);
void	_navlight(uchar_t);
void	_cpupwr(uchar_t);
