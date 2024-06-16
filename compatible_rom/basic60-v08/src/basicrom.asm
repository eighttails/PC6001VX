;Compatible BASIC for PC-6001
; by AKIKAWA, Hisashi  2013-2024

;This software is redistributable under the LGPLv2.1 or any later version.


;subroutine entry address
BOOT	equ	0000h
CHKPAR	equ	0008h
ANADAT	equ	0010h
JPHK1	equ	0018h
CPHLDE	equ	0020h
CHKSGN	equ	0028h
JPHK2	equ	0038h
ERRSTR	equ	0384h
ERRIN	equ	038bh
OK	equ	0390h
EDIT	equ	0442h
CMDEND	equ	06eah
INTPRT2	equ	0709h
FTOI2	equ	0741h
PIEND	equ	0955h
CHKEXTR	equ	0a8fh
INPT1EX	equ	0a9bh
TOUPHL	equ	0beeh
ABTOF	equ	0d16h
INT1INC	equ	0de3h
INT1ARG	equ	0de4h
IN90H	equ	0e78h
OUT90H	equ	0e8fh
INTGRP	equ	0eb0h
INTKEY	equ	0eb5h
IKBF	equ	0ef6h
IKPOP	equ	0f2bh
INTTIM	equ	0f74h
GETCH	equ	0fbch
GETC	equ	0fc4h
GETKBF	equ	103ah
CLRKBF	equ	1058h
STICK	equ	1061h
PRTC	equ	1075h
PRTCH	equ	10aah
PATCH1	equ	10eah
SETCSR	equ	116dh
CSRON	equ	1179h
CSROFF	equ	1181h
Y2AD	equ	11b8h
XY2AD	equ	11cdh
DELLIN	equ	11dah
PRTCHXY	equ	1257h
SCRLUP	equ	1260h
SCRLDW	equ	12a9h
PRTFKEY	equ	12b5h
CHGMOD	equ	1390h
CHGDSP	equ	13edh
CHGACT	equ	140ch
AD2GAD	equ	1478h
CGROM	equ	14a0h
PRTCHAD	equ	14afh
GXY2SXY	equ	1578h
SXY2AD	equ	15bah
SETATT	equ	15c0h
GETCOLR	equ	15d5h
SETBO	equ	18c3h
PUTPRT	equ	1a1ch
CNVKANA	equ	1a4fh
ROPEN	equ	1a61h
GETCMT	equ	1a70h
RCLOSE	equ	1aaah
WOPEN	equ	1ab8h
PUTCMT	equ	1acch
CHK90H	equ	1aedh
WCLOSE	equ	1b06h
BLNKAST	equ	1b2ah
RLOFF	equ	1b49h
RLON	equ	1b4bh
OUTB0H	equ	1b54h
PLSTPS	equ	1b60h
PLSTOP	equ	1bb3h
SETPSG2	equ	1bbeh
SETPSG	equ	1bc5h
BELL	equ	1bcdh
JOYSTK	equ	1ca6h
PATCH2	equ	1cb4h
CNSMAIN	equ	1d52h
SETCNSL	equ	1d73h
SETC3	equ	1dbbh
CLS	equ	1dfbh
SCRMAIN	equ	1e39h
PLAY	equ	1eb3h
SCALE2	equ	2030h		;for mkII
SCALE	equ	204bh
JOYTBL	equ	2286h
CSRTBL	equ	2296h
GETFN	equ	2539h
CMTSKP	equ	254eh
FOUND	equ	2576h
PUTFN	equ	2583h
INPOPN	equ	259ah
PRTOPN	equ	25a8h
WAIT	equ	25e5h
PUTDEV	equ	26c7h
PUTPRNL	equ	2701h
PUTNLX	equ	272dh
PUTNL	equ	2739h
STOPESC	equ	274dh
CTLL	equ	2874h
INPTSCR	equ	28f9h
INPT1	equ	2905h
SETGXY	equ	2d13h
PSET	equ	2d47h
LINEBF	equ	2de4h
LINE	equ	2e1fh
LINEB	equ	2e35h
PAPOP	equ	2ef1h
PAINT	equ	2efah
PUTSINC	equ	30ceh
PUTS	equ	30cfh
GC	equ	310fh
RESSTK	equ	34f9h
DE2HL4	equ	3913h
PUTI1	equ	397ah		;for mkII
INLNUM	equ	3a99h
PUTI2	equ	3aa1h
I2TOA	equ	3aa5h
FTOA	equ	3aach
RNDPLS	equ	3bafh

;BASIC command
C_RUN	equ	0781h		;RUN
C_DATA	equ	07e0h		;DATA
C_REM	equ	07e2h		;REM
C_LPRT	equ	087ah		;LPRINT
C_PRT	equ	087eh		;PRINT
C_LOCA	equ	1cd2h		;LOCATE
C_CNSL	equ	1cf6h		;CONSOLE
C_COLR	equ	1d9bh		;COLOR
C_SCRN	equ	1e04h		;SCREEN
F_STCK	equ	2236h		;STICK()
F_STRG	equ	2256h		;STRIG()
C_LCPY	equ	22a6h		;LCOPY
C_PRST	equ	2d37h		;PRESET
C_PSET	equ	2d3ch		;PSET
C_CLR	equ	35a9h		;CLEAR

;work area
STOPFLG	equ	0fa18h		;03h=STOP,1bh=ESC
CMTSTAT	equ	0fa19h		;bit1=data received, bit4=error
INPDEV	equ	0fa1ah		;input device
CMTBUF	equ	0fa1dh		;CMT data
ASTSTAT	equ	0fa1eh		;cload asterisk status
BAUD	equ	0fa1fh		;baud rate (00=600baud, ff=1200baud)
HEIGHT	equ	0fa20h		;Y max+1
PORTB0H	equ	0fa27h		;port b0h
TMCNT	equ	0fa28h		;time count, fa28-fa2b
CONSOL4	equ	0fa2dh		;console 4th parameter
CSRBLNK	equ	0fa2eh		;0=cursor blink off, 1=on
CSRSTAT	equ	0fa2fh		;0=normal, ff=reversed
GRPWRK	equ	0fa30h		;input graphic character work
GRPFLG	equ	0fa31h		;print graphic character flag
FKEYCNT	equ	0fa32h		;function key counter
RSEED	equ	0fa51h		;random seed, fa51-fa55
PRTPOS	equ	0fa57h		;printer head position (0-origin)
DEVICE	equ	0fa58h		;output device
CMMCNST	equ	0fa59h		;constant for print-comma
STACK	equ	0fa5bh		;initial stack pointer
LINENUM	equ	0fa5dh		;current line number
STARTAD	equ	0fa5fh		;basic program start address
CMDTBL	equ	0fa61h		;command jump table
FNCTBL	equ	0fae5h		;function jump table
FKEYTBL	equ	0fb3dh		;function key data
FKEYAD	equ	0fb8dh		;function key address
KYBFIN	equ	0fb8fh		;key buffer pointer for input
KYBFOUT	equ	0fb90h		;key buffer pointer for output
KYBFSZ	equ	0fb92h		;key buffer size
KYBFAD	equ	0fb93h		;key buffer address
RSBFIN	equ	0fb95h		;RS-232C buffer in
RSBFOUT	equ	0fb96h		;RS-232C buffer out
RSBFSZ	equ	0fb98h		;RS-232C buffer size
RSBFAD	equ	0fb99h		;RS-232C buffer address

;PLAY command
BUFAIN	equ	0fba1h		;channelA buffer pointer for input
BUFAOUT	equ	0fba2h		;channelA buffer pointer for output
BUFASZ	equ	0fba4h		;channelA buffer size
BUFAAD	equ	0fba5h		;channelA buffer address
BUFBIN	equ	0fba7h		;channelB buffer in
BUFCIN	equ	0fbadh		;channelC buffer in

KEYBUF	equ	0fbb9h		;key buffer, fbb9-fbf8
RSBUF	equ	0fbf9h		;RS-232C buffer, fbf9-fc38

;PLAY command
BUFA	equ	0fc39h		;channelA buffer, fc39-fc78
BUFB	equ	0fc79h		;channelB buffer, fc79-fcb8
BUFC	equ	0fcb9h		;channelC buffer, fcb9-fcf8
CHANNEL	equ	0fd14h		;channel (0-2)
SVBCK	equ	0fd15h		;backup of S/V-value
PLAYLEN	equ	0fd17h		;length
PLAYSTR	equ	0fd18h		;string address
PLAYST	equ	0fd1bh		;PLAY status
PLWKA	equ	0fd1dh		;PLAY work for channelA
REMAIN	equ	0fd1dh-PLWKA	;remaining time
PLLEN	equ	0fd1fh-PLWKA	;string length
PLADR	equ	0fd20h-PLWKA	;string address
OCTAVE	equ	0fd2ch-PLWKA	;O-value
LENGTH	equ	0fd2dh-PLWKA	;L-value
TEMPO	equ	0fd2eh-PLWKA	;T-value
VOLUME	equ	0fd2fh-PLWKA	;V-value
PERIOD	equ	0fd30h-PLWKA	;M-value
PLWKB	equ	0fd42h		;PLAY work for channelB
PLWKC	equ	0fd67h		;PLAY work for channelC

PAGES	equ	0fd8ch		;How many pages?
USREND	equ	0fd8dh		;user area end, fd8d-fd8e
SCREEN2	equ	0fd8fh		;screen 2nd parameter-1
SCREEN3	equ	0fd90h		;screen 3rd parameter-1

VRAM	equ	0fd91h		;VRAM address (high)
SCREEN1	equ	0fd92h		;screen 1st parameter-1
COLOR1	equ	0fd93h		;color 1st parameter
COLOR2	equ	0fd94h		;color 2nd parameter
COLOR3	equ	0fd95h		;color 3rd parameter (1,2 -> 0,2)
M1COLOR	equ	0fd96h		;screen mode 1 color parameters, fd96-fd98
M2COLOR	equ	0fd99h		;screen mode 2 color parameters, fd99-fd9b
M3COLOR	equ	0fd9ch		;screen mode 3 color parameters, fd9c-fd9e
M4COLOR	equ	0fd9fh		;screen mode 4 color parameters, fd9f-fda1
CONSOL1	equ	0fda2h		;console 1st line number+1
CONSOL2	equ	0fda3h		;console last line number+1
FSTLIN	equ	0fda4h		;=(CONSOL1)
LASTLIN	equ	0fda5h		;sc12:(CONSOL2)-(CONSOL3) sc34:(CONSOLE2)
CONSOL3	equ	0fda6h		;console 3rd parameter
FKEYSFT	equ	0fda7h		;shift key status for function key display
CSRY	equ	0fda8h		;cursor Y+1
CSRX	equ	0fda9h		;cursor X+1
CSRAD	equ	0fdaah		;cursor address
WIDTH	equ	0fdach		;X max+1
GRPX1	equ	0fdaeh		;graphic X, fdae-fdaf
GRPY1	equ	0fdb0h		;graphic Y, fdb0-fdb1
VRAMBIT	equ	0fdb2h		;color bit/dot pattern
VRAMAD	equ	0fdb3h		;VRAM attribute address
LINEST	equ	0fdb7h		;line connection status, fdb7-fdc6 (0=connect to next line)
PAGE1	equ	0fdc8h		;page1 data, fdc8-fdfe
PAGE2	equ	0fdffh		;page2 data, fdff-fe35
PAGE3	equ	0fe36h		;page3 data, fe36-fe6c
PAGE4	equ	0fe6dh		;page4 data, fe6d-fea3

INPTXY	equ	0fea4h		;INPUT command initial position
INPTX	equ	0fea6h		;INPUT command end position, or length
INPTPAG	equ	0fea7h		;INPUT command page
INSMODE	equ	0fea8h		;0=not insert mode, ff=insert mode
STOPSC2	equ	0feaah		;screen 2nd parameter -1 in stop
STOPSC3	equ	0feabh		;screen 3rd parameter -1 in stop
ATTDAT	equ	0feach		;color attribute data
GRPX2	equ	0feadh		;graphic X, fead-feae
GRPY2	equ	0feafh		;graphic Y, feaf-feb0
GRPX3	equ	0feb1h		;graphic work, feb1-feb2
GRPY3	equ	0feb3h		;graphic work, feb3-feb4
PAWRK	equ	0feb5h		;paint work, feb5-feb6
PACNT	equ	0feb7h		;paint work
BORDERA	equ	0fec5h		;paint border color attribute
BORDERC	equ	0fec6h		;paint border color code
GMKYBUF	equ	0fecah		;reply to game key query
;TARGET	equ	0fecbh		;target file name, fecb-fed0
FNAME	equ	0fed1h		;loading file name, fed1-fed6
VERIFY	equ	0fed8h		;load/verify flag 00h=load ffh=verify
INPBUF	equ	0fedah		;input buffer, feda-ff21, (ff22)-(ff23)=00h
ARGTYP	equ	0ff25h		;0=value 1=string
STREND	equ	0ff27h		;strings area end
BASICAD	equ	0ff29h		;basic area start
STRDSC1	equ	0ff2dh		;temporary string descriptor, ff2d-ff30
STRDSC2	equ	0ff31h		;ff31-ff34
STRDSC3	equ	0ff35h		;ff35-ff38
STRDSC4	equ	0ff39h		;ff39-ff3c
STRAD	equ	0ff3dh		;string area start address - 1
GCWRK	equ	0ff41h		;garbage collection work
DATALN	equ	0ff45h		;data line number
INPRD	equ	0ff49h		;INPUT(=0) or READ(>0)
PROGAD	equ	0ff4eh		;program address in interpreting
TMP	equ	0ff50h		;temporary
STOPLN	equ	0ff52h		;line number at stop
STOPAD	equ	0ff54h		;stop address
VARAD	equ	0ff56h		;variable area start address
ARRAD	equ	0ff58h		;array area start address
FREEAD	equ	0ff5ah		;free area start address
DATAAD	equ	0ff5ch		;data command address
FNPAR	equ	0ff5eh		;FN() parameter name
FNARG	equ	0ff60h		;FN() argument value, ff60-ff64
OPRTR	equ	0ff65h		;operator, FAC1-1
FAC1	equ	0ff66h		;floating accumlator 1, ff66-ff6a
FAC2	equ	0ff6dh		;floating accumlator 2, ff6d-ff71
FAC3	equ	0ff72h		;floating accumlator 3 (string conversion)
HOOK	equ	0ff8ah		;hook area, ff8a-ffe3
HOOKPED	equ	0ff8ah		;hook for print end
HOOKERR	equ	0ff8dh		;hook for error
HOOKEDT	equ	0ff93h		;hook for screen editor
HOOKCLP	equ	0ff99h		;hook for changing link pointer
HOOKPRT	equ	0ffaeh		;hook for PRINT# command
HOOKINP	equ	0ffbah		;hook for INPUT# command
HOOKRES	equ	0ffc9h		;hook for RESSTK
HOOKPUT	equ	0ffcfh		;hook for PUTDEV
HOOKLCP	equ	0ffd2h		;hook for LCOPY
HOOKSTP	equ	0ffd8h		;hook for stop key
HOOK18H	equ	0ffdbh		;hook for 0018h
HOOK38H	equ	0ffe1h		;hook for 0038h

COLUMNS	equ	32
ROWS	equ	16

FOR_	equ	081h
DATA_	equ	083h
GOTO_	equ	088h
RUN_	equ	089h
GOSUB_	equ	08ch
REM_	equ	08eh
DEF_	equ	093h
PRINT_	equ	095h
SCREEN_	equ	09fh
CMDLAST	equ	0aah		;last command
TAB_	equ	0c2h
TO_	equ	0c3h
FN_	equ	0c4h
SPC_	equ	0c5h
INKEY_	equ	0c6h
THEN_	equ	0c7h
NOT_	equ	0c8h
STEP_	equ	0c9h
PLUS_	equ	0cah
MINUS_	equ	0cbh
ASTRSK_	equ	0cch
GT_	equ	0d1h
EQ_	equ	0d2h
LT_	equ	0d3h
FNC1ST	equ	0d4h		;1st function
USR_	equ	0d7h
LEFT_	equ	0eah
MID_	equ	0ech
STICK_	equ	0efh
STRIG_	equ	0f0h
FNCLAST	equ	0f1h		;last function


;;;;;;;;;;;;;;;;;;;;;;;;;

;boot
	org	BOOT
	di
	jp	COLD


;check parameter
; compare (hl) and data at (sp)
;input: sp,hl
;output: a=data, hl=data address, z-flag(a=00h or 3ah), c-flag(a=30h-39h)
_CHKPAR:ds	CHKPAR-_CHKPAR
	org	CHKPAR

	ex	(sp),hl
	ld	a,(hl)
	inc	hl
	ex	(sp),hl
	cp	(hl)
	jp	nz,SNERR


;skip space and analyze a byte
;input: hl=address-1
;output: a=data, hl=data address, z-flag(a=00h or 3ah), c-flag(a=30h-39h)
;destroy: f
_ANADAT:ds	ANADAT-_ANADAT
	org	ANADAT

	call	SKIPSPINC
	jp	CHKFIG


;jump hook
_JPHK1:	ds	JPHK1-_JPHK1
	org	JPHK1

	jp	HOOK18H


;compare hl and de
;input: hl,de
;output: f
;destroy: af
_CPHLDE:ds	CPHLDE-_CPHLDE
	org	CPHLDE

	ld	a,h
	sub	d
	ret	nz
	ld	a,l
	sub	e
	ret


;check sign of FAC1
;input: FAC1
;output: a(0:FAC1=0 1:FAC1>0 ff:FAC1<0), z(1:FAC1=0)
;destroy: af
_CHKSGN:ds	CHKSGN-_CHKSGN
	org	CHKSGN
	ld	a,(FAC1+4)
	or	a
	ret	z
	jp	CHKSGN2


;jump hook
_JPHK2:	ds	JPHK2-_JPHK2
	org	JPHK2

	jp	HOOK38H


;hot start
HOT:
;initialize function key
	ld	hl,FKEYLST
	ld	de,FKEYTBL
	ld	c,0ah
SETFKLP1:
	ld	a,(hl)
	inc	hl
	push	hl		;
	call	CNVASC

	ld	b,08h
SETFKLP2:
	dec	b
	ld	(de),a
	inc	de
	inc	hl
	ld	a,(hl)
	or	a
	jp	p,SETFKLP2

	pop	hl		;
	ld	a,(hl)
	inc	hl

SETFKLP3:
	ld	(de),a
	inc	de
	xor	a
	djnz	SETFKLP3

	dec	c
	jr	nz,SETFKLP1
	call	PRTFKEY

;check external ROM
	ld	hl,(4000h)
	ld	de,'A'+'B'*100h
	rst	CPHLDE
	ld	hl,(4002h)
	call	z,JPHL

;How Many Pages?
PAGELP:
	ld	hl,HOWMANY
	call	PUTS
	call	INPT1
	rst	ANADAT
	ld	d,00h
	jr	nc,SETPAGEMAX
	call	ATOI2
	ld	a,d
	or	a
	jr	nz,PAGEFC
	ld	a,e
	dec	a
	ld	hl,PAGES
	cp	(hl)
	jr	c,SETPAGE
PAGEFC:
	ld	e,08h		;?FC Error
PAGEERR:
	call	PRTERR
	jr	PAGELP

SETPAGEMAX:
	ld	e,02h		;?SN Error
	jr	nz,PAGEERR
	or	a
	ld	e,26h		;?MO Error
	jr	nz,PAGEERR	;colon

	ld	hl,PAGES
	ld	e,(hl)
SETPAGE:
	ld	(hl),e

	call	NEW
	ld	hl,ENDTBL-1
	add	hl,de		;d=0
	ld	h,(hl)
	ld	l,0ffh
	ld	(USREND),hl
	ld	bc,50
	call	CLRMAIN

;print start message
	call	CHK6001A
	ld	hl,SYSNAME
	jr	nz,PC6001
	ld	hl,SYSNAME2
PC6001:
	call	PUTS
	ld	hl,BASICVER
	call	PUTS
	call	CHKFRE
;	ld	hl,002fh	;for compatibility with N60-BASIC
	ld	hl,0028h	;for compatibility with N60-BASIC
	add	hl,de
	call	PUTI2
	ld	hl,BFREE
PUTSEDIT:
	call	PUTS
	jp	EDIT


;initialize table
IOTBL93:
	db	0c0h		;bit7: mode definition
				;bit65:portA,portC-upper=mode 2
				;bit4: portA=output
				;bit3: portC-upper=output
				;bit2: portB,portC-lower=mode 0
				;bit1: portB=output
				;bit0: portC-lower=output
	db	0dh		;enable 8255 INT for writing
	db	09h		;enable 8255 INT for reading
	db	05h		;CGROM off
	db	03h		;CRTC
	db	0fh		;nobf=1 (for emulator?)

IOTBL81:
	db	12h		;reset error, DTR=1, RxE/TxEN=0
	db	12h		; set twice for synchronous mode
	db	52h		;reset, reset error, DTR=1, RxE/TxEN=0
	db	4fh		;stop bits=1, parity disable,
				; character length=8bits, baud rate factor=64
	db	37h		;reset error, RTS/DTR/RxE/TxEN=1

;for mkII
IOTBLF0:
;F0:	db	0ddh		;0000-7fff:internal RAM for reading (test)
F0:	db	71h		;0000-3fff:internal ROM for reading
				;4000-7fff:external ROM for reading
F1:	db	0ddh		;8000-ffff:internal RAM for reading
F2:	db	55h		;0000-ffff:internal RAM for writing
F3:	db	0c2h		;wait and interrupt control
				; bit7: M1 wait (1=on)
				; bit6: ROM wait (1=on)
				; bit5: RAM wait (1=on)
				; bit4: INT2 interrupt address (1=output)
				; bit3: INT1 interrupt address (1=output)
				; bit2: timer interrupt (1=disable)
				; bit1: INT2 interrupt (1=disable)
				; bit0: INT1 interrupt (1=disable)
F4:	db	00h		;INT1 address
F5:	db	00h		;INT2 address
F6:	db	03h		;timer count up
F7:	db	06h		;timer interrupt address
F8:	db	0c3h		;CG rom access control
				; bit7: wait (0=off, 1=on)
				; bit6: read enable (0=disable, 1=enable)
				; bit5: bit2 mask (1=mask)
				; bit4: bit1 mask (1=mask)
				; bit3: bit0 mask (1=mask)
				; bit2-0: CGROM address (=A15-A13)

;interrupt
INTTBL:
	dw	INTKEY		;fa02 normal key
	dw	INT232		;fa04 RS-232C
	dw	INTTIM		;fa06 2ms timer
	dw	INTCMT		;fa08 CMT read
	dw	EIRET		;fa0a ?
	dw	EIRET		;fa0c ?
	dw	INTWSTP		;fa0e CMT write stop
	dw	INTRSTP		;fa10 CMT read stop
	dw	INTERR		;fa12 CMT error
	dw	INTGRP		;fa14 GRAPH key etc.
	dw	INTGAM		;fa16 reply to game key query


BFREE:
	db	" Bytes free", 00h
ENDTBL:
	db	0f9h, 0dfh, 0bfh, 9fh
HOWMANY:
	db	"How Many Pages", 00h
QMARK:
	db	"? ", 00h
SYSNAME:
	db	9ah, 0deh, 96h, 0fdh, 00h
SYSNAME2:
	db	"Compatible ", 00h
BASICVER:
	db	"BASIC Ver.0.8", 0dh, 0ah, 00h

PAGEDATA:
	db	0c0h		;fd91	VRAM address
	db	00h		;fd92	screen 1st parameter-1
	db	01h		;fd93	color 1st parameter
	db	00h		;fd94	color 2nd parameter
	db	00h		;fd95	color 3rd parameter (1,2 -> 0,2)
	db	01h, 00h, 00h	;fd96	screen mode 1 color parameters
	db	01h, 00h, 00h	;fd99	screen mode 2 color parameters
	db	02h, 00h, 00h	;fd9c	screen mode 3 color parameters
	db	03h, 00h, 00h	;fd9f	screen mode 4 color parameters
	db	01h		;fda2	console 1st line number+1
	db	10h		;fda3	console last line number+1
	db	01h		;fda4	=(fda2)
	db	0fh		;fda5	sc12:(fda3)-(fda6) sc34:(fda3)
	db	01h		;fda6	console 3rd parameter
	db	00h		;fda7	shift key status for function key disp
	db	01h		;fda8	cursor Y+1
	db	01h		;fda9	cursor X+1
	dw	0c200h		;fdaa	cursor address
	db	COLUMNS		;fdac	X max+1
	db	00h		;fdad
	dw	0000h		;fdae	graphic X
	dw	0000h		;fdb0	graphic Y
	db	00h		;fdb2
	db	00h		;fdb3
	db	00h		;fdb4
	db	00h		;fdb5
	db	10h		;fdb6	LINEST-1


FKEYLST:
	db	9ah, ' '	;COLOR
	db	0a3h, 22h	;CLOAD"
	db	GOTO_, ' '	;GOTO
	db	97h, ' '	;LIST
	db	RUN_, 0dh	;RUN
	db	9fh, ' '	;SCREEN
	db	0a4h, 22h	;CSAVE"
	db	PRINT_, ' '	;PRINT
	db	0a7h, ' '	;PLAY
	db	96h, 0dh	;CONT


OPRTBL:
;c4-c7
;	dw					F_FN,	F_SPC,	F_INKY,	C_THEN
;c8-cf
	dw	O_NOT,	C_STEP,	O_PLS,	O_MNS,	O_MUL,	O_DIV,	O_POW,	O_AND
;d0-d3
	dw	O_OR,	O_GTEQLT

CMDLST:
;80-8f
	dw	C_END,	C_FOR,	C_NEXT,	C_DATA,	C_INPT,	C_DIM,	C_READ,	C_LET
	dw	C_GOTO,	C_RUN,	C_IF,	C_RSTR,	C_GSUB,	C_RET,	C_REM,	C_STOP
;90-9f
	dw	C_OUT,	C_ON,	C_LPRT,	C_DEF,	C_POKE,	C_PRT,	C_CONT,	C_LIST
	dw	C_LLST,	C_CLR,	C_COLR,	C_PSET,	C_PRST,	C_LINE,	C_PAIN,	C_SCRN
;a0-aa
	dw	C_CLS,	C_LOCA,	C_CNSL,	C_CLD,	C_CSV,	C_EXEC,	C_SOUN,	C_PLAY
	dw	C_KEY,	C_LCPY,	C_NEW

FNCLST:
;d4-df
	dw					F_SGN,	F_INT,	F_ABS,	F_USR
	dw	F_FRE,	F_INP,	F_LPOS,	F_POS,	F_SQR,	F_RND,	F_LOG,	F_EXP
;e0-ef
	dw	F_COS,	F_SIN,	F_TAN,	F_PEEK,	F_LEN,	F_HEX,	F_STR,	F_VAL
	dw	F_ASC,	F_CHR,	F_LEFT,	F_RGT,	F_MID,	F_POIN,	F_CSRL,	F_STCK
;f0-f1
	dw	F_STRG,	F_TIME


CMDNAME:
	db	'E'+80h,"ND",	'F'+80h,"OR",	'N'+80h,"EXT",	'D'+80h,"ATA"
	db	'I'+80h,"NPUT",	'D'+80h,"IM",	'R'+80h,"EAD",	'L'+80h,"ET"
	db	'G'+80h,"OTO",	'R'+80h,"UN",	'I'+80h,"F",	'R'+80h,"ESTORE"
	db	'G'+80h,"OSUB",	'R'+80h,"ETURN",'R'+80h,"EM",	'S'+80h,"TOP"

	db	'O'+80h,"UT",	'O'+80h,"N",	'L'+80h,"PRINT",'D'+80h,"EF"
	db	'P'+80h,"OKE",	'P'+80h,"RINT",	'C'+80h,"ONT",	'L'+80h,"IST"
	db	'L'+80h,"LIST",	'C'+80h,"LEAR",	'C'+80h,"OLOR",	'P'+80h,"SET"
	db	'P'+80h,"RESET",'L'+80h,"INE",	'P'+80h,"AINT",	'S'+80h,"CREEN"

	db	'C'+80h,"LS",	'L'+80h,"OCATE",'C'+80h,"ONSOLE",'C'+80h,"LOAD"
	db	'C'+80h,"SAVE",	'E'+80h,"XEC",	'S'+80h,"OUND",	'P'+80h,"LAY"
	db	'K'+80h,"EY",	'L'+80h,"COPY",	'N'+80h,"EW"

FNCNAME:
	db					'T'+80h,"AB(",	'T'+80h,"O"
	db	'F'+80h,"N",	'S'+80h,"PC(",	'I'+80h,"NKEY$",'T'+80h,"HEN"
	db	'N'+80h,"OT",	'S'+80h,"TEP",	'+'+80h,	'-'+80h
	db	'*'+80h,	'/'+80h,	'^'+80h,	'A'+80h,"ND"

	db	'O'+80h,"R",	'>'+80h,	'='+80h,	'<'+80h
	db	'S'+80h,"GN",	'I'+80h,"NT",	'A'+80h,"BS",	'U'+80h,"SR"
	db	'F'+80h,"RE",	'I'+80h,"NP",	'L'+80h,"POS",	'P'+80h,"OS"
	db	'S'+80h,"QR",	'R'+80h,"ND",	'L'+80h,"OG",	'E'+80h,"XP"

	db	'C'+80h,"OS",	'S'+80h,"IN",	'T'+80h,"AN",	'P'+80h,"EEK"
	db	'L'+80h,"EN",	'H'+80h,"EX$",	'S'+80h,"TR$",	'V'+80h,"AL"
	db	'A'+80h,"SC",	'C'+80h,"HR$",	'L'+80h,"EFT$",	'R'+80h,"IGHT$"
	db	'M'+80h,"ID$",	'P'+80h,"OINT",	'C'+80h,"SRLIN",'S'+80h,"TICK"

	db	'S'+80h,"TRIG",	'T'+80h,"IME",	80h


_ERRBELL:ds	ERRSTR-1-_ERRBELL
	org	ERRSTR-1
ERRBELL:
	db	07h

_ERRSTR:ds	ERRSTR-_ERRSTR
	org	ERRSTR
	db	" Error", 00h

_ERRIN:	ds	ERRIN-_ERRIN
	org	ERRIN
	db	" in ", 00h

_OK:
	ds	OK-_OK
	org	OK
	db	"Ok", 0dh, 0ah, 00h

ERRID:
	db	"NF", "SN", "RG", "OD", "FC", "OV", "OM", "UL"
	db	"BS", "DD", "/0", "ID", "TM", "OS", "LS", "ST"
	db	"CN", "UF", "TR", "MO", "FD"


;print error message
;input: e=error number
;destroy: af,bc,de,hl
PRTERR:
	ld	a,'?'
	call	PRTC
	ld	hl,ERRID
	ld	d,00h
	add	hl,de
	ld	a,(hl)
	call	PRTC
	inc	hl
	ld	a,(hl)
	call	PRTC
	ld	hl,ERRBELL
	jp	PUTS


NFERR:
	ld	e,00h
	db	01h		;ld bc,****
F_HEX:
C_TO:
C_TAB:
F_SPC:
C_THEN:
C_STEP:
SNERR:
	ld	e,02h
	db	01h		;ld bc,****
RGERR:
	ld	e,04h
	db	01h		;ld bc,****
ODERR:
	ld	e,06h
	db	01h		;ld bc,****
FCERR:
F_USR:
	ld	e,08h
	db	01h		;ld bc,****
OVERR:
	ld	e,0ah
	db	01h		;ld bc,****
OMERR:
	ld	e,0ch
	db	01h		;ld bc,****
ULERR:
	ld	e,0eh
	db	01h		;ld bc,****
BSERR:
	ld	e,10h
	db	01h		;ld bc,****
DDERR:
	ld	e,12h
	db	01h		;ld bc,****
DIV0ERR:
	ld	e,14h
	db	01h		;ld bc,****
IDERR:
	ld	e,16h
	db	01h		;ld bc,****
TMERR:
	ld	e,18h
	db	01h		;ld bc,****
OSERR:
	ld	e,1ah
	db	01h		;ld bc,****
LSERR:
	ld	e,1ch
	db	01h		;ld bc,****
STERR:
	ld	e,1eh
	db	01h		;ld bc,****
CNERR:
	ld	e,20h
	db	01h		;ld bc,****
UFERR:
	ld	e,22h
	db	01h		;ld bc,****
TRERR:
	ld	e,24h
	db	01h		;ld bc,****
MOERR:
	ld	e,26h
	db	01h		;ld bc,****
FDERR:
	ld	e,28h

ERROR:
	call	HOOKERR
	ld	sp,(STACK)
	xor	a
	ld	(STRDSC2),a
	ld	(STRDSC3),a
	ld	(STRDSC4),a
	dec	a
	ld	(STOPAD+1),a	;(STOPAD)>=fa00h
	call	PRTNLXTXT
	call	PRTERR

PRTLNUM:
	call	PLSTOP

	ld	hl,(LINENUM)
	ld	a,h
	and	l
	inc	a
	call	nz,INLNUM	;if (LINENUM)<>0ffffh
	jp	EDIT


;program edit and direct command mode
_EDIT:	ds	EDIT-_EDIT
	org	EDIT

	call	HOOKEDT
	call	CLRKBF
	call	PRTNLX		;for graphic screen
	call	PRTNLXTXT
	ld	hl,OK
	call	PUTS
EDIT2:
	call	PUTPRNL
	ld	hl,0ffffh
	ld	(LINENUM),hl

EDITLP:
	call	INPTSCR		;hl=INPBUF-1
	call	c,PLSTOP
	call	CNVIL
	ld	hl,INPBUF-1
	rst	ANADAT		;
	inc	a
	dec	a
	jr	z,EDITLP
	jp	nc,INTPRT	;


;change program
CHGPRG:
	call	GETLN
	push	hl		;buffer address after line number
	push	de		;line number
	push	hl		;buffer address after line number
	call	SRCHLN
	call	c,DELPRG	;c-flag=1
	pop	hl		;buffer address after line number
	inc	(hl)
	dec	(hl)
	jr	nz,INSPRG
	jr	c,CHGLKP	;
	jp	ULERR

;insert program
INSPRG:
	xor	a
	ld	de,0005h
PRGLP:
	inc	e
	inc	a
	inc	hl
	inc	(hl)
	dec	(hl)
	jr	nz,PRGLP

	ld	hl,(VARAD)
	push	hl
;	or	a
	sbc	hl,bc
	push	hl		;slide size
	add	hl,bc		;old (VARAD)
	add	hl,de		;old (VARAD)+size
	call	CHKSTK
	ld	(VARAD),hl
	ex	de,hl
	pop	bc		;slide size
	pop	hl		;old (VARAD)
	dec	hl
	dec	de
	lddr

COPYPRG:
	inc	hl		;address to be inserted in
	inc	hl
	inc	de
	inc	de
	ld	(hl),e
	inc	hl
	ld	(hl),d
	inc	hl
	pop	de		;line number
	ld	(hl),e
	inc	hl
	ld	(hl),d
	inc	hl
	ex	de,hl
	pop	hl		;buffer address after line number

	ld	c,a		;b=0
	ldir
;	jr	CHGLKP


;change link pointer
CHGLKP:
	call	HOOKCLP
	call	RESSTK
	call	CHGLKPMAIN
	jr	EDIT2


;change link pointer (main)
;output: hl=last address
;destroy: af,bc,de
CHGLKPMAIN:
	ld	de,(STARTAD)
;skip address and line number
CHGLKPLP:
	ld	h,d
	ld	l,e
	ld	a,(hl)
	inc	hl
	or	(hl)
	ret	z

	ld	bc,0003h
	add	hl,bc

;search for next line
	xor	a
	ld	c,a		;b=0
	cpir

;next line address
	ex	de,hl
	ld	(hl),e
	inc	hl
	ld	(hl),d
	jr	CHGLKPLP


;input: bc=address to be deleted, hl=next line address
;output: c-flag=1
;destroy: af,de,hl
DELPRG:
	push	bc		;address to be deleted
	ex	de,hl		;de <- next line address
	ld	hl,(VARAD)
	or	a
	sbc	hl,de
	ld	b,h
	ld	c,l
	ex	de,hl		;hl <- next line address
	pop	de		;address to be deleted
	push	de
	ldir
	ld	(VARAD),de
	pop	bc
	scf
	ret


;search for line number
;input: de=line number
;output: z=1,c=1) bc=address, hl=next line address
;	 z=0,c=0) bc=next line address, hl=next next line address
;	 z=1,c=0) bc=hl=end address
;destroy: af
SRCHLN:
	ld	hl,(LINENUM)
	rst	CPHLDE
	jr	nc,SRCHSTART	;less than or equal to current line number
	ld	hl,(PROGAD)
SRCHLP1:
	ld	a,(hl)
	inc	hl
	or	a
	jr	nz,SRCHLP1

;for PORTOPIA (mk2)
;	jr	SRCHLP2
	call	SRCHLP2
	ret	c

SRCHSTART:
	ld	hl,(STARTAD)	;search from program start address

SRCHLP2:
	ld	b,h
	ld	c,l
	ld	a,(hl)
	inc	hl
	or	(hl)
	jr	z,SRCHEND	;program end
	inc	hl
	inc	hl
	ld	a,(hl)
	dec	hl
	cp	d
	jr	nz,SRCHNZ
	ld	a,(hl)
	cp	e
SRCHNZ:
	ld	a,(bc)
	dec	hl
	ld	h,(hl)
	ld	l,a
	jr	c,SRCHLP2	;(hl,hl-1)-de
	inc	hl
	jr	nz,SRCHEND	;(hl,hl-1)-de
	scf
SRCHEND:
	dec	hl
	dec	bc
	dec	hl
	ret


;convert to intermediate language
;input: hl=INPBUF-1
CNVIL:
	ld	d,h
	ld	e,l

ILLP1:
	inc	de
	ld	a,(de)

	or	a
	jp	m,ILLP1		;skip kana

	inc	hl
	jr	z,ILEND1	;00=line end

	call	CHKFIG
	jr	c,ILPUT

	cp	22h		;double quotation mark
	jr	z,ILDQ
	cp	14h
	jr	z,ILGRP
	cp	'*'
	jr	c,ILPUT		;00h-29h
	cp	'?'
	jr	nz,ILCMP

;question mark
	ld	(hl),PRINT_	;95h
	jr	ILLP1

;graphic character
ILGRP:
	ld	(hl),a
	inc	hl
	inc	de
	ld	a,(de)
ILPUT:
	ld	(hl),a
	jr	ILLP1


;double quotation mark
ILDQ:
	ld	(hl),a

;REM
ILREM:
	ld	c,a		;double quotation mark or 0(REM)
	ld	b,01h		;double quotation counter

;REM,DATA,""
;loop until (de)=0 or (de)=c
ILLP2:
	inc	de
	inc	hl
	ld	a,(de)
ILEND1:
	ld	(hl),a
	or	a
	jr	z,ILEND2
	cp	22h
	jr	nz,ILNZ
	inc	b
ILNZ:
	bit	0,b
	jr	nz,ILLP2
	cp	c
	jr	nz,ILLP2
	jr	ILLP1

ILEND2:
	inc	hl
	ld	(hl),a		;a=0
	inc	hl
	ld	(hl),a		;a=0
	ret


;found
ILFOUND:
	pop	af		;cancel
	pop	hl
	ld	(hl),b
	ld	a,b
	sub	REM_
	jr	z,ILREM
	cp	DATA_-REM_
	jr	nz,ILLP1

;DATA
ILDAT:
	ld	bc,003ah	;b=0, c=':'
	jr	ILLP2


;not found
ILNC:
	pop	hl
	ld	a,(de)
	ld	(hl),a
	jr	ILLP1

;compare with command string
ILCMP:
	call	TOUPPER
	ld	(de),a

	push	hl
	ld	hl,CMDNAME
	ld	b,80h

ILLP3:
	ld	a,(de)
	or	80h
	cp	(hl)
	jr	nz,ILLP6

;1st character matched
	push	de
ILLP4:
	inc	hl
	ld	a,(hl)
	rlca
	jr	c,ILFOUND

	ld	a,b
	cp	GOTO_
ILLP5:
	inc	de
	ld	a,(de)
	jr	nz,NOGOTO
	cp	' '
	jr	z,ILLP5
NOGOTO:
	call	TOUPPER
	cp	(hl)
	jr	z,ILLP4
	pop	de

ILLP6:
	inc	hl
	ld	a,(hl)
	rlca
	jr	nc,ILLP6
	inc	b

	ld	a,b
	cp	FNCLAST+1
	jr	nc,ILNC
	cp	CMDLAST+1
	jr	nz,ILLP3
	ld	b,TAB_
	jr	ILLP3


;LLIST command
C_LLST:
	ld	a,01h
	ld	(DEVICE),a
;	jp	C_LIST


;LIST command
C_LIST:
	pop	af		;cancel return address
	ld	de,0000h
	call	CHKCLN
	jr	z,LISTTOEND

	call	GETLN
	push	de		;start line number
	call	CHKCLN
	jr	z,LISTSTRT

	cp	MINUS_
	jp	nz,SNERR
	call	CHKCLNINC
	jr	nz,LISTNZ
	pop	de
LISTTOEND:
	ld	bc,65530
	push	bc
	jr	LISTSTRT

LISTNZ:
	call	GETLN
	call	CHKCLN
	jp	nz,SNERR

	pop	hl
	push	de		;push end line number
	ex	de,hl		;de <- start line number


;de=start line number, (sp)=end line number
LISTSTRT:
	call	SRCHLN
	ld	h,b
	ld	l,c

LISTLP1:
	call	STOPESC

	inc	hl
	ld	a,(hl)
	inc	hl
	or	(hl)
	jr	z,LISTEND2

	call	PUTNL

	pop	de		;end line number
	push	de

	inc	hl
	ld	a,(hl)
	inc	hl
	push	hl
	ld	h,(hl)
	ld	l,a

	inc	de
	rst	CPHLDE
	jr	nc,LISTEND

	call	PUTI2
	ld	a,' '
	call	PUTDEV

	pop	hl

	ld	bc,0000h	;double quotation mark and DATA counter
	ld	d,b		;REM counter
LISTLP3:
	inc	hl
	ld	a,(hl)
	or	a
	jr	z,LISTLP1

	cp	22h
	jr	z,LISTDQ

	bit	0,b
	jr	nz,LISTPUT

	inc	c
	dec	c
	jr	nz,LISTCHKCLN

	inc	d
	dec	d
	jr	nz,LISTPUT

	cp	REM_
	jr	z,LISTREM
	cp	DATA_
	jr	z,LISTDATA
	or	a
	jp	m,LISTCMD
LISTPUT:
	call	PUTDEV
	jr	LISTLP3

LISTEND:
	pop	af		;cancel stack
LISTEND2:
	pop	af		;cancel stack
	jp	EDIT

LISTDQ:
	inc	b
	jr	LISTPUT

LISTCHKCLN:
	cp	':'
	jr	nz,LISTPUT
	dec	c
	jr	LISTPUT

LISTREM:
	inc	d
LISTDATA:
	inc	c
LISTCMD:
	push	hl
	call	CNVASC

LISTLP4:
	call	PUTDEV
	inc	hl
	ld	a,(hl)
	or	a
	jp	p,LISTLP4
	pop	hl
	jr	LISTLP3


;FOR command
C_FOR:
	pop	af		;cancel return address
	call	CHKVAR
	ld	a,(hl)
	cp	'('
	jp	z,SNERR

	call	GETVAR
	ld	hl,(PROGAD)

	push	de		;variable address

	call	VARIN
	call	CHKNUM
	ld	a,(hl)
	cp	TO_
	jp	nz,SNERR
	call	NARGMOINC
	ex	de,hl
	call	PUSHF1		;TO value
	ex	de,hl
	call	CHKCLN		;a=(hl)
	ld	(PROGAD),hl
	call	z,SETPLS1	;no change in z-flag
	jr	z,STEP1
	cp	STEP_
	jp	nz,SNERR
	call	NARGMOINC
STEP1:
	call	POPF2		;TO value
	pop	bc		;variable address

	ld	hl,(PROGAD)
	push	hl		;program address
	ld	hl,0002h
	add	hl,sp
FORLP:
	call	SRCHSTK2
	jr	z,FOROK		;not found or gosub identifier found

;check program address
	ld	de,0011h
	add	hl,de
	ld	e,(hl)
	inc	hl
	ld	d,(hl)
	inc	hl
	ex	(sp),hl
	rst	CPHLDE
	ex	(sp),hl
	jr	nz,FORLP

	ld	sp,hl
	push	de		;program address

FOROK:
	ld	hl,(LINENUM)
	push	hl		;line number
	call	PUSHF2		;TO value
	call	PUSHF1		;STEP value
	push	bc		;variable address
	ld	a,FOR_		;identifier
	push	af
	inc	sp
	jr	INTPEND


;
;
;
;
_COMMAND:ds	CMDEND-14-_COMMAND
	org	CMDEND-14

;execute command
COMMAND:
	cp	CMDLAST+1	;abh
	jp	nc,SNERR
	ld	de,CMDTBL
	call	JPTBL

;intepretation end
INTPEND:
	ld	hl,(PROGAD)

;command end
_CMDEND:ds	CMDEND-_CMDEND
	org	CMDEND

	dec	hl
	rst	ANADAT

;	call	CHKCLN

	jp	z,INTPRT
	jp	SNERR


RUNLP:
	ld	a,(hl)
	inc	hl
	or	(hl)
	jp	z,EDIT
	inc	hl
	ld	e,(hl)
	inc	hl
	ld	d,(hl)
	ld	(LINENUM),de
	jp	INTPRT2


;0709h: used by PORTOPIA
_INTPRT2:ds	INTPRT2-_INTPRT2
	org	INTPRT2

	inc	hl

INTPRT:
	call	CHKSTOP
	jp	z,STOP1
	ld	a,(hl)
	inc	hl
	ld	(PROGAD),hl
	or	a
	jp	m,COMMAND
	jr	z,RUNLP
	cp	':'
	jr	z,INTPRT
	cp	' '
	jr	z,INTPRT
	dec	hl
	call	C_LET
	jr	INTPEND


;continued from C_RUN
;RUN with parameter
RUNPAR:
	call	RESSTK
	ld	hl,(PROGAD)
	call	C_GOTO
	ld	h,b
	ld	l,c
	inc	hl
	jr	RUNLP


;convert float to 2-byte integer for USR() [-32768,32767]
;input: FAC1
;output: de
;destroy: af,bc,hl
_FTOI2:	ds	FTOI2-_FTOI2
	org	FTOI2

	call	FTOI
	ld	a,(FAC1+3)
	xor	d
	ret	p
	jp	FCERR


;convert float to 1-byte integer
;input: FAC1
;output: e, a(0=ok)
;destroy: f,bc,de,hl
FTOI1:
	call	FTOI2
	ld	a,d
	or	a
	ret	z
	jp	FCERR


;convert FAC1 and FAC2 to 2-byte integer
;input: FAC1, FAC2
;output: hl, de
;destroy: af,bc,FAC1,FAC2
F12TOI2:
	ld	a,(ARGTYP)
	or	a
	jp	nz,TMERR
	call	FTOI2
	push	de		;
	call	EXFAC
	call	FTOI2
	pop	hl		;
	ret


;get line number from string (0-65529)
;input: hl=program address
;output: de=line number, hl=next address
;destroy: af,bc
GETLN:
	call	ATOI2
	push	hl
	ld	hl,65529
	rst	CPHLDE
	pop	hl
	ret	nc
	ld	de,6553
GETLNLP:
	dec	hl
	ld	a,(hl)
	cp	' '
	ret	nz
	jr	GETLNLP


;RUN command
_C_RUN:	ds	C_RUN-_C_RUN
	org	C_RUN

	call	CHKCLN
	jr	nz,RUNPAR	;with parameter
	call	RESSTK
	ld	hl,(STARTAD)
	jp	RUNLP


;GOSUB command
C_GSUB:
	call	C_GOTO
	dec	hl
GSUBLP:
	call	CHKCLNINC
	jr	nz,GSUBLP

	ex	(sp),hl
	push	hl
	call	CHKFRE
	ld	hl,(LINENUM)
	ex	(sp),hl
	ld	a,GOSUB_
	push	af		;gosub identifier
	inc	sp
JPHL:
	jp	(hl)


;GOTO command
;output: bc=new address, hl=old address (after line number)
C_GOTO:
	call	GETLN
	push	hl
	call	SRCHLN
	pop	hl
	jp	nc,ULERR
	ld	(PROGAD),bc
	ret


;RETURN command
C_RET:
	pop	bc		;return address
RETLP:
	call	SRCHSTK
	jp	nc,RGERR	;not found
	jr	z,RETOK		;gosub identifier found
	ld	hl,0013h
	add	hl,sp
	ld	sp,hl
	jr	RETLP

RETOK:
	inc	sp
	pop	hl
	ld	(LINENUM),hl
	pop	hl
	ld	(PROGAD),hl
	push	bc		;return address
	ret


;search stack for for/gosub identifier
;output: c-flag(1=found), z-flag(0=for/1=gosub if found)
;destroy: af,de,hl
SRCHSTK:
	ld	hl,0002h
	add	hl,sp

;search (hl)
;destroy: af,de
SRCHSTK2:
	ld	de,(STACK)
	rst	CPHLDE
	ret	nc		;(z-flag=1)
	ld	a,(hl)
	cp	GOSUB_
	scf
	ret


;DATA command
_C_DATA:ds	C_DATA-_C_DATA
	org	C_DATA

	db	01h, ':'	;ld bc,0e3ah/nop


;REM command
_C_REM:	ds	C_REM-_C_REM
	org	C_REM

	ld	c,00h

;input: c
DATREM:
	ld	b,00h
DATALP:
	ld	a,(hl)
	or	a
	jp	z,SETPRGAD

	bit	0,b
	jr	nz,DATANZ
	cp	c
	jp	z,SETPRGAD
DATANZ:
	inc	hl
	cp	22h		;double quotation mark
	jr	nz,DATALP
	inc	b
	jr	DATALP


;LET command
C_LET:
	call	CHKVAR
	call	GETVAR
	ld	hl,(PROGAD)
;	jp	VARIN


;put into variable
;input: bc=variable name, hl=program address, de=variable address
;output: hl=next program address
;destroy: FAC1,FAC2,af,bc,de
VARIN:
	call	SKIPSP
	cp	EQ_		;0d2h
	jp	nz,SNERR

	push	de		;variable address
	inc	hl
	bit	7,c
	jr	z,VARNUM

;string
	call	STRARG
	ld	b,a
	ld	a,d
	cp	0fah		;fa00h=work area start address
	ld	a,b
	ex	de,hl
	call	nc,MAKESTR
	ld	hl,STRDSC1
	ld	bc,0004h
	jr	VARINOK

VARNUM:
	call	NARGMO
	ld	hl,FAC1
	ld	bc,0005h
VARINOK:
	pop	de		;variable address
	ldir
	ld	hl,(PROGAD)
	ret


;ON command
C_ON:
	call	INT1ARG
	ld	a,(hl)
	ld	d,a		;
	or	04h
	cp	GOSUB_		;88h(GOTO) or 8ch(GOSUB)
	jp	nz,SNERR

ONLP:
	inc	hl
	dec	e
	jr	z,ONZ
	push	de
	call	GETLN
	ld	(PROGAD),hl
	pop	de
	ld	a,(hl)
	cp	','
	jr	z,ONLP
	ret

ONZ:
	pop	af		;cancel return address
	ld	a,d		;
	jp	COMMAND


;IF command
C_IF:
	call	NARGMO
	ld	a,(hl)
	cp	THEN_
	jr	z,IFOK
	cp	GOTO_
	jp	nz,SNERR
IFOK:
	ld	a,(FAC1+4)
	or	a
	jp	z,C_REM
;	call	SKIPSPINC
;	sub	'0'
;	cp	'9'-'0'+1

	rst	ANADAT

	jp	c,C_GOTO
IFEND:
	pop	af		;cancel return address
	jp	INTPRT		;skip colon check


;LPRINT command
_C_LPRT:ds	C_LPRT-_C_LPRT
	org	C_LPRT

	ld	a,01h
	jr	PRTDEV


;PRINT command
_C_PRT:	ds	C_PRT-_C_PRT
	org	C_PRT

	call	HOOKPRT
	call	DEVNUM
PRTDEV:
	ld	(DEVICE),a
	rlca
	call	c,PRTOPN

PRTLP1:
	call	CHKCLN		;a=(hl)
	jp	z,PRTEOL
PRTLP2:
	inc	hl
	cp	';'
	jr	z,PRTNEXT
	cp	','
	jr	z,PRTCMM
	cp	SPC_
	jr	z,SPCTAB
	sub	TAB_
	jr	z,SPCTAB

	dec	hl
	call	ARG
	ld	a,(ARGTYP)
	or	a
	jr	z,PRTNUM

;string
	call	STRARG2
	ld	hl,(PROGAD)
	or	a
	jr	z,PRTLP1
	ld	b,a
PRTSLP:
	ld	a,(de)
	inc	de
	call	PUTDEV
	djnz	PRTSLP
	jr	PRTLP1

PRTNUM:
	push	hl
	call	FTOA

	ld	a,(WIDTH)
	add	a,03h
	ld	c,a
	xor	a
	cpir
	ld	a,(CSRX)
	cp	c
	call	nc,PUTNL
	ld	hl,FAC3
	call	PUTS
	pop	hl
	jr	PRTLP1

PRTCMM:
	call	CMMMAIN
	call	nc,PUTNL

PRTNEXT:
	call	CHKCLN
	jr	nz,PRTLP2
	jr	PRTEND

CMMMAIN:
	ld	e,0eh
	ld	a,(DEVICE)
	or	a
	jr	z,CMM0
	dec	a
	jr	nz,CMM2

;printer
CMM1:
	ld	b,08h
TAB1:
	ld	a,(PRTPOS)
TABLP:
	sub	e
	jr	c,PUTSPNEG
	djnz	TABLP
	ret

;CRT
CMM0:
	ld	b,01h
TAB0:
	ld	a,(CSRX)
	dec	a
	jr	TABLP

PUTSPNEG:
	neg
PUTSP:
	ld	b,a
PUTSPLP:
	ld	a,' '
	call	PUTDEV
	djnz	PUTSPLP
	scf
	ret

CMM2:
;RS-232C,CMT
	ld	b,0eh
	jr	PUTSPLP

SPCTAB:
	call	SPCTABMAIN
	jr	PRTNEXT

SPCTABMAIN:
	push	af		;00=tab
	call	FNCNUMR
	push	hl
	call	FTOI1
	pop	hl
	pop	af
	or	a
	jr	nz,PRTSPC

PRTTAB:
	ld	b,01h
	ld	a,(DEVICE)
	or	a
	jr	z,TAB0		;0=CRT
	dec	a
	jr	z,TAB	1	;1=printer

PRTSPC:
	ld	a,e
	or	a
	jr	nz,PUTSP
	ret


;change to text screen and print new line in CRT
;if (CSRX)>1 then print new line
PRTNLXTXT:
	call	CHGTXT
PRTNLX:
	xor	a
	ld	(DEVICE),a
	ld	a,(CSRX)
	dec	a
	ret	z
	jp	PUTNL


PRTEOL:
;	xor	a
;	ld	(GRPFLG),a
	call	PUTNL

PRTEND:
	ld	(PROGAD),hl

;print# and input# end
;destroy: af,hl
_PIEND:	ds	PIEND-_PIEND
	org	PIEND

	call	HOOKPED
	ld	hl,DEVICE
	ld	a,(hl)
	ld	(hl),00h
	rlca
	ret	nc
	jp	WCLOSE


REDO:
	db	"?Redo from start", 0dh, 0ah, 00h

;input: a=input(0)/read(>0), hl=input address
;output: a=length, hl=next address, bc=start address
;destroy: af,bc
INPRDSTR:
	ld	(INPRD),a
	ld	b,00h		;length
	call	SKIPSP
	cp	22h
	jr	nz,IRSNDQ	;no double quotation

;with double quotation
	inc	hl
	push	hl
IRSLP1:
	ld	a,(hl)
	or	a
	jr	z,IRSEND
	inc	hl
	cp	22h
	jr	z,IRSEND
	inc	b
;	jp	z,LSERR
	jr	IRSLP1

;no double quotation
IRSNDQ:
	push	hl
IRSLP2:
	ld	a,(hl)
	or	a
	jr	z,IRSEND
	cp	','
	jr	z,IRSEND
	cp	':'
	jr	nz,IRSNZ
	ld	a,(INPRD)
	or	a
	jr	nz,IRSEND		;READ
IRSNZ:
	inc	hl
	inc	b
;	jp	z,LSERR
	jr	IRSLP2

IRSEND:
	ld	a,b
	pop	bc
	ret


;INPUT command
C_INPT:
	ld	a,h
	cp	0fah
	jp	nc,IDERR
	dec	hl
	ld	(STOPAD),hl
	inc	hl

	call	HOOKINP
	call	DEVNUM
	ld	(INPDEV),a
	dec	a
	jp	z,FCERR		;printer
	inc	a
	jr	nz,INPTSTRT

;INPUT #0,
	call	CHGTXT
	call	SKIPSP
	cp	22h		;double quotation mark
	jr	nz,INPTSTRT

PROMPT:
	inc	hl
	ld	a,(hl)
	or	a
	jp	z,SNERR
	cp	22h		;double quotation mark
	jr	z,PROMPTEND
	call	PRTC
	jr	PROMPT

PROMPTEND:
	call	SKIPSPINC
	cp	';'
	jp	nz,SNERR
	inc	hl

INPTSTRT:
	push	hl		;program address (push for "?Redo from start")

INPTMAIN:
	ld	(PROGAD),hl
	ld	a,(INPDEV)
	or	a
	jr	nz,INPTEXT

	call	INPT1

	pop	de		;program address
	pop	bc		;return address
	jp	c,STOP3
	push	bc		;return address
	push	de		;program address

	ld	a,0ffh
	ld	(STOPAD+1),a
	jr	INPTANA

;input from external device
;input: a=(INPDEV), hl=INPBUF-1
;destroy: af,bc,de,hl
INPTEXT:
	rlca
	push	af		;c-flag=0:RS-232C, 1=CMT
	call	c,INPOPN
	call	INPT1EX
	pop	af
	call	c,RCLOSE
;	jr	INPTANA

;analyze input buffer
INPTANA:
	ld	de,INPBUF
	ld	hl,(PROGAD)
INPTLP:
	call	CHKVAR
	ld	(PROGAD),hl
	ld	a,(INPBUF)
	or	a
	jr	z,INPTNEXT	;if no input, no change in varible
	push	de		;input buffer
	call	GETVAR
	pop	hl		;input buffer
	bit	7,c
	jr	nz,INPTSTR

INPTNUM:
	call	INPRDNUM
	jr	INPTERR

INPTSTR:
	xor	a
	call	INPRDSTR
	push	hl		;input buffer
	ld	h,b
	ld	l,c
	push	de		;variable address
	call	MAKESTR
	pop	de		;variable address
	ld	hl,STRDSC1
	ld	bc,0004h
	ldir
	pop	hl		;input buffer

INPTERR:
	call	CHKCLN		;for compatibility with N6X-BASIC
	ex	de,hl
	jr	z,INPTNEXT
	cp	','
	jr	z,INPTNEXT
	ld	a,(INPDEV)
	or	a
	jp	nz,FDERR
	ld	hl,REDO
	call	PUTS
	pop	hl		;program address
	jr	INPTSTRT

INPTNEXT:
	ld	hl,(PROGAD)
	ld	a,(hl)
	cp	','
	jr	nz,INPTEND

;comma
	inc	hl
	ld	a,(INPBUF)
	or	a
	jr	z,INPTLP
	ld	a,(de)
	inc	de
	cp	','
	jr	z,INPTLP
	ld	a,(INPDEV)
	or	a
	jp	nz,ODERR
	ld	a,'?'
	call	PRTC
	jp	INPTMAIN

;not comma
INPTEND:
	pop	af		;cancel program address
	ex	de,hl
;	jr	CHKEXTR

;Extra ignored check?
;used by EXAS BASIC compiler
;input: hl=input buffer address
;destroy: af,bc,de,hl
_CHKEXTR:ds	CHKEXTR-_CHKEXTR
	org	CHKEXTR

	ld	a,(hl)
	or	a
	ld	hl,EXTRA
	call	nz,PUTS
	jp	PIEND


;one line input for external device
;used by EXAS BASIC compiler
;output: hl=INPBUF-1
;destroy: af,bc
_INPT1EX:ds	INPT1EX-_INPT1EX
	org	INPT1EX

	ld	a,(INPDEV)
	rlca
	push	af		;c-flag=0:RS-232C, 1=CMT
	ld	hl,INPBUF
	ld	b,71
INPTEXTLP:
	pop	af		;c-flag=0:RS-232C, 1=CMT
	push	af
	jr	c,INPTCMT

INPT232:
	push	hl
INPT232LP:
	ld	a,37h
	out	(81h),a
	ld	a,(STOPFLG)
	cp	03h
	jp	z,STOPSP
	call	GETRSBF
	jr	z,INPT232LP
	pop	hl

INPTCHKRET:
	cp	0dh
	jr	z,INPTEXTEND
	ld	(hl),a
	inc	hl
	djnz	INPTEXTLP

INPTEXTEND:
	pop	af
	ld	(hl),00h
	ld	hl,INPBUF-1
	ret

INPTCMT:
	call	GETCMTTR
	jr	INPTCHKRET

EXTRA:
	db	"?Extra ignored", 0dh, 0ah, 00h


;READ command
C_READ:
	call	CHKVAR
	call	GETVAR
	push	bc		;variable name
	push	de		;variable address

	ld	hl,(DATAAD)
	ld	de,(DATALN)
	ld	a,(hl)
	cp	','
	jr	z,READDATA
	cp	':'
	jr	z,READLP3
	or	a
	jp	nz,ODERR

READLP1:
	inc	hl
READLP2:
	ld	c,a		;always a=0, double quation mark counter
	ld	a,(hl)
	inc	hl
	or	(hl)
	jp	z,ODERR
	inc	hl
	ld	e,(hl)
	inc	hl
	ld	d,(hl)

READLP3:
	inc	hl
	ld	a,(hl)
	or	a
	jr	z,READLP1
	cp	DATA_
	jr	z,READDATA
	cp	22h		;double quotation mark
	jr	z,READDQ
	cp	':'
	jr	z,READLP3
	cp	' '
	jr	z,READLP3
	sub	REM_
	jr	z,READREM

READLP4:
	inc	hl
	ld	a,(hl)
	or	a
	jr	z,READLP1
	bit	0,c
	jr	nz,SKIPCLN
	cp	':'
	jr	z,READLP3
SKIPCLN:
	cp	22h		;double quotation mark
	jr	nz,READLP4
READDQ:
	inc	c
	jr	READLP4

;a=0
READREM:
;	xor	a
	ld	b,a
	ld	c,a
	cpir
	jr	READLP2

READDATA:
	ld	(DATALN),de
	inc	hl
	pop	de		;variable address
	pop	bc		;variable name
	bit	7,c
	jr	nz,READSTR

READNUM:
	call	INPRDNUM
	jr	READERR

;de=variable address
READSTR:
	ld	a,b		;>0
	call	INPRDSTR
	ld	(de),a
	inc	de
	inc	de
	ld	a,c
	ld	(de),a
	inc	de
	ld	a,b
	ld	(de),a

;error check
READERR:
	call	CHKCLN
	jr	z,READNEXT
	cp	','
	jr	z,READNEXT
	ld	hl,(DATALN)
	ld	(LINENUM),hl
	jp	SNERR

READNEXT:
	ld	(DATAAD),hl
	ld	hl,(PROGAD)
	ld	a,(hl)
	cp	','
	ret	nz
	inc	hl
	jp	C_READ


;for INPUT/READ
;input: de=variable adderss, hl=program address
INPRDNUM:
	push	de		;variable address
	call	ATOF
	pop	de		;variable address
	push	hl		;input buffer/data address
	ld	hl,FAC1
	call	SETF
	pop	hl		;input buffer/data address
	ret


;get a numerical argument for function
;input: hl=program address
;output: FAC1
;destroy: FAC2,af,bc,de,hl
FNCNUM:
	call	CHKLPAR
FNCNUMR:
	call	ARG
	call	CHKRPAR
	jr	CHKNUM


;get a numerical argument and check result (?MO Error)
;input: hl=program address-1
;output: FAC1, hl=next address
;destroy: af,bc,de,FAC2
NARGMOINC:
	inc	hl

;get a numerical argument and check result (?MO Error)
;input: hl=program address
;output: FAC1, hl=next address
;destroy: af,bc,de,FAC2
NARGMO:
	call	CHKCLN
	jp	z,MOERR
;	call	NUMARG
;	ret


;get a numerical argument and check result
;input: hl=program address
;output: FAC1, hl=next address
;destroy: af,bc,de,FAC2
NUMARG:
	call	ARG
;	call	CHKNUM
;	ret


;check numeric or not
;output: z-flag (1=ok)
;destroy: af
CHKNUM:
	ld	a,(ARGTYP)
	or	a
	ret	z
	dec	a
CHKNERR:
	jp	z,TMERR
	jp	SNERR


;check string argument or not
;output: a=length, hl=string descriptor address, z-flag(1=ok)
;destroy: f
CHKSTR:
	ld	a,(ARGTYP)
	dec	a
	jr	nz,CHKSERR
	ld	hl,(FAC1+1)
	ld	a,(hl)
	ret

CHKSERR:
	inc	a
	jr	CHKNERR


;check (
;input: hl=program address
;output: hl=next address
;destroy: af
CHKLPAR:
	call	SKIPSP
	cp	'('
	jp	nz,SNERR
	call	CHKCLNINC
	ret	nz
	jp	MOERR


;check )
;input: hl=program address
;output: hl=next address
;destroy: af
CHKRPAR:
	ld	a,(hl)
	cp	')'
	jp	nz,SNERR
	inc	hl
SETPRGAD:
	ld	(PROGAD),hl
	ret


;alphabet, numeral, or katakana? (case ignored)
ALPNUM2:
	cp	0e0h
	ccf
	ret	c
	cp	0a1h
	ret	nc
	call	TOUPPER
;	call	ALPNUM
;	ret


;alphabet (capital) or numeral?
;input: a
;output: c-flag(1=yes,0=no)
;destroy: f
ALPNUM:
	cp	'Z'+1
	ret	nc
	cp	'A'
	ccf
	ret	c
;	jp	CHKFIG


;analyze for 0010h
;input: a
;output: z-flag(1=00h or 3ah), c-flag(1=30h-39h)
;destroy: f
CHKFIG:
	or	a
	ret	z
	cp	'0'
	ccf
	ret	nc
	cp	':'
;	ret	z
;	cp	'9'+1
	ret


;convert lowercase letter to uppercase letter
;input: hl=data address
;output: a
;destroy: f
_TOUPHL:ds	TOUPHL-_TOUPHL
	ld	a,(hl)

;0bef: used by 256 bytes monitor
;input: a=data
TOUPPER:
	cp	'z'+1
	ret	nc
TOUPPER2:
	cp	'a'
	ret	c
	sub	'a'-'A'
	ret


;OR operator
O_OR:
	call	F12TOI2
	ld	a,h
	or	d
	ld	h,a
	ld	a,l
	or	e
	ld	l,a
	jp	S2TOF


;AND operator
O_AND:
	call	F12TOI2
	ld	a,h
	and	d
	ld	h,a
	ld	a,l
	and	e
	ld	l,a
	jp	S2TOF


;NOT operator
O_NOT:
	call	CHKNUM
	call	EXFAC
	call	FTOI2
	ld	a,d
	cpl
	ld	h,a
	ld	a,e
	cpl
	ld	l,a
	jp	S2TOF


;>,=,< operator
;bit0: >
;bit1: =
;bit2: <
O_GTEQLT:
	ld	a,(ARGTYP)
	or	a
	jr	z,GTEQLTZ
	sub	03h		;string and string
	jp	nz,TMERR
	ld	(ARGTYP),a	;a=0
	call	CMPSTR
	jr	GTEQLTEND
GTEQLTZ:
	call	CMPF
GTEQLTEND:
	ld	a,c		;operator
	jr	z,CASE_EQ
	jr	nc,CASE_GT
CASE_LT:
	rrca
CASE_EQ:
	rrca
CASE_GT:
	rrca
	jr	c,SETTRUE
;	jr	SETFALSE

SETFALSE:
SETZERO:
	ld	hl,ZERO
SETF1:
	ld	de,FAC1
SETF:
	ld	bc,0005h
	ldir
	ret

SETF2:
	ld	de,FAC2
	jr	SETF

SETTRUE:
SETMNS1:
	ld	hl,MNSONE
	jr	SETF1


CMPSTR:
	ld	hl,STRDSC4
	ld	b,(hl)
	ld	(hl),00h
	ld	a,(STRDSC1)
	cp	b
	push	af		;flag
	jr	c,CMPSNC
	ld	a,b
CMPSNC:
	or	a
	jr	z,CMPSZ

	ld	de,(STRDSC1+2)
	ld	hl,(STRDSC4+2)
	ld	b,a
CMPSLP:
	ld	a,(de)
	cp	(hl)
	jr	nz,CMPSNZ
	inc	de
	inc	hl
	djnz	CMPSLP
CMPSZ:
	pop	af		;flag
	ret

CMPSNZ:
	pop	hl		;cancel stack
	ret


;input: a=integer (unsigned)
I1TOFA:
	ld	l,a

;convert 1- or 2-byte integer to float
;input: hl=integer (unsigned)
;output: FAC1
;destroy: af,b,hl
I1TOF:
	ld	h,00h
I2TOF:
	push	de
	ld	de,0000h
	ld	a,h
	or	l
	jr	z,I2TOFZERO
	ld	b,10h
I2TOFLP:
	add	hl,hl
	jr	c,I2TOFEND
	djnz	I2TOFLP

I2TOFEND:
	srl	h
	rr	l
	ex	de,hl
I2TOFEND2:
	ld	a,b
	add	a,80h
I2TOFZERO:
;	ld	(FAC1),hl
;	ld	(FAC1+2),de
	ld	(FAC1+4),a
	call	SETI4
	pop	de
	ret


;convert float to 2-byte integer [-65535,65535]
;(round toward minus infinity)
;input: FAC1
;output: de
;destroy: af,bc,hl
FTOI:
	ld	de,0000h
	ld	a,(FAC1+4)
	or	a
	ret	z
	sub	91h
	jp	nc,FCERR
	neg
	ld	b,a

	ld	hl,(FAC1)
	ld	a,h
	or	l		;fraction

	ld	hl,(FAC1+2)
	ld	c,h		;sign
	set	7,h
	dec	b
	jr	z,FTOIZ
FTOILP:
	srl	h
	rr	l
	jr	nc,FTOINC
	rlca			;a>0
FTOINC:
	djnz	FTOILP

FTOIZ:
	ex	de,hl
	bit	7,c		;sign
	ret	z

	neg			;set c-flag if a>0
	sbc	hl,de
	ex	de,hl
	ret


;get integer part from float
;(round toward zero)
;input: FAC1
;output: FAC1
;destroy: af,b,hl
GETINT:
	ld	a,(FAC1+4)
	cp	81h
	jp	c,SETZERO
	sub	0a0h
	ret	nc
	ld	hl,FAC1
GETILP1:
	add	a,08h		;
	jr	c,GETIC
	ld	(hl),00h
	inc	hl
	jr	GETILP1
GETIC:
	ld	b,a
	inc	b
	xor	a
GETILP2:
	rra
	scf
	djnz	GETILP2
	and	(hl)
	ld	(hl),a
	ret


;convert 1-byte signed integer to float
;input: b=integer (signed)
;output: FAC1
;destroy: af,bc,hl
_S1TOF:	ds	ABTOF-5-_S1TOF
	org	ABTOF-5
S1TOF:
	ld	a,b
	rlca
	sbc	a,a		;a=0(b<80h) or ffh(b>=80h)
	jr	ABTOF


;convert 2-byte integer to float for USR()
;input: ab
;output: FAC1
;destroy: af,bc,hl
_ABTOF:	ds	ABTOF-_ABTOF
	org	ABTOF

	ld	h,a
	ld	l,b
;	jr	S2TOF


;convert 2-byte signed integer to float
;input: hl=integer (signed)
;output: FAC1
;destroy: af,b,hl
S2TOF:
	bit	7,h
	jp	z,I2TOF
	xor	a
	sub	l
	ld	l,a
	sbc	a,h
	sub	l
	ld	h,a
	call	I2TOF
	jr	NEGABSNZ


;FAC1=-abs(FAC1)
;destroy: af,hl
NEGABS:
	ld	a,(FAC1+4)
	or	a
	ret	z

;FAC1=-abs(FAC1) without checking zero
;destroy: f,hl
NEGABSNZ:
	ld	hl,FAC1+3
	set	7,(hl)
	ret


;LPOS() function
F_LPOS:
	ld	a,(ARGTYP)
	cp	02h
	jp	nc,SNERR
	ld	hl,(PRTPOS)
	jp	SETI1


;POS() function
F_POS:
	ld	a,(ARGTYP)
	cp	02h
	jp	nc,SNERR
	ld	hl,(CSRX)
	dec	l
	jp	SETI1


;CSRLINE function
F_CSRL:
	ld	hl,(CSRY)
	dec	l
	jp	I1TOF


;DEF command
C_DEF:
	ld	de,(LINENUM)
	ld	a,d
	and	e
	inc	a
	jp	z,IDERR
	call	SKIPSP
	cp	FN_
	jp	nz,SNERR
	inc	hl
	call	CHKVAR
	bit	7,c
	jp	nz,TMERR
	set	7,b
	push	hl		;program address
	call	SRCHVAR
	call	c,MAKEVAR
	pop	hl		;program address
	call	CHKLPAR
	call	CHKVAR
	bit	7,c
	jp	nz,TMERR
	call	CHKRPAR
	call	SKIPSP
	cp	EQ_
	jp	nz,SNERR
	call	SKIPSPINC

	ex	de,hl
	ld	(hl),e
	inc	hl
	ld	(hl),d
	inc	hl
	ld	(hl),b
	inc	hl
	ld	(hl),c
;	inc	hl
;	ld	(hl),00h
	ex	de,hl

	dec	hl
DEFLP:
	call	CHKCLNINC
	jr	nz,DEFLP

	ld	(PROGAD),hl
	ret


;INP() function
F_INP:
	call	CHKNUM
	call	FTOI1
	ld	b,d		;d=0
	ld	c,e
	in	l,(c)
	ld	h,d		;d=0
	jp	I2TOF


;OUT command
C_OUT:
	call	INT1ARG
	ld	a,(hl)
	cp	','
	jp	nz,SNERR
	push	de		;
	call	INT1INC
	pop	bc		;
	out	(c),e
	ret


;increment hl and get a 1-byte integer argument [0,255]
_INT1INC:ds	INT1INC-_INT1INC
	org	INT1INC

	inc	hl

;get a 1-byte integer argument [0,255]
;input: hl=program address
;output: FAC1,a,de=integer, hl=next address
;destroy: FAC2,f,bc
_INT1ARG:ds	INT1ARG-_INT1ARG
	org	INT1ARG

	call	INT2ARG
	ld	a,d
	or	a
	ld	a,e
	ret	z
	jp	FCERR


;PEEK() function
F_PEEK:
	call	CHKNUM
	call	FTOI
	ld	a,(de)
	jp	I1TOFA


;POKE command
C_POKE:
	call	INTARG
	ld	a,(hl)
	cp	','
	jp	nz,SNERR
	push	de		;
	call	NARGMOINC
	call	FTOI1
	pop	hl		;
	ld	(hl),e
	ret


;get a 2-byte integer argument [-65535,65535]
;input: hl=program address
;output: FAC1,de=integer, hl=next address
;destroy: FAC2,af,bc
INTARG:
	call	NARGMO
	push	hl
	call	FTOI
	pop	hl
	ret


;get a 2-byte integer argument [-32768,32767]
;input: hl=program address
;output: FAC1,de=integer, hl=next address
;destroy: FAC2,af,bc
INT2ARG:
	call	NARGMO
	push	hl
	call	FTOI2
	pop	hl
	ret


;get two 2-byte integer arguments [-65535,65535]
;input: hl=program address
;output: FAC1, bc=1st, de=2nd, hl=next address
;destroy: af
INTARG2:
	call	INTARG
	ld	a,(hl)
	cp	','
	jp	nz,SNERR
	inc	hl
	push	de		;
	call	INTARG
	pop	bc		;
	ret


;get space attribute data for scroll
;output: a
;destroy: f
GETSPA:
	ld	a,(SCREEN1)
	dec	a
	jp	m,CNVATT1
	jr	z,GETSPA2

GETSPAG:
	dec	a
	ld	a,(COLOR3)
	jr	z,GETSPA3
	add	a,50h
GETSPA3:
	add	a,8ch		;screen mode 3=8ch, screen mode 4=dch
	ret

GETSPA2:
	ld	a,(COLOR2)
	or	a
	jr	nz,GETSPA2NZ
	ld	a,(COLOR1)

GETSPA2NZ:
	cp	05h
	ld	a,(COLOR3)
	jr	c,GETSPAC
	xor	02h
GETSPAC:
	add	a,60h
	ret


;get data from port 90h
;output: a
;destroy: f
_IN90H:	ds	IN90H-_IN90H
	org	IN90H

	ld	a,0ch
	out	(93h),a		;disable 8255 INT for writing
IN90HLP:
	in	a,(92h)
	cpl
	and	28h
	jr	nz,IN90HLP	;wait for ibf=1,intr=1
	ld	a,0dh
	out	(93h),a		;enable 8255 INT for writing
	in	a,(90h)
	ret


;output to port 90h
;input: a=data
;destroy: none
_OUT90H:ds	OUT90H-_OUT90H
	org	OUT90H

	push	af
	ld	a,08h
	out	(93h),a		;disable 8255 INT for reading
OUT90HLP:
	in	a,(92h)
	rlca
	jr	nc,OUT90HLP	;wait for nobf=1
	ld	a,09h
	out	(93h),a		;enable 8255 INT for reading
	pop	af
	out	(90h),a
	ret


PLSONE:
	db	00h, 00h, 00h, 00h, 81h

MNSONE:
	db	00h, 00h, 00h, 80h, 81h

ZERO:
	db	00h, 00h, 00h, 00h, 00h


;interrupt for graphic key
_INTGRP:ds	INTGRP-_INTGRP
	org	INTGRP

	push	bc
	ld	b,01h
	jr	KEYCOMMON


;interrupt for normal key
_INTKEY:ds	INTKEY-_INTKEY
	org	INTKEY

	push	bc
	ld	b,00h

KEYCOMMON:
	push	af
	call	IN90H

;0ebch: used by SPACE HARRIER
	push	de
	push	hl
	or	a
	jr	z,IKPOP
;	dec	b
;	jr	nz,NORMAL
	djnz	NORMAL

GRAPH:
	cp	0fch		;SHIFT+PAGE, only for emulator
	jr	z,RESSTOP
	cp	0feh		;KANA, only for emulator
	jr	z,RESSTOP

	call	CLICK
	cp	0fah		;STOP
	jr	z,STOPKEY
	jr	nc,IKPOP	;CAPS/MODE, for mk2
	ld	b,a		;;
	ld	hl,KYBFIN
	ld	c,(hl)		;
	ld	a,14h
	call	PUTKBF
	jr	c,RESSTOP
	ld	a,b		;;
	jr	IKBF2


;normal key
NORMAL:
	call	CLICK
	cp	03h		;Ctrl-C
	jr	z,STOPKEY2
	cp	1bh
	jr	z,STOPKEY2
	cp	7fh
	jr	nz,IKBF
	ld	a,08h		;DEL->ctrl-H, not necessary for emulator?

	jr	IKBF


;0ef6h: used by PORTOPIA
_IKBF:ds	IKBF-_IKBF
	org	IKBF

	ld	hl,KYBFIN
	ld	c,(hl)		;
	call	PUTKBF
	jr	c,RESSTOP
	cp	14h
	jr	nz,RESSTOP
	ld	a,0efh		;Ctrl-T->14h+0efh

IKBF2:
	call	PUTKBF
	jr	nc,RESSTOP

	ld	hl,KYBFIN
	ld	(hl),c		;
	jr	RESSTOP


;interrupt for reply to key query
INTGAM:
	push	af
	call	IN90H
	ld	(GMKYBUF),a
	pop	af
	ei
	ret


STOPKEY:
	call	HOOKSTP
	ld	a,03h
STOPKEY2:
	call	PUTKBF
	jr	SETSTOP

RESSTOP:
	xor	a
SETSTOP:
	ld	(STOPFLG),a
;	jr	IKPOP


;pop register for key interrupt
_IKPOP:	ds	IKPOP-_IKPOP
	org	IKPOP

	pop	hl
	pop	de

;0f2dh: used by SPACE HARRIER
	pop	af
	pop	bc
	ei
	ret


;put into PLAY command buffer
;input: a=data
;destroy: f,hl
PUTPLBF:
	call	GETPLAD
	jr	PUTBF


;put into key buffer
;input: a
;output: c-flag (1=buffer full)
;destroy: f,hl
PUTKBF:
	ld	hl,KYBFIN
;	call	PUTBF
;	ret


;put characer into buffer
;input: a=character, hl=buffer control address
;output: c-flag (1=buffer full)
;destroy: f,hl
PUTBF:
	push	af
	ld	a,(hl)		;in
	inc	a
	inc	l		;inc hl
	inc	l		;inc hl
	inc	l		;inc hl
	and	(hl)		;size
	dec	l		;dec hl
	dec	l		;dec hl
	cp	(hl)		;out
	jr	z,BFFULL

	dec	l		;dec hl
	ld	(hl),a		;in
	inc	l		;inc hl
	inc	l		;inc hl
	inc	l		;inc hl
	inc	l		;inc hl
	add	a,(hl)		;address
	inc	l		;inc hl
	ld	h,(hl)
	ld	l,a
	jr	nc,PTBNC
	inc	h
PTBNC:
	pop	af
	ld	(hl),a
	or	a		;reset c-flag
	ret

BFFULL:
	pop	af
	scf
	ret


;interrupt for RS-232C
INT232:
	push	af
	push	hl
	ld	a,17h		;RTS=0
	out	(81h),a

	in	a,(80h)
	ld	hl,RSBFIN
	call	PUTBF

	ld	a,37h		;RTS=1
	out	(81h),a
	pop	hl
	jr	POPAF


;interrupt for timer
_INTTIM:ds	INTTIM-_INTTIM
	org	INTTIM

	push	bc
	push	af
	push	de
	push	hl

	call	PLAYINT

	ld	hl,TMCNT
	inc	(hl)
	inc	(hl)
	jr	nz,IKPOP
	inc	l		;inc hl
	inc	(hl)
	jr	nz,TIMCSR
	inc	l
	inc	(hl)
	jr	nz,TIMCSR
	inc	l
	inc	(hl)

TIMCSR:
;cursor blink
	ld	a,(SCREEN1)
	ld	hl,CSRBLNK
	cpl
	rrca
	and	(hl)
	call	nz,CSRREV	;(SCREEN1)=0or1, (CSRBLNK)=1
	jr	IKPOP


;interrupt for CMT load
INTCMT:
	push	af
	call	IN90H
	ld	(CMTBUF),a
	ld	a,02h
SETCMTST:
	ld	(CMTSTAT),a
POPAF:
	pop	af
EIRET:
	ei
	ret


;interrupt for CMT write stop
INTWSTP:
;interrupt for CMT read stop
INTRSTP:
	push	af
	ld	a,03h
	ld	(STOPFLG),a
	jr	POPAF


;interrupt for CMT error
INTERR:
	push	af
	ld	a,10h
	jr	SETCMTST


;get input character
;output: a, z-flag(1=no input)
;destroy: f
_GETCH:	ds	GETCH-_GETCH
	org	GETCH

	push	hl
	call	GETCH2
	pop	hl
	ret


;wait for input and get a character
;output: a
;destroy: f
_GETC:	ds	GETC-_GETC
	org	GETC

	push	hl
GETCLP1:
	call	CSRON
	ld	h,00h
GETCLP2:
	inc	h
	call	z,PRTFKEY2
	call	GETCH
	jr	z,GETCLP2

	call	CSROFF
	cp	0feh		;page switching key
	jr	z,GETCPAGE
	ld	h,a
	call	CHGTXT
	ld	a,h
	pop	hl
	ret

GETCPAGE:
	ld	a,(SCREEN2)
	inc	a
	ld	hl,PAGES
	cp	(hl)
	jr	c,GETCC
	xor	a
GETCC:
	call	CHGSCR
	jr	GETCLP1


GETCH2:
	ld	a,(FKEYCNT)
	or	a
	jr	nz,GETFKEY
	ld	hl,GRPWRK
	or	(hl)		;a=0
	jr	z,GETCHZ	;(hl)=0?
	ld	(hl),00h
	cp	0efh		;Ctrl-T=14h+0efh
	ret	z
	add	a,30h
	ret			;z-flag=0

GETCHZ:
;(GRPWRK)=0
	call	GETKBF
	ret	z
	cp	14h
	ret	nz

;14h+**
	call	GETKBF
	cp	0f0h
	jr	nc,CNVFKEY
	ld	(GRPWRK),a
	ld	a,14h
	ret			;z-flag=0

;a=f0-f9
CNVFKEY:
	ld	hl,FKEYTBL+80h
;	and	0fh
	add	a,a
	add	a,a
	add	a,a
	add	a,l		;no carry
	ld	l,a
;	jr	nc,CNVFKNC
;	inc	h
;CNVFKNC:
	ld	a,07h
	jr	GETFKEY2

;function key
GETFKEY:
	ld	hl,(FKEYAD)
	dec	a
GETFKEY2:
	ld	(FKEYCNT),a
	ld	a,(hl)
	inc	hl
	ld	(FKEYAD),hl
	or	a
	ret	nz
	ld	(FKEYCNT),a	;=0
	ret			;z-flag=1: no input


;get a character from key buffer
;output: a,z-flag(1=no input)
;destroy: f,hl
_GETKBF:ds	GETKBF-_GETKBF
	org	GETKBF

	ld	hl,KYBFIN
CALLGETBF:
	di
	call	GETBF
	ei
	ret


;get a character from RS-232C buffer
;output: a,z-flag(1=no input)
;destroy: f,hl
GETRSBF:
	ld	hl,RSBFIN
	jr	CALLGETBF


;get from PLAY command buffer
;input: b=channel+1 (1,2,3)
;output: a,z-flag(1=no input)
;destroy: f,hl
GETPLBF:
	ld	hl,BUFAIN-6
	ld	a,b
	add	a,a		;*2
	add	a,b		;*3
	add	a,a		;*6
	add	a,l		;no carry
	ld	l,a
	jp	GETBF


;clear key buffer pointer
;destroy: hl,(af,bc,de)
_CLRKBF:ds	CLRKBF-_CLRKBF
	org	CLRKBF

	ld	hl,0000h
	ld	(KYBFIN),hl	;(KYBFIN),(KYBFOUT)=0
	ret


;send key-query to sub CPU and get reply
;output: a (space-0-left-right-down-up-stop-shift)
;destroy: f (no change in c-flag)
_STICK:	ds	STICK-_STICK
	org	STICK

	ld	a,0ffh
	ld	(GMKYBUF),a
	ld	a,06h
	di
	call	OUT90H
	ei
STICKLP:
	ld	a,(GMKYBUF)
	inc	a
	jr	z,STICKLP	;wait for bit6=0
	dec	a
	ret


;print a character in CRT
;input: a=character code
;destroy: none
_PRTC:	ds	PRTC-_PRTC
	org	PRTC

	push	af
	push	hl

	ld	hl,(GRPFLG)
	inc	l
	dec	l
	jr	nz,PRTGRP
	cp	20h
	jr	nc,PRTCNC
	push	de
	push	bc
	call	PRTCTL
	pop	bc
	pop	de
	jr	PRTCEND

PRTGRP:
	sub	30h
	ld	hl,GRPFLG
	ld	(hl),00h
PRTCNC:
	call	PRTCH

PRTCEND:
	pop	hl
	pop	af
	ret


;print a character in graphic mode (screen mode 3,4)
;input: a=character
;destroy: af,hl
PRT34:
	ld	hl,(CSRY)
	call	PRT34XY
CTLRGT:
	ld	hl,(CSRAD)
	inc	hl
	jr	CHKNL

;print a character in CRT (no control code)
;input: a=character code
;destroy: af,hl
_PRTCH:ds	PRTCH-_PRTCH
	org	PRTCH

	ld	hl,(SCREEN1)
	bit	1,l
	jr	nz,PRT34	;screen mode 3,4

	ld	hl,(CSRAD)
	call	PRT12

;next line?
CHKNL:
	ld	(CSRAD),hl
	ld	hl,CSRX
	inc	(hl)
	ld	a,(WIDTH)
	cp	(hl)
	ret	nc
CHKNL2:
	ld	(hl),1

	ld	a,(LASTLIN)
	ld	hl,(CONSOL1)	;l=(CONSOL1)
	sub	l
	jr	z,CTLLF
	ld	a,0ffh

CTLLF:
	inc	a		;0 or 1
	ld	hl,(CSRY)	;l=y+1,h=x+1

	push	hl
	push	de
	call	SETLINE
	pop	de
	pop	hl

	inc	l
	ld	a,(LASTLIN)
	cp	l
	jp	nc,SETCSR

;scroll
	push	de
	ld	d,h		;x+1
	ld	hl,(CONSOL1-1)	;h=(CONSOL1)
	jr	SKPPATCH1


;10eah: to be patched by iP6/iP6win
_PATCH1:ds	PATCH1-_PATCH1
	org	PATCH1
	db	00h

SKPPATCH1:
	ld	l,a
	sbc	a,h		;c-flag=1, (LASTLIN)-(CONSOL1)-1
	call	nc,SCRLUP

	ld	h,d		;x+1
	call	Y2AD
	call	DELLIN
	pop	de
	jr	SETCSR


PRTCTL:
	cp	14h
	jr	z,CTLGRP
	cp	07h
	jp	z,BELL
	sub	0ah
	jr	z,CTLLF
	dec	a
	jr	z,CTLHOM	;0bh
	dec	a
	jp	z,CTLL		;0ch
	ld	hl,(CSRY)	;l=y+1,h=x+1
	dec	a
	jr	z,CTLCR		;0dh
	sub	1ch-0dh
	jr	z,CTLRGT	;1ch
	dec	a
	jr	z,CTLLFT	;1dh
	dec	a
	jr	z,CTLUP		;1eh
	dec	a
	ret	nz
;	jr	z,CTLDWN	;1fh

CTLDWN:
	inc	l
	jr	CTLUPNZ

CTLLFT:
	dec	h
	jr	nz,SETCSR
	ld	a,(CONSOL1)
	cp	l
	ret	nc
	dec	l
	ld	a,(WIDTH)
	ld	h,a
	jr	SETCSR

CTLUP:
	ld	a,(CONSOL1)
	cp	l
	ret	z
	dec	l
	jr	nz,CTLUPNZ
	ld	l,01h
CTLUPNZ:
	ld	a,(LASTLIN)
	cp	l
	jr	nc,SETCSR
	ld	l,a
	jr	SETCSR

CTLGRP:
	ld	(GRPFLG),a	;a=14h
	ret

CTLHOM:
	ld	hl,(CONSOL1)	;l=(CONSOL1)

CTLCR:
	ld	h,1
	jr	SETCSR


;convert VRAM address to text screen (x,y)
;input: hl=address
;output: h=x+1, l=y+1(0=error)
;destroy: af
AD2XY:
	bit	1,h
	jr	z,AD2XYZ

	ld	a,h
	and	01h
	ld	h,a
	ld	a,l
	and	1fh
	add	hl,hl
	add	hl,hl
	add	hl,hl
	ld	l,h
	ld	h,a
	inc	h
	inc	l
	ret

AD2XYZ:
	ld	l,00h
	ret


;set cursor position
;input: h=x+1, l=y+1
;output: (fda8h)=y+1,(fda9h)=x+1,(fdaa-fdab)=VRAM address
;destroy: none
_SETCSR:ds	SETCSR-_SETCSR
	org	SETCSR

	push	hl
	ld	(CSRY),hl
	call	XY2AD
	ld	(CSRAD),hl
	pop	hl
	ret


;cursor blink on
;destroy: none
_CSRON:
	ds	CSRON-_CSRON
	org	CSRON

	push	af
	ld	a,01h
	ld	(CSRBLNK),a
	pop	af
	ret


;cursor blink off
;destroy: hl
_CSROFF:
	ds	CSROFF-_CSROFF
	org	CSROFF

	push	af
	xor	a
	ld	(CSRBLNK),a
	ld	a,(CSRSTAT)
	or	a
	call	nz,CSRREV
	pop	af
	ret


;reverse cursor
;destroy: af,hl
CSRREV:
	ld	hl,CSRSTAT
	ld	a,(hl)
	cpl
	ld	(hl),a
	ld	hl,(CSRAD)
	dec	h
	dec	h
	ld	a,(hl)
	bit	6,a		;semi-graphic mode?
	jr	nz,CSRSEMI
	xor	01h
	ld	(hl),a
	ret

CSRSEMI:
	inc	h
	inc	h
	ld	a,(hl)
	cpl
	ld	(hl),a
	ret


;get left edge address
;input: l=y+1
;output: de=VRAM address
;destroy: none
_Y2AD:ds	Y2AD-_Y2AD
	org	Y2AD

	push	af
	ld	a,l
	dec	a
	rrca
	rrca
	rrca
	ld	d,a
	and	0e0h
	ld	e,a
	xor	d
	ld	d,a
	ld	a,(VRAM)
	add	a,02h
	add	a,d
	ld	d,a
	pop	af
	ret


;get VRAM character address (screen mode 1,2)
;input: h=x+1, l=y+1
;output: hl=VRAM address
;destroy: none
_XY2AD:	ds	XY2AD-_XY2AD
	org	XY2AD

	push	af
	push	de
	call	Y2AD
	dec	h
	ld	l,h
	ld	h,00h
	add	hl,de
	pop	de
	pop	af
	ret


;delete line
;input: de=left edge address (for screen mode 1,2)
;destroy: none
_DELLIN:ds	DELLIN-_DELLIN
	org	DELLIN

	push	af
	push	hl
	push	de
	push	bc

	ld	a,(SCREEN1)
	cp	02h
	jr	nc,DELG

	push	de		;
	dec	d
	dec	d
	ld	h,d
	ld	l,e
	inc	de
	call	GETSPA
	ld	(hl),a
	ld	bc,COLUMNS-1
	ldir

	pop	hl		;
	ld	c,COLUMNS-1

DELLEND:
	ld	d,h
	ld	e,l
	inc	de
	call	GETSP
	ld	(hl),a
	ldir

	pop	bc
	pop	de
	pop	hl
	pop	af
	ret


;for graphic mode (screen3,4)
DELG:
	ex	de,hl
	call	AD2GAD
	ld	bc,0180h-1
	jr	DELLEND


;for graphic mode (screen mode 3,4)
SCRLG:
;bc=a*32*12=a*(256+128)
	ld	c,b		;b=0
	ld	b,a
	srl	b
	rr	c
	add	a,b
	ld	b,a

	ld	h,1
	call	XY2GAD
	ex	de,hl
	ld	hl,COLUMNS*12
	add	hl,de
	pop	af		;up/down
	jr	nc,SCRLATT
	ex	de,hl
	dec	de
	dec	hl

SCRLATT:
	call	LDIDR
	pop	bc
	pop	de
	pop	hl

;scroll INPUT prompt position
;z=0: up,h=first,l=last
;z=1: down,h=last,l=first
	push	hl
	push	de

	ld	e,0-01h
	jr	nz,SCRLIUP
	ld	e,01h
	ld	a,h
	ld	h,l
	ld	l,a
SCRLIUP:
	ld	a,(CSRAD+1)
	ld	d,a
	ld	a,(INPTPAG)
	xor	d
	and	0f0h
	jr	nz,SCRLIEND

	ld	a,(INPTXY)
	cp	h
	jr	c,SCRLIEND
	inc	l
	cp	l
	jr	nc,SCRLIEND
	add	a,e
	ld	(INPTXY),a
SCRLIEND:
	pop	de
	pop	hl
	ret


;print a character in CRT (no control code, no move in cursor)
;input: a=character code, h=x+1, l=y+1
;destroy: none
_PRTCHXY:ds	PRTCHXY-_PRTCHXY
	org	PRTCHXY

	push	hl
	call	XY2AD
	call	PRTCHAD
	pop	hl
	ret


;scroll up
;input: h=first line+1, l=last line+1
;destroy: af
_SCRLUP:ds	SCRLUP-_SCRLUP
	org	SCRLUP

	ld	a,l
	sub	h		;a=(LASTLIN)-(CONSOL1) < 16
;	ret	z
;	ret	c
	push	hl

;c-flag=0:scroll up, c-flag=1:scroll down
SCRLUD:
	push	de
	push	bc
	push	af		;up/down

;line connection status
	ld	l,h
	push	hl
	push	af		;up/down
	call	SETLINE		;get work area address
	ld	d,h
	ld	e,l
	ld	b,00h
	ld	c,a
	pop	af		;up/down
	jr	nc,SCRLNC1
	ld	(de),a		;>0
	dec	e		;dec de
	dec	l		;dec hl
	dec	l		;dec hl
	dec	l		;dec hl
SCRLNC1:
	inc	l		;inc hl
	call	LDIDR

	ld	(de),a		;>0
	ld	hl,(SCREEN1)
	dec	l
	dec	l
	pop	hl
	jp	p,SCRLG		;screen mode 3,4

	call	MUL32
	call	Y2AD
	ld	hl,COLUMNS
	add	hl,de
	pop	af		;up/down
	push	af		;up/down
	jr	nc,SCRLNC2
	ex	de,hl
	dec	de
	dec	hl

SCRLNC2:
	push	hl
	push	de
	push	bc
	call	LDIDR
	pop	bc
	pop	de
	pop	hl

;attribute
	dec	h
	dec	h
	dec	d
	dec	d
	pop	af		;up/down
	jp	SCRLATT


;scroll down
;input: h=last line+1, l=first line+1
;destroy: af
_SCRLDW:ds	SCRLDW-_SCRLDW
	org	SCRLDW

	ld	a,h
	sub	l
;	ret	z
;	ret	c
	push	hl
	scf			;set c-flag
	jr	SCRLUD


;print function key
;input: none
;output: none
;destroy: af
_PRTFKEY:ds	PRTFKEY-_PRTFKEY
	org	PRTFKEY

	xor	a
	ld	(FKEYSFT),a

PRTFKEY2:
	ld	a,(SCREEN1)
	cp	02h
	ret	nc		;screen mode 3 or 4

	push	hl
	push	de
	push	bc

	ld	hl,(HEIGHT)
	call	Y2AD

	call	STICK		;bit0=shift
	ld	hl,(CONSOL3)	;(CONSOL3)=0 or 1
	and	l
	rlca
	or	l
	ld	hl,FKEYSFT
	cp	(hl)
	jr	z,PFKEND

	ld	(hl),a
	rrca
	jr	nc,DELFK

	ex	de,hl
	ld	de,FKEYTBL
	rrca
	jr	nc,PFKSTRT
	ld	e,FKEYTBL+8*5-(FKEYTBL+8*5)/256*256	;de=FKEYTBL+8*5

PFKSTRT:
	call	CHRREV
	ld	a,(SCREEN2)
	add	a,'1'
	call	PRT12

	call	CHRREV
	ld	a,' '
	call	PRT12

	ld	c,05h
FKEYLP1:
	call	CHRREV
	ld	b,05h
FKEYLP2:
	ld	a,(de)
	or	a
	jr	z,FKEYNEXT
	cp	' '
	jr	nc,FKEYNC
	ld	a,' '
FKEYNC:
	call	PRT12
	inc	e		;inc de
	djnz	FKEYLP2

FKEYNEXT:
	inc	b
FKEYLP3:
	ld	a,b
	dec	a
	call	z,CHRREV
	ld	a,' '
	call	PRT12
	inc	e		;inc de
	djnz	FKEYLP3

	inc	e		;inc de
	inc	e		;inc de
	dec	c
	jr	nz,FKEYLP1

PFKEND2:
	call	SETCNSL2
PFKEND:
	pop	bc
	pop	de
	pop	hl
	ret

DELFK:
	call	DELLIN
	jr	PFKEND2


;ldir or lddr
;input: bc,de,hl,c-flag(0=ldir,1=lddr)
;output: bc=0,de,hl
;destroy: f
LDIDR:
	jr	c,LDIDRC
	ldir
	ret
LDIDRC:
	lddr
	ret


;get page data address
;input: a=page-1
;output: hl=address, bc=size
;destroy: af
GETPGAD:
	ld	hl,PAGE1
	ld	bc,PAGE2-PAGE1
	or	a
PGADLP:
	ret	z
	add	hl,bc
	dec	a
	jr	PGADLP


;get corrected color parameter
;output: a
;destroy: f
CORRC:
	ld	a,(SCREEN1)
	srl	a
	ld	a,(COLOR1)
	jr	nc,CORRC13
	jr	nz,CORRC4

;screen mode 2: 0~8
CORRC2:
	cp	08h
	ret	c
	ld	a,08h
	ret

;screen mode 1,3: 1~4
CORRC13:
	or	a
	jr	z,CORRCZ
	cp	04h
	ret	c
	ld	a,04h
	ret

;screen mode 4: 0~1
CORRC4:
	or	a
	ret	z
CORRCZ:
	ld	a,01h
	ret


;reverse character attribute
;destroy: af
CHRREV:
	call	CORRC
	xor	07h
	cp	05h
	jr	c,REVEND
	and	03h
REVEND:
	ld	(COLOR1),a
	jp	SETATT


;change to text screen
;input: none
;output: none
;destroy: af
CHGTXT:
	ld	a,(SCREEN3)
	call	CHGACT
	ld	a,(SCREEN1)
	cp	02h
	ret	c
	xor	a
CHGSCR:
	call	CHGACT
	jr	CHGDSP


;change screen mode
;input: a=mode-1
;destroy: af,de
_CHGMOD:ds	CHGMOD-_CHGMOD
	org	CHGMOD

	cp	04h
	jp	nc,FCERR
	push	hl
	ld	hl,SCREEN1
	ld	e,(hl)		;
	cp	e
	jr	z,CMODEND
	ld	(hl),a

	push	bc
	push	af		;;

	cp	02h
	ld	a,COLUMNS	;screen mode 1,2,4:width=32
	jr	nz,SETWID
	rra			;screen mode 3:width=16
SETWID:
	ld	(WIDTH),a

	ld	hl,M1COLOR
	ld	bc,0003h
	ld	d,b		;b=0
	add	hl,de		;
	add	hl,de
	add	hl,de
	ex	de,hl
	ld	hl,COLOR1
	ldir

	pop	af		;;
	ld	hl,M1COLOR
	ld	d,b		;b=0
	ld	e,a
	add	hl,de
	add	hl,de
	add	hl,de
	ld	de,COLOR1
	ld	c,03h		;b=0
	ldir

	call	GETSPA
	ld	hl,(VRAM-1)	;h=(VRAM)
	ld	l,b		;b=0
	ld	(hl),a
	inc	b
	ld	d,h
	ld	e,b		;de=hl+1
	dec	c		;bc=1ffh
	ldir

	call	SETCNSL2
	ld	c,01h
	ld	a,(HEIGHT)
	scf
	call	CLSMAIN

	pop	bc
CMODEND:
	pop	hl
	ret


;change display page
;page1=8000h or c000h	out (0b0h),4 or 0
;page2=e000h		out (0b0h),2
;page3=c000h		out (0b0h),0
;page4=a000h		out (0b0h),6
;input: a=page-1
;destroy: af
_CHGDSP:ds	CHGDSP-_CHGDSP
	org	CHGDSP

	ld	(SCREEN3),a
	or	a
	jr	nz,DSPOK
	ld	a,(BASICAD+1)	;84h or c4h
	rlca
	rlca
	add	a,a		;32KB:page1=8000h
				;16KB:page1=c000h
DSPOK:
;0,1,2,3->4,2,0,6
	inc	a
	cpl
	rlca
;	and	06h

	push	bc
	ld	b,00000110b
	call	OUTB0H
	pop	bc

	jp	PRTFKEY2


;change active page
;input: a=page-1
;destroy: none
_CHGACT:ds	CHGACT-_CHGACT
	org	CHGACT

	push	af
	push	hl

	ld	hl,SCREEN2
	cp	(hl)
	jr	z,ACTEND

	push	de
	push	bc

	ld	d,a		;
	ld	a,(hl)
	ld	(hl),d		;
	call	GETPGAD
	ld	a,d		;
	ex	de,hl
	ld	hl,VRAM		;top of active page data
	ldir

	call	GETPGAD
	ld	de,VRAM
	ldir

	call	PRTFKEY

	pop	bc
	pop	de

ACTEND:
	pop	hl
	pop	af
	ret


;check connection to previous character
;input: hl=VRAM address
;output: z-flag(1:connect)
;destroy: af
CHKLINE:
	ld	a,l
	and	1fh
	jr	nz,CHKLINENZ
	push	hl
	add	hl,hl
	add	hl,hl
	add	hl,hl
	ld	a,h
	and	0fh
	ld	hl,LINEST-1
	add	a,l		;no carry
	ld	l,a
	ld	a,(hl)
	pop	hl
	or	a
	ret

CHKLINENZ:
	xor	a
	ret


;cut connection before cursor line
;output: hl=line status address
;destroy: af,de
CUTLINE:
	ld	a,(CSRY)	;a>0

;input: a=y+1
;output: hl=line status address
;destroy: f,de
CUTLINE2:
	ld	l,a
	dec	l
;	jp	SETLINE

;set line connection status (connection to next line)
;input: l=y+1, a=data (0=connect or not)
;output: hl=line status address
;destroy: f,de
SETLINE:
	ld	h,00h
	ld	de,LINEST-1
	add	hl,de
	ld	(hl),a
	ret


;convert character VRAM address to graphic VRAM address
;(screen mode 1,2 -> screen mode 3,4)
;input: hl=character VRAM address
;output: hl=graphic VRAM address
;destroy: none
_AD2GAD:ds	AD2GAD-_AD2GAD
	org	AD2GAD

	push	af
	call	AD2XY
	pop	af
;	jr	XY2GAD


;get VRAM address (screen mode 3,4)
;input: h=x+1, l=y+1
;output: hl=graphic VRAM address =(VRAM+200h)+y*0180h+x
;destroy: none
XY2GAD:
	push	af
	dec	h
	dec	l
	ld	a,(SCREEN1)
	cp	02h
	jr	nz,XY2GADC
	sla	h		;screen mode 3
XY2GADC:
	sla	h
	ld	a,(VRAM)
	add	a,02h
	add	a,l
	srl	l
	rr	h
	add	a,l
	ld	l,h
	ld	h,a
	pop	af
	ret


;get CGROM address (DE=6000h+A*10h)
;input: a=character code
;output: de=address
;destroy: none
_CGROM:	ds	CGROM-_CGROM
	org	CGROM

	push	af
	rlca
	rlca
	rlca
	rlca
	ld	d,a
	and	0f0h
	ld	e,a
	xor	d
	add	a,60h
	ld	d,a
	pop	af
	ret

;	push	af		;push flag
;	ex	de,hl
;	ld	h,06h
;	ld	l,a
;	add	hl,hl
;	add	hl,hl
;	add	hl,hl
;	add	hl,hl
;	ex	de,hl
;	pop	af		;pop flag
;	ret


;print a character in CRT (no control code, no move in cursor)
;input: a=character code, hl=VRAM address
;destroy: none
_PRTCHAD:ds	PRTCHAD-_PRTCHAD
	org	PRTCHAD

	push	hl
	push	af
	ld	a,(SCREEN1)
	cp	02h
	jr	c,PRTCHXY12

;screen mode 3,4
	call	AD2XY
	pop	af
	push	af
	call	PRT34XY
	jr	PRTCHADEND

PRTCHXY12:
	pop	af
	push	af
	call	PRT12

PRTCHADEND:
	pop	af
	pop	hl
	ret


;print a character in text mode (screen mode 1,2)
;input: a=character code, hl=VRAM address
;output: hl=hl+1
;destroy: af
PRT12:
	ld	(hl),a
	call	CNVATT1
	dec	h
	dec	h
	ld	(hl),a
	inc	h
	inc	h
	inc	hl
	ret


;print a character in graphic mode (screen mode 3,4)
;input: a=character code, h=x+1, l=y+1
;destroy af,hl
PRT34XY:
	push	de
	push	bc

	call	CGROM
	ld	a,(COLOR1)
	call	SETATT

	call	XY2GAD

	ld	a,04h
	out	(93h),a		;CGROM ON

	ld	b,0ch
PRT34XYLP:
	push	de
	ld	a,(SCREEN1)
	rrca			;
	ld	a,(de)
	jr	nc,CALL3	;
	call	PRT4
	jr	CALL34END
CALL3:
	call	PRT3
	dec	hl

CALL34END:
	ld	de,COLUMNS
	add	hl,de
	pop	de
	inc	e		;inc de
	djnz	PRT34XYLP

	ld	a,05h
	out	(93h),a		;CGROM OFF

	pop	bc
	pop	de
	ret

PRT3:
	ld	e,a
	call	PRT3HALF
	inc	hl
PRT3HALF:
	ld	d,04h
PRT3LP:
	rlc	e
	rla
	rrca
	rlca
	rla
	dec	d
	jr	nz,PRT3LP

PRT4:
	ld	c,a
	ld	a,(ATTDAT)
	xor	(hl)
	and	c
	xor	(hl)
	ld	(hl),a
	ret


;click sound
;destroy: de
CLICK:
	push	af
	ld	a,(CONSOL4)
	or	a
	jr	z,SKPCLK

	push	bc

	ld	a,08h		;register8=ch.A volume
	ld	e,0fh
	call	SETPSG2
	push	de		;

	ld	a,01h		;register1=ch.A coarse tune
	ld	e,00h
	ld	b,e
	ld	c,a
	call	SETPSG2		;;

	call	WAITLP		;bc=0001h
	ld	a,01h		;register1=ch.A coarse tune
	ld	e,d		;;
	call	SETPSG

	ld	a,08h		;register8=ch.A volume
	pop	de		;
	ld	e,d
	call	SETPSG

	pop	bc

SKPCLK:
	pop	af
	ret


;graphic cordinates -> screen mode cordinates
;input: bc=graphic X, de=graphic Y
;output: bc=screen mode X, de=screen mode Y, (c-flag=1 for EXAS BASIC compiler)
;destroy: af
_GXY2SXY:ds	GXY2SXY-_GXY2SXY
	org	GXY2SXY

	push	hl
	call	CHKGXY
	pop	hl
	ld	a,(SCREEN1)
	srl	a
	jr	c,GXY2SXY24
	jr	nz,GXY2SXY3

;screen mode 1: 32x16
GXY2SXY1:
	push	bc
	call	DIV12
	pop	bc
	srl	c
SRLC2:
	srl	c

;screen mode 3: 128x192
GXY2SXY3:
	srl	c
	scf			;for EXAS BASIC compiler
	ret

GXY2SXY24:
;	scf
	ret	nz		;screen mode 4: 256x192

;screen mode 2: 64x48
GXY2SXY2:
	srl	e
	srl	e
	jr	SRLC2


;de=de/12
;input: de<192
;output: de=int(de/12), a=de mod 12
;destroy: f,b=0,l
DIV12:
	ld	a,e
	ld	e,d		;d=0
	ld	l,01100000b
	ld	b,04h
DIV12LP:
	cp	l
	jr	c,DIV12C
	sub	l
DIV12C:
	ccf
	rl	e
	srl	l
	djnz	DIV12LP
	ret


;graphic cordinates -> VARM address
; 60: screen1=+0000h, screen2-4=+0200h
; 66: screen1=+0000h, screen2=+0400h, screen3-4=+2000h
;input: bc=graphic X, de=graphic Y
;output: hl=VRAM address, a=bit
;destroy: af,bc,e
GXY2AD:
	call	GXY2SXY
	jp	SXY2AD


;screen mode cordinates -> VARM address
; 60: screen1=+0000h, screen2-4=+0200h
; 66: screen1=+0000h, screen2=+0400h, screen3-4=+2000h
;input: bc=screen mode X, de=screen mode Y
;output: hl=(fdb3-fdb4)=attribute address, a=(fdb2)=bit
;destroy: f
_SXY2AD:ds	SXY2AD-_SXY2AD
	org	SXY2AD

	jp	SXY2ADMAIN


;set attribute data
;set attribute data
;input: a=color code
;output: a,(ATTDAT)=attribute, (c-flag=0 for EXAS BASIC compiler)
;destroy: f
_SETATT:ds	SETATT-_SETATT
	org	SETATT

	call	CNVATT
	ld	(ATTDAT),a
	ret


;get color code
;input: (fdb3h-fdb4h)=attribute address, (fdb2h)=bit
;output: a=color
;destroy: f,hl
_GETCOLR:ds	GETCOLR-_GETCOLR
	org	GETCOLR

	jp	GETCOLRMAIN


;convert to attribute data (screen mode 1)
; or color code (screen moode 2)
; or bit pattern (screen mode 3,4)
;input: a=color code
;output: a=attribute, (c-flag=0 for EXAS BASIC compiler)
;destroy: f
CNVATT:
	push	af		;color
	ld	a,(SCREEN1)
	srl	a
	jr	z,ATT12		;screen mode 1 or 2
	jr	nc,ATT3		;screen mode 3

ATT4:
	pop	af
	or	a
	ret	z
	ld	a,0ffh
	ret

ATT3:
	pop	af
	or	a
	ret	z

	push	bc
	ld	b,04h
	cp	b
	jr	c,ATT3C
	ld	a,b
ATT3C:
	dec	a
	ld	c,a
	dec	b		;=3
ATT3LP:
	rrca
	rrca
	or	c
	djnz	ATT3LP
	pop	bc
	ret

ATT12:
	jr	nc,ATT1

ATT2:
	pop	af
	cp	09h
	ccf			;reset c-flag for EXAS BASIC compiler
	ret	nc
	ld	a,08h
	or	a		;reset c-flag for EXAS BASIC compiler
	ret


;get attribue data for screen mode 1
;output: a=attirbute value
;destroy: f
CNVATT1:
	ld	a,(COLOR1)
	push	af
ATT1:
	pop	af
	or	a
	jr	z,ATT1OK
	dec	a
	cp	04h
	jr	c,ATT1OK
	ld	a,03h
ATT1OK:
	add	a,20h
	push	hl
	ld	hl,COLOR3
	xor	(hl)
	pop	hl
	ret


;get space character data for scroll
;output: a
;destroy: f
GETSP:
	ld	a,(SCREEN1)
	dec	a
	ld	a,' '
	ret	m		;screen mode 1
	ld	a,(COLOR2)
	jr	nz,CNVATT	;screen mode 3,4

;screen mode 2
GETSP2:
	or	a
	jr	z,GETSP2Z	;color,0
	dec	a
	rrca
	rrca
	or	3fh
	ret

GETSP2Z:
	ld	a,(COLOR1)
	or	a
	ret	z
	dec	a
	cp	04h
	jr	c,GETSP2ZC
	sub	04h
GETSP2ZC:
	rrca
	rrca
	ret


;cold start (initialize hardware)
COLD:

;8255 (port 93h)
	ld	bc,0693h
	ld	hl,IOTBL93
	otir

;8251 (port 81h)
	ld	bc,0581h
;	ld	hl,IOTBL81
	otir

;for mkII
;port f0h-f8h and port c1h
	ld	bc,09efh
;	ld	hl,IOTBLF0
F0LP:
	inc	c
	outi
	jr	nz,F0LP
	ld	a,06h
	out	(0c1h),a	;32x16 text mode

;interrupt
;	ld	hl,INTTBL
	ld	de,0fa02h
	ld	c,11*2		;b=0
	ldir
	ld	a,d
	ld	i,a
	im	2

;clear work area
	xor	a
	ld	h,d
	ld	l,e
	ld	(hl),a
	inc	de
	ld	bc,0ffffh-0fa18h
	ldir

;stack pointer
	ld	sp,0f9ffh-50	;temporary

;relay,VRAM,timer
;	xor	a
	out	(0b0h),a	;bit3=CMT relay, bit21=VRAM address=c000, bit0=timer on

	dec	a
	ld	(BAUD),a	;a=ffh:1200 baud

;misc
	ld	a,' '
	ld	(ASTSTAT),a
	ld	a,01h
	ld	(CONSOL4),a
	ld	a,0eh
	ld	(CMMCNST),a	;used by EXAS BASIC compiler

;hook
	ld	hl,HOOK
	ld	e,03h		;d=0
	ld	b,1eh
COLDLP1:
	ld	(hl),0c9h
	add	hl,de
	djnz	COLDLP1


;buffer
	ld	hl,KEYBUF
	ld	e,40h		;d=0
	ld	(KYBFAD),hl
	add	hl,de		;hl=RSBUF
	ld	(RSBFAD),hl
	add	hl,de		;hl=BUFA
	ld	(BUFAAD),hl
	add	hl,de		;hl=BUFB
	ld	(BUFAAD+6),hl
	add	hl,de		;hl=BUFC
	ld	(BUFAAD+12),hl

	ld	hl,KYBFSZ
	ld	e,06h		;d=0
	ld	b,e		;e=6
COLDLP2:
	ld	(hl),3fh	;KYBFSZ,RSBFSZ,,BUFASZ,BUFBSZ,BUFCSZ
	add	hl,de
	djnz	COLDLP2

;command jump table
	ld	hl,CMDLST
	ld	de,CMDTBL
	ld	c,2*(CMDLAST-80h+1)	;b=0
	ldir

;function jump table
;	ld	hl,FNCLST
	ld	de,FNCTBL
	ld	c,2*(FNCLAST-FNC1ST+1)	;b=0
	ldir

;screen
	ld	a,ROWS
	ld	(HEIGHT),a

	ld	de,VRAM		;top of active page data
	ld	a,05h
COLDLP3:
	ld	hl,PAGEDATA
	ld	c,PAGE1-VRAM	;b=0
	ldir
	dec	a
	jr	nz,COLDLP3

;	ld	de,PAGE4+(PAGE4-PAGE3)
	ex	de,hl
	ld	de,0-(PAGE4-PAGE3)
	ld	a,0a0h
COLDLP5:
	add	hl,de
	ld	(hl),a		;(PAGE4)=a0h, (PAGE3)=c0h, (PAGE2)=e0h
	add	a,20h
	jr	nz,COLDLP5

;RND()
;	call	SETZERO
	call	RNDMNS

;16KB or 32KB?
	ld	hl,8000h

;for iP6/iP6win: 16KB if patched
	ld	a,(PATCH1)
	or	a
	jr	nz,RAM16K

	ld	a,(hl)
	cpl
	ld	(hl),a
	cp	(hl)
	ld	a,04h
	jr	z,RAM32K
RAM16K:
	ld	h,0c0h
	ld	a,02h
RAM32K:
	ld	(PAGES),a
	ld	b,a		;

	ld	a,h
	ld	(VRAM),a
	set	2,h		;h=h+40h
	ld	(BASICAD),hl	;l=0
	inc	l		;inc hl
	ld	(STARTAD),hl

COLDLP4:
	ld	a,b		;
	dec	a
	call	CHGACT
	call	CLS
	djnz	COLDLP4

	xor	a
	call	CHGDSP

;PLAY
	call	PLSTOP		;ei

	jp	HOT


;analyze an argument
;input: hl=program address
;output: hl=next address, FAC1=numerical value or pointer to string descriptor
;	 (ARGTYP)=0(numeric), 1(string), other(cannot analyze)
;destroy: FAC2,af,bc,de,hl
ARG:
	push	hl
	call	SETZERO		;for first +-
	pop	hl
	xor	a
	ld	e,a
	push	de		;e=dummy operator
	dec	a
	ld	(ARGTYP),a	;ff=unknown
	ld	(OPRTR),a
ARGLP:
	push	hl
	call	CHKFRE
	pop	hl
ARGLP2:
	call	SKIPSP
	ld	b,a		;
	inc	hl
	ld	(PROGAD),hl

;command, function, operator
	or	a
	jp	m,ARGCMD

;previous argument without operator?
	ld	a,(OPRTR)
	or	a
	jr	z,ARGTAIL
;[A-Z]
	ld	a,b		;
	sub	'A'
	cp	'Z'-'A'+1
	jr	c,ARGVAR
;[0-9&.]
	sub	'0'-'A'
	cp	'9'-'0'+1
	ld	a,b		;
	jr	c,ARGNUM
	cp	'&'
	jr	z,ARGNUM
	cp	'.'
	jr	z,ARGNUM
;others
	cp	22h		;double quotation mark
	jp	z,ARGSTR
	cp	'('
	jr	z,PARL
ARGTAIL:
	dec	hl
	ld	(PROGAD),hl
	xor	a
	jp	OPR

PARL:
	call	CHKCLN
	jp	z,MOERR
	call	ARG
	ld	a,(ARGTYP)
	cp	02h
	jp	nc,SNERR
	call	CHKRPAR
	jr	ARGLP

ARGNUM:
	call	ARGNCHK
	dec	hl
	call	ATOF
	jr	ARGLP2

ARGNCHK:
	xor	a
	ld	(OPRTR),a
	ld	(ARGTYP),a
	ret

ARGVAR:
	push	hl		;program address
	call	GETNAME
	ld	a,(ARGTYP)
	push	af		;;
	ld	a,(OPRTR)
	push	af		;;;
	ex	de,hl
	call	PUSHF1
	ex	de,hl

	push	bc		;variable name
	ld	a,(hl)
	cp	'('
	jr	z,ARGVARR
	ld	(PROGAD),hl
	call	SRCHVAR
	jr	nc,ARGVNC
	ld	de,ZERO
	jr	ARGVNC
ARGVARR:
	call	GETARR
ARGVNC:
	pop	bc		;variable name
	call	POPF1
	pop	af		;;;
	ld	(ARGTYP),a
	pop	af		;;
	ld	(OPRTR),a
	pop	hl		;program address
	bit	7,c
	jr	nz,ARGVSTR
ARGVNUM:
	call	ARGNCHK
	ex	de,hl
	ld	de,FAC1
	ldi
ARGVLDIR:
	ld	bc,0004h
	ldir
	jp	ARGNEXT2


;string variable
ARGVSTR:
	call	ARGSCHK
	ex	de,hl
	ld	de,STRDSC1
	jr	ARGVLDIR

ARGSTR:
	call	ARGSCHK
	ld	(STRDSC1+2),hl
	ld	e,00h
ARGSLP:
	ld	a,(hl)
	or	a
	jr	z,ARGSOK
	inc	hl
	cp	22h		;double quotation mark
	jr	z,ARGSOK
	inc	e
	jp	z,LSERR
	jr	ARGSLP
ARGSOK:
	ld	(STRDSC1),de
	jp	ARGLP2

;destroy: af,bc
ARGSCHK:
	xor	a
	ld	(OPRTR),a	;a=0
	inc	a
	ld	(ARGTYP),a	;a=1
	ld	bc,STRDSC1
	ld	(FAC1+1),bc
	ret

ARGCMD:
	cp	FNCLAST+1	;0f2h
	jp	nc,SNERR
	sub	PLUS_		;0cah
	cp	FNC1ST-PLUS_
	ld	a,b		;
	jp	c,OPR		;0cah-0d3h

	ld	a,(OPRTR)
	or	a
	jp	z,ARGTAIL

	ld	a,b		;
	cp	NOT_		;0c8h
	jp	z,OPR
	cp	SCREEN_		;9fh
	jr	z,FNC
	cp	FN_		;0c4h
	jr	z,FNC
	cp	INKEY_		;0c6h
	jr	z,FNC
	cp	PLUS_		;0cah
	jp	c,ARGTAIL

;	jr	FNC

;function
FNC:
	xor	a
	ld	(ARGTYP),a
	ld	a,b		;
	cp	INKEY_
	jp	z,F_INKY
	cp	FN_
	jp	z,F_FN
	cp	SCREEN_
	jp	z,F_SCRN

	call	CALLFNC
FNCRTN:
	ld	hl,(PROGAD)
CLRSTRD:
	ld	a,(ARGTYP)
	or	a
	jr	nz,CLRSTRDNZ
	ld	(STRDSC1),a	;=0
CLRSTRDNZ:
	xor	a
	ld	(OPRTR),a
	jp	ARGLP


;set border attribute
;input: a=color code
;destroy: af
_SETBO:	ds	SETBO-_SETBO
	org	SETBO

	ld	(BORDERC),a
	call	CNVATT
	ld	(BORDERA),a
	ret


CALLFNC:
	cp	STICK_
	jr	z,STKSTR
	cp	STRIG_
	jr	nz,NOTSTKSTR
STKSTR:
	dec	hl		;for STICK(),STRIG()

NOTSTKSTR:
	cp	LEFT_
	jr	nc,SKIPARG

;SGN()...CHR$()
	push	af
	call	CHKLPAR
	call	ARG
	call	CHKRPAR
	pop	af

SKIPARG:
	ld	de,FNCTBL-(FNC1ST-80h)*2
	cp	USR_
	jp	nz,JPTBL

;USR() function
	push	hl		;program address
	call	JPTBL
	call	CHKNUM
	xor	a
	ld	(STRDSC1),a
	pop	af		;cancel program address
	ret


;operator
OPR:
	cp	LT_		;0d3h
	jr	nz,OPRNZ
	inc	a		;0d3h -> 0d4h
OPRNZ:
	ld	d,a		;operator
	ld	a,(OPRTR)
	inc	a
	cp	02h
	jp	nc,OPROPR	;not 00 nor ff
	ld	a,(ARGTYP)
	inc	a
	ld	a,d		;operator
	jr	nz,NOTHEAD
;first +, for numeric and string
	cp	PLUS_
	jr	z,ARGNEXT2
	cp	MINUS_
	jr	z,NOTHEAD
	cp	NOT_
	jr	z,NOTHEAD
	or	a
	jp	nz,SNERR
NOTHEAD:
	call	GETPRIO
	ld	e,a
	pop	bc		;c=previous operator
	push	bc
	ld	a,c
	or	a
	jr	z,ARGHEAD

	call	GETPRIO
	cp	e
	jr	nc,CALLOP
	ld	a,d		;operator
ARGNEXT:
	ld	(OPRTR),a
	ld	a,(ARGTYP)
	dec	a
	jr	z,ARGNEXTS
	call	PUSHF1
	jr	ARGNEXT2
ARGNEXTS:
	call	COPYSTR
	ld	hl,(FAC1-1)	;h=length, l=operator
	res	6,l
	push	hl
ARGNEXT2:
	ld	hl,(PROGAD)
	jp	ARGLP


CALLOP:
	ld	a,(ARGTYP)
	dec	a
	jr	z,CALLOPS

;numeric
	call	CPYFAC
	call	POPF1

CALLOPEND:
	bit	6,c		;c=previous operator
	jr	nz,TYPNUM
	set	6,c
	ld	hl,ARGTYP
	set	1,(hl)
TYPNUM:
	push	de		;;d=following operator
	ld	a,c
	cp	GT_+1
	jr	c,CALLOPOK
;>,=,<
	ld	a,GT_

CALLOPOK:
	ld	de,OPRTBL-(0c8h-80h)*2
	call	JPTBL
	xor	a
	ld	(OPRTR),a
	pop	af		;;a=following operator
	jp	OPR

;string
CALLOPS:
	push	bc		;c=previous operator
	ld	a,(STRDSC1)
	ld	hl,(STRDSC1+2)
	call	BACKSTR
	ld	(STRDSC4),a
	ld	(STRDSC4+2),hl
	pop	bc
	pop	hl
	ld	(FAC1-1),hl
	jr	CALLOPEND

ARGHEAD:
	add	a,d		;d=0?
	jr	nz,ARGNEXT
	pop	hl		;cancel dummy operator
	ld	hl,(PROGAD)
	ret

;operator and operator
OPROPR:
	ld	a,d		;operator
	cp	NOT_		;0c8h
	jr	z,ARGNEXT
	cp	PLUS_		;0cah
	jr	z,ARGNEXT2
	cp	MINUS_		;0cbh
	jr	z,OPRMNS
	cp	GT_		;0d1h
	jp	c,SNERR

;>,=,<
;bit0: >
;bit1: =
;bit2: <
	ld	hl,OPRTR
	ld	a,(hl)
	cp	GT_
	jp	c,SNERR
	or	d
	cp	(hl)
	jp	z,SNERR
	ld	(hl),a

;copy numeric/string bit
	and	07h		;<=> bits
	pop	bc		;c=previous operator
	or	c
	ld	c,a
	push	bc
	jp	ARGNEXT2

;-
OPRMNS:
	call	SETMNS1
	ld	a,ASTRSK_	;0cch
	jp	ARGNEXT


;input: a=operator
;output: a=priority
;destroy: f,hl
GETPRIO:
	or	a
	ret	z
	ld	hl,PRIO+GT_-NOT_
	or	40h		;numeric/string bit
	sub	GT_		;0d1h
	jr	nc,PRIONC	;>,=,<
	add	a,l
	ld	l,a
	jr	c,PRIONC
	dec	h
PRIONC:
	ld	a,(hl)
	ret

PRIO:
;operator priority table
;		not   +  -  *  /  ^ and or >=<
	db	3, 0, 5, 5, 6, 6, 7, 2, 1, 4


;check string descriptor and copy
;destroy: af,bc,de,hl
COPYSTR:
	ld	hl,STRDSC4
	ld	a,(hl)
	or	a
	jp	nz,STERR
	dec	hl		;STRDSC3+3
	ld	de,STRDSC4+3
	ld	bc,000ch
	lddr			;STRDSC3->STRDSC4, STRDSC2->STRDSC3, STRDSC1->STRDSC2
;	xor	a
	inc	hl
	ld	(hl),a		;(STRDSC1)=0
	ret


;copy back string descriptor
;destroy: f,bc
BACKSTR:
	push	hl
	push	de
	ld	hl,STRDSC2
	ld	de,STRDSC1
	ld	bc,000ch
	ldir			;STRDSC2->STRDSC1, STRDSC3->STRDSC2, STRDSC4->STRDSC3
	ex	de,hl
	ld	(hl),b		;(STRDSC4)=0
	pop	de
	pop	hl
	ret


;send graphic data to PC-6021
;input: c
;destroy: af
SENDGRP:
	ld	a,80h
SENDGLP:
	rlc	c
	rra
	jr	nc,SENDGLP
	jr	PRINTER


;put a character to printer
;input: a
;destroy: none
_PUTPRT:ds	PUTPRT-_PUTPRT
	org	PUTPRT

	push	af
	ld	a,(GRPFLG)
	or	a
	jr	z,NOTGRP
	xor	a
SETGRP:
	ld	(GRPFLG),a
	pop	af
	ret

NOTGRP:
	pop	af
	push	af
	cp	14h
	jr	z,SETGRP
	call	CNVKANA2
	call	PRINTER
	pop	af
	ret


;put data to printer
;input: a
;destroy: af
PRINTER:
	push	af
PRINTERLP:
	in	a,(0c0h)
	and	02h
	jr	nz,PRINTERZ	;ready
	call	STOPESC
	jr	PRINTERLP

PRINTERZ:
	pop	af
	cpl
	out	(91h),a
	ld	a,01h
	out	(93h),a		;strobe (>1us)
	xor	a
	out	(93h),a
	ret


;convert hiragana -> katakana
;input: a
;output: a
;destroy: f
_CNVKANA:ds	CNVKANA-_CNVKANA
	org	CNVKANA

	cp	86h
	ret	c
	cp	0a0h
	jr	c,HIRAGANA
	cp	0e0h
	ret	c
HIRAGANA:
	xor	20h
	ret


;CMT open for verify
;destroy: a
VRFOPN:
	ld	a,0ffh
	ld	(VERIFY),a
;	jr	ROPEN


;open for tape read
;destroy: none
_ROPEN:ds	ROPEN-_ROPEN
	org	ROPEN

	push	af
	push	bc
	ld	b,1eh
	call	OPENCMT
	call	RLON
	pop	bc
	pop	af
	ret


;get 1 character from CMT
;output: a=data, z(0=error, 1=no error)
;destroy: f
_GETCMT:ds	GETCMT-_GETCMT
	org	GETCMT

	ld	a,(STOPFLG)
	cp	03h
	jr	z,CMTSTP
	ld	a,(CMTSTAT)
	bit	4,a
	ret	nz		;read error, z-flag=0
	and	02h
	jr	z,GETCMT
	xor	a		;set z-flag
	ld	(CMTSTAT),a
	ld	a,(CMTBUF)
	ret

CMTSTP:
	call	CHKVRF
	jp	STOPSP


;call GETCMT, if z=0 then ?TR Error
GETCMTTR:
	call	GETCMT
	ret	z
	call	CHKVRF
	jp	TRERR


;check verify or not
CHKVRF:
	call	RCLOSE
CHKVRF2:
	ld	a,(VERIFY)
	or	a
	call	z,NEW
	ret


;close for tape read
;destroy: none
_RCLOSE:ds	RCLOSE-_RCLOSE
	org	RCLOSE

	push	af
	push	bc
	ld	a,1ah
CLOSEEND:
	call	OUT90H
	call	RLOFF
	pop	bc
	pop	af
	ret


;open for tape write
;destroy: none
_WOPEN:	ds	WOPEN-_WOPEN
	org	WOPEN

	push	af
	push	bc
	call	RLON
	call	BLANK
	ld	b,3eh
	call	OPENCMT
	call	BLANK
	pop	bc
	pop	af
	ret


;put 1 character to CMT
;input: a=data
;destroy: none
_PUTCMT:ds	PUTCMT-_PUTCMT
	org	PUTCMT

	push	af
PUTCMT2:
	call	CHKWSTP
	ld	a,38h
	call	OUT90H
	pop	af
	jp	OUT90H


;wait and check stop for tape write
;destroy: af,bc
BLANK:
	call	WAIT
;	jr	CHKWSTP

;check stop for tape write
;destroy: af (when not stop)
CHKWSTP:
	ld	a,(STOPFLG)
	cp	03h
	ret	nz
	call	RLOFF
	jp	STOPSP


;check 8255 status (ready for output to port 90h)
;output: z(0=ready)
;destroy: af,(b)
_CHK90H:ds	CHK90H-_CHK90H
	org	CHK90H

	ld	a,08h
	out	(93h),a		;disable 8255 INT for reading
	in	a,(92h)
	and	88h		;nobf=1,intr=1?
	jp	pe,CHK90PE
	cp	a		;set z-flag
CHK90PE:
	ld	a,09h
	out	(93h),a		;enable 8255 INT for reading
	ret


;close for tape write
;destroy: none
_WCLOSE:ds	WCLOSE-_WCLOSE
	org	WCLOSE

	push	af
	push	bc
	ld	bc,03b0h
	call	WAITLP
	ld	a,3ah
	jr	CLOSEEND


;open CMT subroutine
;input: b (1eh=read, 3eh=write)
;destroy: af,b
OPENCMT:
	ld	a,(BAUD)	;00=600baud, others=1200baud
	cp	01h
	sbc	a,a		;ff=600baud, 00=1200baud
	add	a,b
	call	OUT90H		;read=1dh(600baud), 1eh(1200baud)
				;write=3dh(600baud), 3eh(1200baud)
	and	0f8h
	inc	a		;read=19h, write=39h
;	di
	call	OUT90H
;	ei
	xor	a
	ld	(CMTSTAT),a
;	ld	(STOPFLG),a
	ret


;blink asterisk
;destroy: af
_BLNKAST:ds	BLNKAST-_BLNKAST
	org	BLNKAST

	push	hl
	ld	hl,ASTSTAT
	ld	a,(hl)
	xor	'*'-' '
	ld	(hl),a

;	ld	hl,(WIDTH-1)	;h=(WIDTH)
;	dec	h
;	ld	l,01h

;for compatibility with N60-BASIC
	ld	hl,01h+(COLUMNS-1)*100h

	call	PRTCHXY
	pop	hl
	ret


;CMT relay on/off
;destroy: af,b
_RLOFF:	ds	RLOFF-_RLOFF
	org	RLOFF

	jr	RLOFF2

_RLON:	ds	RLON-_RLON
	org	RLON

	db	3eh		;ld a,
RLOFF2:
	xor	a		;afh

	ld	b,00001000b	;bit3=CMT relay
	jp	OUTB0H


;output to port b0h
;input: a=new data, b=change bit
;destroy: af
_OUTB0H:ds	OUTB0H-_OUTB0H
	org	OUTB0H

	push	hl
	ld	hl,PORTB0H
	xor	(hl)
	and	b
	xor	(hl)
	out	(0b0h),a
	ld	(hl),a
	pop	hl
	ret


;PLAY stop sub
;destroy: none
_PLSTPS:ds	PLSTPS-_PLSTPS
	org	PLSTPS

	push	af
	push	hl
	push	de
	push	bc

	ld	a,07h		;ch.ABC=tone,portAB=in
	ld	e,38h
	call	SETPSG
	inc	a		;register8=ch.A volume
	ld	e,00h
	call	SETPSG
	inc	a		;register9=ch.B volume
	call	SETPSG
	inc	a		;register10=ch.C volume
	call	SETPSG

	xor	a
	ld	(PLAYST),a
	ld	(CHANNEL),a

	ld	h,a
	ld	l,a
	ld	(BUFAIN),hl	;clear BFIN and BFOUT for ch.A
	ld	(BUFBIN),hl	;clear BFIN and BFOUT for ch.B
	ld	(BUFCIN),hl	;clear BFIN and BFOUT for ch.C

	ld	hl,PLWKA
	ld	(hl),a
	ld	d,h
	ld	e,l
	inc	de
	ld	bc,PLWKB-PLWKA-1
	ldir

	ld	hl,PLWKTBL
	ld	de,PLWKA+OCTAVE
	ld	c,05h		;b=0
	ldir

	ld	hl,PLWKA
	ld	de,PLWKB
	ld	c,PLWKC-PLWKA	;b=0
	ldir

	pop	bc
	pop	de
	pop	hl
	pop	af
	ret


;stop and initialize for PLAY command
;destroy: none
_PLSTOP:ds	PLSTOP-_PLSTOP
	org	PLSTOP
	di
	call	PLSTPS
	ei
	ret


PLWKTBL:
	db	4		;O-value
	db	4		;L-value
	db	120		;T-value
	db	8		;V-value
	db	255		;M-value(low)


;get PSG resister value and set new value
;input: a=register, e=value
;output: d=old value
;destroy: none
_SETPSG2:ds	SETPSG2-_SETPSG2
	org	SETPSG2

	push	af
	out	(0a0h),a
	in	a,(0a2h)
	ld	d,a
	pop	af
;	jp	SETPSG


;set PSG register
;input: a=register, e=value
;destroy: none
_SETPSG:ds	SETPSG-_SETPSG
	org	SETPSG

	push	af
	out	(0a0h),a
	ld	a,e
	out	(0a1h),a
	pop	af
	ret


;bell
;destroy: af,bc,e
_BELL:	ds	BELL-_BELL
	org	BELL

	call	PLSTOP
;tune
	xor	a		;register0=ch.A 8bit fine tune
	ld	e,55h
	call	SETPSG
	ld	e,a		;a=0
	inc	a		;register1=ch.A 4bit coarse tune
	call	SETPSG
;volume
	ld	e,07h
;	ld	a,08h		;register8=ch.A volume
	add	a,e		;register8=ch.A volume
	call	SETPSG
	ld	bc,0400h
	ld	e,c		;c=0
	call	WAITLP
;volume
	ld	a,08h		;register8=ch.A volume
	jr	SETPSG


;play subroutine called by time interrupt
;destroy: af,bc,de,hl
PLAYINT:
	ld	a,(PLAYST)
	or	a
	ret	z
PLIZ:
	rrca
	rrca
	rrca
	ld	c,a
	ld	hl,PLWKC
	ld	b,03h
PLILP:
	push	hl
	rlc	c
	jr	nc,PLINC

	ld	d,(hl)
	inc	l		;inc	hl
	ld	e,(hl)
	dec	de
	ld	(hl),e
	dec	l		;dec	hl
	ld	(hl),d
	ld	a,d
	or	e
	jr	nz,PLINEXT
	jr	PLIGET

PLINC:
	ld	a,(PLAYST)
	or	a
	jr	nz,PLINEXT	;if (PLAYST)<>0

PLIGET:
;length
	call	GETPLBF
;	jr	z,PLINEXT
	jr	z,PLIRES
	cp	0ffh
	jr	z,PLIRES
	pop	de
	push	de
	ld	(de),a
	call	GETPLBF		;no change in de
	inc	e		;inc de
	ld	(de),a

;tune
	call	GETPLBF
	ld	e,a
	ld	a,b
	add	a,a
	dec	a		;register1,3,5=4bit coarse tune
	call	SETPSG
	call	GETPLBF
	ld	e,a
	ld	a,b
	dec	a
	add	a,a		;register0,2,4=8bit fine tune
	call	SETPSG

;volume or envelope
	call	GETPLBF
	ld	d,a		;
	cp	10h
	jr	c,PLIVOL
	ld	a,10h		;envelope
PLIVOL:
	ld	e,a
	ld	a,b
	add	a,07h		;register8,9,10=volume
	call	SETPSG
	ld	a,d		;
	sub	10h
	jr	c,PLISET

;period
	ld	d,a		;
	call	GETPLBF
	ld	e,a
	ld	a,0bh
	call	SETPSG
	call	GETPLBF
	ld	e,a
	ld	a,0ch
	call	SETPSG

;envelope pattern
	ld	e,d		;
	inc	a		;register13=envelope pattern
	call	SETPSG

;status
PLISET:
	set	0,c

PLINEXT:
	pop	hl
	ld	a,l
	add	a,PLWKA-PLWKB	;no carry
	ld	l,a
	djnz	PLILP

	ld	hl,PLAYST
	ld	a,(hl)
	ld	(hl),c
	or	a
	ret	z		;old (PLAYST)=0
	ld	a,c
	or	a
	ret	nz		;new (PLAYST)<>0
	jp	PLIZ		;old<>0 and new=0

;reset
PLIRES:
	res	0,c
	ld	a,b
	add	a,07h		;register8,9,10=volume
	ld	e,00h
	call	SETPSG
	jr	PLINEXT


;get play buffer address
;output: hl
;destroy: f
GETPLAD:
	ld	h,a
	ld	a,(CHANNEL)
	ld	l,a
	add	a,a		;a*2
	add	a,l		;a*3
	add	a,a		;a*6
	add	a,BUFAIN-BUFAIN/256*256		;no carry
	ld	l,a
	ld	a,h
	ld	h,BUFAIN/256	;higher byte
	ret


;joystick
;input: a=1or2
;output: a (0-0-trigger2-trigger1-right-left-down-up)
;destroy: f
_JOYSTK:ds	JOYSTK-_JOYSTK
	org	JOYSTK

	push	de
	rrca
	scf
	rra
	ld	e,a		;c0h or 80h

	ld	a,07h		;register7=portABin-out,noise,tone
	di
	out	(0a0h),a
	in	a,(0a2h)
	jr	SKPPATCH2


;1cb4h-1cb5h: to be patched by PC6001V/VX/VW
_PATCH2:ds	PATCH2-_PATCH2
	org	PATCH2
	db	00h, 00h


SKPPATCH2:
	ld	d,a		;

	and	10111111b	;parallel port A=in
	or	10000000b	;parallel port B=out
	out	(0a1h),a

	ld	a,0fh		;register15=parallel port B
	call	SETPSG		;c0h or 80h

	dec	a		;register14=parallel port A
	out	(0a0h),a
	in	a,(0a2h)
	cpl
	ld	e,d		;
	ld	d,a		;;

	ld	a,07h
	call	SETPSGEI

	ld	a,d		;;
	pop	de
	ret


;LOCATE command
_C_LOCA:ds	C_LOCA-_C_LOCA
	org	C_LOCA

	call	INT1ARG
	ld	de,(WIDTH)
	cp	e
	jp	nc,FCERR
	inc	a
	push	af		;
	ld	a,(hl)
	cp	','
	jp	nz,SNERR
	call	INT1INC

	ld	hl,(HEIGHT)	;l=(HEIGHT)
	cp	l
	jp	nc,FCERR
	inc	a

	pop	hl		;
	ld	l,a
	jp	SETCSR


;CONSOLE command
_C_CNSL:ds	C_CNSL-_C_CNSL
	org	C_CNSL

	ld	bc,(CONSOL1)	;c=(CONSOL1),b=(CONSOL2)
	push	bc

	call	CHKCMM
	jr	z,CNSLPAR2

	call	INT1ARG
	ld	a,(HEIGHT)
	dec	a
	cp	e
	jr	c,CNSLC
	ld	a,e
CNSLC:
	inc	a
	pop	bc
	ld	c,a
	push	bc

	call	CHKCLCM
	jr	z,CNSLEND

CNSLPAR2:
	call	CHKCMM
	jr	z,CNSLPAR3

	call	INT1ARG

	pop	bc
	add	a,c
	jp	c,FCERR
	dec	a
	ld	b,a
	push	bc

	call	CHKCLCM
	jr	z,CNSLEND

CNSLPAR3:
	call	CHKCMM
	jr	z,CNSLPAR4

	call	INT1ARG
	cp	02h
	jp	nc,FCERR
	ld	(CONSOL3),a

	call	PRTFKEY2
	call	CHKCLCM
	jr	z,CNSLEND

CNSLPAR4:
	call	INT1ARG
	cp	02h
	jp	nc,FCERR
	ld	(CONSOL4),a

CNSLEND:
	pop	de
	ld	a,d
	cp	e
	jp	c,FCERR
;	jr	CNSMAIN


;CONSOLE command main part
;used by EXAS BASIC compiler
;destroy: af,de,hl
_CNSMAIN:ds	CNSMAIN-_CNSMAIN
	org	CNSMAIN

	call	SETCNSL

	ld	a,(CONSOL1)
	call	CUTLINE2

	ld	a,(CSRY)
	dec	a
	ld	hl,LASTLIN
	cp	(hl)
	ret	c
	ld	l,(hl)
	jp	CTLCR


;set console parameter
;destroy: af,de,hl
_SETCNSL2:ds	SETCNSL-4-_SETCNSL2
	org	SETCNSL-4
SETCNSL2:
	ld	de,(CONSOL1)

;set console parameter
;input: d=last line+1, e=first line+1
;destroy: af,de,hl
_SETCNSL:ds	SETCNSL-_SETCNSL
	org	SETCNSL
;2nd parameter
	ld	a,(CONSOL3)
	ld	h,a
	ld	a,(HEIGHT)
	cp	d
	jr	nc,CNSLNC1
	ld	d,a
CNSLNC1:

;1st parameter
	sub	h
	cp	e
	jr	nc,CNSLNC2
	ld	e,a
CNSLNC2:
	ld	l,e

;last line
	ld	h,a		;h=(HEIGHT)-(CONSOL3)
	cp	d
	ccf
	ld	a,(SCREEN1)
	rra
	or	a
	jr	z,CNSLZ
	ld	h,d		;if screen mode 3,4 or 2nd<height-3rd
CNSLZ:

	ld	(CONSOL1),de
	ld	(FSTLIN),hl
	ret


;COLOR command
_C_COLR:ds	C_COLR-_C_COLR
	org	C_COLR

	ld	de,COLOR1
	ld	b,02h
COLRLP:
	call	CHKCMM
	jr	z,COLRZ
	push	bc
	push	de
	call	INT1ARG
	pop	de
	pop	bc
	ld	(de),a
	call	CHKCLN		;a=(hl)
	ret	z
	cp	','
	ret	nz
	inc	hl
COLRZ:
	inc	de
	djnz	COLRLP

COLRPAR3:
	call	INT1ARG
;	jp	SETC3


;set color 3rd parameter
;input: a=3rd color parameter (1 or 2)
;destroy: af,bc,de
_SETC3:	ds	SETC3-_SETC3
	org	SETC3

	ex	de,hl
	dec	a
	cp	02h
	jp	nc,FCERR
	add	a,a		;0 or 2

	ld	hl,COLOR3
	cp	(hl)
	ret	z
	ld	(hl),a

	ld	hl,(VRAM-1)
	ld	bc,0002h	;32*16=256*2
	ld	l,b		;b=0
SETC3LP:
	ld	a,(hl)
	xor	02h
	ld	(hl),a
	inc	hl
	djnz	SETC3LP
	dec	c
	jr	nz,SETC3LP
	ex	de,hl
	ret


;clear screen and reset graphic cordinate
;input: c-flag=1: c=first line+1, a=last line-first line+1
;       c-flag=0: using console parameters
;destroy: af,bc,de,hl
CLSMAIN:
	ld	hl,0000h
	ld	(GRPX1),hl
	ld	(GRPY1),hl
	inc	l
	call	CTLCR		;(0,0)
	jp	c,CLSMAIN2	;if a,c given
	jp	CTLL


;CLS command
;destroy: af,de
_CLS:	ds	CLS-_CLS
C_CLS:
	org	CLS

	push	hl
	push	bc
	or	a		;reset c-flag
	call	CLSMAIN
	pop	bc
	pop	hl
	ret


;SCREEN command
_C_SCRN:ds	C_SCRN-_C_SCRN
	org	C_SCRN

	xor	a		;1st parameter=none
	push	af
	ld	bc,(SCREEN2)
	ld	d,c		;2nd parameter-1
	ld	e,b		;3rd parameter-1

	call	CHKCMM
	jr	z,SCRPAR2

	push	de
	call	INT1ARG
	pop	de
	pop	bc
	push	af		;1st parameter

	call	CHKCLCM
	jr	z,GOSCRMAIN

SCRPAR2:
	call	CHKCMM
	jr	z,SCRPAR3

	push	de
	call	INT1ARG
	pop	de
	dec	a
	ld	d,a		;2nd parameter-1

	call	CHKCLCM
	jr	z,GOSCRMAIN

SCRPAR3:
	push	de
	call	INT1ARG
	pop	de
	dec	a
	ld	e,a		;3rd parameter-1

GOSCRMAIN:
	pop	af
	dec	a
	ld	c,a		;1st parameter-1
;	jr	SCRMAIN


;SCREEN command main part
;used by EXAS BASIC compiler
;input: c=1st paraeter-1(ff=none), d=2nd parameter-1, e=3rd parameter-1
;destroy: af,de
_SCRMAIN:ds	SCRMAIN-_SCRMAIN
	org	SCRMAIN

	ld	a,(PAGES)
	dec	a
	cp	d
	jp	c,FCERR
	cp	e
	jp	c,FCERR

	ld	a,c
	inc	a
	cp	03h
	jr	c,SCROK
	cp	05h
	jp	nc,FCERR
	ld	a,d
	or	a
	jp	z,FCERR

SCROK:
	ld	a,d
	call	CHGACT

	ld	a,(SCREEN3)
	cp	e
	ld	a,e
	call	nz,CHGDSP

	ld	a,(SCREEN1)
	cp	c
	ret	z
	ld	a,c
	inc	c
	ret	z		;1st parameter=none
	jp	CHGMOD


;TIME function
F_TIME:
	ld	hl,(TMCNT)
	ld	de,(TMCNT+2)
	jp	I4TOF


;SOUND command
C_SOUN:
	call	CHKCLN
	jp	z,SNERR
	call	INT1ARG
	cp	10h
	jp	nc,FCERR
	push	af
	ld	a,(hl)
	cp	','
	jp	nz,SNERR
	call	INT1INC
	pop	af
	di
SETPSGEI:
	call	SETPSG
	ei
	ret


;PLAY command
C_PLAY:
	call	PLAY
	ld	(PROGAD),hl
	ret


;play routine
;input: hl=data address, (z=0)
;output: hl=next address
;destroy: af,bc,de
_PLAY:	ds	PLAY-_PLAY
	org	PLAY

	push	ix
	ld	de,(PROGAD)
	push	de

	xor	a
	ld	(PLWKB+PLLEN),a
	ld	(PLWKC+PLLEN),a

	ld	ix,PLWKA
	ld	b,03h
PLAYLP1:
	push	bc
	call	STRARG
	ld	(ix+PLLEN),a
	ld	(ix+PLADR),e
	ld	(ix+PLADR+1),d
	ld	bc,PLWKB-PLWKA
	add	ix,bc
	pop	bc
	call	CHKCLCM
	jr	z,PLSTR0
	djnz	PLAYLP1

;check string length=0?
PLSTR0:
	ld	hl,PLWKC+PLLEN
	ld	de,PLWKB-PLWKC
	ld	b,03h
PLAYLP2:
	ld	a,(hl)
	or	a
	jr	nz,PLAYNZ1
	ld	a,b
	dec	a
	ld	(CHANNEL),a
	ld	a,0ffh
	push	hl
	di
	call	PUTPLBF
	ei
	pop	hl
PLAYNZ1:
	add	hl,de
	djnz	PLAYLP2

PLAYLP3:
	ld	a,(STOPFLG)
	cp	03h
	call	z,PLSTOP
	jr	z,PLAYEND

	di
	ld	ix,PLWKC
	ld	bc,0300h	;c=PLAYLEN check counter
PLAYLP4:
	ld	a,b
	dec	a
	ld	(CHANNEL),a
	ld	a,(ix+PLLEN)
	or	a
	jr	z,PLAYNEXT

	ld	l,(ix+PLADR)
	ld	h,(ix+PLADR+1)

	ld	(PLAYLEN),a
	ld	(PLAYSTR),hl
	push	bc
	call	CNVPLAY
	pop	bc
	ld	a,(PLAYLEN)
	ld	(ix+PLLEN),a
	ld	hl,(PLAYSTR)
	ld	(ix+PLADR),l
	ld	(ix+PLADR+1),h

	inc	c
	or	a
	jr	nz,PLAYNEXT
	dec	c
	dec	a
	call	PUTPLBF		;a=ffh
PLAYNEXT:
	ld	de,PLWKB-PLWKC
	add	ix,de
	djnz	PLAYLP4

	ld	hl,PLAYST
	ld	a,(hl)
	or	a
	jr	nz,PLAYNZ2

	inc	a		;=01h
	ld	(PLWKA+REMAIN+1),a
	ld	(PLWKB+REMAIN+1),a
	ld	(PLWKC+REMAIN+1),a
	ld	a,07h
	ld	(hl),a

PLAYNZ2:
	ei
	ld	a,c		;c=PLAYLEN check counter
	or	a
	jr	nz,PLAYLP3

PLAYEND:
	pop	de
	ld	hl,(PROGAD)
	ld	(PROGAD),de
	pop	ix
	ret


;convert play data
;input: a=string length, hl=string address, ix=play work
;destroy: af,bc,de,hl
CNVPLAY:
	call	GETPLAD
	ld	d,(hl)
	inc	d		;in+1
	inc	hl
	ld	a,(hl)		;out
	sub	d
	jr	nc,CNVPLNC

	inc	hl
	inc	hl		;size
	inc	a
	add	a,(hl)
CNVPLNC:
	cp	08h
	ret	c

	call	PLAYGETC
	ret	z
	call	PLAYINC
	call	TOUPPER
	call	PLAYCMD
	jr	CNVPLAY


PLAYCMD:
	sub	'A'
	cp	'G'-'A'+1
	jp	c,TONE
	cp	'M'-'A'
	jr	z,PLAYM
	cp	'S'-'A'
	jr	z,PLAYS
	cp	'L'-'A'
	jr	z,PLAYL
	cp	'T'-'A'
	jr	z,PLAYT
	cp	'O'-'A'
	jr	z,PLAYO
	cp	'R'-'A'
	jp	z,PLAYR
	cp	'N'-'A'
	jp	z,PLAYN
	cp	'V'-'A'
	jp	nz,FCERR


PLAYV:
	call	GETNUM1
	jr	c,PLAYVNUM
	ld	a,08h
PLAYVNUM:
	cp	10h
	jp	nc,FCERR
PUTV:
	ld	(ix+VOLUME),a
	ret

PLAYM:
	call	GETNUM2
	jr	c,PLAYMNUM
	ld	de,00ffh
PLAYMNUM:
	ld	a,d
	or	e
	jp	z,FCERR
	ld	(ix+PERIOD),e
	ld	(ix+PERIOD+1),d
	ret

PLAYS:
	call	GETNUM1
	cp	10h
	jp	nc,FCERR
	add	a,10h
	jr	PUTV

PLAYL:
	call	GETNUM1
	jr	c,PLAYLNUM
	ld	a,04h
PLAYLNUM:
	dec	a
	cp	40h
	jp	nc,FCERR
	inc	a
	ld	(ix+LENGTH),a
	ret

PLAYT:
	call	GETNUM1
	jr	c,PLAYTNUM
	ld	a,120
PLAYTNUM:
	cp	32
	jp	c,FCERR
	ld	(ix+TEMPO),a
	ret

PLAYO:
	call	GETNUM1
	jr	c,PLAYONUM
	ld	a,04h
PLAYONUM:
	dec	a
	cp	08h
	jp	nc,FCERR
	inc	a
	ld	(ix+OCTAVE),a
	ret


;get a character from PLAY string
;output: a=data, hl=address, z-flag(1=no character)
;destroy: f,hl
PLAYGETC:
	ld	a,(PLAYLEN)
	or	a
	ret	z		;a=0 if no character
	ld	hl,(PLAYSTR)
	ld	a,(hl)
	cp	' '
	ret	nz
	call	PLAYINC
	jr	PLAYGETC


;scale data
;address probably equals to 6001mkII/6601 ROM (used by BELUGA)
_SCALE2:ds	SCALE2-_SCALE2
	org	SCALE2

;		c/b+	c+/d-	d	d+/e-	e/f-	f/e+
	dw	0ee8h,	0e12h,	0d48h,	0c89h,	0bd5h,	0b2bh
;		f+/g-	g	g+/a-	a	a+/b-	b/c-
	dw	0a8ah,	09f3h,	0964h,	08ddh,	085eh,	07e6h


;scale data
_SCALE:	ds	SCALE-_SCALE
	org	SCALE

;		c/b+	c+/d-	d	d+/e-	e/f-	f/e+
	dw	0ee8h,	0e12h,	0d48h,	0c89h,	0bd5h,	0b2bh
;		f+/g-	g	g+/a-	a	a+/b-	b/c-
	dw	0a8ah,	09f3h,	0964h,	08ddh,	085eh,	07e6h


TONETBL:
;		a   b   c  d  e  f   g
	db	18, 22, 0, 4, 8, 10, 14


;increment (PLAYSTR) and decrement (PLAYLEN)
;destroy: f,hl
PLAYINC:
	ld	hl,PLAYLEN
	dec	(hl)
	ld	hl,PLAYSTR
	inc	(hl)
	ret	nz
	inc	l		;inc hl
	inc	(hl)
	ret


PLAYN:
	call	GETNUM1
	jp	nc,FCERR
	cp	97
	jp	nc,FCERR
	or	a
	jr	z,PLAYN0

	ld	b,00h
PLAYNLP:
	inc	b
	sub	0ch
	jr	nc,PLAYNLP
	add	a,a
	ld	d,0ffh
	ld	e,a
	ld	hl,SCALE+24
	add	hl,de
	jr	GETFREQ

PLAYR:
	xor	a
	push	af		;volume
	push	de		;(tune)
	call	GETNUM1
	jr	nc,PLAYRNC
	or	a
	jr	nz,CHKLEN
	jp	FCERR

PLAYRNC:
	ld	a,04h
	jr	CALCLEN

;accidental mark
PLAYPLS:
	call	PLAYINC
	inc	e
	inc	e
	ld	a,e
	sub	24
	jr	c,NOACC
	ld	e,a		;=0
	jr	NOACC

PLAYMNS:
	call	PLAYINC
	dec	e
	dec	e
	jp	p,NOACC
	ld	e,22
	jr	NOACC

PLAYN0:
;	xor	a
	push	af		;volume=0
	push	de		;(tune)

;for compatibility with PC-6001
	ld	a,80
	jr	CALCLEN


TONE:
	ld	hl,TONETBL
	ld	d,00h
	ld	e,a
	add	hl,de
	ld	e,(hl)
	call	PLAYGETC
	jr	z,NOACC
	cp	'+'
	jr	z,PLAYPLS
	cp	'#'
	jr	z,PLAYPLS
	cp	'-'
	jr	z,PLAYMNS
	cp	'='
	jp	z,FCERR

NOACC:
	ld	b,(ix+OCTAVE)
	ld	hl,SCALE
;	ld	d,00h
	add	hl,de		;d=0
GETFREQ:
	ld	a,(hl)
	inc	hl
	ld	d,(hl)
	dec	b
	jr	z,OCTAVE1
OCTVLP:
	srl	d
	rra
	djnz	OCTVLP
OCTAVE1:
	ld	e,a
	ld	a,(ix+VOLUME)

PUSHVOL:
	push	af		;volume
	push	de		;tune
	call	GETNUM1
	or	a
	jr	nz,CHKLEN
	ld	a,(ix+LENGTH)
CHKLEN:
	cp	65
	jp	nc,FCERR

CALCLEN:
;60[s/min]/T[/min]/(L/4)/(8192/3993600[Hz])=117000/T/L
	ld	b,(ix+TEMPO)	;T-value
	call	MULINT1		;hl=T*L (<=3fc0h)
	ld	de,0e484h	;117000/2
	call	DIVINT2		;bc=(117000/2)/(T*L)*2
	ld	d,b
	ld	e,c

;dotted note
DOTLP:
	call	PLAYGETC
	cp	'.'
	jr	nz,NOTDOT
	call	PLAYINC
	srl	b
	rr	c
	ex	de,hl
	add	hl,bc
	ex	de,hl
	jr	DOTLP
NOTDOT:
;	di
;for compatibility with PC-6001
	ld	hl,002dh
	rst	CPHLDE
	jr	c,LENOK
	ex	de,hl
LENOK:
	ld	a,d		;length
	call	PUTPLBF
	ld	a,e
	call	PUTPLBF
	pop	de		;tune
	ld	a,d
	call	PUTPLBF
	ld	a,e
	call	PUTPLBF
	pop	af		;volume
	call	PUTPLBF
	cp	10h
	jr	c,PLCMDEND
	ld	a,(ix+PERIOD)	;period
	call	PUTPLBF
	ld	a,(ix+PERIOD+1)
	call	PUTPLBF
PLCMDEND:
;	ei
	ret


GETNUM1:
;output: a,c-flag(1=number)
;destroy: f,bc,de,hl
	call	GETNUM2
	ld	a,e
	ret	nc
	inc	d
	dec	d
	ret	z		;c-flag=1
	jp	FCERR


GETNUM2:
;output: de,c-flag(1=number)
;destroy: af,bc,de
	call	PLAYGETC	;hl=address if z-flag=0
	ld	de,0000h
	ret	z		;c-flag=0
	cp	'='
	jr	z,GETNUMEQ
	cp	';'
	jr	z,GETNUMSEMI
	sub	'0'
	cp	'9'-'0'+1
	ret	nc		;c-flag=0
	ld	a,(PLAYLEN)
	ld	b,a
	call	ATOI2LEN
	ld	a,b
	ld	(PLAYLEN),a
	ld	(PLAYSTR),hl
	call	PLAYGETC
	jr	z,GETNUMZ
	cp	';'
	call	z,PLAYINC
GETNUMZ:
	scf			;set c-flag
	ret

GETNUMSEMI:
	call	PLAYINC
	xor	a		;reset c-flag
	ld	d,a
	ld	e,a
	ret

GETNUMEQ:
	call	PLAYINC
	ld	de,INPBUF
GETNUMEQLP:
	call	PLAYGETC
	jp	z,FCERR
	call	PLAYINC
	call	TOUPPER2
	ld	(de),a
	inc	de
	cp	';'
	jr	nz,GETNUMEQLP

	ld	hl,INPBUF
	ld	b,(hl)
	ld	a,b
	sub	'A'
	cp	'Z'-'A'+1
	jp	nc,FCERR
	inc	hl
	call	GETNAME
	cp	'$'
	jp	z,TMERR
	cp	'('
	jr	z,GETNUMARR

	call	SRCHVAR
	jr	nc,GETNUMEQNC
	ld	de,ZERO
GETNUMEQNC:
	ex	de,hl
	call	SETF1
	call	FTOI
	scf
	ret


GETNUMARR:
	ld	de,(PROGAD)
	push	de
	call	GETARR
	pop	hl
	ld	(PROGAD),hl
	jr	GETNUMEQNC


;hl=a*b
;input: a,b
;output: hl
;destroy: af,c
MULINT1:
	ld	hl,0000h
	ld	c,l		;=0
	jr	MULI1NZ
MULI1C:
	add	hl,bc
MULI1NZ:
	srl	b
	rr	c
	add	a,a
	jr	c,MULI1C
	jr	nz,MULI1NZ
	ret


;dehl=de*hl
;input: de,hl
;output: dehl
;destroy: af,bc
MULINT2:
	ld	a,h
	ld	c,l
	ld	hl,0000h
	ld	b,10h
MULI2LP:
	rra
	rr	c
	jr	nc,MULI2NC
	add	hl,de
MULI2NC:
	rr	h
	rr	l
	djnz	MULI2LP
	rra
	rr	c
	ld	e,c
	ld	d,a
	ex	de,hl
	ret


;bc=de/hl*2
;input: de,hl(<8000h)
;output: bc...hl
;destroy: af,de,hl
DIVINT2:
	xor	a
	ld	bc,0200h	;*2^(b-1)
DIVILP1:
	inc	b
	adc	hl,hl		;c-flag=0
	jp	p,DIVILP1
	ex	de,hl
DIVILP2:
	or	a
	sbc	hl,de
	jr	nc,DIVNC
	add	hl,de
DIVNC:
	ccf
	rl	c
	rla
	srl	d
	rr	e
	djnz	DIVILP2

	ld	b,a
	ret


;STICK() function
_F_STCK:ds	F_STCK-_F_STCK
	org	F_STCK

	call	FNCI1

;2239: used by SNAKE & ALIEN, MAD CHASER
	or	a
	jr	z,STCK0
	call	JOYSTK
	ld	hl,JOYTBL
	jr	CNVSTCK

STCK0:
	call	STICK
	rrca
	rrca
	ld	hl,CSRTBL
CNVSTCK:
	and	0fh
	ld	d,00h
	ld	e,a
	add	hl,de
	ld	l,(hl)
	jp	I1TOF


;STRIG() function
_F_STRG:ds	F_STRG-_F_STRG
	org	F_STRG

	call	FNCI1

;2259: used by SNAKE & ALIEN, MAD CHASER
	or	a
	jr	z,STRG0
	call	JOYSTK
	and	10h
STRGEND:
	jp	z,SETZERO
;	jp	SETPLS1

SETPLS1:
	ld	hl,PLSONE
	jp	SETF1

STRG0:
	call	STICK
	and	80h
	jr	STRGEND


;get a 1-byte integer for function for stick()/strig()
;input: hl=program address-1
;output: FAC1,a, hl=next address
;destroy: FAC2,f,bc
FNCI1:
	inc	hl
	call	FNCNUM
	call	FTOI1
	add	a,e
	cp	03h
	ret	c
	jp	FCERR


;joystick -> stick()
_JOYTBL:ds	JOYTBL-_JOYTBL
	org	JOYTBL
;		    u   d   ud  l   ul  dl  udl
	db	0,  1,  5,  0,  7,  8,  6,  7,
;		r   ur  dr  udr lr  ulr dlr udlr
	db	3,  2,  4,  3,  0,  1,  5,  0


;cursor key -> stick()
_CSRTBL:ds	CSRTBL-_CSRTBL
	org	CSRTBL
;		    u   d   ud  r   ur  dr  udr
	db	0,  1,  5,  0,  3,  2,  4,  3
;		l   ul  dl  udl rl url drl udrl
	db	7,  8,  6,  7,  0,  1,  5,  0


;LCOPY command
_C_LCPY:ds	C_LCPY-_C_LCPY
	org	C_LCPY

	call	HOOKLCP
	ld	hl,LCPYHD
	ld	b,05h
LCPYLP1:
	ld	a,(hl)
	call	PRINTER
	inc	hl
	djnz	LCPYLP1

	ld	hl,(VRAM-1)	;h=(VRAM)
	inc	h
	inc	h
	ld	l,b		;b=0
	ld	a,(SCREEN1)
	cp	02h
	jr	c,LCPYTXT

LCPYGRP:
	ld	de,1800h
LCPYLP2:
	ld	c,(hl)
	call	SENDGRP
	inc	hl
	dec	de
	ld	a,d
	or	e
	jr	nz,LCPYLP2

LCPYEND:
	ld	bc,2000h
	call	LCPYLP3

	ld	bc,060ah
LCPYLP3:
	ld	a,c
	call	PRINTER
	djnz	LCPYLP3
	ret

;header for lcopy
LCPYHD:
	db	20h, 20h, 0ah, 1dh, 0c1h

;lcopy text screen
LCPYTXT:
	ld	a,04h
	out	(93h),a		;CGROM ON
	ld	a,(HEIGHT)
	ld	b,a
LCPYTLP1:
	push	bc
	ld	c,0ch
LCPYTLP2:
	ld	b,COLUMNS
LCPYTLP3:
	push	bc
	ld	a,(hl)
	call	CGROM
	ld	a,e
	add	a,0ch
	sub	c
	ld	e,a
	ld	a,(de)

	ld	d,a		;
	dec	h
	dec	h
	ld	a,(hl)		;attribute
	inc	h
	inc	h
	bit	6,a
	jr	nz,LCPYSEMI
	rrca
	ld	a,d		;
	jr	nc,NOTREV
	cpl
NOTREV:
	ld	c,a
	call	SENDGRP
	inc	hl
	pop	bc
	djnz	LCPYTLP3

	dec	c
	jr	z,LCPYTZ
	ld	de,0-COLUMNS
	add	hl,de
	jr	LCPYTLP2

LCPYTZ:
	pop	bc
	djnz	LCPYTLP1

	ld	a,05h
	out	(93h),a		;CGROM OFF
	jr	LCPYEND

LCPYSEMI:
;c=	9-12	xx**....
;	5-8	xx..**..
;	1-4	xx....**

	ld	a,(hl)
	dec	c
	bit	2,c
	jr	nz,SEMI58	;c=5-8
	bit	3,c
	jr	z,SEMI14	;c=1-4

	rrca
	rrca
SEMI58:
	rrca
	rrca
SEMI14:
	and	03h

;00h->00h, 01h->0fh, 02h->f0h, 03h->ffh
	rra			;c-flag=0
	jr	nc,SEMINC
	add	a,0fh*2
SEMINC:
	rra			;c-flag=0
	jr	nc,NOTREV
	add	a,0f0h
	jr	NOTREV


;KEY command
C_KEY:
	call	INT1ARG
	dec	a
	cp	0ah
	jp	nc,FCERR
	ld	de,FKEYTBL
	add	a,a		;*2
	add	a,a		;*4
	add	a,a		;*8
	add	a,e		;no carry
	ld	e,a
	ld	a,(hl)
	cp	','
	jp	nz,SNERR
	push	de		;
	inc	hl
	call	STRARG
	ex	de,hl
	pop	de		;

	ld	bc,08ffh
	inc	a
KEYLP1:
	dec	a
	jr	z,KEYLP2
	ldi
	djnz	KEYLP1
	jr	KEYEND

KEYLP2:
	ld	(de),a		;a=0
	inc	de
	djnz	KEYLP2
KEYEND:
	jp	PRTFKEY


;CSAVE command
C_CSV:
	call	STRARG
	or	a
	jp	z,FCERR
	ld	c,a

;write header (d3h*10)
	call	WOPEN
	ld	a,0d3h
	ld	b,0ah
CSVLP1:
	call	PUTCMT
	djnz	CSVLP1

;file name
	ld	b,06h

	inc	c
CSVLP2:
	ld	a,(de)
	dec	c
	jr	nz,CSVNZ
	xor	a
	inc	c
CSVNZ:
	call	PUTCMT
	inc	de
	djnz	CSVLP2

;data
CSVDATA:
	ld	bc,24b0h
	call	WAITLP

	ld	hl,(STARTAD)
	ld	de,(VARAD)
CSVLP3:
	ld	a,(hl)
	call	PUTCMT
	inc	hl
	rst	CPHLDE
	jr	nz,CSVLP3

;footer (00h*9)
	ld	b,09h
CSVLP4:
	call	PUTCMT		;a=0
	djnz	CSVLP4

	jp	WCLOSE


;CLOAD command
C_CLD:
	pop	af		;cancel return address
	call	SKIPSP
	cp	PRINT_
	ld	c,00h		;file name length
	ld	a,c
	jr	nz,CLDNZ1
	inc	hl
	cpl
CLDNZ1:
	ld	(VERIFY),a
	push	af		;verify?
	call	CHKCLN
	jr	z,NOTARGET
	call	STRARG
	or	a
	jp	z,FCERR
	ld	c,a
NOTARGET:
	call	ROPEN

CLDLP1:
	ex	de,hl
	call	GETFN
	ex	de,hl

;compare file name
	ld	a,c
	or	a
	jr	z,CLDFND

	push	de		;target

	ld	hl,FNAME
	inc	c
	ld	b,06h
CLDLP2:
	ld	a,(de)
	dec	c
	jr	nz,CLDNZ2
	xor	a
	inc	c
CLDNZ2:
	cp	(hl)
	jr	nz,CLDSKIP
	inc	hl
	inc	de
	djnz	CLDLP2
	pop	de		;target

CLDFND:
	ld	hl,FOUND
	call	PUTFN
	call	CHKVRF2

	pop	af		;verify?
	ld	c,a		;c=0: not verify

	ld	hl,(STARTAD)
CLDLP3:
	ld	b,0ah
CLDLP4:
	ex	de,hl
	ld	hl,0-0002h
	add	hl,sp
	rst	CPHLDE
	jr	z,CLDOM		;over memory error
	ex	de,hl
	call	GETCMTTR
	inc	c
	dec	c
	jr	z,CLDZ2		;c=0: not verify
	cp	(hl)
	jr	nz,CLDBAD
CLDZ2:
	ld	(hl),a
	inc	hl
	or	a
	jr	nz,CLDLP3
	call	BLNKAST
	djnz	CLDLP4
	call	RCLOSE

	ld	(VARAD),hl

	xor	a
	ld	(DEVICE),a
	ld	hl,OK
	call	PUTS
	jp	CHGLKP

CLDSKIP:
	ld	hl,SKIP
	call	PUTFN

;check footer (00h*10)
	ld	de,0a00h
	call	CMTSKP
	pop	de		;target
	jr	CLDLP1

CLDOM:
	call	RCLOSE
	call	NEW
	jp	OMERR

CLDBAD:
	call	RCLOSE
	ld	hl,BAD
	jp	PUTSEDIT

BAD:
	db	"Bad", 00h


;add 4-byte integer and 4byte integer
;input: FAC1(integer),FAC2(integer)
;output: FAC1(integer),f
;destroy: de,hl
ADDINT4:
	ld	hl,(FAC1)
	ld	de,(FAC2)
	add	hl,de
	ld	(FAC1),hl
	ld	hl,(FAC1+2)
	ld	de,(FAC2+2)
	adc	hl,de
	ld	(FAC1+2),hl
	ret


;subtract 4-byte integer from 4byte integer
;input: FAC1(integer),FAC2(integer)
;output: FAC1(integer),f
;destroy: de,hl
SUBINT4:
	ld	hl,(FAC1)
	ld	de,(FAC2)
	or	a
	sbc	hl,de
	ld	(FAC1),hl
	ld	hl,(FAC1+2)
	ld	de,(FAC2+2)
	sbc	hl,de
	ld	(FAC1+2),hl
	ret


;a=FAC1(int4)/(hl)<=9, FAC1%=(hl), hl+=4
;input: FAC1(integer), hl=division factor (4byte int) address
;output: a=FAC1/(hl4), FAC1=FAC1%(hl4), hl=hl+4, a=[0,9]
;destroy: f,FAC2
DIVINT4:
	push	bc
	push	de
	call	SETF2
	dec	hl
	push	hl
	xor	a
DIVINT4LP:
	inc	a
	call	SUBINT4
	jr	nc,DIVINT4LP
	dec	a
	call	ADDINT4
	pop	hl
	pop	de
	pop	bc
	ret


;negate 4-byte integer
;input: FAC1(integer)
;output: FAC1(integer), c-flag, z-flag
;destroy: af,hl
NEGINT4:
	ld	hl,FAC1
	xor	a
	sub	(hl)
	ld	(hl),a
	inc	hl
	ld	a,00h
	sbc	a,(hl)
	ld	(hl),a
	inc	hl
	ld	a,00h
	sbc	a,(hl)
	ld	(hl),a
	inc	hl
	ld	a,00h
	sbc	a,(hl)
	ld	(hl),a
	ret


;compare 4-byte integer and 4-byte integer
;input: FAC1(integer), FAC2(integer)
;output: c-flag,z-flag
;destroy: f,de,hl
CMPINT4:
	ld	hl,(FAC1+2)
	ld	de,(FAC2+2)
	or	a
	sbc	hl,de
	ret	nz
	ld	hl,(FAC1)
	ld	de,(FAC2)
	sbc	hl,de
	ret


;decrement 4-byte integer
;input: FAC1
;output: FAC1
;destroy: af,hl
DECINT4:
	ld	hl,(FAC1)
	ld	a,h
	or	l		;
	dec	hl
	ld	(FAC1),hl
	ret	nz		;
	ld	hl,(FAC1+2)
	dec	hl
	ld	(FAC1+2),hl
	ret


;4-byte integer=0?
;output: a(4 bytes OR),z
;destroy: af
CHKINT4:
	push	hl
	ld	hl,(FAC1)
	ld	a,h
	or	l
	ld	hl,(FAC1+2)
	or	h
	or	l
	pop	hl
	ret


;convert float to 4-byte integer (>=0)
;(round toward zero)
;input: FAC1
;output: FAC1 (integer)
;destroy: af,b,de,hl
FTOI4:
	ld	a,81h+1fh
	ld	hl,FAC1+4
	sub	(hl)		;
	ld	b,a
	ld	hl,(FAC1)
	ld	de,(FAC1+2)
	set	7,d
	jr	z,FTOI4END	;
FTOI4LP:
	srl	d
	rr	e
	rr	h
	rr	l
	djnz	FTOI4LP
FTOI4END:
;	jp	SETI4

;set 4-byte integer in FAC1
;input: de-hl
SETI4:
	ld	(FAC1),hl
	ld	(FAC1+2),de
	ret



;convert 4-byte integer to float (unsigned)
;input: de-hl
;output: FAC1
;destroy: af,b,hl
I4TOF:
	push	de
	ld	a,d
	or	e
	or	h
	or	l
	jp	z,I2TOFZERO

	ld	a,d
	ld	b,20h
I4TOFLP:
	add	a,80h
	jr	c,I4TOFEND
	add	hl,hl
	rl	e
	rla
	djnz	I4TOFLP

I4TOFEND:
	ld	d,a
	jp	I2TOFEND2


;check cload header and get file name
;destroy: af,b,de (if no error)
_GETFN:	ds	GETFN-_GETFN
	org	GETFN

;check header (d3h*10)
	ld	de,0ad3h
	call	CMTSKP
;file name
	ld	de,FNAME

	ld	b,06h
GETFNLP:
	call	GETCMTTR
	ld	(de),a
	inc	de
	djnz	GETFNLP
	ret


;skip CMT data
;input: d=count, e=data
;destroy: af,b
_CMTSKP:ds	CMTSKP-_CMTSKP
	org	CMTSKP

	ld	b,d
CMTSKPLP:
	call	GETCMTTR
	cp	e
	jr	nz,CMTSKP
	djnz	CMTSKPLP
	ret


_FOUND:	ds	FOUND-_FOUND
	org	FOUND

	db	"Found:", 00h

SKIP:
	db	"Skip:", 00h


;put message and file name in device
;input: hl=message address
;destroy: af,hl,(bc,de,FAC1)
_PUTFN:	ds	PUTFN-_PUTFN
	org	PUTFN

	call	PUTS
	ld	hl,FNAME
	call	PUTS
	jp	PUTNL


;CMT open for INPUT #-1
;destroy: af,b
_INPOPN:ds	INPOPN-_INPOPN
	org	INPOPN

;	ld	a,0ffh
;	ld	(VERIFY),a
;	call	ROPEN
	call	VRFOPN
	push	de
	ld	de,069ch
	call	CMTSKP
	pop	de
	ret


;CMT open for PRINT #-1
;destroy: a,b
_PRTOPN:ds	PRTOPN-_PRTOPN
	org	PRTOPN

	call	WOPEN
	ld	a,9ch
	ld	b,06h
PRTOPNLP:
	call	PUTCMT
	djnz	PRTOPNLP
	ret


;convert intermediate code to command/function ascii characters
;input: a=code
;output: a=first character, hl=address
;destroy: f,b,hl
CNVASC:
	ld	hl,CMDNAME-1
	cp	TAB_
	jr	c,CNVASCC
;function
	ld	hl,FNCNAME-1
	sub	TAB_-80h

CNVASCC:
	sub	7fh
	ld	b,a
CNVASCLP:
	inc	hl
	ld	a,(hl)
	sub	80h
	jr	c,CNVASCLP
	djnz	CNVASCLP
	ret


;wait for about 3.5s
;destroy: af,bc
_WAIT:	ds	WAIT-_WAIT
	org	WAIT

	ld	bc,0000h
;25e8
WAITLP:
	ld	a,(STOPFLG)
	cp	03h
	ret	z

	ex	(sp),hl		;waste time
	ex	(sp),hl		;waste time
	nop			;waste time

	dec	bc
	ld	a,b
	or	c
	jr	nz,WAITLP
	ret


;EXEC command
C_EXEC:
	call	NARGMO
	call	FTOI

;for compatibility with N60-BASIC
	ld	hl,(PROGAD)
	ex	de,hl
	ld	a,h
	or	l
	neg			;set c-flag if address<>0
	push	de
	call	JPHL
	pop	de
	ret


;get a character from buffer
;input: hl=buffer map address
;output: a,z-flag(1=no input)
;destroy: f,hl
GETBF:
	ld	a,(hl)		;in
	inc	l		;inc hl
	cp	(hl)		;out
	ret	z
	ld	a,(hl)
	inc	a
	inc	l		;inc hl
	inc	l		;inc hl
	and	(hl)		;size
	dec	l		;dec hl
	dec	l		;dec hl
	ld	(hl),a		;out
	inc	l		;inc hl
	inc	l		;inc hl
	inc	l		;inc hl
	add	a,(hl)		;hl=(buffer address)+a
	inc	l		;inc hl, l>0, reset z-flag
	ld	h,(hl)
	ld	l,a
	jr	nc,GETBFNC
	inc	h		;h>0, reset z-flag
GETBFNC:
	ld	a,(hl)
	ret


;input: STRDSC1, STRDSC4
;destroy: af,bc,de,hl
ADDSTR:
	ld	a,(STRDSC4)
ADDSTR2:
	ld	b,a		;;
	ld	a,(STRDSC1)
	ld	c,a		;
	add	a,b		;c+b
	jp	c,LSERR		;over 255 characters

	call	GCCHECK
	ld	(STRAD),hl
	inc	hl
	ex	de,hl
	ld	hl,(STRDSC1+2)
	ld	(STRDSC1+2),de

	ld	a,b		;;
	add	a,c		;
	ld	(STRDSC1),a
	sub	b		;c=0?

	ld	a,b
	ld	b,00h
	jr	z,ADDSZ1
	ldir
ADDSZ1:
	or	a
	jr	z,ADDSZ2
	ld	c,a		;b=0
	ld	hl,(STRDSC4+2)
	ldir
ADDSZ2:
	xor	a
	ld	(STRDSC4),a
	inc	a
	ld	(ARGTYP),a	;=1
	ret


;check string area and call GC if necessary
;input: a=new string length
;output: hl=new (STRAD)+1
;destroy: af,de
GCCHECK:
	push	bc
	ld	d,00h
	ld	e,a
	push	de
	call	CHKSAREA
	pop	de
	jr	nc,GCCHECKOK
	push	de
	call	GC
	pop	de
	call	CHKSAREA
	jp	c,OSERR
GCCHECKOK:
	pop	bc
	ret

;check string area
CHKSAREA:
	ld	hl,(STRAD)
	or	a
	sbc	hl,de
	ld	de,(STACK)
	rst	CPHLDE
	ret


;get a string argument
;input: hl=program address
;output: a=length, de=string address, hl=next address, z-flag(1=ok)
;destroy: f,bc,FAC1,FAC2
STRARG:
	call	CHKCLN
	jp	z,MOERR
	call	ARG
STRARG2:
	ex	de,hl
	call	CHKSTR
;get string pointer
	inc	hl
	inc	hl
	ld	b,(hl)
	inc	hl
	ld	h,(hl)
	ld	l,b
	ex	de,hl
	ret


;get string,integer argument for LEFT$(),RIGHT$(),MID$()
;input: hl=program address
;output: STRDSC1, a=length, hl=string address, FAC1,de=integer
;destroy: FAC2,f,bc
ARGSI1:
	call	CHKLPAR
	call	STRARG
	push	af
	ld	(STRDSC1),a	;length
	ld	(STRDSC1+2),de	;string address
	ld	a,(hl)
	cp	','
	jp	nz,SNERR
	call	ARGI1
	ld	hl,(STRDSC1+2)	;string address
	pop	af		;length
	ret


;protect string descriptor and get integer argument
;input: hl=program address
;output: FAC1,de=integer
;destroy: FAC2,af,bc,hl
ARGI1:
	push	hl		;program address
	call	COPYSTR
	pop	hl
	call	INT1INC
	jp	BACKSTR


;put a character in device
;input: a=character code, (DEVICE)=0:CRT, 1:printer, 2:RS-232C, 80h-ffh:CMT
; (1,4,5,8,9,c,d...:printer, 2,3,6,7,a,b,e,f...:RS-232C)
;destroy: af
_PUTDEV:ds	PUTDEV-_PUTDEV
	org	PUTDEV

	call	HOOKPUT
	push	af
	ld	a,(DEVICE)
	or	a
	jr	z,PUTCRT
	jp	m,PUTCMT2
	and	02h
	jr	z,PUTPRT2
;	jr	nz,PUT232

;input: a
;output: none
;destroy: af
PUT232:
	ld	a,17h		;RTS=0
	out	(81h),a

PUT232LP:
	in	a,(81h)
	cpl
	and	05h
	jr	z,PUT232Z	;if TxEMPTY=1, TxRDY=1

	ld	a,(STOPFLG)
	cp	03h
	jr	nz,PUT232LP
;stop
	call	PUT232END
	jr	STOPSP

PUT232Z:
	pop	af
	out	(80h),a
PUT232END:
	ld	a,37h		;RTS=1
	out	(81h),a
	ret


PUTPRTTAB:
	ld	a,' '
	call	PUTPRT3		;a<-(PRTPOS)
	and	07h
	jr	nz,PUTPRTTAB
	ret


;put new line to printer
;destroy: af
_PUTPRNL:ds	PUTPRNL-_PUTPRNL
	org	PUTPRNL
	ld	a,01h
	ld	(DEVICE),a
	ld	a,(PRTPOS)
	or	a
	call	nz,PUTNL
	xor	a
	ld	(DEVICE),a
	ret


PUTPRT2:
	pop	af
	cp	09h
	jr	z,PUTPRTTAB
PUTPRT3:
	call	PUTPRT
	sub	0dh
	jr	z,PUTPRTZ	;a=0 if CR
	ret	c
	ld	a,(PRTPOS)
	inc	a
PUTPRTZ:
	ld	(PRTPOS),a
	ret


PUTCRT:
	pop	af
	jp	PRTC


;put new line if necessary
;destroy: af
_PUTNLX:ds	PUTNLX-_PUTNLX
	org	PUTNLX
	ld	a,(DEVICE)
	or	a
	jp	z,PRTNLX
	dec	a
	jr	z,PUTPRNL
	jr	PUTNL


;put new line
;destroy: af
_PUTNL:	ds	PUTNL-_PUTNL
	org	PUTNL

	ld	a,0dh
	call	PUTDEV
	ld	a,0ah
	jr	PUTDEV


;input: a=length, hl=string address
;destroy: af,bc,de,hl
MAKESTR:
	ld	(STRDSC1),a
	ld	(STRDSC1+2),hl
	xor	a
	jp	ADDSTR2


;STOP and ESC
;destroy: af
_STOPESC:ds	STOPESC-_STOPESC
	org	STOPESC

	call	CHKSTOP
	ret	nz
;	jp	STOPSP

;reset sp and stop
STOPSP:
	ld	sp,(STACK)
	ld	hl,0ffffh
	jp	STOP2


;check STOP/ESC
;output: z-flag (1=STOP)
;destroy: af
CHKSTOP:
	ld	a,(STOPFLG)
	cp	03h
	ret	z
	cp	1bh
	ret	nz
	call	GETCH		;cancel ESC
	jr	CHKSTOP


;INKEY$
F_INKY:
	ld	hl,FNCRTN
	push	hl

	call	GETCH
	jr	z,INKYZ

	cp	03h
	jr	z,INKYZ
	cp	1bh
	jr	z,INKYZ

	ld	b,a		;
INKYNZ:
	ld	a,01h		;new string length=1
	ld	(STRDSC1+3),a	;old string descriptor address < (STRAD)
	call	GCCHECK
	ld	(STRAD),hl
	inc	hl
	ld	(hl),b		;
	ld	(STRDSC1+2),hl	;new string descriptor address
	ld	a,01h
INKYLEN:
	ld	(STRDSC1),a

INKYEND:
	ld	hl,STRDSC1
	ld	(FAC1+1),hl
	ld	a,01h
	ld	(ARGTYP),a
	ret

INKYZ:
	xor	a
	jr	INKYLEN


;get device number
;in	out
;#0	0	CRT
;#-1	80h	CMT
;#-2	2	RS-232C
;#-3	1	printer
;input: hl=program address
;output: a, hl,(PROGAD)=next address
;destroy: f,bc,de
DEVNUM:
	call	SKIPSP
	cp	'#'
	ld	a,00h
	ret	nz

	inc	hl
	call	INT2ARG
	push	hl
	dec	de
	ld	hl,0004h
	add	hl,de
	jp	nc,FCERR
	ld	a,l
	inc	a
	cp	03h
	jr	c,DEVNUMC	;#-2,#-3
	and	01h
	rrca
DEVNUMC:
	pop	hl
	push	af
	call	CHKCMM
	jp	nz,SNERR
	ld	(PROGAD),hl
	pop	af
	ret


;convert string to 2-byte integer (unsigned)
;input: hl=program address
;output: de=integer, hl=next address
;destroy: af,bc
ATOI2:
	ld	b,00h

;input: hl=program address, b=string length
;output: b=remaining length, de=integer, hl=next address
ATOI2LEN:
	ld	de,0000h
ATOI2LP:
	call	SKIPSP
	sub	'0'
	cp	'9'-'0'+1
	ret	nc

	push	hl

	ld	hl,0-6554
	add	hl,de
	jr	c,ATOI2END

	ld	h,d
	ld	l,e
	add	hl,hl
	add	hl,hl
	add	hl,de
	add	hl,hl

	add	a,l
	ld	l,a
	jr	nc,ATOI2NC
	inc	h
	jr	z,ATOI2END

ATOI2NC:
	ex	de,hl
	pop	hl
	inc	hl
	djnz	ATOI2LP
	ret

ATOI2END:
	pop	hl
	ret


;convert string to float (integer part)
;input: hl=address
;output: FAC1, hl=next address
;destroy: FAC2,af,bc,de
ATOIF:
	push	hl
	call	SETZERO
	pop	hl
	call	SKIPSPA
	ret	z

	ld	b,00h		;sign for CTOF
	inc	hl
	cp	'+'
	jr	z,ATOIFLP
	cp	PLUS_
	jr	z,ATOIFLP
	inc	b
	cp	'-'
	jr	z,ATOIFLP
	cp	MINUS_
	jr	z,ATOIFLP
	dec	b
	dec	hl
ATOIFLP:
	call	CTOF
	jr	c,ATOIFLP
	ret


;FAC1 = FAC1 * 10 + [0-9]
;input: FAC1, hl=address, b=sign(0=plus, 1=minus)
;output: FAC1, c-flag(1=OK), hl=next address
;destroy: FAC2,af,de
CTOF:
	call	SKIPSPA
	ret	z		;c-flag=0

	sub	'0'
	cp	'9'-'0'+1
	ret	nc
	inc	hl
	push	hl

	push	bc
	push	af
	call	ABS
	call	MULF10
	call	CPYFAC
	pop	af
	call	I1TOFA
	call	ADDF
	pop	bc

	ld	a,b
	rrca
	call	c,NEGABS

	pop	hl
	scf
	ret


;insert a character
;input: a=data (for checking BELL)
;output: hl=VRAM address
;destroy: f,bc,de
INSERT:
	cp	07h
	jr	nz,INSNZ
	call	PRTC
	ld	a,' '
INSNZ:
	push	af

	ld	hl,(CSRAD)

	call	GETSCRC
	ld	b,a		;

	inc	hl
	call	CHKLINE
	jr	nz,INSSCRL

	call	CNVATT1
	ld	c,a

INSLP:
	call	GETSCRC
	dec	h
	dec	h
	ld	(hl),c
	inc	h
	inc	h

	ld	(hl),b
	ld	b,a
	inc	hl
	call	CHKLINE
	jr	z,INSLP

	ld	a,b
	cp	' '
	jr	nz,INSSCRL
	pop	af
	ret


;clear screen using console parameters
;destroy: af,bc,de,hl
_CTLL:	ds	CTLL-_CTLL
	org	CTLL

	call	CTLHOM
	ld	bc,(FSTLIN)	;c=(FSTLIN),b=(LASTLIN)
	ld	a,b
	sub	c
	inc	a

;input: e=first line+1, a=last line-first line+1
CLSMAIN2:
	ld	b,00h
	ld	hl,LINEST-1
	add	hl,bc
	ld	b,a
	inc	b
CLSLP1:
	ld	(hl),b		;b>0
	inc	hl
	djnz	CLSLP1

	ld	b,a
CLSLP2:
	ld	l,c
	call	Y2AD
	call	DELLIN
	inc	c
	djnz	CLSLP2

	jp	PRTFKEY


INSSCRL:
	push	hl

	call	AD2XY

	ld	a,(LASTLIN)
	cp	l
	jr	c,INSSCRLU

	ld	h,a		;(LASTLIN)
	jr	z,NOSCRLD
	call	SCRLDW

INSSCRLEND:
	ld	c,l		;;
	dec	l
	xor	a

INSSCRLEND2:
	call	SETLINE
	ld	l,c		;;
	call	Y2AD
	call	DELLIN
	pop	hl

	dec	h
	dec	h
	call	CNVATT1
	ld	(hl),a
	inc	h
	inc	h

	ld	(hl),b		;
	pop	af
	ret

INSSCRLU:
	pop	hl
	ld	de,0-COLUMNS
	add	hl,de
	push	hl

	ld	l,a		;(LASTLIN)
	ld	a,1eh		;up
	call	PRTC

	ld	a,(CONSOL1)
	ld	h,a
	sub	l
	ld	c,l
	jr	z,NOSCRLU	;a=0

	call	SCRLUP
	jr	INSSCRLEND

NOSCRLD:
	xor	a
	ld	(INPTXY),a	;scroll out
	jr	INSSCRLEND

NOSCRLU:
;	xor	a
	ld	(INPTXY),a	;scroll out
	inc	a
	jr	INSSCRLEND2


;get character from screen
;input: hl
;output: a
;destroy: f
GETSCRC:
	ld	a,(hl)
	dec	h
	dec	h
	bit	6,(hl)
	jr	z,GETSCRCZ
	ld	a,' '
GETSCRCZ:
	inc	h
	inc	h
	ret


;screen input
;output: hl=INPBUF-1, c-flag(1=stop)
;destroy: af,bc,de
_INPTSCR:ds	INPTSCR-_INPTSCR
	org	INPTSCR

	call	SCREDIT
	ret	c
	ld	a,71
	jr	COPYIBF


;one line input for keyboard
;output: hl=INPBUF-1, c-flag(1=stop)
;destroy: af,bc,de
_INPT1:	ds	INPT1-_INPT1
	org	INPT1

	ld	hl,QMARK
	call	PUTS
	call	SCREDIT
	ret	c
	ld	a,(INPTX)
;	jr	COPYIBF

;copy characters to input buffer from VRAM
;input: a=length, hl=start position address
;output: hl=INPBUF-1, c-flag=0
;destroy: af,bc,de
COPYIBF:
	ld	de,INPBUF
	or	a
	jr	z,CPIBFZ
	ld	c,a
	ld	b,71
CPIBFLP1:
	call	GETSCRC
	cp	20h
	jr	nc,CPIBFNC
	ex	de,hl
	ld	(hl),14h
	ex	de,hl
	djnz	CPIBFGRP
	xor	a
	jr	CPIBFNZ
CPIBFZ:
	ld	(de),a
	jr	CPIBFNZ
CPIBFGRP:
	inc	de
	add	a,30h

CPIBFNC:
	ld	(de),a
	inc	de
	inc	hl
	call	CHKLINE
	jr	nz,CPIBFNZ
	dec	c
	jr	z,CPIBFNZ
	djnz	CPIBFLP1

CPIBFNZ:
	dec	hl
	call	AD2XY
	call	SETCSR
	call	PUTNL

CPIBFLP2:
	dec	de
	ld	a,(de)
	cp	' '
	jr	z,CPIBFLP2
	inc	de
	xor	a
	ld	(de),a

	ld	hl,INPBUF-1
	ret


;screen edit
;output: hl=start position address, c-flag(1=stop)
;destroy: af,bc,de
SCREDIT:
	call	CUTLINE
	ld	hl,(CSRY)
	ld	(INPTX-1),hl	;h=end position for INPUT command
	ld	(INPTXY),hl	;initial position for INPUT command
	ld	a,(CSRAD+1)
	ld	(INPTPAG),a	;page for INPUT command

SEDLP:
	call	GETC
	call	SEDCMD
	jr	SEDLP


;one line input main routine
SEDCMD:
	cp	20h
	jr	c,SEDCTL

	ld	hl,(INSMODE)
	inc	l
	dec	l
	call	nz,INSERT

SEDPRTC:
	call	PRTC

CHKIPOS2:
	ld	hl,(CSRAD)

;check cursor position for input command
;input: hl=cursor address
;output: hl=cursor XY, de=cursor address
;destroy: af
CHKIPOS:
	ld	d,h
	ld	e,l
	call	AD2XY
	ld	a,(INPTXY)	;y+1
	cp	l
	ret	nz

	ld	a,(INPTX)
	cp	h
	jr	nc,IPOSNC
	ld	a,h
	ld	(INPTX),a
IPOSNC:
	ld	a,(INPTXY+1)	;x+1
	cp	h
	ret	c

	ld	a,(INPTPAG)
	xor	d
	and	0f0h
	ret	nz

	ld	(INPTXY),hl
	ret


NOINSTBL:
	db	02h, 03h, 05h, 06h, 0bh, 0ch, 0dh, 15h, 1ch, 1dh, 1eh, 1fh

DELSTR:
	db	1dh, ' ', 1dh, 00h


;control code
SEDCTL:
	ld	hl,NOINSTBL
	ld	bc,000ch
	cpir
	jr	nz,SEDCMDNZ
	ld	hl,INSMODE
	ld	(hl),b		;b=0
SEDCMDNZ:
	ld	hl,(CSRAD)

	cp	02h
	jr	z,SEDCTLB
	cp	03h
	jp	z,SEDSTOP
	cp	05h
	jr	z,SEDCTLE
	cp	06h
	jp	z,SEDCTLF
	cp	08h
	jr	z,SEDDEL
	cp	09h
	jp	z,SEDTAB
	cp	0ah
	jp	z,SEDCTLJ
	cp	0dh
	jp	z,SEDRET
	cp	15h
	jr	z,SEDCTLU
	cp	12h
	jr	nz,SEDPRTC	;14h,1ch,1dh,1eh,1fh

SEDINS:
	ld	hl,INSMODE
	ld	a,(hl)
	cpl
	ld	(hl),a
	ret


SEDDEL:
	call	CHKLINE
	ex	de,hl
	ld	hl,DELSTR
	jr	z,SEDDELZ
	inc	de
	inc	hl
SEDDELZ:
	push	de		;
	call	PUTS
	call	CHKIPOS2
	pop	de		;
	ld	h,d
	ld	l,e
	dec	de
	call	CNVATT1
	ld	c,a
DELLP:
	call	GETSCRC
	ld	(de),a

	dec	h
	dec	h
	ld	(hl),c
	inc	h
	inc	h

	inc	de
	inc	hl

	call	CHKLINE
	jr	z,DELLP

	dec	hl
	ld	(hl),' '
	dec	h
	dec	h
	ld	(hl),c

	ret


CTLULP:
	dec	hl
SEDCTLU:
	call	CHKLINE
	jr	z,CTLULP

SEDCTLE:
	dec	h
	dec	h
	call	CNVATT1
	ld	(hl),a
	inc	h
	inc	h
	ld	(hl),' '
	inc	hl
	call	CHKLINE
	jr	z,SEDCTLE
	ret


SEDCTLB:
	call	CHKCTLB
	jr	nc,SEDCTLB
CTLBLP:
	call	CHKCTLB
	jr	c,CTLBLP
	inc	hl
	call	CHKIPOS
	jp	SETCSR

CHKCTLB:
	dec	hl
	ld	d,h
	ld	e,l
	call	AD2XY
	ld	a,(CONSOL1)
	inc	l
	cp	l
	jr	c,CTLBC
CTLBNC:
	pop	af		;cancel return address
	jp	CTLCR


SEDCTLF:
	call	CHKCTLF
	jr	c,SEDCTLF
CTLFLP:
	call	CHKCTLF
	jr	nc,CTLFLP
	ex	de,hl
	call	SETCSR
	jp	CHKIPOS2

CHKCTLF:
	inc	hl
	ld	d,h
	ld	e,l
	call	AD2XY
	ld	a,(LASTLIN)
	cp	l
	jr	c,CTLFC
CTLBC:
	ex	de,hl
	call	GETSCRC
	jp	ALPNUM2

CTLFC:
	ld	l,a
	ld	h,COLUMNS
	pop	af		;cancel return address
	jp	SETCSR


SEDCTLJ:
	ld	a,(INSMODE)
	or	a
	ld	a,0ah
	jp	z,SEDPRTC

	ld	hl,(CSRY)	;l=y+1, h=x+1
	ld	a,COLUMNS+1
	sub	h
	ld	b,a
CTLJLP:
	push	bc
	ld	a,' '
	call	INSERT
	ld	hl,(CSRAD)
	ld	(hl),a
	pop	bc
	djnz	CTLJLP

	call	AD2XY
	call	SETCSR
	ld	a,h		;>0
	jp	SETLINE


SEDTAB:
	ld	a,(CSRX)
	sub	COLUMNS-07h
	sbc	a,07h
	ret	z
	cpl
	and	07h
	inc	a
	ld	b,a
	ld	a,(INSMODE)
	and	04h
	add	a,1ch		;1ch(right) or 20h(space)
	ld	c,a
SEDTABLP:
	push	bc
	ld	a,c
	call	SEDCMD
	pop	bc
	djnz	SEDTABLP
	ret


SEDSTOP:
	pop	hl		;cancel return address
	ld	hl,(CSRY)
	xor	a
	ld	h,a
	ld	de,LINEST-2
	add	hl,de
STOPLP:
	inc	hl
	or	(hl)
	jr	z,STOPLP

;	or	a
	sbc	hl,de
	dec	l
	call	SETCSR
	call	PUTNL

	ld	hl,INPBUF
	xor	a
	ld	(STOPFLG),a
	ld	(hl),a
	dec	hl
	scf
	ret


SEDRET:
	pop	af		;cancel return address

	ld	a,(INPTPAG)
	xor	h		;(CSRAD+1)
	and	0f0h
	jr	nz,SETLENMAX	;if other page

	ld	a,(CSRY)
	ld	de,(INPTXY)
	cp	e
	jr	nz,SETLENMAX

	ld	bc,LINEST-1
	ld	a,c
	add	a,e		;no carry
	ld	c,a
	ld	a,(bc)
	or	a
	jr	z,SETLENMAX	;if connected to next line
	dec	bc
	ld	a,(bc)
	or	a
	jr	z,SETLENMAX	;if connected to previous line

	ld	a,(INPTX)	;end position
	sub	d		;(INPTXY+1)
	jr	SETLEN

SETLENMAX:
	ld	a,71
SETLEN:
	ld	(INPTX),a	;length

SRETLP:
	call	CHKLINE
	dec	hl
	jr	z,SRETLP

	inc	hl
	ld	d,h		;
	call	AD2XY
	ld	a,(INPTPAG)
	xor	d		;
	and	0f0h
	jr	nz,SRETNZ	;if other page

	ld	de,(INPTXY)	;initial position
	ld	a,l
	cp	e
	jr	nz,SRETNZ	;if other line
	ex	de,hl		;if same line
SRETNZ:
	or	a		;reset c-flag
	jp	XY2AD


;x=x+-1
;input: b(bit7=0:increment, bit7=1:decrement), (GRPX3)
;output: hl,(GRPX3)=X, z-flag(1=overflow)
;destroy: af,de
INCGX:
	ld	hl,(GRPX3)
	bit	7,b
	jr	nz,INCGXNZ
	inc	hl
	ld	a,h
	or	l
	jr	INCGXEND
INCGXNZ:
	ld	a,h
	or	l
	dec	hl
INCGXEND:
	ld	(GRPX3),hl
	ret


;y=y+1
;input: (GRPY3)
;output: de=(GRPY3)
;destroy: af
INCGY:
	ld	de,(GRPY3)
	inc	de
	ld	(GRPY3),de
	ret


;error-=|dx|*2
;input: bc=dx, hl=error
;output: hl=error
;destroy: af
SUB2DX:
	ld	a,b
	rlca
	jr	c,SUB2DXNZ
;dx>0
;	or	a
	sbc	hl,bc
	sbc	hl,bc
	ret

;dx<0
SUB2DXNZ:
	add	hl,bc
	add	hl,bc
	ret


;x1 <-> x2, y1 <-> y2
;input: bc=x1, de=y1, (GRPX2)=x2, (GRPY2)=y2
;output: (GRPX2)=larger x,(GRPY2)=larger y,(GRPX3)=smaller x,(GRPY3)=smaller y
;destroy: bc,de,hl
SORTXY:
	push	af
	or	a
	call	CHKGXY23
	jr	c,SORTXOK
	ld	(GRPY2),hl
	ld	(GRPY3),de
SORTXOK:
	ld	hl,(GRPX2)
	ld	de,(GRPX3)
	rst	CPHLDE
	jr	c,SORTX
	pop	af
	ret


;(x1,y1) <-> (x2,y2)
;input: bc=x1, de=y1, (GRPX2)=x2, (GRPY2)=y2
;output: (GRPX2),(GRPY2)=larger y with x, (GRPX3),(GRPY3)=small y with x
;destroy: bc,de,hl
SORTY:
	push	af
	scf
	call	CHKGXY23
	jr	c,NOSORT
	ld	(GRPY2),hl
	ld	(GRPY3),de
	ld	hl,(GRPX2)
	ld	de,(GRPX3)
SORTX:
	ld	(GRPX2),de
	ld	(GRPX3),hl
NOSORT:
	pop	af
	ret


;sort XY and put bc,de,hl
;output: bc=smaller x, de=smaller y, hl=larger y
;destroy: none
SORTXY2:
	call	SORTXY
	ld	bc,(GRPX3)	;smaller x, b=0
	ld	de,(GRPY3)	;smaller y
	ld	hl,(GRPY2)	;larger y
	ret


;push (GRPX2),(GRPY2)
;destroy: af,hl
PUSHGXY:
	pop	af		;return address
	ld	hl,(GRPX2)
	push	hl
	ld	hl,(GRPY2)
	push	hl
	push	af		;return address
	ret


;input: bc=x1, de=y1, (GRPX2)=x2, (GRPY2)=y2,
;       c-flag(1=convert to screen mode cordinate)
;output: bc=(GRPX2)=x2, de=(GRPY2)=y2, hl=(GRPY3)=y1, (GRPX3)=x1, z,c(y1>y2?)
;destroy: af
CHKGXY23:
	push	af
	call	c,GXY2SXY
	pop	af
	push	af
	call	nc,CHKGXY
	pop	af
	ld	(GRPX3),bc
	ld	(GRPY3),de
	ld	bc,(GRPX2)
	ld	de,(GRPY2)
	push	af
	call	c,GXY2SXY
	pop	af
	call	nc,CHKGXY
	ld	(GRPX2),bc
	ld	(GRPY2),de

	ld	hl,(GRPY3)
	rst	CPHLDE
	ret


;check and round graphic coordinates (x,y)
;input: bc=x, de=y
;output: bc=x, de=y
;destroy: af,hl
CHKGXY:
	ld	a,b
	or	a
	jr	z,CHKMNS
	ld	c,0ffh
CHKMNS:
	or	d
	jp	m,FCERR

	ld	a,(SCREEN1)
	ld	b,a		;
	rrca
	cpl
	ld	hl,CONSOL3
	and	(hl)
	rrca
	ld	hl,191
	jr	nc,NOFKEY
	ld	hl,191-12
NOFKEY:
	rst	CPHLDE
	jr	nc,CHKYOK
	ex	de,hl
CHKYOK:
	ld	a,b		;
	dec	a
	jr	z,ROUNDY2
	jp	p,ROUNDX	;screen mode 3,4

;round off y for screen mode 1
ROUNDY1:
	call	DIV12
	ld	a,e
	add	a,a		;*2
	add	a,e		;*3
	add	a,a		;*6
	add	a,a		;*12
	jr	ROUNDY2END

;round off y for screen mode 2
ROUNDY2:
	ld	a,e
	and	0fch
ROUNDY2END:
	ld	e,a

ROUNDX:
	inc	b		;
	ld	a,0f0h
ROUNDXLP:
	scf
	rra			;f8,fc,fe,ff
	djnz	ROUNDXLP

	and	c
	ld	c,a		;b=0
	ret


PSCOLR:
	push	bc		;x
	push	de		;y
	ld	b,a		;color
	ld	a,(hl)
	cp	','
	ld	a,b		;color
	call	z,INT1INC
	pop	de		;y
	pop	bc		;x
	jp	SETATT


;screen mode 2
PSET2:
	dec	h
	dec	h
	ld	a,(ATTDAT)
	ld	e,(hl)		;
	or	a
	jr	nz,PS2NZ1
	bit	6,e		;
	jr	nz,PS2NZ2
PS2NZ1:
	call	GETSPA2NZ
	ld	(hl),a
PS2NZ2:
	inc	h
	inc	h
	bit	6,e		;
	jr	nz,PS2SEMI	;semi-graphic mode?
	call	GETSP
	ld	(hl),a

PS2SEMI:
	ld	a,(hl)
	and	3fh
	or	d
	ld	c,a		;;

	ld	a,(ATTDAT)
	or	a
	jr	z,PSET2C0
	dec	a
	and	03h
	rrca
	rrca
	add	a,c		;;
	ld	(hl),a
	pop	hl
	ret

PSET2C0:
	ld	a,d
	cpl
	and	(hl)
	ld	(hl),a
	pop	hl
	ret


;continued from BFG
BFG2:
	ld	a,(GRPX3)	;smaller x
	and	0f8h
	ld	b,a
	ld	a,(GRPX2)	;larger x
	and	0f8h
	sub	b		;
	rrca
	rrca
	rrca			;int(larger/8)-int(smaller/8)
	ld	b,a		;bytes
	jr	nz,BFGLP3	;a=0?
	ld	a,d
	and	e
	ld	d,a

BFGLP3:
;left part
	push	bc
	ld	a,(ATTDAT)
	xor	(hl)
	and	d		;d=left part mask
	xor	(hl)
	ld	(hl),a

	dec	b		;
	jp	m,BFGNEXT

;middle part
	push	hl		;VRAM address
	ld	a,(ATTDAT)
	jr	z,BFGRGT	;
BFGLP4:
	inc	hl
	ld	(hl),a
	djnz	BFGLP4

;right part
BFGRGT:
	inc	hl
	xor	(hl)
	and	e		;e=right part mask
	xor	(hl)
	ld	(hl),a
	pop	hl		;VRAM address

BFGNEXT:
	ld	bc,0020h
	add	hl,bc
	pop	bc
	dec	c
	jr	nz,BFGLP3
	jp	LINEEND2


;if |dy/dx|<=1
SLOPEX:
	inc	hl
;	or	a
	sbc	hl,de		;error=-|dx|
	exx			;bc'=dx, de'=dy, hl'=error
LINELP2:
	ld	bc,(GRPX3)
	ld	de,(GRPY3)
	call	PSETSXY

	exx
	add	hl,de		;error+=dy
	add	hl,de		;error+=dy
	bit	7,h
	jr	nz,ERRMNS

;if error>=0
	call	SUB2DX		;error-=|dx|*2
	exx
	call	INCGY		;increment Y
	exx

ERRMNS:
	ld	a,b
	exx
	ld	b,a
	call	INCGX		;increment or decrement X
	jp	z,LINEEND

	ex	de,hl		;de=X
	ld	hl,(GRPX2)
	exx
	bit	7,b
	exx
	jr	z,LINEZ
	ex	de,hl		;if dx<0
LINEZ:
	rst	CPHLDE
	jr	nc,LINELP2
	jp	LINEEND


;get [step](x,y) cordinate
;input: hl=program address
;output: bc=graphic X, de=graphic Y,
;	 (GRPX1)=(GRPX2),(GRPY1)=(GRPY2)=0-32767
;	 a=next data, hl=next data address, z(, or not)
;destroy: f
GETGXY:
	call	SKIPSP
	cp	STEP_
	push	af		;STEP?
	jr	nz,GETGXYNZ
	inc	hl
GETGXYNZ:
	call	CHKLPAR
	call	INTARG2		;bc,de
	call	CHKRPAR
	pop	af		;STEP?
	call	SETGXY
	call	SKIPSP
	cp	','
	ret


;input: bc=x, de=y, z(0=absolute, 1=relative)
;output: bc=x, de=y, (GRPX1)=(GRPX2)=x, (GRPY1)=(GRPY2)=y
;destroy: f
_SETGXY:ds	SETGXY-_SETGXY
	org	SETGXY

	push	hl
	jr	nz,SETGXYNZ
	ld	hl,(GRPX1)
	add	hl,bc
	ld	b,h
	ld	c,l
	ld	hl,(GRPY1)
	add	hl,de
	ex	de,hl
SETGXYNZ:
	ld	(GRPX1),bc
	ld	(GRPX2),bc
	ld	(GRPY1),de
	ld	(GRPY2),de
	pop	hl
	ret


;PRESET command
_C_PRST:ds	C_PRST-_C_PRST
	org	C_PRST

	ld	a,(COLOR2)
	jr	PSETRST

;PSET command
_C_PSET:ds	C_PSET-_C_PSET
	org	C_PSET

	ld	a,(COLOR1)

;PSET and PRESET
PSETRST:
	push	af
	call	GETGXY
	pop	af
	call	PSCOLR
;	jp	PSET


;input: bc=x1, de=y1, (ATTDAT)=attribute
;destroy: af,bc,de
_PSET:	ds	PSET-_PSET
	org	PSET

	call	GXY2SXY

PSETSXY:
	push	hl
	call	SXY2AD
	ld	d,a
	ld	a,(SCREEN1)
	dec	a
	jp	z,PSET2
	ld	a,(ATTDAT)
	jp	m,PSET1

;screen mode 3,4
PSET34:
	xor	(hl)
	and	d
	xor	(hl)
PSET1:
	ld	(hl),a
	pop	hl
	ret


;POINT() function
F_POIN:
	call	GETGXY
	call	POINT
	jp	I1TOFA


;skip @
SKIPAT:
	call	SKIPSP
	cp	'@'
	ret	nz
	jp	SKIPSPINC


;LINE command
C_LINE:
	call	SKIPAT
	cp	MINUS_
	ld	bc,(GRPX1)
	ld	de,(GRPY1)
	jr	z,LINE2P

LINE1P:
	call	GETGXY
	cp	MINUS_
	jp	nz,SNERR

LINE2P:
	push	bc		;x1
	push	de		;y1

	inc	hl
	call	GETGXY
	ld	(GRPX2),bc	;x2
	ld	(GRPY2),de	;y2
	ld	a,(COLOR1)
	jr	nz,NOTBOX
	ld	e,a
	call	SKIPSPINC
	cp	','
	jr	z,CHKBF

;color
	call	INT1ARG
	ld	a,(hl)
	cp	','
	jr	z,CHKBF
	ld	a,e

NOTBOX:
	call	SETATT
	pop	de		;y1
	pop	bc		;x1
	jr	LINE

;check 'B' and 'F'
CHKBF:
	call	SKIPSPINC
	cp	'B'
	jp	nz,SNERR

	ld	a,e
	call	SETATT

	call	SKIPSPINC
	cp	'F'
	jr	nz,BOX
	inc	hl		;for box fill
BOX:
	ld	(PROGAD),hl
	pop	de		;y1
	pop	bc		;x1
	jr	nz,LINEB
	jr	LINEBF


;input: bc=x1, de=y1, (GRPX2)=x2, (GRPY2)=y2, (ATTDAT)=attribute
;destroy: af,bc,de
_LINEBF:ds	LINEBF-_LINEBF
	org	LINEBF

	push	hl
	call	PUSHGXY
	call	SORTXY2

	ld	a,(SCREEN1)
	cp	02h
	jr	c,BF12

;line bf (graphic mode)
BFG:
	ld	a,(GRPX2)	;larger x
	jr	nz,BF4
	inc	a		;screen mode 3
BF4:
	and	07h
	inc	a
	ld	b,a
	xor	a
BFGLP1:
	scf
	rra			;right part mask
	djnz	BFGLP1

;	or	a
	sbc	hl,de
	inc	l
	push	hl		;l=lines

	ld	l,a		;right part mask

	ld	a,c		;smaller x
	and	07h
	ld	b,a
	ld	a,0ffh
	jr	z,BFGZ
BFGLP2:
	srl	a
	djnz	BFGLP2
BFGZ:
	ld	h,a		;left part mask

	push	hl
	call	GXY2AD		;b=0
	pop	de		;d=left part mask, e=right part mask
	pop	bc		;c=lines

	jp	BFG2


;by Bresenham's line algorithm
;input: bc=x1, de=y1, (GRPX2)=x2, (GRPY2)=y2, (ATTDAT)=attribute
;destroy: af
_LINE:	ds	LINE-_LINE
	org	LINE

	push	hl
	call	PUSHGXY
	push	de
	push	bc

	call	SORTY
	ld	bc,(GRPY3)	;start (smaller y)
	ld	hl,(GRPY2)	;end (larger y)
	or	a
	sbc	hl,bc		;dy
	jr	LINE2


;input: bc=x1, de=y1, (GRPX2)=x2, (GRPY2)=y2, (ATTDAT)=attribute
;destroy: af,bc,de
_LINEB:	ds	LINEB-_LINEB
	org	LINEB

	push	hl
	call	PUSHGXY
	call	SORTXY2
	ex	de,hl

;down-up-right-left
	push	bc
	call	LINE
	ld	(GRPY2),hl	;smaller y
	ex	de,hl
	call	LINE
	ld	bc,(GRPX2)	;larger x
	ex	de,hl		;larger y
	call	LINE
	pop	bc		;smaller x
	ld	(GRPX2),bc
	call	LINE
	jr	LINEEND2


;line bf (screen mode 1,2)
;hl=larger y, a=screen mode-1
BF12:
	ld	(GRPY2),de
	call	LINE
	rst	CPHLDE
	jr	z,LINEEND2

	push	hl
	ld	a,(SCREEN1)
	add	a,a
	add	a,a
	ld	hl,000ch
	jr	z,BF12Z
	ld	l,a
BF12Z:
	add	hl,de		;+12 or +4
	ex	de,hl
	pop	hl
	jr	BF12


LINE2:
	ex	de,hl
	ld	bc,(GRPX3)	;start (x)
	ld	hl,(GRPX2)	;end (x)
	or	a
	sbc	hl,bc		;dx
	ld	b,h
	ld	c,l

	jr	c,DXMNS		;if dx<0
;if dx>=0, hl=-hl=-|dx|
	xor	a
	sub	l
	ld	l,a
	sbc	a,h
	sub	l
	ld	h,a
DXMNS:
	dec	hl
	add	hl,de
	jp	nc,SLOPEX	;if |dy/dx|<=1

;if |dy/dx|>1
	ld	h,d		;error=dy
	ld	l,e

	exx			;bc'=dx, de'=dy, hl'=error
	ld	de,(GRPY3)
LINELP1:
	ld	bc,(GRPX3)
	call	PSETSXY

	exx
	call	SUB2DX		;error-=|dx|*2
	bit	7,h
	jr	z,ERRPLS

;if error<0
	add	hl,de		;error+=dy
	add	hl,de		;error+=dy
	ld	a,b
	exx
	ld	b,a
	call	INCGX		;increment or decrement X
	exx
;	jr	c,LINEEND

ERRPLS:
	exx
	call	INCGY		;increment Y
	ld	hl,(GRPY2)
	rst	CPHLDE
	jr	nc,LINELP1

LINEEND:
	pop	bc
	pop	de

LINEEND2:
	pop	hl
	ld	(GRPY2),hl
	pop	hl
	ld	(GRPX2),hl
	pop	hl
	ret


;PAINT command
C_PAIN:
	call	SKIPAT
	call	GETGXY
	push	bc		;X
	push	de		;Y
	ld	a,(COLOR1)
	call	z,INT1INC
	call	SETATT
	ld	a,(hl)
	cp	','
	ld	a,(BORDERA)
	jr	nz,PANZ
	call	INT1INC
	ld	(BORDERC),a
PANZ:
	ld	e,a
	jp	PAPOP


;paint used by OKHOTSK
;input: e=color code, (sp)=Y, (sp+2)=X
_PAPOP:ds	PAPOP-_PAPOP
	org	PAPOP

	ld	a,e
	call	SETBO
	pop	de		;Y
	pop	bc		;X
	jp	PAINT


;input: bc=x, de=y, (ATTDAT)=attribute, (BORDERA)=attribute, (BORDERC)=color
;destroy: af,bc,de
_PAINT:	ds	PAINT-_PAINT
	org	PAINT

	ld	a,(SCREEN1)
	dec	a
	ret	z		;screen mode 2
	push	hl
	push	af
	call	GXY2AD
	ld	d,a
	pop	af
	jp	m,PAINT1	;screen mode 1
PAMAIN:
	call	PADWN
	pop	hl
	ret

PAINT1:
	ld	d,0ffh
	jr	PAMAIN


PADWN:
	call	PALINE
	ret	z
	push	hl		;address
	push	de		;bit
	ld	bc,(PAWRK)
	push	bc
	call	PAUP2
	pop	hl
	ld	(PAWRK),hl
	pop	de		;bit
	pop	hl		;address

PADWN2:
	ld	a,(SCREEN1)
	or	a
	ld	e,19h
	jr	nz,PADWNG
	ld	e,01h
PADWNG:
	ld	a,h
	and	1fh
	cp	e
	jr	c,PADWNOK
	ld	a,l
	cp	0e0h
	ret	nc
PADWNOK:
	ld	bc,COLUMNS
	add	hl,bc

	xor	a
	ld	(PACNT),a
PADWNLP1:
	call	SRCHOK
	jr	c,CALLDWN
	push	hl		;address
	ld	hl,PACNT
	inc	(hl)
	push	de		;bit
	call	CHKFRE
	pop	de
	pop	hl
	push	hl
	push	de

;	ld	a,l
;	inc	a
;	and	1fh
;	jr	nz,PADWNNZ
;	bit	0,d
;	jr	nz,CALLDWN
;PADWNNZ:
;	call	INCGXPA

	call	SRCHNG
	jr	nc,PADWNLP1

CALLDWN:
	ld	a,(PACNT)
	or	a
	ret	z
PADWNLP2:
	pop	de		;bit
	pop	hl		;address
	dec	a
	ld	(PACNT),a
	jr	z,PADWN
	push	af
	call	PADWN
	pop	af
	jr	PADWNLP2


PAUP:
	call	PALINE
	ret	z
	push	hl		;address
	push	de		;bit
	ld	bc,(PAWRK)
	push	bc
	call	PADWN2
	pop	hl
	ld	(PAWRK),hl
	pop	de		;bit
	pop	hl		;address

PAUP2:
;if (hl - start address) < 0020h
	ld	a,(SCREEN1)
	and	02h		;0(screen mode 1) or 2(screen mode 3,4)
	ld	e,a
	ld	a,h
	and	1fh
	cp	e
	jr	nz,PAUPNZ1
	ld	a,l
	cp	20h
	ret	c
PAUPNZ1:
	ld	bc,0-COLUMNS
	add	hl,bc

	xor	a
	ld	(PACNT),a
PAUPLP1:
	call	SRCHOK
	jr	c,CALLUP
	push	hl		;address
	ld	hl,PACNT
	inc	(hl)
	push	de		;bit
	call	CHKFRE
	pop	de
	pop	hl
	push	hl
	push	de

;	ld	a,l
;	inc	a
;	and	1fh
;	jr	nz,PAUPNZ2
;	bit	0,d
;	jr	nz,CALLUP
;PAUPNZ2:
;	call	INCGXPA

	call	SRCHNG
	jr	nc,PAUPLP1

CALLUP:
	ld	a,(PACNT)
	or	a
	ret	z
PAUPLP2:
	pop	de		;bit
	pop	hl		;address
	dec	a
	ld	(PACNT),a
	jr	z,PAUP
	push	af
	call	PAUP
	pop	af
	jr	PAUPLP2


;paint 1 line
;input: hl=address, d=bit
;output: hl, d, z-flag(1=not painted)
;destroy: af,bc
PALINE:
	call	STOPESC
	call	CHKPA
	ret	z
	push	hl		;address
	push	de		;bit
	call	PAINTR
	ld	a,l
	and	1fh
	ld	l,a
	ld	h,d
	ld	(PAWRK),hl	;X_right
	pop	de		;bit
	pop	hl		;address
;	jr	PAINTL		;z-flag=0


;paint to left direction
;input: hl=address, d=bit
;output: hl, d, z-flag=0
;destroy: af,bc
PAINTL:
	ld	a,l
	and	1fh
	jr	nz,PALOK
	bit	7,d
	ret	nz

PALOK:
	call	DECGXPA
	call	CHKPA
	jr	nz,PAINTL
;	jp	INCGXPA


;increment x for paint
;input: hl=address, d=bit
;output: hl, d, z-flag=0
;destroy: af
INCGXPA:
	rrc	d
	jp	po,INCGXPA4	;screen mode 4
	rrc	d		;screen mode 1 or 3
INCGXPA4:
	ret	nc
	inc	hl
	ret


;paint to right direction
;input: hl=address, d=bit
;output: hl,d
;destroy: af,bc
PAINTR:
	ld	a,l
	inc	a
	and	1fh
	jr	nz,PAROK
	bit	0,d
	ret	nz

PAROK:
	call	INCGXPA
	call	CHKPA
	jr	nz,PAINTR
;	jp	DECGXPA


;decrement x for paint
;input: hl=address, d=bit
;output: hl, d
;destroy: af
DECGXPA:
	rlc	d
	jp	po,DECGXPA4	;screen mode 4
	rlc	d		;screen mode 1 or 3
DECGXPA4:
	ret	nc
	dec	hl
	ret


;check and paint
;input: hl=address, d=bit
;output: z(1=not painted)
;destroy: af
CHKPA:
	ld	a,(BORDERA)
	xor	(hl)
	and	d
	ret	z

	ld	a,(ATTDAT)
	xor	(hl)
	and	d
	xor	(hl)
	ld	(hl),a
	or	d		;d>0, reset z-flag
	ret


;search for paintable area
;input: hl=address, d=bit
;output: hl, d, c-flag(1=over,0=OK)
;destroy: af,e
SRCHOK:
	ld	a,l
	and	1fh
	ld	e,a
	ld	a,(PAWRK)	;address low (5 bits)
	cp	e
	ret	c

	jr	nz,SRCHOKNZ
	ld	a,(PAWRK+1)	;bit
	dec	a
	cp	d
	ccf
	ret	c

SRCHOKNZ:
	ld	e,(hl)
	ld	a,(BORDERA)
	xor	e
	and	d
	jr	z,SRCHOKZ

	ld	a,(ATTDAT)
	xor	e
	and	d
	ret	nz		;c-flag=0

SRCHOKZ:
	call	INCGXPA
	ld	a,l
	and	1fh
	jr	nz,SRCHOK
	ld	a,d
	rlca
	jr	nc,SRCHOK
	ret			;c-flag=1


;search for unpaintable area
;get unpaintable area
;input: hl=address, d=bit
;output: hl, d, c-flag(1=over,0=NG)
;destroy: af,e
SRCHNG:
	ld	a,l
	and	1fh
	ld	e,a
	ld	a,(PAWRK)	;address low (5 bits)
	cp	e
	ret	c

	jr	nz,SRCHNGNZ
	ld	a,(PAWRK+1)	;bit
	dec	a
	cp	d
	ccf
	ret	c

SRCHNGNZ:
	ld	e,(hl)
	ld	a,(BORDERA)
	xor	e
	and	d
	ret	z		;c-flag=0

	call	INCGXPA
	ld	a,l
	and	1fh
	jr	nz,SRCHNG
	ld	a,d
	rlca
	jr	nc,SRCHNG
	ret			;c-flag=1


;STR$() function
F_STR:
	call	CHKNUM
	call	FTOA
	push	hl
	ld	bc,0101h
	xor	a
	cpir
	sub	c
	pop	hl
	call	MAKESTR
	jp	INKYEND


;put characters (skip first character) (used by LIZARD)
_PUTSINC:ds	PUTSINC-_PUTSINC
	org	PUTSINC

	inc	hl

;put characters
;input: hl,(DEVICE)=0:CRT, 1:printer, 2:RS-232C, 80h-ffh:CMT
;destroy: af,hl,(bc,de,FAC1)
_PUTS:	ds	PUTS-_PUTS
	org	PUTS

	ld	a,(hl)
	or	a
	ret	z
	cp	22h		;double quotation mark
	ret	z
	call	PUTDEV
	jr	PUTSINC


;input: bc=candidate of string descriptor address
;       hl=string descriptor address to be checked
;	(GCWRK)=candidate of string data address to be comacted
;	stack=string descriptor address pointing to same data (endmark=0000)
;output: bc=string descriptor address to be compacted
;	 (GCWRK)=string data address to be comacted
;	 hl=next string descriptor address
;	 stack=string descriptor address pointing to same data (endmark=0000)
;destroy: af,de
GETSTR:
	push	hl
	ld	a,(hl)
	inc	hl
	inc	hl
	ld	e,(hl)
	inc	hl
	ld	d,(hl)
	inc	hl

	or	a
	jr	z,GETNOSTR2	;length=0

	push	hl
	ld	hl,(VARAD)
	rst	CPHLDE
	jr	nc,GETNOSTR	;in program area
	ld	hl,(STRAD)
	rst	CPHLDE
	jr	c,GETNOSTR	;in work area or have been compacted
	ld	hl,(GCWRK)
	rst	CPHLDE
	pop	hl
	jr	z,GETSTRZ
	jr	nc,GETNOSTR2	;de < candidate

;new candidate found
	ld	(GCWRK),de
	pop	bc
	pop	de		;return address

GETSTRLP:
	pop	af		;pushed address? (a=higher byte)
	push	af
	push	de		;return address
	or	a
	ret	z		;no data
	pop	de		;return address
	pop	af		;cancel
	jr	GETSTRLP

GETSTRZ:
	pop	af
	pop	de		;return address
	push	af
	push	de		;return address
	ret


;garbage collection
;destroy: af,bc,de,hl
_GC:	ds	GC-_GC
	org	GC

	ld	hl,(STREND)
	ld	(STRAD),hl

;	ld	hl,0000h
	ld	h,00h		;higher byte
GCLP1:
	push	hl
	ld	(GCWRK),hl
	ld	hl,(VARAD)

;normal variable
GCVAR:
	ld	de,(ARRAD)
	rst	CPHLDE
	jr	nc,GCARR
	inc	hl
	ld	a,(hl)
	rlca
	jr	c,GCVARS
	ld	de,0006h
	add	hl,de
	jr	GCVAR
GCVARS:
	inc	hl
	call	GETSTR
	jr	GCVAR


GETNOSTR:
	pop	hl
GETNOSTR2:
	pop	af	;cancel
	ret


;array variable
GCARR:
	ld	de,(FREEAD)
	rst	CPHLDE
	jr	nc,GCSDSC
	inc	hl
	ld	a,(hl)
	inc	hl
	ld	e,(hl)
	inc	hl
	ld	d,(hl)
	inc	hl
	rlca
	jr	c,GCARRS
	add	hl,de
	jr	GCARR

GCARRS:
	ex	de,hl
	add	hl,de
	ex	de,hl
	push	de
	ld	e,(hl)		;dimensions
	ld	d,00h
	inc	hl
	add	hl,de
	add	hl,de
GCARRLP:
	pop	de
	rst	CPHLDE
	jr	nc,GCARR
	ld	(TMP),de
	call	GETSTR
	ld	de,(TMP)
	push	de
	jr	GCARRLP

;temporary string descriptor
GCSDSC:
	ld	hl,STRDSC1
	call	GETSTR
;	ld	hl,STRDSC2
	call	GETSTR
;	ld	hl,STRDSC3
	call	GETSTR
;	ld	hl,STRDSC4
	call	GETSTR

	ld	hl,(GCWRK)
	ld	a,h
;	or	l
	or	a		;higher byte
	pop	de		;cancel
	ret	z
	push	de

;compaction
;input:	bc=string descriptor address
;	hl=string data address to be comacted

	ld	a,(bc)
	push	bc		;
	ld	b,00h
	ld	c,a
	add	hl,bc
	dec	hl
	ld	de,(STRAD)
	lddr
	ld	(STRAD),de
	inc	de

	pop	hl		;
GCLP2:
	inc	hl
	inc	hl
	ld	(hl),e
	inc	hl
	ld	(hl),d
	pop	hl
	ld	a,h
;	or	l
	or	a		;higher byte
	jp	z,GCLP1
	jr	GCLP2


;LEN() function
F_LEN:
	call	CHKSTR
SETI1A:
	ld	l,a
SETI1:
	call	I1TOF
SETTYP0:
	xor	a
	ld	(ARGTYP),a	;0=numeric
	ret


;ASC() function
F_ASC:
	call	STRARG2
	or	a
	jp	z,FCERR
	ld	a,(de)
	jr	SETI1A


;CHR$() function
F_CHR:
	call	CHKNUM
	call	FTOI1
	ld	b,e
	jp	INKYNZ


;LEFT$() function
F_LEFT:
	call	ARGSI1
	ld	c,00h
	jr	MID


;RIGHT$() function
F_RGT:
	call	ARGSI1
	ld	d,a		;
	sub	e
	jr	nc,RGTNC
	xor	a
RGTNC:
	ld	c,a
	ld	e,d		;
	ld	a,d
	jr	MID


;MID$() function
F_MID:
	call	ARGSI1
	ld	d,a		;length
	dec	e
	push	de		;d=length, e=2nd parameter-1
	inc	e
	jp	z,FCERR		;if 2nd parameter=0

	ld	hl,(PROGAD)
	ld	e,0ffh
	ld	a,(hl)
	cp	','
	call	z,ARGI1
	pop	bc		;b=length, c=2nd parameter-1
	ld	a,b

;input: STRDSC1=string, a=string length, c=2nd parameter-1, e=3rd parameter
MID:
	sub	c
	jr	nc,MIDNC
	xor	a
MIDNC:
	cp	e
	jr	c,MIDC
	ld	a,e
MIDC:
	ld	b,a		;output length
	call	GCCHECK
	ld	a,b		;output length
	ld	hl,(STRDSC1+2)
	ld	b,00h
	add	hl,bc
	call	MAKESTR
	ld	hl,(PROGAD)
	call	CHKRPAR
	jp	INKYEND


;VAL() function
F_VAL:
	call	STRARG2
	ld	hl,(STREND)
	inc	hl
	ex	de,hl

	ld	b,00h
	ld	c,a
	add	hl,bc
	rst	CPHLDE
	ld	a,(hl)
	push	af
	push	hl
	jr	z,VALZ
	xor	a
VALZ:
	ld	(hl),a
;	or	a
	sbc	hl,bc
	call	ATOF
	pop	hl
	pop	af
	ld	(hl),a
	jp	SETTYP0


;FRE() function
F_FRE:
	ld	hl,ARGTYP
	ld	a,(hl)
	or	a
	jr	z,FRENUM
	dec	a
	jp	nz,SNERR
;string
	ld	(hl),a		;=0
	ld	(STRDSC1),a	;=0
	call	GC
	ld	hl,(STRAD)
	ld	de,(STACK)
	jr	CALCFRE
;numeric
FRENUM:
	ld	hl,0-0006h	;for compatibility with N60-BASIC
	add	hl,sp
	ld	de,(FREEAD)
CALCFRE:
	or	a
	sbc	hl,de
	jp	I2TOF


;DIM command
C_DIM:
	call	CHKVAR

	ld	a,(hl)
	cp	'('
	jr	nz,NOTARR
	inc	hl

	push	hl		;program address
	call	SRCHARR
	jp	nc,DDERR
	call	SETARNM
	call	VARSIZE
	pop	hl		;program address
	push	de		;array address
	push	de		;array address
	push	bc		;element bytes

	ld	c,b		;b=0
DIMLP:
	push	bc		;dimensions
	call	INT2ARG
	pop	bc		;dimensions
	bit	7,d
	jp	nz,FCERR
	inc	c
	jp	z,OMERR		;over 255 dimensions
	inc	de		;+1

	pop	hl		;element bytes
	pop	af		;array address
	push	de		;size
	push	af		;array address
	push	hl		;element bytes

	ld	hl,(PROGAD)
	ld	a,(hl)
	inc	hl
	cp	','
	jr	z,DIMLP
	cp	')'
	jp	nz,SNERR
	call	MAKEARR

DIMNEXT:
	ld	hl,(PROGAD)
	dec	hl
	rst	ANADAT
	ret	z
	inc	hl
	cp	','
	ret	nz
	jr	C_DIM

NOTARR:
	ld	(PROGAD),hl
	call	SRCHVAR
	call	c,MAKEVAR
	jr	DIMNEXT


;get variable address
;input: bc=variable name, hl=program address
;output: de=variable address, (PROGAD)=next program address
;destroy: FAC1,FAC2,af,hl
GETVAR:
	push	bc
	ld	a,(hl)
	cp	'('
	jr	z,GETVARR
	ld	(PROGAD),hl
	call	SRCHVAR
	call	c,MAKEVAR
	pop	bc
	ret
GETVARR:
	call	GETARR
	pop	bc
	ret


;get array address
;input: bc=variable name, hl=program address of '('
;output: de=array address
;destroy: af,bc,hl
GETARR:
	inc	hl
	ld	(PROGAD),hl
	push	bc		;variable name
	call	SRCHARR
	jr	nc,ARROK

	push	de		;array address
	ld	hl,(PROGAD)
	push	hl		;program address
	call	DIMAUTO
	pop	hl		;program address
	ld	(PROGAD),hl
	pop	de		;array address
	inc	de
	inc	de
ARROK:
	ex	de,hl
	inc	hl
	inc	hl
	ld	b,00h
	ld	c,(hl)		;dimensions
	add	hl,bc
	add	hl,bc
	push	hl		;array address
	ld	a,c
	ex	af,af'		;dimensions
	push	af
	ld	c,b		;=0
	push	bc		;pointer
	push	hl		;array address
	ld	hl,0001h
	ld	(TMP),hl	;multiplied sizes

GETALP:
	ld	hl,(TMP)
	push	hl		;multiplied sizes
	ld	hl,(PROGAD)
	call	INT2ARG		;get suffix
	pop	hl		;multiplied sizes
	ld	(TMP),hl
	pop	hl		;array address
	push	de		;suffix
	ld	d,(hl)		;de=size of each dimension
	dec	hl
	ld	e,(hl)
	dec	hl
	ex	(sp),hl		;push array address, pop suffix
	rst	CPHLDE
	jp	nc,BSERR

	push	de		;size
	ld	de,(TMP)	;multiplied sizes
	call	MULINT2		;hl = multiplied sizes * suffix
	pop	bc		;size
	ex	de,hl
	pop	hl		;array address
	ex	(sp),hl		;push array address, pop pointer
	add	hl,de		;pointer += multiplied size * suffix
	push	hl		;pointer
	ld	hl,(TMP)	;multiplied sizes
	ld	d,b		;size
	ld	e,c
	call	MULINT2		;hl = multiplied size * size
	ld	(TMP),hl
	pop	hl		;pointer
	ex	(sp),hl		;push poniter, pop array address
	push	hl		;array address

	ex	af,af'		;dimensions
	dec	a
	ld	b,a
	ex	af,af'
	ld	hl,(PROGAD)
	ld	a,(hl)
	cp	','
	jr	nz,NOTCMM
	inc	hl
	ld	(PROGAD),hl
	inc	b
;	dec	b
;	jr	nz,GETALP
	djnz	GETALP
	jp	BSERR

NOTCMM:
	inc	b
	dec	b
	jp	nz,BSERR
	call	CHKRPAR

	pop	de		;array address
	pop	hl		;pointer
	pop	af
	ex	af,af'
	pop	de		;array address
	pop	bc		;variable name
	ld	a,c
	ld	b,h
	ld	c,l
	add	hl,hl		;*2
	add	hl,hl		;*4
	rlca
	jr	c,NCMMSTR
	add	hl,bc		;*5
NCMMSTR:
	add	hl,de
	inc	hl
	ex	de,hl
	ret


DIMAUTO:
	call	SETARNM
	push	de		;array address
	call	VARSIZE
	push	de		;array address
	push	bc		;element bytes

	ld	hl,(PROGAD)
	ld	c,b		;b=0
AUTOLP1:
	push	bc		;dimensions
	call	INT2ARG		;de=size
;	ld	a,d
;	or	a
;	jp	nz,BSERR
;	ld	a,e
;	ld	e,0bh		;de=10+1
;	cp	e
;	jp	nc,BSERR

	ld	de,000bh

	pop	bc		;dimensions
	inc	c
	bit	2,c
	jp	nz,BSERR	;over 3 dimensions

	ex	de,hl
	pop	de		;element bytes
	ex	(sp),hl		;push size, pop array address
	push	hl		;array address
	push	de		;element bytes


	ld	hl,(PROGAD)
	ld	a,(hl)
	inc	hl
	cp	','
	jr	z,AUTOLP1
	cp	')'
	jp	nz,SNERR
	call	MAKEARR
	ret


;set array name
;input: bc=name, de=array address
;output: de=array address (next of name)
;destroy: af,hl
SETARNM:
	ex	de,hl
	call	CHKSTK
	ld	(hl),b
	inc	hl
	ld	(hl),c
	inc	hl
	ex	de,hl
	ret


;check variable name
;input: hl=program address
;output: a=next character, bc=name, hl=next address
;destroy: f
CHKVAR:
	ld	a,(hl)
	inc	hl
	cp	' '
	jr	z,CHKVAR
	ld	b,a
	sub	'A'
	cp	'Z'-'A'+1
	jp	nc,SNERR
;	call	GETNAME
;	ret


;get variable name
;input: hl=2nd character address, b=1st character
;output: a=next character, bc=name, hl=next address
;destroy: f
GETNAME:
	ld	c,00h
	dec	hl
GETNMLP1:
	inc	hl
	ld	a,(hl)
	cp	' '
	jr	z,GETNMLP1
	cp	'$'
	jr	z,NAMESTR
	call	ALPNUM
	ret	nc
	ld	c,a
GETNMLP2:
	inc	hl
	ld	a,(hl)
	cp	' '
	jr	z,GETNMLP2
	call	ALPNUM
	jr	c,GETNMLP2
	cp	'$'
	ret	nz
NAMESTR:
	inc	hl
	set	7,c
	ret


;search for variable
;input: bc=variable name
;output: de=variable address, c-flag(1=not found)
;destroy: af,hl
SRCHVAR:
	ld	hl,(FNPAR)
	ld	a,l
	cp	b
	jr	nz,SRCHVMAIN
	ld	a,h
	cp	c
	ld	de,FNARG
	ret	z

SRCHVMAIN:
	ld	de,(VARAD)
SRCHVLP:
	ld	hl,(ARRAD)
	dec	hl
	rst	CPHLDE
	ret	c		;not found
	ld	a,(de)
	inc	de
	cp	b		;
	ld	a,(de)
	inc	de
	jr	nz,VARNEXT	;
	cp	c
	ret	z		;found
VARNEXT:
	rlca			;c-flag=1 if string
	ccf
	ld	hl,6-2
	adc	hl,de
	ex	de,hl
	jr	SRCHVLP


;make variable
;input: bc=variable name, de=variable address
;output: de=de+2
;destroy: af,bc,hl
MAKEVAR:
	push	bc		;variable name
	push	de		;variable address

	ld	de,0006h
	bit	7,c
	jr	nz,MAKEVSTR	;numeric=7, string=6
	inc	e
MAKEVSTR:
	ld	hl,(FREEAD)
	ld	bc,(ARRAD)
	or	a
	sbc	hl,bc
	ld	b,h
	ld	c,l

	ld	hl,(FREEAD)

	add	hl,de
	push	de		;numeric=7, string=6
	call	CHKSTK
	pop	de		;numeric=7, string=6
	ld	(FREEAD),hl
	ld	hl,(ARRAD)
	add	hl,de
	ld	(ARRAD),hl

	ld	a,b
	or	c
	jr	z,NOLDDR
	ld	hl,(FREEAD)
	push	hl		;
;	or	a
	sbc	hl,de
	dec	hl
	pop	de		;
	dec	de
	lddr
NOLDDR:
	pop	hl		;variable address
	pop	bc		;variable name
	ld	(hl),b
	inc	hl
	ld	(hl),c
	inc	hl
	push	hl		;variable address

	ex	de,hl
	ld	hl,ZERO
	call	VARSIZE
	ldir
	pop	de		;variable address
	ret


;get variable element size
;input: bc=variable name
;output: bc=5(numeric) or 4(string)
;destroy: f
VARSIZE:
	bit	7,c
	ld	bc,0005h
	ret	z
	dec	c
	ret


;search for array variable
;input: bc=variable name
;output: de=array address, c-flag(1=not found)
;destroy: af,hl
SRCHARR:
	ld	de,(ARRAD)
SRCHALP:
	ld	hl,(FREEAD)
	dec	hl
	rst	CPHLDE
	ret	c		;not found
	ld	a,(de)
	inc	de
	cp	b		;
	ld	a,(de)
	inc	de
	jr	nz,ARRNEXT	;
	cp	c
	ret	z		;found
ARRNEXT:
	ex	de,hl
	ld	e,(hl)
	inc	hl
	ld	d,(hl)
	inc	hl
	add	hl,de
	ex	de,hl
	jr	SRCHALP


;make array
;input: c=dimensions, hl=program address
;pushed: size*[1,255], array address, element bytes
;destroy: af,bc,de,hl
MAKEARR:
	ld	(PROGAD),hl
	pop	hl		;return address
	ld	(TMP),hl
	pop	de		;element bytes
	pop	hl		;array address
	inc	hl
	inc	hl
	ld	(hl),c		;dimensions
	inc	hl
	ex	de,hl

	ld	b,c
MKALP:
	ex	(sp),hl		;push element bytes, pop size
	ex	de,hl
	ld	(hl),e
	inc	hl
	ld	(hl),d
	inc	hl
	ex	(sp),hl		;push array address, pop element bytes
	push	bc		;dimensions
	call	MULINT2		;element bytes *= each dimension size
	pop	bc		;dimensions
	ld	a,d
	or	e
	jp	nz,BSERR	;for compatibility with N60-BASIC
	pop	de		;array address
	djnz	MKALP

	push	hl		;element bytes
	ex	de,hl
	ld	h,b
	ld	l,c
	add	hl,hl
	inc	hl
	add	hl,de
	jp	c,OMERR

	push	hl		;dimensions*2 + element bytes + 1
	ld	de,(FREEAD)
	inc	de		;+2(total bytes)
	inc	de
	inc	de		;+2(name)
	inc	de
	add	hl,de
	jp	c,OMERR
	call	CHKSTK
	ld	(FREEAD),hl

	ld	a,c		;dimensions
	pop	de		;dimensions*2 + element bytes + 1
	pop	hl		;element bytes
	ex	(sp),hl		;push element bytes, pop array address

	ld	(hl),e
	inc	hl
	ld	(hl),d
	inc	hl
	inc	hl
	add	hl,bc
	add	hl,bc

	ld	(hl),b		;b=0
	ld	d,h
	ld	e,l
	inc	de
	pop	bc		;element bytes
	dec	bc
	ldir
	ld	hl,(TMP)	;return address
	jp	(hl)


;NEW command
C_NEW:
	call	CHKCLN
	jp	nz,SNERR
	ld	hl,EDIT
	push	hl

NEWRESSTK:
;destroy: af,bc,de,hl
	ld	hl,RESSTK
	push	hl
;	jp	NEW

;destroy: af,hl
NEW:
	ld	hl,(BASICAD)
	xor	a
	ld	(hl),a
	inc	hl
	ld	(hl),a
	inc	hl
	ld	(hl),a
	inc	hl
	ld	(VARAD),hl
	ret


;check free size
;output: de=free size
;destroy: f,hl
CHKFRE:
	ld	hl,(FREEAD)

;check stack size
;hl <= stack - 003bh?
;input: hl=address
;output: de=free size
;destroy: f
CHKSTK:
	ex	de,hl
;	ld	hl,0-003bh+1
	ld	hl,0-0034h+1
	add	hl,sp
	sbc	hl,de		;c-flag=1
	jp	c,OMERR
	ex	de,hl
	ret


;reset stack and variables
;destroy: af,bc,de,hl
_RESSTK:ds	RESSTK-_RESSTK
	org	RESSTK

	pop	bc		;return address
	ld	sp,(STACK)
	push	bc		;return address
	ld	a,0ffh
	ld	(STOPAD+1),a
	ld	hl,(VARAD)
	ld	(ARRAD),hl
	ld	(FREEAD),hl
	call	HOOKRES
RSTRZ:
	ld	hl,(BASICAD)
	ld	(DATAAD),hl
	ret


;RESTORE command
C_RSTR:
	call	CHKCLN
	ld	(PROGAD),hl
	jr	z,RSTRZ
	call	GETLN
	ld	(PROGAD),hl
	call	SRCHLN
	jp	nc,ULERR
	ld	(DATAAD),bc
	ret


;STOP command
C_STOP:
	call	CHKCLN
	jp	nz,SNERR
	pop	af		;cancel return address

STOP1:
	ld	a,h
	cp	0fah
	jr	nc,STOPNC
STOP2:
	ld	(STOPAD),hl
STOP3:
	ld	hl,(SCREEN2)	;l=(SCREEN2),h=(SCREEN3)
	ld	(STOPSC2),hl	;(STOPSC2)=l,(STOPSC3)=h
	ld	hl,(LINENUM)
	ld	(STOPLN),hl
STOPNC:
	ei
	xor	a
	ld	(STOPFLG),a

	call	PRTNLX
	call	CHGTXT
	ld	hl,BREAK
	call	PUTS
	jp	PRTLNUM

BREAK:
	db	07h, "Break", 00h


;END command
C_END:
	pop	af		;cancel stack
	jp	EDIT


;CONT command
C_CONT:
	pop	af		;cancel return address

	ld	a,h
	cp	0fah
	jp	c,CNERR

	ld	hl,(STOPAD)
	ld	a,h
	cp	0fah
	jp	nc,EDIT		;if direct command mode

	sbc	a,a		;c-flag=1
	ld	(STOPAD+1),a	;a=ffh

	ex	de,hl
	ld	hl,(STOPLN)
	ld	(LINENUM),hl
	ld	hl,(STOPSC2)	;l=(STOPSC2),h=(STOPSC3)
	ld	a,l
	call	CHGACT
	ld	a,h
	call	CHGDSP
	ex	de,hl

	jp	INTPRT


;CLEAR command
_C_CLR:	ds	C_CLR-_C_CLR
	org	C_CLR

	call	CHKCLN
	jp	z,RESSTK
	call	INT2ARG
	bit	7,d
	jp	nz,FCERR
	push	de		;1st parameter
	ld	de,(STREND)
	ld	a,(hl)
	cp	','
	jr	nz,CLRNZ
	call	NARGMOINC
	call	FTOI
	dec	de
	ld	hl,(USREND)
	rst	CPHLDE
	jp	c,FCERR

CLRNZ:
	ex	de,hl
	pop	bc		;1st parameter

;input: bc=1st parameter, hl=2nd parameter-1
CLRMAIN:
	push	hl		;2nd parameter-1
	or	a
	sbc	hl,bc
;	jp	c,OMERR
	jr	c,CLRC
	ex	de,hl
	ld	hl,(VARAD)
	ld	bc,0038h
	add	hl,bc
	ex	de,hl
	rst	CPHLDE
CLRC:
	jp	c,OMERR
	ld	(STACK),hl
	pop	hl		;2nd parameter-1
	ld	(STREND),hl
	ld	(STRAD),hl
	jp	RESSTK


;NEXT command
C_NEXT:
	pop	af		;cancel return address
	ld	b,00h
NEXTLP:
	call	CHKCLN
	call	nz,CHKVAR
	ld	(PROGAD),hl

NEXTVLP:
	call	SRCHSTK
	jp	z,NFERR		;not found or gosub identifier found
	inc	sp		;identifier

	pop	de		;variable address

	ld	a,b
	or	a
	jr	z,NEXTOK

	dec	de
	ld	a,(de)
	cp	c
	jr	nz,NEXTNZ
	dec	de
	ld	a,(de)
	inc	de
	inc	de
	cp	b
	jr	z,NEXTOK
NEXTNZ:
	ld	hl,0010h	;STEP(6)+TO(6)+line number(2)+program address(2)
	add	hl,sp
	ld	sp,hl
	jr	NEXTVLP

NEXTOK:
	call	POPF1		;STEP value
	call	PUSHF1		;STEP value

	push	de		;variable address
	ex	de,hl
	call	SETADDF
	pop	de		;variable address
	push	de		;variable address
	ld	hl,FAC1
	call	SETF

	pop	de		;variable address

	call	POPF1		;STEP value
	call	POPF2		;TO value
	call	PUSHF2		;TO value
	call	PUSHF1		;STEP value

	push	de		;variable address

	rst	CHKSGN
	ex	de,hl		;variable address
	call	SETF1		;no change in z,c-flag
	jr	z,STEP0		;if STEP value = 0
	call	nc,EXFAC	;if STEP value > 0
	call	CMPF
	jr	c,NEXTEND	;for-next loop end

NEXTCONT:
	ld	hl,000eh	;variable address(2)+STEP(6)+TO(6)
	add	hl,sp
	ld	e,(hl)
	inc	hl
	ld	d,(hl)
	ld	(LINENUM),de
	inc	hl
	ld	e,(hl)
	inc	hl
	ld	d,(hl)
	ld	(PROGAD),de
	ld	a,FOR_
	push	af		;identifier
	inc	sp
	jp	INTPEND

STEP0:
	call	CMPF
	jr	nz,NEXTCONT

NEXTEND:
	ld	hl,0012h	;variable address(2)+STEP(6)+TO(6)+line number(2)+program address(2)
	add	hl,sp
	ld	sp,hl

	ld	hl,(PROGAD)
	ld	a,(hl)
	cp	','
	jp	nz,INTPEND
	inc	hl
	ld	b,a		;not variable name
	jp	NEXTLP


;+ operator
O_PLS:
	ld	a,(ARGTYP)
	or	a
	jr	z,ADDF

PLSSTR:
	cp	03h		;string and string
	jp	z,ADDSTR
	jp	TMERR


;FAC1=FAC1+1
;destroy: FAC2,af,bc,de,hl
INCF1:
	ld	hl,PLSONE

;set FAC2 and add
;input: FAC1, hl=float address
SETADDF:
	call	SETF2
	jr	ADDF


;- operator
O_MNS:
	call	CHKNUM
;	call	SUBF
;	ret


;FAC1 = FAC1 - FAC2
;input: FAC1,FAC2
;output: FAC1
;destroy: FAC2,af,b,de,hl
SUBF:
	ld	hl,FAC2+3
CHGSGNADD:
	ld	a,(hl)
	xor	80h
	ld	(hl),a
;	call	ADDF
;	ret


;FAC1 = FAC1 + FAC2
;input: FAC1,FAC2
;output: FAC1
;destroy: FAC2,af,bc,de,hl
ADDF:
	ld	hl,(FAC2+3)	;l=(FAC2+3),h=(FAC2+4)
	ld	a,h
	or	a
	ret	z		;FAC2=0

	push	hl
	set	7,l
	ld	(FAC2+3),hl

	ld	hl,(FAC1+3)	;l=(FAC1+3),h=(FAC1+4)
	push	hl
	set	7,l
	ld	(FAC1+3),hl

;	sub	h
;	jr	c,ADD1GT2	;|FAC1| > |FAC2|
;	jr	nz,ADD1LT2	;|FAC1| < |FAC2|

	call	CMPF
	jr	c,ADD1GT2	;|FAC1| > |FAC2|

ADD1LT2:
	call	EXFAC

	pop	de		;e=(FAC1+3),d=(FAC1+4)
	pop	hl		;l=(FAC2+3),h=(FAC2+4)

	ld	a,d
	or	a
	jr	nz,ADDCHKSGN
	ld	(FAC1+3),hl
	ret

ADD1GT2:
	pop	hl		;l=(FAC1+3),h=(FAC1+4)
	pop	de		;e=(FAC2+3),d=(FAC2+4)

ADDCHKSGN:
	ld	a,h
	sub	d
	ld	b,a		;

	ld	a,l
	push	af		;sign

	xor	e
	rlca
	push	af		;c-flag=0:same sign
	jr	nc,SAMESGN

	call	EXFAC
	call	NEGINT4
	call	EXFAC

SAMESGN:
	ld	c,00h		;guard/round/sticky bit
	ld	hl,(FAC2)
	ld	de,(FAC2+2)

ADDFLP1:
	ld	a,b
	sub	08h
	jr	c,EXPLT8
	ld	b,a

	ld	a,l
	and	3fh
	jr	nz,SETSBIT
	inc	c
	dec	c
	jr	z,NOSBIT
SETSBIT:
	ld	c,20h		;set sticky bit
NOSBIT:
	xor	l
	or	c
	ld	c,a
	ld	l,h
	ld	h,e
	ld	e,d
	pop	af		;same sign?
	push	af
	sbc	a,a		;00h or ffh
	ld	d,a
	jr	ADDFLP1

EXPLT8:
	add	a,08h
	jr	z,SAMEEXP

ADDFLP2:
	pop	af		;same sign?
	push	af
	rr	d
	rr	e
	rr	h
	rr	l

	ld	a,c
	rr	c
	and	20h		;sticky bit
	or	c
;	and	0e0h
	ld	c,a
	djnz	ADDFLP2

SAMEEXP:
	ld	(FAC2),hl
	ld	(FAC2+2),de

	ex	de,hl

	ld	hl,(FAC1)
;	ld	de,(FAC2)
	add	hl,de
	ld	(FAC1),hl
	ld	hl,(FAC1+2)
	ld	de,(FAC2+2)
	adc	hl,de
	ld	(FAC1+2),hl

	rla
	pop	hl		;l-bit0=same sign?
	xor	l
	rrca
	jr	nc,SEARCH1

	ld	hl,FAC1+4
	inc	(hl)
	jp	z,OVERR

;	scf			;c-flag=1
	dec	l		;dec hl,FAC1+3
	rr	(hl)
	dec	l		;dec hl,FAC1+2
	rr	(hl)
	dec	l		;dec hl,FAC1+1
	rr	(hl)
	dec	l		;dec hl,FAC1
	rr	(hl)

	ld	a,c
	rr	c
	and	20h		;sticky bit
	or	c
;	and	0e0h
	ld	c,a

SEARCH1:
	ld	hl,(FAC1)
	ld	de,(FAC1+2)
;	ld	a,h
;	or	l
;	or	d
;	or	e
;	or	c
;	jp	z,MULFZERO

ADDFLP3:
	bit	7,d
	jr	nz,CHKROUND
	sla	c
	adc	hl,hl
	rl	e
	rl	d
	ld	a,(FAC1+4)
	dec	a
	ld	(FAC1+4),a
	jr	nz,ADDFLP3
	jp	MULFZERO	;pop af/jp SETZERO

;round to the nearest even
CHKROUND:
	ld	a,l
	rrca
	ld	a,c
	adc	a,a
	jr	nc,ADDSGN	;guard bit=0
	jr	z,ADDSGN	;round bit=0 and sticky bit=0 and l=even

;round up
	inc	hl
	ld	a,h
	or	l
	jr	nz,ADDSGN
	inc	de
	ld	a,d
	or	e
	jr	nz,ADDSGN

	ld	hl,FAC1+4
	inc	(hl)
	jp	z,OVERR
;	ld	d,80h
	ld	h,e		;e=0
	ld	l,e		;e=0

ADDSGN:
	pop	af		;sign
	xor	d
	and	80h
	xor	d
	ld	d,a

	jp	SETI4


;FAC1 = FAC1 * 0.1
;input: FAC1
;output: FAC1
;destroy: FAC2,af,bc,de,hl
DIVF10:
	ld	hl,EM1
	jr	SETMULF

;0.1
EM1:
	db	0cdh, 0cch, 0cch, 4ch, 7dh


;FAC1 = FAC1 * 10
;input: FAC1
;output: FAC1
;destroy: FAC2,af,bc,de,hl
MULF10:
	ld	hl,TEN

;set FAC2 and multiply
;input: FAC1, hl=float address
SETMULF:
	call	SETF2
	jr	MULF

TEN:
	db	00h, 00h, 00h, 20h, 84h


;* operator
O_MUL:
	call	CHKNUM
;	call	MULF
;	ret


;FAC1 = FAC1 * FAC2
;input: FAC1,FAC2
;output: FAC1
;destroy: FAC2,af,bc,de,hl
MULF:

;result=bc-de-de'-hl'

	call	MULDIVSGN
	jp	z,SETZERO	;FAC2=0
	ret	c		;FAC1=0

	ld	bc,0000h
	ld	d,b
	ld	e,c

	push	af		;sign
	exx
	push	hl
	push	de
	push	bc

	ld	hl,(FAC1)
	ld	de,(FAC1+2)
;	or	a		;c-flag=0

	ld	b,20h
MULFLP:
;shift result and FAC1
	exx
	rr	b
	rr	c
	rr	d
	rr	e
	exx
	rr	d
	rr	e
	rr	h
	rr	l
	jr	nc,MULFNC1

;add
	exx
	ld	hl,(FAC2)
	add	hl,de
	ex	de,hl
	ld	hl,(FAC2+2)
	adc	hl,bc
	ld	b,h
	ld	c,l
	exx
MULFNC1:
	djnz	MULFLP

	exx
	ld	hl,FAC1+4
	jr	nc,MULFNC2

;shift result
	rr	b
	rr	c
	rr	d
	rr	e
	exx
	rr	d
;	rr	e
;	rr	h
;	rr	l
	exx
	inc	(hl)

MULFNC2:
	dec	(hl)		;;

;round to the nearest even
	exx
	ld	a,d
	adc	a,a		;check d-bit7
	jr	nc,NOROUNDUP
	or	e
	or	h
	or	l
	pop	bc
	pop	de
	pop	hl
	exx
	jr	nz,ROUNDUP
	ld	a,e
	rrca
	jr	nc,MULFEND

ROUNDUP:
	inc	de
	ld	a,d
	or	e
	jr	nz,MULFEND
	inc	bc
	ld	a,b
	or	c
	jr	nz,MULFEND
;	ld	b,80h
	inc	(hl)
	jp	z,OVERR

MULFEND:
	ld	(FAC1),de
	ld	(FAC1+2),bc

;exponent
	ld	a,(FAC2+4)
	add	a,(hl)
	jr	c,MULFC
	sub	80h		;;not 81h
	jr	nc,MULEXP

MULFZERO:
	pop	af		;cancel
	jp	SETZERO

MULFC:
	sub	80h		;;not 81h
MULFCHKOV:
	jp	nc,OVERR
MULEXP:
	ld	(hl),a		;FAC1+4
	jr	z,MULFZERO
	dec	l		;dec hl,FAC1+3
	pop	af		;sign
	rlca
	ld	a,(hl)
	rla
	rrca
	ld	(hl),a
	ret

NOROUNDUP:
	pop	bc
	pop	de
	pop	hl
	exx
	jr	MULFEND


;check signs and set sign bits of FAC1/2 for MULF/DIVF
;output: a-bit7(xor of FAC1,2 sign), c-flag(1:FAC1=0) z-flag(1:FAC2=0)
;destroy: f,hl
MULDIVSGN:
	ld	hl,FAC2+4
	ld	a,(hl)		;FAC2+4
	or	a
	ret	z
	ld	l,FAC1+4-(FAC1+4)/256*256
	ld	a,(hl)		;FAC1+4
	cp	01h
	ret	c
	dec	l
	ld	a,(hl)		;FAC1+3
	set	7,(hl)
	ld	l,FAC2+3-(FAC2+3)/256*256
	xor	(hl)		;FAC2+3, clear c-flag
	set	7,(hl)
	dec	h		;clear z-flag
	ret


;/ operator
O_DIV:
	call	CHKNUM
;	call	DIVF
;	ret


;FAC1 = FAC1 / FAC2
;input: FAC1,FAC2
;output: FAC1
;destroy: FAC2,af,bc,de,hl
DIVF:
	call	MULDIVSGN
	jp	z,DIV0ERR	;FAC2=0
	ret	c		;FAC1=0

	ld	hl,0000h
	ld	d,h
	ld	e,l

	push	af		;sign
	exx			;
	push	hl
	push	de

	call	CMPINT4
	jr	nc,DIVFNC
	ld	hl,FAC1+4
	dec	(hl)
	call	SLAF1

DIVFNC:
	ld	b,20h
DIVFLP:
	jr	c,DIVFC1
	call	CMPINT4
	jr	c,DIVFC2
DIVFC1:
	ld	hl,(FAC1)
	ld	de,(FAC2)
	or	a
	sbc	hl,de
	ld	(FAC1),hl
	ld	hl,(FAC1+2)
	ld	de,(FAC2+2)
	sbc	hl,de
	ld	(FAC1+2),hl
	or	a		;reset c-flag
DIVFC2:
	ccf
	exx			;
	adc	hl,hl
	rl	e
	rl	d
	exx			;

	call	SLAF1
	djnz	DIVFLP

	rla			;store c-flag
	call	CMPINT4		;no change in a
	pop	de
	pop	hl
	exx

	rra
	jr	c,DIVFUP
	rlca
	jr	c,DIVFNOUP

;round to the nearest even
	jr	nz,DIVFUP
	ld	a,l
	rrca
	jr	nc,DIVFNOUP

;round up
DIVFUP:
	inc	hl
	ld	a,h
	or	l
	jr	nz,DIVFNOUP
	inc	de
	ld	a,d
	or	e
	jr	nz,DIVFNOUP
	ld	hl,FAC1+4
	inc	(hl)
	jp	z,OVERR
	ex	de,hl		;hl<-0
	ld	de,8000h

DIVFNOUP:
	ld	(FAC1),hl
	ld	(FAC1+2),de

	ld	a,(FAC2+4)
	ld	b,a
	ld	hl,FAC1+4
	ld	a,(hl)
	sub	b
	jr	c,DIVF1LT2

;FAC1>=FAC2
	add	a,81h
	ccf
	jp	MULFCHKOV


;FAC1<FAC2
DIVF1LT2:
	jp	p,MULFZERO
	add	a,81h
	jp	MULEXP


;continued from CHKSGN
CHKSGN2:
	ld	a,(FAC1+3)
	rlca
	sbc	a,a		;00h or ffh
	ret	c		;ffh
	inc	a
	ret			;01h


;SGN() function
F_SGN:
	call	CHKNUM
	rst	CHKSGN
	ret	z
	inc	a
	jp	z,SETMNS1
	jp	SETPLS1


;(de)->(hl):4
;input: de=source address, hl=destination address
;destroy: b,de,hl
_DE2HL4:ds	DE2HL4-_DE2HL4
	org	DE2HL4

	ld	b,04h
DE2HL4LP:
	ld	a,(de)
	ld	(hl),a
	inc	de
	inc	hl
	djnz	DE2HL4LP
	ret


;ABS() function
F_ABS:
	call	CHKNUM
ABS:
	ld	hl,FAC1+3
	res	7,(hl)
	ret


;push FAC1
;input: FAC1
;output: sp=sp-6
;destroy: af,hl
PUSHF1:
	pop	af		;return address
	ld	hl,(FAC1+3)
	push	hl
	ld	hl,(FAC1+1)
	push	hl
	ld	hl,(FAC1-1)
	push	hl
	push	af		;
	ret


;pop FAC1
;input: none
;output: FAC1,sp=sp+6
;destroy: af,hl
POPF1:
	pop	af		;return address
	pop	hl
	ld	(FAC1-1),hl
	pop	hl
	ld	(FAC1+1),hl
	pop	hl
	ld	(FAC1+3),hl
	push	af		;
	ret


;pop FAC2
;input: none
;output: FAC2,sp=sp+6
;destroy: af,hl
POPF2:
	pop	af		;return address
	pop	hl
	ld	(FAC2-1),hl
	pop	hl
	ld	(FAC2+1),hl
	pop	hl
	ld	(FAC2+3),hl
	push	af		;
	ret


;copy FAC1 to FAC2
;input: FAC1
;output: FAC2
;destroy: hl
CPYFAC:
	ld	hl,(FAC1-1)
	ld	(FAC2-1),hl
	ld	hl,(FAC1+1)
	ld	(FAC2+1),hl
	ld	hl,(FAC1+3)
	ld	(FAC2+3),hl
	ret


;exchange FAC1 and FAC2
;input: FAC1,FAC2
;output: FAC1,FAC2
;destroy: hl
EXFAC:
	push	af
	call	PUSHF2
	call	CPYFAC
	call	POPF1
	pop	af
	ret


;print 1-byte integer in device (unsigned)
;input: a
;destroy: af,bc,de,hl
_PUTI1:	ds	PUTI1-_PUTI1
	org	PUTI1

	ld	h,00h
	ld	l,a
	jp	PUTI2


;INT() function
;(round toward minus infinity)
F_INT:
	call	CHKNUM
INT:
	call	CPYFAC
	call	GETINT
	ld	a,(FAC2+3)
	rlca
	ret	nc
	call	CMPF
	ret	z
	ld	hl,MNSONE
	jp	SETADDF


;convert string to float
;input: hl=address
;output: FAC1, hl=next address
;destroy: FAC2, af,bc,de
ATOF:
	push	hl
	call	SETZERO
	pop	hl
	call	SKIPSPA
	ret	z
	or	a
	ret	m		;reject PLUS_ and MINUS_ in VAL()
	cp	'&'
	jp	z,ATOFHEX

	call	ATOIF

	call	SKIPSPA
	ret	z
	inc	hl
	ld	c,00h		;after-dot counter
	cp	'.'
	jr	z,ATOFDOT
	or	20h
	cp	'e'
	jr	z,ATOFEXP
	dec	hl
	ret

ATOFLP1:
	call	CTOF
	jr	nc,CHKEXP
	inc	c
ATOFDOT:
	call	SKIPSPA
	jr	nz,ATOFLP1
	jr	ATOFEXP

CHKEXP:
	call	SKIPSPA
	jr	z,ATOFE0
	or	20h
	cp	'e'
	jr	nz,ATOFE0
	inc	hl
	jr	ATOFEXP

ATOFEXP:
	ex	de,hl
	call	PUSHF1
	ex	de,hl
	push	bc		;c=after-dot counter

	call	ATOIF
	jr	CALCEXP

ATOFE0:
	ex	de,hl
	call	PUSHF1
	ex	de,hl
	push	bc		;c=after-dot counter

	push	hl
	call	SETZERO
	pop	hl

CALCEXP:
	pop	bc		;c=after-dot counter
	push	hl		;address
	call	CPYFAC
	ld	l,c		;c=after-dot counter
	call	I1TOF
	call	SUBF21
	pop	de		;address

	call	CPYFAC
	call	POPF1
	push	de		;address

	ld	hl,FAC2+3
	ld	a,(hl)
	push	af		;E sign
	res	7,(hl)

ATOFLP2:
	ld	a,(FAC2+4)
	or	a
	jr	z,ATOFEND	;E0

	ld	a,(FAC1+4)
	or	a
	jr	z,ATOFEND	;=0

	call	PUSHF1
	call	SETMNS1
	call	ADDF
	call	CPYFAC
	call	POPF1

	pop	bc		;b=E sign
	push	bc
	call	PUSHF2
	ld	a,b
	rlca
	push	af
	call	nc,MULF10
	pop	af
	call	c,DIVF10
	call	POPF2

	jr	ATOFLP2

ATOFEND:
	pop	af
	pop	hl		;address
	ret

ATOFHEX:
	call	HEX
	ex	de,hl
	call	S2TOF
	ex	de,hl
	ret


;convert hexadecimal string to integer
;input: hl="&" address
;output: de=value, hl=next address
;destroy: af,c,de
HEX:
	call	SKIPSPAINC
	jp	z,SNERR
	or	20h
	sub	'h'		;
	jp	nz,SNERR

	ld	d,a		;=0
	ld	e,a		;=0
	ld	c,05h
HEXLP:
	push	de
	call	SKIPSPAINC
	pop	de
	ret	z
	cp	DEF_		;93h
	jr	z,HEXDEF
	sub	'0'
	cp	'9'-'0'+1
	jr	c,HEX0F
	or	20h
	sub	'a'-'0'
	cp	'f'-'a'+1
	ret	nc		;not hex
	add	a,0ah
HEX0F:
	dec	c
	jp	z,OVERR
	ex	de,hl
	add	hl,hl		;*2
	add	hl,hl		;*4
	add	hl,hl		;*8
	add	hl,hl		;*16
	add	a,l		;no carry
	ld	l,a
	ex	de,hl
	jr	HEXLP

HEXDEF:
	ld	d,e
	ld	e,0deh
	ld	a,0fh
	jr	HEX0F


;increment hl and skip space and check (STREND) for ATOF
SKIPSPAINC:
	inc	hl

;skip space and check (STREND) for ATOF
;input: hl=address
;output: a=data, hl=data address, z-flag(1=no data)
;destroy: f,de
SKIPSPA:
	ld	de,(STREND)
	inc	de
SKIPSPALP:
	rst	CPHLDE
	ret	z
	ld	a,(hl)
	or	a
	ret	z
	cp	' '
	ret	nz
	inc	hl
	jr	SKIPSPALP


;print "in *****"
;input: hl
;destory: af,bc,de,hl
_INLNUM:
	ds	INLNUM-_INLNUM
	org	INLNUM

	push	hl		;
	ld	hl,ERRIN
	call	PUTS
	pop	hl		;
;	jp	PUTI2


;print 2-byte integer in device (unsigned)
;input: hl
;destory: af,bc,de,hl
_PUTI2:	ds	PUTI2-_PUTI2
	org	PUTI2

	ld	de,PUTSINC
	push	de
;	jp	I2TOA


;convert 2-byte integer (unsigned) to string
;input: hl
;output: FAC3, hl=FAC3
;destroy: af,bc,de
_I2TOA:	ds	I2TOA-_I2TOA
	org	I2TOA

	call	I2TOF
	jp	FTOA


;convert float to string
;input: FAC1
;output: FAC3, hl=FAC3
;destroy: FAC1,FAC2,af,bc,de
_FTOA:	ds	FTOA-_FTOA
	org	FTOA

	ld	hl,FAC3
	ld	(hl),' '
	rst	CHKSGN
	jp	z,FTOAZERO	;a=0
	dec	a
	jr	z,FTOAPLS	;if FAC1>0
	ld	(hl),'-'
FTOAPLS:
	call	ABS
	ld	hl,E9
	call	SETCMPF
	jr	nc,LARGE	;FAC1 >= 1e9
	ld	hl,EM2
	call	SETCMPF
	jr	c,SMALL		;FAC1 < 1e-2

FTOA2:
	ld	b,0ah
FTOALP1:
	push	bc
	ld	hl,E8
	call	SETCMPF
	jr	nc,FTOAOK
	call	MULF10
	pop	bc
	djnz	FTOALP1
	push	bc

FTOAOK:
	ld	a,(FAC1+4)
	sub	97h
	ld	b,a
	ld	a,(FAC1)
FTOALP2:
	rlca
	djnz	FTOALP2
	call	c,INCF1		;round up

	call	FTOI4

	ld	hl,INTE8
	ld	de,FAC3+1
	pop	bc
	ld	c,b		;
	dec	b
FTOALP3:
	dec	b
	jp	m,FTOADOT
	call	DIVINT4
	add	a,'0'
	ld	(de),a
	inc	de
	jr	FTOALP3

FTOADOT:
	call	CHKINT4
	jr	z,FTOAEND

	ld	a,'.'
	ld	(de),a
	inc	de

	ld	a,c		;
	or	a
FTOALP4:
	call	nz,DIVINT4
	add	a,'0'
	ld	(de),a
	inc	de
	call	CHKINT4
	jr	nz,FTOALP4

FTOAEND:
	ld	(de),a		;a=0
	ld	hl,FAC3
	ret

SMALL:
	ld	bc,002dh	;c='-'
SMALLLP:
	inc	b
	push	bc
	call	MULF10
	ld	hl,E0
	call	SETCMPF
	pop	bc
	jr	c,SMALLLP
	jr	LARSMA

LARGE:
	ld	bc,002bh	;c='+'
LARGELP:
	inc	b
	push	bc
	call	DIVF10
	ld	hl,E2
	call	SETCMPF
	pop	bc
	jr	nc,LARGELP

LARSMA:
	push	bc		;
	call	FTOA2

LARLP2:
	inc	hl
	ld	a,(hl)
	or	a
	jr	nz,LARLP2

	ld	(hl),'E'
	inc	hl
	pop	bc		;
	ld	a,b
	ld	(hl),c
	inc	hl
	ld	bc,2f0ah	;b='0'-1, c=10
LARLP3:
	inc	b
	sub	c
	jr	nc,LARLP3
	add	a,c
	ld	(hl),b

FTOAZERO:
	inc	hl
	add	a,'0'
	ld	(hl),a
	inc	hl
	ex	de,hl
	xor	a
	jr	FTOAEND


;999999999.5
E9:
	db	0feh, 27h, 6bh, 6eh, 9eh

;99999999.95
E8:
	db	0feh, 1fh, 0bch, 3eh, 9bh

;9.999999995
E2:
	db	0ffh, 0ffh, 0ffh, 1fh, 84h

;0.9999999995
E0:
	db	0feh, 0ffh, 0ffh, 7fh, 80h

;0.00999999995
EM2:
	db	3ch, 0ah, 0d7h, 23h, 7ah

INTE8:
	db	00h,0e1h,0f5h, 05h	;10^8
	db	80h, 96h, 98h, 00h	;10^7
	db	40h, 42h, 0fh, 00h	;10^6
	db	0a0h,86h, 01h, 00h	;10^5
	db	10h, 27h, 00h, 00h	;10^4
	db	0e8h,03h, 00h, 00h	;10^3
	db	64h, 00h, 00h, 00h	;10^2
	db	0ah, 00h, 00h, 00h	;10^1
	db	01h, 00h, 00h, 00h	;10^0


;RND() function
_F_RND:	ds	RNDPLS-9-_F_RND
	org	RNDPLS-9
F_RND:
	call	CHKNUM
	rst	CHKSGN
	jr	z,RND0		;rnd(0)
	inc	a
	jr	z,RNDMNS	;rnd(-x)

;rnd(+x)
_RNDPLS:ds	RNDPLS-_RNDPLS
	org	RNDPLS

	ld	hl,RSEED
	call	SETF1
RNDMNS:
	call	DECINT4
	ld	hl,RNDFCT
	call	SETF2
	call	MULRND
RND0:
	ld	hl,(RSEED)
	ld	de,(RSEED+2)
	call	I4TOF
	ld	hl,FAC1+4
	ld	a,(hl)
	or	a
	ret	z
	sub	20h
	ld	(hl),a
	ret


RNDFCT:
	db	65h, 52h, 0fh, 00h


;RSEED = (FAC1 (integer) * FAC2 (integer)) & ff,ff,ff,ff,ffh
; for RND() function
;input: FAC1,FAC2 (not zero)
;output: RSEED
;destroy: FAC1,af,bc,de,hl
MULRND:
	ld	hl,ZERO
	ld	de,RSEED
	call	SETF
	xor	a
	ld	(FAC1+4),a

	exx			;
	push	hl
	push	de
	ld	hl,(FAC2)
	ld	de,(FAC2+2)
	exx			;

	ld	c,20h
MULRLP1:
	exx			;
	srl	d
	rr	e
	rr	h
	rr	l
	exx			;
	jr	nc,MULRNC

	ld	de,FAC1
	ld	hl,RSEED
	or	a
	ld	b,04h
MULRLP2:
	ld	a,(de)
	adc	a,(hl)
	ld	(hl),a
	inc	e		;inc de
	inc	l		;inc hl
	djnz	MULRLP2

MULRNC:
	call	SLAF1
	dec	c
	jr	nz,MULRLP1

	exx			;
	pop	de
	pop	hl
	exx			;
	ret


;push FAC2
;input: FAC2
;output: sp=sp-6
;destroy: af,hl
PUSHF2:
	pop	af		;return address
	ld	hl,(FAC2+3)
	push	hl
	ld	hl,(FAC2+1)
	push	hl
	ld	hl,(FAC2-1)
	push	hl
	push	af		;
	ret


;EXP() function
;exp(x)=2^n * e^a, n=[x/log2], a=x-n*log2
F_EXP:
	call	CHKNUM
EXP:
	call	PUSHF1		;x
	ld	hl,LOG2
	call	SETF2
	call	DIVF		;x/log2

	ld	hl,(FAC1+3)
	ld	a,h		;(FAC1+4)
	cp	89h
	jr	nc,EXPLARGE	;|x/log2|>=256

	call	INT		;n=[x/log2]
	call	FTOI
	push	de		;n
	ld	hl,LOG2
	call	SETMULF		;n*log2
	pop	de		;n
	call	POPF2		;x
	push	de		;n
	call	SUBF21		;a=x-n*log2

;1+a(1+a/2(1+a/3(1+...(1+a/10))))))))))
	call	CPYFAC
	call	SETPLS1

	ld	c,0ah
EXPLP:
	call	PUSHF2		;a
	push	bc
	call	MULF		;a*y
	call	CPYFAC
	pop	hl
	push	hl
	call	I1TOF
	call	EXFAC
	call	DIVF		;a*y/b
	call	INCF1		;a*y/b+1
	pop	bc
	call	POPF2		;a
	dec	c
	jr	nz,EXPLP

	pop	de		;-256<=n<=255
;	ld	a,(FAC1+4)	;=81h?
	ld	a,81h
	add	a,e
	ld	(FAC1+4),a	;exponent
	bit	7,d
	jr	nz,EXPNZ
	ret	nc		;0<=n<=126
EXPOV:
	jp	OVERR		;127<=n<=255
EXPNZ:
	dec	a
	ret	p		;-128=<n<=-1
EXPZERO:
	jp	SETZERO		;-256=<n<=-129

EXPLARGE:
	bit	7,l		;(FAC1+3)
	jr	z,EXPOV		;x/log2>=256
	call	POPF1
	jr	EXPZERO		;x/log2<=-256

;log(2)
LOG2:
	db	0f8h, 17h, 72h, 31h, 80h


;LOG() function
;log(x)=log(a*2^n)=log(a)+n*log(2)
;a'=(a-1)/(a+1)
;log(a)=2*(a'+a'^3/3+a'^5/5+...)=2a'(1+1/3*a'^2(1+3/5*a'^2(...(1+19/21*a'^2))))))))))
F_LOG:
	call	CHKNUM
LOG:
	rst	CHKSGN
	dec	a
	jp	nz,FCERR	;x<=0

	ld	hl,FAC1+4
	ld	a,(hl)
	sub	81h
	push	af		;n
	ld	(hl),81h	;a
	call	PUSHF1		;a
	call	INCF1
	call	POPF2		;a
	call	PUSHF1		;a+1
	call	SETMNS1
	call	ADDF		;a-1
	call	POPF2		;a+1
	call	DIVF		;a'=(a-1)/(a+1)
	call	PUSHF1		;a'
	call	CPYFAC
	call	MULF		;a'^2
	call	CPYFAC		;a'^2
	call	SETPLS1		;y

	ld	c,12h
LOGLP:
	call	PUSHF2		;a'^2
	push	bc
	call	MULF		;y*a'^2
	call	CPYFAC
	pop	hl		;l=c
	push	hl
	inc	l
	call	I1TOF
	call	EXFAC
	call	DIVF		;y*a'^2/(c+1)
	call	CPYFAC
	pop	hl
	dec	l
	push	hl
	call	I1TOF		;c-1
	call	MULF		;y*a'^2*(c-1)/(c+1)
	call	INCF1		;y=1+y*a'^2*(c-1)/(c+1)
	pop	bc
	call	POPF2		;a'^2
	dec	c
	jr	nz,LOGLP

	call	POPF2		;a'
	call	MULF		;a'*y
	ld	hl,FAC1+4
	ld	a,(hl)
	or	a
	jr	z,LOGZ
	inc	(hl)		;2*a'*y
	jp	z,OVERR
LOGZ:
	pop	bc		;b=n
	call	PUSHF1		;2*a'*y
	call	S1TOF
	ld	hl,LOG2
	call	SETMULF		;n*log(2)
	call	POPF2		;2*a'*y
	jp	ADDF		;n*log(2)+2*a'*y


;^ operator
O_POW:
	call	CHKNUM

	ld	hl,(FAC2+3)
	ld	a,h		;(FAC2+4)
	or	a
	jp	z,SETPLS1	;x^0

	rst	CHKSGN
	jr	z,POWZERO
	inc	a
	jr	z,POWNEG

;x^y=exp(ylog(x))
POWPOS:
	call	PUSHF2		;y
	call	LOG		;log(x)
	call	POPF2		;y
	call	MULF		;ylog(x)
	jp	EXP

;0^y
POWZERO:
	ld	a,l		;(FAC2+3)
	rlca
	ret	nc		;0^(positive)
	jp	DIV0ERR		;0^(negative)

;(negative)^y
POWNEG:
	ld	a,h		;(FAC2+4)
	cp	0a1h
	jp	nc,FCERR

	call	PUSHF1
	call	PUSHF2
	call	POPF1

	call	GETINT
	call	CMPF
	jp	nz,FCERR	;y=not integer
	call	FTOI4
	ld	a,(FAC1)	;even/odd
	ld	b,a

	call	POPF1
	push	bc		;
	call	ABS
	call	POWPOS
	pop	af		;
	rrca
	ret	nc		;y=even
	jp	NEGABSNZ	;y=odd


;COS() function
;cos(x)=sin(pi/2-|x|)
F_COS:
	call	CHKNUM
COS:
	call	NEGABS
	ld	hl,PIDIV2
	call	SETADDF		;pi/2-|x|
	jr	SIN

;pi/2
PIDIV2:
	db	0a2h, 0dah, 0fh, 49h, 81h


;SIN() function
;x-x^3/3!+x^5/5!-...=x(1-x^2/(2*3)(1-x^2/(4*5)(...(1-x^2/(12*13)))))))))))
F_SIN:
	call	CHKNUM
SIN:
	call	PUSHF1		;x
	ld	hl,PI2
	call	SETF2
	call	DIVF		;x/2pi
	call	GETINT		;int(x/2pi)
	ld	hl,PI2
	call	SETMULF		;int(x/2pi)*2pi
	call	POPF2		;x
	call	SUBF21		;x'=x-int(x/2pi)*2pi

	call	SINRANGE	;[-pi,+pi]
	call	SINRANGE	;[-pi/2,+pi/2]

	call	PUSHF1		;x'
	call	CPYFAC
	call	MULF		;x'^2

	call	CPYFAC
	call	SETPLS1		;y=1

	ld	b,0ch
SINLP:
	call	PUSHF2		;x'^2
	push	bc
	call	MULF		;x'^2*y
	pop	bc
	push	bc
	call	PUSHF1		;x'^2*y

	ld	a,b
	inc	b
	call	MULINT1		;hl=b(b+1)
	call	I2TOF		;hl->FAC1

	call	CPYFAC
	call	POPF1		;x'^2*y
	call	DIVF		;x'^2/b/(b+1)*y
	call	NEGABS		;-x'^2/b/(b+1)*y (y>0)
	call	INCF1		;y=1-x'^2/b/(b+1)*y

	pop	bc
	call	POPF2		;x'^2
	dec	b
	djnz	SINLP

	call	POPF2		;x'
	call	MULF
	ld	a,(FAC1+4)
	sub	81h
	ret	nz

;if |result|>=1
	ld	(FAC1),a	;=0
	ret

;[-pi/2,+pi/2]
SINRANGE:
	ld	hl,PIDIV2	;pi/2
	call	SETCMPF
	jr	nc,RANGENC
	ld	hl,FAC2+3
	set	7,(hl)		;-pi/2
	call	CMPF
	ret	nc
RANGENC:
	ld	hl,FAC2+4
	inc	(hl)		;+-pi

;FAC1=FAC2-FAC1
SUBF21:
	ld	hl,FAC1+3
	jp	CHGSGNADD

;pi*2
PI2:
	db	0a2h, 0dah, 0fh, 49h, 83h


;SQR() function
;y_n+1=(y_n + x/y_n)/2
F_SQR:
	call	CHKNUM
	rst	CHKSGN
	ret	z		;sqr(0)=0
	inc	a
	jp	z,FCERR		;sqr(-x)

	call	PUSHF1
	ld	hl,FAC1+4
	ld	a,(hl)
	sub	81h
	rra			;=sra a,c-flag=bit7
	add	a,81h
	ld	(hl),a		;exponent of y_0 = (exponent of x)/2

	ld	b,05h
SQRLP:
	call	CPYFAC
	call	POPF1		;x
	call	PUSHF1		;x
	push	bc
	call	PUSHF2		;;y
	call	DIVF		;x/y
	call	POPF2		;;y
	call	ADDF		;x/y+y
	ld	hl,FAC1+4
	dec	(hl)		;y=(x/y+y)/2
	pop	bc
	djnz	SQRLP
	call	POPF2
	ret


;TAN() function
;tan(x)=sin(x)/cos(x)
F_TAN:
	call	CHKNUM
	call	PUSHF1		;x
	call	COS		;cos(x)
	call	CPYFAC
	call	POPF1		;x
	call	PUSHF2		;cos(x)
	call	SIN		;sin(x)
	call	POPF2		;cos(x)
	jp	DIVF		;sin(x)/cos(x)


;input: bc=graphic X, de=graphic Y
;output: a=color
;destroy: f,bc,de,hl
POINT:
	call	GXY2AD
;	jp	GETCOLRMAIN

;input: (fdb3h-fdb4h)=attribute address, (fdb2h)=bit
;output: a=color
;destroy: f,hl
GETCOLRMAIN:
	ld	hl,(VRAMAD)
	ld	a,(SCREEN1)
	srl	a
	ld	a,(VRAMBIT)
	jr	nz,GETCOLR34
	jr	c,GETCOLR2

;screen mode 1
GETCOLR1:
	and	(hl)
	inc	a
	ret

;screen mode 2
GETCOLR2:
	and	(hl)
	ret	z

	ld	a,(hl)		;bit7-6
	rlca
	res	1,h
	xor	(hl)		;bit1
	and	81h
	xor	(hl)
	rlca
	and	07h
	inc	a
	ret

;screen mode 3
;screen mode 4
GETCOLR34:
	push	bc
	ld	b,a
	and	(hl)
	rlca
GETCOLRLP:
	rrca
	srl	b
	jr	nc,GETCOLRLP
	pop	bc
	ret	z		;screen mode 4
	inc	a		;screen mode 3
	ret


;SCREEN() function
;not called but jumped
F_SCRN:
	call	CHKLPAR
	call	INT1ARG
	push	af		;
	ld	a,(hl)
	cp	','
	jp	nz,SNERR
	call	INT1INC
	call	CHKRPAR
	pop	hl		;
	ld	l,e
	inc	h
	inc	l
	call	XY2AD
	ld	l,(hl)
	call	I1TOF
	jp	FNCRTN


;FN() function
;not called but jumped
F_FN:
	call	CHKVAR
	push	hl		;program address
	set	7,b
	call	SRCHVAR
	jp	c,UFERR
	pop	hl		;program address
	push	de		;function address
	call	FNCNUM
	pop	bc		;function address
	push	hl		;program address

;push old FN() argument
	ld	hl,(FNARG+3)
	push	hl
	ld	hl,(FNARG+1)
	push	hl
	ld	a,(FNARG)
	push	af
	ld	hl,(FNPAR)
	push	hl		;old FN() parameter

	push	bc		;function address
	ld	hl,FAC1
	ld	de,FNARG
	call	SETF
	pop	hl		;function address

	ld	e,(hl)
	inc	hl
	ld	d,(hl)
	inc	hl
	ld	a,(hl)
	inc	hl
	ld	h,(hl)
	ld	l,a
	ld	(FNPAR),hl

	ex	de,hl
	call	ARG

	pop	hl		;old FN() parameter
	ld	(FNPAR),hl

;pop old FN() argument
	pop	af
	ld	(FNARG),a
	pop	hl
	ld	(FNARG+1),hl
	pop	hl
	ld	(FNARG+3),hl
	pop	hl		;program address
	jp	CLRSTRD


;set FAC2 and compare
;input: FAC1,hl=float address
;output: c-flag, z-flag, FAC2
;destroy: af,bc,de,hl
SETCMPF:
	call	SETF2
;	jp	CMPF


;compare FAC2 and FAC1
;input: FAC1,FAC2
;output: c-flag,z-flag
;destroy: af,b,de,hl
CMPF:
	ld	de,FAC1+3
	ld	hl,FAC2+3
	or	h		;h>0, reset z-flag
	ld	a,(hl)
	rlca
	ld	a,(de)
	jr	c,CMPFNEG
	rlca
	ret	c

CMPFPOS:
	inc	de
	inc	hl
	ld	b,05h
CMPFLP:
	ld	a,(de)
	cp	(hl)
	ret	nz
	dec	hl
	dec	de
	djnz	CMPFLP
	ret

CMPFNEG:
	rlca
	ret	nc

;FAC1<0, FAC2<0
	ex	de,hl
	jr	CMPFPOS


;shift left arithmetic for FAC1
;input: FAC1
;output: FAC1,c-flag
;destroy: f,hl
SLAF1:
	ld	hl,FAC1
	sla	(hl)
	inc	hl
	rl	(hl)
	inc	hl
	rl	(hl)
	inc	hl
	rl	(hl)
	ret


;jump subroutine in table
;input: de=table address, a=data(00h-7fh or 80h-ffh)
;output: de=jump address, a=(hl)
;destroy: f
JPTBL:
	ex	de,hl
	add	a,a
	add	a,l
	ld	l,a
	jr	nc,JPTBLNC
	inc	h
JPTBLNC:
	ld	a,(hl)
	inc	hl
	ld	h,(hl)
	ld	l,a

	ex	de,hl
	ld	a,(hl)		;for ALIEN IN MAZE
	push	de
	ret


;skip space
;input: hl=program address-1
;output: a=data, hl=next address
;destroy: f
SKIPSPINC:
	inc	hl
SKIPSP:
	ld	a,(hl)
	cp	' '
	ret	nz
	jr	SKIPSPINC


;check colon and line end
;input: hl=program address
;output: a=(hl), z-flag(1= 00h or ":")
;destroy: af
CHKCLNINC:
	inc	hl
CHKCLN:
	ld	a,(hl)
	or	a
	ret	z
	cp	':'
	ret	z
	cp	' '
	ret	nz
	jr	CHKCLNINC


;check comma or argument
;input: hl=program address
;output: a=(hl), f(z=1:comma), hl=next address
CHKCMM:
	call	CHKCLN		;a=(hl)
	jp	z,MOERR
	inc	hl
	cp	' '
	jr	z,CHKCMM
	cp	','
	ret	z
	dec	hl
	ret


;check colon and comma
;input: hl=program address
;output: a=(hl), z-flag(1= 00h or ":" ), hl=hl+1(comma)
;destroy: af
CHKCLCM:
	call	CHKCLN
	ret	z
	cp	','
	jp	nz,SNERR
	inc	hl
	or	a		;reset z-flag
	ret


;bc=a*32
;input: a(<=16), b=0
;output: bc
;destroy: f
MUL32:
	add	a,a		;*2
	add	a,a		;*4
	add	a,a		;*8
	add	a,a		;*16
	rl	b
	add	a,a		;*32
	rl	b
	ld	c,a
	ret


;convert hiragana -> katakana (PC-6001)
;convert kana -> 00h (PC-6001A)
CNVKANA2:
	or	a
	ret	p
	push	hl
	push	bc
	ld	c,a
	call	CHK6001A
	ld	a,c
	pop	bc
	pop	hl
	jp	nz,CNVKANA
	xor	a
	ret


;check if PC-6001A
;output: z-flag(1=PC-6001A)
;destroy: af,b,hl
CHK6001A:
	ld	a,04h
	out	(93h),a		;CGROM ON
	ld	hl,6fd0h	;nn
	ld	b,0ch
	xor	a
CHK6001ALP:
	or	(hl)
	inc	hl
	djnz	CHK6001ALP
	ld	a,05h
	out	(93h),a		;CGROM OFF
	ret


;input: bc=screen mode X, de=screen mode Y, z-flag(1=mode5)
SXY2ADMAIN:
	push	bc
	push	de
	ld	a,(SCREEN1)
	ld	h,a		;(SCREEN1)
	srl	a
	jr	nz,SXY2AD34
	ld	a,03h
	jr	nc,SXY2AD1

;(VRAM)*256+int(de/3)*32+bc/2
;screen mode 2
SXY2AD2:
	ex	de,hl
	add	hl,hl
	add	hl,hl
	ex	de,hl
	call	DIV12
;a=0,4,8
	rrca
	rrca
;e=0-15,a=0,1,2
	srl	c
	rla
;c=0-31,e=0-15,a=0-5
	ld	b,a
	ld	a,40h
	jr	SXY2ADBIT

SXY2AD34:
	jr	c,SXY2AD4

;(VRAM)*256+de*32+bc/4
;screen mode 3
SXY2AD3:
	ld	a,81h
	sla	c

;(VRAM)*256+de*32+bc/8
;screen mode 4
SXY2AD4:
	ld	d,a		;81h=screen mode 3=81h, 01h=screen mode 4
	ld	a,c
	and	07h
	ld	b,a
	ld	a,d		;81h=screen mode 3=81h, 01h=screen mode 4
	srl	c
	srl	c
	srl	c
SXY2ADBIT:
	inc	b
SXY2ADLP:
	rrca
	djnz	SXY2ADLP

;(VRAM)*256+de*32+bc
;screen mode 1
SXY2AD1:
	ld	d,h		;(SCREEN1)
	ld	h,c
	ld	l,e
	inc	h
	inc	l
	call	XY2AD
	inc	d		;(SCREEN1)
	dec	d		;(SCREEN1)
	jr	nz,SXY2ADNZ
	dec	h
	dec	h
SXY2ADNZ:
	ld	(VRAMAD),hl
	ld	(VRAMBIT),a
	pop	de
	pop	bc
	ret


;ROM end
_4000H:	ds	4000h-_4000H

	end
