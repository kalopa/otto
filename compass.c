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
 */
#include <stdio.h>
#include <avr.h>

#include "otto.h"
#include "ioregs.h"

/*
 * LSM303 Register definitions
 */
#define CRA_REG_M	0x00
#define CRB_REG_M	0x01
#define MR_REG_M	0x02
#define OUT_X_H_M	0x03
#define OUT_X_L_M	0x04
#define OUT_Y_H_M	0x05
#define OUT_Y_L_M	0x06
#define OUT_Z_H_M	0x07
#define OUT_Z_L_M	0x08
#define SR_REG_M	0x09
#define IRA_REG_M	0x0A
#define IRB_REG_M	0x0B
#define IRC_REG_M	0x0C

#define CTRL_REG1_A	0x20
#define CTRL_REG2_A	0x21
#define CTRL_REG3_A	0x22
#define CTRL_REG4_A	0x23
#define CTRL_REG5_A	0x24
#define FILTER_RESET_A	0x25
#define REFERENCE_A	0x26
#define STATUS_REG_A	0x27
#define OUT_X_L_A	0x28
#define OUT_X_H_A	0x29
#define OUT_Y_L_A	0x2A
#define OUT_Y_H_A	0x2B
#define OUT_Z_L_A	0x2C
#define OUT_Z_H_A	0x2D
#define INT1_CFG_A	0x30
#define INT1_SOURCE_A	0x31
#define INT1_THS_A	0x32
#define INT1_DURATION_A	0x33

uchar_t					desired_mch;
volatile uchar_t		actual_mch;

/*
 *
 */
void
compassinit()
{
	desired_mch = 0;
	actual_mch = 0;
	/* ::FIXME:: */
}

/*
 * Read a value from the onboard LSM303DLH compass module from ST. The
 * device is mounted on a SparkFun breakout board (SEN-10703) and accessed
 * via the TWI interface. The device features a 3-axis accelerometer and a
 * 3-axis magnetometer.
 *
 * This function reads the various parameters and computes the
 * tilt-compensated magnetic compass heading. As a side-effect it also stores
 * the accelerometer settings.
 */
void
compass_read()
{
	/* ::FIXME:: */
	actual_mch = 180;
}

/*
 * Perform a compass calibration operation.
 */
void
compass_calibrate()
{
	printf("Ccal\n");
	/* ::FIXME:: */
}
