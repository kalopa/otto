#
# Copyright (c) 2007, Kalopa Research Limited.  All rights
# reserved.  Unpublished rights reserved under the copyright laws
# of the United States and/or the Republic of Ireland.
#
# The software contained herein is proprietary to and embodies the
# confidential technology of Kalopa Research Limited.  Possession,
# use, duplication or dissemination of the software and media is
# authorized only pursuant to a valid written license from Kalopa
# Research Limited.
#
# RESTRICTED RIGHTS LEGEND   Use, duplication, or disclosure by the
# U.S.  Government is subject to restrictions as set forth in
# Subparagraph (c)(1)(ii) of DFARS 252.227-7013, or in FAR 52.227-19,
# as applicable.
#
# THIS SOFTWARE IS PROVIDED BY KALOPA RESEARCH LIMITED "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
# TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
# PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL KALOPA
# RESEARCH LIMITED BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
# OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
# STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
# ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
# ABSTRACT
#
DEVICE=	atmega8
ASRCS=	locore.s
CSRCS=	main.c mother.c wind.c compass.c rudder.c sail.c \
	alarm.c timer.c serial.c analog.c twi.c eeprom.c
BIN=	otto

MOTHER=../mother

AVR?=../libavr
IOREGS=$(AVR)/ioregs.h
REGVALS=$(AVR)/regvals.inc
STDPROM=$(AVR)/stdprom.x

DEVICE?=atmega8
PROG?=usbtiny
OBJS?=	$(ASRCS:.s=.o) $(CSRCS:.c=.o)

AR=avr-ar
AS=avr-as
CC=avr-gcc
C++=avr-c++
G++=avr-g++
GCC=avr-gcc
LD=avr-ld
NM=avr-nm
OBJDUMP=avr-objdump
RANLIB=avr-ranlib
STRIP=avr-strip
AVR_PROG=sudo avrdude

FIRMWARE=otto.hex
EEPROM_DATA=eeprom.hex

LFUSE=0xef
HFUSE=0xcd

LC_COLLATE=C

ASFLAGS= -mmcu=$(DEVICE) -I$(AVR)
CFLAGS=	-Wall -O2 -mmcu=$(DEVICE) -I$(AVR)
LDFLAGS=-nostartfiles -L$(AVR)
LIBS=	-lavr

all:	$(BIN)

clean:
	rm -f $(BIN) $(OBJS) $(FIRMWARE) alarms.h

program: $(FIRMWARE) $(EEPROM_DATA)
	$(AVR_PROG) -p $(DEVICE) -c $(PROG) -U flash:w:$(FIRMWARE):i -U eeprom:w:$(EEPROM_DATA):i
	rm $(FIRMWARE) $(EEPROM_DATA)

eeprog: $(EEPROM_DATA)
	$(AVR_PROG) -p $(DEVICE) -c $(PROG) -U eeprom:w:$(EEPROM_DATA):i

fuses:
	$(AVR_PROG) -p $(DEVICE) -c $(PROG) -U lfuse:w:$(LFUSE):m
	$(AVR_PROG) -p $(DEVICE) -c $(PROG) -U hfuse:w:$(HFUSE):m

erase:
	$(AVR_PROG) -p $(DEVICE) -c $(PROG) -e

tags:	$(ASRCS) $(CSRCS)
	ctags $(CSRCS)

$(FIRMWARE): $(BIN)
	avr-objcopy -O ihex $(BIN) $(FIRMWARE)

$(EEPROM_DATA): constants.rb
	./constants.rb > $(EEPROM_DATA)

$(OBJS): $(IOREGS) $(REGVALS)

$(BIN):	$(OBJS) $(STDPROM)
	$(CC) -o $(BIN) -T $(STDPROM) $(LDFLAGS) $(OBJS) $(LIBS)

$(OBJS): otto.h alarms.h $(IOREGS) $(REGVALS)

alarms.h: $(MOTHER)/makeincludes/mkalarm.rb
	$(MOTHER)/makeincludes/mkalarm.rb
