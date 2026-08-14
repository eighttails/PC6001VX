;simple alternative system ROM 1 file for PC-6001mkIISR/6601SR
; by AKIKAWA, Hisashi	2022-2026

;This software is redistributable under the LGPLv2.1 or any later version.


	org	0000h

BOOT:
	di

	ld	hl,START
	ld	de,0ff00h
	ld	bc,SYS1END-START
	ldir
	jp	0ff00h


START:
;memory read/write
	ld	hl,IOTBL60+0ff00h-START
	ld	bc,1060h
IO60LP:
	outi
	inc	c
	ld	a,b
	or	b
	jr	nz,IO60LP

;interrupt
	ld	a,0feh
	out	(0fbh),a	;interrupt vector address control
	ld	a,0ffh
	out	(0fah),a	;interrupt control (0=enable,1=disable)
				;bit0:sub CPU	DI
				;bit1:joy stick	DI
				;bit2:timer	DI
				;bit3:voice	DI
				;bit4:VRTC	DI
				;bit5:RS-232C	DI
				;bit6:printer	DI
				;bit7:ext int	DI

;interrupt address
	ld	hl,IOTBLB8+0ff00h-START
	ld	bc,08b8h
IOB8LP:
	outi
	inc	c
	ld	a,b
	or	b
	jr	nz,IOB8LP


;display
	ld	a,0fdh		;CRT controller type
	out	(0c8h),a
	ld	a,07h		;32x16 text mode
	out	(0c1h),a
;	ld	a,04h		;relay off, timer on, VRAM=0000h
;	out	(0b0h),a

	rst	00h

;table
IOTBL60:
	db	0f0h		;0000-1fff:66-ROM 0000-1fff for reading
	db	0f2h		;2000-3fff:66-ROM 2000-3fff for reading
	db	0f4h		;4000-5fff:66-ROM 4000-5fff for reading
	db	0f6h		;6000-7fff:66-ROM 6000-7fff for reading
	db	08h		;8000-9fff:internal RAM 8000-9fff for reading
	db	0ah		;a000-bfff:internal RAM a000-bfff for reading
	db	0ch		;c000-dfff:internal RAM c000-dfff for reading
	db	0eh		;e000-ffff:internal RAM e000-ffff for reading
	db	00h		;0000-1fff:internal RAM 0000-1fff for writing
	db	02h		;2000-3fff:internal RAM 2000-3fff for writing
	db	04h		;4000-5fff:internal RAM 4000-5fff for writing
	db	06h		;6000-7fff:internal RAM 6000-7fff for writing
	db	08h		;8000-9fff:internal RAM 8000-9fff for writing
	db	0ah		;a000-bfff:internal RAM a000-bfff for writing
	db	0ch		;c000-dfff:internal RAM c000-dfff for writing
	db	0eh		;e000-ffff:internal RAM e000-ffff for writing

IOTBLB8:
	db	02h		;interrupt address (sub CPU)
	db	10h		;interrupt address (joy stick)
	db	06h		;interrupt address (timer)
	db	20h		;interrupt address (voice)
	db	22h		;interrupt address (VRTC)
	db	10h		;interrupt address (RS-232C)
	db	10h		;interrupt address (printer)
	db	10h		;interrupt address (ext int)

SYS1END:
	ds	8000h-SYS1END
	end
