;Compatible BASIC for PC-6601
; by AKIKAWA, Hisashi  2017

;This software is redistributable under the LGPLv2.1 or any later version.


TALK	equ	4000h

MF	equ	0000h		;male/female
SPEED	equ	0001h		;0-4
COMMAND	equ	0002h		;0-4,fe
QMAG	equ	0003h		;0 or 4
VOICED	equ	0004h		;0=unvoiced 1=voiced
TDIFF	equ	0005h		;time residual frames
PDIFF	equ	0006h		;pitch difference
APSTCNT	equ	0007h		;apostrophe counter
SYLBCNT	equ	0008h		;syllable counter
WORDCNT	equ	0009h		;word counter
TMPPTCH	equ	000ah		;temporary pitch (for ^ and +)
HATCNT	equ	000bh		;^ counter
PLUSCNT	equ	000ch		;+ counter
OUTADR	equ	000eh		;output address

;sing
SHARP	equ	0010h		;#
VOLUME	equ	0011h		;V-value << 4
OCTAVE	equ	0012h		;O-value
LENGTH	equ	0013h		;L-value (by frames)
REST	equ	0014h		;rest or not
MMLPTCH	equ	0015h		;MML pitch
MMLLEN	equ	0016h		;MML length
MMLADR	equ	0017h		;MML address
KEYFLG	equ	0fa5ah		;special key flags


F1	equ	0020h		;f1,f1diff,b1,b1diff,...,b5diff
F1DIFF	equ	0021h

STACK	equ	00f0h

WORK	equ	0100h

FRAMES	equ	10h		;frames for 1 syllable
FRAMES2	equ	40h		;frmaes (sing)
;FRAMES2	equ	48h		;frmaes (sing)
;FRAMES2	equ	4ch		;frmaes (sing)


;;;;;;;;;;;;;;;;;;;;;;;;;

	org	4000h

;input: hl=string address, e=length
;destroy: af,bc,de,hl,af',bc',d',hl'
_TALK:	ds	TALK-_TALK
	org	TALK

	ld	(STACK),sp

;check male/female
	ld	a,e
	or	a
	jp	z,TALKERR

	ld	a,(hl)
	or	20h
	cp	'f'
	jr	z,MFOK
	cp	'm'
	jp	nz,TALKERR
MFOK:
	ld	(MF),a
	ld	d,a

	dec	e
	jp	z,TALKERR
	inc	hl


;check speed ((1)-6, space=2)
	ld	a,(hl)
	cp	' '
	jr	nz,CHKSPD1
	ld	a,'2'
CHKSPD1:
	sub	'1'
	jr	nz,CHKSPD2
	ld	a,(KEYFLG)
	and	20h
	jp	z,TALKERR
	xor	a
CHKSPD2:
	cp	6-1+1
	jp	nc,TALKERR
	ld	(SPEED),a

	dec	e
	jp	z,TALKERR
	inc	hl

;check internal messages(0-4)/sing(#)/talk(others)
	ld	a,(hl)
	sub	'0'
	cp	'4'-'0'+1
	jp	c,TALKOUT	;internal message

	dec	e
	jp	z,TALKERR

;clear work area
	exx
	ld	hl,F1
	ld	b,4*5
CLRLP:
	ld	(hl),00h
	inc	hl
	djnz	CLRLP
	ld	hl,WORK
	ld	(OUTADR),hl
	ld	(hl),b		;b=0
	ld	hl,SHARP
	ld	(hl),b
	inc	hl
	ld	(hl),0a0h	;VOLUME
	exx

	push	hl
	push	de

	cp	'#'-'0'
	jr	z,PRESING

	ld	a,d
	cp	'f'
	ld	a,06h
	jr	z,FEMALE
	ld	a,27h
FEMALE:
	ld	(PDIFF),a
	jr	SETPITCH


;prepare for singing
PRESING:
	dec	e
	jp	z,TALKERR
	inc	hl
	ld	a,(hl)
	cp	':'
	jr	nz,PRESING
	inc	hl
	ld	(MMLADR),hl
	ld	a,e
	ld	(MMLLEN),a

	push	hl
	push	af

	ld	hl,SHARP
	ld	(hl),01h
	inc	hl
	ld	(hl),080h	;VOLUME
	inc	hl
	ld	(hl),04h	;OCTAVE
	inc	hl
	ld	(hl),FRAMES2	;LENGTH
	inc	hl
	ld	(hl),00h	;REST
	inc	hl
	ld	(hl),00h	;MMLPTCH

	call	ANAMML
	pop	af
	pop	hl
	ld	(MMLADR),hl
	ld	(MMLLEN),a

SETPITCH:
	ld	hl,WORK
	call	CHGRAM0
PRESINGLP:
	ld	a,(PDIFF)
	or	a		;
	jr	z,PRESINGEND
	ld	a,1
	ld	(TDIFF),a
	call	NOSOUND1
	jr	PRESINGLP

PRESINGEND:
	call	CHGROM0
	ld	(OUTADR),hl

	pop	de
	pop	hl
;	jr	TALKEXT


;external message
;d=male/female, e=length, hl=string address
TALKEXT:
	inc	hl
	ld	a,(hl)
	cp	' '
	jr	nz,EXTOK
	dec	e
	jp	z,TALKERR
	jr	TALKEXT

EXTOK:

	xor	a
	ld	(SYLBCNT),a
	ld	(WORDCNT),a

EXTLP1:
	call	CHGRAM0
	ld	a,(WORDCNT)
	inc	a
	cp	4
	jp	nc,TALKERR
	ld	(WORDCNT),a
	call	CHGROM0
	xor	a
	ld	(TMPPTCH),a
	ld	(HATCNT),a

;word start
	xor	a
	ld	(APSTCNT),a
	ld	(PLUSCNT),a
	ld	(PDIFF),a
EXTLP2:
	ld	a,FRAMES
	ld	(TDIFF),a
	call	ANAMML
EXTLP3:
	ld	a,(hl)
;	cp	'.'
;	jp	z,EXTEND
;	cp	':'
;	jp	z,EXTEND

	cp	'^'
	jp	z,HAT
	cp	'+'
	jp	z,PLUS

	or	20h		;lowercase
	call	CHKVOW
	jp	z,VOWEL


;consonant
	dec	e
	jp	z,TALKERR

	ld	b,a		;
	inc	hl

	push	hl
	push	de

	ld	a,(hl)
	ld	e,a

	or	20h		;
	ld	c,1
	cp	'y'
	jr	z,EXTZ		;contracted sound
	call	CHKVOW
	jp	nz,TALKERR
EXTZ:

	call	GETOUTADR
	ex	de,hl


;voiced sound
;b=character, c=vowel(0-4), de=output address, h=male/female, l=next character
	ld	a,01h
	ld	(VOICED),a

	ld	a,b		;
	cp	'b'
	jp	z,CON_B
	cp	'd'
	jp	z,CON_D
	cp	'g'
	jp	z,CON_G
	cp	'w'
	jp	z,CON_W
	cp	'y'
	jp	z,CON_Y
	cp	'r'
	jp	z,CON_R
	cp	'm'
	jp	z,CON_M
	cp	'n'
	jp	z,CON_N
	cp	'z'
	jp	z,CON_Z

;unvoiced sound
	xor	a
	ld	(VOICED),a
	ld	a,b		;

	cp	'p'
	jp	z,CON_P
	cp	'k'
	jp	z,CON_K
	cp	't'
	jp	z,CON_T
	cp	's'
	jp	z,CON_S
	cp	'h'
	jp	z,CON_H
	cp	'c'
	jp	z,CON_C
	jp	TALKERR


TALKERR:
	call	CHGRAM0
	ld	hl,(STACK)
	call	CHGROM0
	ld	sp,hl
	ld	a,01h
	ret


HAT:
	ld	a,01h
	ld	(HATCNT),a
	inc	hl
	ld	a,(hl)
	cp	'^'
	jp	z,TALKERR
	cp	'+'
	ld	a,0-6
	jr	nz,HATNZ
	ld	a,01h
	ld	(PLUSCNT),a
	inc	hl
	dec	e
	jp	z,TALKERR
	ld	a,0-15
HATNZ:
	ld	(TMPPTCH),a
	ld	b,a
	call	CHGRAM0

	ld	a,(SHARP)
	or	a
	jr	nz,HATEND

	ld	a,(PDIFF)
	add	a,b
	ld	(PDIFF),a
	dec	e
	jp	z,TALKERR
HATEND:
	call	CHGROM0
	jp	EXTLP3


PLUS:
	inc	hl
	ld	a,(hl)
	cp	'+'
	jp	z,TALKERR
	cp	'^'
	jp	z,TALKERR

	call	CHGRAM0

	ld	a,(SHARP)
	or	a
	jr	nz,PLUSEND

	ld	a,(PDIFF)
	ld	b,a
	ld	a,(HATCNT)
	or	a
	jr	z,PLUSZ
;^+
	ld	a,0-15
	ld	(TMPPTCH),a
	ld	a,0-9
	jr	PLUSPDIFF

PLUSZ:
	ld	a,0-6
	ld	(TMPPTCH),a
PLUSPDIFF:
	add	a,b
	ld	(PDIFF),a
	call	CHGROM0

	dec	e
	jp	z,TALKERR

PLUSEND:
	ld	a,01h
	ld	(PLUSCNT),a

	jp	EXTLP3

APOST:
	call	CHGRAM0
	ld	a,(APSTCNT)
	or	a
	jp	nz,TALKERR
	inc	a
	ld	(APSTCNT),a

	ld	a,(SHARP)
	or	a
	call	CHGROM0
	jp	nz,VOWEND
	call	CHGRAM0

	push	hl
	ld	hl,(OUTADR)
	ld	bc,0-(FRAMES*7-6)
	add	hl,bc
	ld	bc,7
APOSTLP:
	ld	a,(hl)
	and	07h
	jr	z,APOSTZ
	add	hl,bc
	jr	APOSTLP

APOSTZ:
	ld	a,(VOLUME)
	or	05h
	ld	(hl),a		;bit7-4=amp, bit3=FV, bit2-0=pitch=-3
	add	hl,bc
	ld	(hl),a
	call	CHGROM0
	pop	hl
	dec	e
	jp	z,EXTEND
	ld	a,12		;PDIFF=+12
	ld	(PDIFF),a
	jp	VOWEND


SPACELP:
	inc	hl
SPACE:
	dec	e
	jp	z,EXTEND
	ld	a,(hl)
	cp	' '
	jr	z,SPACELP
	cp	'^'
	jp	z,TALKERR
	push	hl
	call	CHGRAM0

	ld	a,(SHARP)
	or	a
	jr	nz,NOPLUS


;	ld	hl,(OUTADR)
;	dec	hl
;	ld	(hl),0c3h	;pitch=+3
;	ld	hl,TMPPTCH
;	ld	a,(hl)
;	add	a,3
;	ld	(hl),a

	ld	a,(PLUSCNT)
	or	a
	jr	z,NOPLUS

	ld	hl,PDIFF
	ld	a,(HATCNT)
	or	a
	ld	a,(hl)
	jr	z,NOHAT

	add	a,9
	ld	(hl),a
	ld	a,0-6
	ld	(TMPPTCH),a
	jr	NOPLUS

NOHAT:
	add	a,6
	ld	(hl),a
	xor	a
	ld	(TMPPTCH),a

NOPLUS:
	ld	a,(APSTCNT)
	or	a
	jr	z,NOAPST1
	ld	hl,PDIFF
	ld	a,(hl)
	sub	6
	ld	(hl),a
NOAPST1:

	xor	a
	ld	(PLUSCNT),a
	ld	(APSTCNT),a

	call	CHGROM0
	pop	hl

	ld	a,(hl)
	inc	hl
	cp	'?'
	jp	z,QMARK
	cp	'/'
	jp	z,SLASH
	dec	hl

	jp	EXTLP2


SLASHLP1:
	inc	hl
SLASH:
	dec	e
	jp	z,TALKERR
	ld	a,(hl)
	cp	' '
	jr	z,SLASHLP1
	cp	'.'
	jp	z,TALKERR
	cp	':'
	jp	z,TALKERR

	push	hl
	push	de

	call	CHGRAM0

	ld	a,(SHARP)
	or	a
	jr	nz,QSLSING

	ld	hl,(OUTADR)
	dec	hl
	ld	a,(VOLUME)
	or	03h
	ld	(hl),a		;pitch=+3
	inc	hl
	ld	a,(PDIFF)
	ld	c,a
	ld	a,(TMPPTCH)
	sub	c
	ld	c,a
	call	CHGROM0	
	inc	c
	inc	c
	inc	c

;? and /
;input: hl=output address, c=pdiff
QSLASH:
	call	CHGRAM0
	ld	a,(APSTCNT)
	or	a
	ld	a,c
	jr	z,NOAPST2
	add	a,6
NOAPST2:
	neg
	ld	(PDIFF),a
	ld	a,FRAMES*2
	ld	(TDIFF),a
	call	NOSOUND1
	pop	de
	pop	hl
	jp	EXTLP1


;q and / for singing
;input: hl=string address
QSLSING:
	ld	a,(hl)
	cp	':'		;for ?
	jr	z,QMARKZ2
	ld	a,1
	ld	(REST),a
	ld	a,FRAMES
	ld	(TDIFF),a
	call	ANAMML
	call	CHGRAM0
	ld	a,(REST)
	or	a
	jp	z,TALKERR
	xor	a
	ld	(REST),a
	ld	hl,(OUTADR)
	call	NOSOUND1
	ld	(hl),0
	call	CHGROM0
	pop	de
	pop	hl
	jp	EXTLP1


QMARKLP1:
	inc	hl
QMARK:
	dec	e
	jr	z,QMARKZ1
	ld	a,(hl)
	cp	'.'
	jp	z,TALKERR
	cp	' '
	jr	z,QMARKLP1
QMARKZ1:
	inc	e

	push	hl
	push	de

	call	CHGRAM0
	ld	a,(SHARP)
	or	a
	jr	nz,QSLSING

	ld	a,(PDIFF)
	ld	c,a
	ld	hl,TMPPTCH
	ld	a,(hl)
	sub	21
	ld	(hl),a
	sub	c
	ld	c,a
	ld	hl,(OUTADR)
	dec	hl
	ld	de,0-7
	ld	b,7
QMARKLP2:
	ld	a,(hl)
	or	05h
	ld	(hl),a
	add	hl,de
	djnz	QMARKLP2

;	ld	hl,(OUTADR)
;	call	CHGROM0
;
;	pop	de
;	dec	e
;	push	de
;	jp	nz,QSLASH
;
;	pop	de
;	pop	hl
;	jp	EXTEND


	pop	de
	pop	hl
	push	hl
	dec	e
	jr	z,QMARKZ3
	ld	a,(hl)
	cp	':'
	jr	z,QMARKZ3
	push	de
	call	CHGRAM0
	ld	hl,(OUTADR)
	call	CHGROM0
	jp	nz,QSLASH
QMARKZ2:
	pop	de
QMARKZ3:
	pop	hl
	jp	EXTEND


;pa,pi,pu,pe,po
CON_P:
	ld	a,02h
	jr	BPCOMMON

;ba,bi,bu,be,bo
CON_B:
	ld	a,01h

BPCOMMON:
	ld	b,a
	call	NOSOUND2
	ld	hl,BTBL

;consonant common routine
;input: c=vowel(0-4), de=output address, hl=table address
CONCOMMON:
	call	CHGRAM0
	ld	a,(MF)
	cp	'm'		;
	call	CHGROM0
	ld	a,c
	jr	z,BMALE		;
	add	a,05h
BMALE:
	add	a,a
	ld	c,a
	ld	b,0
	add	hl,bc
	ld	a,(hl)
	inc	hl
	ld	h,(hl)
	ld	l,a
CONCOMMON2:
	ld	b,(hl)
	inc	hl
	ex	de,hl

BLP:
	push	bc
	push	de
	call	PUT7
	pop	de
	ex	de,hl
	ld	bc,10
	add	hl,bc
	ex	de,hl
	pop	bc
	djnz	BLP
	ld	(OUTADR),hl

	pop	de
	pop	hl
	jp	EXTLP3


;da,di,du,de,do
CON_D:
CON_T:
	ld	b,02h
	call	NOSOUND2
	ld	hl,DTBL
	jr	CONCOMMON

;ga,gi,gu,ge,go
CON_G:
CON_K:
	ld	b,02h
	call	NOSOUND2
	ld	hl,GTBL
	jr	CONCOMMON


;c=0-4
CON_W:
	ld	a,c
	or	a		;wa
	jp	nz,TALKERR
	ld	a,h		;male/female
	cp	'm'
	ld	hl,M_WA
	jr	z,CONCOMMON2
	ld	hl,F_WA
	jr	CONCOMMON2


CON_Y:
	ld	a,l		;next character
	cp	'y'
	jp	z,TALKERR
	ld	a,c
	dec	a
	jp	z,TALKERR	;yi
	ld	hl,YTBL
	jp	CONCOMMON


CON_R:
	ld	hl,RTBL
	jp	CONCOMMON

CON_M:
	ld	hl,MTBL
	jp	CONCOMMON

CON_N:
	ld	hl,NTBL
	jp	CONCOMMON

CON_Z:
;	ld	b,01h
;	call	NOSOUND2
	ld	hl,ZTBL
	jp	CONCOMMON

CON_S:
	ld	hl,STBL
	jp	CONCOMMON

CON_H:
	ld	a,l		;next character
	cp	'y'
	jr	z,CON_HY
	ld	hl,HTBL
	jp	CONCOMMON

CON_HY:
	ld	a,h		;male/female
	cp	'm'
	ld	hl,M_HY
	jp	CONCOMMON2
	ld	hl,F_HY
	jp	CONCOMMON2


CON_C:
	ld	b,02h
	call	NOSOUND2
	ld	hl,CTBL
	jp	CONCOMMON


;c=0-4,d=male/female
VOWEL:
	push	hl
	push	de

	ld	a,01h
	ld	(VOICED),a
	inc	hl
	ld	a,(hl)

	sub	'*'
	jr	nz,VOWNZ
	ld	(VOICED),a	;a=0

	pop	de
	dec	e
	ex	(sp),hl
	push	de

VOWNZ:
	ld	hl,VOWTBL
	ld	a,d		;male/female
	cp	'm'
	ld	a,c
	jr	z,VOWMALE
	add	a,05h
VOWMALE:
	add	a,a
	ld	c,a
	ld	b,0
	add	hl,bc
	ld	e,(hl)
	inc	hl
	ld	d,(hl)

	call	GETOUTADR
	call	PUT7
	call	SYLLABLE

	pop	de
	pop	hl
	inc	hl
	dec	e
	jp	z,EXTEND


VOWEND:
	ld	a,(hl)
	inc	hl
	cp	'.'
	jp	z,EXTEND
	cp	':'
	jp	z,EXTEND
	cp	27h		;apostrophe
	jp	z,APOST
	cp	' '
	jp	z,SPACE
	cp	'/'
	jp	z,SLASH
	cp	'?'
	jp	z,QMARK
	cp	'-'
	jr	z,LONG
	cp	'+'
	jp	z,TALKERR
	cp	'^'
	jp	z,TALKERR
	or	20h
	cp	'q'
	jp	z,GEMINATE
	cp	'x'
	jp	z,CON_X

	dec	hl
	jp	EXTLP2


;long sound
LONG:
	ld	a,e
	dec	a
	jr	z,LONGZ
	ld	a,(hl)
	cp	'-'
	jp	z,TALKERR
	sub	'*'
	jr	nz,LONGZ
	ld	(VOICED),a	;a=0
	inc	hl
	dec	e
LONGZ:
	push	hl
	push	de
	call	GETOUTADR
	ld	a,FRAMES
	ld	(TDIFF),a
	call	ANAMML
	xor	a
	ld	(PDIFF),a
	call	SYLLABLE
	pop	de
	pop	hl
	dec	e
	jp	z,EXTEND
	jp	VOWEND



;put a syllable
;hl=output address
SYLLABLE:
	call	CHGRAM0
	ld	a,(SYLBCNT)
	inc	a
	cp	1fh
	jp	nc,TALKERR
	ld	(SYLBCNT),a

	ld	a,(VOICED)
	or	08h
	ld	d,a
	ld	a,(TDIFF)
	ld	e,a
	or	08h

	ld	a,(VOLUME)
	ld	c,a

SYLBLP1:
	ld	(hl),d		;bit7-3=time=1, bit2=qmag=0, bit1=S.I=0, bit0=V/UV
	inc	hl
	ld	b,5
SYLBLP2:
	ld	(hl),0		;fn=bn=0
	inc	hl
	djnz	SYLBLP2
	call	CHKPDIFF
	or	c		;volume
	ld	(hl),a		;bit7-4=amp, bit3=FV=0, bit2-0=pitch=0

	inc	hl
	dec	e
	jr	nz,SYLBLP1

	ld	a,(PDIFF)
	or	e
	jr	z,SYLBEND
	inc	e

	jr	SYLBLP1

SYLBEND:

	call	CHGROM0

	ld	(OUTADR),hl
	ld	(hl),0
;	ld	a,FRAMES
;	ld	(TDIFF),a
	ret


;geminated consonant
GEMINATE:
	push	hl
	push	de

GEMLP1:
	dec	e
	jp	z,TALKERR
	ld	a,(hl)
	inc	hl
	cp	' '
	jr	z,GEMLP1

	cp	'.'
	jp	z,TALKERR
	cp	':'
	jp	z,TALKERR
	cp	'?'
	jp	z,TALKERR

	pop	de
	pop	hl
	dec	e

	sub	'*'
	jr	nz,GEMNZ
	ld	(VOICED),a
	inc	hl
	dec	e
GEMNZ:
	push	hl
	push	de
	call	GETOUTADR
	ld	a,FRAMES
	ld	(TDIFF),a
	call	ANAMML
	call	CHGRAM0
	call	NOSOUND1
	call	CHGROM0

	ld	(OUTADR),hl
	ld	(hl),0

	call	CHGRAM0
	ld	a,(SYLBCNT)
	inc	a
	cp	1fh
	jp	nc,TALKERR
	ld	(SYLBCNT),a
	call	CHGROM0

	pop	de
	pop	hl

	ld	a,(hl)
	inc	hl
	cp	27h		;apostrophe
	jp	z,APOST
	cp	' '
	jp	z,SPACE
	or	20h
	cp	'x'
	jr	z,CON_X
	dec	hl
	jp	EXTLP2


CON_X:
	ld	a,01h
	ld	(VOICED),a
	ld	a,e
	dec	a
	jr	z,XZ
	ld	a,(hl)
	sub	'*'
	jr	nz,XNZ
	ld	(VOICED),a	;a=0
	inc	hl
XNZ:
	ld	a,e
	dec	a
	jr	z,XZ
	ld	a,(hl)
	or	20h
	cp	'x'
	jp	z,TALKERR
XZ:
	push	hl
	push	de
	call	GETOUTADR
	ld	a,FRAMES
	ld	(TDIFF),a
	call	ANAMML
	ld	de,M_MA+1
	call	PUT7
	call	SYLLABLE
	pop	de
	pop	hl
	dec	e
	jp	z,EXTEND

;check xqx
	push	hl
	push	de
XLP1:
	dec	e
	jr	z,XEND
	ld	a,(hl)
	inc	hl
	cp	27h		;apostrophe
	jr	z,XLP1
	cp	'*'
	jr	z,XLP1
	or	20h
	cp	'q'
	jr	nz,XEND

	inc	e
XLP22:
	dec	e
	jr	z,XEND
	ld	a,(hl)
	inc	hl
	cp	27h		;apostrophe
	jr	z,XLP22
	cp	'*'
	jr	z,XLP22
	or	20h
	cp	'x'
	jp	z,TALKERR

XEND:
	pop	de
	pop	hl


	jp	VOWEND


EXTEND:
	call	CHGRAM0
	ld	hl,(WORK)
	call	CHGROM0
	ld	a,l
	or	a
	jp	z,TALKERR

	ld	a,0feh
;	jr	TALKOUT

TALKOUT:
	ld	(COMMAND),a	;0-4,fe


;mode selection data
;bit2: 0=10ms/frame, 1=20ms/frame
;bit10: 00=normal, 01=slow, 10=fast
;123456->012345->201645
	call	CHGRAM0
	ld	a,(SPEED)	;0-5
	ld	hl,SPDTBL
	ld	b,0
	ld	c,a
	add	hl,bc
	ld	a,(hl)
	out	(0e2h),a	;mode selection

;command data
	ld	a,(COMMAND)
	out	(0e3h),a
	cp	0feh
	jr	nz,TALKEND	;internal message

;external message
	ld	hl,WORK
EXTMESLP:
	call	OUT7
	ld	a,(hl)
	or	a
	jr	nz,EXTMESLP

TALKEND:
	call	CHGROM0
	ld	a,(KEYFLG)
	and	0efh
	ld	(KEYFLG),a
	ld	a,20h
	ret


;input: de=voice data address, hl=output address
;output: hl=output end+1
;destroy: af,bc,de,af',bc',d',hl'
PUT7:
	xor	a		;first data
	push	af

PUT7LP1:
	push	de
	push	hl
	xor	a
	ld	(QMAG),a

;set qmag factor
	ld	hl,F1
	ld	bc,0500h	;c=no residual?

	call	CHGRAM0
PUT7LP2:
	ld	a,(de)
	sub	(hl)
	inc	hl
	ld	(hl),a
	or	c
	ld	c,a

	ld	a,(hl)
	add	a,16
	cp	32
	jr	c,NOQMAG1	;-16<=fdiff<=15
	ld	a,04h
	ld	(QMAG),a
NOQMAG1:
	inc	de
	inc	de
	inc	de
	inc	de
	inc	de
	inc	hl

	ld	a,(de)
	sub	(hl)
	inc	hl
	ld	(hl),a
	or	c
	ld	c,a

	ld	a,(hl)
	add	a,4
	cp	8
	jr	c,NOQMAG2	;-4<=bdiff<=3
	ld	a,04h
	ld	(QMAG),a
NOQMAG2:
	dec	de
	dec	de
	dec	de
	dec	de
	inc	hl
	djnz	PUT7LP2

	pop	hl
	pop	de
	pop	af
	jr	z,PUT7Z		;first data?

;	ld	a,(PDIFF)
;	or	c

	ld	a,c
	or	a


	jp	z,PUT7END

PUT7Z:
	ld	a,01h
	or	a
	push	af		;z=0
	push	de

	ld	a,(QMAG)
	or	a
	ld	bc,0ff0h	;-16<=f<=15
	ld	de,03fch	;-4<=b<=3
	jr	z,NOQMAG3
	ld	bc,1ee0h	;-32<=f<=30
	ld	de,06f8h	;-8<=b<=6
NOQMAG3:
	exx

	ld	d,a		;qmag
	call	CHGROM0

	ld	hl,F1DIFF
	ld	b,10

PUT7LP3:
	call	CHGRAM0
	ld	c,(hl)		;diff
	call	CHGROM0
	ld	a,c

	exx
	or	a
	jp	p,PUT7PLS1
;diff<0
	cp	c
	jr	nc,DIFFEND
	ld	a,c
	jr	DIFFEND

;diff>=0
PUT7PLS1:
	cp	b
	jr	c,DIFFEND
	ld	a,b
DIFFEND:
	exx

	inc	d
	dec	d
	jr	z,NOQMAG4	;qmag=0?
	or	a
	jp	p,PUT7PLS2
	inc	a
PUT7PLS2:
	and	0feh
NOQMAG4:

	dec	hl
	ld	c,a
	call	CHGRAM0
	ld	a,(hl)
	add	a,c
	ld	(hl),a
	call	CHGROM0

	ld	a,d		;qmag=0?
	or	a
	jr	z,NOQMAG5
	sra	c
NOQMAG5:
	ld	a,c

	inc	hl
	ld	(hl),a
	inc	hl
	inc	hl
	exx

;exchange bc and de
	push	bc
	push	de
	pop	bc
	pop	de
	exx

	djnz	PUT7LP3

	call	CHGRAM0
	ld	a,(VOICED)	;bit0=V/UV
	or	d		;bit2=qmag
	or	08h		;bit7-3=time, bit1=S.I
	ld	d,a
	call	CHGROM0
	ld	a,d

	exx
	ld	(hl),a		;bit7-3=time, bit2=qmag, bit1=S.I, bit0=V/UV
	inc	hl
	call	CHGRAM0
	ld	de,F1DIFF
	ld	b,5
PUT7LP4:
	ld	a,(de)
	and	1fh
	add	a,a
	add	a,a
	add	a,a
	ld	c,a
	inc	de
	inc	de
	ld	a,(de)
	and	07h
	inc	de
	inc	de
	or	c
	ld	(hl),a
	inc	hl
	djnz	PUT7LP4

	call	CHKPDIFF
	ld	b,a
	ld	a,(VOLUME)
	or	b


	ld	(hl),a		;bit7-4=amp, bit3=FV, bit2-0=pitch
	inc	hl

	ld	a,(TDIFF)
	dec	a
	jr	nz,PUT7NZ
	inc	a
PUT7NZ:
	ld	(TDIFF),a

	call	CHGROM0

	pop	de
	jp	PUT7LP1


PUT7END:
;	dec	hl
;	ld	(hl),0c0h	;bit7-4=amp, bit3=FV, bit2-0=pitch
;	inc	hl
	ret


;analyze MML for singing
;destroy: af,bc
ANAMML:
	call	CHGRAM0
	ld	a,(SHARP)
	or	a
	jp	z,MMLEND

	push	hl
	push	de


	ld	hl,(MMLADR)
MMLLP1:
	ld	a,(MMLLEN)
	or	a
	jp	z,TALKERR
	dec	a
	ld	(MMLLEN),a
	
	ld	a,(hl)
	inc	hl
	ld	(MMLADR),hl

	cp	' '
	jr	z,MMLLP1

	or	20h
	cp	'r'
	jp	z,SINGR

	ld	b,a
	xor	a
	ld	(REST),a
	ld	a,b

	cp	'o'
	jp	z,SINGO
	cp	'l'
	jp	z,SINGL
	cp	'v'
	jp	z,SINGV

	sub	'a'
	cp	'g'-'a'+1
	jp	nc,TALKERR
	ld	hl,TONETBL
	ld	d,0
	ld	e,a
	add	hl,de
	ld	e,(hl)

;check +-
	ld	a,(MMLLEN)
	or	a
	jr	z,MMLZ
	ld	hl,(MMLADR)
	ld	a,(hl)
	cp	'+'
	jr	nz,NOTPLUS
	inc	e
	jr	MMLP
NOTPLUS:
	cp	'-'
	jr	nz,MMLZ
	dec	e
	jp	p,MMLP
	dec	d
MMLP:
	inc	hl
	ld	(MMLADR),hl
	ld	a,(MMLLEN)
	dec	a
	ld	(MMLLEN),a

MMLZ:
	ld	hl,SCALE-12+1
	add	hl,de
	ld	a,(OCTAVE)	;2~5
	dec	a
	ld	de,12
MMLLP2:
	add	hl,de
	dec	a
	jr	nz,MMLLP2
	ld	a,(MMLPTCH)
	ld	e,a
	ld	a,(hl)
	sub	e
	ld	(PDIFF),a

;check number
	ld	hl,(MMLADR)
SETLEN:
	call	GETNUM
	ld	(MMLADR),hl
	ld	de,(LENGTH)	;e=(LENGTH)
	call	c,NUM2LEN

	ld	a,(TDIFF)
	add	a,e
	sub	FRAMES
	ld	(TDIFF),a

	pop	de
	pop	hl
MMLEND:
	call	CHGROM0
	ret

;O command
SINGO:
	call	GETNUM
	jp	nc,TALKERR
	cp	2
	jr	nc,SINGONC
	ld	a,2
SINGONC:
	cp	5+1
	jr	c,SINGOC
	ld	a,5
SINGOC:
	ld	(OCTAVE),a
	jp	MMLLP1

;V command
SINGV:
	call	GETNUM
	jp	nc,TALKERR
	cp	0fh+1
	jr	c,SINGVEND
	ld	a,0fh
SINGVEND:
	add	a,a		;*2
	add	a,a		;*4
	add	a,a		;*8
	add	a,a		;*16
	ld	(VOLUME),a
	jp	MMLLP1

;L command
SINGL:
	call	GETNUM
	jp	nc,TALKERR
	call	NUM2LEN
	ld	a,e
	ld	(LENGTH),a
	jp	MMLLP1

SINGR:
	ld	a,(REST)
	or	a
	jp	z,TALKERR
	jp	SETLEN


;convert note-number to frames
;input: a=number, hl=MML address
;output: e=frames, hl=next MML address
;destroy: af,bc,de
NUM2LEN:
	or	a
	jr	nz,NUMGT0
	inc	a
NUMGT0:
	cp	16+1
	jr	c,NUMLE16
	ld	a,16
NUMLE16:
	push	hl
	ld	hl,FRAMES2*4
	call	DIVI		;de=FRAMES2*4/a<=0100h
	pop	hl

	ld	a,(MMLLEN)
	ld	b,a		;;
	or	a
	ret	z
	ld	a,(hl)
	cp	'.'
	ret	nz
	inc	hl
	dec	b		;;
	ld	a,b
	ld	(MMLLEN),a
	ld	a,e		;
	rra			;c-flag=0
	add	a,e		;*1.5
	ret	c
	ld	e,a
	ret


;get 2-digit number
;input: hl=MML address
;output: hl=next MML address, a=data, c-flag(0=no data), (MMLLEN)
;destroy: f,de

GETNUMLP1:
	ex	de,hl
	dec	(hl)
	ex	de,hl
	inc	hl

GETNUM:
	ld	de,MMLLEN
	ld	a,(de)
	or	a
	ret	z

	ld	a,(hl)
	cp	' '
	jr	z,GETNUMLP1

	sub	'0'
	cp	'9'-'0'+1
	ret	nc

	ld	d,a

GETNUMLP2:
	inc	hl
	ld	a,(MMLLEN)
	dec	a
	ld	(MMLLEN),a
	ld	a,d
	ret	z		;c-flag=1

	ld	a,(hl)
	cp	' '
	jr	z,GETNUMLP2

	ld	a,(hl)
	sub	'0'
	cp	'9'-'0'+1
	ld	e,a
	ld	a,d
	ccf
	ret	c

	ld	a,(MMLLEN)
	dec	a
	ld	(MMLLEN),a

	ld	a,d
	add	a,a		*2
	add	a,a		*4
	add	a,d		*5
	add	a,a		*10
	add	a,e

	inc	hl
	scf
	ret


;check vowel
;input: a (aiueo)
;output: c (0-4), z-flag(1=ok)
;destroy: f
CHKVOW:
	ld	c,0
	cp	'a'
	ret	z
	inc	c
	cp	'i'
	ret	z
	inc	c
	cp	'u'
	ret	z
	inc	c
	cp	'e'
	ret	z
	inc	c
	cp	'o'
	ret


;get (OUTADR)
;output: hl
;destroy: a
GETOUTADR:
	call	CHGRAM0
	ld	hl,(OUTADR)
	jp	CHGROM0


;de=hl/a, (a!=0)
;input: a, hl
;output: de
;destroy: af,bc
DIVI:
	ld	de,0
	ld	b,8
DIVILP1:
	inc	b
	add	a,a
	jr	nc,DIVILP1
	rra
	ld	c,a
DIVILP2:
	ld	a,h
	jr	nc,DIVINC1
	sub	c
	or	a		;reset c-flag
	jr	DIVINC2
DIVINC1:
	sub	c
	jr	nc,DIVINC2
	add	a,c
DIVINC2:
	ld	h,a
	ccf
	rl	e
	rl	d
	add	hl,hl
	djnz	DIVILP2

	ret	nc
	inc	de		;round up
	ret


;check (PDIFF) and get -4<=a<=3 (111...011)
;output: a=value, (PDIFF)=residual, (MMLPTCH)
;destroy: f,b
CHKPDIFF:
	ld	a,(PDIFF)
	ld	b,a
	or	a
	jp	m,DECPITCH2
	cp	03h+1
	jr	c,PITCHEND2
	ld	b,03h
	jr	PITCHEND2
DECPITCH2:
	cp	0fch
	jr	nc,PITCHEND2
	ld	b,0fch
PITCHEND2:
	sub	b
	ld	(PDIFF),a
	ld	a,(MMLPTCH)
	add	a,b
	ld	(MMLPTCH),a
	ld	a,b
	and	07h
	ret


;make no sound with pitch after address
;input: hl=output address
;output: hl,(OUTADR)=next output address
;destroy: af,bc,de
;(RAM)
NOSOUND1:
	ex	de,hl

	ld	a,(TDIFF)
	ld	b,a
NSND1LP:
	push	bc
	ld	hl,NODATA
	ld	bc,6
	ldir
	call	CHKPDIFF
	ld	(de),a
	inc	de
	pop	bc
	djnz	NSND1LP

	ex	de,hl
	ld	(hl),0
	ld	(OUTADR),hl
	ret


;make no sound before address
;input: b=length, de=output address
;destroy: f,hl
NOSOUND2:
	push	de
	push	bc
	ex	de,hl
	dec	hl
	ld	de,0-7
	call	CHGRAM0
NSND2LP:
	ld	a,(hl)
	and	0fh
	ld	(hl),a
	add	hl,de
	djnz	NSND2LP
	call	CHGROM0
	pop	bc
	pop	de
	ret


;input: hl
;output: hl=hl+7
;destroy: af,bc
OUT7:
	ld	b,7
OUT7LP1:
	ld	c,0
OUT7LP2:
	inc	c
	jp	z,OUT7ERR	;time out
	in	a,(0e0h)	;status register
	bit	4,a
	jr	nz,OUT7ERR	;ERR=1
	and	40h
	call	z,CHGROM0
	call	z,CHGRAM0
	jr	z,OUT7LP2	;wait for REQ=1
	ld	a,(hl)
	out	(0e0h),a
	inc	hl
	djnz	OUT7LP1
	ret

OUT7ERR:
	ld	a,(KEYFLG)
	or	10h
	ld	(KEYFLG),a
	ld	hl,(STACK)
	call	CHGROM0
	ld	sp,hl
	ld	a,20h
	ret


;change 0000-3fff to internal RAM
;destroy: a
CHGRAM0:
	di
	ld	a,2dh		;0000-3fff:internal RAM, 4000-7fff:VOICE ROM
;	ld	a,7dh		;for test
	out	(0f0h),a
	ret


;change 0000-3fff to BASIC ROM
;destroy: a
CHGROM0:
	ld	a,21h		;0000-3fff:BASIC ROM, 4000-7fff:VOICE ROM
;	ld	a,77h		;for test
	out	(0f0h),a
	ei
	ret


;;voice table
;speed
SPDTBL:
	db	02h, 00h, 01h, 06h, 04h, 05h

;vowel
VOWTBL:
	dw	M_A, M_I, M_U, M_E, M_O
	dw	F_A, F_I, F_U, F_E, F_O

BTBL:
	dw	M_BA, M_BI, M_BU, M_BE, M_BO
	dw	F_BA, F_BI, F_BU, F_BE, F_BO

DTBL:
	dw	M_DA, M_DI, M_DU, M_DE, M_DO
	dw	F_DA, F_DI, F_DU, F_DE, F_DO

GTBL:
	dw	M_GA, M_GI, M_GU, M_GE, M_GO
	dw	F_GA, F_GI, F_GU, F_GE, F_GO

YTBL:
	dw	M_YA, M_YI, M_YU, M_YE, M_YO
	dw	F_YA, F_YI, F_YU, F_YE, F_YO

RTBL:
	dw	M_RA, M_RI, M_RU, M_RE, M_RO
	dw	F_RA, F_RI, F_RU, F_RE, F_RO

MTBL:
	dw	M_MA, M_MI, M_MU, M_ME, M_MO
	dw	F_MA, F_MI, F_MU, F_ME, F_MO

NTBL:
	dw	M_NA, M_NI, M_NU, M_NE, M_NO
	dw	F_NA, F_NI, F_NU, F_NE, F_NO

ZTBL:
	dw	M_ZA, M_ZI, M_ZU, M_ZE, M_ZO
	dw	F_ZA, F_ZI, F_ZU, F_ZE, F_ZO

STBL:
	dw	M_SA, M_SI, M_SU, M_SE, M_SO
	dw	F_SA, F_SI, F_SU, F_SE, F_SO

HTBL:
	dw	M_HA, M_HI, M_HU, M_HE, M_HO
	dw	F_HA, F_HI, F_HU, F_HE, F_HO

CTBL:
	dw	M_CA, M_CI, M_CU, M_CE, M_CO
	dw	F_CA, F_CI, F_CU, F_CE, F_CO


;voice data

;648,1028,2560Hz
M_A:
	db	0, 28, 0, 6, 5
	db	6, 0, 6, 0-5, 0-7

;360,2226,2834Hz
M_I:
	db	0, 17, 0, 8, 18
	db	6, 0, 6, 0-5, 0-7

;322,1112,2215Hz
M_U:
	db	0, 15, 0, 3, 6
	db	6, 0, 6, 0-5, 0-7

;524,1741,2340Hz
M_E:
	db	0, 24, 0, 4, 14
	db	6, 0-3, 6, 0-6, 0-8

;447,781,2500Hz
M_O:
	db	0, 21, 0, 6, 0
	db	6, 0, 6, 0-5, 0-7

;1057,1787,2962Hz
F_A:
	db	0, 37, 0, 9, 15
	db	6, 0, 6, 0-5, 0-7

;355,2992,3906Hz
F_I:
	db	0, 17, 0, 14, 24
	db	6, 0, 6, 0-5, 0-7

;383,1719,2749Hz
F_U:
	db	0, 19, 0, 7, 14
	db	6, 0, 6, 0-5, 0-7

;598,2724,3511Hz
F_E:
	db	0, 27, 0, 12, 22
	db	6, 0, 6, 0-5, 0-7

;561,851,3142Hz
F_O:
	db	0, 25, 0, 10, 1
	db	6, 0, 6, 0-5, 0-7

;150,800,1750Hz->
M_BA:
M_BI:
M_BU:
M_BE:
M_BO:
F_BA:
F_BI:
F_BU:
F_BE:
F_BO:
	db	1
	db	0, 1, 0, 0-1, 0
	db	6, 0, 6, 0-5, 0-7

;150,1800,2600Hz->
M_DA:
M_DI:
M_DU:
M_DE:
M_DO:
F_DA:
F_DI:
F_DU:
F_DE:
F_DO:
	db	1
	db	0, 1, 0, 6, 15
	db	0, 0, 0, 0-7, 0-9

;150,2350,2750Hz->
M_GA:
M_GI:
M_GU:
M_GE:
M_GO:
F_GA:
F_GI:
F_GU:
F_GE:
F_GO:
	db	1
	db	0, 1, 0, 6, 10
	db	6, 2, 6, 0-5, 0-9

;300,600,2200Hz->
M_WA:
	db	3
	db	0, 14, 0, 3, 0-5
	db	6, 0, 6, 0-5, 0-7
	db	0, 20, 0, 4, 0-1
	db	6, 0, 6, 0-5, 0-7
	db	0, 25, 0, 5, 2
	db	6, 0, 6, 0-5, 0-7

F_WA:
	db	3
	db	0, 14, 0, 3, 0-5
	db	6, 0, 6, 0-5, 0-7
	db	0, 25, 0, 5, 4
	db	6, 0, 6, 0-5, 0-7
	db	0, 32, 0, 7, 10
	db	6, 0, 6, 0-5, 0-7


;300,2200,3050Hz->
M_YA:
	db	8
	db	0, 14, 0, 9, 18
	db	6, 0, 6, 0-5, 0-7
	db	0, 16, 0, 9, 17
	db	6, 0, 6, 0-5, 0-7
	db	0, 18, 0, 9, 16
	db	6, 0, 6, 0-5, 0-7
	db	0, 20, 0, 8, 14
	db	6, 0, 6, 0-5, 0-7
	db	0, 22, 0, 8, 13
	db	6, 0, 6, 0-5, 0-7
	db	0, 24, 0, 8, 11
	db	6, 0, 6, 0-5, 0-7
	db	0, 25, 0, 7, 9
	db	6, 0, 6, 0-5, 0-7
	db	0, 26, 0, 7, 7
	db	6, 0, 6, 0-5, 0-7

M_YI:
M_YU:
	db	8
	db	0, 14, 0, 9, 18
	db	6, 0, 6, 0-5, 0-7
	db	0, 14, 0, 9, 17
	db	6, 0, 6, 0-5, 0-7
	db	0, 14, 0, 8, 16
	db	6, 0, 6, 0-5, 0-7
	db	0, 15, 0, 7, 15
	db	6, 0, 6, 0-5, 0-7
	db	0, 15, 0, 7, 13
	db	6, 0, 6, 0-5, 0-7
	db	0, 15, 0, 6, 12
	db	6, 0, 6, 0-5, 0-7
	db	0, 15, 0, 5, 10
	db	6, 0, 6, 0-5, 0-7
	db	0, 15, 0, 4, 8
	db	6, 0, 6, 0-5, 0-7

M_YE:
	db	4
	db	0, 14, 0, 9, 18
	db	6, 0, 6, 0-5, 0-7
	db	0, 17, 0, 8, 17
	db	6, 0, 6, 0-5, 0-7
	db	0, 20, 0, 7, 16
	db	6, 0, 6, 0-5, 0-7
	db	0, 22, 0, 6, 15
	db	6, 0, 6, 0-5, 0-7

M_YO:
	db	8
	db	0, 14, 0, 9, 18
	db	6, 0, 6, 0-5, 0-7
	db	0, 15, 0, 9, 17
	db	6, 0, 6, 0-5, 0-7
	db	0, 16, 0, 8, 15
	db	6, 0, 6, 0-5, 0-7
	db	0, 17, 0, 8, 13
	db	6, 0, 6, 0-5, 0-7
	db	0, 18, 0, 8, 11
	db	6, 0, 6, 0-5, 0-7
	db	0, 19, 0, 7, 9
	db	6, 0, 6, 0-5, 0-7
	db	0, 20, 0, 7, 6
	db	6, 0, 6, 0-5, 0-7
	db	0, 21, 0, 6, 3
	db	6, 0, 6, 0-5, 0-7


;300,2200,3050Hz->
F_YA:
	db	8
	db	0, 14, 0, 9, 18
	db	6, 0, 6, 0-5, 0-7
	db	0, 19, 0, 9, 18
	db	6, 0, 6, 0-5, 0-7
	db	0, 23, 0, 9, 17
	db	6, 0, 6, 0-5, 0-7
	db	0, 26, 0, 9, 17
	db	6, 0, 6, 0-5, 0-7
	db	0, 29, 0, 9, 17
	db	6, 0, 6, 0-5, 0-7
	db	0, 31, 0, 9, 16
	db	6, 0, 6, 0-5, 0-7
	db	0, 33, 0, 9, 16
	db	6, 0, 6, 0-5, 0-7
	db	0, 35, 0, 9, 15
	db	6, 0, 6, 0-5, 0-7

;300,2200,3050Hz->
F_YI:
F_YU:
	db	8
	db	0, 14, 0, 9, 18
	db	6, 0, 6, 0-5, 0-7
	db	0, 15, 0, 9, 18
	db	6, 0, 6, 0-5, 0-7
	db	0, 15, 0, 9, 17
	db	6, 0, 6, 0-5, 0-7
	db	0, 16, 0, 9, 17
	db	6, 0, 6, 0-5, 0-7
	db	0, 16, 0, 8, 16
	db	6, 0, 6, 0-5, 0-7
	db	0, 17, 0, 8, 16
	db	6, 0, 6, 0-5, 0-7
	db	0, 18, 0, 8, 15
	db	6, 0, 6, 0-5, 0-7
	db	0, 18, 0, 8, 15
	db	6, 0, 6, 0-5, 0-7


;300,2200,3050Hz->
F_YE:
	db	8
	db	0, 14, 0, 9, 18
	db	6, 0, 6, 0-5, 0-7
	db	0, 16, 0, 10, 19
	db	6, 0, 6, 0-5, 0-7
	db	0, 18, 0, 10, 19
	db	6, 0, 6, 0-5, 0-7
	db	0, 20, 0, 10, 20
	db	6, 0, 6, 0-5, 0-7
	db	0, 21, 0, 11, 20
	db	6, 0, 6, 0-5, 0-7
	db	0, 23, 0, 11, 21
	db	6, 0, 6, 0-5, 0-7
	db	0, 24, 0, 11, 21
	db	6, 0, 6, 0-5, 0-7
	db	0, 25, 0, 12, 22
	db	6, 0, 6, 0-5, 0-7

F_YO:
	db	8
	db	0, 14, 0, 9, 18
	db	6, 0, 6, 0-5, 0-7
	db	0, 16, 0, 9, 17
	db	6, 0, 6, 0-5, 0-7
	db	0, 18, 0, 9, 15
	db	6, 0, 6, 0-5, 0-7
	db	0, 19, 0, 10, 14
	db	6, 0, 6, 0-5, 0-7
	db	0, 21, 0, 10, 12
	db	6, 0, 6, 0-5, 0-7
	db	0, 22, 0, 10, 10
	db	6, 0, 6, 0-5, 0-7
	db	0, 23, 0, 10, 7
	db	6, 0, 6, 0-5, 0-7
	db	0, 24, 0, 10, 4
	db	6, 0, 6, 0-5, 0-7

;150,1800,2600Hz
M_RA:
F_RA:
M_RI:
M_RE:
F_RI:
F_RE:
	db	1
	db	0, 1, 0, 6, 14
	db	6, 0, 6, 0-5, 0-7


M_RU:
	db	4
	db	0, 20, 0, 0-2, 9
	db	6, 0, 6, 0-5, 0-7
	db	0, 19, 0, 0-1, 8
	db	6, 0, 6, 0-5, 0-7
	db	0, 18, 0, 1, 7
	db	6, 0, 6, 0-5, 0-7
	db	0, 17, 0, 2, 7
	db	6, 0, 6, 0-5, 0-7

M_RO:
	db	4
	db	0, 20, 0, 0-2, 9
	db	6, 0, 6, 0-5, 0-7
	db	0, 21, 0, 0, 7
	db	6, 0, 6, 0-5, 0-7
	db	0, 21, 0, 2, 5
	db	6, 0, 6, 0-5, 0-7
	db	0, 21, 0, 4, 2
	db	6, 0, 6, 0-5, 0-7

F_RU:
	db	4
	db	0, 20, 0, 0-2, 9
	db	6, 0, 6, 0-5, 0-7
	db	0, 20, 0, 1, 10
	db	6, 0, 6, 0-5, 0-7
	db	0, 19, 0, 3, 12
	db	6, 0, 6, 0-5, 0-7
	db	0, 19, 0, 5, 13
	db	6, 0, 6, 0-5, 0-7

;425,1300,1600Hz
F_RO:
	db	4
	db	0, 20, 0, 0-2, 9
	db	6, 0, 6, 0-5, 0-7
	db	0, 22, 0, 2, 7
	db	6, 0, 6, 0-5, 0-7
	db	0, 23, 0, 5, 5
	db	6, 0, 6, 0-5, 0-7
	db	0, 24, 0, 7, 3
	db	6, 0, 6, 0-5, 0-7

;275,900,2200Hz
M_MA:
M_MI:
M_MU:
M_ME:
M_MO:
F_MA:
F_MI:
F_MU:
F_ME:
F_MO:
	db	1
	db	0, 12, 0, 3, 2
	db	6, 0, 6, 0, 0

;275,1700,2600Hz
M_NA:
M_NI:
M_NU:
M_NE:
M_NO:
F_NA:
F_NI:
F_NU:
F_NE:
F_NO:
	db	1
	db	0, 12, 0, 6, 14
	db	6, 0, 6, 0, 0

M_ZA:
M_ZI:
M_ZU:
M_ZE:
M_ZO:
F_ZA:
F_ZI:
F_ZU:
F_ZE:
F_ZO:
;d:150,1800,2600Hz->
;z:200,1800,2600Hz
	db	3
	db	0, 1, 0, 6, 15
	db	0, 0, 0, 0-7, 0-9
	db	0, 1, 6, 6, 15
	db	0, 2, 0, 0-2, 0-4
	db	0, 7, 6, 6, 15
	db	0, 2, 0, 0-2, 0-4

M_SA:
M_SI:
M_SU:
M_SE:
M_SO:
F_SA:
F_SI:
F_SU:
F_SE:
F_SO:
;200,1800,2600Hz
	db	3
	db	0, 7, 6, 6, 15
	db	0, 2, 0, 0-2, 0-4
	db	0, 7, 6, 6, 15
	db	0, 2, 0, 0-2, 0-4
	db	0, 7, 6, 6, 15
	db	0, 2, 0, 0-2, 0-4


M_HA:
	db	4
	db	0, 28, 0, 6, 5
	db	6, 0, 6, 0-5, 0-7
	db	0, 28, 0, 6, 5
	db	6, 0, 6, 0-5, 0-7
	db	0, 28, 0, 6, 5
	db	6, 0, 6, 0-5, 0-7
	db	0, 28, 0, 6, 5
	db	6, 0, 6, 0-5, 0-7

M_HI:
	db	4
	db	0, 17, 0, 8, 18
	db	6, 0, 6, 0-5, 0-7
	db	0, 17, 0, 8, 18
	db	6, 0, 6, 0-5, 0-7
	db	0, 17, 0, 8, 18
	db	6, 0, 6, 0-5, 0-7
	db	0, 17, 0, 8, 18
	db	6, 0, 6, 0-5, 0-7

M_HU:
	db	4
	db	0, 15, 0, 3, 6
	db	6, 0, 6, 0-5, 0-7
	db	0, 15, 0, 3, 6
	db	6, 0, 6, 0-5, 0-7
	db	0, 15, 0, 3, 6
	db	6, 0, 6, 0-5, 0-7
	db	0, 15, 0, 3, 6
	db	6, 0, 6, 0-5, 0-7

M_HE:
	db	6
	db	0, 24, 0, 4, 14
	db	6, 0-3, 6, 0-6, 0-8
	db	0, 24, 0, 4, 14
	db	6, 0-3, 6, 0-6, 0-8
	db	0, 24, 0, 4, 14
	db	6, 0-3, 6, 0-6, 0-8
	db	0, 24, 0, 4, 14
	db	6, 0-3, 6, 0-6, 0-8
	db	0, 24, 0, 4, 14
	db	6, 0-3, 6, 0-6, 0-8
	db	0, 24, 0, 4, 14
	db	6, 0-3, 6, 0-6, 0-8

M_HO:
	db	4
	db	0, 21, 0, 6, 0
	db	6, 0, 6, 0-5, 0-7
	db	0, 21, 0, 6, 0
	db	6, 0, 6, 0-5, 0-7
	db	0, 21, 0, 6, 0
	db	6, 0, 6, 0-5, 0-7
	db	0, 21, 0, 6, 0
	db	6, 0, 6, 0-5, 0-7

M_HY:
	db	4
	db	0, 14, 0, 9, 18
	db	6, 0, 6, 0-5, 0-7
	db	0, 14, 0, 9, 18
	db	6, 0, 6, 0-5, 0-7
	db	0, 14, 0, 9, 18
	db	6, 0, 6, 0-5, 0-7
	db	0, 14, 0, 9, 18
	db	6, 0, 6, 0-5, 0-7

F_HA:
	db	4
	db	0, 37, 0, 9, 15
	db	6, 0, 6, 0-5, 0-7
	db	0, 37, 0, 9, 15
	db	6, 0, 6, 0-5, 0-7
	db	0, 37, 0, 9, 15
	db	6, 0, 6, 0-5, 0-7
	db	0, 37, 0, 9, 15
	db	6, 0, 6, 0-5, 0-7

F_HI:
	db	4
	db	0, 17, 0, 14, 24
	db	6, 0, 6, 0-5, 0-7
	db	0, 17, 0, 14, 24
	db	6, 0, 6, 0-5, 0-7
	db	0, 17, 0, 14, 24
	db	6, 0, 6, 0-5, 0-7
	db	0, 17, 0, 14, 24
	db	6, 0, 6, 0-5, 0-7

F_HU:
	db	4
	db	0, 19, 0, 7, 14
	db	6, 0, 6, 0-5, 0-7
	db	0, 19, 0, 7, 14
	db	6, 0, 6, 0-5, 0-7
	db	0, 19, 0, 7, 14
	db	6, 0, 6, 0-5, 0-7
	db	0, 19, 0, 7, 14
	db	6, 0, 6, 0-5, 0-7

F_HE:
	db	4
	db	0, 27, 0, 12, 22
	db	6, 0, 6, 0-5, 0-7
	db	0, 27, 0, 12, 22
	db	6, 0, 6, 0-5, 0-7
	db	0, 27, 0, 12, 22
	db	6, 0, 6, 0-5, 0-7
	db	0, 27, 0, 12, 22
	db	6, 0, 6, 0-5, 0-7

F_HO:
	db	4
	db	0, 25, 0, 10, 1
	db	6, 0, 6, 0-5, 0-7
	db	0, 25, 0, 10, 1
	db	6, 0, 6, 0-5, 0-7
	db	0, 25, 0, 10, 1
	db	6, 0, 6, 0-5, 0-7
	db	0, 25, 0, 10, 1
	db	6, 0, 6, 0-5, 0-7

F_HY:
	db	4
	db	0, 14, 0, 9, 18
	db	6, 0, 6, 0-5, 0-7
	db	0, 14, 0, 9, 18
	db	6, 0, 6, 0-5, 0-7
	db	0, 14, 0, 9, 18
	db	6, 0, 6, 0-5, 0-7
	db	0, 14, 0, 9, 18
	db	6, 0, 6, 0-5, 0-7

M_CA:
M_CI:
M_CU:
M_CE:
M_CO:
F_CA:
F_CI:
F_CU:
F_CE:
F_CO:
;d:150,1800,2600Hz->
;3:200,1300,2400Hz
	db	1
	db	0, 1, 6, 6, 15
	db	0-4, 0, 0-2, 0-5, 0-7


;no sound
NODATA:
	db	09h, 00h, 00h, 00h, 00h, 00h


TONETBL:
;		a  b   c  d  e  f  g
	db	9, 11, 0, 2, 4, 5, 7


SCALE:
;		c-
	db	98
;o2		c/b+	c+/d-	d	d+/e-	e/f-	f/e+
	db	98,	98,	98,	98,	91,	84
;		f+/g-	g	g+a/-	a	a+/b-	b/c-
	db	77,	71,	66,	60,	55,	51

;o3		c/b+	c+/d-	d	d+/e-	e/f-	f/e+
	db	46,	42,	38,	34,	30,	27,
;		f+/g-	g	g+a/-	a	a+/b-	b/c-
	db	24,	21,	18,	15,	13,	10

;o4		c/b+	c+/d-	d	d+/e-	e/f-	f/e+
	db	8,	6,	4,	2,	0,	0-2,
;		f+/g-	g	g+a/-	a	a+/b-	b/c-
	db	0-3,	0-5,	0-6,	0-7,	0-9,	0-10

;o5		c/b+	c+/d-	d	d+/e-	e/f-	f/e+
	db	0-11,	0-12,	0-13,	0-14,	0-15,	0-16
;		f+/g-	g	g+a/-	a	a+/b-	b/c-
	db	0-17,	0-17,	0-18,	0-19,	0-19,	0-20
;		b+
	db	0-20



;VOICEROM end
_8000H:	ds	8000h-_8000H

	end
