;
; Copyright (c) 2007, Kalopa Research Limited.  All rights
; reserved.  Unpublished rights reserved under the copyright laws
; of the United States and/or the Republic of Ireland.
;
; The software contained herein is proprietary to and embodies the
; confidential technology of Kalopa Research Limited.  Possession,
; use, duplication or dissemination of the software and media is
; authorized only pursuant to a valid written license from Kalopa
; Research Limited.
;
; RESTRICTED RIGHTS LEGEND   Use, duplication, or disclosure by the
; U.S.  Government is subject to restrictions as set forth in
; Subparagraph (c)(1)(ii) of DFARS 252.227-7013, or in FAR 52.227-19,
; as applicable.
;
; THIS SOFTWARE IS PROVIDED BY KALOPA RESEARCH LIMITED "AS IS" AND
; ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
; TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
; PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL KALOPA
; RESEARCH LIMITED BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
; SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
; LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
; OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
; CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
; STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
; ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
; ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
;
; ABSTRACT
;
.include "regvals.inc"
;
; Interrupt Vector table.
	.arch	atmega8
	.section .vectors,"ax",@progbits
	.global	__vectors
	.func	__vectors
__vectors:
	rjmp	_reset				; Main reset
	rjmp	nointr				; External Interrupt 0
	rjmp	nointr				; External Interrupt 1
	rjmp	nointr				; Timer 2 Compare
	rjmp	nointr				; Timer 2 Overflow
	rjmp	nointr				; Timer 1 Capture Event
	rjmp	_clkintr			; Timer 1 Compare Match A
	rjmp	nointr				; Timer 1 Compare Match B
	rjmp	nointr				; Timer 1 Counter Overflow
	rjmp	nointr				; Timer 0 Counter Overflow
	rjmp	nointr				; SPI Serial Transfer Complete
	rjmp	_sio_in				; USART Rx Complete
	rjmp	_sio_out			; USART Data Register Empty
	rjmp	nointr				; USART Tx Complete
	rjmp	nointr				; ADC Conversion Complete
	rjmp	nointr				; EEPROM Ready
	rjmp	nointr				; Analog Comparator
	rjmp	nointr				; Two Wire Interface
	rjmp	nointr				; Store Program Memory Ready
	.string	"Copyr. 2013 KRL"
	.endfunc
;
	.section .init6,"ax",@progbits
;
; I/O Port initialization
	.global	_ioinit
	.func	_ioinit
_ioinit:
	ldi	r16,0x18			; PORTB=0001 1000
	out	PORTB,r16
	ldi	r16,0x27
	out	DDRB,r16			; DDRB=IIOI IOOO

	ldi	r16,0x00			; PORTC=0000 0000
	out	PORTC,r16
	out	DDRC,r16			; DDRC=IIII IIII

	ldi	r16,0xf0			; PORTD=1111 0000
	out	PORTD,r16
	ldi	r16,0xce
	out	DDRD,r16			; DDRD=OOII OOOI
	ret
	.endfunc
;
; ***** Interrupt Service Routines *****
;
; Clock interrupt.
;
; This fires every 5ms. The job is advance the steppers where
; appropriate and call the upper level routine, if warranted.
	.global	_clkintr
	.func	_clkintr
_clkintr:
	push	r24				; Save the status register
	in 	r24,SREG
	push	r24
	push	r25
;
	push	r18
	push	r19
	push	r26
	push	r27
;
; Check for servo faults
	sbis	PINB,3				; Rudder servo fault?
	rjmp	clk1				; Yes.
	sbic	PINB,4				; Sail servo fault?
	rjmp	clk3				; No.
;
; Deal with a sail servo fault signal.
	lds	r24,alarm_state			; Turn on SAILSRV_FAULT
	ori	r24,0x04
	rjmp	clk2
;
; Deal with a rudder servo fault signal.
clk1:	lds	r24,alarm_state			; Turn on RUDDSRV_FAULT
	ori	r24,0x02
;
clk2:	sts	alarm_state,r24			; Save the new alarm state
	cbi	PORTB,0				; Turn on the servo reset
	rjmp	clk4				; Skip servo adjustments
;
clk3:	sbi	PORTB,0				; Remove servo reset
	rcall	rudder_setpos
	rcall	sail_setpos
;
clk4:	lds	r24,tick			; Increment the tick
	inc 	r24
	cpi 	r24,200				; One second?
	brne	clk5
;
; A second has passed - call the upper-level routine.
	rcall	clocktick			; Call the main clock tick
	clr	r24				; Clear the tick counter
;
clk5:	sts	tick,r24
	pop	r27				; Restore the registers
	pop	r26
	pop	r19
	pop	r18
;
; Common return routine from interrupts.
isrout:
	pop	r25				; Restore the status register
	pop	r24
	out	SREG,r24
	pop	r24
;
nointr:
	reti					; Return from interrupt
	.endfunc
;
; uchar_t _rudder_home();
;
; Return the status of the rudder-zero switch.
	.global	_rudder_home
	.func	_rudder_home
_rudder_home:
	ldi	r24,0
	ret
	.endfunc
;
; void _rudder_inc();
;
; Step up the rudder stepper.
	.global	_rudder_inc
	.func	_rudder_inc
_rudder_inc:
	sbi	PORTD,2				; SDIR1 = 1
;
	lds	r24,cur_rudder			; Increment the position.
	lds	r25,cur_rudder+1
	adiw	r24,1
	rjmp	rudd1
	.endfunc
;
; void _rudder_dec();
;
; Step down the rudder stepper.
	.global	_rudder_dec
	.func	_rudder_dec
_rudder_dec:
	cbi	PORTD,2				; SDIR1 = 0
;
	lds	r24,cur_rudder			; Decrement the position.
	lds	r25,cur_rudder+1
	sbiw	r24,1
rudd1:	sts	cur_rudder+1,r25
	sts	cur_rudder,r24
;
	nop					; Wait for direction settling.
	nop
	nop
	nop
	nop
;
	sbi	PORTB,1				; Set the STEP pulse
	ret
	.endfunc
;
; uchar_t _sail_home();
;
; Return the status of the sail-zero switch.
	.global	_sail_home
	.func	_sail_home
_sail_home:
	ldi	r24,0
	ret
	.endfunc
;
; void _sail_inc();
;
; Step up the sail stepper.
	.global	_sail_inc
	.func	_sail_inc
_sail_inc:
	sbi	PORTD,3				; SDIR2 = 1
;
	lds	r24,cur_sail			; Decrement the position.
	lds	r25,cur_sail+1
	adiw	r24,1
	rjmp	sail1
	.endfunc
;
; void _sail_dec();
;
; Step down the sail stepper.
	.global	_sail_dec
	.func	_sail_dec
_sail_dec:
	cbi	PORTD,3				; SDIR2 = 0
;
	lds	r24,cur_sail			; Decrement the position.
	lds	r25,cur_sail+1
	sbiw	r24,1
sail1:	sts	cur_sail+1,r25
	sts	cur_sail,r24
;
	nop					; Wait for direction settling.
	nop
	nop
	nop
	nop
;
	sbi	PORTB,2				; Set the STEP pulse
	ret
	.endfunc
;
; void _servo_reset();
;
; Reset the servo controllers.
	.global	_servo_reset
	.func	_servo_reset
_servo_reset:
	cbi	PORTB,0				; Hold down the reset pin
	ret
	.endfunc
;
; uchar_t _keysense();
;
; Return the sense of the key switch.
        .global _keysense
        .func   _keysense
_keysense:
        in      r24,PIND
        andi    r24,0x04                        ; Status of mission key
        ret
        .endfunc
;
; void _buzzer(uchar_t);
;
; Sound the alarm!
        .global _buzzer
        .func   _buzzer
_buzzer:
        tst     r24                             ; Buzzer on or off?
        breq    bz1
        sbi     PORTD,7                         ; Enable the FET
        ret
bz1:    cbi     PORTD,7                         ; Turn off the FET
        ret
        .endfunc
;
; void _navlight(uchar_t);
;
; Turn on/off the navigation lights
        .global _navlight
        .func   _navlight
_navlight:
        tst     r24                             ; Lights on or off?
        breq    nl1
        sbi     PORTD,6                         ; Enable the FET
        ret
nl1:    cbi     PORTD,6                         ; Turn off the FET
        ret
        .endfunc
;
; void _cpupwr(uchar_t);
;
; Turn on or off the main CPU.
        .global _cpupwr
        .func   _cpupwr
_cpupwr:
        tst     r24                             ; Mother on or off?
        breq    cp1
        cbi     PORTB,1                         ; Turn on the CPU
        ret
cp1:    sbi     PORTB,1                         ; Turn off the CPU
        ret
        .endfunc
;
; Some useful (low level) variables...
	.weak	__heap_end
	.set	__heap_end,0
;
; Fin.
