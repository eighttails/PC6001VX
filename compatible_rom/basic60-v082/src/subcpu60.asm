;sub CPU (uPD8049) program for PC-6001
; by AKIKAWA, Hisashi  2024

;This software is redistributable under the LGPLv2.1 or any later version.


;user RAM: $20-$7f
.equ	MODE	$20		;operation mode
.equ	TMCNT	$21		;timer count
.equ	MODIFY	$22		;modifier key: 0=graph,1=ctrl,2=shift,3=ctrl
.equ	KANA	$23		;0=alphanumeric,1=hiragana,3=katakana
.equ	KEY	$24		;for auto repeat
.equ	AUTO	$25		;for auto repeat
.equ	RBAUD	$26		;baud rate for reading (1=1200, 2=600)
.equ	WBAUD	RBAUD+1		;baud rate for writing (1=1200, 2=600)
.equ	P1VAL	WBAUD+1		;port1 value(bit7-4) in key scan mode
.equ	RSEN	$29		;RS-232C enable (0=disable, 2=enable)

;constant
.equ	MODE_KEYRS	0
.equ	MODE_QUERY	1
.equ	MODE_READ	2
.equ	MODE_WRITE	3


;page 0
	.org	$000

;$000: boot
	jmp	INIT

;$003: #INT interrupt
	.org	$003
	sel	rb1
	mov	r7,a
	jmp	INT

;$007: timer/counter interrupt, interrupt period=480/8MHz*256=15.36ms
	.org	$007
	sel	rb1
	mov	r0,TMCNT
	inc	@r0
	retr


;#INT interrupt
INT:
	ins	a,bus		;command
	anl	a,$3f		;00-3f
	add	a,CMDJMP
	jmpp	@a

;jump table
CMDJMP:
	.db	INTEND, INTEND, INTEND, INTEND, INTEND, INTEND, QUERY, INTEND
	.db	INTEND, INTEND, INTEND, INTEND, RSOPEN, INTEND, INTEND, INTEND
	.db	INTEND, INTEND, INTEND, INTEND, INTEND, INTEND, INTEND, INTEND
	.db	INTEND, ROPEN, RCLOSE, INTEND, INTEND, R600, R1200, INTEND
	.db	INTEND, INTEND, INTEND, INTEND, INTEND, INTEND, INTEND, INTEND
	.db	INTEND, INTEND, INTEND, INTEND, INTEND, INTEND, INTEND, INTEND
	.db	INTEND, INTEND, INTEND, INTEND, INTEND, INTEND, INTEND, INTEND
	.db	JMPPUTCMT, WOPEN, WCLOSE, INTEND, INTEND, W600, W1200, INTEND

;$06: game key query
QUERY:
	mov	a,MODE_QUERY
SETMODE:
	mov	r0,MODE
	mov	@r0,a
INTEND:
	mov	a,r7
	retr

;$0c: RS-232C open
RSOPEN:
	mov	a,$02
	mov	r0,RSEN
	mov	@r0,a
	jmp	INTEND

;$19: CMT open for reading
ROPEN:
	mov	a,MODE_READ
	jmp	SETMODE

;$39: CMT open for writing
WOPEN:
	clr	a
	mov	t,a
	orl	p1,$20		;P15=CMT out
	mov	a,MODE_WRITE
	jmp	SETMODE

;$38: put to CMT
JMPPUTCMT:
	mov	r0,MODE
	mov	a,@r0
	xrl	a,MODE_WRITE
	jnz	INTEND
	jmp	PUTCMT

;$3a: CMT close for writing
WCLOSE:
	anl	p1,$df
;$1a: CMT close for reading
RCLOSE:
	mov	a,MODE_KEYRS
	jmp	SETMODE

;$1d: set 600 baud for reading
R600:
	mov	a,$02
	jmp	SETRBAUD

;$1e: set 1200 baud for reading
R1200:
	mov	a,$01
SETRBAUD:
	mov	r0,RBAUD
	mov	@r0,a
	jmp	INTEND

;$3d: set 600 baud for writing
W600:
	mov	a,$02
	jmp	SETWBAUD

;$3e: set 1200 baud for writing
W1200:
	mov	a,$01
SETWBAUD:
	mov	r0,WBAUD
	mov	@r0,a
	jmp	INTEND


;initialize
INIT:
	mov	r0,$20
	mov	r2,$7f-$20+1
	clr	a
INITLP:
	mov	@r0,a		;=0
	inc	r0
	djnz	r2,INITLP

	inc	a
	mov	r0,RBAUD
	mov	@r0,a		;=1:1200 baud
	inc	r0		;WBUAD
	mov	@r0,a		;=1:1200 baud
	inc	r0		;P1VAL
	mov	a,$d0		;bit7=Z80 #INT
				;bit6=kana lamp off
				;bit5=#CMTout
				;bit4=select key matrix or CMTin/RxRDY
				;bit3-0=matrix Y value
	mov	@r0,a
	outl	p1,a

	mov	r0,KANA
	mov	a,$00		;PC-6001
;	mov	a,$02		;PC-6001A
	mov	@r0,a

	en	tcnti
	strt	t

;main loop
MAIN:
	en	i
	mov	r0,MODE
	mov	a,@r0
	add	a,MODEJMP
	jmpp	@a

MODEJMP:
	.db	KEYRS, JMPGAMEKEY, JMPREAD, JMPWRITE

KEYRS:
	dis	i
	call	KEYSCAN
	call	RS232C
	jmp	MAIN

JMPGAMEKEY:
	jmp	GAMEKEY

JMPREAD:
	jmp	READ

JMPWRITE:
	jmp	WRITE


;generate z80 interrupt with data
;input: a=interrupt number, r2=data
;destroy: a
Z80INT:
	call	Z80INTNUM
	mov	a,r2
	outl	bus,a
	movx	a,@r0		;for floating
Z80INTLP:
	jt0	Z80INTLP	;wait for IBF=0
	ret


;check RS-232C
RS232C:
	anl	p1,$ef		;bit4=select key matrix or CMTin/RxRDY
	in	a,p2		;bit1=#RxRDY
	orl	p1,$10		;bit4=select key matrix or CMTin/RxRDY
	cpl	a
	mov	r0,RSEN
	anl	a,@r0
	jb1	RSINT
	ret

RSINT:
	clr	a
	mov	@r0,a
	mov	a,$04
;	jmp	Z80INTNUM

;generate z80 interrupt
;input: a=interrupt number
;destroy: none
Z80INTNUM:
	outl	bus,a
	movx	a,@r0		;for floating
	anl	p1,$7f		;P17 off (Z80 #INT)
Z80INTNLP:
	jt0	Z80INTNLP	;wait for IBF=0
	orl	p1,$80		;P17 on (Z80 #INT)
	ret


;page 1
	.org	$100

;key scan
KEYSCAN:
	mov	r0,P1VAL
	mov	a,@r0
	mov	r3,a		;P1 value
	outl	p1,a
	in	a,p2
	rrc	a
	mov	r2,3
SCANLP1:
	rrc	a		;ctrl,shift,graph
	jnc	MODFND		;modifier key found
	djnz	r2,SCANLP1
MODFND:
	mov	r0,MODIFY
	mov	a,r2
	mov	@r0,a		;0=none,1=graph,2=shift,3=ctrl

	mov	r0,KEY
	mov	r2,9
SCANLP2:
	mov	a,r3		;P1 value
	orl	a,r2		;bit3-0: matrix Y value(1-9)
	outl	p1,a
	in	a,p2
	cpl	a
	jnz	FOUND1
	djnz	r2,SCANLP2
	dec	a
	mov	@r0,a		;=ff
	ret

FOUND1:
	mov	r3,8
SCANLP3:
	rlc	a
	jc	FOUND2
	djnz	r3,SCANLP3
FOUND2:
	dec	r2
	dec	r3
	mov	a,r2
	rl	a
	rl	a
	rl	a
	add	a,r3
	mov	r2,a		;(Y-1)*8+X

;auto repeat
	mov	a,@r0		;r0=KEY
	xrl	a,r2
	mov	r1,AUTO
	jz	CHKRPT

;new key in
	mov	a,r2
	mov	@r0,a		;r0=KEY
	mov	@r1,-$14	;r1=AUTO: repeat delay
	mov	r0,TMCNT
KEYIN:
	mov	@r0,$00		;reset timer counter

	mov	r0,MODIFY
	mov	a,r2
	xrl	a,$40
	jnz	NOTKANA

;kana key
	mov	r1,KANA
	mov	a,@r1
	xrl	a,$01
	mov	@r1,a
	cpl	a
	anl	a,$01
	rr	a
	rr	a
	orl	a,$b0
	mov	r1,P1VAL
	mov	@r1,a
	outl	p1,a

NOTKANA:
	mov	a,r2
	xrl	a,$43
	jnz	NOTPAGE

;page switching key
	mov	a,@r0		;r0=MODIFY
	xrl	a,$02
	jnz	NOTPAGE

;shift+page switching key
	mov	r1,KANA
	mov	a,@r1
	xrl	a,$02		;PC-6001
;	orl	a,$02		;PC-6001A
	mov	@r1,a

NOTPAGE:
	call	CHKSTPFK
	mov	a,@r0		;r0=MODIFY
	add	a,<KEYJMP
	call	JMPPAGE1	;input=r2, output=r3,a
	mov	r2,a
	mov	a,r3
	call	Z80INT		;input: a,r2
KEYEND:
	ret

;check key repeat
;input: r1=AUTO
;output: r0=TMCNT
;destroy: a
CHKRPT:
	mov	r0,TMCNT
	mov	a,@r0
	add	a,@r1		;r1=AUTO
	jnc	KEYEND
	mov	@r1,-$03	;repeat rate
	jmp	KEYIN


;for call jmpp (page 1)
JMPPAGE1:
	jmpp	@a

;key scan jump table
KEYJMP:
	.db	<JMPNOMOD, <JMPGRAPH, <JMPSHIFT, <JMPCTRL

JMPNOMOD:
	jmp	NOMOD
JMPGRAPH:
	jmp	GRAPH
JMPSHIFT:
	jmp	SHIFT
JMPCTRL:
	jmp	CTRL

;reply to game key query
GAMEKEY:
	mov	r0,MODE
	mov	@r0,$00

	mov	r0,P1VAL
	mov	a,@r0
	outl	p1,a
	in	a,p2
	cpl	a
	rr	a
	rr	a
	anl	a,$01		;shift key
	mov	r2,a

	orl	p1,$05
	in	a,p2
	cpl	a
	anl	a,80h		;space key
	orl	a,r2
	mov	r2,a

	mov	a,@r0
	orl	a,$08
	outl	p1,a
	in	a,p2
	cpl	a
	anl	a,$3e		;left-right-down-up-stop
	orl	a,r2

	mov	r2,a
	mov	a,$16
	call	Z80INT
	jmp	MAIN


;page 2
	.org	$200

;no modifier key
;alphanumeric mode
NOMODTBL1:
;y1
	.db	$31, $71, $61, $7a, $6b, $69, $38, $2c
;y2
	.db	$32, $77, $73, $78, $6c, $6f, $39, $2e
;y3
	.db	$33, $65, $64, $63, $3b, $70, $f0, $2f
;y4
	.db	$34, $72, $66, $76, $3a, $40, $f1, $00
;y5
	.db	$35, $74, $67, $62, $5d, $5b, $f2, $20
;y6
	.db	$36, $79, $68, $6e, $2d, $5e, $f3, $30
;y7
	.db	$37, $75, $6a, $6d, $00, $5c, $f4, $00
;y8
	.db	$0d, $fa, $1e, $1f, $1c, $1d, $09, $1b
;y9
	.db	$00, $12, $7f, $fe, $0c, $00, $00, $00

;no modifier key
;hiragana mode
NOMODTBL2:
;y1
	.db	$e7, $e0, $e1, $e2, $e9, $e6, $f5, $e8
;y2
	.db	$ec, $e3, $e4, $9b, $f8, $f7, $f6, $f9
;y3
	.db	$91, $92, $9c, $9f, $fa, $9e, $f0, $f2
;y4
	.db	$93, $9d, $ea, $eb, $99, $de, $f1, $fb
;y5
	.db	$94, $96, $97, $9a, $f1, $df, $f2, $20
;y6
	.db	$95, $fd, $98, $f0, $ee, $ed, $f3, $fc
;y7
	.db	$f4, $e5, $ef, $f3, $00, $b0, $f4, $00
;y8
	.db	$0d, $fa, $1e, $1f, $1c, $1d, $09, $1b
;y9
	.db	$00, $12, $7f, $fe, $0c, $00, $00, $00

;no modifier key
;katakana mode
NOMODTBL3:
;y1
	.db	$c7, $c0, $c1, $c2, $c9, $c6, $d5, $c8
;y2
	.db	$cc, $c3, $c4, $bb, $d8, $d7, $d6, $d9
;y3
	.db	$b1, $b2, $bc, $bf, $da, $be, $f0, $d2
;y4
	.db	$b3, $bd, $ca, $cb, $b9, $de, $f1, $db
;y5
	.db	$b4, $b6, $b7, $ba, $d1, $df, $f2, $20
;y6
	.db	$b5, $dd, $b8, $d0, $ce, $cd, $f3, $dc
;y7
	.db	$d4, $c5, $cf, $d3, $00, $b0, $f4, $00
;y8
	.db	$0d, $fa, $1e, $1f, $1c, $1d, $09, $1b
;y9
	.db	$00, $12, $7f, $fe, $0c, $00, $00, $00

;no modifier key
;table address offset: alphanumeric,hiragana,alphanumeric,katakana
NOMODOFST:
	.db	<NOMODTBL1, <NOMODTBL2, <NOMODTBL1, <NOMODTBL3

;no modifier key
;input: r2=(Y-1)*8+X
;output: r3=interrupt number, a=key code
;destroy: a,r1
NOMOD:
	mov	r1,KANA
	mov	a,@r1
	add	a,<NOMODOFST
	movp	a,@a
	add	a,r2		;(Y-1)*8+X+offset
	movp	a,@a		;key code
	ret


;check stop key or function key
;input: r2=(Y-1)*8+X
;output: r3=interrupt number
;destroy: a
CHKSTPFK:
	mov	r3,$02		;interrupt number (normal key)
	mov	a,r2
	xrl	a,$39
	jz	INT14		;STOP key

;function key: r2=$16,$1e,$26,$2e,$36
	mov	a,r2
	add	a,-$36-1
	jc	NOTFKEY
	add	a,$36+1-$16
	jnc	NOTFKEY
	anl	a,$07
	jnz	NOTFKEY
INT14:
	mov	r3,$14		;interrupt number (special key)
NOTFKEY:
	ret


;page 3
	.org	$300
;+graph key
;alphanumeric mode
GRPTBL1:
;y1
	.db	$07, $00, $00, $00, $00, $16, $0d, $1f
;y2
	.db	$01, $00, $0c, $1c, $1e, $00, $0e, $1d
;y3
	.db	$02, $18, $14, $1a, $82, $10, $00, $80
;y4
	.db	$03, $12, $15, $11, $81, $00, $00, $83
;y5
	.db	$04, $19, $13, $1b, $85, $84, $00, $00
;y6
	.db	$05, $19, $0a, $00, $17, $00, $00, $0f
;y7
	.db	$06, $00, $00, $0b, $00, $09, $00, $00
;y8
	.db	$00, $00, $00, $00, $00, $00, $00, $00
;y9
	.db	$00, $12, $7f, $fe, $0c, $00, $00, $00

;+graph key
;hiragana mode
;katakana mode
GRPTBL2:
;y1
	.db	$00, $00, $00, $00, $00, $00, $00, $00
;y2
	.db	$00, $00, $00, $00, $00, $00, $00, $00
;y3
	.db	$00, $00, $00, $00, $00, $00, $00, $00
;y4
	.db	$00, $00, $00, $00, $00, $00, $00, $00
;y5
	.db	$00, $00, $00, $00, $00, $00, $00, $00
;y6
	.db	$00, $00, $00, $00, $00, $00, $00, $00
;y7
	.db	$00, $00, $00, $00, $00, $00, $00, $00
;y8
	.db	$00, $00, $00, $00, $00, $00, $00, $00
;y9
	.db	$00, $12, $7f, $fe, $0c, $00, $00, $00

;+ctrl key
;alphanumeric mode
CTLTBL1:
;y1
	.db	$00, $11, $01, $1a, $0b, $09, $00, $00
;y2
	.db	$00, $17, $13, $18, $0c, $0f, $00, $00
;y3
	.db	$00, $05, $04, $03, $00, $10, $00, $00
;y4
	.db	$00, $12, $06, $16, $00, $00, $00, $00
;y5
	.db	$00, $14, $07, $02, $1d, $1b, $00, $00
;y6
	.db	$00, $19, $08, $0e, $00, $1e, $00, $00
;y7
	.db	$00, $15, $0a, $0d, $00, $1c, $00, $00
;y8
	.db	$00, $00, $00, $00, $00, $00, $00, $00
;y9
	.db	$00, $12, $7f, $fe, $0c, $00, $00, $00

;+graph key
;table address offset: alphanumeric,hiragana,alphanumeric,katakana
GRPOFST:
	.db	<GRPTBL1, <GRPTBL2, <GRPTBL1, <GRPTBL2

;+ctrl key
;table address offset: alphanumeric,hiragana,alphanumeric,katakana
CTLOFST:
	.db	<CTLTBL1, <GRPTBL2, <CTLTBL1, <GRPTBL2

;with GRAPH key
;input: r2=(Y-1)*8+X
;output: r3=interrupt number, a=key code
;destroy: r1
GRAPH:
	mov	r1,KANA
	mov	a,@r1
	jb0	KANAGRP
;alphanumeric mode
	mov	r3,$14		;interrupt number (special key)

KANAGRP:
	add	a,<GRPOFST
	movp	a,@a
	add	a,r2		;(Y-1)*8+X+offset
	movp	a,@a		;key code
	ret


;with CTRL key
;input: r2=(Y-1)*8+X
;output: r3=interrupt number, a=key code
;destroy: r1
CTRL:
	mov	r1,KANA
	mov	a,@r1
	add	a,<CTLOFST
	movp	a,@a
	add	a,r2		;(Y-1)*8+X+offset
	movp	a,@a		;key code
	ret


;page 4
	.org	$400
;+shift key
;alphanumeric mode
SFTTBL1:
;y1
	.db	$21, $51, $41, $5a, $4b, $49, $28, $3c
;y2
	.db	$22, $57, $53, $58, $4c, $4f, $29, $3e
;y3
	.db	$23, $45, $44, $43, $2b, $50, $f5, $3f
;y4
	.db	$24, $52, $46, $56, $2a, $40, $f6, $5f
;y5
	.db	$25, $54, $47, $42, $00, $00, $f7, $20
;y6
	.db	$26, $59, $48, $4e, $3d, $00, $f8, $00
;y7
	.db	$27, $55, $4a, $4d, $00, $00, $f9, $00
;y8
	.db	$0d, $fa, $1e, $1f, $1c, $1d, $09, $1b
;y9
	.db	$00, $12, $7f, $00, $0b, $00, $00, $00

;+shift key
;hiragana mode
SFTTBL2:
;y1
	.db	$00, $00, $00, $8f, $00, $00, $8d, $a4
;y2
	.db	$00, $00, $00, $00, $00, $00, $8e, $a1
;y3
	.db	$87, $88, $00, $00, $00, $00, $f5, $a5
;y4
	.db	$89, $00, $00, $00, $00, $00, $f6, $00
;y5
	.db	$8a, $00, $00, $00, $a3, $a2, $f7, $20
;y6
	.db	$8b, $00, $00, $00, $00, $00, $f8, $86
;y7
	.db	$8c, $00, $00, $00, $00, $00, $f9, $00
;y8
	.db	$0d, $fa, $1e, $1f, $1c, $1d, $09, $1b
;y9
	.db	$00, $12, $7f, $00, $0b, $00, $00, $00

;+shift key
;katakana mode
SFTTBL3:
;y1
	.db	$00, $00, $00, $af, $00, $00, $ad, $a4
;y2
	.db	$00, $00, $00, $00, $00, $00, $ae, $a1
;y3
	.db	$a7, $a8, $00, $00, $00, $00, $f5, $a5
;y4
	.db	$a9, $00, $00, $00, $00, $00, $f6, $00
;y5
	.db	$aa, $00, $00, $00, $a3, $a2, $f7, $20
;y6
	.db	$ab, $00, $00, $00, $00, $00, $f8, $a6
;y7
	.db	$ac, $00, $00, $00, $00, $00, $f9, $00
;y8
	.db	$0d, $fa, $1e, $1f, $1c, $1d, $09, $1b
;y9
	.db	$00, $12, $7f, $00, $0b, $00, $00, $00

;+shift KEY
;table address offset: alphanumeric,hiragana,alphanumeric,katakana
SFTOFST:
	.db	<SFTTBL1, <SFTTBL2, <SFTTBL1, <SFTTBL3

;with SHIFT key
;input: r2=(Y-1)*8+X
;output: r3=interrupt number, a=key code
;destroy: r1
SHIFT:
	mov	r1,KANA
	mov	a,@r1
	add	a,<SFTOFST
	movp	a,@a
	add	a,r2		;(Y-1)*8+X+offset
	movp	a,@a		;key code
	ret


;page 5
	.org	$500

;command $38
PUTCMT:
;start bit
	call	WAVE1200

PUTCMTLP1:
	call	CHKSTOP
 	jnc	PUTCMTSTOP

	jni	PUTCMTDATA	;check #INT=0
	jmp	PUTCMTLP1

PUTCMTDATA:
	ins	a,bus
	mov	r2,a		;data

;data
	mov	r3,8
PUTCMTLP2:
	mov	a,r2		;data
	rrc	a
	mov	r2,a		;data
	call	FSK
	djnz	r3,PUTCMTLP2

;stop bit
	call	WAVE2400
	call	WAVE2400
	call	WAVE2400

	jmp	INTEND

PUTCMTSTOP:
	call	WSTOPINT
	ins	a,bus
	jmp	INTEND


;write 2400Hz header
WRITE:
	call	CHKSTOP
	jnc	WRITESTOP
	dis	i
	call	WAVE2400
	jmp	MAIN

WRITESTOP:
	call	WSTOPINT
	jmp	MAIN

WSTOPINT:
	mov	r0,MODE
	mov	a,MODE_KEYRS
	mov	@r0,a
	mov	a,$0e
	jmp	Z80INTNUM

;input: c-flag=bit
;destroy: a,r1,r4,r5,r6
FSK:
	jnc	WAVE1200

;bit=1: 2400Hz*2
;destroy: a,r1,r4,r5,r6
WAVE2400:
	mov	r4,-3		;square wave width 1, 480/8MHz*3=180us
	mov	r5,-4		;square wave width 2, 480/8MHz*4=240us
	call	SQWAVE
	jmp	SQWAVE

;bit=0: 1200Hz*1
;timer tick=480/8MHz=60us
;destroy: a,r1,r4,r5,r6
WAVE1200:
	mov	r4,-7		;square wave width 1, 480/8MHz*7=420us
	mov	r5,-7		;square wave width 2, 480/8MHz*7=420us
;	jmp	SQWAVE

;square wave
;input: r5,r6=square wave width
;destroy: a,r1,r6
SQWAVE:
	mov	r1,WBAUD
	mov	a,@r1
	mov	r6,a
SQWLP1:
	mov	a,t
	add	a,r4		;square wave width 1
	jnc	SQWLP1

;	clr	a
	mov	t,a
	anl	p1,$df		;P15=CMT out
SQWLP2:
	mov	a,t
	add	a,r5		;square wave width 2
	jnc	SQWLP2

;	clr	a
	mov	t,a
	orl	p1,$20		;P15=CMT out
	djnz	r6,SQWLP1
	ret


;read data
READ:
;header or stop bit
	mov	r0,MODE
	mov	a,@r0
	xrl	a,MODE_READ
	jnz	READEND

	call	CHKSTOP
	jnc	READSTOP

	call	MEASFREQ
	jnc	READ		;wait for bit=1

;start bit
READLP1:
	mov	r0,MODE
	mov	a,@r0
	xrl	a,MODE_READ
	jnz	READEND

	call	CHKSTOP
	jnc	READSTOP

	call	MEASFREQ
	jc	READLP1		;wait for bit=0

;data
	mov	r3,8
READLP2:
	call	MEASFREQ
	mov	a,r2		;data
	rrc	a
	mov	r2,a		;data
	djnz	r3,READLP2

;stop bit
;	call	MEASFREQ
;	jnc	READERR

	mov	a,$08
	call	Z80INT

READEND:
	jmp	MAIN

READERR:
	mov	r0,MODE
	mov	@r0,$00
	mov	a,$12
	call	Z80INTNUM
	jmp	MAIN

READSTOP:
	mov	r0,MODE
	mov	a,MODE_KEYRS
	mov	@r0,a
	mov	a,$10
	call	Z80INTNUM
	jmp	MAIN

;measure frequency
;output: c-flag(0=1200Hz 1=2400Hz)
;destroy: a,r1,r7
MEASFREQ:
	call	MEASFREQ1
	jc	MEASFREQ1	;2400Hz*2or4
	ret			;1200Hz*1or2

;measure frequency
;output: c-flag(0=1200Hz 1=2400Hz)
;destroy: a,r1,r7
MEASFREQ1:
	call	EDGE

	mov	r1,RBAUD
	mov	a,@r1		;1=1200baud, 2=600baud
	mov	r7,-10		;1200baud: t=0-9->bit=1, t=10-255->bit=0
	dec	a
	jz	RBAUD1200
	mov	r7,-20		;600baud: t=0-19->bit=1, t=20-255->bit=0
RBAUD1200:
	mov	a,t		;1200baud: t=7->2400Hz, t=14->1200Hz
				; 600baud: t=14->2400Hz, t=28->1200Hz
	add	a,r7
	cpl	c
	clr	a
	mov	t,a
	ret

;wait for CMTin 1->0 edge
;output: t=count
;destroy: a,r1,r7
EDGE:
	anl	p1,$ef		;bit4=select key matrix or CMTin/RxRDY

	mov	r1,RBAUD
	mov	a,@r1
	mov	r7,a
EDGELP1:
	mov	a,t
	add	a,-30
	jc	EDGEERR
	in	a,p2		;bit3=CMTin
	cpl	a
	jb3	EDGELP1		;wait for CMTin=1
EDGELP2:
	mov	a,t
	add	a,-30
	jc	EDGEERR
	in	a,p2		;bit3=CMTin
	jb3	EDGELP2		;wait for CMTin=0

	djnz	r7,EDGELP1
	ret

EDGEERR:
	orl	p1,$10		;bit4=select key matrix or CMTin/RxRDY
	ret

;check stop key
;output: c-flag(0=push stop)
;destroy: a
CHKSTOP:
	anl	p1,$f0
	orl	p1,$18		;bit4=select key matrix or CMTin/RxRDY
	in	a,p2
	rrc	a
	rrc	a
	ret


;page 6
	.org	$600

;page 7
	.org	$700

;2KB in total
	.org	$800

	.end
