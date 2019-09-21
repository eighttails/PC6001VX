;Compatible BASIC for PC-6601
; by AKIKAWA, Hisashi  2017-2019

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
PUTC	equ	1075h
PUTCHR	equ	10aah
PATCH1	equ	10eah
SETCSR	equ	116dh
CSRON	equ	1179h
CSROFF	equ	1181h
Y2AD	equ	11b8h
XY2AD	equ	11cdh
DELLIN	equ	11dah
PUTCH	equ	1257h
SCRLU60	equ	1260h
SCRLD60	equ	12a9h
CHGMOD	equ	1390h
CHGDSP	equ	13edh
CHGACT	equ	140ch
AD2GAD	equ	1478h
CGROM	equ	14a0h
PUTCH2	equ	14afh
SETATT	equ	15c0h
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
SOUND2	equ	1bbeh
SOUND	equ	1bc5h
BELL	equ	1bcdh
JOYSTK	equ	1ca6h
PATCH2	equ	1cb4h
SETCNSL	equ	1d73h
SETC3	equ	1dbbh
CLS	equ	1dfbh
PLAY	equ	1eb3h
SCALE	equ	204bh
GETFN	equ	2539h
CMTSKP	equ	254eh
FOUND	equ	2576h
PRTFN	equ	2583h
INPOPN	equ	259ah
PRTOPN	equ	25a8h
WAIT	equ	25e5h
PUTDEV	equ	26c7h
PUTNL	equ	2739h
STOPESC	equ	274dh
INPT1	equ	28f9h
SETGXY	equ	2d13h
PSET	equ	2d47h
LINEBF	equ	2de4h
LINE	equ	2e1fh
LINEB	equ	2e35h
PAINT2	equ	2ef1h
PAINT	equ	2efah
PUTSINC	equ	30ceh
PUTS	equ	30cfh
GC	equ	310fh
RESSTK	equ	34f9h
CHKMOD	equ	3995h
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
C_CRCL	equ	4a72h		;CIRCLE
C_COL66	equ	6b4ah		;COLOR (mode5)
C_SCR66	equ	6b9bh		;SCREEN (mode5)
C_LIN66	equ	70c9h		;LINE (mode5)

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
PRTPOS	equ	0fa57h		;printer head position
DEVICE	equ	0fa58h		;output device
KEYFLG	equ	0fa5ah		;special key flags
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
LASTLIN	equ	0fda5h		;sc12:(CONSOL2)-(CONSOL3) sc34:(CONSOLE2)
CONSOL3	equ	0fda6h		;console 3rd parameter
FKEYSFT	equ	0fda7h		;shift key status for function key display
CSRY	equ	0fda8h		;cursor Y+1
CSRX	equ	0fda9h		;cursor X+1
CSRAD	equ	0fdaah		;cursor address
WIDTH	equ	0fdach		;X max+1
GRPX1	equ	0fdaeh		;graphic X, fdae-fdaf
GRPY1	equ	0fdb0h		;graphic Y, fdb0-fdb1
LINEST	equ	0fdb5h		;line connection status, (0=connect) fdb5-fdb7
PAGE1	equ	0fdb9h		;page1 data, fdb9-fde0
PAGE2	equ	0fde1h		;page2 data, fde1-fe08
PAGE3	equ	0fe09h		;page3 data, fe09-fe30
PAGE4	equ	0fe31h		;page4 data, fe31-fe58

INPTXY	equ	0fe59h		;INPUT command initial position
INPTX	equ	0fe5bh		;INPUT command end position
INPTPAG	equ	0fe5ch		;INPUT command page
INSMODE	equ	0fe5dh		;0=not insert mode, ff=insert mode
STOPSC2	equ	0fe5eh		;screen 2nd parameter -1 in stop
STOPSC3	equ	0fe5fh		;screen 3rd parameter -1 in stop
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
HOOKCLP	equ	0ff99h		;hook for changing link pointer
HOOKSTP	equ	0ffd8h		;hook for stop key
HOOK18H	equ	0ffdbh		;hook for 0018h
HOOK38H	equ	0ffe1h		;hook for 0038h
ASPECT	equ	0ffe6h		;aspect ratio for CIRCLE command

;constant
COLUMNS	equ	32
ROWS	equ	16

FOR	equ	081h
DATA	equ	083h
GOTO	equ	088h
RUN	equ	089h
GOSUB	equ	08ch
REM	equ	08eh
DEF	equ	093h
PRINT	equ	095h
PSET_	equ	09bh
SCREEN	equ	09fh
CMDLAST	equ	0aah
TAB	equ	0c2h
TO	equ	0c3h
FN	equ	0c4h
SPC	equ	0c5h
INKEY	equ	0c6h
THEN	equ	0c7h
NOT_	equ	0c8h
STEP	equ	0c9h
PLUS	equ	0cah
MINUS	equ	0cbh
AND_	equ	0cfh
OR_	equ	0d0h
ASTRSK	equ	0cch
GT_	equ	0d1h
EQ_	equ	0d2h
LT_	equ	0d3h
FNC1ST	equ	0d4h
USR	equ	0d7h
LEFT_	equ	0eah
MID_	equ	0ech
FNCLAST	equ	0f1h


;extended basic subroutine entry address
DISK	equ	4274h
INIDSK	equ	42b9h
SETFATP	equ	51f2h
DISK2	equ	5cfdh

;6601 subroutine entry address
CHGRAM	equ	016bh
SCALE2	equ	2030h
PUTI1	equ	397ah
TALK	equ	4000h
CALLINI	equ	601ch
SCRLU66	equ	63e2h
SCRLD66	equ	642fh
AD2GAD2	equ	65cch
SETBO66	equ	69f3h
SETC366	equ	6b7ah
SCRN66	equ	6b9eh
LINBF66	equ	70e7h
LINEB66	equ	7122h
PA66POP	equ	7161h
PAINT66	equ	716ah
LOADHEX	equ	7af9h

;6601 work area
REGF	equ	0fcf9h
REGA	equ	0fcfah
REGC	equ	0fcfbh
REGB	equ	0fcfch
REGE	equ	0fcfdh
REGD	equ	0fcfeh
REGL	equ	0fcffh
REGH	equ	0fd00h
REGPC	equ	0fd02h
REGSP	equ	0fd04h
BRKADR1	equ	0fd06h
BRKSAV1	equ	0fd08h
BRKADR2	equ	0fd09h
BRKSAV2	equ	0fd0bh
MONFLG	equ	0fd0eh		;b6=RAM/ROM, b5=printer, b4=echo, b3=CMT/RS-232C, b0=CRT

ATTDAT2	equ	0fe60h
BORDERA2	equ	0fe61h
PORTF0H	equ	0fe64h		;port f0h
MODE	equ	0fe65h		;mode-1
READRAM	equ	0fe8dh		;subroutine: read RAM data for PC-6601
OUTF0H	equ	0fea1h		;subroutine: output port f0h
LDIRRAM	equ	0fe93h		;subroutine: LDIR in RAM for PC-6601
CHGROM	equ	0fe98h		;subroutine: change 0000-7fff to BASIC ROM
LDDRRAM	equ	0fea5h		;subroutine: LDDR in RAM for PC-6601
PAWRK2	equ	0feb8h		;paint work, feb8-feb9
FKEYLEN	equ	0fec7h

;work area for FDD
FILES	equ	0fb31h
DRIVES	equ	0fb3bh
RDWRCHK	equ	0fe70h		;read or write or check FD
RMSECT	equ	0fe71h		;remaining sector
TRACK	equ	0fe72h
DRIVE	equ	0fe73h
SECTOR	equ	0fe74h
DRVBIT	equ	0fe75h		;bit4=drvieA,bit5=drvieB
RETRY	equ	0fe77h
NSECT	equ	0fe78h		;1-4
STATBF	equ	0fe79h		;FDC status, fe79-fe7f
ERRAD	equ	0ffe8h		;jump address in error of 10 counts

;6601 constant
CLMN66	equ	40
ROWS66	equ	20
CLAST66	equ	0c1h
FLAST66	equ	0f9h

;FD I/O port
FDCNTL	equ	0b1h		;FD control
FDCINT	equ	0b2h		;FDC interrupt
B2CNTL	equ	0b3h		;port b2 control?
FDBUF	equ	0d0h		;d0-d3
MOTORST	equ	0d4h		;FDD motor status
MOTOR	equ	0d6h		;FDD motor on/off
BUFSIZ	equ	0dah		;0e=use port d0, 0d=d0-d1, 0c=d0-d2, 0b=d0-d3
FDCSTAT	equ	0dch		;FDC status register
FDCDATA	equ	0ddh		;FDC data register
ADJUST	equ	0deh		;?


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
_JPHK1	:ds	JPHK1-_JPHK1
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
	cp	'A'
	jr	c,SETFKC
	cp	'Z'+1
	jr	nc,SETFKC
	add	a,'a'-'A'
SETFKC:
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
	ld	hl,4003h
	call	CHKEXTAB
	jr	nz,SKIPMENU	;if (4000h)='A' and (4001h)='B'
	call	CHKEXTCD
	ld	a,0a1h		;0000-5fff:BASIC ROM, 6000-7fff:external ROM
	out	(0f0h),a
	call	CHKEXT6000	;check 6000h and select mode
SKIPMENU:
	push	af		;a=mode, z=mode 5?

;How Many Pages?
PAGELP:
	ld	hl,HOWMANY
	call	PUTS
	call	INPT1
	rst	ANADAT
	jr	c,PAGEOK
	ld	hl,PAGES
	ld	e,(hl)
	ld	d,a		;if return, a=0
	jr	CHKPAGE

PAGEOK:
	ld	b,00h
	call	ATOI2
CHKPAGE:
	ld	a,d
	or	a
	jr	nz,PAGELP

	ld	a,e
	dec	a
	ld	hl,PAGES
	cp	(hl)
	jr	nc,PAGELP
	ld	(hl),e

	pop	af		;a=mode, z=mode 5?
	jp	z,MODE5
	cp	02h
	jr	c,MODE12
MODE34:
	ld	a,0a1h		;0000-5fff:BASIC ROM, 6000-7fff:external ROM
	out	(0f0h),a
	push	de
	call	SETTBL
	pop	de
MODE12:
	call	NEW
	ld	hl,ENDTBL-1
	add	hl,de		;d=0
	ld	h,(hl)
	ld	l,0ffh
	ld	(USREND),hl
	ld	bc,50
	call	CLRMAIN

;print start message
	ld	hl,SYSNAME
START:
	call	PUTS
	call	CHKFRE
	ld	hl,002fh
	add	hl,de
	call	PUTI2
	ld	hl,BFREE
PUTSEDIT:
	call	PUTS
	jp	EDIT


;initialize table
IOTBL93:
	db	0c0h		;8255 portA=mode2 portB=mode0
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

IOTBLF0:
F0:	db	11h		;0000-7fff:BASIC ROM
;F0:	db	71h		;0000-3fff:BASIC ROM, 4000-7fff:external ROM
F1:	db	0ddh		;8000-ffff:internal RAM for reading
F2:	db	55h		;0000-ffff:internal RAM for writing
F3:	db	0c2h		;wait & int control
F4:	db	00h		;INT1 address
F5:	db	00h		;INT2 address
F6:	db	03h		;timer count up
F7:	db	06h		;timer interrupt address
F8:	db	0c3h		;CG rom access control

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


	db	00h		;STOPFLG
	db	00h		;CMTSTAT
	db	00h, 00h, 00h
	db	00h		;CMTBUF
	db	' '		;ASTSTAT
	db	0ffh		;BAUD
	db	ROWS		;HEIGHT


BFREE:
	db	" Bytes free", 00h
ENDTBL:
	db	0f9h, 0dfh, 0bfh, 9fh
HOWMANY:
	db	"How Many Pages"
QMARK:
	db	"? ", 00h
SYSNAME:
	db	"60", 9ah, 0deh, 96h, 0fdh, "BASIC", 0dh, 0ah, 00h

PAGEDATA:
	db	80h		;fd91	VRAM address
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
	db	00h		;fda4
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
	db	0ffh		;fdb4	LINEST-1
	db	0ffh		;fdb5	LINEST
	db	0ffh		;fdb6
	db	0ffh		;fdb7


;change 0000-7fffh to RAM
;destroy: none
_CHGRAM:ds	CHGRAM-_CHGRAM
	org	CHGRAM

	push	af
	ld	a,0ddh		;0000-7fff:internal RAM for reading
	di
	jp	OUTF0H


FKEYLST:
	db	9ah, ' '	;COLOR
	db	0a3h, 22h	;CLOAD"
	db	88h, ' '	;GOTO
	db	97h, ' '	;LIST
	db	89h, 0dh	;RUN
	db	9fh, ' '	;SCREEN
	db	0a4h, 22h	;CSAVE"
	db	95h, ' '	;PRINT
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
	dw	F_COS,	F_SIN,	F_TAN,	F_PEEK,	F_LEN,	SNERR,	F_STR,	F_VAL
	dw	F_ASC,	F_CHR,	F_LEFT,	F_RHT,	F_MID,	F_POIN,	F_CSRL,	F_STCK
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
;destroys: af,bc,de,hl
PRTERR:
	ld	a,'?'
	call	PUTC
	ld	hl,ERRID
	ld	a,e
	cp	30h
	jr	c,PRTERRC
	ld	hl,ERRIDEX
PRTERRC:
	ld	d,00h
	add	hl,de
	ld	a,(hl)
	call	PUTC
	inc	hl
	ld	a,(hl)
	call	PUTC
	ld	hl,ERRBELL
	jp	PUTS


NFERR:
	ld	e,00h
	db	01h		;ld bc,****
C_TO:
C_TAB:
F_SPC:
C_THEN:
C_STEP:

;mode 5 (unimplemnted)
C_RENM:
C_BLD:
C_BSAV:
C_FLS:
C_LFLS:
C_LOAD:
C_MRG:
C_NAME:
C_SAVE:
C_FLD:
C_LSET:
C_RSET:
C_OPEN:
C_CLOS:
C_DSKO:
C_KILL:
F_PAD:
F_DSKI:
F_LOF:
F_LOC:
F_EOF:
F_DSKF:
F_CVS:
F_MKS:


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
	ld	sp,(STACK)

;	xor	a
;	ld	(STRDSC2),a
;	ld	(STRDSC3),a
;	ld	(STRDSC4),a
;	dec	a
;	ld	(STOPAD+1),a	;(STOPAD)>=fa00h

	ld	hl,STRDSC2
	ld	bc,0c00h
ERRORLP:
	ld	(hl),c
	inc	hl
	djnz	ERRORLP
	ld	(STOPAD),hl	;>=fa00h

	call	PRTNLXTXT
	call	PRTERR

PRTLNUM:
	call	PLSTOP

	ld	hl,(LINENUM)
	ld	a,h
	and	l
	inc	a
	call	nz,INLNUM	;if (LINENUM)<>0ffffh
;	jp	EDIT


;program edit and direct command mode
_EDIT:	ds	EDIT-_EDIT
	org	EDIT

	call	CLRKBF
EDIT2:
	ld	a,01h
	ld	(DEVICE),a
	ld	a,(PRTPOS)
	or	a
	call	nz,PUTNL
	call	PRTNLXALL
	ld	hl,OK
	call	PUTS

	ld	hl,0ffffh
	ld	(LINENUM),hl

EDITLP:
	call	INPT1
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
	push	hl		;buffer address
	push	de		;line number
	push	hl		;buffer address
	call	SRCHLN
	call	c,DELPRG	;c-flag=1
	pop	hl		;buffer address
	inc	(hl)
	dec	(hl)
	jr	nz,INSPRG
	jr	c,PRGEND	;
	jp	ULERR

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
	pop	hl		;buffer address

	ld	c,a		;b=0
	ldir

PRGEND:
	call	RESSTK
	call	CHGLKP
	jr	EDITLP


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
	pop	bc		;address to be deleted
	scf
	ret


;change link pointer
;destroy: af,bc,de,hl
CHGLKP:
	call	HOOKCLP
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
	jr	z,SRCHEND2	;program end
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
	jr	nz,SRCHEND	;(hl,hl-1)-de
	scf
SRCHEND:
	dec	bc
	dec	hl
	ret

SRCHEND2:
	dec	hl
	jr	SRCHEND


;convert to intermediate language
;input: hl=INPBUF-1
CNVIL:
	ld	d,h
	ld	e,l

ILLP1:
	inc	de
	ld	a,(de)
	or	a
	jp	m,ILLP1		;kana
	inc	hl
	jp	z,ILEND
	cp	22h		;double quotation mark
	jr	z,ILDQ
	cp	'?'
	jr	z,ILQ
	cp	14h
	jr	z,ILGRP

	push	hl
	call	TOUPPER
	or	80h
	ld	(de),a

	ld	b,80h
	ld	hl,CMDNAME

ILLP2:
	push	de
	ld	a,(de)
	cp	(hl)
	jr	nz,ILLP5

;1st character matched
ILLP3:
	inc	hl
	ld	a,(hl)
	rlca
	jr	c,ILFOUND

	ld	a,b
	cp	GOTO
ILLP4:
	inc	de
	ld	a,(de)
	jr	nz,NOGOTO
	cp	' '
	jr	z,ILLP4
NOGOTO:
	call	TOUPPER
	cp	(hl)
	jr	z,ILLP3

ILLP5:
	inc	hl
	ld	a,(hl)
	rlca
	jr	nc,ILLP5
	pop	de
	inc	b

	ld	a,(MODE)
	cp	02h
	jp	nc,CNVIL66

	ld	a,b
	cp	FNCLAST+1
	jr	nc,ILNC
	cp	CMDLAST+1
	jr	nz,ILLP2
	ld	b,TAB
	jr	ILLP2

;not found
ILNC:
	ld	a,(de)
	and	7fh
	ld	b,a
	jr	ILPUT

ILDQ:
	ld	(hl),a

ILREM:
	ld	c,a		;double quotation mark or 0(REM)
	ld	b,01h		;double quotation counter

;REM,DATA,""
;loop until (de)=0 or (de)=c
ILLP6:
	inc	de
	inc	hl
	ld	a,(de)
	ld	(hl),a
	or	a
	jr	z,ILEND2
	cp	22h
	jr	nz,ILNZ
	inc	b
ILNZ:
	bit	0,b
	jr	nz,ILLP6
	cp	c
	jr	nz,ILLP6
	jr	ILLP1

ILQ:
	ld	(hl),PRINT	;95h
	jr	ILLP1

ILGRP:
	ld	(hl),a
	inc	de
	inc	hl
	ld	a,(de)
	ld	(hl),a
	jp	ILLP1

;found
ILFOUND:
	pop	af		;cancel
ILPUT:
	pop	hl
	ld	(hl),b
	ld	a,b
	sub	REM
	jr	z,ILREM
	cp	DATA-REM
	jp	nz,ILLP1

ILDAT:
	ld	bc,003ah	;b=0,c=':'
	jr	ILLP6

ILEND:
	ld	(hl),a		;a=0
ILEND2:
	inc	hl
	ld	(hl),a		;a=0
	inc	hl
	ld	(hl),a		;a=0
	ret


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

	cp	MINUS
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

	cp	REM
	jr	z,LISTREM
	cp	DATA
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
	cp	TO
	jp	nz,SNERR
	inc	hl
	call	NUMARGMO
	ex	de,hl
	call	PUSHF1		;TO value
	ex	de,hl
	call	CHKCLN		;a=(hl)
	ld	(PROGAD),hl
	call	z,SETPLS1	;no change in z-flag
	jr	z,STEP1
	cp	STEP
	jp	nz,SNERR
	inc	hl
	call	NUMARGMO
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
	ld	a,FOR		;identifier
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
	cp	CLAST66+1
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


;continued from C_RUN
;input: hl(=(STARTAD)), z(1=colon or 00h)
RUN2:
	jr	z,RUNLP
	ex	de,hl
	call	C_GOTO
	ld	h,b
	ld	l,c
	inc	hl

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

VAR:
	call	CHKVAR
	call	GETVAR
	ld	hl,(PROGAD)
	call	VARIN
	jr	INTPEND


;get a 1-byte integer for function
;input: hl=program address
;output: FAC1,e, hl=next address, a(0=ok)
;destroy: FAC2,f,bc
FNCI1:
	call	FNCNUM
;	jp	FTOI1


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


;convert float to 2-byte integer for USR() (-32768~32767)
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
;output: de=integer, hl=next address
;destroy: af,bc
GETLN:
	ld	b,00h
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


;run command
_C_RUN:	ds	C_RUN-_C_RUN
	org	C_RUN

	call	RESSTK
	call	CHKCLN
	ex	de,hl
	ld	hl,(STARTAD)
	jp	RUN2


;GOSUB command
C_GSUB:
	call	C_GOTO
	dec	hl
GSUBLP:
	call	CHKCLNINC
	jr	nz,GSUBLP

	ld	b,h
	ld	c,l

	call	CHKFRE
	ld	hl,(LINENUM)
	ex	(sp),hl
	push	bc		;program address
	ld	a,GOSUB
	push	af		;gosub identifier
	inc	sp
JPHL:
	jp	(hl)


;GOTO command
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
	ld	(PROGAD),hl
	pop	hl
	ld	(LINENUM),hl
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
	cp	GOSUB
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
	pop	af		;cancel return address
	jp	VAR


;ON command
C_ON:
	call	INT1ARG
	ld	a,(hl)
	ld	d,a		;
	or	04h
	cp	GOSUB		;88h(GOTO) or 8ch(GOSUB)
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
	ld	a,d		;
	cp	GOTO
	jr	z,C_GOTO
	jp	C_GSUB


;IF command
C_IF:
	call	NUMARGMO
	ld	a,(hl)
	cp	THEN
	jr	z,IFOK
	cp	GOTO
	jp	nz,SNERR
IFOK:
	ld	a,(FAC1+4)
	or	a
	jr	z,C_REM
;	call	SKIPSPINC
;	sub	'0'
;	cp	'9'-'0'+1

	rst	ANADAT

	jp	c,C_GOTO
IFEND:
	ld	(PROGAD),hl
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

	call	DEVNUM
PRTDEV:
	ld	(DEVICE),a
	rlca
	call	c,PRTOPN

PRTLP1:
	call	CHKCLN		;a=(hl)
	jr	z,PRTEOL
PRTLP2:
	cp	';'
	jr	z,PRTNEXT
	cp	','
	jr	z,PRTCMM
	cp	SPC
	jp	z,PRTSPC
	cp	TAB
	jp	z,PRTTAB

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

PRTEOL:
;	xor	a
;	ld	(GRPFLG),a
	call	PUTNL
	jr	PRTEND

PRTCMM:
	call	CMMMAIN
PRTNEXT:
	inc	hl
PRTNEXT2:
	call	CHKCLN
	jr	nz,PRTLP2
PRTEND:
	ld	(PROGAD),hl
	ld	a,(DEVICE)
	rlca
	ret	nc
	jp	WCLOSE

CMMMAIN:
	ld	a,(DEVICE)
	or	a
	jr	z,CMM0	;CRT
	dec	a
	jr	z,CMM1	;printer

;RS-232C,CMT
	ld	b,0eh
	jr	PUTSPLP

;CRT
CMM0:
	ld	a,(CSRX)
	sub	0fh
	jp	nc,PUTNL
CMMSP:
	neg
;	call	PUTSP
;	ret

;a>0
PUTSP:
	ld	b,a
PUTSPLP:
	ld	a,' '
	call	PUTDEV
	djnz	PUTSPLP
	ret

;printer
CMM1:
	ld	a,(PRTPOS)
	ld	b,08h
CMM1LP:
	sub	0eh
	jr	c,CMMSP
	djnz	CMM1LP
	jp	PUTNL

PRTSPC:
	inc	hl
	call	FNCNUMR
	push	hl
	call	FTOI1
	pop	hl
PRTSPC2:
	ld	a,e
PRTSPC3:
	or	a
	call	nz,PUTSP
	jr	PRTNEXT2

PRTTAB:
	inc	hl
	call	FNCNUMR
	push	hl
	call	FTOI1
	pop	hl

	ld	a,(DEVICE)
	or	a
	jr	z,PRTTAB0	;0=CRT
	dec	a
	jr	nz,PRTSPC2	;2,80-ff=RS-232C,CMT

;printer
PRTTAB1:
	ld	a,(PRTPOS)
	jr	TABCOMMON

;CRT
PRTTAB0:
	ld	a,(CSRX)
	dec	a
TABCOMMON:
	sub	e
	jr	nc,PRTNEXT2
	neg
	jr	PRTSPC3


;print new line in CRT (graphic and text screen)
PRTNLXALL:
	call	PRTNLX

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


;INPUT command
C_INPT:
	ld	a,h
	cp	0fah
	jp	nc,IDERR
	dec	hl
	ld	(STOPAD),hl
	inc	hl

	call	DEVNUM
	ld	(INPDEV),a
	ld	b,a		;
	or	a
	call	z,CHGTXT
	push	hl		;program address
	ld	a,(hl)
	cp	22h		;double quotation mark
	jr	nz,INPTMAIN

	ld	a,b		;
	or	a
	jp	nz,SNERR
	pop	af		;cancel program address
PROMPT:
	inc	hl
	ld	a,(hl)
	or	a
	jp	z,SNERR
	cp	22h		;double quotation mark
	jr	z,PROMPTEND
	call	PUTC
	jr	PROMPT
PROMPTEND:
	call	SKIPSPINC
	cp	';'
	jp	nz,SNERR
	inc	hl
	ld	(PROGAD),hl
	push	hl		;program address

INPTMAIN:
	ld	a,(INPDEV)
	or	a
	jp	nz,INPTEXT

	call	CUTLINE

	ld	hl,QMARK
	call	PUTS
	call	INPT1
	pop	de		;program address
	pop	bc		;return address
	jp	c,STOP3
	push	bc		;return address
	push	de		;program address

	ld	a,0ffh
	ld	(STOPAD+1),a

	inc	hl		;INPBUF
	ld	a,(INPTX)
	ld	b,a
	or	a
	jr	z,INPTLP2
	ld	a,14h
INPTLP1:
	cp	(hl)
	jr	nz,INPTNZ
	inc	hl
INPTNZ:
	inc	hl
	djnz	INPTLP1

INPTLP2:
	ld	(hl),b		;b=0
	dec	hl
	ld	a,(hl)
	cp	' '
	jr	z,INPTLP2

;analyze input buffer
INPTANA:
	ld	de,INPBUF
INPTLP3:
	ld	hl,(PROGAD)
	call	CHKVAR
	push	de		;input buffer
	call	GETVAR
	pop	hl		;input buffer

	bit	7,c
	jr	nz,INPTSTR

INPTNUM:
	push	de		;variable address
	ld	a,0ffh
	call	ATOF
	pop	de		;variable address
	ld	a,(INPBUF)
	or	a
	jr	z,INPTNEXT
	push	hl		;input buffer
	ld	hl,FAC1
	call	SETF
	pop	hl		;input buffer
	ld	a,(hl)
	or	a
	jr	z,INPTNEXT
	cp	','
	jr	z,INPTNEXT
	ld	a,(INPDEV)
	or	a
	jp	nz,FDERR
	ld	hl,REDO
	call	PUTS
	pop	hl		;program address
	ld	(PROGAD),hl
	push	hl		;program address
	jr	INPTMAIN

INPTNEXT:
	ex	de,hl
	ld	hl,(PROGAD)
	dec	hl
	rst	ANADAT
	cp	','
	jr	nz,INPTEND
	inc	hl
	ld	(PROGAD),hl

	ld	a,(INPBUF)
	or	a
	jr	z,INPTLP3
	ld	a,(de)
	inc	de
	cp	','
	jr	z,INPTLP3
	ld	a,(INPDEV)
	or	a
	jp	nz,ODERR
	ld	a,'?'
	call	PUTC
	jp	INPTMAIN

INPTEND:
	pop	af		;cancel program address
	ld	a,(de)
	cp	','
	ret	nz
	ld	hl,EXTRA
	jp	PUTS


INPTSLP1:
	inc	hl
INPTSTR:
	ld	a,(hl)
	cp	' '
	jr	z,INPTSLP1
	ld	c,00h		;double quotation mark count
	cp	22h		;double quotation mark
	jr	nz,INPTSNZ
	inc	hl
	inc	c
INPTSNZ:
	push	hl
	ld	b,00h

INPTSLP2:
	ld	a,c
	or	a
	ld	a,(hl)
	jr	nz,INPTCHKDQ
	cp	','
	jr	z,INPTSZ
INPTCHKZ:
	or	a
	jr	z,INPTSZ
	inc	hl
	inc	b
	jr	INPTSLP2

INPTCHKDQ:
	cp	22h		;double quotation mark
	jr	nz,INPTCHKZ
	dec	c
	inc	hl
	jr	INPTSLP2

INPTSZ:
	ex	(sp),hl		;push input buffer
	ld	a,b
	push	de		;variable address
	call	MAKESTR
	pop	de		;variable address
	ld	hl,STRDSC1
	ld	bc,0004h
	ldir
	pop	hl		;input buffer
	jr	INPTNEXT


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
	cp	DATA
	jr	z,READDATA
	cp	22h		;double quotation mark
	jr	z,READDQ
	cp	':'
	jr	z,READLP3
	cp	' '
	jr	z,READLP3
	sub	REM
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
	call	SKIPSPINC

	pop	de		;variable address
	pop	bc		;variable name
	bit	7,c
	jr	nz,READSTR

	or	a
	jr	z,READNUM
	cp	'&'
	jr	z,READNUM
	cp	'/'
	jr	z,READERR
	sub	'+'
	cp	'9'-'+'+1	;+,-.0123456789
	jr	nc,READERR
READNUM:
	push	de		;variable address
	ld	a,0ffh
	call	ATOF
	ex	(sp),hl		;push data address, pop variable address
	ex	de,hl
	ld	hl,FAC1
	call	SETF
	pop	hl		;data address
	jr	READEND

READSTR:
	ld	bc,0000h	;b=double quotation mark?, c=length
	cp	22h		;double quotation mark
	jr	nz,RDSNDQ
	inc	b
	inc	hl
RDSNDQ:
	inc	de
	inc	de
	ld	a,l
	ld	(de),a
	inc	de
	ld	a,h
	ld	(de),a
	dec	de
	dec	de
	dec	de

RDSLP2:
	ld	a,(hl)
	or	a
	jr	z,RDSEND
	bit	0,b
	jr	z,RDSCHK
	cp	22h		;double quotation mark
	jr	nz,RDSNEXT
	call	CHKCLNINC
	jr	z,RDSEND
	cp	','
	jr	z,RDSEND

READERR:
	ld	hl,(DATALN)
	ld	(LINENUM),hl
	jp	SNERR

RDSCHK:
	cp	','
	jr	z,RDSEND
	cp	':'
	jr	z,RDSEND

RDSNEXT:
	inc	hl
	inc	c
	jr	nz,RDSLP2
	jp	LSERR

RDSEND:
	ld	a,c
	ld	(de),a

READEND:
	ld	(DATAAD),hl
	ld	hl,(PROGAD)
	ld	a,(hl)
	cp	','
	ret	nz
	inc	hl
	jp	C_READ


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
;input: hl=program address
;output: FAC1, hl=next address
;destroy: af,bc,de,FAC2
NUMARGMO:
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


;convert lowercase letter to uppercase letter
;input: a
;output: a
;destroy: f
TOUPPER:
	cp	'z'+1
	ret	nc
TOUPPER2:
	cp	'a'
	ret	c
	sub	'a'-'A'
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
	jr	z,EQ
	jr	nc,GT
LT:
	rrca
EQ:
	rrca
GT:
	rrca
	jr	c,SETTRUE
;	jr	SETFALSE

SETFALSE:
SETZERO:
	ld	hl,ZERO
SETF1:
	ld	de,FAC1
SETF:
	ldi
	ldi
	ldi
	ldi
	ldi
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
;	ld	(FAC1),hl
;	ld	(FAC1+2),de
;	ret
	jp	SETI4


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
	jr	z,I2TOFZERO

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
	jr	I2TOFEND2


;convert float to 2-byte integer (-65535~65535)
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
	ld	(hl),0
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
;destroy: af,bc,hl
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

;FAC1=-abs(FAC1)
;destroy: f,hl
NEGABS:
	ld	hl,FAC1+3
	set	7,(hl)
	ret


;NEGABS with checking zero
;destroy: af,hl
NEGABS2:
	ld	a,(FAC1+4)
	or	a
	ret	z
	jr	NEGABS


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
	cp	FN
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
;	ld	(hl),0
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


MODESTR:
	db	0dh, 0ah, "MODE ",00h
PAGESTR:
	db	" PAGES", 0dh, 0ah, 00h
FILESTR:
	db	" FILES", 0dh, 0ah, 00h


;increment hl and get a 1-byte integer argument (0~255)
_INT1INC:ds	INT1INC-_INT1INC
	org	INT1INC

	inc	hl

;get a 1-byte integer argument (0~255)
;input: hl=program address
;output: FAC1,a,de=integer, hl=next address
;destroy: FAC2, f, bc
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
	call	NUMARGMO
	ld	a,(hl)
	cp	','
	jp	nz,SNERR
	push	hl
	call	FTOI
	pop	hl
	push	de		;
	inc	hl
	call	NUMARGMO
	call	FTOI1
	pop	hl		;
	ld	(hl),e
	ret


;get a 2-byte integer argument (-65535~65535)
;input: hl=program address
;output: FAC1,de=integer, hl=next address
;destroy: FAC2,af,bc
INTARG:
	call	NUMARGMO
	push	hl
	call	FTOI
	pop	hl
	ret


;get a 2-byte integer argument (-32768~32767)
;input: hl=program address
;output: FAC1,de=integer, hl=next address
;destroy: FAC2,af,bc
INT2ARG:
	call	NUMARGMO
	push	hl
	call	FTOI2
	pop	hl
	ret


;get two 2-byte integer arguments (-65535~65535)
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


;get two 1-byte integer arguments (0~255)
;input: hl=program address
;output: FAC1, bc=1st, de=2nd, hl=next address
;destroy: af
INT1ARG2:
	call	INT1ARG
	ld	a,(hl)
	cp	','
	jp	nz,SNERR
	push	de
	call	INT1INC
	pop	bc
	ret


;get two 2-byte integer arguments (-32768~32767)
;input: hl=program address
;output: FAC1, bc=1st, de=2nd, hl=next address
;destroy: af
INT2ARG2:
	call	INT2ARG
	ld	a,(hl)
	cp	','
	jp	nz,SNERR
	inc	hl
	push	de		;
	call	INT2ARG
	pop	bc		;
	ret


;convert character VRAM address to attribute address
;input: hl
;output: hl
;destroy: f (no change in c-flag)
CHR2ATT:
	bit	2,h
	jr	z,CHR2ATTZ
	dec	h
	dec	h
CHR2ATTZ:
	dec	h
	dec	h
	ret


;call SCRLUP or SCRLU66
;destroy: af
SCRLUP:
	call	CHKMOD
	jp	nz,SCRLU60
	jp	SCRLU66


;call SCRLD60 or SCRLD66
;destroy: af
SCRLDW:
	call	CHKMOD
	jp	nz,SCRLD60
	jp	SCRLD66


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


;output port 90h
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
	jr	z,RESSTOP
	call	CLICK
;	dec	b
;	jr	nz,NORMAL
	djnz	NORMAL

GRAPH:
	ld	b,a		;;
	sub	0fah
	jr	z,STOPKEY
	jr	nc,SPECIAL
	ld	a,14h
	jr	IKBF


;interrupt for reply to key query
INTGAM:
	push	af
	call	IN90H
	ld	(GMKYBUF),a
	pop	af
	ei
	ret


;normal key
NORMAL:
	cp	03h		;Ctrl-C
	jr	z,STOPKEY2
	cp	1bh
	jr	z,STOPKEY2
	ld	b,0efh		;;Ctrl-T->14h+0efh
	cp	7fh
	jr	nz,NOTDELKEY
	ld	a,08h		;DEL->ctrl-H, not necessary for emulator?

NOTDELKEY:
	ld	hl,KEYFLG
	bit	0,(hl)
	call	nz,TOUPPER
	jr	IKBF


;0ef6h: used by PORTOPIA
_IKBF:ds	IKBF-_IKBF
	org	IKBF

	ld	hl,KYBFIN
	ld	c,(hl)		;
	call	PUTKBF
;	jr	c,RESSTOP
	cp	14h
	jr	nz,RESSTOP

	ld	a,b		;;
	call	PUTKBF
	jr	nc,RESSTOP

	ld	hl,KYBFIN
	ld	(hl),c		;
	jr	RESSTOP


STOPKEY:
	call	HOOKSTP
	ld	a,03h
STOPKEY2:
	call	PUTKBF
	jr	SETSTOP


;fb	CAPS		bit0
;fc	SHIFT+PAGE	bit1
;fd	MODE		bit3
;fe	KANA		bit2
SPECIAL:
	cp	03h		;MODE key
	jr	nz,NOTMODE
	ld	a,08h
NOTMODE:
	ld	hl,KEYFLG
	xor	(hl)
	ld	(hl),a

	xor	a
	ld	(FKEYSFT),a

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
INT232LP:
	in	a,(81h)
	and	02h
	jr	z,INT232LP	;wait for RxRDY=1
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
	ld	h,0
GETCLP2:
	call	MODEKEY
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
	ld	(hl),0
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
	ret


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
;output: a,z
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
;destroy: a,(f,bc,de,hl)
_CLRKBF:ds	CLRKBF-_CLRKBF
	org	CLRKBF

	ld	a,(KYBFIN)
	ld	(KYBFOUT),a
	ret


;send key-query to sub CPU and get reply
;output: a (space-0-left-right-down-up-stop-shift)
;destroy: f
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
	cp	0ffh		;bit6=0?
	jr	z,STICKLP
	ret


;put a character in CRT
;input: a=character code
;destroy: none
_PUTC:	ds	PUTC-_PUTC
	org	PUTC

	push	af
	push	hl

	ld	hl,(GRPFLG)
	inc	l
	dec	l
	jr	nz,PUTGRP
	cp	20h
	jr	nc,PUTCNC
	push	de
	push	bc
	call	PUTCTL
	pop	bc
	pop	de
	jr	PUTCEND

PUTGRP:
	sub	30h
	ld	hl,GRPFLG
	ld	(hl),0
PUTCNC:
	call	PUTCHR

PUTCEND:
	pop	hl
	pop	af
	ret


;for graphic mode (screen3,4)
;a=character
PUTCHRG:
	ld	hl,(CSRY)
	call	PUTG
CTLRHT:
	ld	hl,(CSRAD)
	inc	hl
	jr	CHKNL

;put a character in CRT (no control code)
;input: a=character code
;destroy: af,hl
_PUTCHR:ds	PUTCHR-_PUTCHR
	org	PUTCHR

	ld	hl,(SCREEN1)
	bit	1,l
	jr	nz,PUTCHRG	;screen mode 3,4

	ld	hl,(CSRAD)
	call	PUT12

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


;10eah: to be patched by iP6plus
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


PUTCTL:
	cp	14h
	jr	z,CTLGRP
	cp	07h
	jp	z,BELL
	sub	0ah
	jr	z,CTLLF
	dec	a
	jr	z,CTLHOM	;0bh
	dec	a
	jp	z,CLS		;0ch
	ld	hl,(CSRY)	;l=y+1,h=x+1
	dec	a
	jr	z,CTLCR		;0dh
	sub	1ch-0dh
	jr	z,CTLRHT	;1ch
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


;convert VRAM address to text screen (x,y)
;input: hl=address
;output: h=x+1, l=y+1(0=error)
;destroy: af
AD2XY:
	push	de
	ld	de,COLUMNS

	bit	2,h
	jr	z,AD2XYZ

	ld	e,CLMN66
	dec	h
	dec	h
AD2XYZ:
	dec	h
	dec	h

	xor	a
AD2XYLP:
	inc	a
	sbc	hl,de
	bit	3,h
	jr	z,AD2XYLP
	add	hl,de
	inc	l
	ld	h,l
	ld	l,a
	pop	de
	ret

CTLHOM:
	ld	hl,(CONSOL1)	;l=(CONSOL1)

CTLCR:
	ld	h,1
	jr	SETCSR


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

	bit	2,h
	jp	nz,CSRREV66

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


Y2AD2:
	adc	a,02h
	ld	d,a
	pop	af
	ret


;get left edge address
;input: l=y+1
;output: de=VRAM address
;destroy: none
_Y2AD:ds	Y2AD-_Y2AD
	org	Y2AD

	push	af
	call	CHKMOD
	jp	z,Y2AD66

	ld	a,l
	dec	a
	add	a,a		;*2
	add	a,a		;*4
	add	a,a		;*8
	add	a,a		;*16
	add	a,a		;*32
	ld	e,a
	ld	a,(VRAM)
	jr	Y2AD2


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
	ld	h,0
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

	ex	de,hl

	ld	a,(SCREEN1)
	cp	02h		;;
	bit	2,h		;;;
	jp	nz,DELLIN66	;;;
	jr	nc,DELG		;;

	push	hl		;
;	ld	bc,COLUMNS
	ld	bc,2001h
	dec	h
	dec	h

DELLATT:
	call	GETSPA
	push	bc
	call	MEMSET
	pop	bc
	pop	hl		;

DELLEND:
	call	GETSP
	call	MEMSET
	jp	POPALL


;for graphic mode (screen3,4)
DELG:
;	ld	bc,COLUMNS*12
	ld	bc,8002h
	call	AD2GAD
	jr	DELLEND


;for graphic mode (screen mode 3,4)
SCRLG:
;bc=a*32*12=a*(256+128)
	ld	c,b		;b=0
	ld	b,a
	rra			;c-flag=0
	rr	c
	add	a,b
	ld	b,a

	ld	h,1
	call	XY2GAD
	ex	de,hl
	ld	hl,COLUMNS*12
	add	hl,de
	pop	af		;up/down
	jr	nz,SCRLATT
	ex	de,hl
	dec	de
	dec	hl

SCRLATT:
	call	LDIDR

SCRLATT2:
	pop	bc
	pop	hl

;scroll INPUT prompt position
;z=0: up,h=first,l=last
;z=1: down,h=last,l=first
	push	hl

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
	pop	hl

	pop	de
	ret


;set memory
;input: a=data, b=inner loops, c=outer loops, hl=start address
;destroy: af,bc,hl
MEMSET:
	ld	(hl),a
	inc	hl
	djnz	MEMSET
	dec	c
	jr	nz,MEMSET
	ret


;put a character in CRT (no control code, no move in cursor)
;input: a=character code, h=x+1, l=y+1
;destroy: none
_PUTCH:	ds	PUTCH-_PUTCH
	org	PUTCH

	push	hl
	call	XY2AD
	call	PUTCH2
	pop	hl
	ret


;scroll up
;input: h=first line+1, l=last line+1
;destroy: af
_SCRLU60:ds	SCRLU60-_SCRLU60
	org	SCRLU60

	ld	a,l
	sub	h		;a=(LASTLIN)-(CONSOL1) < 16
;	ret	z
;	ret	c

;z=0:scroll up, z=1:scroll down
SCRLUD60:
	push	de
	push	hl
	push	bc
	push	af		;up/down

;line connection status
	ld	b,a
	inc	b
	ld	d,b		;>0
SCRLUD60LP:
	call	CHKLINE4
	ld	c,a
	ld	a,d
	push	hl
	call	SETLINE
	pop	hl
	ld	d,c
	dec	l
	pop	af		;up/down
	push	af
	jr	nz,SCRLUD60NZ
	inc	l
	inc	l
SCRLUD60NZ:
	djnz	SCRLUD60LP

	ld	l,h
	ld	a,(SCREEN1)
	cp	02h
	pop	de		;d=a
	push	de
	ld	a,d
	jr	nc,SCRLG	;screen mode 3 4

	call	MUL32
	call	Y2AD
	ld	hl,COLUMNS
	add	hl,de
	pop	af		;up/down
	jr	nz,SCRL1NZ
	ex	de,hl
	dec	de
	dec	hl

SCRL1NZ:
	push	hl
	push	de
	push	bc
	call	LDIDR
	pop	bc
	pop	de
	pop	hl

;attribute
	res	1,h
	res	1,d
	jp	SCRLATT


;scroll down
;input: h=last line+1, l=first line+1
;destroy: af
_SCRLD60:ds	SCRLD60-_SCRLD60
	org	SCRLD60

	ld	a,h
	sub	l
;	ret	z
;	ret	c
	cp	a		;set z-flag
	jr	SCRLUD60


;print function key
;input: none
;output: none
;destroy: af
PRTFKEY:
	xor	a
	ld	(FKEYSFT),a

PRTFKEY2:
	ld	a,(SCREEN1)
	cp	02h
	ret	nc		;screen mode 3 or 4

	push	hl
	push	de
	push	bc

	ld	hl,FKEYSFT
	ld	a,(CONSOL3)
	or	a
	jr	z,DELFK

	ld	de,FKEYTBL
	call	STICK
	and	01h
	jr	z,FKEYZ
	ld	e,FKEYTBL+8*5-(FKEYTBL+8*5)/256*256	;de=FKEYTBL+8*5
FKEYZ:
	add	a,a
	inc	a
	cp	(hl)
	jr	z,PFKEND
	ld	(hl),a

	push	de
	ld	hl,(HEIGHT)
	call	Y2AD
	ex	de,hl
	pop	de


	call	CHRREV
	ld	a,(SCREEN2)
	add	a,'1'
	call	PUT12

	call	CHRREV
	ld	a,' '
	call	PUT12

	ld	c,05h
FKEYLP1:
	call	CHRREV
	ld	a,(FKEYLEN)
	ld	b,a
FKEYLP2:
	ld	a,(de)
	or	a
	jr	z,FKEYNEXT
	cp	20h
	jr	nc,FKEYNC
	ld	a,' '
FKEYNC:
	call	PUT12
	inc	e		;inc de
	djnz	FKEYLP2

FKEYNEXT:
	inc	b
FKEYLP3:
	ld	a,b
	dec	a
	call	z,CHRREV
	ld	a,' '
	call	PUT12
	inc	e		;inc de
	djnz	FKEYLP3

;	inc	e		;inc de
;	inc	e		;inc de

	ld	a,e
	and	0f8h
	or	FKEYTBL-FKEYTBL/8*8
	ld	e,a

	dec	c
	jr	nz,FKEYLP1

	call	CHKMOD
	call	z,PFK66

PFKEND2:
	call	SETCNSL2
PFKEND:
	pop	bc
	pop	de
	pop	hl
	ret

DELFK:
	cp	(hl)		;hl=FKEYSFT
	jr	z,PFKEND
	ld	(hl),a
	ld	hl,(HEIGHT)
	call	Y2AD
	call	DELLIN
	jr	PFKEND2


;reverse character attribute
;destroy: af
CHRREV:
	call	CHKMOD
	jp	z,CHRREV66
	ld	a,(SCREEN1)
	xor	03h
	jr	z,REVEND	;screen mode 4
	rrca
	ld	a,(COLOR1)
	jr	c,REV13

REV2:
	or	a
	jp	z,SETATT
	cp	08h
	jr	c,REV2C
	ld	a,08h
REV2C:
	dec	a
	jr	REVEND

REV13:
	or	a
	jr	z,REVEND
	dec	a
	and	03h
REVEND:
	xor	01h
	inc	a
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
	jp	CHGDSP


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

	call	CHKMOD
	call	z,CHGMOD66

	ldir

	call	SETCNSL2
	ld	l,01h
	ld	a,(HEIGHT)
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

	push	hl
	push	bc

	call	GETPGAD
	call	CHKMOD		;
	ld	a,(hl)
	call	z,CHGDSP66
	rlca
	rlca
	rlca
	rlca
	xor	04h

	ld	b,00000110b
	call	OUTB0H

	pop	bc
	pop	hl

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


;get VRAM dot pattern address (screen mode 3,4)
;hl=(VRAM+0200h)+(X/8)+Y*32
;input: bc=graphic X, de=graphic Y
;output: hl=VRAM address, d=bit
;destroy: af,bc,e
GXY2GAD:
	call	CHKMOD
	jp	z,GXY2GAD66
GXY2GAD60:
	call	CHKGXY
	ex	de,hl
	add	hl,hl		;*2
	add	hl,hl		;*4
	add	hl,hl		;*8
	add	hl,hl		;*16
	add	hl,hl		;*32
	ld	a,(VRAM)
	add	a,02h
	ld	b,a
	ld	a,c
	srl	c
GXY2GADEND:
	srl	c
	srl	c
	add	hl,bc

	and	07h
	inc	a
	ld	b,a

	ld	a,(SCREEN1)
	rrca
	ld	a,01h
	jr	c,GXY2GADLP	;screen mode 4

	dec	b
	res	0,b
	inc	b
	ld	a,81h

GXY2GADLP:
	rrca
	djnz	GXY2GADLP
	ld	d,a
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


;put a character in CRT (no control code, no move in cursor)
;input: a=character code, hl=VRAM address
;destroy: none
_PUTCH2:ds	PUTCH2-_PUTCH2
	org	PUTCH2

	push	hl
	push	af
	ld	a,(SCREEN1)
	cp	02h
	jr	c,PUTCH12

;screen mode 3,4
	call	AD2XY
	pop	af
	push	af
	call	PUTG
	jr	PUTCH2END

PUTCH12:
	pop	af
	push	af
	call	PUT12

PUTCH2END:
	pop	af
	pop	hl
	ret


;put character in screen mode 1,2
;input: a=character code, hl=VRAM address
;output: hl=hl+1
;destroy: af
PUT12:
	push	hl
	ld	(hl),a
	call	CHR2ATT
	call	CNVATT1
	ld	(hl),a
	pop	hl
	inc	hl
	ret


;put character in graphic mode (screen mode 3,4)
;input: a=character code, h=x+1, l=y+1
;destroy af,hl
PUTG:
	push	de
	push	bc

	call	CGROM
	ld	a,(COLOR1)
	call	SETATT

	call	CHKMOD
	jp	z,PUTG66
	call	XY2GAD

	ld	a,04h
	out	(93h),a		;CGROM ON

	ld	b,0ch
PUTGLP:
	push	de
	ld	a,(SCREEN1)
	rrca			;
	ld	a,(de)
	jr	nc,CALL3	;
	call	PUT4
	jr	CALL34END
CALL3:
	call	PUT3
	dec	hl

CALL34END:
	ld	de,COLUMNS
	add	hl,de
	pop	de
	inc	e		;inc de
	djnz	PUTGLP

	ld	a,05h
	out	(93h),a		;CGROM OFF

	pop	bc
	pop	de
	ret

PUT3:
	ld	e,a
	call	PUTHALF
	inc	hl
PUTHALF:
	ld	d,04h
PUT3LP:
	rlc	e
	rla
	rrca
	rlca
	rla
	dec	d
	jr	nz,PUT3LP

PUT4:
	ld	c,a
	ld	a,(ATTDAT)
	xor	(hl)
	and	c
	xor	(hl)
	ld	(hl),a
	ret


;read data from CGROM
;input: de
;output: d
;destroy: a
READCGROM:
	ld	a,04h
	out	(93h),a		;CGROM ON
	ld	a,(de)
	ld	d,a
	ld	a,05h
	out	(93h),a		;CGROM OFF
	ret


;get space attribute data for scroll
;output: a
;destroy: f
GETSPA:
	call	CHKMOD
	ld	a,(SCREEN1)
	jp	z,GETSPA66
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


;check connection to previous character
;input: hl=VRAM address
;output: z-flag(1:connect)
;destroy: af
CHKLINE:
	push	hl
	call	AD2XY
	call	CHKLINE2
	pop	hl
	ret


;check connection to previous character
;input: h=x+1, l=y+1
;output: z-flag(1:connect), a(0 or not)
;destroy: f
CHKLINE2:
	ld	a,h
	dec	a
	jr	z,CHKLINE3
	xor	a
	ret


;check connection to previous line
;input: l=y+1
;output: z-flag(1:connect), a(0 or not)
;destroy: f
CHKLINE3:
	push	hl
	dec	l
	jr	CHKLMAIN

;check connection to next line
;input: l=y+1
;output: z-flag(1:connect), a(0 or not)
;destroy: f
CHKLINE4:
	push	hl
CHKLMAIN:
	call	CALCLINE
	and	(hl)
	pop	hl
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
	ld	d,a
	call	CALCLINE
	inc	d
	dec	d
	jr	z,SETLINEZ
	or	(hl)
	ld	(hl),a
	ld	a,d
	ret

SETLINEZ:
	cpl
	and	(hl)
	ld	(hl),a
	ld	a,d
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
	call	SOUND2
	push	de		;

	ld	a,01h		;register1=ch.A coarse tune
	ld	e,00h
	ld	b,e
	ld	c,a
	call	SOUND2		;;

	call	WAITLP		;bc=0001h
	ld	a,01h		;register1=ch.A coarse tune
	ld	e,d		;;
	call	SOUND

	ld	a,08h		;register8=ch.A volume
	pop	de		;
	ld	e,d
	call	SOUND

	pop	bc

SKPCLK:
	pop	af
	ret


;set attribute data
;input: a=color code
;output: a,(ATTDAT)=attribute
;destroy: f
_SETATT:ds	SETATT-_SETATT
	org	SETATT

	push	af
	call	CHKMOD
	jp	z,SETATT66
SETATT661:
	pop	af
	call	CNVATT
	ld	(ATTDAT),a
	ret


;convert to attribute data (screen mode 1)
; or color code (screen moode 2)
; or bit pattern (screen mode 3,4)
;input: a=color code
;output: a=attribute
;destroy: f
CNVATT:
	push	af		;color
	ld	a,(SCREEN1)
	dec	a
	jr	z,ATT2
	jp	m,ATT1
	dec	a
	jr	z,ATT3

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
;	ld	b,04h
;	cp	b
;	jr	c,ATT3C
;	ld	a,b
;ATT3C:
;	dec	a
;	ld	c,a
;	dec	b		;=3
	ld	b,03h
	dec	a
	and	b
	ld	c,a
ATT3LP:
	rrca
	rrca
	or	c
	djnz	ATT3LP
	pop	bc
	ret

ATT2:
	pop	af
	cp	09h
	ret	c
	ld	a,08h
	ret


;get attribue data for screen mode 1
;output: a=attirbute value
;destroy: f
CNVATT1:
	ld	a,(COLOR1)
	push	af
ATT1:
	call	CHKMOD
	jp	z,ATT66
	pop	af
	or	a
	jr	z,ATT1OK
	dec	a
;	cp	04h
;	jr	c,ATT1OK
;	ld	a,03h
	and	03h
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
	call	CHKMOD
	ld	a,(SCREEN1)
	jp	z,GETSP66
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


;clear screen using console parameter
;destroy: af,bc,de,hl
CLSMAIN2:
	ld	hl,(CONSOL1)	;l=(CONSOL1)
	ld	a,(LASTLIN)
	sub	l
	inc	a

;clear from first line to last line
;input: l=first line+1, a=last line-first line+1
;destroy: af,bc,de,hl
CLSMAIN:
	ld	b,a
	ld	c,l
	call	Y2AD
CLSLP:
	call	DELLIN
	ld	hl,COLUMNS
	bit	2,d
	jr	z,CLS60
	ld	l,CLMN66	;h=0
CLS60:
	add	hl,de
	push	hl		;
	ld	l,c
	ld	a,c		;>0
	call	SETLINE
	inc	c
	pop	de		;
	djnz	CLSLP

	call	CTLHOM		;h<-1
	dec	h
	ld	l,h		;hl=0000h
	ld	(GRPX1),hl
	ld	(GRPY1),hl

	jp	PRTFKEY


;cold start (initialize hardware)
COLD:

;8255 (port 93h)
	ld	bc,0693h
	ld	a,b
	ld	hl,IOTBL93
	otir

;port c1h
;	ld	a,06h
	out	(0c1h),a	;32x16 text mode

;8251 (port 81h)
	ld	bc,0581h
;	ld	hl,IOTBL81
	otir

;port f0h-f8h
	ld	bc,09efh
;	ld	hl,IOTBLF0
F0LP:
	inc	c
	outi
	jr	nz,F0LP

;interrupt
;	ld	hl,INTTBL
	ld	de,0fa02h
	ld	c,HEIGHT-0fa00h+1	;b=0

	ldir
	ld	a,d
	ld	i,a
	im	2

;clear work area

COLDLP1:
	xor	a
	ld	(de),a
	inc	de
	ld	a,d
	or	e
	jr	nz,COLDLP1

;stack pointer
	ld	hl,0e700h
	ld	sp,hl		;temporary

;relay,VRAM,timer
	inc	a		;a=1
	ld	(CONSOL4),a

;hook
	ld	hl,HOOK
	ld	b,1eh
	ld	e,03h		;d=0
COLDLP2:
	ld	(hl),0c9h
	add	hl,de
	djnz	COLDLP2

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
COLDLP3:
	ld	(hl),3fh	;KYBFSZ,RSBFSZ,,BUFASZ,BUFBSZ,BUFCSZ
	add	hl,de
	djnz	COLDLP3

;command/function jump table
	ld	hl,CMDTBL
	ld	b,0+(FILES-CMDTBL)/2
COLDLP4:
	ld	(hl),SNERR-SNERR/100h*100h
	inc	hl
	ld	(hl),SNERR/100h
	inc	hl
	djnz	COLDLP4

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
	ld	de,VRAM		;top of active page data
	ld	a,05h
	ld	(FKEYLEN),a
COLDLP5:
	ld	hl,PAGEDATA
	ld	c,PAGE1-VRAM	;b=0
	ldir
	dec	a
	jr	nz,COLDLP5

;	ld	de,PAGE4+(PAGE4-PAGE3)
	ex	de,hl
	ld	de,0-(PAGE4-PAGE3)
	ld	a,0a0h
COLDLP6:
	add	hl,de
	ld	(hl),a		;(PAGE4)=a0h, (PAGE3)=c0h, (PAGE2)=e0h
	add	a,20h
	jr	nz,COLDLP6

;display
;	xor	a
	call	CHGACT
	call	CHGDSP
	call	CLS

;RND()
;	call	SETZERO
	call	RNDMNS

;PLAY
	call	PLSTOP		;ei

;initilize using 4000h-
;	call	COLD2

;port f0h
	ld	a,11h
	ld	(PORTF0H),a

;for mode select
	ld	a,0ffh
	ld	(PROGAD),a	;invalid value, not set yet

;RAM access subroutine
	ld	hl,READRAM_SRC
	ld	de,READRAM
	ld	bc,RAMEND-READRAM_SRC
	ldir

;RAM size and stack work
	ld	b,08h
	call	WAITLP
	call	GETCH
	ld	hl,0c400h
	ld	b,02h
	jr	z,COLD16K
	dec	a
;	or	02h
	or	b
	sub	'4'-1
	jr	nz,COLD16K
	inc	a
	ld	(MODE),a	;a=1 (mode=2)
	ld	h,84h
	ld	b,04h
COLD16K:
	ld	(BASICAD),hl
	inc	l
	ld	(STARTAD),hl
	dec	l
	ld	a,b
	ld	(PAGES),a
	ld	h,0f9h
	ld	(STACK),hl

	ld	a,71h		;0000-3fff:BASIC ROM, 4000-7fff:external ROM
	out	(0f0h),a

	jp	HOT


;analyze an argument
;input: hl=program address
;output: hl=next address, FAC1=numerical value or pointer to string descriptor
;	(ARGTYP)=0(numeric), 1(string), other(cannot analyze)
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
	ld	a,(hl)
	ld	b,a		;
	inc	hl
	ld	(PROGAD),hl
;command, function, operator
	or	a
	jp	m,ARGCMD
;space
	cp	' '
	jr	z,ARGLP2

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
	ld	a,0ffh
	call	ATOF
	ld	(PROGAD),hl
	jr	ARGLP

;destroy: af,bc
ARGNCHK:
	xor	a
	ld	(OPRTR),a
	ld	(ARGTYP),a
	ret

ARGVAR:
	push	hl		;program address
	call	GETNAME
	ld	a,(ARGTYP)
	ld	d,a
	ld	a,(OPRTR)
	ld	e,a
	push	de		;;
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
	pop	hl		;;
	ld	a,h
	ld	(ARGTYP),a
	ld	a,l
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
	ld	(PROGAD),hl
	jp	ARGLP

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
	call	CHKMOD
	ld	a,b		;
	jr	z,ARGCMD66
	cp	FNCLAST+1	;0f2h
	jp	nc,SNERR
ARGCMD66:
	cp	FLAST66+1	;0fah
	jp	nc,SNERR
	sub	PLUS		;0cah
	cp	FNC1ST-PLUS
	ld	a,b		;
	jr	c,OPR		;0cah-0d3h

	ld	a,(OPRTR)
	or	a
	jp	z,ARGTAIL

	ld	a,b		;
	cp	NOT_		;0c8h
	jr	z,OPR
	cp	SCREEN		;9fh
	jr	z,FNC
	cp	FN		;0c4h
	jr	z,FNC
	cp	INKEY		;0c6h
	jr	z,FNC
	cp	PLUS		;0cah
	jp	c,ARGTAIL

;	jr	FNC

;function
FNC:
	xor	a
	ld	(ARGTYP),a
	ld	a,b		;
	cp	INKEY
	jp	z,F_INKY
	cp	FN
	jp	z,F_FN
	cp	SCREEN
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

CALLFNC:
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
	cp	USR
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
	cp	PLUS
	jr	z,ARGNEXT2
	cp	MINUS
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
	push	de		;;d=following operator
	push	bc		;c=previous operator
	ld	a,(ARGTYP)
	dec	a
	jr	z,CALLOPS

;numeric
	call	CPYFAC
	pop	bc		;
	pop	de		;;
	call	POPF1

CALLOPEND:
	bit	6,c
	jr	nz,TYPNUM
	set	6,c
	ld	hl,ARGTYP
	set	1,(hl)
TYPNUM:
	push	de		;;
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
	pop	af		;;
	jp	OPR

;string
CALLOPS:
	ld	bc,(STRDSC1)
	ld	de,(STRDSC1+2)
	call	BACKSTR
	ld	(STRDSC4),bc
	ld	(STRDSC4+2),de
	pop	bc
	pop	de
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
	cp	PLUS		;0cah
	jr	z,OPRPLS
	ld	hl,MNSONE
	cp	MINUS		;0cbh
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

;+
OPRPLS:
	ld	hl,PLSONE
;-
OPRMNS:
	call	SETF1
	ld	a,ASTRSK	;0cch
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
;destroy: af,hl
BACKSTR:
	push	de
	push	bc
	ld	hl,STRDSC2
	ld	de,STRDSC1
	ld	bc,000ch
	ldir			;STRDSC2->STRDSC1, STRDSC3->STRDSC2, STRDSC4->STRDSC3
	xor	a
	ld	(de),a		;(STRDSC4)=0
	pop	bc
	pop	de
	ret


;send graphic data to PC-6021
;input: c
;destroy: af,b
SENDGRP:
	ld	b,08h
SENDGLP:
	rlc	c
	rra
	djnz	SENDGLP
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
	call	CNVKANA
	call	PRINTER
	pop	af
	ret


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
	xor	a		;set z-flag
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
	di
	call	OUT90H
	ei
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

	ld	hl,(WIDTH-1)	;h=(WIDTH)
	dec	h
	ld	l,01h
	call	XY2AD
	ld	(hl),a

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


;output port b0h
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
	call	SOUND
	inc	a		;register8=ch.A volume
	ld	e,00h
	call	SOUND
	inc	a		;register9=ch.B volume
	call	SOUND
	inc	a		;register10=ch.C volume
	call	SOUND

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

POPALL:
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
_SOUND2:ds	SOUND2-_SOUND2
	org	SOUND2

	push	af
	out	(0a0h),a
	in	a,(0a2h)
	ld	d,a
	pop	af
;	jp	SOUND


;set PSG register
;input: a=register, e=value
;destroy: none
_SOUND:	ds	SOUND-_SOUND
	org	SOUND

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
	call	SOUND
	ld	e,a		;a=0
	inc	a		;register1=ch.A 4bit coarse tune
	call	SOUND
;volume
	ld	e,07h
;	ld	a,08h		;register8=ch.A volume
	add	a,e		;register8=ch.A volume
	call	SOUND
	ld	bc,0400h
	ld	e,c		;c=0
	call	WAITLP
;volume
	ld	a,08h		;register8=ch.A volume
	jr	SOUND


;play subroutine called by time interrupt
;destroy: af,bc,de,hl
PLAYINT:
	ld	a,(CHANNEL)
	inc	a
	ld	a,(PLAYST)
	jr	z,PLIZ
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
	ld	hl,PLAYST
	ld	a,(CHANNEL)
	inc	a
	or	(hl)
	jr	nz,PLINEXT	;if (PLAYST)<>0 or (CHANNEL)<>ff

PLIGET:
;length
	call	GETPLBF
	jr	z,PLINEXT
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
	call	SOUND
	call	GETPLBF
	ld	e,a
	ld	a,b
	dec	a
	add	a,a		;register0,2,4=8bit fine tune
	call	SOUND

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
	call	SOUND
	ld	a,d		;
	sub	10h
	jr	c,PLISET

;period
	ld	d,a		;
	call	GETPLBF
	ld	e,a
	ld	a,0bh
	call	SOUND
	call	GETPLBF
	ld	e,a
	ld	a,0ch
	call	SOUND

;envelope pattern
	ld	e,d		;
	inc	a		;register13=envelope pattern
	call	SOUND

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
	inc	c
	dec	c
	ret	nz		;new (PLAYST)<>0
	or	a		;old (PLAYST)
	jp	nz,PLAYINT
	ld	(CHANNEL),a	;=0
	ret

;reset
PLIRES:
	res	0,c
	ld	a,b
	add	a,07h		;register8,9,10=volume
	ld	e,00h
	call	SOUND
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
	call	SOUND		;c0h or 80h

	dec	a		;register14=parallel port A
	out	(0a0h),a
	in	a,(0a2h)
	cpl
	ld	e,d		;
	ld	d,a		;;

	ld	a,07h
	call	SOUNDEI

	ld	a,d		;;
	pop	de
	ret


;LOCATE command
_C_LOCA:ds	C_LOCA-_C_LOCA
	org	C_LOCA

	call	INT1ARG2
	ld	h,c
	ld	a,(WIDTH)
	dec	a
	cp	h
	jp	c,FCERR
	ld	l,e
	ld	a,(HEIGHT)
	dec	a
	cp	l
	jp	c,FCERR
	inc	h
	inc	l
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


;used by MINTMARK
;set console parameter
;input: d=last line+1, e=first line+1
;destroy: af,bc,hl
_SETCNSL:ds	SETCNSL-_SETCNSL
	org	SETCNSL

	ld	(CONSOL1),de

SETCNSL2:
	ld	a,(HEIGHT)
	ld	hl,CONSOL2
	cp	(hl)
	jr	nc,CNSLNC
	ld	(hl),a
CNSLNC:
	ld	c,(hl)		;c=(CONSOL2)

	ld	hl,CONSOL3
	sub	(hl)
	ld	b,a		;b=(HEIGHT)-(CONSOL3)

	dec	hl		;LASTLIN
	cp	c
	ccf
	ld	a,(SCREEN1)
	rra
	or	a
	jr	nz,CNSLNZ
	ld	c,b		;if screen mode 1,2 and (CONSOL2)>(HEIGHT)-(CONSOL3)
CNSLNZ:
	ld	(hl),c

	ld	hl,CONSOL1
	ld	a,(hl)
	cp	b
	ret	c
	ld	(hl),b
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


;CLS command
;destroy: af,de
_CLS:	ds	CLS-_CLS
C_CLS:
	org	CLS

	push	hl
	push	bc
	call	CLSMAIN2
	pop	bc
	pop	hl
	ret


;SCREEN command
_C_SCRN:ds	C_SCRN-_C_SCRN
	org	C_SCRN

	ld	a,(SCREEN1)
	ld	e,a
	ld	d,01h		;d>0: no 1st parameter
	push	de
	ld	de,(SCREEN2)	;e=(SCREEN2),d=(SCREEN3)
	push	de

	call	CHKCMM
	jr	z,SCRPAR2
	call	INT1ARG
	dec	e
	pop	bc		;c=2nd,b=3rd parameter
	ex	de,hl
	ex	(sp),hl		;cancel 1st parameter, l=1st parameter, h=0
	ex	de,hl
	push	bc		;c=2nd,b=3rd parameter

	call	CHKCLCM
	jr	z,SCRMAIN

SCRPAR2:
	call	CHKCMM
	jr	z,SCRPAR3
	call	INT1ARG
	dec	a

	pop	de		;e=2nd,d=3rd parameter
	ld	e,a
	push	de

	call	CHKCLCM
	jr	z,SCRMAIN

SCRPAR3:
	call	INT1ARG
	dec	a

	pop	de		;e=2nd,d=3rd parameter
	ld	d,a
	push	de

SCRMAIN:
	pop	de		;e=2nd,d=3rd parameter
	pop	bc		;c=1st parameter

	ld	a,(PAGES)
	dec	a
	cp	d
	jp	c,FCERR
	cp	e
	jp	c,FCERR

	ld	a,c
	cp	02h
	jr	c,SCROK
	cp	04h
	jp	nc,FCERR
	ld	a,e
	or	a
	jr	nz,SCROK
	or	b		;b>0: no 1st parameter
	jp	z,FCERR		;if 1st>=2 and 2nd=0
SCROK:
	push	de		;e=2nd,d=3rd parameter
	ld	a,e
	call	CHGACT

	pop	af		;a=3rd parameter
	ld	hl,SCREEN3
	cp	(hl)
	call	nz,CHGDSP

	ld	a,b
	or	a
	ld	a,c
	jp	z,CHGMOD	;b>0: no 1st parameter
	ret


;TIME function
F_TIME:
	ld	hl,(TMCNT)
	ld	de,(TMCNT+2)
	jp	I4TOF


;SOUND command
C_SOUN:
	call	CHKCLN
	jp	z,SNERR
	call	INT1ARG2
	ld	a,c
	cp	10h
	jp	nc,FCERR
	di
SOUNDEI:
	call	SOUND
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
	jr	nz,PLAYNZ
	ld	a,b
	dec	a
	ld	(CHANNEL),a
	ld	a,0ffh
	push	hl
	call	PUTPLBF
	pop	hl
PLAYNZ:
	add	hl,de
	djnz	PLAYLP2


PLAYLP3:
	ld	a,(STOPFLG)
	cp	03h
	call	z,PLSTOP
	jr	z,PLAYEND

	ld	ix,PLWKC
	ld	b,03h
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

	or	a
	jr	nz,PLAYNEXT
	dec	a
	call	PUTPLBF		;a=ffh

PLAYNEXT:
	ld	de,PLWKB-PLWKC
	add	ix,de
	djnz	PLAYLP4

	ld	a,0ffh
	ld	(CHANNEL),a

	ld	a,(PLWKA+PLLEN)
	ld	hl,PLWKB+PLLEN
	or	(hl)
	ld	hl,PLWKC+PLLEN
	or	(hl)
	jr	nz,PLAYLP3

	di
	call	PLAYINT
	ei

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
	ld	hl,PLAYTBL
	ld	b,08h
PLCMDLP:
	cp	(hl)
	inc	hl
	jp	z,JPTBLNC
	inc	hl
	inc	hl
	djnz	PLCMDLP
	jp	FCERR


PLAYTBL:
	db	'V'-'A'
	dw	PLAYV
	db	'M'-'A'
	dw	PLAYM
	db	'S'-'A'
	dw	PLAYS
	db	'L'-'A'
	dw	PLAYL
	db	'T'-'A'
	dw	PLAYT
	db	'O'-'A'
	dw	PLAYO
	db	'R'-'A'
	dw	PLAYR
	db	'N'-'A'
	dw	PLAYN


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
;output: a,z-flag(1=no character)
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
	inc	l
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
	jr	CHKLEN

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
	jp	P,NOACC
	ld	e,22
	jr	NOACC

PLAYN0:
	xor	a
	push	af		;volume
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
	ld	d,00h
	add	hl,de
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
;60[s/min]/T[/min]/(L/4)/(8192/4000000[Hz])=117187.5/T/L
	ld	b,(ix+TEMPO)	;T-value
	call	MULINT1		;hl=T*L (<=3fc0h)
	ld	de,0e4e2h	;117187.5/2
	call	DIVINT2		;bc=(117187.5/2)/(T*L)*2
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

	di
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
	ei
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
	call	PLAYGETC
	ld	de,0000h
	ret	z		;c-flag=0
	cp	'='
	jr	z,GETNUMEQ
	cp	';'
	jr	z,GETNUMSEMI
	sub	'0'
	cp	'9'-'0'+1
	ret	nc		;c-flag=0
	ld	hl,(PLAYSTR)
	push	hl		;
	ld	a,(PLAYLEN)
	ld	b,a
	call	ATOI2
	ld	(PLAYSTR),hl
	pop	bc		;
	or	a
	sbc	hl,bc
	ld	a,(PLAYLEN)
	sub	l
	ld	(PLAYLEN),a
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
	ex	de,hl
	jr	nc,GETNUMEQNC
	ld	hl,ZERO
GETNUMEQNC:
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
	ex	de,hl
	jr	GETNUMEQNC


;STICK() function
F_STCK:
	call	FNCI1		;a=0
	add	a,e
	jr	z,STCK0		;e=0?
	cp	03h
	jp	nc,FCERR
	call	JOYSTK
;exchange bit2 and bit3
	ld	b,a
	and	0ch
	ld	a,b
	jp	pe,CNVSTCK
	xor	0ch
	jr	CNVSTCK

STCK0:
	call	STICK
	rrca
	rrca
CNVSTCK:
	and	0fh
	ld	hl,STCKTBL
	add	a,l
	ld	l,a
	jr	nc,STCKNC
	inc	h
STCKNC:
	ld	l,(hl)
	jp	I1TOF

STCKTBL:
;		    u   d   ud  r   ur  dr  udr
	db	0,  1,  5,  0,  3,  2,  4,  3
;		l   ul  dl  udl rl url drl udrl
	db	7,  8,  6,  7,  0,  1,  5,  0


;STRIG() function
F_STRG:
	call	FNCI1		;a=0
	add	a,e
	jr	z,STRG0		;e=0?
	cp	03h
	jp	nc,FCERR
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


;LCOPY command
C_LCPY:
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
	xor	a
	ld	(DEVICE),a

	ld	c,a		;file name length
	call	SKIPSP
	cp	PRINT
	ld	a,c		;=0
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
	call	PRTFN
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
;	inc	c
;	dec	c
;	call	z,RESSTK	;c=0: not verify
	call	RESSTK

	call	CHGLKP
	jp	EDIT2

CLDSKIP:
	ld	hl,SKIP
	call	PRTFN

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


;input by external device
;input: a=(INPDEV), hl=INPBUF-1
;destroy: af,bc,de,hl
INPTEXT:
	cp	02h
	push	af
	jr	z,INPT232
	rlca
	jp	nc,FCERR
	call	INPOPN
INPT232:
	ld	hl,INPBUF
	ld	b,71
INPTEXTLP:
	pop	af
	push	af
	jr	nz,INPTCMT

	push	hl
INPT232LP:
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
	call	nz,RCLOSE
	ld	(hl),0
	ld	hl,INPBUF-1
	jp	INPTANA

INPTCMT:
	call	GETCMTTR
	jr	INPTCHKRET

EXTRA:
	db	"?Extra ignored", 0dh, 0ah, 00h

REDO:
	db	"?Redo from start", 0dh, 0ah, 00h


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
;output: bc
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


;print message and file name
;input: hl=message address
;destroy: af,hl,(bc,de,FAC1)
_PRTFN:ds	PRTFN-_PRTFN
	org	PRTFN

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
	cp	CMDLAST+1
	jr	c,CNVASCC
	ld	hl,CMDNAME66-1
	cp	CLAST66+1
	jr	nc,CNVFNC
	sub	CMDLAST-7fh
	jr	CNVASCC

;function
CNVFNC:
	ld	hl,FNCNAME-1
	sub	TAB-80h
	cp	FNCLAST-(TAB-80h)+1
	jr	c,CNVASCC
	ld	hl,FNCNAME66-1
	sub	FNCLAST+1-TAB

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

	pop	af		;waste time
	push	af		;waste time

	dec	bc
	ld	a,b
	or	c
	ret	z
	jr	WAITLP


;EXEC command
C_EXEC:
	call	NUMARGMO
	call	FTOI

;for compatibility with N60-BASIC
	ld	hl,(PROGAD)
	ex	de,hl
	ld	a,h
	or	l
	neg		;set c-flag if hl<>0
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


;input: a=length, hl=string address
;destroy: af,bc,de,hl
MAKESTR:
	ld	(STRDSC1),a
	ld	(STRDSC1+2),hl
	xor	a
	jr	ADDSTR2

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
STRARG3:
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


;get string,integer argument
;input: hl=program address
;output: STRDSC1, a=length, hl=string address, FAC1,de=integer
;destroy: FAC1, FAC2, f,bc
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
;destroy: FAC1, FAC2, af, bc, de
ARGI1:
	push	hl		;program address
	call	COPYSTR
	pop	hl
	call	INT1INC
	jp	BACKSTR


;put a character in device
;input: a=character code, (DEVICE)=0:CRT, 1:printer, 2:RS-232C, 80h-ffh:CMT
;destroy: af
_PUTDEV:ds	PUTDEV-_PUTDEV
	org	PUTDEV

	push	af
	ld	a,(DEVICE)
	or	a
	jr	z,PUTCRT
	jp	m,PUTCMT2
	dec	a
	jr	z,PUTPRT2
	dec	a
	jr	z,PUT232
	pop	af
	ret


PUTPRT2:
	pop	af
	cp	09h
	jr	z,PUTPRTTAB
PUTPRT3:
	push	af
	call	PUTPRT
	pop	af
	sub	0dh
	jr	z,PUTPRTZ	;a=0 if CR
	ret	c
	ld	a,(PRTPOS)
	inc	a
PUTPRTZ:
	ld	(PRTPOS),a
	ret


PUTPRTTAB:
	push	bc
	ld	a,(PRTPOS)
	cpl
	and	07h
	inc	a
	ld	b,a
PUTPRTTABLP:
	ld	a,' '
	call	PUTPRT3
	djnz	PUTPRTTABLP
	pop	bc
	ret


PUTCRT:
	pop	af
	jp	PUTC


;input: a
;output: none
;destroy: af
PUT232:
	ld	a,17h		;RTS=0
	out	(81h),a

PUT232LP:
	in	a,(81h)
	cpl
	and	81h
	jr	z,PUT232Z	;if DSR=1, TxRDY=1

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


;put new line
;destroy: af
_PUTNL:	ds	PUTNL-_PUTNL
	org	PUTNL

	ld	a,0dh
	call	PUTDEV
	ld	a,0ah
	jp	PUTDEV


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
CHKSTLP:
	ld	a,(STOPFLG)
	cp	1bh
	jr	z,CHKSTLP
	call	GETCH		;ESC end
	jr	CHKSTOP


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
	ld	a,01h
	call	GCCHECK
	ld	(STRAD),hl
	inc	hl
	ld	(hl),b		;
	ld	(STRDSC1+2),hl
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


;convert string to 2-byte integer (unsigned)
;input: hl=program address, b=string length
;output: de=integer, hl=next address
;destroy: af,bc
ATOI2:
	ld	de,0000h
	dec	hl
ATOI2LP:
	inc	hl
	ld	a,(hl)
	cp	' '
	jr	z,ATOI2LP
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
	djnz	ATOI2LP
	inc	hl
	ret

ATOI2END:
	pop	hl
	ret


;convert string to float (integer part)
;input: b=length, hl=program address
;output: FAC1, hl=next address, b=remained length, d=sign(0=plus, 1=minus)
;destroy: FAC2, af,de
ATOIF:
	ld	a,b		;
	push	hl
	call	SETZERO
	pop	hl
	ld	b,a		;
	ld	d,00h		;sign

ATOIFLP1:
	call	SKIPSPB
	ret	z		;all space

	dec	b
	cp	'+'
	jr	z,ATOIFLP2
	cp	PLUS
	jr	z,ATOIFLP2
	inc	d
	cp	'-'
	jr	z,ATOIFLP2
	cp	MINUS
	jr	z,ATOIFLP2
	inc	b
	dec	d
	dec	hl
ATOIFLP2:
	call	CTOF
	ret	nc
	djnz	ATOIFLP2
	ret


;FAC1 = FAC1 * 10 + [0-9]
;input: FAC1, hl=program address, b=length(>0), d=sign(0=plus, 1=minus)
;output: FAC1, c-flag(1=OK), b=remained length(for space), hl=next address
;destroy: FAC2,af
CTOF:
	call	SKIPSPB
	dec	hl
	ret	z

	sub	'0'
	cp	'9'-'0'+1
	ret	nc
	inc	hl
	push	hl		;program address

	push	de
	push	bc
	push	af
	call	ABS
	call	MULF10
	call	CPYFAC
	pop	af
	call	I1TOFA
	call	ADDF
	pop	bc
	pop	de

	ld	a,d
	rrca
	call	c,NEGABS2

	pop	hl		;program address
	scf
	ret


;insert a character
;input: a=data (for checking BELL)
;output: hl=VRAM address
;destroy: f,bc,de
INSERT:
	cp	07h
	jr	nz,INSNZ
	call	PUTC
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

	push	hl
	call	CHR2ATT
	ld	(hl),c
	pop	hl

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

	ld	a,b
	call	PUT12
	dec	hl

	pop	af
	ret

INSSCRLU:
	pop	hl
	ld	de,0-COLUMNS
	bit	2,h
	jr	z,INSSCRLU60
	ld	e,0-CLMN66	;d=0ffh
INSSCRLU60:
	add	hl,de
	push	hl

	ld	l,a		;(LASTLIN)
	ld	a,1eh		;up
	call	PUTC

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
	call	CHKMOD
	jp	z,GETSCRC66
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


;one line input
;output: hl=INPBUF-1, c-flag(1=stop)
;destroy: af,bc,de
_INPT1:	ds	INPT1-_INPT1
	org	INPT1

	ld	hl,(CSRY)
	ld	(INPTX-1),hl	;h=end position for INPUT command
	ld	(INPTXY),hl	;initial position for INPUT command
	ld	a,(CSRAD+1)
	ld	(INPTPAG),a	;page for INPUT command

INPT1LP:
	call	GETC
	call	INPT1CMD
	jr	INPT1LP


;one line input main routine
INPT1CMD:
	cp	20h
	jr	c,INPT1CTL

	ld	hl,(INSMODE)
	inc	l
	dec	l
	call	nz,INSERT

INPT1PUTC:
	call	PUTC

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


;control code
INPT1CTL:
	ld	hl,NOINSTBL
	ld	bc,000ch
	cpir
	jr	nz,INPT1CMDNZ
	ld	hl,INSMODE
	ld	(hl),b		;b=0
INPT1CMDNZ:
	ld	hl,(CSRAD)

	cp	02h
	jp	z,INPT1CTLB
	cp	03h
	jp	z,INPT1STOP
	cp	05h
	jr	z,INPT1CTLE
	cp	06h
	jp	z,INPT1CTLF
	cp	08h
	jr	z,INPT1DEL
	cp	09h
	jp	z,INPT1TAB
	cp	0ah
	jp	z,INPT1CTLJ
	cp	0dh
	jp	z,INPT1RET
	cp	12h
	jr	z,INPT1INS
	cp	14h
	jr	z,INPT1PUTC
	cp	15h
	jr	z,INPT1CTLU
	jr	INPT1PUTC	;1ch,1dh,1eh,1fh


INPT1INS:
	ld	hl,INSMODE
	ld	a,(hl)
	cpl
	ld	(hl),a
	ret


INPT1DEL:
	call	CHKLINE
	push	hl		;
	ld	hl,DELSTR
	jr	z,INPT1DELZ
	inc	hl
INPT1DELZ:
	call	PUTS
	call	CHKIPOS2
	pop	hl		;
	ld	d,h
	ld	e,l
	dec	de
	call	CNVATT1
	ld	c,a
DELLP:
	call	GETSCRC
	ld	(de),a

	push	hl
	call	CHR2ATT
	ld	(hl),c
	pop	hl

	inc	de
	inc	hl

	call	CHKLINE
	jr	z,DELLP

	dec	hl
	ld	(hl),' '
	call	CHR2ATT
	ld	(hl),c

	ret

DELSTR:
	db	1dh, ' ', 1dh, 00h


CTLULP:
	dec	hl
INPT1CTLU:
	call	CHKLINE
	jr	z,CTLULP

INPT1CTLE:
	push	hl
	call	CHR2ATT
	call	CNVATT1
	ld	(hl),a
	pop	hl

	ld	(hl),' '
	inc	hl
	call	CHKLINE
	jr	z,INPT1CTLE
	ret


INPT1CTLB:
	call	CHKCTLB
	jr	nc,INPT1CTLB
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


INPT1CTLF:
	call	CHKCTLF
	jr	c,INPT1CTLF
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
;	ld	l,a
;	ld	h,COLUMNS

	ld	hl,(WIDTH-1)	;h=(WIDTH)
	ld	l,a

	pop	af		;cancel return address
	jp	SETCSR


INPT1CTLJ:
	ld	a,(INSMODE)
	or	a
	ld	a,0ah
	jp	z,INPT1PUTC

	ld	hl,(CSRY)	;l=y+1, h=x+1
;	ld	a,COLUMNS+1

	ld	a,(WIDTH)
	inc	a

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


INPT1TAB:
	ld	a,(WIDTH)
	sub	07h
	ld	b,a
	ld	a,(CSRX)
	sub	b
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
INPT1TABLP:
	push	bc
	ld	a,c
	call	INPT1CMD
	pop	bc
	djnz	INPT1TABLP
	ret


INPT1STOP:
	pop	hl		;cancel return address
	ld	hl,(CSRY)
STOPLP:
	inc	l
	call	CHKLINE3
	jr	z,STOPLP
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


INPT1RET:
	pop	af		;cancel return address

	ld	a,(CSRAD+1)
	ld	d,a
	ld	a,(INPTPAG)
	xor	d		;
	and	0f0h
	jr	nz,IRETNZ1

	ld	a,(CSRY)
	ld	de,(INPTXY)
	cp	e
	jr	nz,IRETNZ1

	push	hl
	ld	l,e
	call	CHKLINE3
	pop	hl
	jr	z,IRETNZ1

	ld	a,(INPTX)
	sub	d
	jr	SETINPTX
IRETNZ1:
	ld	a,71
SETINPTX:
	ld	(INPTX),a

IRETLP1:
	call	CHKLINE
	dec	hl
	jr	z,IRETLP1
	inc	hl
	ld	d,h		;
	call	AD2XY
	ld	a,(INPTPAG)
	xor	d		;
	and	0f0h
	jr	nz,IRETNZ2

	ld	de,(INPTXY)	;initial position
	ld	a,l
	cp	e
	jr	nz,IRETNZ2
	ex	de,hl

IRETNZ2:
	call	XY2AD

IRETLP2:
	call	GETSCRC
	ld	de,INPBUF
	ld	b,71
IRETLP3:
	call	GETSCRC
	cp	20h
	jr	nc,IRETNC
	ex	de,hl
	ld	(hl),14h
	ex	de,hl
;	dec	b
;	jr	nz,IRETGRP
	djnz	IRETGRP
	xor	a
	ld	(de),a
	jr	IRETNZ3
IRETGRP:
	inc	de
	add	a,30h

IRETNC:
	ld	(de),a
	inc	de
	inc	hl
	call	CHKLINE
	jr	nz,IRETNZ3
	djnz	IRETLP3

IRETNZ3:
	dec	hl
	call	AD2XY
	call	SETCSR

IRETLP4:
	dec	de
	ld	a,(de)
	cp	' '
	jr	z,IRETLP4
	inc	de
	xor	a
	ld	(de),a

	call	PUTNL

	ld	hl,INPBUF-1
	ret

NOINSTBL:
	db	02h, 03h, 05h, 06h, 0bh, 0ch, 0dh, 15h, 1ch, 1dh, 1eh, 1fh


;x=x+-(8 or 4 or 2 or 1)
;input: b(bit7=0:increment, bit7=1:decrement), (GRPX3)
;output: hl,(GRPX3)=X
;destroy: af,de
INCGX:
	ld	a,(SCREEN1)
	inc	a
	ld	d,a
	ld	a,10h
INCGXLP:
	rrca
	dec	d
	jr	nz,INCGXLP

	ld	hl,(GRPX3)
	ld	e,a
	bit	7,b
	jr	nz,INCGXNZ

	add	hl,de		;de=8,4,2,1
	jr	INCGXEND

INCGXNZ:
;	or	a
	sbc	hl,de		;de=8,4,2,1

INCGXEND:
	ld	(GRPX3),hl
	ret


;y=y+(12 or 4 or 1 or 1)
;input: (GRPY3)
;output: de=(GRPY3)
;destroy: af
INCGY:
	call	CHKMOD
	jp	z,INCGY66
	ld	a,(SCREEN1)
	dec	a
	ld	a,04h		;screen mode 2
	jr	z,INCGYZ
	ld	a,01h		;screen mode 3,4
	jp	p,INCGYZ
	ld	a,0ch		;screen mode 1
INCGYZ:
	ld	de,(GRPY3)
	add	a,e
	ld	e,a
	ld	(GRPY3),a	;<256
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


;input: bc=x1, de=y1, (GRPX2)=x2, (GRPY2)=y2
;output: bc=(GRPX2)=x2, de=(GRPY2)=y2, hl=(GRPY3)=y1, (GRPX3)=x1, z,c(y1>y2?)
;destroy: af
CHKGXY23:
	call	CHKGXY
	ld	(GRPX3),bc
	ld	(GRPY3),de
	ld	bc,(GRPX2)
	ld	de,(GRPY2)
	call	CHKGXY
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
	call	CHKMOD
	jp	z,CHKGXY66
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


;screen mode 2
PSET2:
	push	bc		;X
	call	GXY2AD60

	ld	a,(ATTDAT)
	ld	e,(hl)		;
	or	a
	jr	nz,PS2NZ1
	bit	6,e
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
	pop	bc		;X
	call	MASK2
	ld	b,a

	ld	a,(hl)
	and	3fh
	or	b
	ld	c,a

	ld	a,(ATTDAT)
	or	a
	jr	z,PSET2C0
	dec	a
	and	03h
	rrca
	rrca
	add	a,c
	ld	(hl),a
	pop	hl
	ret

PSET2C0:
	ld	a,b
	cpl
	and	(hl)
	ld	(hl),a
	pop	hl
	ret


;screen mode 2 dot bit mask
;input: c=X, d=Y mod 12
;output: a
; bit5 bit4
; bit3 bit2
; bit1 bit0
;destroy: f,bc
MASK2:
	ld	a,c
	cpl
	and	04h
	add	a,04h
	bit	2,d
	ret	nz	;4<=d<=7
	bit	3,d
	jr	nz,MASK2NZ
	rlca
	rlca
	ret		;0<=d<=3
MASK2NZ:
	rrca
	rrca
	ret		;8<=d<=11


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

	ld	a,b
	or	a
	jr	z,BFGNEXT

;middle part
	push	hl		;VRAM address
	ld	a,(ATTDAT)
	dec	b
	jr	z,BFGRHT
BFGLP4:
	inc	hl
	ld	(hl),a
	djnz	BFGLP4

;right part
BFGRHT:
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


;|dy/dx|<=1
SLOPEX:
	xor	a
	sub	l
	ld	l,a
	sbc	a,h
	sub	l
	ld	h,a
	push	hl		;error=-|dx|

LINELP2:
	ld	bc,(GRPX3)
	ld	de,(GRPY3)
	call	PSET

	pop	hl		;error
	pop	bc		;dx
	pop	de		;dy
	push	de		;dy
	push	bc		;dx

	add	hl,de		;error+=dy
	add	hl,de		;error+=dy
	bit	7,h
	jr	nz,ERRMNS

;if error>=0
	call	SUB2DX		;error-=|dx|*2
	call	INCGY		;increment Y

ERRMNS:
	push	hl		;error
	call	INCGX		;increment or decrement X
	jp	c,LINEEND

	ex	de,hl		;de=X
	ld	hl,(GRPX2)
	bit	7,b
	jr	z,LINEZ
	ex	de,hl		;if dx<0
LINEZ:
	rst	CPHLDE
	jr	nc,LINELP2
	jp	LINEEND


;get (step)(x,y) cordinate
;input: hl=program address
;output: bc=graphic X, de=graphic Y,
;	 (GRPX1)=(GRPX2),(GRPY1)=(GRPY2)=0-32767
;	 a=next data, hl=next data address, z(, or not)
;destroy: f
GETGXY:
	call	SKIPSP
	cp	STEP
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
C_PRST:
	ld	a,(COLOR2)
	jr	PSETRST

;PSET command
C_PSET:
	ld	a,(COLOR1)

;PSET and PRESET
PSETRST:
	call	SETATT
	call	GETGXY
	call	z,PSCOLR
;	jp	PSET


;input: bc=x1, de=y1, (ATTDAT)=attribute
;destroy: af,bc,de
_PSET:	ds	PSET-_PSET
	org	PSET

	push	hl
;	call	CHKGXY
	call	CHKMOD
	jp	z,PSET66
	ld	a,(SCREEN1)
	dec	a
	jp	z,PSET2
	jp	p,PSETG


;screen mode 1
PSET1:
	call	GXY2AD60
	ld	a,(ATTDAT)
	jr	PSETEND


;screen mode 3,4
PSETG:
	call	GXY2GAD60
	ld	a,(ATTDAT)
	xor	(hl)
	and	d
	xor	(hl)
PSETEND:
	ld	(hl),a
	pop	hl
	ret


PSCOLR:
	push	bc		;x
	push	de		;y
	call	INT1INC
	pop	de		;y
	pop	bc		;x
	jp	SETATT


;POINT() function
F_POIN:
	call	GETGXY
	call	POINT
	jp	I1TOFA


;prepare for line command
PRELINE:
	call	SKIPSP
	cp	MINUS
	ld	bc,(GRPX1)
	ld	de,(GRPY1)
	jr	z,LINE2P

LINE1P:
	call	GETGXY
	cp	MINUS
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
	or	a		;reset c-flag
	jr	PRELEND

;check 'B' and 'F'
CHKBF:
	call	SKIPSPINC
	cp	'B'
	jp	nz,SNERR

	call	SKIPSPINC
	cp	'F'
	jr	nz,BOX
	inc	hl		;for box fill
BOX:
	ld	(PROGAD),hl
	ld	a,e
	scf

PRELEND:
	pop	de		;y1
	pop	bc		;x1

	push	af
	call	SETATT
	pop	af
	ret


;LINE command
C_LINE:
	call	PRELINE
	jr	nc,LINE
	jr	nz,LINEB
;	jr	LINEBF


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
	call	GXY2GAD60	;b=0
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
BF12:
	ld	(GRPY2),de
	call	LINE
	rst	CPHLDE
	jr	z,LINEEND2
	call	INCGY
	jr	BF12


LINE2:
	call	CHKMOD
	ld	a,(SCREEN1)
	jp	z,LINE66
	rrca
	jr	c,SKIPY
;screen mode 1 or 3: dy=dy*2
	add	hl,hl

SKIPY:
	ex	de,hl
	ld	bc,(GRPX3)	;start (x)
	ld	hl,(GRPX2)	;end (x)
	or	a
	sbc	hl,bc		;dx
	or	a
	jr	nz,SKIPX
;screen mode 1: dx=dx*3
	ld	b,h
	ld	c,l
	add	hl,hl
	add	hl,bc

SKIPX:
	push	de		;dy
	push	hl		;dx

	bit	7,h
	jr	z,DXPLUS	;dx>0
;if dx<0, hl=-hl
	xor	a
	sub	l
	ld	l,a
	sbc	a,h
	sub	l
	ld	h,a
DXPLUS:
	rst	CPHLDE
	jp	nc,SLOPEX	;if |dy/dx|<=1

;|dy/dx|>1
	push	de		;error=dy

	ld	de,(GRPY3)
LINELP1:
	ld	bc,(GRPX3)
	call	PSET

	pop	hl		;error
	pop	bc		;dx
	pop	de		;dy
	push	de		;dy
	push	bc		;dx

	call	SUB2DX		;error-=|dx|*2
	push	hl
	bit	7,h
	jr	z,ERRPLS

;if error<0
	pop	hl
	add	hl,de		;error+=dy
	add	hl,de		;error+=dy
	push	hl		;error
	call	INCGX		;increment or decrement X
;	jr	c,LINEEND

ERRPLS:
	call	INCGY		;increment Y
	ld	hl,(GRPY2)
	rst	CPHLDE
	jr	nc,LINELP1

LINEEND:
	pop	af		;cancel error
	pop	af		;cancel dx
	pop	af		;cancel dy

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
;	jp	PAINT2


;paint used by OKHOTSK
;input: e=color code, (sp)=Y, (sp+2)=X
_PAINT2:ds	PAINT2-_PAINT2
	org	PAINT2

	ld	a,e
	call	CNVATT
	ld	(BORDERA),a
	pop	de		;Y
	pop	bc		;X


;input: bc=x, de=y, (ATTDAT)=attribute, (BORDERA)=attribute, (BORDERC)=color
;destroy: af,bc,de
_PAINT:	ds	PAINT-_PAINT
	org	PAINT

	ld	a,(SCREEN1)
	dec	a
	ret	z		;screen mode 2
	push	hl
	jp	m,PAINT1	;screen mode 1
	call	GXY2GAD60
PAMAIN:
	call	PADWN
	pop	hl
	ret

PAINT1:
	call	GXY2AD60
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
	call	CMPPAX
	ret	c

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
	call	GXLARGE
	jr	nz,SRCHOK
	scf
	ret


;search for unpaintable area
;get unpaintable area
;input: hl=address, d=bit
;output: hl, d, c-flag(1=over,0=NG)
;destroy: af,e
SRCHNG:
	call	CMPPAX
	ret	c

	ld	e,(hl)
	ld	a,(BORDERA)
	xor	e
	and	d
	ret	z		;c-flag=0

	call	GXLARGE
	jr	nz,SRCHNG
	scf
	ret


;compare  X to (PAWRK)
;input: hl=address, d=bit
;output: f
;destroy: af,e
CMPPAX:
	ld	a,l
	and	1fh
	ld	e,a
	ld	a,(PAWRK)	;address low (5 bits)
	cp	e
	ret	nz

	ld	a,(PAWRK+1)	;bit
	dec	a
	cp	d
	ccf
	ret


;increment x and compare graphic width (=320)
;output: z-flag(1=over)
;destroy: af
GXLARGE:
	call	INCGXPA
	ld	a,l
	and	1fh
	ret	nz
	ld	a,d
	cpl
	and	80h
	ret


;STR$() function
F_STR:
	call	CHKNUM
	call	FTOA
	ld	d,h
	ld	e,l

	ld	b,0-01h
STRLP:
	inc	b
	ld	a,(de)
	inc	de
	or	a
	jr	nz,STRLP

	ld	a,b
	call	MAKESTR
	jp	INKYEND


;MODE key
;destroy: af
MODEKEY:
	push	hl
	push	de
	push	bc

	ld	hl,KEYFLG
	bit	3,(hl)
	jr	z,MODEEND
	res	3,(hl)

	ld	hl,(LINENUM)
	ld	a,h
	and	l
	inc	a
	jr	nz,MODEEND	;not direct command mode

	call	CSROFF

	ld	hl,MODESTR
	call	PUTS
	ld	a,(MODE)
	inc	a
	call	PUTI1
	call	PUTNL

	ld	a,(PAGES)
	call	PUTI1
	ld	hl,PAGESTR
	call	PUTS

	ld	a,(DRVBIT)		;?
	or	a
	jr	z,SKPFILES

	ld	a,(FILES)
	call	PUTI1
	ld	hl,FILESTR
	call	PUTS

SKPFILES:
	call	CSRON

MODEEND:
	pop	bc
	pop	de
	pop	hl
	ret


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
F_RHT:
	call	ARGSI1
	ld	d,a		;
	sub	e
	jr	nc,RHTNC
	xor	a
RHTNC:
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
	ex	de,hl
	call	ATOF
	jr	SETTYP0


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
	or	a
	jr	CALCFRE
;numeric
FRENUM:
	ld	h,a		;a=0
	ld	l,a
	add	hl,sp
	ld	de,(FREEAD)
CALCFRE:
;	or	a
	sbc	hl,de
	jp	I2TOF


;DIM command
C_DIM:
	call	CHKVAR

	ld	a,(hl)
	inc	hl
	cp	'('
	jr	nz,NOTARR

	push	hl		;program address
	call	SRCHARR
	jp	nc,DDERR
	call	SETARNM
	pop	hl		;program address
	push	de		;array address
	bit	7,c
	ld	bc,0004h
	jr	nz,DIMSTR
	inc	c		;numeric:bc=5, string:bc=4
DIMSTR:
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

	ex	de,hl
	ex	(sp),hl		;push size, pop array address
	ex	de,hl

	push	de		;array address
	push	hl		;element bytes
	ld	hl,(PROGAD)
	ld	a,(hl)
	inc	hl
	cp	','
	jr	z,DIMLP
	cp	')'
	jp	nz,SNERR

CALLMKA:
	call	MAKEARR
	ld	hl,(PROGAD)

	dec	hl
	rst	ANADAT
	ret	z
	inc	hl

	cp	','
	ret	nz
	jr	C_DIM


NOTARR:
	dec	hl
	ld	(PROGAD),hl
	call	SRCHVAR
	ret	nc
	jp	MAKEVAR


;get variable address
;input: bc=variable name, hl=program address
;output: de=variable address, (PROGAD)=next program address
;destroy: FAC1,FAC2,af,de
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
	jp	SNERR

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

	bit	7,c
	ld	bc,0004h
	jr	nz,AUTOSTR
	inc	c		;numeric:bc=5, string:bc=4
AUTOSTR:
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
	ld	a,c
	cp	04h
	jp	nc,BSERR	;over 3 dimensions

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
;output: bc=name, hl=next address
;destroy: af
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
;output: bc=name, hl=next address
;destroy: af
GETNAME:
	ld	c,00h
	dec	hl
NAMELP1:
	inc	hl
	ld	a,(hl)
	cp	' '
	jr	z,NAMELP1
	cp	'$'
	jr	z,NAMESTR
	call	ALPNUM
	ret	nc
	ld	c,a
NAMELP2:
	inc	hl
	ld	a,(hl)
	cp	' '
	jr	z,NAMELP2
	call	ALPNUM
	jr	c,NAMELP2
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
	bit	7,c
	ld	bc,0004h
	jr	nz,INISTR
	inc	c		;numeric:bc=5, string:bc=4
INISTR:
	ldir
	pop	de		;variable address
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
;pushed: size*(1~255), array address, element bytes
;destroy:
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
	call	NUMARGMO
	ld	hl,FAC1
	ld	bc,0005h
VARINOK:
	pop	de		;variable address
	ldir
	ld	hl,(PROGAD)
	ret


;NEW command
C_NEW:
	ld	hl,EDIT
	push	hl

NEWRESSTK:
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
	ld	hl,0-003bh+1
	add	hl,sp
	sbc	hl,de		;c-flag=1
	jp	c,OMERR
	ex	de,hl
	ret


;reset stack and variables
;destroy: a,bc,de
_RESSTK:ds	RESSTK-_RESSTK
	org	RESSTK

	ex	de,hl
	pop	bc		;return address
	ld	sp,(STACK)
	push	bc		;return address
	ld	a,0ffh
	ld	(STOPAD+1),a

	ld	hl,(VARAD)
	ld	(ARRAD),hl
	ld	(FREEAD),hl
RSTRZ:
	ld	hl,(BASICAD)
	ld	(DATAAD),hl
	ex	de,hl
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
C_CLR:
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
	inc	hl
	call	NUMARGMO
	call	FTOI
	dec	de

CLRNZ:
	ld	hl,(USREND)
	rst	CPHLDE
	jp	c,FCERR
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

	call	POPF2		;STEP value
	call	POPF1		;TO value
	call	PUSHF1		;TO value
	call	PUSHF2		;STEP value

	push	de		;variable address

	ld	hl,(FAC2+3)
	ld	a,h
	or	a		;z-flag
	ld	a,l
	rlca			;c-flag
	ex	de,hl		;variable address
	call	SETF2		;no change in z,c-flag
	jr	z,STEP0		;if STEP value = 0
	call	c,EXFAC		;if STEP value < 0
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
	ld	a,FOR
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
	inc	hl
	cp	','
	jp	nz,INTPEND
	ld	b,a		;not variable name
	jp	NEXTLP


;+ operator
O_PLS:
	ld	a,(ARGTYP)
	or	a
	jr	z,ADDF

PLSSTR:
	sub	03h		;string and string
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

	sub	h
	jr	c,ADD1GT2	;|FAC1| > |FAC2|
	jr	nz,ADD1LT2	;|FAC1| < |FAC2|

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

;set 4-byte integer in FAC1
;input: de-hl
SETI4:
	ld	(FAC1),hl
	ld	(FAC1+2),de
	ret


;FAC1 = FAC1 * 10
;input: FAC1
;output: FAC1
;destroy: FAC2,af,bc,de,hl
MULF10:
	ld	hl,TEN
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

	ld	a,(FAC1+4)
	or	a
	ret	z

	ld	hl,FAC2+4
	ld	a,(hl)
	or	a
	jp	z,SETZERO

	dec	l		;dec hl,FAC2+3
	ld	a,(hl)
	set	7,(hl)
	ld	l,FAC1+3-(FAC1+3)/256*256	;hl=FAC1+3
	xor	(hl)
	push	af		;sign
	set	7,(hl)

	ld	bc,0000h
	ld	d,b
	ld	e,c

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
	ld	(hl),a		;exponent
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


;FAC1 = FAC1 * 0.1
;input: FAC1
;output: FAC1
;destroy: FAC2,af,bc,de,hl
DIVF10:
	ld	hl,EM1
	call	SETF2
	jp	MULF

;0.1
EM1:
	db	0cdh, 0cch, 0cch, 4ch, 7dh


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
	ld	a,(FAC1+4)
	or	a
	ret	z

	ld	hl,FAC2+4
	ld	a,(hl)
	or	a
	jp	z,DIV0ERR

	dec	l		;dec hl,FAC2+3
	ld	a,(hl)
	set	7,(hl)
	ld	l,FAC1+3-(FAC1+3)/256*256	;hl=FAC1+3
	xor	(hl)
	push	af		;sign
	set	7,(hl)

	ld	hl,0000h
	ld	d,h
	ld	e,l

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
	jp	p,SETZERO
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


;print 1-byte integer (unsigned)
;input: a
;destroy: af,bc,de,hl
_PUTI1:	ds	PUTI1-_PUTI1
	org	PUTI1

	ld	h,0
	ld	l,a
	jp	PUTI2


;INT() function
;(round toward minus infinity)
F_INT:
	call	CHKNUM
INT:
	call	CPYFAC
	call	GETINT
	call	CMPF
	ret	z
	ret	c
	ld	hl,MNSONE
	jp	SETADDF


;check BASIC mode
;output: a=mode, z(1=mode 5)
;destroy: f
_CHKMOD:
	ds	CHKMOD-_CHKMOD
	org	CHKMOD

	ld	a,(MODE)
	cp	04h
	ret


;convert string to float
;input: a=max length, hl=program address
;output: FAC1, hl=next address (except for VAL())
;destroy: FAC2, af,bc,de
ATOF:
	ld	b,a
	call	SKIPSPB
	jp	z,SETZERO
	cp	'&'
	jp	z,ATOFHEX
	dec	hl

	call	ATOIF

	call	SKIPSPB
	ret	z
	ld	c,00h		;after-dot counter
	cp	'.'
	jr	z,ATOFDOT
	dec	b
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
	djnz	ATOFLP1
	jr	ATOFEXP

CHKEXP:
	call	SKIPSPB
	jr	z,ATOFEXP
	dec	b
	or	20h
	cp	'e'
	jr	z,ATOFEXP

	dec	hl
	ld	b,00h
ATOFEXP:
	ex	de,hl
	call	PUSHF1
	ex	de,hl

	push	bc		;c=after-dot counter
	ld	a,b
	or	a
	jr	z,ATOFE0
	call	ATOIF
	jr	CALCEXP

ATOFE0:
	push	hl		;program address
	call	SETZERO
	pop	hl		;program address

CALCEXP:
	pop	bc		;c=after-dot counter
	push	hl		;program address
	call	PUSHF1
	ld	l,c
	call	I1TOF
	call	POPF2
	call	SUBF21

	pop	de		;program address
	call	CPYFAC
	call	POPF1
	push	de		;program address

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
	pop	hl		;program address
	ret

ATOFHEX:
	dec	hl
	dec	b
	call	HEX
	ex	de,hl
	call	S2TOF
	ex	de,hl
	ret


;convert hexadecimal string to integer
;input: hl="&" address, b=max length-1("&")
;output: de=decimal, hl=next address
;destroy: af,bc
HEX:
	ld	de,0000h
HEXLP1:
	inc	hl
	ld	a,(hl)
	cp	' '
	jr	nz,CHKH
	djnz	HEXLP1
	ret

CHKH:
	or	20h
	cp	'h'
	jp	nz,SNERR
	dec	b
	ld	c,05h

HEXLP2:
	inc	hl
	ld	a,(hl)
	cp	' '
	jr	z,HEXEND
	cp	DEF		;93h
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
HEXEND:
	djnz	HEXLP2
	inc	hl
	ret

HEXDEF:
	ld	d,e
	ld	e,0deh
	ld	a,0fh
	jr	HEX0F


;skip space and decrement b
;input: hl=program address, b=length
;output: a=data, hl=next address, b=remained length, z-flag(1:b=0)
;destroy: f
SKIPSPB:
	inc	b
SKIPSPBLP:
	dec	b
	ret	z
	ld	a,(hl)
	inc	hl
	cp	' '
	ret	nz
	jr	SKIPSPBLP


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


;print 2-byte integer (unsigned)
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

FTOA2:
	ld	hl,E8
	call	SETCMPF
	jr	nc,LARGE	;FAC1 >= 1e8
	ld	hl,EM2
	call	SETCMPF
	jr	c,SMALL		;FAC1 < 1e-2

	ld	b,09h
FTOALP1:
	push	bc
	ld	hl,E7
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

	ld	hl,INTE7
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
	ld	b,00h
SMALLLP:
	dec	b
	push	bc
	call	MULF10
	ld	hl,E0
	call	SETCMPF
	pop	bc
	jr	c,SMALLLP
	jr	LARSMA

LARGE:
	ld	b,00h
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
	ld	(hl),'+'
	pop	af		;
	jr	c,LARPLS
	neg
	ld	(hl),'-'

LARPLS:
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


;99999999.5
E8:
	db	0f0h, 1fh, 0bch, 3eh, 9bh

;9999999.95
E7:
	db	0f3h, 7fh, 96h, 18h, 98h

;9.99999995
E2:
	db	0f3h, 0ffh, 0ffh, 1fh, 84h

;0.999999995
E0:
	db	0ebh, 0ffh, 0ffh, 7fh, 80h

;0.00999999995
EM2:
	db	30h, 0ah, 0d7h, 23h, 7ah

INTE7:
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
;destroy: FAC1,af,b,de,hl
MULRND:
	ld	hl,ZERO
	ld	de,RSEED
	call	SETF
	xor	a
	ld	(FAC1+4),a

	exx			;
	push	hl
	push	de
	push	bc
	exx			;

	ld	hl,(FAC2)
	ld	de,(FAC2+2)

	ld	b,20h
MULRLP1:
	srl	d
	rr	e
	rr	h
	rr	l
	exx			;
	jr	nc,MULRNC

	ld	de,FAC1
	ld	hl,RSEED
	or	a
	ld	b,05h
MULRLP2:
	ld	a,(de)
	adc	a,(hl)
	ld	(hl),a
	inc	e		;inc de
	inc	l		;inc hl
	djnz	MULRLP2

MULRNC:
	call	SLAF1
	ld	hl,FAC1+4
	rl	(hl)
	exx			;
	djnz	MULRLP1

	exx			;
	pop	bc
	pop	de
	pop	hl
	exx			;
	ret


;EXP() function
;exp(x)=2^n*e^a, n=[x/log2], a=x-n*log2
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
	jr	nc,EXPLARGE

	call	INT		;n=[x/log2]
	call	FTOI
	push	de		;n
	ld	hl,LOG2
	call	SETF2
	call	MULF		;n*log2
	pop	de		;n
	call	POPF2
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

	pop	de		;n
;	ld	a,(FAC1+4)	;=81h?
	ld	a,81h
	add	a,e
	ld	(FAC1+4),a	;e^a*2^n
	ld	a,c		;c=0
	adc	a,d
	ret	z
	jp	OVERR

EXPLARGE:
	bit	7,l		;(FAC1+3)
	jp	z,OVERR		;x>0
	call	POPF1
	jp	SETZERO		;x<0

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
	call	SETF2
	call	MULF		;n*log(2)
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
	jp	NEGABS		;y=odd


;COS() function
;cos(x)=sin(pi/2-|x|)
F_COS:
	call	CHKNUM
COS:
	call	NEGABS2
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
	call	SETF2
	call	MULF		;int(x/2pi)*2pi
	call	POPF2		;x
	call	SUBF21		;x'=x-int(x/2pi)*2pi

	call	SINRANGE	;-pi~+pi
	call	SINRANGE	;-pi/2~+pi/2

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
	call	NEGABS2		;-x'^2/b/(b+1)*y (y>0)
	call	INCF1		;y=1-x'^2/b/(b+1)*y

	pop	bc
	call	POPF2		;x'^2
	dec	b
	djnz	SINLP

	call	POPF2		;x'
	call	MULF

;if |sin(x)|>1
	ld	a,(FAC1+4)
	sub	81h
	ret	nz
	ld	(FAC1),a	;=0
	ret

;-pi/2~+pi/2
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


;input: bc=X, de=Y
;output: a=color
;destroy: f,bc,de,hl
POINT:
	call	CHKMOD
	jp	z,POINT66
	ld	a,(SCREEN1)
	dec	a
	jr	z,POINT2
	jp	p,POINTG

;screen mode 1
POINT1:
	call	GXY2AD60
	ld	a,(hl)
	sub	1fh
	ret

;graphic mode (screen mode 3,4)
POINTG:
	call	GXY2GAD60
	ld	a,(hl)
	and	d
	rlca
POINTLP:
	rrca
	rrc	d
	jr	nc,POINTLP
POINTOK:
	rrc	d
	ret	nc		;screen mode 4
	inc	a		;screen mode 3
	ret

;screen mode 2
POINT2:
	push	bc		;x
	call	GXY2AD60
	pop	bc		;x
	call	MASK2
	ld	b,(hl)		;
	inc	h
	inc	h
	and	(hl)
	ret	z

	ld	a,b		;
	rrca
	rrca
	ld	a,(hl)
	rla
	rla
	rla
	and	07h
	inc	a
	ret


;SCREEN() function
;not called but jumped
F_SCRN:
	call	CHKLPAR
	call	INT1ARG2
	call	CHKRPAR
	ld	h,b
	ld	l,e
	inc	h
	inc	l
	call	XY2AD
	call	CHKMOD
	jp	z,F_SCRN66
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
;output: de=jump address
;destroy: af
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


;get VRAM attirbute address (screen mode 1,2)
;hl=(VRAM)+X/8+Y/12*32
;input: bc=graphic X, de=graphic Y
;output: hl=VRAM address, d=Y mod 12
;destroy: af,bc,e
GXY2AD:
	call	CHKMOD
	jp	z,GXY2AD66
GXY2AD60:
	call	CHKGXY
	call	DIV12
	ex	de,hl
	ld	d,a
	add	hl,hl		;*2
	add	hl,hl		;*4
	add	hl,hl		;*8
	add	hl,hl		;*16
	add	hl,hl		;*32
	ld	a,(VRAM)
	ld	b,a
	srl	c
	srl	c
	srl	c
	add	hl,bc
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


;ldir or lddr
;input: bc,de,hl,z(0=ldir,1=lddr)
;output: bc=0,de,hl
;destroy: f
LDIDR:
	jr	z,LDIDRZ
	ldir
	ret
LDIDRZ:
	lddr
	ret


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


;calculate line status address and bit (connection to next line)
;input: l=y+1
;output: hl=line status address, a=bit
;destroy: f
CALCLINE:
	push	bc
	ld	a,l
	ld	hl,LINEST-1
CALCLLP1:
	inc	hl
	sub	08h
	jr	nc,CALCLLP1
	add	a,09h
	ld	b,a
	ld	a,80h
CALCLLP2:
	rlca
	djnz	CALCLLP2
	pop	bc
	ret


;check external ROM (4000=CD? 6000=CD?)
;input: hl=4003h or 6003h
;output: a=mode, z-flag(0=found, 1=not found)
;destroy: f,bc,de
CHKEXTCD:
	ld	d,'C'
	db	01h		;ld bc,

;check external ROM (4000=AB? 6000=AB?)
;input: hl=4003h or 6003h
;output: a=mode, z-flag(0=found, 1=not found)
;destroy: f,bc,de
CHKEXTAB:
	ld	d,'A'
	ld	e,d
	inc	e		;de="AB" or "CD"
	push	hl
	ld	b,(hl)
	dec	l
	ld	c,(hl)
	dec	l
	ld	a,(hl)
	dec	l
	ld	h,(hl)
	ld	l,a
	rst	CPHLDE
	jr	nz,NOABCD	;"AB" "CD" not found
	ld	h,b
	ld	l,c
	call	JPHL
	db	0f6h		;or afh, reset z-flag

NOABCD:
	xor	a		;set z-flag
	ld	a,(MODE)
	pop	hl
	ret


;60 ROM end
_4000H:	ds	4000h-_4000H


;read RAM data
;input: hl
;output: a
;destroy: none
;fe8dh
READRAM_SRC:
	call	CHGRAM
	ld	a,(hl)
	jr	CHGROM_SRC


;LDIR in RAM
;input: bc,de,hl
;output: bc=0, de=de+bc, hl=hl+bc
;destroy: f (no changes in szc-flag)
;fe93h
LDIRRAM_SRC:
	call	CHGRAM
	ldir
;	jp	CHGROM


;change 0000-7fffh to BASIC ROM
;destroy: none
;fe98h: used by HURRY FOX
CHGROM_SRC:
	push	af
	ld	a,11h
;	ld	a,(PORTF0H)
	out	(0f0h),a
	ei
	pop	af
	ret
	nop


;fea1h
OUTF0H_SRC:
	out	(0f0h),a
	pop	af
	ret


;LDDR in RAM
;input: bc,de,hl
;output: bc=0, de=de-bc, hl=hl-bc
;destroy: f (no changes in szc-flag)
;fea5h
LDDRRAM_SRC:
	call	CHGRAM
	lddr
	jr	CHGROM_SRC
RAMEND:


;check exteral ROM (6000-) and select mode
;output: z(1=mode 5), a=mode-1(if z=0)
CHKEXT6000:
	ld	hl,MODE
	inc	(hl)
	inc	(hl)		;mode1->mode3, mode2->mode4
	ld	hl,6003h
	call	CHKEXTAB
	ret	nz		;AB found, skip menu
	call	CHKEXTCD
	call	CHGROM		;0000-7fff:BASIC ROM
	jp	SELMOD


;continued from CHGMOD for mode 5
CHGMOD66:
	ld	a,(SCREEN2)
	ld	b,a
	ld	a,(SCREEN3)
	cp	b
	call	z,CHGDSP

	ld	a,(SCREEN1)
	dec	a
	jr	nz,CHGMOD66NZ
	call	CNVATT1
	or	80h
	ld	(hl),a

CHGMOD66NZ:
	dec	a
	ld	a,CLMN66
	jr	nz,CHGMOD66OK
	ld	a,CLMN66/2
CHGMOD66OK:
	ld	(WIDTH),a
	ld	bc,CLMN66*ROWS66-1
	ret


;continued from CHGDSP for mode 5
CHGDSP66:
	push	af

	ld	a,(SCREEN2)
	ld	b,a
	ld	a,(SCREEN3)
	cp	b
	jr	nz,CHGDSP66NZ
	ld	hl,VRAM
CHGDSP66NZ:
	inc	hl
	ld	a,(hl)		;(SCREEN1)
	cp	02h
	ld	a,00h
	jr	c,CHGDSP12
	jr	nz,CHGDSP4
CHGDSP3:
	add	a,04h
CHGDSP12:
	add	a,04h
CHGDSP4:
	out	(0c1h),a

	ld	a,(COLOR3)
	out	(0c0h),a

	pop	af
	rrca
	ret


;set command/function table for mode3,4,5
SETTBL:
	ld	hl,F_HEX
	ld	(0fb07h),hl	;HEX$
	ld	hl,C_CLDEX
	ld	(0faa7h),hl	;CLOAD
	ld	hl,C_CSVEX
	ld	(0faa9h),hl	;CLOAD

	ld	hl,CMDLST66
	ld	de,CMDTBL+(CMDLAST-80h+1)*2
	ld	bc,FNCLST66-CMDLST66
	ldir

;	ld	hl,FNCLST66
	ld	de,FNCTBL+(FNCLAST-FNC1ST+1)*2
	ld	bc,CMDNAME66-FNCLST66
	ldir


	ld	de,SNERR

	ld	a,(DRVBIT)
	or	a
	jr	nz,SETTBLNZ

;BLOAD-KILL -> ?SN Error
	ld	hl,0fabfh
	ld	b,0fh
SETTBLLP1:
	ld	(hl),e
	inc	hl
	ld	(hl),d
	inc	hl
	djnz	SETTBLLP1

SETTBLNZ:
	call	CHKMOD
	ret	z

;TALK-DELETE -> ?SN Error
	ld	hl,0faddh
	ld	b,04h
SETTBLLP2:
	ld	(hl),e
	inc	hl
	ld	(hl),d
	inc	hl
	djnz	SETTBLLP2

;DSKI$-MKS$ -> ?SN Error
	ld	hl,0fb23h
	ld	b,07h
SETTBLLP3:
	ld	(hl),e
	inc	hl
	ld	(hl),d
	inc	hl
	djnz	SETTBLLP3
	ret


CNVIL66:
	ld	a,b
	cp	CMDLAST+1
	jr	z,IL661
	cp	CLAST66+1
	jr	z,IL662
	cp	FNCLAST+1
	jr	z,IL663
	cp	FLAST66+1
	jp	c,ILLP2
	jp	ILNC

IL661:
	ld	hl,CMDNAME66
	jp	ILLP2
IL662:
	ld	hl,FNCNAME
	ld	b,TAB
	jp	ILLP2
IL663:
	ld	hl,FNCNAME66
	jp	ILLP2


CMDLST66:
;ab-af
	dw				C_RENM,	C_CRCL,	C_GET,	C_PUT,	C_BLD
;b0-bf
	dw	C_BSAV,	C_FLS,	C_LFLS,	C_LOAD,	C_MRG,	C_NAME,	C_SAVE,	C_FLD
	dw	C_LSET,	C_RSET,	C_OPEN,	C_CLOS,	C_DSKO,	C_KILL,	C_TALK,	C_MON
;c0-c1
	dw	C_KANJ,	C_DEL


FNCLST66:
;f2-f9
	dw			F_PAD,	F_DSKI,	F_LOF,	F_LOC,	F_EOF,	F_DSKF
	dw	F_CVS,	F_MKS


CMDNAME66:
	db							'R'+80h,"ENUM"
	db	'C'+80h,"IRCLE",'G'+80h,"ET",	'P'+80h,"UT",	'B'+80h,"LOAD"

	db	'B'+80h,"SAVE",	'F'+80h,"ILES",	'L'+80h,"FILES",'L'+80h,"OAD"
	db	'M'+80h,"ERGE",	'N'+80h,"AME",	'S'+80h,"AVE",	'F'+80h,"IELD"

	db	'L'+80h,"SET",	'R'+80h,"SET",	'O'+80h,"PEN",	'C'+80h,"LOSE"
	db	'D'+80h,"SKO$",	'K'+80h,"ILL",	'T'+80h,"ALK",	'M'+80h,"ON"

	db	'K'+80h,"ANJI",	'D'+80h,"ELETE"


FNCNAME66:
	db					'P'+80h,"AD",	'D'+80h,"SKI$"
	db	'L'+80h,"OF",	'L'+80h,"OC",	'E'+80h,"OF",	'D'+80h,"SKF"

	db	'C'+80h,"VS",	'M'+80h,"KS$",	80h


;error ID for extended BASIC
ERRIDEX:
	db	"FO", "IE", "BN"
	db	"FF", "BM", "AO", "IO", "FE", "DF", "EF", "NM"
	db	"DS", "AT", "DN", "TS", "RD", "SP", "CF", "FW"


FOERR:
	ld	e,2ah
	db	01h		;ld bc,****
IEERR:
	ld	e,2ch
	db	01h		;ld bc,****
BNERR:
	ld	e,2eh
	db	01h		;ld bc,****
FFERR:
	ld	e,30h
	db	01h		;ld bc,****
BMERR:
	ld	e,32h
	db	01h		;ld bc,****
AOERR:
	ld	e,34h
	db	01h		;ld bc,****
IOERR:
	ld	e,36h
	db	01h		;ld bc,****
FEERR:
	ld	e,38h
	db	01h		;ld bc,****
DFERR:
	ld	e,3ah
	db	01h		;ld bc,****
EFERR:
	ld	e,3ch
	db	01h		;ld bc,****
NMERR:
	ld	e,3eh
	db	01h		;ld bc,****
DSERR:
	ld	e,40h
	db	01h		;ld bc,****
ATERR:
	ld	e,42h
	db	01h		;ld bc,****
DNERR:
	ld	e,44h
	db	01h		;ld bc,****
TSERR:
	ld	e,46h
	db	01h		;ld bc,****
RDERR:
	ld	e,48h
	db	01h		;ld bc,****
SPERR:
	ld	e,4ah
	db	01h		;ld bc,****
CFERR:
	ld	e,4ch
	db	01h		;ld bc,****
FWERR:
	ld	e,4eh

	jp	ERROR


;read/write/check disk
;input:
; a=the number of sector, b=track, c=sector, de=address
; ix=work address, (ix+00h)=drive, (ix+11b)=error count
; c-flag=0,z-flag=0: read
; c-flag=0,z-flag=1: check
; c-flag=1: write
;output: c-flag=1 if error, (de=de+data size)
;destroy: (de)
_DISK:	ds	DISK-_DISK
	org	DISK
	jp	DSKMAIN


;initialize FDD
;output: a=ndrive, z-flag=(1:no drive)
;destroy: f,(bc,de,hl)
_INIDSK:ds	INIDSK-_INIDSK
	org	INIDSK

	push	bc
	ld	c,00h		;ndrive
	call	FDSTBY


;for no FDD
	xor	a
	ld	(DRVBIT),a

;for emulator?
	in	a,(FDCSTAT)
	inc	a
	jr	z,INIDSKEND

	call	MOTORABON
	call	SPECIFY
DRIVEA:
	xor	a		;drive
	ld	(DRVBIT),a
	call	RECALIB
	jr	c,DRIVEB
	ld	a,10h
	ld	(DRVBIT),a
	inc	c
DRIVEB:
	ld	a,01h		;drive
	call	RECALIB
	call	FDOFF
	ld	a,(DRVBIT)
	jr	c,INIDSKEND
	or	20h
	ld	(DRVBIT),a
	inc	c
INIDSKEND:
	ld	a,c
	or	a		;z-flag=1 if no FDD
	pop	bc
	ret


;continued from DISK
DSKMAIN:
	push	af
	push	hl
	push	de
	push	bc

	ld	(RMSECT),a
	ld	a,02h
	jr	c,SETRWC	;2=write
	ld	a,01h
	jr	z,SETRWC	;1=check
	xor	a		;0=read
SETRWC:
	ld	(RDWRCHK),a
	ld	a,b
	cp	28h
	jr	nc,DSKERR	;if track>39
	ld	(TRACK),a
	ld	a,c
	cp	11h
	jr	nc,DSKERR	;if sector>16
	ld	(SECTOR),a
	ld	a,(ix+00h)
	cp	02h
	jr	nc,DSKERR	;if drive>1
	ld	(DRIVE),a
	call	MOTORON
;	ld	a,(DRIVE)
;	call	RECALIB
;	jr	c,DSKERR
	call	FDSTBY
	call	SEEK
	jr	c,DSKERR
DSKLP:
	ld	a,(RMSECT)
	cp	04h
	jr	c,LT4SECT
	ld	a,04h
LT4SECT:
	ld	(NSECT),a
	ld	a,(RDWRCHK)
	cp	02h		;write
	call	z,WT4SECT	;z-flag is set
	call	nz,RD4SECT
	jr	c,DSKERR
	ld	a,(RMSECT)
	sub	05h
	jr	c,DSKEND
	inc	a
	ld	(RMSECT),a
	ld	a,(SECTOR)
	add	a,04h
	ld	(SECTOR),a
	jr	DSKLP
DSKEND:
	call	FDOFF
	pop	bc
	pop	hl		;de
	ld	a,(RDWRCHK)
	cp	01h		;1=check
	jr	nz,NOCHECK	;de changes (read/write)
	ex	de,hl		;pop de (check)
NOCHECK:
	pop	hl
	pop	af
	or	a		;reset c-flag: ok
	ret

DSKERR:
	call	FDOFF
	pop	bc
	pop	de
	pop	hl
	ld	a,(ix+1bh)
	inc	a
	ld	(ix+1bh),a
	cp	0ah
	jr	nc,ERRJMP
	pop	af
	scf			;set c-flag: error
	ret

ERRJMP:
	pop	af
	inc	sp		;return address
	inc	sp		;return address
	push	hl
	ld	hl,(ERRAD)
	ex	(sp),hl
	ret


;input: de
;output: de=de+data size
;destroy: af,bc,hl
RD4SECT:
	call	SETBSIZ
	call	RWDATA
	ret	c		;error
	ex	de,hl
	ld	c,FDBUF
	ld	a,(NSECT)
RDATALP:
	push	af		;
	dec	hl
	inc	h
	ld	b,0ffh
	ld	a,(RDWRCHK)
	or	z
	jr	nz,CHECK
	indr			;8MSB=ff-01
	ind			;8MSB=00
	inc	h
CHECK:
	inc	hl
	inc	c
	pop	af		;
	dec	a
	jr	nz,RDATALP
	ex	de,hl
	ret


;write 4 sectors
;input: de
;output: de=de+data size
;destroy: af,bc,hl
WT4SECT:
	call	SETBSIZ
	ex	de,hl
	ld	c,FDBUF
	ld	a,(NSECT)
WDATALP:
	dec	hl
	inc	h
	ld	b,00h
	otdr			;8MSB=ff-00!!
	inc	hl
	inc	h
	inc	c
	dec	a
	jr	nz,WDATALP
	ex	de,hl
	call	RWDATA
	ret


;read data or write data
;destroy: af,b,hl
RWDATA:
	xor	a
	ld	(RETRY),a
RWDTLP:
	call	FDCRDY
	call	DRVRDY
	ld	a,(RDWRCHK)
	cp	02h		;0=read,1=check,2=write
	ld	a,46h		;46h=read data command
	jr	nz,SETCMD
	dec	a		;45h=write data command
SETCMD:
	call	WRTFDC
	di
	ld	a,(DRIVE)	;drvie/head
	call	WRTFDC
	ld	a,(TRACK)	;track
	call	WRTFDC
	xor	a		;head
	call	WRTFDC
	ld	a,(SECTOR)	;sector
	call	WRTFDC
	ld	a,01h		;sector length (128*2^n byte/sector)
	call	WRTFDC
	ld	a,10h		;EOT (End Of Track)
	call	WRTFDC
	ld	a,14h		;GPL (GaP Length)
	call	WRTFDC
	ld	a,0ffh		;DTL (DaTa Length)
	call	WRTFDC
	ld	a,(RDWRCHK)
	cp	02h		;0=read,1=check,2=write
	ld	a,0f2h		;read
	jr	nz,SETDMA
	ld	a,0f0h		;write
SETDMA:
	out	(FDCNTL),a
	ei
	call	INTRPT
	call	FDSTBY
	ld	hl,STATBF
	ld	b,07h
STATLP:
	call	REDFDC
	ld	(hl),a
	inc	hl
	djnz	STATLP
	ld	a,(STATBF)
	and	11000000b
	ret	z		;NT
	ld	hl,RETRY
	inc	(hl)
	ld	a,(hl)
	add	a,0-0ah
	jr	nc,RWDTLP	;if retry<10
	ret			;c-flag=1,z-flag=1


;FDC specify command
;destroy: af
SPECIFY:
	call	FDCRDY
	call	DRVRDY
	ld	a,03h		;specify
	call	WRTFDC
	ld	a,0bfh		;step rate time=5ms/10ms,
				;head unload time=240ms/480ms
	call	WRTFDC
	ld	a,26h		;head load time=38ms/76ms, DMA mode
	call	WRTFDC
	ret


;FDC recalibrate command
;input: a=drive
;output: c-flag(1=error)
;destroy: af,b
RECALIB:
	push	af
	call	FDCRDY
	call	DRVRDY
	ld	a,07h		;recalibrate command
	call	WRTFDC
	pop	af		;drive/head
	call	WRTFDC
	ld	b,00h		;track
	jr	SENSE


;FDC seek command
;output: c-flag(1=error)
;destroy: af,b
SEEK:
	call	FDCRDY
	call	DRVRDY
	ld	a,0fh		;seek command
	call	WRTFDC
	ld	a,(DRIVE)	;drive/head
	call	WRTFDC
	ld	a,(TRACK)	;track
	call	WRTFDC
	ld	b,a		;track
;	jr	SENSE


;FDC sense command
;input: b=track
;output: c-flag(1=error)
;destroy: af
SENSE:
	call	INTRPT
	call	FDCRDY
	ld	a,08h		;sense interrupt status command
	call	WRTFDC
	call	REDFDC		;st0
	ld	(STATBF),a
	call	REDFDC		;track
	ld	(STATBF+1),a
;error check
	ld	a,(STATBF)
	and	11010000b	;bit6-7:interrupt code, bit4=fault
	jr	nz,SENSEER
	ld	a,(STATBF)
	and	00100000b	;seek End
	jr	z,SENSE
	ld	a,(STATBF+1)	;track
	cp	b
	ret	z
SENSEER:
	scf
	ret


;wait for FDC ready
;destroy: af
FDCRDY:
	in	a,(FDCSTAT)
	and	00010000b
	jr	nz,FDCRDY
	ret


;wait for drives ready
;destroy: af
DRVRDY:
	in	a,(FDCSTAT)
	and	00001111b
	jr	nz,DRVRDY
	ret


;wait for FDC interrupt
;destroy: af
INTRPT:
	ld	a,0feh
	out	(B2CNTL),a	;use b2-bit0?
	in	a,(FDCINT)	;bit0=1:interrupt
	rrca
	jr	nc,INTRPT
	ret


;read data from FDC
;destroy: f
;output: a
REDFDC:
	in	a,(FDCSTAT)
;	and	11000000b
;	cp	11000000b	;RQM=1,DIO=1(output)
;	jr	nz,REDFDC
	rlca
	jr	nc,REDFDC	;wait for RQM=1
	rlca
	jr	c,READOK	;DIO=1(output)
	ld	a,0ffh
	ret
READOK:
	in	a,(FDCDATA)
	ret


;write data to FDC
;destroy: none
WRTFDC:
	push	af
WRTFDCLP:
	in	a,(FDCSTAT)
;	and	11000000b
;	cp	10000000b	;RQM=1,DIO=0(input)
;	jr	nz,WRTFDCLP
	rlca
	jr	nc,WRTFDCLP	;wait for RQM=1
	rlca
	jr	nc,WRTOK	;DIO=0(input)
	call	REDFDC
	jr	WRTFDCLP
WRTOK:
	pop	af
	out	(FDCDATA),a
	ret


;set FD buffer size
;destroy: af
SETBSIZ:
	ld	a,(NSECT)
	sub	10h
	cpl
	out	(BUFSIZ),a
	ret


;motor on
;input: a=drive (0 or 1)
;destroy: af
MOTORON:
	or	a
	jr	z,AON
	dec	a
	ret	nz
BON:
	in	a,(MOTORST)	;motor status
	and	02h
	ret	z		;on status
	and	0fdh		;driveB on (bit1=0)
	jr	ABON
AON:
	in	a,(MOTORST)	;motor status
	and	01h
	ret	z		;on status
	and	0feh		;driveA on (bit0=0)
ABON:
	out	(MOTOR),a	;motor on

	ld	a,08h
	out	(ADJUST),a	;compensation?

	push	bc
	ld	bc,3000h	;about 0.6sec
	call	WAITLP
	pop	bc
	ret

MOTORABON:
	in	a,(MOTORST)	;motor status
	and	03h
	ret	z		;A,B=on
	and	0fch
	jr	ABON


;set FDD stand-by mode
;destroy: a
FDSTBY:
	ld	a,0f3h
	out	(FDCNTL),a
	ret


;set FDD off
FDOFF:
;destroy: a
	ld	a,0f7h
	out	(FDCNTL),a
	ret


;HEX$() function
F_HEX:
	call	CHKNUM
	call	FTOI

	ld	hl,FAC3
	ld	(STRDSC1+2),hl
	ld	c,0		;length
	ld	a,d
	call	CNVHEX2
	ld	a,e
	call	CNVHEX2
	jr	nz,HEXNZ	;if c<>0
	ld	(hl),'0'
	inc	c
HEXNZ:
	ld	a,c
	ld	(STRDSC1),a
	call	MAKESTR
	jp	INKYEND


;input: a=data, c=length, hl=address
;output: c=length, z-flag(1:c=0)
;destroy: af
CNVHEX2:
	ld	b,a
	rrca
	rrca
	rrca
	rrca
	call	CNVHEX1
	ld	a,b
;	call	CNVHEX1
;	ret

CNVHEX1:
	and	0fh
	jr	nz,CNVHEXNZ
	inc	c
	dec	c
	ret	z
CNVHEXNZ:
	cp	0ah
	sbc	a,69h
	daa
	ld	(hl),a
	inc	hl
	inc	c
	ret


;CIRCLE command work area
XRAD	equ	GRPX2		;x-radius
YRAD	equ	GRPY2		;y-radius
DEDX	equ	FAC1-1		;de/dx
DEDY	equ	FAC2-1		;de/dy
ERR	equ	FAC3		;error

STARTX	equ	0ff78h
STARTY	equ	0ff7ah
ENDX	equ	0ff7ch
ENDY	equ	0ff7eh
CRCLFLG	equ	0ff81h


CRCLFDATA:
	db	77h, 0ffh, 0ffh, 0ffh	;end quad.-start quad.=4
	db	66h, 77h, 0eeh, 0ffh	;end quad.-start quad.=3
	db	44h, 66h, 0cch, 0eeh	;end quad.-start quad.=2
	db	00h, 44h, 88h, 0cch	;end quad.-start quad.=1
	db	00h, 00h, 00h, 88h	;end quad.-start quad.=0

CRCLMAIN:
	ld	hl,(XRAD)
	ld	de,(ASPECT)
	call	MULINT2
	call	ROUNDI2
	ld	(YRAD),hl	;yrad=r*a

CRCLMAIN2:
	call	POPF1		;end angle
	call	POPF2		;start angle


	ld	hl,FAC1+3
	ld	a,(hl)
	push	af		;sign of end angle
	res	7,(hl)
	ld	hl,FAC2+3
	ld	a,(hl)
	push	af		;sign of start angle
	res	7,(hl)

	call	PUSHF2		;end angle
	call	CMPF

	sbc	a,a
	inc	a		;start<=end?
	push	af

	call	ANGL2XY
	ld	(ENDX),hl
	ld	(ENDY),de

	pop	af
	rlca
	rlca
	or	b
	ld	d,a		;bit2=start<=end?, bit10=end quad.

	call	POPF1		;start angle
	push	de

	call	ANGL2XY
	ld	(STARTX),hl
	ld	(STARTY),de

	pop	af
	rlca
	rlca
	or	b
	ld	(CRCLFLG),a	;bit4=start<=end?, bit32=end quad., bit10=start quad.


	ld	hl,(XRAD)
	ld	d,h
	ld	e,l
	call	MULINT2		;a*a=aa
	push	hl		;aa
	push	de
	add	hl,hl
	rl	e
	rl	d
	add	hl,hl
	rl	e
	rl	d
	ld	b,d
	ld	c,e
	ex	de,hl
	exx			;bcde'=4aa

	ld	hl,(YRAD)
	ld	d,h
	ld	e,l
	call	MULINT2		;b*b=bb
	push	hl		;bb
	push	de
	add	hl,hl
	rl	e
	rl	d
	add	hl,hl
	rl	e
	rl	d
	add	hl,hl
	rl	e
	rl	d
	push	hl		;8bb
	push	de

;de/dx_ini=8abb
	ex	de,hl
	ld	hl,(XRAD)
	call	MULINT2		;a*8bb=8abb (low)
	ld	(DEDX),hl
	ld	(DEDX+2),de
	ld	hl,(XRAD)
	pop	de		;8bb (high)
	push	de
	call	MULINT2		;a*8bb=8abb (high)
	ld	bc,(DEDX+2)
	add	hl,bc
	ld	(DEDX+2),hl
	jr	nc,CRCLNC1
	inc	de
CRCLNC1:
	ld	(DEDX+4),de

;err_ini=bb(1-4a)
	srl	d
	rr	e
	ld	(ERR+4),de	;4abb
	ld	hl,(DEDX+2)
	rr	h
	rr	l
	ld	(ERR+2),hl
	ld	hl,(DEDX)
	rr	h
	rr	l
	ld	(ERR),hl

	pop	bc		;8bb
	pop	hl
	pop	de		;bb(high)
	ex	(sp),hl		;bb(low)
	push	bc

	push	de
	ld	de,(ERR)
;	or	a
	sbc	hl,de
	ld	(ERR),hl
	pop	hl
	ld	de,(ERR+2)
	sbc	hl,de
	ld	(ERR+2),hl
	ld	hl,0000h
	ld	de,(ERR+4)
	sbc	hl,de
	ld	(ERR+4),hl

;de/dy_ini=-4aa
	exx
	push	de		;4aa
	push	bc
	xor	a
	ld	h,a
	ld	l,a
	sbc	hl,de
	ld	(DEDY),hl
	ld	h,a
	ld	l,a
	sbc	hl,bc
	ld	(DEDY+2),hl
	sbc	a,00h
	ld	h,a
	ld	l,a
	ld	(DEDY+4),hl
	pop	bc
	pop	hl
	add	hl,hl
	ex	de,hl
	rl	c
	rl	b		;8aa
	exx

	ld	hl,(XRAD)	;x=x-radius
	ld	(GRPX3),hl
	ld	hl,0000h	;y=0
	ld	(GRPY3),hl

CRCLLP1:
	ld	b,01h
	call	CRCLPSET
	ld	hl,(GRPY3)
	inc	hl
	ld	(GRPY3),hl

;de/dy+=8aa
	exx
	ld	hl,(DEDY)
	add	hl,de
	ld	(DEDY),hl
	ld	hl,(DEDY+2)
	adc	hl,bc
	ld	(DEDY+2),hl
	jr	nc,CRCLNC2
	ld	hl,(DEDY+4)
	inc	hl
	ld	(DEDY+4),hl
CRCLNC2:
	exx

;err+=de/dy
	ld	hl,(ERR)
	ld	de,(DEDY)
	add	hl,de
	ld	(ERR),hl
	ld	hl,(ERR+2)
	ld	de,(DEDY+2)
	adc	hl,de
	ld	(ERR+2),hl
	ld	hl,(ERR+4)
	ld	de,(DEDY+4)
	adc	hl,de
	ld	(ERR+4),hl

;if err>=0 and x>0 then x=x-1:de/dx-=8bb:err-=de/dx
	ld	a,h
	rlca
	jr	c,CRCLC1
	ld	hl,(GRPX3)
	ld	a,h
	or	l
	jr	z,CRCLC1
	dec	hl
	ld	(GRPX3),hl

	pop	bc		;8bb
	pop	de

	ld	hl,(DEDX)
	or	a
	sbc	hl,de
	ld	(DEDX),hl
	ld	hl,(DEDX+2)
	sbc	hl,bc
	ld	(DEDX+2),hl
	jr	nc,CRCLNC3
	ld	hl,(DEDX+4),hl
	dec	hl
	ld	(DEDX+4),hl
CRCLNC3:
	push	de		;8bb
	push	bc

	ld	hl,(ERR)
	ld	de,(DEDX)
	or	a
	sbc	hl,de
	ld	(ERR),hl
	ld	hl,(ERR+2)
	ld	de,(DEDX+2)
	sbc	hl,de
	ld	(ERR+2),hl
	ld	hl,(ERR+4)
	ld	de,(DEDX+4)
	sbc	hl,de
	ld	(ERR+4),hl

CRCLC1:
;if de/dx<de/dy or (de/dx=0 and y<b) then loop
	ld	hl,(DEDY)
	ld	de,(DEDX)
	or	a
	sbc	hl,de
	ld	hl,(DEDY+2)
	ld	de,(DEDX+2)
	sbc	hl,de
	ld	hl,(DEDY+4)
	ld	de,(DEDX+4)
	sbc	hl,de
	bit	7,h
	jp	nz,CRCLLP1

	ld	a,d
	or	e
	ld	hl,(DEDX)
	or	h
	or	l
	ld	hl,(DEDX+2)
	or	h
	or	l
	jr	nz,CRCLNZ1
	ld	hl,(GRPY3)
	ld	de,(YRAD)
;	or	a
	sbc	hl,de
	jp	c,CRCLLP1

CRCLNZ1:
;de/dx_ini=-4bb
	pop	bc		;8bb
	pop	de
	push	de
	push	bc
	srl	b		;4bb
	rr	c
	rr	d
	rr	e
	xor	a
	ld	h,a
	ld	l,a
	sbc	hl,de
	ld	(DEDX),hl
	ld	h,a
	ld	l,a
	sbc	hl,bc
	ld	(DEDX+2),hl
	sbc	a,00h
	ld	h,a
	ld	l,a
	ld	(DEDX+4),hl
	pop	bc		;8bb
	pop	de
	exx

	push	de		;8aa
	push	bc


;de/dy_ini=8aab
	ld	hl,(YRAD)
	call	MULINT2		;8aa*b=8aab (low)
	ld	(DEDY),hl
	ld	(DEDY+2),de
	ld	hl,(YRAD)
	pop	de		;8aa (high)
	push	de
	call	MULINT2		;8aa*b=8aab (high)
	ld	bc,(DEDY+2)
	add	hl,bc
	ld	(DEDY+2),hl
	jr	nc,CRCLNC4
	inc	de
CRCLNC4:
	ld	(DEDY+4),de

;err_ini=aa(1-4b)
	srl	d
	rr	e
	ld	(ERR+4),de	;4aab
	ld	hl,(DEDY+2)
	rr	h
	rr	l
	ld	(ERR+2),hl
	ld	hl,(DEDY)
	rr	h
	rr	l
	ld	(ERR),hl

	pop	bc		;8aa
	pop	hl
	pop	de		;aa(high)
	ex	(sp),hl		;aa(low)
	push	bc

	push	de
	ld	de,(ERR)
;	or	a
	sbc	hl,de
	ld	(ERR),hl
	pop	hl
	ld	de,(ERR+2)
	sbc	hl,de
	ld	(ERR+2),hl
	ld	hl,0000h
	ld	de,(ERR+4)
	sbc	hl,de
	ld	(ERR+4),hl

	ld	hl,0000h	;x=0
	ld	(GRPX3),hl
	ld	hl,(YRAD)	;y=y-radius
	ld	(GRPY3),hl

CRCLLP2:
	ld	b,02h
	call	CRCLPSET
	ld	hl,(GRPX3)
	inc	hl
	ld	(GRPX3),hl

;de/dx+=8bb
	exx
	ld	hl,(DEDX)
	add	hl,de
	ld	(DEDX),hl
	ld	hl,(DEDX+2)
	adc	hl,bc
	ld	(DEDX+2),hl
	jr	nc,CRCLNC5
	ld	hl,(DEDX+4)
	inc	hl
	ld	(DEDX+4),hl
CRCLNC5:
	exx

;err+=de/dx
	ld	hl,(ERR)
	ld	de,(DEDX)
	add	hl,de
	ld	(ERR),hl
	ld	hl,(ERR+2)
	ld	de,(DEDX+2)
	adc	hl,de
	ld	(ERR+2),hl
	ld	hl,(ERR+4)
	ld	de,(DEDX+4)
	adc	hl,de
	ld	(ERR+4),hl

;if err>=0 and y>0 then y=y-1:de/dy-=8aa:err-=de/dy
	ld	a,h
	rlca
	jr	c,CRCLC2
	ld	hl,(GRPY3)
	dec	hl
	ld	a,h
	rlca
	jr	c,CRCLC2
	ld	(GRPY3),hl	;y=y-1

	pop	bc		;8aa
	pop	de

	ld	hl,(DEDY)
	or	a
	sbc	hl,de
	ld	(DEDY),hl
	ld	hl,(DEDY+2)
	sbc	hl,bc
	ld	(DEDY+2),hl
	jr	nc,CRCLNC6
	ld	hl,(DEDY+4),hl
	dec	hl
	ld	(DEDY+4),hl
CRCLNC6:
	push	de		;8aa
	push	bc

	ld	hl,(ERR)
	ld	de,(DEDY)
	or	a
	sbc	hl,de
	ld	(ERR),hl
	ld	hl,(ERR+2)
	ld	de,(DEDY+2)
	sbc	hl,de
	ld	(ERR+2),hl
	ld	hl,(ERR+4)
	ld	de,(DEDY+4)
	sbc	hl,de
	ld	(ERR+4),hl

CRCLC2:
;if de/dx<de/dy or (de/dy=0 and x<a) then loop
	ld	hl,(DEDX)
	ld	de,(DEDY)
	or	a
	sbc	hl,de
	ld	hl,(DEDX+2)
	ld	de,(DEDY+2)
	sbc	hl,de
	ld	hl,(DEDX+4)
	ld	de,(DEDY+4)
	sbc	hl,de
	bit	7,h
	jp	nz,CRCLLP2

	ld	a,d
	or	e
	ld	hl,(DEDY)
	or	h
	or	l
	ld	hl,(DEDY+2)
	or	h
	or	l
	jr	nz,CRCLNZ2
	ld	hl,(GRPX3)
	ld	de,(XRAD)
;	or	a
	sbc	hl,de
	jp	c,CRCLLP2


CRCLNZ2:
	pop	bc		;8aa
	pop	de


	ld	hl,(STARTX)
	ld	bc,(GRPX1)	;xcenter
	add	hl,bc
	ld	(GRPX2),hl
	ld	hl,(GRPY1)	;ycenter
	ld	de,(STARTY)
	or	a
	sbc	hl,de
	ld	(GRPY2),hl
	add	hl,de
	ex	de,hl

	pop	af		;sign of start angle
	rlca
	jr	c,CRCLC3
	ld	bc,(GRPX2)
	ld	de,(GRPY2)
CRCLC3:
	call	LINE

	ld	hl,(ENDX)
	ld	bc,(GRPX1)	;xcenter
	add	hl,bc
	ld	(GRPX2),hl
	ld	hl,(GRPY1)	;ycenter
	ld	de,(ENDY)
	or	a
	sbc	hl,de
	ld	(GRPY2),hl
	add	hl,de
	ex	de,hl

	pop	af		;sign of end angle
	rlca
	jr	c,CRCLC4
	ld	bc,(GRPX2)
	ld	de,(GRPY2)

CRCLC4:
	jp	LINE


;input: b(1:condition=y, 2:condition=x)
;destroy: af,bc,de,hl,af'
CRCLPSET:
	call	STOPESC

	ld	hl,(STARTY)
	ld	de,(GRPY3)
	push	hl
	push	de
	ld	hl,(STARTX)
	ld	de,(GRPX3)

	ld	a,(CRCLFLG)
	and	03h		;bit10=start quad.(3,2,1.0)
	jr	z,STARTQ4
	jp	pe,STARTQ1
	rrca
	jr	c,STARTQ3

STARTQ2:
	djnz	STARTQ2X
	pop	de
	pop	hl
	or	a
	sbc	hl,de
	rlc	h
	ccf			;y<=STARTY?
	jr	STARTQEND
STARTQ2X:
	add	hl,de
	rlc	h
	ccf			;-x<=STARTX?
	jr	STARTQ1XEND

STARTQ3:
	djnz	STARTQ3X
	pop	de
	pop	hl
	add	hl,de
	rlc	h
	ccf			;-y<=STARTY?
	jr	STARTQEND
STARTQ3X:
	dec	hl
	add	hl,de
	rlc	h		;-x>=STARTX?
	jr	STARTQ1XEND

STARTQ4:
	djnz	STARTQ4X
	pop	de
	pop	hl
	dec	hl
	add	hl,de
	rlc	h		;-y>=STARTY?
	jr	STARTQEND
STARTQ4X:
	scf
	sbc	hl,de
	rlc	h		;x>=STARTX?
	jr	STARTQ1XEND

;1st quad.
STARTQ1:
	djnz	STARTQ1X
	pop	de
	pop	hl
	scf
	sbc	hl,de
	rlc	h		;y>=STARTY?
	jr	STARTQEND
STARTQ1X:
;	or	a
	sbc	hl,de
	rlc	h
	ccf			;x<=STARTX?

STARTQ1XEND:
	pop	de
	pop	hl

STARTQEND:
	rl	c		;start ok?

	ld	hl,(ENDX)
	ld	de,(GRPX3)
	push	hl
	push	de
	ld	hl,(ENDY)
	ld	de,(GRPY3)

	ld	a,(CRCLFLG)
	and	0ch		;bit32=start quad.(3,2,1.0)
	jr	z,ENDQ4
	jp	pe,ENDQ1
	and	04h
	jr	nz,ENDQ3

ENDQ2:
	djnz	ENDQ2Y
	pop	de
	pop	hl
	dec	hl
	add	hl,de
	rlc	h		;-x>=ENDX?
	jr	ENDQEND
ENDQ2Y:
	scf
	sbc	hl,de
	rlc	h		;y>=ENDY?
	jr	ENDQ1YEND

ENDQ3:
	djnz	ENDQ3Y
	pop	de
	pop	hl
	add	hl,de
	rlc	h
	ccf			;-x<=ENDX?
	jr	ENDQEND
ENDQ3Y:
	dec	hl
	add	hl,de
	rlc	h		;-y>=ENDY?
	jr	ENDQ1YEND

ENDQ4:
	djnz	ENDQ4Y
	pop	de
	pop	hl
;	or	a
	sbc	hl,de
	rl	h
	ccf			;x<=ENDX?
	jr	ENDQEND
ENDQ4Y:
	add	hl,de
	rlc	h
	ccf			;-y<=ENDY?
	jr	ENDQ1YEND

ENDQ1:
	djnz	ENDQ1Y
	pop	de
	pop	hl
	scf
	sbc	hl,de
	rlc	h		;x>=ENDX?
	jr	ENDQEND
ENDQ1Y:
	or	a
	sbc	hl,de
	rlc	h
	ccf			;y<=ENDY?
ENDQ1YEND:
	pop	de
	pop	hl
ENDQEND:
	rl	c		;bit1=start ok, bit0=end ok

	ld	a,(CRCLFLG)	;bit4=start<=end?, bit32=end quad., bit10=start quad.
	ld	b,a
	rrca
	rrca
	sub	b
	and	03h		;end quadrant-start quadrant
	add	a,a
	add	a,a
	jr	nz,CRCLPNZ
	xor	b
	and	0efh		;copy b-bit4
	xor	b
CRCLPNZ:
	xor	c
	and	0fch		;copy c-bit1,0
	xor	c
	ld	e,a
	ld	d,00h
	ld	hl,CRCLFDATA
	add	hl,de

	ld	a,b		;(CRCLFLG)
	and	03h		;start quad.
	ld	b,a
	ld	a,(hl)
	inc	b
CRCLPLP:
	rlca
	djnz	CRCLPLP
	ex	af,af'

	ld	hl,(GRPX1)	;xcenter
	ld	de,(GRPX3)	;x
	or	a
	sbc	hl,de
	push	hl		;xcenter-x
	add	hl,de
	add	hl,de
	ld	b,h
	ld	c,l

	ld	hl,(GRPY1)	;ycenter
	ld	de,(GRPY3)	;y
	or	a
	sbc	hl,de
	push	hl		;ycenter-y
	add	hl,de
	add	hl,de
	ex	de,hl

	push	de		;ycenter+y
	push	bc		;xcenter+x

	ex	af,af'
	ld	h,a
	ex	af,af'
	bit	4,h

	call	nz,PSET		;4th quadrant (xcenter+x,ycenter+y)

	pop	bc
	pop	hl
	pop	de
	push	hl
	push	de

	ex	af,af'
	ld	h,a
	ex	af,af'
	bit	7,h

	call	nz,PSET		;1st quadrant (xcenter+x,ycenter-y)
	pop	de
	pop	hl
	pop	bc
	push	hl
	push	bc

	ex	af,af'
	ld	h,a
	ex	af,af'
	bit	6,h

	call	nz,PSET		;2nd quadrant (xcenter-x,ycenter-y)
	pop	bc
	pop	de

	ex	af,af'
	ld	h,a
	ex	af,af'
	bit	5,h

	ret	z
	jp	PSET		;3rd quadrant (xcenter-x,ycenter+y)


_C_CRCL:ds	C_CRCL-_C_CRCL
	org	C_CRCL

	ld	a,(COLOR1)
	call	SETATT
	call	GETGXY
	jp	nz,SNERR
	inc	hl
	call	INT2ARG		;radius
	ld	a,d
	rlca
	jp	c,FCERR
	ld	(XRAD),de

	ld	hl,00d7h	;default aspect ratio=215/256
	ld	(ASPECT),hl
	call	SETZERO		;default start angle=0
	call	PUSHF1
	ld	hl,PI2		;default end angle=2pi
	call	SETF1
	call	PUSHF1

	ld	hl,(PROGAD)
	call	CHKCLCM
	jp	z,CRCLMAIN
	call	CHKCMM
	jr	z,CRCLPAR3
	call	INT1ARG		;color
	call	SETATT

	call	CHKCLCM
	jp	z,CRCLMAIN

CRCLPAR3:
	call	CHKCMM
	jr	z,CRCLPAR4
	call	NUMARG		;start angle

	call	POPF2		;default end angle
	pop	af		;cancel default start angle
	pop	af
	pop	af
	call	PUSHF1		;new start angle
	call	PUSHF2		;default end angle

	ld	hl,FAC1+3
	res	7,(hl)
	ld	hl,PI2
	call	SETCMPF
	jp	nc,FCERR	;if angle>=|2pi| error

	ld	hl,(PROGAD)
	call	CHKCLCM
	jp	z,CRCLMAIN

CRCLPAR4:
	call	CHKCMM
	jr	z,CRCLPAR5
	call	NUMARG		;end angle
	call	POPF2		;default end angle
	call	PUSHF1		;new end angle

	ld	hl,FAC1+3
	res	7,(hl)

	ld	hl,(PROGAD)
	call	CHKCLCM
	jp	z,CRCLMAIN

CRCLPAR5:
	call	NUMARG		;aspect ratio
	ld	hl,(FAC1+3)
	ld	a,l		;(FAC1+3)
	rlca
	ld	a,h		;(FAC1+4)
	jr	nc,ASPPOS

;aspect ratio < 0
ASPNEG:
	cp	89h
	jp	nc,FCERR	;aspect ratio <= -256
	call	CPYFAC
	call	GETINT
	call	SUBF
	call	NEGABS
	call	INCF1
	jr	ASPSMALL

;aspect ratio >= 0
ASPPOS:
	cp	81h
	jr	c,ASPSMALL

;aspect ratio >= 1
ASPLARGE:
	call	CPYFAC
	ld	hl,(XRAD)
	ld	(YRAD),hl
	call	I2TOF
	call	PUSHF1		;y-radius(=r)
	call	DIVF		;r/asp
	call	FTOI
	ex	de,hl
	ld	(XRAD),hl	;xrad=r/asp
	call	I2TOF
	call	POPF2		;y-radius
	call	DIVF		;1/asp
	call	SETASP		;replace aspect ratio by 256/asp
	jp	CRCLMAIN2

;set aspect ratio
;input: FAC1=aspect ratio
;output: de=aspect ratio*256 (round)
SETASP:
	ld	hl,FAC1+4
	ld	a,(hl)
	add	a,09h
	ld	(hl),a		;*512
	call	FTOI
	srl	d		;/2
	rr	e
	jr	nc,SETASPNC
	inc	de
SETASPNC:
	ld	(ASPECT),de
	ret


;0 <= aspect ratio < 1
ASPSMALL:
	call	SETASP
	jp	CRCLMAIN


;input: FAC1=angle
;output: hl=x=r*cos (signed), de=y=r*sin (signed), b=quadrant(3,2,1,0)
;destroy: af,c,af',bc',de',hl'
ANGL2XY:
	ld	hl,(XRAD)
	ld	de,(YRAD)
	or	a
	sbc	hl,de
	push	af		;YRAD>XRAD?
	jr	c,ANGL2XYC1
	add	hl,de
	ex	de,hl
ANGL2XYC1:
	push	de		;radius
	call	CORDIC		;hl=cos, de=sin
	ex	(sp),hl		;push cos, pop radius
	pop	af		;cos
	push	bc		;b=quadrant(4,3,2,1)
	push	af		;cos

	push	hl		;radius
	call	MULRCS		;de=r*sin
	pop	hl		;radius
	ex	de,hl
	ex	(sp),hl		;push r*sin, pop cos
	call	MULRCS		;de=r*cos
	pop	hl		;r*sin

	pop	bc		;b=quadrant(4,3,2,1)
	pop	af		;YRAD>XRAD?
	push	bc		;b=quadrant(4,3,2,1)
	jr	c,ANGL2XYC2

;aspect ratio<=1
	push	de		;r*cos
	ld	de,(ASPECT)
	call	MULINT2SR	;hl=r*sin*aspect ratio
	ex	de,hl		;de=r*sin*aspect ratio
	pop	hl		;r*cos
	jr	ANGL2XYEND

;aspect ratio>1
ANGL2XYC2:
	push	hl		;r*sin
	ld	hl,(ASPECT)
	call	MULINT2SR	;hl=r*cos*(1/aspect ratio)
	pop	de		;r*sin

ANGL2XYEND:
	pop	bc		;b=quadrant(4,3,2,1)
	dec	b		;3,2,1,0
	ret


;CORDIC correction factor
;9949/16384=0.6073=cos(atan(1))*cos(atan(1/2))*cos(atan(1/4))*...
CORR	equ	9949

;CORDIC algorithm
;input: FAC1=angle
;output: hl=cos, de=sin (2bit.14bit), b=quardrant(4,3,2,1)
;destroy: af,c,af',bc',de',hl'
CORDIC:
	call	ABS
	ld	b,04h
CRDCLP1:
	exx
	ld	hl,PIDIV2
	call	SETCMPF
	jr	c,CRDCC1
	call	SUBF
	exx
	djnz	CRDCLP1

	ld	a,(FAC1+4)
	cp	66h
	jp	nc,FCERR
	inc	b		;4th quadrant
	exx
	ld	hl,PIDIV2
	call	SETF1

CRDCC1:
	exx
	ld	hl,CORR		;initial cos (2bit.14bit)
	ld	d,h		;initial sin (2bit.14bit)
	ld	e,l

	push	bc		;b=quadrant(4,3,2,1)
	ld	a,b
	cp	03h
	jr	z,QUAD2		;2nd quadrant
	jr	nc,QUAD1	;1st quadrant
	ld	de,0-CORR
	dec	a
	jr	z,QUAD4		;4th quadrant
;	jr	nz,QUAD3	;3rd quadrant

QUAD3:
QUAD2:
	ld	hl,0-CORR
QUAD4:
QUAD1:
	exx


	ld	hl,FAC1+4
	ld	a,(hl)
	or	a
	jr	z,CRDCZ1
	add	a,0fh		;*32768
	ld	(hl),a

CRDCZ1:
	call	FTOI
	ld	hl,ATAN
	exx

	xor	a
	ex	af,af'		;sign of residual angle

	ld	bc,1000h
CRDCLP2:

;residual angle (1bit.15bit)
	exx
	ld	c,(hl)
	inc	hl
	ld	b,(hl)
	inc	hl
	ex	de,hl
	ex	af,af'		;sign of residual angle
	jr	nz,CRDCNZ
	or	a
	sbc	hl,bc
	db	3eh		;ld a,
CRDCNZ:
	add	hl,bc
	bit	7,h
	ex	af,af'		;sign of residual angle
	ex	de,hl
	exx

	inc	c
	push	bc
	push	de		;sin
	push	hl		;cos
	ld	b,c
CRDCLP3:
	sra	h
	rr	l
	rra
	sra	d
	rr	e
	djnz	CRDCLP3

	jr	nc,CRDCNC1
	inc	de
CRDCNC1:
	rlca
	jr	nc,CRDCNC2
	inc	hl
CRDCNC2:

	ex	(sp),hl
	ex	af,af'		;sign of residual angle
	jr	z,CRDCZ2

;residual<0
	ex	af,af'		;sign of residual angle
	add	hl,de		;cos=cos+(sin>>c)
	ex	de,hl
	pop	bc		;cos>>c
	pop	hl		;sin
	or	a
	sbc	hl,bc		;sin=sin-(cos>>c)
	jr	CRDCLP2END

CRDCZ2:
;residual>0
	ex	af,af'		;sign of residual angle
	or	a
	sbc	hl,de		;cos=cos-(sin>>c)
	ex	de,hl
	pop	bc		;cos>>c
	pop	hl		;sin
	add	hl,bc		;sin=sin+(cos>>c)

CRDCLP2END:
	ex	de,hl
	pop	bc
	djnz	CRDCLP2

;round hl
	ld	a,l
	cp	03h
	jr	c,CRDCC2
	cp	0feh
	jr	c,CRDCC3
	inc	h
CRDCC2:
	ld	l,00h
CRDCC3:

	pop	bc		;b=quadrant(4,3,2,1)
;round de
	ld	a,e
	cp	03h
	jr	c,CRDCC4
	cp	0feh
	ret	c
	inc	d
CRDCC4:
	ld	e,00h
	ret


ATAN:
	dw	25736	;atan(1)*32768
	dw	15193	;atan(1/2)*32768
	dw	8027	;atan(1/4)*32768
	dw	4075	;atan(1/8)*32768
	dw	2045	;atan(1/16)*32768
	dw	1024	;atan(1/32)*32768
	dw	512	;atan(1/64)*32768
	dw	256	;atan(1/128)*32768
	dw	128	;atan(1/256)*32768
	dw	64	;atan(1/512)*32768
	dw	32	;atan(1/1024)*32768
	dw	16	;atan(1/2048)*32768
	dw	8	;atan(1/4096)*32768
	dw	4	;atan(1/8192)*32768
	dw	2	;atan(1/16384)*32768
	dw	1	;atan(1/32768)*32768


;radius * (cos or sin)
;de=(de*hl)>>18 (round)
;input: de,hl
;output: de
;destroy: af,bc,hl
MULRCS:
	call	MULINT2S	;dehl=radius*(cos or sin)
	add	hl,hl
	rl	e
	rl	d
	add	hl,hl
	rl	e
	rl	d

	rlc	h
	ret	nc
	inc	de
	ret


;hl=(de*hl)>>8 (signed, round)
;input: de,hl
;output: hl
;destroy: af,bc,de
MULINT2SR:
	call	MULINT2S

;hl=(dehl)>>8 (round)
ROUNDI2:
	rlc	l
	ld	l,h
	ld	h,e
	ret	nc
	inc	hl
	ret


;dehl=de*hl (signed)
;input: de,hl
;output: dehl
;destroy: af,bc
MULINT2S:
	push	hl
	push	de
	call	MULINT2
	pop	bc
	ex	(sp),hl
	ex	de,hl
	bit	7,b
	jr	z,MULI2SNC1
	or	a
	sbc	hl,de
MULI2SNC1:
	bit	7,d
	jr	z,MULI2SNC2
	or	a
	sbc	hl,bc
MULI2SNC2:
	ex	de,hl
	pop	hl
	ret


;GET command
C_GET:
	call	SKIPSP
	cp	'@'
	jr	z,GETAT

;FDD command
;not implemented

	ret


;GET@ command
GETAT:
	inc	hl
	call	GETGXY
	cp	MINUS
	jp	nz,SNERR
	push	bc
	push	de
	inc	hl
	call	GETGXY
	jp	nz,SNERR
	pop	de
	pop	bc
	push	hl
	call	SORTXY
	pop	hl
	inc	hl
	call	CHKVAR
	ld	(PROGAD),hl
	call	SRCHARR
	jp	c,FCERR
	bit	7,c
	jp	nz,GETATFILE

	call	GETARR0
	dec	bc
	dec	bc
	dec	bc
	dec	bc
	push	bc		;array data size

	ex	de,hl		;
	ld	hl,(GRPX2)
	ld	bc,(GRPX3)
	ld	(TMP),bc	;smaller x

	ld	a,(SCREEN1)
	or	a
	jp	z,FCERR		;screen mode 1
;	or	a
	sbc	hl,bc
	cp	02h
	cpl
	adc	a,05h		;1,2,3->4,2,1
	ld	b,a		;x dot size

	add	a,l
	ld	l,a
	jr	nc,GETATNC1
	inc	h
GETATNC1:

	call	CHKMOD
	ld	a,b		;x dot size
	jr	nz,GETATNZ1
;mode5
	cp	04h
	jr	nc,GETATNZ1
	add	a,a		;2,1->4,2

GETATNZ1:
	push	af		;x bit size

	ex	de,hl		;
	inc	hl
	ld	(hl),e
	inc	hl
	ld	(hl),d
	inc	hl

	ex	de,hl		;
	ld	hl,(GRPY2)
	ld	bc,(GRPY3)
	or	a
	sbc	hl,bc
	ex	de,hl		;

	ld	a,(SCREEN1)
	dec	a
	jr	nz,GETATNZ2
	call	CHKMOD
	jr	z,GETATZ2

;60 screen mode 2
	srl	d
	rr	e
	srl	d
	rr	e
	jr	GETATNZ2

;66 screen mode 2
GETATZ2:
	push	hl
	call	DIV5
	pop	hl

;y size
GETATNZ2:
	inc	de
	ld	(hl),e
	inc	hl
	ld	(hl),d

	pop	af		;x bit size
	ld	e,a
	ld	d,00h		;x counter initial value

	push	hl		;array address
	push	de		;d=x counter, e=x bit size

GETATLP1:
	ld	bc,(GRPX3)
	ld	de,(GRPY3)
	call	POINT
	ld	c,a
	call	CHKMOD
	jr	z,GETATZ1
	ld	a,(SCREEN1)
	cp	02h
	jr	z,GETATZ1
	inc	c
GETATZ1:
	ld	a,c
	dec	a
	pop	de
	pop	hl

	ld	c,a		;data
	ld	a,d
	sub	e		;counter -= x bit size
	ld	d,a
	jr	nc,GETATNC2

	add	a,08h
	ld	d,a
	exx
	pop	bc
	dec	bc
	ld	a,b
	or	c
	jp	z,FCERR
	push	bc
	exx

	inc	hl
	ld	(hl),0

GETATNC2:
	ld	a,c		;data
	jr	z,GETATZ3
	ld	b,d
GETATLP2:
	rlca
	djnz	GETATLP2
GETATZ3:
	or	(hl)
	ld	(hl),a

	push	hl
	push	de

	ld	hl,(GRPX3)
	ld	de,(GRPX2)
	rst	CPHLDE
	jr	nc,GETATNC3
	ld	b,00h
	call	INCGX
	jr	GETATLP1

GETATNC3:
	pop	de
	ld	d,00h		;x counter initial value
	push	de
	ld	hl,(TMP)	;smaller x
	ld	(GRPX3),hl
	call	INCGY
	ld	hl,(GRPY2)
;	ld	de,(GRPY3)
	rst	CPHLDE
	jr	nc,GETATLP1

GETATEND:
	pop	de
	pop	hl
	pop	bc
	ret


GETATFILE:
;save to FDD file
;not implemented
	jp	SNERR




;get array(0) address and last size
;input: de=array address
;output: bc=size, hl=array(0) address-1
;destroy: af,de
GETARR0:
	ex	de,hl
	ld	c,(hl)		;bc=bytes
	inc	hl
	ld	b,(hl)
	inc	hl
	ld	a,(hl)		;dimensions

	dec	bc
GETARR0LP:
	inc	hl
	inc	hl
	dec	bc
	dec	bc
	dec	a
	jr	nz,GETARR0LP
	ret


;PUT command
C_PUT:
	call	SKIPSP
	cp	'@'
	jr	z,PUTAT

;FDD command
;not implemented

	ret


;PUT@ command
PUTAT:
	inc	hl
	call	GETGXY
	jp	nz,SNERR
	inc	hl
	call	CHKVAR
	ld	(PROGAD),hl

;	or,and,xor,pset,preset
	call	SKIPSP
	cp	','
	jr	nz,PUTATNZ1
	call	SKIPSPINC
	inc	hl
	cp	'X'
	jr	nz,PUTATNZ2
	call	SKIPSP
	inc	hl
	cp	OR_
	jp	nz,SNERR
PUTATNZ1:
	ld	a,'X'
PUTATNZ2:
	ld	(PROGAD),hl
	ex	af,af'		;put@ condition

	call	SRCHARR
	jp	c,FCERR
	bit	7,c
	jp	nz,PUTATFILE

	call	GETARR0
	inc	hl
	ld	c,(hl)
	inc	hl
	ld	b,(hl)		;bc=x dot size
	inc	hl
	ld	e,(hl)
	inc	hl
	ld	d,(hl)		;de=y dot size
	push	hl		;array address

	exx
	ld	hl,TBL-2
	call	CHKMOD
	ld	a,(SCREEN1)
	jr	z,PUTATZ1
	inc	hl
	inc	hl
PUTATZ1:
	or	a
	jp	z,FCERR
	ld	c,a		;(SCREEN1)
	ld	d,00h
	add	a,a
	ld	e,a
	add	hl,de
	ld	d,(hl)		;x bit size
	inc	hl
	ld	e,(hl)		;mask
	pop	hl		;array address

	ld	b,00h
	call	CHKMOD
	jr	z,PUTATZ2	;mode5: 1-origin
	ld	a,c		;(SCREEN1)
	rrca
	jr	c,PUTATNZ3	;mode1-4,screen2,4: 0-origin
PUTATZ2:
	inc	b		;0=0-origin, 1=1-origin
PUTATNZ3:
	exx

	ld	hl,(GRPY1)
	ld	(GRPY3),hl
	add	hl,de
	ex	de,hl
	ld	hl,(GRPX1)
	add	hl,bc
	ld	b,h
	ld	c,l

PUTATLP1:
	exx
	ld	c,0		;shift count
	exx

	push	de		;y-end
	ld	hl,(GRPX1)	;x-start
	ld	(GRPX3),hl

PUTATLP2:
	push	bc		;x-end

	ld	bc,(GRPX3)
	ld	de,(GRPY3)
	push	bc		;x
	push	de		;y
	call	POINT

	exx
	sub	b		;0- or 1-origin
	exx
	ld	b,a		;;

	exx
	ld	a,c
	sub	d
	ld	c,a
	ld	a,(hl)
	jr	z,PUTATZ3
	jr	nc,PUTATNC
	inc	hl
	ld	a,c
	and	07h
	ld	c,a
	ld	a,(hl)
PUTATNC:
	push	bc
	ld	b,c
PUTATLP3:
	rrca
	djnz	PUTATLP3
	pop	bc
PUTATZ3:
	and	e
	exx

	ex	af,af'		;put@ condition
	cp	'X'
	jr	z,PUTATXOR
	cp	PSET_		;9bh
	jr	z,PUTATPSET
	cp	AND_		;cfh
	jr	z,PUTATAND
	cp	OR_		;d0h
	jr	z,PUTATOR

PUTATPRESET:
	ex	af,af'
	cpl
	jr	PUTATSET

PUTATPSET:
	ex	af,af'
	jr	PUTATSET

PUTATAND:
	ex	af,af'
	and	b		;;
	jr	PUTATSET

PUTATOR:
	ex	af,af'
	or	b		;;
	jr	PUTATSET

PUTATXOR:
	ex	af,af'
	xor	b		;;
;	jr	PUTATSET

PUTATSET:
	exx
	add	a,b		;0- or 1-origin
	exx
	call	SETATT
	pop	de		;y
	pop	bc		;x
	call	PSET

;	ld	b,00h
;	call	INCGX
	ld	a,(SCREEN1)
	cp	02h
	cpl
	adc	a,05h		;1,2,3 -> 4,2,1
	ld	d,00h
	ld	e,a
	ld	hl,(GRPX3)
	add	hl,de
	ld	(GRPX3),hl

	pop	bc		;x-end
	or	a
	sbc	hl,bc
	jr	c,PUTATLP2

	call	INCGY
	ex	de,hl
	pop	de		;y-end
	or	a
	sbc	hl,de
	jp	c,PUTATLP1

	ret


;bit size and mask
TBL:
	db	04h, 0fh	;mode5,screen2
	db	04h, 0fh	;mode5,screen3	mode1-4,screen2
	db	02h, 03h	;mode5,screen4	mode1-4,screen3
	db	01h, 01h	;		mode1-4,screen4


PUTATFILE:
;load from FDD file
;not implemented
	jp	SNERR



;CSAVE command (extended)
C_CSVEX:
	call	SKIPSP
	cp	ASTRSK		;cch
	jp	nz,C_CSV
	inc	hl
	call	CHKVAR
	ld	(PROGAD),hl
	call	SRCHARR
	jp	c,FCERR
	call	GETARR0

	push	bc
	call	PRTOPN
	pop	bc
CSVEXLP:
	inc	hl
	ld	a,(hl)
	call	PUTCMT
	dec	bc
	ld	a,b
	or	c
	jr	nz,CSVEXLP
	jp	WCLOSE


;CLOAD command (extended)
C_CLDEX:
	call	SKIPSP
	cp	ASTRSK		;cch
	jp	nz,C_CLD
	inc	hl
	call	CHKVAR
	ld	(PROGAD),hl
	call	SRCHARR
	jp	c,FCERR
	call	GETARR0

	push	bc
	call	INPOPN
	pop	bc
CLDEXLP:
	call	GETCMT
	jp	nz,TRERR
	inc	hl
	ld	(hl),a
	dec	bc
	ld	a,b
	or	c
	jr	nz,CLDEXLP
	jp	RCLOSE



;set FAT pointer
;input: ix, a=drive
;output: (ix+00h)=drive, (0fb33h)=pointer, a,de,hl?
;destroy: f
_SETFATP:ds	SETFATP-_SETFATP
	org	SETFATP

	ld	(ix+00h),a
	ret


;read/write disk
;input:
; a=the number of sector, b=track, c=sector, hl=address
; ix=work address, (ix+00h)=drive
; c-flag=0,z-flag=0: read
; c-flag=0,z-flag=1: check
; c-flag=1: write
;output:
; c-flag=1 if error
_DISK2:	ds	DISK2-_DISK2
	org	DISK2

	push	af
	xor	a
	ld	(ix+1bh),a
	pop	af
	push	af
	push	hl
	push	de
	push	bc
	ex	de,hl
	call	DISK
	pop	bc
	pop	de
	pop	hl
	jr	c,DISK2ERR
	pop	af
	or	a		;reset c-flag: ok
	ret

DISK2ERR:
	pop	af
	scf			;set c-flag: error
	ret


;60EX ROM end
_6000H:	ds	6000h-_6000H



_CALLINI:ds	CALLINI-_CALLINI
	org	CALLINI

	call	INIDSK
	ret


;print menu and select mode
;output: z(1=mode 5), a=mode-1(if z=0)
SELMOD:
	ld	a,(PROGAD)
	cp	05h
	jp	c,SELECTED

;FD auto start
	call	INIDSK
	ld	(DRIVES),a
	jr	z,SKIPFD

	ld	a,02h
	ld	(MODE),a
	ld	(PAGES),a

	ld	hl,SKIPFD
	ld	(ERRAD),hl
	ld	ix,0c400h
	xor	a
	ld	(ix+00h),a	;drive
	ld	(ix+11h),a	;error count
	ld	bc,0001h	;track=0, sector=1
	ld	de,0f900h	;load address
	ld	a,c		;1 sector
BOOTSECT:
	or	a		;reset z- and c-flag for reading
	call	DISK
	jr	c,BOOTSECT

	ld	a,(0f900h)
	ld	de,"SY"
	cp	d		;'S'
	jr	nz,SKIPFD
	ld	hl,(0f901h)
	rst	CPHLDE
	call	z,0f903h

	ld	a,(PROGAD)
	cp	05h
	jr	c,SELECTED

;select mode
SKIPFD:
	ld	hl,EDIT
	ld	(ERRAD),hl
	xor	a
	ld	(MODE),a

	call	CLS
	ld	hl,0110h
	call	SETCSR
	ld	a,' '
	ld	b,1fh
SLMDLP1:
	call	PUTC
	djnz	SLMDLP1

	ld	hl,MENU
	ld	b,09h
SLMDLP2:
	push	hl
	ld	h,02h
	ld	a,0dh
	sub	b
	ld	l,a
	call	SETCSR
	pop	hl
	push	hl
	push	bc
	call	PUTS
	pop	bc
	pop	hl

SLMDLP3:
	ld	a,(hl)
	inc	hl
	or	a
	jr	nz,SLMDLP3
	djnz	SLMDLP2

SLMDLP4:
	call	GETKBF
	jr	z,SLMDLP4
	sub	'1'
	cp	05h
	jr	nc,SLMDLP4

SELECTED:
	cp	04h
	push	af		;mode 5?
	jr	c,SETMOD
	dec	a		;mode 5->4 for "How Many..."
SETMOD:
	ld	(MODE),a

	and	01h
	ld	a,04h
	ld	hl,8000h
	jr	nz,RAM32K
	ld	h,0c0h
	rra			;c-flag=0
RAM32K:
	ld	(PAGES),a
	ld	b,a		;;

	ld	a,h
	ld	(VRAM),a
	set	2,h		;h=h+40h
	ld	(BASICAD),hl	;l=0
	inc	l		;inc hl
	ld	(STARTAD),hl

SLMDLP5:
	ld	a,b		;;
	dec	a
	call	CHGACT
	call	CLS
	djnz	SLMDLP5

	xor	a
	call	CHGDSP

	pop	af
	ret


REGDATA:
	dw	0aaffh, 0bbcch, 0ddeeh, 1234h

MODE5:
	xor	a
	ld	(VRAM),a
	inc	a
	ld	(COLOR2),a
	ld	(M1COLOR+1),a
	ld	(M2COLOR+1),a
	ld	(M3COLOR+1),a
	ld	(M4COLOR+1),a
	inc	a
	ld	(COLOR3),a
	ld	(M1COLOR+2),a
	ld	(M2COLOR+2),a
	ld	(M3COLOR+2),a
	ld	(M4COLOR+2),a
	ld	a,10h
	ld	(COLOR1),a
	ld	(M1COLOR),a
	ld	(M2COLOR),a
	ld	(M3COLOR),a

	ld	a,CLMN66
	ld	(WIDTH),a
	ld	a,ROWS66
	ld	(HEIGHT),a
	ld	(CONSOL2),a
	dec	a
	ld	(LASTLIN),a
	ld	a,06h
	ld	(FKEYLEN),a
	ld	a,04h
	ld	(MODE),a
	ld	(M4COLOR),a

;	ld	a,04h
	ld	hl,VRAM
	ld	de,PAGE1
MODE5LP1:
	ld	bc,PAGE1-VRAM
	ldir
	dec	a
	jr	nz,MODE5LP1

	ld	l,a		;=0
	ld	d,a		;=0
	ld	a,(PAGES)

	ld	h,80h
	cp	03h
	jr	c,SETAD
	ld	h,88h
	jr	z,SETAD
	ld	h,0c8h
SETAD:
	ld	(BASICAD),hl
	inc	l
	ld	(STARTAD),hl

	ld	hl,0fa00h-47	;no fdd
	ld	(USREND),hl
	ld	(STREND),hl
	ld	(STRAD),hl
	ld	hl,0fa00h-47-300
	ld	(STACK),hl

	ld	hl,C_COL66
	ld	(0fa95h),hl	;COLOR command
	ld	hl,C_LIN66
	ld	(0fa9bh),hl	;LINE command
	ld	hl,C_PAI66
	ld	(0fa9dh),hl	;PAINT command
	ld	hl,F_PEEK66
	ld	(0fb03h),hl	;PEEK() function

	push	af		;a=(PAGES)
	ld	d,a
	ld	e,00h
MODE5LP2:
	ld	a,d
	dec	a
	call	GETPGAD

	ld	(hl),e
	ld	a,e
	ld	(VRAM),a
	add	a,40h
	ld	e,a
	dec	d
	jr	nz,MODE5LP2
	pop	bc		;b=(PAGES)

MODE5LP3:
	ld	a,b
	dec	a
	call	CHGACT
	call	CLS
	djnz	MODE5LP3

	ld	a,04h		;40x20 text mode
	out	(0c1h),a	;bit3: 0=320x200, 1=160x200
				;bit2: 0=graphic, 1=text
				;bit1: 0=40x20,   1=32x16
;	ld	a,04h		;relay off,timer on, VRAM=0000h
;	out	(0b0h),a	;bit3=CMT relay: 0=off, 1=on
				;bit21=VRAM address:
				; 00=c000h(60)/8000h(60m)
				; 01=e000h(60)/c000h(60m)
				; 10=8000h(60)/0000h(60m)*
				; 11=a000h(60)/4000h(60m)
				;bit0=timer: 0=on, 1=off

	rrca			;2
	out	(0c0h),a	;CSS2=1

	xor	a
	call	CHGDSP

	call	NEWRESSTK
	call	RESSTK

	call	SETTBL

	ld	hl,SYSNAME66
	jp	START


MENU:
	db	"  *** PC-6601 ",9ah, 0deh, 96h, 0fdh, "BASIC ***",00h
	db	0ah,00h
	db	"1:60 BASIC          (RAM-16K)",00h
	db	"2:60 BASIC          (RAM-32K)",00h
	db	"3:60 EXTENDED BASIC (RAM-16K)",00h
	db	"4:60 EXTENDED BASIC (RAM-32K)",00h
	db	"5:66 BASIC          (RAM-64K)",00h
	db	0ah,00h
	db	"      SELECT BASIC MODE",00h


SYSNAME66:
	db	"66", 9ah, 0deh, 96h, 0fdh, "BASIC Ver.0.2.1", 0dh, 0ah, 00h


;PEEK() function
F_PEEK66:
	call	CHKNUM
	call	FTOI
	ex	de,hl
	call	READRAM
	jp	I1TOFA


;continued from F_SCRN
F_SCRN66:
	call	READRAM
	call	I1TOFA
	jp	FNCRTN


;input: bc=X, de=Y
;output: a=color
;destroy: f,bc,de,hl
POINT66:
	ld	a,(SCREEN1)
	dec	a
	jr	z,POINT662
	jp	p,POINT66G

;screen mode 1
POINT661:
	call	GXY2AD66
	call	READRAM
	rrca
	rrca
	rrca
	rrca
	and	07h
	inc	a
	ret

;graphic mode (screen mode 3,4)
POINT66G:
	push	af		;
	call	GXY2GAD66
	call	READRAM
	and	d
	rlca
	pop	bc		;
;	dec	b
;	jr	nz,POINT664
	djnz	POINT664
	rlca
POINT664:
	ld	e,a
	res	5,h
	call	READRAM
	and	d
	or	e
	rlca
POINT66LP:
	rrca
	rrc	d
	jr	nc,POINT66LP
	inc	a
	ret

;screen mode 2
POINT662:
	push	bc		;x
	call	GXY2AD66
	pop	bc		;x
	call	MASK266
	ld	d,a
	call	READRAM
	ld	b,a
	rlca			;
	inc	h
	inc	h
	inc	h
	inc	h
	call	READRAM
	jr	nc,POINT662C	;
	and	d		;;
POINT662C:
	ld	a,b
	jr	nz,POINT662NZ	;;
	and	70h
	rrca
	rrca
	rrca
	rrca
POINT662NZ:
	and	0fh
	inc	a
	ret


;scroll up
;input: h=first line+1, l=last line+1
;destroy: af
_SCRLU66:ds	SCRLU66-_SCRLU66
	org	SCRLU66

	ld	a,l
	sub	h		;a=(LASTLIN)-(CONSOL1) < 16
;	ret	z
;	ret	c

;z=0:scroll up, z=1:scroll down
SCRLUD66:
	push	de
	push	hl
	push	bc
	push	af		;up/down


;line connection status
	ld	b,a
	inc	b
	ld	d,b		;>0
SCRLUD66LP:
	call	CHKLINE4
	ld	c,a
	ld	a,d
	push	hl
	call	SETLINE
	pop	hl
	ld	d,c
	dec	l
	pop	af		;up/down
	push	af
	jr	nz,SCRLUD66NZ
	inc	l
	inc	l
SCRLUD66NZ:
	djnz	SCRLUD66LP

	ld	l,h
	ld	a,(SCREEN1)
	cp	02h
	pop	de		;d=a
	push	de
	ld	a,d
	jr	nc,SCRLG66	;screen mode 3 4

	call	MUL40
	call	Y2AD
	ld	hl,CLMN66
	add	hl,de
	pop	af		;up/down
	jr	nz,SCRL2NZ
	ex	de,hl
	dec	de
	dec	hl

SCRL2NZ:
	push	hl
	push	de
	push	bc
	call	LDIDRRAM
	pop	bc
	pop	de
	pop	hl

;attribute
	res	2,h
	res	2,d
	jp	SCRLATT66


;scroll down
;input: h=last line+1, l=first line+1
;destroy: af
_SCRLD66:ds	SCRLD66-_SCRLD66
	org	SCRLD66

	ld	a,h
	sub	l
;	ret	z
;	ret	c
	cp	a		;set z-flag
	jr	SCRLUD66


;for graphic mode (screen mode 3,4) for mode 5
SCRLG66:
;bc=a*40*10=a*(256+128+16)

	ld	h,a
	ld	c,b		;b=0
	ld	b,a
	rra			;c-flag=0
	rr	c
	add	a,b
	ld	b,a

	ld	a,h
	add	a,a
	add	a,a
	add	a,a
	add	a,a
	jr	nc,SCRLG66NC1
	inc	b
SCRLG66NC1:
	add	a,c
	ld	c,a
	jr	nc,SCRLG66NC2
	inc	b
SCRLG66NC2:

	ld	h,1
	call	XY2GAD66
	ex	de,hl
	ld	hl,CLMN66*10
	add	hl,de
	pop	af		;up/down
	jr	nz,SCRLG66NZ
	ex	de,hl
	dec	de
	dec	hl

SCRLG66NZ:
	push	hl
	push	de
	push	bc
	call	LDIDRRAM
	pop	bc
	pop	de
	pop	hl
	res	5,h
	res	5,d

SCRLATT66:
	call	LDIDRRAM
	jp	SCRLATT2


;continued from DELLIN
DELLIN66:
	jr	nc,DEL6634
	push	hl
	res	2,h
;	ld	bc,CLMN66
	ld	bc,2801h
	jp	DELLATT

DEL6634:
	call	AD2GAD2
	push	hl
	res	5,h
;	ld	bc,CLMN66*10
	ld	bc,9002h
	jp	DELLATT


;ldir or lddr in RAM
;input: bc,de,hl,z(0=ldir,1=lddr)
;output: bc=0, de, hl
;destroy: f
LDIDRRAM:
	jp	nz,LDIRRAM
	jp	LDDRRAM


;bc=a*40
;input: a(<=20)
;output: bc
;destory: af,h
MUL40:
	ld	b,a
	add	a,a		;*2
	add	a,a		;*4
	add	a,b		;*5
	add	a,a		;*10
	ld	b,l		;;
	ld	h,0
	ld	l,a
	add	hl,hl		;*20
	add	hl,hl		;*40
	ld	c,l
	ld	l,b		;;
	ld	b,h
	ret


;continued from PRTFKEY for mode 5
PFK66:
	call	CHRREV66

	ld	a,(KEYFLG)
	bit	2,a
	jr	z,PFK_ALP
	and	02h
	jr	nz,PFK_KATA
	ld	de,MODE_HIRAGANA
	jr	PFK66END

;katakana
PFK_KATA:
	ld	de,MODE_KATAKANA
	jr	PFK66END

;alphabet
PFK_ALP:
	rrca
	jr	c,PFK_CAPS
	ld	de,MODE_LOWER
	jr	PFK66END

;upper case
PFK_CAPS:
	ld	de,MODE_UPPER

PFK66END:
	ld	b,03h
PFK66LP:
	ld	a,(de)
	call	PUT12
	inc	de
	djnz	PFK66LP
	jp	CHRREV66

MODE_LOWER:
	db	"abc"
MODE_UPPER:
	db	"ABC"
MODE_HIRAGANA:
	db	96h, 0e5h, ' '
MODE_KATAKANA:
	db	0b6h, 0c5h, ' '


;continued from GETSP for mode 5
GETSP66:
	dec	a
	ret	z
	dec	a
	ld	a,' '
	ret	m
	ld	a,(COLOR2)
	jr	z,GETSP663

;screen mode 4
	or	a
	ret	z
	dec	a
	rrca
	and	01h
	jp	CNVATT

;screen mode 3
GETSP663:
	or	a
	ret	z
	dec	a
	rrca
	rrca
	and	03h
	inc	a
	jp	CNVATT


;continued from GETSPA for mode 5
GETSPA66:
	dec	a
	jp	m,CNVATT1
	jr	z,GETSPA662
	dec	a
	ld	a,(COLOR2)
	jr	z,GETSPA663

;screen mode 4
	or	a
	ret	z
	dec	a
	and	01h
	jp	CNVATT

;screen mode 3
GETSPA663:
	or	a
	ret	z
	dec	a
	and	03h
	inc	a
	jp	CNVATT

GETSPA662:
	call	ATT662
	or	80h
	ret


;continued from CNVATT1
ATT662:
	push	af
ATT66:
	pop	af
	push	hl
	or	a
	jr	z,ATT66Z
	dec	a
	and	0fh
ATT66Z:
	ld	l,a
	ld	a,(COLOR2)
	dec	a
	and	07h
	rlca
	rlca
	rlca
	rlca
	or	l
	pop	hl
	ret


;continued from SETATT
SETATT66:
	ld	a,(SCREEN1)
	dec	a
	jr	z,SETATT662	;screen mode 2
	jp	m,SETATT661	;screen mode 1
	dec	a
	jr	z,SETATT663	;screen mode 3

;screen mode 4
	pop	af
	sub	01h
	adc	a,00h		;if a<>0 then a=a-1
	rrca
	push	af
	rrca
	sbc	a,a		;bit1=0:00h, =1:ffh
	ld	(ATTDAT2),a
	pop	af
	sbc	a,a		;bit0=0:00h, =1:ffh
	ld	(ATTDAT),a
	ret


;screen mode 3
SETATT663:
	pop	af
	sub	01h
	adc	a,00h		;if a<>0 then a=a-1

;	push	af
;	rrca
;	rrca
;	and	03h
;	inc	a
;	call	CNVATT
;	ld	(ATTDAT2),a
;	pop	af
;	and	03h
;	inc	a
;	call	CNVATT
;	ld	(ATTDAT),a
;	ret

	push	bc
	and	0fh
	ld	b,a
	rrca
	rrca
	rrca
	rrca
	or	b
	ld	b,a
	and	33h
	ld	c,a
	xor	b
	ld	b,a
	rrca
	rrca
	or	b
	ld	(ATTDAT2),a
	ld	a,c
	rrca
	rrca
	or	c
	ld	(ATTDAT),a
	pop	bc
	ret


SETATT662:
	pop	af
	dec	a
	and	0fh
	inc	a
	ld	(ATTDAT),a
	ret


;convert character VRAM address to graphic VRAM address for mode 5
;(screen mode 1,2 -> screen mode 3,4)
;input: hl=character VRAM address
;output: hl=graphic VRAM address
;destroy: none
_AD2GAD2:ds	AD2GAD2-_AD2GAD2
	org	AD2GAD2

	push	af
	call	AD2XY
	pop	af
	call	XY2GAD66
	ret


;get VRAM address (screen mode 3,4) for mode 5
;input: h=x+1, l=y+1
;output: hl=graphic VRAM address =(VRAM+4000h)+y*400+x
;destroy: none
XY2GAD66:
	push	af
	push	de
	push	bc

	dec	h
	dec	l
	ld	a,(SCREEN1)
	cp	02h
	ld	a,h
	jr	nz,XY2GAD66C
	add	a,a		;screen mode 3
XY2GAD66C:
	ld	h,0
	add	hl,hl		;*2
	add	hl,hl		;*4
	add	hl,hl		;*8
	add	hl,hl		;*16
	ld	d,h
	ld	e,l
	add	hl,hl		:*32
	add	hl,hl		:*64
	add	hl,hl		:*128
	ld	b,h
	ld	c,l
	add	hl,hl		:*256
	add	hl,bc
	add	hl,de

	add	a,l		;c-flag
	ld	l,a

	ld	a,(VRAM)
	adc	a,20h		;c-flag
	add	a,h
	ld	h,a

	pop	bc
	pop	de
	pop	af
	ret


Y2AD66:
	push	hl
	ld	a,l
	ld	de,CLMN66
	ld	hl,(VRAM-1)	;h=(VRAM)
	set	2,h
	ld	l,d		;=0
Y2AD66LP:
	add	hl,de
	dec	a
	jr	nz,Y2AD66LP

;	or	a
	sbc	hl,de
	ex	de,hl
	pop	hl
	pop	af
	ret


;get VRAM dot pattern address (screen mode 3,4) for mode 5
;input: bc=graphic X, de=graphic Y
;output: hl=VRAM address, d=bit
;destroy: af,bc,e
;hl=(VRAM+2000h)+(X/8)+Y*40
GXY2GAD66:
	call	CHKGXY66
GXY2GAD662:
	ld	h,d
	ld	l,e
	add	hl,hl		;*2
	add	hl,hl		;*4
	add	hl,de		;*5
	add	hl,hl		;*10
	add	hl,hl		;*20
	add	hl,hl		;*40

	ld	a,(VRAM)
	or	20h
	srl	b
	ld	b,a
	ld	a,c
	rr	c

	jp	GXY2GADEND


;get VRAM attirbute address (screen mode 1,2) for mode 5
;hl=(VRAM)+X/8+Y/10*40
;input: bc=graphic X, de=graphic Y
;output: hl=VRAM address, d=Y mod 10
;destroy: af,bc,e
GXY2AD66:
	call	CHKGXY66
	call	DIV10
	ld	h,d
	ld	l,e
	add	hl,hl		;*2
	add	hl,hl		;*4
	add	hl,de		;*5
	add	hl,hl		;*10
	add	hl,hl		;*20
	add	hl,hl		;*40
	ld	d,a
	srl	b
	rr	c
	srl	c
	srl	c
	ld	a,(VRAM)
	ld	b,a
	add	hl,bc
	ret


CHRREV66:
	push	hl
	ld	hl,(COLOR1)	;l=(COLOR1), h=(COLOR2)
	ld	a,h
	ld	h,l
	ld	l,a
	ld	(COLOR1),hl
	pop	hl
	ret


;continued from CSRREV
CSRREV66:
	res	2,h
	call	READRAM
	cpl
	xor	80h
	ld	(hl),a
	ret


GETSCRC66:
	push	hl
	call	CHR2ATT
	call	READRAM
	pop	hl
	rlca
	jp	nc,READRAM
	ld	a,' '
	ret


PUTG66:
;	push	de
;	push	bc
;
;	call	CGROM
;	ld	a,(COLOR1)
;	call	SETATT

	call	XY2GAD66	;no change in c-flag

	ld	b,0ah
PUTG66LP:
	push	de
	call	READCGROM

	ld	a,(SCREEN1)
	rrca
	jr	nc,CALL366

	call	PUT466
	jr	CALL3466END
CALL366:
	call	PUT366
	dec	hl

CALL3466END:
	ld	de,CLMN66
	add	hl,de
	pop	de
	inc	e		;inc de
	djnz	PUTG66LP

	pop	bc
	pop	de
	ret


PUT366:
	call	PUTHALF66
	inc	hl
	ld	d,c

PUTHALF66:
	ld	e,04h
PUT366LP:
	rlc	d
	rla
	rrca
	rlca
	rla
	dec	e
	jr	nz,PUT366LP
	ld	c,d
	ld	d,a


;input: hl=address, d=bit
;destroy: af,e
PUT466:
	call	READRAM
	ld	e,a
	ld	a,(ATTDAT2)
	xor	e
	and	d
	xor	e
	ld	(hl),a

	res	5,h
	call	READRAM
	ld	e,a
	ld	a,(ATTDAT)
	xor	e
	and	d
	xor	e
	ld	(hl),a
	set	5,h

	ret


;set border attribute used by HURRY FOX
;input: a=color code
;destroy: af,bc
_SETBO66:ds	SETBO66-_SETBO66
	org	SETBO66

	ld	b,a

	ld	a,(ATTDAT)
	push	af
	ld	a,(ATTDAT2)
	push	af

	ld	a,b
	call	SETATT
	ld	a,(ATTDAT)
	ld	(BORDERA),a
	ld	a,(ATTDAT2)
	ld	(BORDERA2),a

	pop	af
	ld	(ATTDAT2),a
	pop	af
	ld	(ATTDAT),a

	ret



;COLOR command
_C_COL66:
	ds	C_COL66-_C_COL66
	org	C_COL66

	ld	de,COLOR1
	ld	b,02h
COLR66LP:
	call	CHKCMM
	jr	z,COLR66Z
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
COLR66Z:
	inc	de
	djnz	COLR66LP

	call	INT1ARG
	ld	a,(SCREEN1)
	ld	c,a
	ld	a,e
	dec	a
	jp	SETC366



;set color 3rd parameter for mode 5
;input: a=3rd color parameter-1, c=screen mode-1
;destroy: af
_SETC366:ds	SETC366-_SETC366
	org	SETC366

	push	af
	ld	a,c
	cp	02h
	jr	c,C366TXT
	jr	z,C3663


;screen mode 4
C3664:
	pop	af		;0,1,2,3,4,5
;	xor	02h		;2,3,0,1,6,7
	jr	C366END

;screen mode 1,2
C366TXT:
	pop	af		;0,1
	add	a,a
;	xor	02h		;2,0
	jr	C366END

;screen mode 3
C3663:
	pop	af		;0,1
	add	a,a
	add	a,a
;	xor	02h		;2,6
C366END:
	xor	02h
	out	(0c0h),a
	ld	(COLOR3),a
	ret


;SCREEN command
_C_SCR66:ds	C_SCR66-_C_SCR66
	org	C_SCR66
	nop
	nop
	nop

;used by PROGRESS
;input: hl=parameter string address
_SCRN66:ds	SCRN66-_SCRN66
	org	SCRN66

	jp	C_SCRN


;continued from PSET
PSET66:
	ld	a,(SCREEN1)
	dec	a
	jr	z,PSET662
	jp	p,PSET66G

;screen mode 1
PSET661:
	call	GXY2AD66
	call	READRAM
	and	8fh
	ld	e,a
	ld	a,(ATTDAT)
	and	07h
	rlca
	rlca
	rlca
	rlca
	or	e
	ld	(hl),a
	pop	hl
	ret


;screen mode 2
PSET662:
	push	bc		;X
	call	GXY2AD66

	call	READRAM
	ld	e,a		;
	ld	a,(ATTDAT)
	or	a
	jr	nz,PS662NZ1
	bit	7,e
	jr	nz,PS662NZ2
PS662NZ1:
	call	GETSPA662
	ld	b,a
	ld	a,(COLOR2)
	call	GETSPA662
	and	07h
	rlca
	rlca
	rlca
	rlca
	or	b
	ld	(hl),a
PS662NZ2:
	set	2,h
	bit	7,e
	jr	nz,PS662SEMI
	call	GETSP
	ld	(hl),a

PS662SEMI:
	pop	bc		;X
	call	MASK266
	ld	b,a

	call	READRAM
;	and	0fh
	or	b
	ld	c,a

	ld	a,(ATTDAT)
	or	a
	jr	z,PSET662C0
	ld	(hl),c
	pop	hl
	ret


PSET662C0:
	ld	a,b
	cpl
	ld	b,a
	call	READRAM
	and	b
	ld	(hl),a
	pop	hl
	ret


;screen mode 2 dot bit mask
;input: c=X, d=Y mod 10
;output: a
; bit3 bit2
; bit1 bit0
;destroy: f,bc
MASK266:
	ld	a,c
	cpl
	and	04h
	add	a,04h
	ld	b,a
	ld	a,d
	cp	05h
	ld	a,b
	ret	c	;0<=d<=4
	rrca
	rrca
	ret		;5<=d<=9


;screen mode 3,4
PSET66G:
	call	GXY2GAD66
	call	PUT466
	pop	hl
	ret


;continued from LINE
LINE66:
	cp	02h
	jr	z,LINE663
	jr	nc,SKIPY66
	add	hl,hl		;screen mode 1,2: dy=dy*4
LINE663:
	add	hl,hl		;screen mode 3: dy=dy*2

SKIPY66:
	ex	de,hl
	ld	bc,(GRPX3)	;start (x)
	ld	hl,(GRPX2)	;end (x)
	or	a
	sbc	hl,bc		;dx
	cp	02h
	jp	nc,SKIPX
	ld	b,h
	ld	c,l
	add	hl,hl
	add	hl,hl
	add	hl,bc		;screen mode 1,2: dx=dx*5
	jp	SKIPX


;check and round graphic coordinates (x,y) for mode 5
;input: bc=x, de=y
;output: bc=x, de=y
;destroy: af,hl
CHKGXY66:
	ld	a,b
	or	d
	jp	m,FCERR
	ld	hl,319
;	or	a
	sbc	hl,bc
	jr	nc,CHKXOK66
	ld	bc,319

CHKXOK66:
	ld	a,(SCREEN1)
	rrca
	cpl
	ld	hl,CONSOL3
	and	(hl)
	rrca
	ld	hl,199
	jr	nc,NOFKEY66
	ld	hl,199-10
NOFKEY66:
	rst	CPHLDE
	jr	nc,CHKYOK66
	ex	de,hl
CHKYOK66:
	ld	a,(SCREEN1)
	dec	a
	jr	z,ROUNDY662
	jp	p,ROUNDX66	;screen mode 3,4

;round off y for screen mode 1
ROUNDY661:
	call	DIV10
	ld	a,e
	add	a,a		;*2
	add	a,a		;*4
	add	a,e		;*5
	add	a,a		;*10
	jr	ROUNDY662END

;round off y for screen mode 2
ROUNDY662:
	call	DIV5

	ld	a,e
	add	a,a		;*2
	add	a,a		;*4
	add	a,e		;*5

ROUNDY662END:
	ld	e,a

ROUNDX66:
	ld	a,(SCREEN1)
	ld	d,a
	inc	d
	ld	a,0f0h
ROUNDX66LP:
	scf
	rra			;f8,fc,fe,ff
	dec	d
	jr	nz,ROUNDX66LP

	and	c
	ld	c,a
	ret			;d=0


;continued from INCGY
INCGY66:
	ld	a,(SCREEN1)
	dec	a
	ld	a,05h		;BASIC mode 5, screen mode 2
	jp	z,INCGYZ
	ld	a,01h		;BASIC mode 5, screen mode 3,4
	jp	p,INCGYZ
	ld	a,0ah		;BASIC mode 5, screen mode 1
	jp	INCGYZ


;e=e/10 (or e/5)
;input: e
;output: e=int(e/10), a=e mod 10
;destroy: f,hl
DIV5:
	ld	h,6
	jr	DIV10OR5
DIV10:
	ld	h,5
DIV10OR5:
	ld	a,e
	ld	e,00h
	ld	l,10100000b
DIV10LP:
	cp	l
	jr	c,DIV10C
	sub	l
DIV10C:
	ccf
	rl	e
	srl	l
	dec	h
	jr	nz,DIV10LP
	ret


BFG662:
	ex	de,hl
	ld	hl,(GRPX2)	;larger x
	ld	bc,(GRPX3)	;smaller x
	ld	a,c
	and	0f8h
	ld	c,a
;	or	a
	sbc	hl,bc

	ld	a,l
	srl	h
	rra
	srl	a
	srl	a		;int(larger/8)-int(smaller/8)

	ex	de,hl		;hl=VRAM address
	pop	de		;d=left part mask, e=right part mask
	pop	bc		;c=lines

	ld	b,a
;	or	a
	jr	nz,BFG66LP3

	ld	a,d
	and	e
	ld	d,a

BFG66LP3:
;left part
	push	bc
	push	de
	ld	c,e		;;
	call	PUT466

	dec	b		;
	jp	m,BFGNEXT66	;

;middle part
	push	hl		;VRAM address
	jr	z,BFGRHT66	;

BFG66LP4:
	inc	hl
	ld	a,(ATTDAT2)
	ld	(hl),a
	ld	a,(ATTDAT)
	res	5,h
	ld	(hl),a
	set	5,h
	djnz	BFG66LP4

;right part
BFGRHT66:
	inc	hl
	ld	d,c		;;
	call	PUT466
	pop	hl		;VRAM address

BFGNEXT66:
	ld	bc,0028h
	add	hl,bc
	pop	de
	pop	bc
	dec	c
	jr	nz,BFG66LP3
	jp	LINEEND2


;LINE command for mode 5
_C_LIN66:ds	C_LIN66-_C_LIN66
	org	C_LIN66

	call	PRELINE
	jp	nc,LINE
	jr	nz,LINEB66
	jr	LINBF66


;input: bc=x1, de=y1, (GRPX2)=x2, (GRPY2)=y2,
; (ATTDAT)=attribute, (ATTDAT2)=attribute2
;destroy: af,bc,de
_LINBF66:ds	LINBF66-_LINBF66
	org	LINBF66

	push	hl
	call	PUSHGXY
	call	SORTXY2

	ld	a,(SCREEN1)
	cp	02h
	jp	c,BF12

;line bf (graphic mdoe)
BFG66:
	ld	a,(GRPX2)	;larger x lower byte
	jr	nz,BF664
	inc	a		;screen mode 3
BF664:
	and	07h
	inc	a
	ld	b,a
	xor	a
BFG66LP1:
	scf
	rra			;right part mask
	djnz	BFG66LP1

;	or	a
	sbc	hl,de
	inc	l
	push	hl		;l=lines

	ld	l,a		;right part mask

	ld	a,c		;smaller x
	and	07h
	ld	b,a
	ld	a,0ffh
	jr	z,BFG66Z
BFG66LP2:
	srl	a
	djnz	BFG66LP2
BFG66Z:
	ld	h,a		;left part mask
	ld	bc,(GRPX3)	;smaller x

	push	hl
	call	GXY2GAD66

	jp	BFG662


;input: bc=x1, de=y1, (GRPX2)=x2, (GRPY2)=y2,
; (ATTDAT)=attribute1, (ATTDAT2)=attribute2
;destroy: af,bc,de
_LINEB66:ds	LINEB66-_LINEB66
	org	LINEB66

	jp	LINEB


;C_PAI66	equ	714ch
;PAINT command for mode 5
;_C_PAI66:ds	C_PAI66-_C_PAI66
;	org	C_PAI66
C_PAI66:

	call	GETGXY
	push	bc		;X
	push	de		;Y
	ld	a,(COLOR1)
	call	z,INT1INC
	call	SETATT
	ld	a,(hl)
	cp	','
	ld	a,(BORDERC)
	call	z,INT1INC
	ld	(BORDERC),a
	ld	e,a


;input: e=color, (sp)=y, (sp+2)=x
;used by PERDU, MINTMARK
_PA66POP:ds	PA66POP-_PA66POP
	org	PA66POP

	ld	a,e
	call	SETBO66
	pop	de		;Y
	pop	bc		;X
	jp	PAINT66


;input: bc=x, de=y, (ATTDAT)=attribute1, (ATTDAT2)=attribute2
; (BORDERA)=attribute, (BORDERA2)=attribute2, (BORDERC)=color
;destroy: af,bc,de
_PAINT66:ds	PAINT66-_PAINT66
	org	PAINT66

	push	hl
	call	CHKGXY66
	pop	hl
	ld	a,(SCREEN1)
	dec	a
	ret	z		;screen mode 2
	ld	(GRPX3),bc
	ld	(GRPY3),de
	push	hl
	jp	m,PAINT661	;screen mode 1
	call	GXY2GAD66
PAMAIN66:
	call	PADWN66
	pop	hl
	ret

PAINT661:
	call	GXY2AD66
	ld	d,0ffh
	jr	PAMAIN66


PADWN66:
	call	PALINE66
	ret	z

	push	hl		;address
	push	de		;bit
	ld	bc,(PAWRK)
	push	bc
	ld	bc,(GRPX3)
	push	bc
	ld	a,(GRPY3)
	push	af

	call	PAUP662

	pop	af
	ld	(GRPY3),a
	pop	hl
	ld	(GRPX3),hl
	pop	hl
	ld	(PAWRK),hl
	pop	de		;bit
	pop	hl		;address

PADWN662:
	push	de
	call	INCGY66
	ld	a,e
	pop	de
	cp	200
	ret	nc

	ld	bc,CLMN66
	add	hl,bc
	call	READVRAM

	xor	a
	ld	(PACNT),a

PADWN66LP1:
	call	SRCHOK66
	jr	c,CALLDWN66

	ld	bc,(GRPX3)
	push	bc
	ld	bc,(GRPY3)
	push	bc

	push	hl
	push	de
	ld	hl,PACNT
	inc	(hl)
	call	CHKFRE
	pop	de
	pop	hl

	push	hl		;address
	push	de		;bit

	call	SRCHNG66
	jr	nc,PADWN66LP1

CALLDWN66:
	ld	a,(PACNT)
	or	a
	ret	z

PADWN66LP2:
	pop	de		;bit
	pop	hl		;address
	pop	bc
	ld	(GRPY3),bc
	pop	bc
	ld	(GRPX3),bc

	dec	a
	ld	(PACNT),a
	jr	z,PADWN66
	push	af
	call	PADWN66
	pop	af
	jr	PADWN66LP2


PAUP66:
	call	PALINE66
	ret	z

	push	hl		;address
	push	de		;bit
	ld	bc,(PAWRK)
	push	bc
	ld	bc,(GRPX3)
	push	bc
	ld	a,(GRPY3)
	push	af

	call	PADWN662

	pop	af
	ld	(GRPY3),a
	pop	hl
	ld	(GRPX3),hl
	pop	hl
	ld	(PAWRK),hl
	pop	de		;bit
	pop	hl		;address

PAUP662:
;	ld	a,(GRPY3)
	or	a
	ret	z

	bit	5,h
	jr	nz,PAUP66NZ
	sub	09h
PAUP66NZ:
	dec	a
	ld	(GRPY3),a
	ld	bc,0-CLMN66
	add	hl,bc
	call	READVRAM

	xor	a
	ld	(PACNT),a

PAUP66LP1:
	call	SRCHOK66
	jr	c,CALLUP66

	ld	bc,(GRPX3)
	push	bc
	ld	bc,(GRPY3)
	push	bc

	push	hl
	push	de
	ld	hl,PACNT
	inc	(hl)
	call	CHKFRE
	pop	de
	pop	hl

	push	hl		;address
	push	de		;bit

	call	SRCHNG66
	jr	nc,PAUP66LP1

CALLUP66:
	ld	a,(PACNT)
	or	a
	ret	z

PAUP66LP2:
	pop	de		;bit
	pop	hl		;address
	pop	bc
	ld	(GRPY3),bc
	pop	bc
	ld	(GRPX3),bc

	dec	a
	ld	(PACNT),a
	jr	z,PAUP66
	push	af
	call	PAUP66
	pop	af
	jr	PAUP66LP2


PALINE66:
	call	READVRAM

	call	STOPESC
	call	CHKPA66
	ret	z

	push	hl		;address
	push	de		;bit
	ld	bc,(GRPX3)
	push	bc

	call	PAINTR66
	ld	hl,(GRPX3)
	ld	(PAWRK),hl	;X_right

	pop	hl
	ld	(GRPX3),hl
	pop	de		;bit
	pop	hl		;address

	call	READVRAM
	call	PAINTL66

	or	d		;d>0, reset z-flag
	ret


;paint to left direction for mode 5
;input: hl=address, d=bit
;output: hl,d
;destroy: af,bc
PAINTL66:
	ld	bc,(GRPX3)
	ld	a,b
	or	c
	ret	z
	call	DECGXPA66
	call	CHKPA66
	jr	nz,PAINTL66
;	jp	INCGXPA66


;increment x for paint for mode 5
;input: hl=address, d=bit
;output: hl, d, bc=(GRPX3)
;destroy: af,bc
INCGXPA66:
	ld	bc,(GRPX3)
	bit	5,h
	jr	z,INCGXPA661	;screen mode 1
	inc	bc
	rrc	d
	jp	po,INCGXPA664	;screen mode 4
	inc	bc		;screen mode 3
	rrc	d
INCGXPA664:
	ld	(GRPX3),bc
	ret	nc

	inc	hl
;	jr	READVRAM


;read VRAM
;input: hl=address
;output: (PAWRK2),(PAWRK2+1)
;destroy: af
READVRAM:
	bit	5,h
	ret	z
	call	READRAM
	ld	(PAWRK2),a
	res	5,h
	call	READRAM
	set	5,h
	ld	(PAWRK2+1),a
	ret


INCGXPA661:
	ld	a,c
	add	a,08h
	ld	c,a
	jr	nc,INCGXPA661NC
	inc	b
INCGXPA661NC:
	ld	(GRPX3),bc
	inc	hl
	ret


;paint to right direction for mode 5
;input: hl=address, d=bit
;output: hl,d
;destroy: af,bc
PAINTR66:
	call	GXLARGE66
	jr	z,DECGXPA66
	call	CHKPA66
	jr	nz,PAINTR66
;	jp	DECGXPA66


;decrement x for paint for mode 5
;input: hl=address, d=bit
;output: hl, d, bc=(GRPX3)
;destroy: af
DECGXPA66:
	ld	bc,(GRPX3)
	bit	5,h
	jr	z,DECGXPA661	;screen mode 1
	dec	bc
	rlc	d
	jp	po,DECGXPA664	;screen mode 4
	dec	c		;dec bc, screen mode3
	rlc	d
DECGXPA664:
	ld	(GRPX3),bc
	ret	nc

	dec	hl
	jr	READVRAM


DECGXPA661:
	ld	a,c
	sub	08h
	ld	c,a
	jr	nc,DECGXPA661NC
	dec	b
DECGXPA661NC:
	ld	(GRPX3),bc
	dec	hl
	ret


;check and paint for mode 5
;input: hl=address, d=bit
;output: z(1=not painted)
;destroy: af,e
CHKPA66:
	call	CMPBOR66
	ret	z

;	ld	bc,(PAWRK2)

	bit	5,h
	jr	z,CHKPA661

	ld	a,(ATTDAT2)
	xor	c
	and	d
	xor	c
	ld	(PAWRK2),a
	ld	(hl),a

	ld	a,(ATTDAT)
	xor	b
	and	d
	xor	b
	ld	(PAWRK2+1),a
	res	5,h
	ld	(hl),a
	set	5,h

	or	d		;d>0, reset z-flag
	ret

;screen mode 1
CHKPA661:
	call	READRAM
	ld	e,a
	ld	a,(ATTDAT)
	rlca
	rlca
	rlca
	rlca
	xor	e
	and	70h
	xor	e
	ld	(hl),a
	or	d		;d>0, reset z-flag
	ret


;search for paintable area for mode 5
;input: hl=address, d=bit
;output: hl,d,c-flag(1=over,0=OK)
;destroy: af,bc,e
SRCHOK66:
	call	CMPPAX66
	ret	c
	call	CMPATT66
	ret	nz
	call	GXLARGE66
	jr	nz,SRCHOK66
	scf
	ret


;search for unpaintable area for mode 5
;get unpaintable area
;input: hl=address, d=bit
;output: hl,d,c-flag(1=over,0=NG)
;destroy: af,bc
SRCHNG66:
	call	CMPPAX66
	ret	c
	call	CMPBOR66
	ret	z
	call	GXLARGE66
	jr	nz,SRCHNG66
	scf
	ret


;increment x and compare graphic width (=320)
;input: hl=address, d=bit
;output: hl, d, bc=(GRPX3), z-flag(1=over)
;destroy: af,bc
GXLARGE66:
	call	INCGXPA66
;	ld	bc,(GRPX3)
	dec	b
	ret	nz
	ld	a,c
	cp	320-256
	ret


;compare (GRPX3) to (PAWRK)
;output: f
;destroy: f,bc
CMPPAX66:
	push	hl
	ld	hl,(PAWRK)
	ld	bc,(GRPX3)
	or	a
	sbc	hl,bc
	pop	hl
	ret


;compare attribute and border color to VRAM
;output: bc=(PAWRK2), z-flag(1=same), c-flag=0
CMPATT66:
	bit	5,h
	jr	z,CMPATT661
	ld	bc,(PAWRK2)
	ld	a,(ATTDAT2)
	xor	c
	and	d
	jr	nz,CMPBOR662
	ld	a,(ATTDAT)
	xor	b
	and	d
	ret	z
	jr	CMPBOR662

;screen mode 1
CMPATT661:
	call	READRAM
	ld	e,a
	ld	a,(ATTDAT)
	rlca
	rlca
	rlca
	rlca
	xor	e
	and	70h
	ret	z

CMPBOR661:
	call	READRAM
	ld	e,a
	ld	a,(BORDERA)
	rlca
	rlca
	rlca
	rlca
	xor	e
	and	70h
	ret


;compare border color to VRAM
;output: bc=(PAWRK2), z-flag(1=same), c-flag=0
CMPBOR66:
	bit	5,h
	jr	z,CMPBOR661
	ld	bc,(PAWRK2)
CMPBOR662:
	ld	a,(BORDERA2)
	xor	c
	and	d
	ret	nz

	ld	a,(BORDERA)
	xor	b
	and	d
	ret


C_KANJ:
	ld	a,(SCREEN1)
	cp	02h
	jp	c,FCERR

	call	GETGXY
	ret	nz

	ld	(GRPX2),bc
	ld	(GRPY2),de

	ld	a,(COLOR1)
	call	SETATT

	call	SKIPSPINC
	ld	(PROGAD),hl
	cp	','
	jr	z,KANJLP1

	call	INT1ARG
	call	SETATT
	ld	a,(hl)
	cp	','
	ret	nz

KANJLP1:
	call	SKIPSPINC
	ld	(PROGAD),hl
	cp	','
	jr	z,KANJLP1

	call	CHKCLN
	ret	z

	call	ARG
	ld	a,(ARGTYP)
	dec	a
	jr	z,KANJSTR
	jp	p,SNERR

KANJNUM:
	call	FTOI

;check extkanji
	ld	hl,0000h
	call	READEKROM
	ld	a,h
	or	l
	jp	z,KANJEXT

	dec	de
	ex	de,hl
	ld	bc,0400h
	or	a
	sbc	hl,bc
	jp	nc,FCERR
	add	hl,bc

	add	hl,hl		;*2
	add	hl,hl		;*4
	add	hl,hl		;*8
	add	hl,hl		;*16

	ld	b,10h
KANJLP2:
	push	bc
	push	hl		;kanji ROM address
	call	READKROM
	call	CALLPUTHL
	pop	hl
	inc	hl		;kanji ROM address
	pop	bc
	djnz	KANJLP2

KLP2END:
	ld	hl,(GRPX2)
	ld	de,0010h
	add	hl,de
	ld	a,(SCREEN1)
	rrca
	jr	c,KNUMC		;screen mode 4
	add	hl,de		;screen mode 3
KNUMC:
	ld	(GRPX2),hl

KNEXT:
	ld	hl,(GRPY1)
	ld	(GRPY2),hl

	ld	hl,(PROGAD)
	dec	hl
	jr	KANJLP1

;string
KANJSTR:
	ld	hl,(GRPY2)
	ld	de,0006h
	add	hl,de
	ld	(GRPY2),hl

	ld	hl,(FAC1+1)
	ld	c,(hl)

	inc	hl
	inc	hl
	ld	b,(hl)
	inc	hl
	ld	h,(hl)
	ld	l,b

KANJLP3:
	ld	a,c
	or	a
	jr	z,KNEXT

	ld	a,(hl)
	cp	14h
	jr	nz,KSTRNZ
	dec	c
	jr	z,KNEXT
	inc	hl
	ld	a,(hl)
	sub	30h
KSTRNZ:
	push	hl		;string address
	call	CGROM

	ld	b,0ah
KANJLP4:
	push	bc
	push	de		;CGROM address
	call	READCGROM

	ld	h,d
	ld	l,00h
	call	CALLPUTHL
	pop	de
	inc	de
	pop	bc
	djnz	KANJLP4

KLP4END:
	ld	hl,(GRPX2)
	ld	de,0008h
	add	hl,de
	ld	a,(SCREEN1)
	rrca
	ld	a,08h
	jr	c,KSTRC		;screen mode 4
	add	hl,de		;screen mode 3
KSTRC:
	ld	(GRPX2),hl
	ld	hl,(GRPY1)
	ld	de,0006h
	add	hl,de
	ld	(GRPY2),hl

	pop	hl		;string address
	inc	hl
	dec	c
	jr	KANJLP3


;extended kanji ROM
KANJEXT:
	ex	de,hl

	ld	de,0020h
	rst	CPHLDE
	jp	c,FCERR
	ld	de,00ffh+1
	rst	CPHLDE
	jr	c,KEXT816

	ld	de,01ffh+1
	rst	CPHLDE
	jr	c,KEXT88

	ld	de,2121h
	rst	CPHLDE
	jp	c,FCERR
	ld	de,277eh+1
	rst	CPHLDE
	jr	c,KEXT1616

	ld	de,3021h
	rst	CPHLDE
	jp	c,FCERR
	ld	de,4f7eh+1
	rst	CPHLDE
	jp	nc,FCERR


;hl=3021h-4f7eh
;16x16 character
;JIS level-1 kanji set
;JIS    =0xxaaaaa 0bbccccc
;address=bbaaaaac cccc0000
	ld	a,l
	xor	h
	and	60h
	xor	h
	jr	KEXT16162


;hl=2120h-277eh
;16x16 character
;JIS    =00100aaa 0bbccccc
;address=00bbaaac cccc0000
KEXT1616:
	res	5,h
	ld	a,l
	and	60h
	rrca
	rrca
	or	h
KEXT16162:
	ld	h,a

	ld	a,l
	add	a,a
	add	a,a
	add	a,a
	add	a,a
	ld	l,a

	ld	a,h
	adc	a,a
	ld	h,a

	ld	b,10h
KANJLP5:
	push	bc
	push	hl		;kanji ROM address
	call	READEKROM
	call	CALLPUTHL
	pop	hl
	inc	hl		;kanji ROM address
	pop	bc
	djnz	KANJLP5
	jp	KLP2END


;hl=100h-1ffh
;8x8 character
KEXT88:
	ld	a,l
	ld	hl,(GRPY2)
	ld	bc,0008h
	add	hl,bc
	ld	(GRPY2),hl
	ld	l,a
	ld	h,02h

	ld	b,04h
	jr	KEXT8162

;hl=20h-ffh
;8x16 character
KEXT816:
	ld	b,08h
	add	hl,hl		;*2
KEXT8162:
	add	hl,hl		;*4
	add	hl,hl		;*8
KANJLP6:
	push	bc
	push	hl		;address
	call	READEKROM
	push	hl
	ld	l,00h
	call	CALLPUTHL
	pop	hl
	ld	h,l
	ld	l,00h
	call	CALLPUTHL
	pop	hl
	inc	hl		;kanji ROM address
	pop	bc
	djnz	KANJLP6
	jp	KLP2END


;read kanji ROM data
;input: hl=address
;output: h=left data, l=right data
;destroy: none
READKROM:
	push	af
	push	bc
	ld	a,03h
	out	(0c3h),a	;port c2h direction: bit0,1=output
	ld	a,12h
	di
	out	(0f0h),a	;0000-3fff:voice/kanji ROM
	ld	a,01h
	out	(0c2h),a	;kanji left
	ld	b,(hl)
	ld	a,03h
	out	(0c2h),a	;kanji right
	ld	l,(hl)
	ld	a,11h
	out	(0f0h),a	;0000-7fff:BASIC ROM
	ei
	ld	h,b
	pop	bc
	pop	af
	ret


;read extended kanji ROM data
;input: hl=address
;output: h=left data, l=right data
;destroy: none
READEKROM:
	push	af
	push	bc
	ld	b,l
	ld	c,0fch
	out	(c),h
	xor	a
	out	(0ffh),a
	inc	c
	in	h,(c)		;c=fdh
	inc	c
	in	l,(c)		;c=feh
	dec	a
	out	(0ffh),a	;a=ffh
	pop	bc
	pop	af
	ret


;check Y, call PUTHL, increment Y
;input: h,l=data, (GRPX2),(GRPY2),(ATTDAT),(ATTDAT2)
;destroy: af,bc,de,hl
CALLPUTHL:
	push	hl
	ld	hl,199
	ld	de,(GRPY2)
	rst	CPHLDE
	pop	hl
	ret	c
	ld	bc,(GRPX2)
	call	PUTHL
	inc	de
	ld	(GRPY2),de
	ret


;put 2-byte data in graphic mode (screen mode 3,4)
;input: bc=x, de=y,h,l=data, (ATTDAT),(ATTDAT2)
;output: bc=bc+16or32
;destroy: af,hl
PUTHL:
	ld	(GRPX3),bc
	ld	(GRPY3),de

	push	hl		;

;check y range
	bit	7,d
	jp	nz,FCERR
	ld	hl,199
	rst	CPHLDE
	jr	nc,PUTHLYOK
	ex	de,hl
PUTHLYOK:

;check x range
	bit	7,b
	jr	z,PUTHLXOK
	ld	b,00h
	ld	a,c
	and	07h
	ld	c,a

PUTHLXOK:
	call	GXY2GAD662
	ld	b,h
	ld	c,l
	pop	hl		;

;h-l -> d-h-l (left justified)
	ld	a,(SCREEN1)
	rrca
	ld	a,d
	ld	d,00h
	jr	nc,PUTHL3

PUTHLLP1:
	add	hl,hl
	rl	d
	rrca
	jr	nc,PUTHLLP1

	ld	a,l
	push	af
	push	hl
	push	de

	ld	h,b
	ld	l,c

	ld	b,03h
PUTHLLP2:
	push	hl
	ld	hl,(GRPX3)	;start x
	ld	de,320
	rst	CPHLDE
	ld	de,0008h
	push	af
	add	hl,de
	pop	af
	ld	(GRPX3),hl
	pop	hl

	pop	de

	jr	nc,PUTHLNC
	call	PUT466
	inc	hl
PUTHLNC:
	djnz	PUTHLLP2

	ld	hl,(GRPX3)
	ld	de,0-0008h
	add	hl,de
	ld	b,h
	ld	c,l
	ld	de,(GRPY3)
	ret


;for screen mode 3
PUTHL3:
;h-l -> d-h-l (left justified)
PUTHLLP3:
	add	hl,hl
	rl	d
	rrca
	rrca
	jr	nc,PUTHLLP3

	ld	a,l
	call	STRETCH
	push	af
	ld	a,l
	rrca
	rrca
	rrca
	rrca
	call	STRETCH
	push	af

	ld	a,h
	call	STRETCH
	push	af
	ld	a,h
	rrca
	rrca
	rrca
	rrca
	call	STRETCH
	push	af

	ld	a,d
	call	STRETCH
	push	af

	ld	h,b
	ld	l,c

	ld	b,05h
	jr	PUTHLLP2

STRETCH:
	push	de
	ld	d,a
	ld	e,04h
STRETCHLP:
	rrc	d
	rra
	rlca
	rrca
	rra
	dec	e
	jr	nz,STRETCHLP
	pop	de
	ret


;TALK command
C_TALK:

;for N66 voicerom
	ld	a,0c9h
	ld	(0fe66h),a

;	call	STRARG

	call	CHKCLN
	ret	z
	call	STRARG3

	ld	hl,TALKRET
	push	hl
	ld	hl,CHGROM
	push	hl
	ld	hl,TALK
	push	hl
	ex	de,hl		;hl=string address
	ld	e,a		;e=length
	push	af		;dummy for OUTF0H

	ld	a,03h
	out	(0c3h),a	;port c2h direction: bit0,1=output
	xor	a
	out	(0c2h),a	;bit0=0:VOICE ROM
	ld	a,21h		;0000-3fff:BASIC ROM, 4000-7fff:VOICE ROM

;for N66 voicerom
	ld	(PORTF0H),a

	jp	OUTF0H

TALKRET:
	cp	20h
	jp	nz,FCERR

	ret


C_MON:
	ld	sp,(STACK)

	push	af		;for Dezeni Land
	push	af		;for Dezeni Land

	ld	a,41h
	ld	(MONFLG),a
	ld	hl,0000h
	add	hl,sp
	ld	(REGSP),hl

	ld	a,0c3h
	ld	(HOOK38H),a
	ld	hl,MONBRK
	ld	(HOOK38H+1),hl
	ld	hl,REGDATA
	ld	de,REGF
	ld	bc,0008h
	ldir

MONLP1:
	ld	hl,READY
	call	MPUTS
MONLP2:
	call	GETPUTC
	ld	hl,MONTBL1
	ld	de,MONTBL2
	ld	b,10h
MONLP3:
	cp	(hl)
	jr	z,MONCALL
	inc	hl
	inc	de
	inc	de
	djnz	MONLP3
	jr	MONERR

MONCALL:
	ex	de,hl
	ld	e,(hl)
	inc	hl
	ld	d,(hl)
	ex	de,hl
	call	JPHL
	jr	MONLP1

MONTBL1:
	db	'B',  'C',  'D',  'E',  'F',  'G',  'L',  'M'
	db	'P',  'Q',  'R',  'S',  'W',  'X',  '?',  03h
MONTBL2:
	dw	MONB, MONC, MOND, MONE, MONF, MONG, MONL, MONM
	dw	MONP, MONQ, MONR, MONS, MONW, MONX, MONQMARK, MONSTOP

MONQMARK:
	ld	hl,MONFLG
	ld	a,(hl)
	xor	40h
	ld	(hl),a
;	jr	MONERR

MONERR:
	ld	hl,MONERRSTR
	call	MPUTS
	ld	hl,(REGSP)
	ld	sp,hl
	jp	MONLP2

MONERRSTR:
	db	"ERROR!?"

READY:
	db	0dh, 0ah, "READY", 0dh, 0ah, 00h


;MON B command
MONB:
	ld	a,'-'
	call	MPUTC
	call	GETPUTC
	jr	z,MONBASIC
	cp	'R'
	jr	z,MONBR
	cp	'G'
	jr	z,MONBG
	jr	MONERR

MONBR:
	ld	hl,(STARTAD)
	ld	d,h
	ld	e,l
	inc	de
	inc	de
	inc	de
MONBRLP1:
	inc	de
	ld	a,(de)
	or	a
	jr	nz,MONBRLP1
	inc	de
	ld	(hl),e
	inc	hl
	ld	(hl),d
MONBRLP2:
	ex	de,hl		;hl=link pointer address
	ld	e,(hl)
	inc	hl
	ld	d,(hl)
	ld	a,d
	or	e
	jr	nz,MONBRLP2
	inc	hl
	ld	(VARAD),hl
	call	RESSTK

MONBASIC:
	ld	sp,(STACK)
	jp	edit

MONBG:
	ld	hl,0000h
	ld	(BRKADR1),hl
	ld	(BRKADR2),hl
	call	GETPUTC
	cp	0dh
	jr	z,MONBGRUN
	cp	','
	jr	z,MONBGZ1
	cp	' '
	jp	nz,MONERR
MONBGZ1:
	ld	a,'-'
	call	MPUTC
	call	GETHEX
	ex	de,hl		;
	cp	0dh
	jr	z,MONBGZ

	ld	a,'-'
	call	MPUTC
	call	GETHEX
	cp	0dh
	jp	nz,MONERR
	ld	(BRKADR2),hl
	ld	a,(hl)
	ld	(BRKSAV2),a
	ld	(hl),0ffh
MONBGZ:
	ex	de,hl		;
	ld	(BRKADR1),hl
	ld	a,(hl)
	ld	(BRKSAV1),a
	ld	(hl),0ffh

MONBGRUN:
	call	RESSTK
	ld	hl,(STARTAD)
	jp	RUNLP

MONBRK:
	ld	(REGSP),sp
	di
	ld	sp,REGPC
	push	hl
	push	de
	push	bc
	push	af
	ld	sp,(REGSP)
	ei

	ld	hl,BRKSTR
	call	MPUTS
	pop	hl
	dec	hl
	ld	(REGPC),hl
	call	PUTHEX4
	call	MPUTNL

	ld	hl,(BRKADR1)
	ld	a,h
	or	l
	jr	z,NOBRK1
	ld	a,(BRKSAV1)
	ld	(hl),a
NOBRK1:
	ld	hl,(BRKADR2)
	ld	a,h
	or	l
	jr	z,NOBRK2
	ld	a,(BRKSAV2)
	ld	(hl),a
NOBRK2:
	call	RESSTK
	jp	MONLP2

BRKSTR:
	db	"BREAK!! ADDR=", 00h


;MON C command
MONC:
	call	MPUTNL
	ld	hl,MONFLG
	ld	a,(hl)
	bit	5,a
	jr	nz,MONCNZ		;printer on?
	or	20h
	ld	(hl),a
	ld	hl,PRTON
	jp	MPUTS

MONCNZ:
	rrca
	jr	nc,MONCNC		;printer only?
	ccf
	rla
	ld	(hl),a
	call	CSROFF
	ld	hl,PRTONLY
	jp	MPUTS

MONCNC:
	rlca
	xor	21h
	ld	(hl),a
	ld	hl,PRTOFF
	jp	MPUTS

PRTON:
	db	'PRT ON', 00h
PRTONLY:
	db	'PRT ONLY', 00h
PRTOFF:
	db	'PRT OFF', 00h


;MON D command
MOND:
	call	GETHEX2
	ex	de,hl

;hl...de
MONDLP1:
	ld	a,' '
	call	MPUTC
	call	PUTHEX4
	ld	a,':'
	call	MPUTC

MONDLP2:
	ld	a,' '
	call	MPUTC
	ld	a,(MONFLG)
	and	40h
	ld	a,(hl)
	call	nz,READRAM
	call	PUTHEX2
	rst	CPHLDE
	jr	nc,MONDEND
	inc	hl
	ld	a,l
	and	07h
	jr	nz,MONDLP2
	call	MPUTNL
	jr	MONDLP1

MONDEND:
	jp	MPUTNL


;MON E command
MONE:
	call	MPUTNL
	ld	hl,MONFLG
	ld	a,(hl)
	xor	10h
	ld	(hl),a
	and	10h
	ld	hl,ECHOON
	jr	nz,MONENZ
	ld	hl,ECHOOFF
MONENZ:
	jp	MPUTS

ECHOON:
	db	"ECHO ON", 00h
ECHOOFF:
	db	"ECHO OFF", 00h


;MON F command
MONF:
	call	GETHEX1
	jp	z,MONERR
	push	hl		;hl=start

	call	GETHEX2
	ld	c,l		;c=data
	pop	hl
MONFLP:
	ld	(hl),c
	rst	CPHLDE
	ret	nc
	inc	hl
	jr	MONFLP


;MON G command
MONG:
	ld	hl,0000h
	ld	(BRKADR1),hl
	ld	(BRKADR2),hl
	call	GETHEX
	ex	de,hl		;
	cp	0dh
	jr	z,MONGZ
	ld	a,'-'
	call	MPUTC
	call	GETHEX1
	ld	(BRKADR1),hl
	ld	a,(hl)
	ld	(BRKSAV1),a
	ld	(hl),0ffh
	jr	z,MONGZ
	ld	a,'-'
	call	MPUTC
	call	GETHEX1
	jp	nz,MONERR
	ld	(BRKADR2),hl
	ld	a,(hl)
	ld	(BRKSAV2),a
	ld	(hl),0ffh

MONGZ:
	ex	de,hl		;
	jp	(hl)


;MON L/R command
MONL:
MONR:
	cpl
	ld	hl,MONFLG
	xor	(hl)
	and	08h		;'L'->00h, 'R'->08h
	xor	(hl)
	ld	(hl),a

	ex	de,hl
	ld	a,'-'
	call	MPUTC
	call	GETHEX1
	jp	nz,MONERR
	ex	de,hl
	push	de		;offset address
	jp	LOADHEX


;load Intel HEX data
;used by CORRIDOR
;input: (sp)=offset, (sp+2)=return address, (MONFLG)-b3(1:CMT,0:RS-232C)
_LOADHEX:ds	LOADHEX-_LOADHEX
	org	LOADHEX

	call	DUMMY		;just return
	pop	de
	bit	3,(hl)		;(MONFLG)
	call	nz,INPOPN
MONRLP1:
	call	MGETDEV
	cp	0dh
	jr	nz,MONRERR
	call	MGETDEV
	cp	0ah
	jr	nz,MONRERR
	call	MGETDEV
	cp	':'
	jr	nz,MONRERR
	call	MDEVHEX2
	jr	z,MONREND
	ld	b,a		;length
	call	MDEVHEX2	;address (high)
	ld	h,a
	call	MDEVHEX2	;address (low)
	ld	l,a
	add	hl,de
	add	a,b
	add	a,h
	ld	c,a		;check sum=b+h+l+...
	call	MDEVHEX2	;record type
	jr	nz,MONRERR
MONRLP2:
	call	MDEVHEX2
	ld	(hl),a
	add	a,c		;check sum
	ld	c,a
	inc	hl
	djnz	MONRLP2

	call	MDEVHEX2	;check sum
	add	a,c
	jr	nz,MONRERR
	jr	MONRLP1

MONREND:
	call	MGETDEV
	cp	1ah
	jr	nz,MONREND
	jp	RCLOSE

;error for MON-R
MONRERR:
	call	RCLOSE
	jp	MONERR


;MON M command
MONM:
	call	GETHEX1
	jp	z,MONERR

	push	hl
	call	GETHEX2
	ld	b,h
	ld	c,l
	pop	hl

	rst	CPHLDE
	jr	c,MONMLP
	ex	de,hl

MONMLP:
	ld	a,(MONFLG)
	and	40h
	ld	a,(hl)
	call	nz,READRAM
	ld	(bc),a
	rst	CPHLDE
	inc	hl
	inc	bc
	jr	c,MONMLP
	ret


;MON P/W command
MONP:
MONW:
	sub	04h		;'P'->4ch, 'W'->53h
	and	05h		;04h, 01h
	rrca			;02h,80h
	push	af

	call	GETHEX2
	rst	CPHLDE
	jp	c,MONERR
	ex	de,hl		;hl=start address

	pop	af		;'P'->02h, 'W'->80h
	ld	(DEVICE),a
	call	c,PRTOPN
MONWLP1:
	ld	a,0dh
	call	PUTDEV
	ld	a,0ah
	call	PUTDEV
	ld	a,':'

	ld	b,10h		;16 bytes
	call	PUTDEV
	ld	a,b		;length
	call	PUTDEVHEX2
	ld	a,h		;address (high)
	call	PUTDEVHEX2
	ld	a,l		;address (low)
	call	PUTDEVHEX2
	xor	a		;record type
	call	PUTDEVHEX2

	xor	a
	sub	b
	sub	h
	sub	l
	ld	c,a		;check sum
MONWLP2:
	ld	a,(hl)
	call	PUTDEVHEX2
	ld	a,c
	sub	(hl)
	ld	c,a		;check sum
	rst	CPHLDE
	jr	nc,MONWEND
	inc	hl
	djnz	MONWLP2
	ld	a,c
	call	PUTDEVHEX2
	jr	MONWLP1

MONWLP3:
	xor	a
	call	PUTDEVHEX2
MONWEND:
	djnz	MONWLP3

	ld	a,c
	call	PUTDEVHEX2
	ld	a,0dh
	call	PUTDEV
	ld	a,0ah
	call	PUTDEV
	ld	a,':'
	call	PUTDEV
	ld	b,08h
MONWLP4:
	xor	a
	call	PUTDEVHEX2
	djnz	MONWLP4
	ld	a,1ah
	call	PUTDEV
	call	WCLOSE
	xor	a
	ld	(DEVICE),a
	ret


;MON Q command
MONQ:
	call	GETHEX2
	ex	de,hl

;hl...de
MONQLP1:
	ld	a,' '
	call	MPUTC
	call	PUTHEX4
	ld	a,':'
	call	MPUTC
	ld	a,' '
	call	MPUTC

MONQLP2:
	ld	a,(MONFLG)
	and	40h
	ld	a,(hl)
	call	nz,READRAM

	cp	20h
	jr	nc,MONQNC
	ld	a,'.'
MONQNC:
	call	MPUTC
	rst	CPHLDE
	jp	nc,MPUTNL
	inc	hl
	ld	a,l
	and	0fh
	jr	nz,MONQLP2
	call	MPUTNL
	jr	MONQLP1


;MON S command
MONS:
	call	GETHEX1
	ret	z
MONSLP:
	ld	a,(MONFLG)
	and	40h
	ld	a,(hl)
	call	nz,READRAM
	call	PUTHEX2
	ld	a,'-'
	call	MPUTC
	ex	de,hl
	call	GETHEX
	ex	de,hl
	jr	nz,MONSNZ
	ld	(hl),e
MONSNZ:
	cp	0dh
	ret	z
	inc	hl
	ld	a,l
	and	0fh
	jr	nz,MONSLP
	call	MPUTNL
	call	PUTHEX4
	ld	a,':'
	call	MPUTC
	ld	a,' '
	call	MPUTC
	jr	MONSLP


;MON X command
MONX:
	call	GETPUTC
	ld	bc,REGWKTBL
	jr	z,PRTREG
	sub	'A'
	jp	z,SETREG
	inc	bc
	inc	bc
	dec	a		;B
	jp	z,SETREG
	inc	bc
	inc	bc
	dec	a		;C
	jp	z,SETREG
	inc	bc
	inc	bc
	dec	a		;D
	jr	z,SETREG
	inc	bc
	inc	bc
	dec	a		;E
	jr	z,SETREG
	inc	bc
	inc	bc
	dec	a		;F
	jr	z,SETREG
	inc	bc
	inc	bc
	cp	'H'-'F'
	jr	z,SETREG
	inc	bc
	inc	bc
	cp	'L'-'F'
	jr	z,SETREG

	cp	'M'
	jp	z,SETREGM
	cp	'P'
	jp	z,SETREGP
	cp	'S'
	jp	z,SETREGS
	jp	MONERR

PRTREG:
	ld	de,REGEQTBL

	ld	l,08h

PRTREGLP1:
	push	hl
	ld	a,(de)
	ld	l,a
	inc	de
	ld	a,(de)
	ld	h,a
	inc	de
PRTREGLP2:
	ld	a,(hl)
	or	a
	jr	z,PRTREGZ
	call	MPUTC
	inc	hl
	jr	PRTREGLP2
PRTREGZ:
	ld	a,(bc)
	ld	l,a
	inc	bc
	ld	a,(bc)
	ld	h,a
	inc	bc
	ld	a,(hl)
	push	bc
	call	PUTHEX2
	pop	bc
	pop	hl
	dec	l
	jr	nz,PRTREGLP1

	ld	hl,REGMEQ
	call	MPUTS
	ld	hl,(REGL)
	call	READRAM
	call	PUTHEX2

	ld	hl,REGPEQ
	call	MPUTS
	ld	hl,(REGPC)
	call	PUTHEX4

	ld	hl,REGSEQ
	call	MPUTS
	ld	hl,(REGSP)
	jp	PUTHEX4

SETREG:
	ld	a,' '
	call	MPUTC
SETREGLP:
	cp	0dh
	ret	z

	ld	hl,REGWKTBLEND-1
	or	a
	sbc	hl,bc
	jp	c,SETREGM

	ld	a,(bc)
	ld	l,a
	inc	bc
	ld	a,(bc)
	ld	h,a
	inc	bc
	ld	a,(hl)
	ex	de,hl		;
	push	bc
	call	PUTHEX2
	ld	a,'-'
	call	MPUTC
	call	GETHEX
	pop	bc
	jr	nz,SETREGLP
	ex	de,hl		;
	ld	(hl),e
	cp	','
	jr	z,SETREGLP
	ret

SETREGM:
SETREGP
SETREGS:
	ret

REGEQTBL:
	dw	REGAEQ, REGBEQ, REGCEQ, REGDEQ, REGEEQ, REGFEQ,
	dw	REGHEQ, REGLEQ

REGWKTBL:
	dw	REGA, REGB, REGC, REGD, REGE, REGF, REGH, REGL
REGWKTBLEND:
REGAEQ:
	db	" A=", 00h
REGBEQ:
	db	" B=", 00h
REGCEQ:
	db	" C=", 00h
REGDEQ:
	db	" D=", 00h
REGEEQ:
	db	" E=", 00h
REGFEQ:
	db	" F=", 00h
REGHEQ:
	db	0dh, 0ah, " H=", 00h
REGLEQ:
	db	" L=", 00h
REGMEQ:
	db	" M=", 00h
REGPEQ:
	db	" P=", 00h
REGSEQ:
	db	" S=", 00h


;output: a,z-flag
;destroy: f
MDEVHEX2:
	push	bc
	call	MDEVAN
	rlca
	rlca
	rlca
	rlca
	ld	b,a
	call	MDEVAN
	or	b
	pop	bc
	ret

;alphabet or numeric
;output: a
;destroy: f
MDEVAN:
	call	MGETDEV
	sub	'0'
	cp	'9'-'0'+1
	ret	c
	sub	'A'-'0'
	cp	'F'-'A'+1
	jp	nc,MONRERR
	add	a,0ah
	ret

;output: a
;destroy: none
MGETDEV:
	ld	a,(MONFLG)
	and	08h
	jr	z,MGETDEVLP	;RS-232C
	call	GETCMT
	jp	nz,MONRERR
	jr	MGETDEVEND

MGETDEVLP:
	ld	a,(STOPFLG)
	cp	03h
	jp	z,MONSTOP2
	call	GETRSBF
	jr	z,MGETDEVLP

MGETDEVEND:
	push	af
	ld	a,(MONFLG)
	and	10h
	jr	z,MGETDEVZ
	pop	af
	jp	MPUTC2

MGETDEVZ:
	pop	af
DUMMY:
	ret

MONSTOP2:
	call	GETCH
MONSTOP:
	ld	hl,(REGSP)
	ld	sp,hl
	push	af		;for Dezeni Land
	push	af		;for Dezeni Land
	call	BELL
	ld	hl,BREAK+1
	call	MPUTS
	jp	MONLP1


;destroy: af
PUTDEVHEX2:
	push	af
	rlca
	rlca
	rlca
	rlca
	call	PUTDEVHEX1
	pop	af

PUTDEVHEX1:
	and	0fh
	cp	0ah
	sbc	a,69h
	daa
	call	PUTDEV
	ret


;input: hl
;destroy: af,b
PUTHEX4:
	ld	a,h
	call	PUTHEX2
	ld	a,l
;	jr	PUTHEX2

;input: a
;destroy: af,b
PUTHEX2:
	ld	b,a
	rrca
	rrca
	rrca
	rrca
	call	PUTHEX1
	ld	a,b
;	jr	PUTHEX1

;input: a
;destroy: af
PUTHEX1:
	and	0fh
	cp	0ah
	sbc	a,69h
	daa
	jp	MPUTC


;output: de=start address, hl=end address
GETHEX2:
	call	GETHEX1
	jp	z,MONERR
	ex	de,hl
	call	GETHEX1
	jp	nz,MONERR
	ret

;output: hl=address, z(terminator=0dh?)
GETHEX1:
	call	GETHEX
	jp	nz,MONERR
	cp	0dh
	ret


;get hex value keyboard input
;output: hl, a(0dh or ',' or ' ' if no error), z-flag(0=no input)
;destroy: af
GETHEX:
	ld	hl,0000h
	call	GETPUTC
	jr	z,GETHEXZ
	cp	' '
	jr	z,GETHEXZ
	cp	','
	jr	nz,GETHEXNZ
GETHEXZ:
	or	a
	ret

GETHEXLP:
	call	GETPUTC
	ret	z
	cp	','
	ret	z
	cp	' '
	ret	z
GETHEXNZ:
	sub	'0'
	cp	'9'-'0'+1
	jr	c,GETHEX0F
	sub	'A'-'0'
	cp	'F'-'A'+1
	jp	nc,MONERR
	add	a,0ah
GETHEX0F:
	add	hl,hl		;*2
	add	hl,hl		;*4
	add	hl,hl		;*8
	add	hl,hl		;*16
	add	a,l
	ld	l,a
	jr	GETHEXLP

;output: a,z-flag(1=return)
;destroy: f
GETPUTC:
;	call	GETC
	call	MGETC
	cp	0dh
	jr	z,GETPUTCRET
	cp	0feh		;page switching key
	jr	nz,MPUTC
	ld	a,(SCREEN2)
	inc	a
	ld	hl,PAGES
	cp	(hl)
	jr	c,GETPUTCC
	xor	a
GETPUTCC:
	call	CHGSCR
	jr	GETPUTC

GETPUTCRET:
	push	af
	call	MPUTNL
	pop	af
	ret


;get a character for MON
;output: a,z-flag(1=return)
;destroy: f
MGETC:
	ld	a,(MONFLG)
	rrca
	call	c,CSRON
MGETCLP:
	call	GETCH
	jr	z,MGETCLP
	push	hl
	call	CSROFF
	pop	hl
	call	TOUPPER
	ret

;put string for MON
MPUTS:
	ld	a,(hl)
	or	a
	ret	z
	call	MPUTC
	inc	hl
	jr	MPUTS


;destroy:  a
;put new line for MON
MPUTNL:
	ld	a,0dh
	call	MPUTC
	ld	a,0ah
;	jr	MPUTC

;put a character for MON
;input: a
;destroy: none
MPUTC:
	push	af
	call	GETCH
	jr	z,MPUTCZ
	call	MGETC
	cp	03h
	jp	z,MONSTOP
MPUTCZ:
	pop	af

;no stop check
;destroy: none
MPUTC2:
	push	af
	push	bc
	ld	b,a		;
	ld	a,(MONFLG)
	ld	c,a		;;
	rrca
	ld	a,b		;
	call	c,PUTC
	ld	a,c		;;
	and	20h
	ld	a,01h
	ld	(DEVICE),a
	ld	a,b		;
	call	nz,PUTDEV
	xor	a
	ld	(DEVICE),a
	pop	bc
	pop	af
	ret


;DELETE command
C_DEL:
	pop	af		;cancel return address
	call	CHKCLN
	jp	z,FCERR

	call	GETLN
	push	de		;start line number
	call	CHKCLN
	jr	z,DELSTRT

	cp	MINUS
	jp	nz,SNERR
	call	CHKCLNINC
	jr	nz,DELNZ
	pop	de
	ld	bc,65530
	push	bc
	jr	DELSTRT

DELNZ:
	call	GETLN
	call	CHKCLN
	jp	nz,SNERR

	pop	hl
	push	de		;push end line number
	ex	de,hl		;de <- start line number

;de=start line number, (sp)=end line number
DELSTRT:
	call	SRCHLN
	pop	de		;end line number
	push	bc		;start address
	call	SRCHLN
	jr	c,DELC
	ld	h,b		;end address
	ld	l,c
DELC:
	pop	de		;start address

	ex	de,hl
	rst	CPHLDE		;start-end
	jp	nc,EDIT
	ex	de,hl

	ld	b,d
	ld	c,e
	push	bc		;dummy,(bc)=0
	call	DELPRG
	call	RESSTK
	call	CHGLKP
	jp	EDIT


;66 ROM end
_8000H:	ds	8000h-_8000H

	end
