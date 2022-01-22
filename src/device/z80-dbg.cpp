/////////////////////////////////////////////////////////////////////////////
//  P C 6 0 0 1 V
//  Copyright 1999,2021 Yumitaro
/////////////////////////////////////////////////////////////////////////////
//***************************************************************************
// QUASI88 --- PC-8801 emulator
//	 Copyright (C) Showzoh Fukunaga 1998
//***************************************************************************
#include "common.h"
#include "z80.h"


#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

// オペランドの型
//	NOTHING	なし	 XX				そのまま表示
//	NUM_8	8bit値	 XX nn			nn を16進で表示
//	NUM_16	16bit値	 XX nn nn		nn nn を16進で表示
//	ADR_REL	相対番地 XX nn			絶対番地を16進で表示
//	PREFIX
//	NOTHING	なし	 XX XX			そのまま表示
//	NUM_8	8bit値	 XX XX nn		nn を16進で表示
//	NUM_16	16bit値	 XX XX nn nn	nn nn を16進で表示
//	INDEX	参照	 XX XX nn		nn を 10進で表示
//	IDX_NUM	参照&値	 XX XX nn mm	nn を 10進、mm を16進で表示
//	UNEXIST	未実装	 XX XX			XX XX を16進で表示
//	SKIP	未実装	 XX				XX を16進で表示
//	PREFIX
//	INDEX	参照	 XX XX nn XX	nn を 10進で表示
//	UNEXIST	未実装	 XX XX XX XX	XX XX XX XX を16進で表示

enum{
	OP_NOTHING,	// XX			-> nothing
	OP_NUM_8,	// XX nn		-> nnH
	OP_NUM_16,	// XX nn mm		-> mmnnH
	OP_ADR_REL,	// XX nn		-> PC+nn
	OP_PREFIX,
	OP_INDEX,	// XX XX nn		-> +nn(d)
	OP_IDX_NUM,	// XX XX nn mm	-> +nn(d),mmH
	OP_UNEXIST,
	OP_SKIP		// XX			-> DB XXH
};


typedef struct{
	int Type;
	const char* Str;
} Mnemonics;


static Mnemonics Instruction[256] =
{
	{OP_NOTHING,	" NOP"},			{OP_NUM_16,		" LD   BC,%04XH"},
	{OP_NOTHING,	" LD   (BC),A"},	{OP_NOTHING,	" INC  BC"},
	{OP_NOTHING,	" INC  B"},			{OP_NOTHING,	" DEC  B"},
	{OP_NUM_8,		" LD   B,%02XH"},	{OP_NOTHING,	" RLCA"},
	{OP_NOTHING,	" EX   AF,AF'"},	{OP_NOTHING,	" ADD  HL,BC"},
	{OP_NOTHING,	" LD   A,(BC)"},	{OP_NOTHING,	" DEC  BC"},
	{OP_NOTHING,	" INC  C"},			{OP_NOTHING,	" DEC  C"},
	{OP_NUM_8,		" LD   C,%02XH"},	{OP_NOTHING,	" RRCA"},

	{OP_ADR_REL,	" DJNZ %04XH"},		{OP_NUM_16,		" LD   DE,%04XH"},
	{OP_NOTHING,	" LD   (DE),A"},	{OP_NOTHING,	" INC  DE"},
	{OP_NOTHING,	" INC  D"},			{OP_NOTHING,	" DEC  D"},
	{OP_NUM_8,		" LD   D,%02XH"},	{OP_NOTHING,	" RLA"},
	{OP_ADR_REL,	" JR   %04XH"},		{OP_NOTHING,	" ADD  HL,DE"},
	{OP_NOTHING,	" LD   A,(DE)"},	{OP_NOTHING,	" DEC  DE"},
	{OP_NOTHING,	" INC  E"},			{OP_NOTHING,	" DEC  E"},
	{OP_NUM_8,		" LD   E,%02XH"},	{OP_NOTHING,	" RRA"},

	{OP_ADR_REL,	" JR   NZ,%04XH"},	{OP_NUM_16,		" LD   HL,%04XH"},
	{OP_NUM_16,		" LD   (%04XH),HL"},{OP_NOTHING,	" INC  HL"},
	{OP_NOTHING,	" INC  H"},			{OP_NOTHING,	" DEC  H"},
	{OP_NUM_8,		" LD   H,%02XH"},	{OP_NOTHING,	" DAA"},
	{OP_ADR_REL,	" JR   Z,%04XH"},	{OP_NOTHING,	" ADD  HL,HL"},
	{OP_NUM_16,		" LD   HL,(%04XH)"},{OP_NOTHING,	" DEC  HL"},
	{OP_NOTHING,	" INC  L"},			{OP_NOTHING,	" DEC  L"},
	{OP_NUM_8,		" LD   L,%02XH"},	{OP_NOTHING,	" CPL"},

	{OP_ADR_REL,	" JR   NC,%04XH"},	{OP_NUM_16,		" LD   SP,%04XH"},
	{OP_NUM_16,		" LD   (%04XH),A"},	{OP_NOTHING,	" INC  SP"},
	{OP_NOTHING,	" INC  (HL)"},		{OP_NOTHING,	" DEC  (HL)"},
	{OP_NUM_8,		" LD   (HL),%02XH"},{OP_NOTHING,	" SCF"},
	{OP_ADR_REL,	" JR   C,%04XH"},	{OP_NOTHING,	" ADD  HL,SP"},
	{OP_NUM_16,		" LD   A,(%04XH)"},	{OP_NOTHING,	" DEC  SP"},
	{OP_NOTHING,	" INC  A"},			{OP_NOTHING,	" DEC  A"},
	{OP_NUM_8,		" LD   A,%02XH"},	{OP_NOTHING,	" CCF"},

	{OP_NOTHING,	" LD   B,B"},		{OP_NOTHING,	" LD   B,C"},
	{OP_NOTHING,	" LD   B,D"},		{OP_NOTHING,	" LD   B,E"},
	{OP_NOTHING,	" LD   B,H"},		{OP_NOTHING,	" LD   B,L"},
	{OP_NOTHING,	" LD   B,(HL)"},	{OP_NOTHING,	" LD   B,A"},
	{OP_NOTHING,	" LD   C,B"},		{OP_NOTHING,	" LD   C,C"},
	{OP_NOTHING,	" LD   C,D"},		{OP_NOTHING,	" LD   C,E"},
	{OP_NOTHING,	" LD   C,H"},		{OP_NOTHING,	" LD   C,L"},
	{OP_NOTHING,	" LD   C,(HL)"},	{OP_NOTHING,	" LD   C,A"},

	{OP_NOTHING,	" LD   D,B"},		{OP_NOTHING,	" LD   D,C"},
	{OP_NOTHING,	" LD   D,D"},		{OP_NOTHING,	" LD   D,E"},
	{OP_NOTHING,	" LD   D,H"},		{OP_NOTHING,	" LD   D,L"},
	{OP_NOTHING,	" LD   D,(HL)"},	{OP_NOTHING,	" LD   D,A"},
	{OP_NOTHING,	" LD   E,B"},		{OP_NOTHING,	" LD   E,C"},
	{OP_NOTHING,	" LD   E,D"},		{OP_NOTHING,	" LD   E,E"},
	{OP_NOTHING,	" LD   E,H"},		{OP_NOTHING,	" LD   E,L"},
	{OP_NOTHING,	" LD   E,(HL)"},	{OP_NOTHING,	" LD   E,A"},

	{OP_NOTHING,	" LD   H,B"},		{OP_NOTHING,	" LD   H,C"},
	{OP_NOTHING,	" LD   H,D"},		{OP_NOTHING,	" LD   H,E"},
	{OP_NOTHING,	" LD   H,H"},		{OP_NOTHING,	" LD   H,L"},
	{OP_NOTHING,	" LD   H,(HL)"},	{OP_NOTHING,	" LD   H,A"},
	{OP_NOTHING,	" LD   L,B"},		{OP_NOTHING,	" LD   L,C"},
	{OP_NOTHING,	" LD   L,D"},		{OP_NOTHING,	" LD   L,E"},
	{OP_NOTHING,	" LD   L,H"},		{OP_NOTHING,	" LD   L,L"},
	{OP_NOTHING,	" LD   L,(HL)"},	{OP_NOTHING,	" LD   L,A"},

	{OP_NOTHING,	" LD   (HL),B"},	{OP_NOTHING,	" LD   (HL),C"},
	{OP_NOTHING,	" LD   (HL),D"},	{OP_NOTHING,	" LD   (HL),E"},
	{OP_NOTHING,	" LD   (HL),H"},	{OP_NOTHING,	" LD   (HL),L"},
	{OP_NOTHING,	" HALT"},			{OP_NOTHING,	" LD   (HL),A"},
	{OP_NOTHING,	" LD   A,B"},		{OP_NOTHING,	" LD   A,C"},
	{OP_NOTHING,	" LD   A,D"},		{OP_NOTHING,	" LD   A,E"},
	{OP_NOTHING,	" LD   A,H"},		{OP_NOTHING,	" LD   A,L"},
	{OP_NOTHING,	" LD   A,(HL)"},	{OP_NOTHING,	" LD   A,A"},

	{OP_NOTHING,	" ADD  A,B"},		{OP_NOTHING,	" ADD  A,C"},
	{OP_NOTHING,	" ADD  A,D"},		{OP_NOTHING,	" ADD  A,E"},
	{OP_NOTHING,	" ADD  A,H"},		{OP_NOTHING,	" ADD  A,L"},
	{OP_NOTHING,	" ADD  A,(HL)"},	{OP_NOTHING,	" ADD  A,A"},
	{OP_NOTHING,	" ADC  A,B"},		{OP_NOTHING,	" ADC  A,C"},
	{OP_NOTHING,	" ADC  A,D"},		{OP_NOTHING,	" ADC  A,E"},
	{OP_NOTHING,	" ADC  A,H"},		{OP_NOTHING,	" ADC  A,L"},
	{OP_NOTHING,	" ADC  A,(HL)"},	{OP_NOTHING,	" ADC  A,A"},

	{OP_NOTHING,	" SUB  B"},			{OP_NOTHING,	" SUB  C"},
	{OP_NOTHING,	" SUB  D"},			{OP_NOTHING,	" SUB  E"},
	{OP_NOTHING,	" SUB  H"},			{OP_NOTHING,	" SUB  L"},
	{OP_NOTHING,	" SUB  (HL)"},		{OP_NOTHING,	" SUB  A"},
	{OP_NOTHING,	" SBC  A,B"},		{OP_NOTHING,	" SBC  A,C"},
	{OP_NOTHING,	" SBC  A,D"},		{OP_NOTHING,	" SBC  A,E"},
	{OP_NOTHING,	" SBC  A,H"},		{OP_NOTHING,	" SBC  A,L"},
	{OP_NOTHING,	" SBC  A,(HL)"},	{OP_NOTHING,	" SBC  A,A"},

	{OP_NOTHING,	" AND  B"},			{OP_NOTHING,	" AND  C"},
	{OP_NOTHING,	" AND  D"},			{OP_NOTHING,	" AND  E"},
	{OP_NOTHING,	" AND  H"},			{OP_NOTHING,	" AND  L"},
	{OP_NOTHING,	" AND  (HL)"},		{OP_NOTHING,	" AND  A"},
	{OP_NOTHING,	" XOR  B"},			{OP_NOTHING,	" XOR  C"},
	{OP_NOTHING,	" XOR  D"},			{OP_NOTHING,	" XOR  E"},
	{OP_NOTHING,	" XOR  H"},			{OP_NOTHING,	" XOR  L"},
	{OP_NOTHING,	" XOR  (HL)"},		{OP_NOTHING,	" XOR  A"},

	{OP_NOTHING,	" OR   B"},			{OP_NOTHING,	" OR   C"},
	{OP_NOTHING,	" OR   D"},			{OP_NOTHING,	" OR   E"},
	{OP_NOTHING,	" OR   H"},			{OP_NOTHING,	" OR   L"},
	{OP_NOTHING,	" OR   (HL)"},		{OP_NOTHING,	" OR   A"},
	{OP_NOTHING,	" CP   B"},			{OP_NOTHING,	" CP   C"},
	{OP_NOTHING,	" CP   D"},			{OP_NOTHING,	" CP   E"},
	{OP_NOTHING,	" CP   H"},			{OP_NOTHING,	" CP   L"},
	{OP_NOTHING,	" CP   (HL)"},		{OP_NOTHING,	" CP   A"},

	{OP_NOTHING,	" RET  NZ"},		{OP_NOTHING,	" POP  BC"},
	{OP_NUM_16,		" JP   NZ,%04XH"},	{OP_NUM_16,		" JP   %04XH"},
	{OP_NUM_16,		" CALL NZ,%04XH"},	{OP_NOTHING,	" PUSH BC"},
	{OP_NUM_8,		" ADD  A,%02XH"},	{OP_NOTHING,	" RST  00H"},
	{OP_NOTHING,	" RET  Z"},			{OP_NOTHING,	" RET"},
	{OP_NUM_16,		" JP   Z,%04XH"},	{OP_PREFIX,		nullptr},
	{OP_NUM_16,		" CALL Z,%04XH"},	{OP_NUM_16,		" CALL %04XH"},
	{OP_NUM_8,		" ADC  A,%02XH"},	{OP_NOTHING,	" RST  08H"},

	{OP_NOTHING,	" RET  NC"},		{OP_NOTHING,	" POP  DE"},
	{OP_NUM_16,		" JP   NC,%04XH"},	{OP_NUM_8,		" OUT  (%02XH),A"},
	{OP_NUM_16,		" CALL NC,%04XH"},	{OP_NOTHING,	" PUSH DE"},
	{OP_NUM_8,		" SUB  %02XH"},		{OP_NOTHING,	" RST  10H"},
	{OP_NOTHING,	" RET  C"},			{OP_NOTHING,	" EXX"},
	{OP_NUM_16,		" JP   C,%04XH"},	{OP_NUM_8,		" IN   A,(%02XH)"},
	{OP_NUM_16,		" CALL C,%04XH"},	{OP_PREFIX,		nullptr},
	{OP_NUM_8,		" SBC  A,%02XH"},	{OP_NOTHING,	" RST  18H"},

	{OP_NOTHING,	" RET  PO"},		{OP_NOTHING,	" POP  HL"},
	{OP_NUM_16,		" JP   PO,%04XH"},	{OP_NOTHING,	" EX   HL,(SP)"},
	{OP_NUM_16,		" CALL PO,%04XH"},	{OP_NOTHING,	" PUSH HL"},
	{OP_NUM_8,		" AND  %02XH"},		{OP_NOTHING,	" RST  20H"},
	{OP_NOTHING,	" RET  PE"},		{OP_NOTHING,	" JP   (HL)"},
	{OP_NUM_16,		" JP   PE,%04XH"},	{OP_NOTHING,	" EX   DE,HL"},
	{OP_NUM_16,		" CALL PE,%04XH"},	{OP_PREFIX,		nullptr},
	{OP_NUM_8,		" XOR  %02XH"},		{OP_NOTHING,	" RST  28H"},

	{OP_NOTHING,	" RET  P"},			{OP_NOTHING,	" POP  AF"},
	{OP_NUM_16,		" JP   P,%04XH"},	{OP_NOTHING,	" DI"},
	{OP_NUM_16,		" CALL P,%04XH"},	{OP_NOTHING,	" PUSH AF"},
	{OP_NUM_8,		" OR   %02XH"},		{OP_NOTHING,	" RST  30H"},
	{OP_NOTHING,	" RET  M"},			{OP_NOTHING,	" LD   SP,HL"},
	{OP_NUM_16,		" JP   M,%04XH"},	{OP_NOTHING,	" EI"},
	{OP_NUM_16,		" CALL M,%04XH"},	{OP_PREFIX,		nullptr},
	{OP_NUM_8,		" CP   %02XH"},		{OP_NOTHING,	" RST  38H"}
};

static Mnemonics Instruction_CB[256]=
{
	{OP_NOTHING,	" RLC  B"},			{OP_NOTHING,	" RLC  C"},
	{OP_NOTHING,	" RLC  D"},			{OP_NOTHING,	" RLC  E"},
	{OP_NOTHING,	" RLC  H"},			{OP_NOTHING,	" RLC  L"},
	{OP_NOTHING,	" RLC  (HL)"},		{OP_NOTHING,	" RLC  A"},
	{OP_NOTHING,	" RRC  B"},			{OP_NOTHING,	" RRC  C"},
	{OP_NOTHING,	" RRC  D"},			{OP_NOTHING,	" RRC  E"},
	{OP_NOTHING,	" RRC  H"},			{OP_NOTHING,	" RRC  L"},
	{OP_NOTHING,	" RRC  (HL)"},		{OP_NOTHING,	" RRC  A"},
	
	{OP_NOTHING,	" RL   B"},			{OP_NOTHING,	" RL   C"},
	{OP_NOTHING,	" RL   D"},			{OP_NOTHING,	" RL   E"},
	{OP_NOTHING,	" RL   H"},			{OP_NOTHING,	" RL   L"},
	{OP_NOTHING,	" RL   (HL)"},		{OP_NOTHING,	" RL   A"},
	{OP_NOTHING,	" RR   B"},			{OP_NOTHING,	" RR   C"},
	{OP_NOTHING,	" RR   D"},			{OP_NOTHING,	" RR   E"},
	{OP_NOTHING,	" RR   H"},			{OP_NOTHING,	" RR   L"},
	{OP_NOTHING,	" RR   (HL)"},		{OP_NOTHING,	" RR   A"},
	
	{OP_NOTHING,	" SLA  B"},			{OP_NOTHING,	" SLA  C"},
	{OP_NOTHING,	" SLA  D"},			{OP_NOTHING,	" SLA  E"},
	{OP_NOTHING,	" SLA  H"},			{OP_NOTHING,	" SLA  L"},
	{OP_NOTHING,	" SLA  (HL)"},		{OP_NOTHING,	" SLA  A"},
	{OP_NOTHING,	" SRA  B"},			{OP_NOTHING,	" SRA  C"},
	{OP_NOTHING,	" SRA  D"},			{OP_NOTHING,	" SRA  E"},
	{OP_NOTHING,	" SRA  H"},			{OP_NOTHING,	" SRA  L"},
	{OP_NOTHING,	" SRA  (HL)"},		{OP_NOTHING,	" SRA  A"},
	
	{OP_NOTHING,	"+SLL  B"},			{OP_NOTHING,	"+SLL  C"},
	{OP_NOTHING,	"+SLL  D"},			{OP_NOTHING,	"+SLL  E"},
	{OP_NOTHING,	"+SLL  H"},			{OP_NOTHING,	"+SLL  L"},
	{OP_NOTHING,	"+SLL  (HL)"},		{OP_NOTHING,	"+SLL  A"},
	{OP_NOTHING,	" SRL   B"},		{OP_NOTHING,	" SRL   C"},
	{OP_NOTHING,	" SRL   D"},		{OP_NOTHING,	" SRL   E"},
	{OP_NOTHING,	" SRL   H"},		{OP_NOTHING,	" SRL   L"},
	{OP_NOTHING,	" SRL   (HL)"},		{OP_NOTHING,	" SRL   A"},
	
	{OP_NOTHING,	" BIT  0,B"},		{OP_NOTHING,	" BIT  0,C"},
	{OP_NOTHING,	" BIT  0,D"},		{OP_NOTHING,	" BIT  0,E"},
	{OP_NOTHING,	" BIT  0,H"},		{OP_NOTHING,	" BIT  0,L"},
	{OP_NOTHING,	" BIT  0,(HL)"},	{OP_NOTHING,	" BIT  0,A"},
	{OP_NOTHING,	" BIT  1,B"},		{OP_NOTHING,	" BIT  1,C"},
	{OP_NOTHING,	" BIT  1,D"},		{OP_NOTHING,	" BIT  1,E"},
	{OP_NOTHING,	" BIT  1,H"},		{OP_NOTHING,	" BIT  1,L"},
	{OP_NOTHING,	" BIT  1,(HL)"},	{OP_NOTHING,	" BIT  1,A"},
	
	{OP_NOTHING,	" BIT  2,B"},		{OP_NOTHING,	" BIT  2,C"},
	{OP_NOTHING,	" BIT  2,D"},		{OP_NOTHING,	" BIT  2,E"},
	{OP_NOTHING,	" BIT  2,H"},		{OP_NOTHING,	" BIT  2,L"},
	{OP_NOTHING,	" BIT  2,(HL)"},	{OP_NOTHING,	" BIT  2,A"},
	{OP_NOTHING,	" BIT  3,B"},		{OP_NOTHING,	" BIT  3,C"},
	{OP_NOTHING,	" BIT  3,D"},		{OP_NOTHING,	" BIT  3,E"},
	{OP_NOTHING,	" BIT  3,H"},		{OP_NOTHING,	" BIT  3,L"},
	{OP_NOTHING,	" BIT  3,(HL)"},	{OP_NOTHING,	" BIT  3,A"},
	
	{OP_NOTHING,	" BIT  4,B"},		{OP_NOTHING,	" BIT  4,C"},
	{OP_NOTHING,	" BIT  4,D"},		{OP_NOTHING,	" BIT  4,E"},
	{OP_NOTHING,	" BIT  4,H"},		{OP_NOTHING,	" BIT  4,L"},
	{OP_NOTHING,	" BIT  4,(HL)"},	{OP_NOTHING,	" BIT  4,A"},
	{OP_NOTHING,	" BIT  5,B"},		{OP_NOTHING,	" BIT  5,C"},
	{OP_NOTHING,	" BIT  5,D"},		{OP_NOTHING,	" BIT  5,E"},
	{OP_NOTHING,	" BIT  5,H"},		{OP_NOTHING,	" BIT  5,L"},
	{OP_NOTHING,	" BIT  5,(HL)"},	{OP_NOTHING,	" BIT  5,A"},
	
	{OP_NOTHING,	" BIT  6,B"},		{OP_NOTHING,	" BIT  6,C"},
	{OP_NOTHING,	" BIT  6,D"},		{OP_NOTHING,	" BIT  6,E"},
	{OP_NOTHING,	" BIT  6,H"},		{OP_NOTHING,	" BIT  6,L"},
	{OP_NOTHING,	" BIT  6,(HL)"},	{OP_NOTHING,	" BIT  6,A"},
	{OP_NOTHING,	" BIT  7,B"},		{OP_NOTHING,	" BIT  7,C"},
	{OP_NOTHING,	" BIT  7,D"},		{OP_NOTHING,	" BIT  7,E"},
	{OP_NOTHING,	" BIT  7,H"},		{OP_NOTHING,	" BIT  7,L"},
	{OP_NOTHING,	" BIT  7,(HL)"},	{OP_NOTHING,	" BIT  7,A"},
	
	{OP_NOTHING,	" RES  0,B"},		{OP_NOTHING,	" RES  0,C"},
	{OP_NOTHING,	" RES  0,D"},		{OP_NOTHING,	" RES  0,E"},
	{OP_NOTHING,	" RES  0,H"},		{OP_NOTHING,	" RES  0,L"},
	{OP_NOTHING,	" RES  0,(HL)"},	{OP_NOTHING,	" RES  0,A"},
	{OP_NOTHING,	" RES  1,B"},		{OP_NOTHING,	" RES  1,C"},
	{OP_NOTHING,	" RES  1,D"},		{OP_NOTHING,	" RES  1,E"},
	{OP_NOTHING,	" RES  1,H"},		{OP_NOTHING,	" RES  1,L"},
	{OP_NOTHING,	" RES  1,(HL)"},	{OP_NOTHING,	" RES  1,A"},
	
	{OP_NOTHING,	" RES  2,B"},		{OP_NOTHING,	" RES  2,C"},
	{OP_NOTHING,	" RES  2,D"},		{OP_NOTHING,	" RES  2,E"},
	{OP_NOTHING,	" RES  2,H"},		{OP_NOTHING,	" RES  2,L"},
	{OP_NOTHING,	" RES  2,(HL)"},	{OP_NOTHING,	" RES  2,A"},
	{OP_NOTHING,	" RES  3,B"},		{OP_NOTHING,	" RES  3,C"},
	{OP_NOTHING,	" RES  3,D"},		{OP_NOTHING,	" RES  3,E"},
	{OP_NOTHING,	" RES  3,H"},		{OP_NOTHING,	" RES  3,L"},
	{OP_NOTHING,	" RES  3,(HL)"},	{OP_NOTHING,	" RES  3,A"},
	
	{OP_NOTHING,	" RES  4,B"},		{OP_NOTHING,	" RES  4,C"},
	{OP_NOTHING,	" RES  4,D"},		{OP_NOTHING,	" RES  4,E"},
	{OP_NOTHING,	" RES  4,H"},		{OP_NOTHING,	" RES  4,L"},
	{OP_NOTHING,	" RES  4,(HL)"},	{OP_NOTHING,	" RES  4,A"},
	{OP_NOTHING,	" RES  5,B"},		{OP_NOTHING,	" RES  5,C"},
	{OP_NOTHING,	" RES  5,D"},		{OP_NOTHING,	" RES  5,E"},
	{OP_NOTHING,	" RES  5,H"},		{OP_NOTHING,	" RES  5,L"},
	{OP_NOTHING,	" RES  5,(HL)"},	{OP_NOTHING,	" RES  5,A"},
	
	{OP_NOTHING,	" RES  6,B"},		{OP_NOTHING,	" RES  6,C"},
	{OP_NOTHING,	" RES  6,D"},		{OP_NOTHING,	" RES  6,E"},
	{OP_NOTHING,	" RES  6,H"},		{OP_NOTHING,	" RES  6,L"},
	{OP_NOTHING,	" RES  6,(HL)"},	{OP_NOTHING,	" RES  6,A"},
	{OP_NOTHING,	" RES  7,B"},		{OP_NOTHING,	" RES  7,C"},
	{OP_NOTHING,	" RES  7,D"},		{OP_NOTHING,	" RES  7,E"},
	{OP_NOTHING,	" RES  7,H"},		{OP_NOTHING,	" RES  7,L"},
	{OP_NOTHING,	" RES  7,(HL)"},	{OP_NOTHING,	" RES  7,A"},
	
	{OP_NOTHING,	" SET  0,B"},		{OP_NOTHING,	" SET  0,C"},
	{OP_NOTHING,	" SET  0,D"},		{OP_NOTHING,	" SET  0,E"},
	{OP_NOTHING,	" SET  0,H"},		{OP_NOTHING,	" SET  0,L"},
	{OP_NOTHING,	" SET  0,(HL)"},	{OP_NOTHING,	" SET  0,A"},
	{OP_NOTHING,	" SET  1,B"},		{OP_NOTHING,	" SET  1,C"},
	{OP_NOTHING,	" SET  1,D"},		{OP_NOTHING,	" SET  1,E"},
	{OP_NOTHING,	" SET  1,H"},		{OP_NOTHING,	" SET  1,L"},
	{OP_NOTHING,	" SET  1,(HL)"},	{OP_NOTHING,	" SET  1,A"},
	
	{OP_NOTHING,	" SET  2,B"},		{OP_NOTHING,	" SET  2,C"},
	{OP_NOTHING,	" SET  2,D"},		{OP_NOTHING,	" SET  2,E"},
	{OP_NOTHING,	" SET  2,H"},		{OP_NOTHING,	" SET  2,L"},
	{OP_NOTHING,	" SET  2,(HL)"},	{OP_NOTHING,	" SET  2,A"},
	{OP_NOTHING,	" SET  3,B"},		{OP_NOTHING,	" SET  3,C"},
	{OP_NOTHING,	" SET  3,D"},		{OP_NOTHING,	" SET  3,E"},
	{OP_NOTHING,	" SET  3,H"},		{OP_NOTHING,	" SET  3,L"},
	{OP_NOTHING,	" SET  3,(HL)"},	{OP_NOTHING,	" SET  3,A"},
	
	{OP_NOTHING,	" SET  4,B"},		{OP_NOTHING,	" SET  4,C"},
	{OP_NOTHING,	" SET  4,D"},		{OP_NOTHING,	" SET  4,E"},
	{OP_NOTHING,	" SET  4,H"},		{OP_NOTHING,	" SET  4,L"},
	{OP_NOTHING,	" SET  4,(HL)"},	{OP_NOTHING,	" SET  4,A"},
	{OP_NOTHING,	" SET  5,B"},		{OP_NOTHING,	" SET  5,C"},
	{OP_NOTHING,	" SET  5,D"},		{OP_NOTHING,	" SET  5,E"},
	{OP_NOTHING,	" SET  5,H"},		{OP_NOTHING,	" SET  5,L"},
	{OP_NOTHING,	" SET  5,(HL)"},	{OP_NOTHING,	" SET  5,A"},
	
	{OP_NOTHING,	" SET  6,B"},		{OP_NOTHING,	" SET  6,C"},
	{OP_NOTHING,	" SET  6,D"},		{OP_NOTHING,	" SET  6,E"},
	{OP_NOTHING,	" SET  6,H"},		{OP_NOTHING,	" SET  6,L"},
	{OP_NOTHING,	" SET  6,(HL)"},	{OP_NOTHING,	" SET  6,A"},
	{OP_NOTHING,	" SET  7,B"},		{OP_NOTHING,	" SET  7,C"},
	{OP_NOTHING,	" SET  7,D"},		{OP_NOTHING,	" SET  7,E"},
	{OP_NOTHING,	" SET  7,H"},		{OP_NOTHING,	" SET  7,L"},
	{OP_NOTHING,	" SET  7,(HL)"},	{OP_NOTHING,	" SET  7,A"}
};

static Mnemonics Instruction_ED[256]=
{
	{OP_UNEXIST,	"+DB   %02XH,%02XH"},	{OP_UNEXIST,	"+DB   %02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH"},	{OP_UNEXIST,	"+DB   %02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH"},	{OP_UNEXIST,	"+DB   %02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH"},	{OP_UNEXIST,	"+DB   %02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH"},	{OP_UNEXIST,	"+DB   %02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH"},	{OP_UNEXIST,	"+DB   %02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH"},	{OP_UNEXIST,	"+DB   %02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH"},	{OP_UNEXIST,	"+DB   %02XH,%02XH"},
	
	{OP_UNEXIST,	"+DB   %02XH,%02XH"},	{OP_UNEXIST,	"+DB   %02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH"},	{OP_UNEXIST,	"+DB   %02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH"},	{OP_UNEXIST,	"+DB   %02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH"},	{OP_UNEXIST,	"+DB   %02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH"},	{OP_UNEXIST,	"+DB   %02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH"},	{OP_UNEXIST,	"+DB   %02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH"},	{OP_UNEXIST,	"+DB   %02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH"},	{OP_UNEXIST,	"+DB   %02XH,%02XH"},
	
	{OP_UNEXIST,	"+DB   %02XH,%02XH"},	{OP_UNEXIST,	"+DB   %02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH"},	{OP_UNEXIST,	"+DB   %02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH"},	{OP_UNEXIST,	"+DB   %02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH"},	{OP_UNEXIST,	"+DB   %02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH"},	{OP_UNEXIST,	"+DB   %02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH"},	{OP_UNEXIST,	"+DB   %02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH"},	{OP_UNEXIST,	"+DB   %02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH"},	{OP_UNEXIST,	"+DB   %02XH,%02XH"},
	
	{OP_UNEXIST,	"+DB   %02XH,%02XH"},	{OP_UNEXIST,	"+DB   %02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH"},	{OP_UNEXIST,	"+DB   %02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH"},	{OP_UNEXIST,	"+DB   %02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH"},	{OP_UNEXIST,	"+DB   %02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH"},	{OP_UNEXIST,	"+DB   %02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH"},	{OP_UNEXIST,	"+DB   %02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH"},	{OP_UNEXIST,	"+DB   %02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH"},	{OP_UNEXIST,	"+DB   %02XH,%02XH"},
	
	{OP_NOTHING,	" IN   B,(C)"},		{OP_NOTHING,	" OUT  (C),B"},
	{OP_NOTHING,	" SBC  HL,BC"},		{OP_NUM_16,		" LD   (%04XH),BC"},
	{OP_NOTHING,	" NEG"},			{OP_NOTHING,	" RETN"},
	{OP_NOTHING,	" IM   0"},			{OP_NOTHING,	" LD   I,A"},
	{OP_NOTHING,	" IN   C,(C)"},		{OP_NOTHING,	" OUT  (C),C"},
	{OP_NOTHING,	" ADC  HL,BC"},		{OP_NUM_16,		" LD   BC,(%04XH)"},
	{OP_UNEXIST,	"+NEG"},			{OP_NOTHING,	" RETI"},
	{OP_UNEXIST,	"+IM   0"},			{OP_NOTHING,	" LD   R,A"},
	
	{OP_NOTHING,	" IN   D,(C)"},		{OP_NOTHING,	" OUT  (C),D"},
	{OP_NOTHING,	" SBC  HL,DE"},		{OP_NUM_16,		" LD   (%04XH),DE"},
	{OP_UNEXIST,	"+NEG"},			{OP_UNEXIST,	"+RETN"},
	{OP_NOTHING,	" IM   1"},			{OP_NOTHING,	" LD   A,I"},
	{OP_NOTHING,	" IN   E,(C)"},		{OP_NOTHING,	" OUT  (C),E"},
	{OP_NOTHING,	" ADC  HL,DE"},		{OP_NUM_16,		" LD   DE,(%04XH)"},
	{OP_UNEXIST,	"+NEG"},			{OP_UNEXIST,	"+RETI"},
	{OP_NOTHING,	" IM   2"},			{OP_NOTHING,	" LD   A,R"},
	
	{OP_NOTHING,	" IN   H,(C)"},		{OP_NOTHING,	" OUT  (C),H"},
	{OP_NOTHING,	" SBC  HL,HL"},		{OP_NUM_16,		"+LD   (%04XH),HL"},
	{OP_UNEXIST,	"+NEG"},			{OP_UNEXIST,	"+RETN"},
	{OP_UNEXIST,	"+IM   0"},			{OP_NOTHING,	" RRD"},
	{OP_NOTHING,	" IN   L,(C)"},		{OP_NOTHING,	" OUT  (C),L"},
	{OP_NOTHING,	" ADC  HL,HL"},		{OP_NUM_16,		"+LD   HL,(%04XH)"},
	{OP_UNEXIST,	"+NEG"},			{OP_UNEXIST,	"+RETI"},
	{OP_UNEXIST,	"+IM   0"},			{OP_NOTHING,	" RLD"},
	
	{OP_NOTHING,	"+IN   F,(C)"},		{OP_UNEXIST,	"+OUT  (C),F"},
	{OP_NOTHING,	" SBC  HL,SP"},		{OP_NUM_16,		" LD   (%04XH),SP"},
	{OP_UNEXIST,	"+NEG"},			{OP_UNEXIST,	"+RETN"},
	{OP_UNEXIST,	"+IM   1"},			{OP_UNEXIST,	"+DB   %02XH,%02XH"},
	{OP_NOTHING,	" IN   A,(C)"},		{OP_NOTHING,	" OUT  (C),A"},
	{OP_NOTHING,	" ADC  HL,SP"},		{OP_NUM_16,		" LD   SP,(%04XH)"},
	{OP_UNEXIST,	"+NEG"},			{OP_UNEXIST,	"+RETI"},
	{OP_UNEXIST,	"+IM   2"},			{OP_UNEXIST,	"+DB   %02XH,%02XH"},
	
	{OP_UNEXIST,	"+DB   %02XH,%02XH"},	{OP_UNEXIST,	"+DB   %02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH"},	{OP_UNEXIST,	"+DB   %02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH"},	{OP_UNEXIST,	"+DB   %02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH"},	{OP_UNEXIST,	"+DB   %02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH"},	{OP_UNEXIST,	"+DB   %02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH"},	{OP_UNEXIST,	"+DB   %02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH"},	{OP_UNEXIST,	"+DB   %02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH"},	{OP_UNEXIST,	"+DB   %02XH,%02XH"},
	
	{OP_UNEXIST,	"+DB   %02XH,%02XH"},	{OP_UNEXIST,	"+DB   %02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH"},	{OP_UNEXIST,	"+DB   %02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH"},	{OP_UNEXIST,	"+DB   %02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH"},	{OP_UNEXIST,	"+DB   %02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH"},	{OP_UNEXIST,	"+DB   %02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH"},	{OP_UNEXIST,	"+DB   %02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH"},	{OP_UNEXIST,	"+DB   %02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH"},	{OP_UNEXIST,	"+DB   %02XH,%02XH"},
	
	{OP_NOTHING,	" LDI"},				{OP_NOTHING,	" CPI"},
	{OP_NOTHING,	" INI"},				{OP_NOTHING,	" OUTI"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH"},	{OP_UNEXIST,	"+DB   %02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH"},	{OP_UNEXIST,	"+DB   %02XH,%02XH"},
	{OP_NOTHING,	" LDD"},				{OP_NOTHING,	" CPD"},
	{OP_NOTHING,	" IND"},				{OP_NOTHING,	" OUTD"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH"},	{OP_UNEXIST,	"+DB   %02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH"},	{OP_UNEXIST,	"+DB   %02XH,%02XH"},
	
	{OP_NOTHING,	" LDIR"},				{OP_NOTHING,	" CPIR"},
	{OP_NOTHING,	" INIR"},				{OP_NOTHING,	" OTIR"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH"},	{OP_UNEXIST,	"+DB   %02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH"},	{OP_UNEXIST,	"+DB   %02XH,%02XH"},
	{OP_NOTHING,	" LDDR"},				{OP_NOTHING,	" CPDR"},
	{OP_NOTHING,	" INDR"},				{OP_NOTHING,	" OTDR"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH"},	{OP_UNEXIST,	"+DB   %02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH"},	{OP_UNEXIST,	"+DB   %02XH,%02XH"},
	
	{OP_UNEXIST,	"+DB   %02XH,%02XH"},	{OP_UNEXIST,	"+DB   %02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH"},	{OP_UNEXIST,	"+DB   %02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH"},	{OP_UNEXIST,	"+DB   %02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH"},	{OP_UNEXIST,	"+DB   %02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH"},	{OP_UNEXIST,	"+DB   %02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH"},	{OP_UNEXIST,	"+DB   %02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH"},	{OP_UNEXIST,	"+DB   %02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH"},	{OP_UNEXIST,	"+DB   %02XH,%02XH"},
	
	{OP_UNEXIST,	"+DB   %02XH,%02XH"},	{OP_UNEXIST,	"+DB   %02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH"},	{OP_UNEXIST,	"+DB   %02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH"},	{OP_UNEXIST,	"+DB   %02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH"},	{OP_UNEXIST,	"+DB   %02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH"},	{OP_UNEXIST,	"+DB   %02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH"},	{OP_UNEXIST,	"+DB   %02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH"},	{OP_UNEXIST,	"+DB   %02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH"},	{OP_UNEXIST,	"+DB   %02XH,%02XH"},
	
	{OP_UNEXIST,	"+DB   %02XH,%02XH"},	{OP_UNEXIST,	"+DB   %02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH"},	{OP_UNEXIST,	"+DB   %02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH"},	{OP_UNEXIST,	"+DB   %02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH"},	{OP_UNEXIST,	"+DB   %02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH"},	{OP_UNEXIST,	"+DB   %02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH"},	{OP_UNEXIST,	"+DB   %02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH"},	{OP_UNEXIST,	"+DB   %02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH"},	{OP_UNEXIST,	"+DB   %02XH,%02XH"},
	
	{OP_UNEXIST,	"+DB   %02XH,%02XH"},	{OP_UNEXIST,	"+DB   %02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH"},	{OP_UNEXIST,	"+DB   %02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH"},	{OP_UNEXIST,	"+DB   %02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH"},	{OP_UNEXIST,	"+DB   %02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH"},	{OP_UNEXIST,	"+DB   %02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH"},	{OP_UNEXIST,	"+DB   %02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH"},	{OP_UNEXIST,	"+DB   %02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH"},	{OP_UNEXIST,	"+DB   %02XH,%02XH"},
};

static Mnemonics Instruction_DD[256]=
{
	{OP_SKIP,		"+DB   %02XH"},			{OP_SKIP,		"+DB   %02XH"},
	{OP_SKIP,		"+DB   %02XH"},			{OP_SKIP,		"+DB   %02XH"},
	{OP_SKIP,		"+DB   %02XH"},			{OP_SKIP,		"+DB   %02XH"},
	{OP_SKIP,		"+DB   %02XH"},			{OP_SKIP,		"+DB   %02XH"},
	{OP_SKIP,		"+DB   %02XH"},			{OP_NOTHING,	" ADD  IX,BC"},
	{OP_SKIP,		"+DB   %02XH"},			{OP_SKIP,		"+DB   %02XH"},
	{OP_SKIP,		"+DB   %02XH"},			{OP_SKIP,		"+DB   %02XH"},
	{OP_SKIP,		"+DB   %02XH"},			{OP_SKIP,		"+DB   %02XH"},
	
	{OP_SKIP,		"+DB   %02XH"},			{OP_SKIP,		"+DB   %02XH"},
	{OP_SKIP,		"+DB   %02XH"},			{OP_SKIP,		"+DB   %02XH"},
	{OP_SKIP,		"+DB   %02XH"},			{OP_SKIP,		"+DB   %02XH"},
	{OP_SKIP,		"+DB   %02XH"},			{OP_SKIP,		"+DB   %02XH"},
	{OP_SKIP,		"+DB   %02XH"},			{OP_NOTHING,	" ADD  IX,DE"},
	{OP_SKIP,		"+DB   %02XH"},			{OP_SKIP,		"+DB   %02XH"},
	{OP_SKIP,		"+DB   %02XH"},			{OP_SKIP,		"+DB   %02XH"},
	{OP_SKIP,		"+DB   %02XH"},			{OP_SKIP,		"+DB   %02XH"},
	
	{OP_SKIP,		"+DB   %02XH"},			{OP_NUM_16,		" LD   IX,%04XH"},
	{OP_NUM_16,		" LD   (%04XH),IX"},	{OP_NOTHING,	" INC  IX"},
	{OP_NOTHING,	"+INC  IXH"},			{OP_NOTHING,	"+DEC  IXH"},
	{OP_NUM_8,		"+LD   IXH,%02XH"},		{OP_SKIP,		"+DB   %02XH"},
	{OP_SKIP,		"+DB   %02XH"},			{OP_NOTHING,	" ADD  IX,IX"},
	{OP_NUM_16,		" LD   IX,(%04XH)"},	{OP_NOTHING,	" DEC  IX"},
	{OP_NOTHING,	"+INC  IXL"},			{OP_NOTHING,	"+DEC  IXL"},
	{OP_NUM_8,		"+LD   IXL,%02XH"},		{OP_SKIP,		"+DB   %02XH"},
	
	{OP_SKIP,		"+DB   %02XH"},			{OP_SKIP,		"+DB   %02XH"},
	{OP_SKIP,		"+DB   %02XH"},			{OP_SKIP,		"+DB   %02XH"},
	{OP_INDEX,		" INC  (IX%+d)"},		{OP_INDEX,		" DEC  (IX%+d)"},
	{OP_IDX_NUM,	" LD   (IX%+d),%02XH"},	{OP_SKIP,		"+DB   %02XH"},
	{OP_SKIP,		"+DB   %02XH"},			{OP_NOTHING,	" ADD  IX,SP"},
	{OP_SKIP,		"+DB   %02XH"},			{OP_SKIP,		"+DB   %02XH"},
	{OP_SKIP,		"+DB   %02XH"},			{OP_SKIP,		"+DB   %02XH"},
	{OP_SKIP,		"+DB   %02XH"},			{OP_SKIP,		"+DB   %02XH"},
	
	{OP_SKIP,		"+DB   %02XH"},			{OP_SKIP,		"+DB   %02XH"},
	{OP_SKIP,		"+DB   %02XH"},			{OP_SKIP,		"+DB   %02XH"},
	{OP_NOTHING,	"+LD   B,IXH"},			{OP_NOTHING,	"+LD   B,IXL"},
	{OP_INDEX,		" LD   B,(IX%+d)"},		{OP_SKIP,		"+DB   %02XH"},
	{OP_SKIP,		"+DB   %02XH"},			{OP_SKIP,		"+DB   %02XH"},
	{OP_SKIP,		"+DB   %02XH"},			{OP_SKIP,		"+DB   %02XH"},
	{OP_NOTHING,	"+LD   C,IXH"},			{OP_NOTHING,	"+LD   C,IXL"},
	{OP_INDEX,		" LD   C,(IX%+d)"},		{OP_SKIP,		"+DB   %02XH"},
	
	{OP_SKIP,		"+DB   %02XH"},			{OP_SKIP,		"+DB   %02XH"},
	{OP_SKIP,		"+DB   %02XH"},			{OP_SKIP,		"+DB   %02XH"},
	{OP_NOTHING,	"+LD   D,IXH"},			{OP_NOTHING,	"+LD   D,IXL"},
	{OP_INDEX,		" LD   D,(IX%+d)"},		{OP_SKIP,		"+DB   %02XH"},
	{OP_SKIP,		"+DB   %02XH"},			{OP_SKIP,		"+DB   %02XH"},
	{OP_SKIP,		"+DB   %02XH"},			{OP_SKIP,		"+DB   %02XH"},
	{OP_NOTHING,	"+LD   E,IXH"},			{OP_NOTHING,	"+LD   E,IXL"},
	{OP_INDEX,		" LD   E,(IX%+d)"},		{OP_SKIP,		"+DB   %02XH"},
	
	{OP_NOTHING,	"+LD   IXH,B"},			{OP_NOTHING,	"+LD   IXH,C"},
	{OP_NOTHING,	"+LD   IXH,D"},			{OP_NOTHING,	"+LD   IXH,E"},
	{OP_NOTHING,	"+LD   IXH,IXH"},		{OP_NOTHING,	"+LD   IXH,IXL"},
	{OP_INDEX,		" LD   H,(IX%+d)"},		{OP_NOTHING,	"+LD   IXH,A"},
	{OP_NOTHING,	"+LD   IXL,B"},			{OP_NOTHING,	"+LD   IXL,C"},
	{OP_NOTHING,	"+LD   IXL,D"},			{OP_NOTHING,	"+LD   IXL,E"},
	{OP_NOTHING,	"+LD   IXL,IXH"},		{OP_NOTHING,	"+LD   IXL,IXL"},
	{OP_INDEX,		" LD   L,(IX%+d)"},		{OP_NOTHING,	"+LD   IXL,A"},
	
	{OP_INDEX,		" LD   (IX%+d),B"},		{OP_INDEX,		" LD   (IX%+d),C"},
	{OP_INDEX,		" LD   (IX%+d),D"},		{OP_INDEX,		" LD   (IX%+d),E"},
	{OP_INDEX,		" LD   (IX%+d),H"},		{OP_INDEX,		" LD   (IX%+d),L"},
	{OP_SKIP,		"+DB   %02XH"},			{OP_INDEX,		" LD   (IX%+d),A"},
	{OP_SKIP,		"+DB   %02XH"},			{OP_SKIP,		"+DB   %02XH"},
	{OP_SKIP,		"+DB   %02XH"},			{OP_SKIP,		"+DB   %02XH"},
	{OP_NOTHING,	"+LD   A,IXH"},			{OP_NOTHING,	"+LD   A,IXL"},
	{OP_INDEX,		" LD   A,(IX%+d)"},		{OP_SKIP,		"+DB   %02XH"},
	
	{OP_SKIP,		"+DB   %02XH"},			{OP_SKIP,		"+DB   %02XH"},
	{OP_SKIP,		"+DB   %02XH"},			{OP_SKIP,		"+DB   %02XH"},
	{OP_NOTHING,	"+ADD  A,IXH"},			{OP_NOTHING,	"+ADD  A,IXL"},
	{OP_INDEX,		" ADD  A,(IX%+d)"},		{OP_SKIP,		"+DB   %02XH"},
	{OP_SKIP,		"+DB   %02XH"},			{OP_SKIP,		"+DB   %02XH"},
	{OP_SKIP,		"+DB   %02XH"},			{OP_SKIP,		"+DB   %02XH"},
	{OP_NOTHING,	"+ADC  A,IXH"},			{OP_NOTHING,	"+ADC  A,IXL"},
	{OP_INDEX,		" ADC  A,(IX%+d)"},		{OP_SKIP,		"+DB   %02XH"},
	
	{OP_SKIP,		"+DB   %02XH"},			{OP_SKIP,		"+DB   %02XH"},
	{OP_SKIP,		"+DB   %02XH"},			{OP_SKIP,		"+DB   %02XH"},
	{OP_NOTHING,	"+SUB  IXH"},			{OP_NOTHING,	"+SUB  IXL"},
	{OP_INDEX,		" SUB  (IX%+d)"},		{OP_SKIP,		"+DB   %02XH"},
	{OP_SKIP,		"+DB   %02XH"},			{OP_SKIP,		"+DB   %02XH"},
	{OP_SKIP,		"+DB   %02XH"},			{OP_SKIP,		"+DB   %02XH"},
	{OP_NOTHING,	"+SBC  A,IXH"},			{OP_NOTHING,	"+SBC  A,IXL"},
	{OP_INDEX,		" SBC  A,(IX%+d)"},		{OP_SKIP,		"+DB   %02XH"},
	
	{OP_SKIP,		"+DB   %02XH"},			{OP_SKIP,		"+DB   %02XH"},
	{OP_SKIP,		"+DB   %02XH"},			{OP_SKIP,		"+DB   %02XH"},
	{OP_NOTHING,	"+AND  IXH"},			{OP_NOTHING,	"+AND  IXL"},
	{OP_INDEX,		" AND  (IX%+d)"},		{OP_SKIP,		"+DB   %02XH"},
	{OP_SKIP,		"+DB   %02XH"},			{OP_SKIP,		"+DB   %02XH"},
	{OP_SKIP,		"+DB   %02XH"},			{OP_SKIP,		"+DB   %02XH"},
	{OP_NOTHING,	"+XOR  IXH"},			{OP_NOTHING,	"+XOR  IXL"},
	{OP_INDEX,		" XOR  (IX%+d)"},		{OP_SKIP,		"+DB   %02XH"},
	
	{OP_SKIP,		"+DB   %02XH"},			{OP_SKIP,		"+DB   %02XH"},
	{OP_SKIP,		"+DB   %02XH"},			{OP_SKIP,		"+DB   %02XH"},
	{OP_NOTHING,	"+OR   IXH"},			{OP_NOTHING,	"+OR   IXL"},
	{OP_INDEX,		" OR   (IX%+d)"},		{OP_SKIP,		"+DB   %02XH"},
	{OP_SKIP,		"+DB   %02XH"},			{OP_SKIP,		"+DB   %02XH"},
	{OP_SKIP,		"+DB   %02XH"},			{OP_SKIP,		"+DB   %02XH"},
	{OP_NOTHING,	"+CP   IXH"},			{OP_NOTHING,	"+CP   IXL"},
	{OP_INDEX,		" CP   (IX%+d)"},		{OP_SKIP,		"+DB   %02XH"},
	
	{OP_SKIP,		"+DB   %02XH"},		{OP_SKIP,		"+DB   %02XH"},
	{OP_SKIP,		"+DB   %02XH"},		{OP_SKIP,		"+DB   %02XH"},
	{OP_SKIP,		"+DB   %02XH"},		{OP_SKIP,		"+DB   %02XH"},
	{OP_SKIP,		"+DB   %02XH"},		{OP_SKIP,		"+DB   %02XH"},
	{OP_SKIP,		"+DB   %02XH"},		{OP_SKIP,		"+DB   %02XH"},
	{OP_SKIP,		"+DB   %02XH"},		{OP_PREFIX,		nullptr},
	{OP_SKIP,		"+DB   %02XH"},		{OP_SKIP,		"+DB   %02XH"},
	{OP_SKIP,		"+DB   %02XH"},		{OP_SKIP,		"+DB   %02XH"},
	
	{OP_SKIP,		"+DB   %02XH"},		{OP_SKIP,		"+DB   %02XH"},
	{OP_SKIP,		"+DB   %02XH"},		{OP_SKIP,		"+DB   %02XH"},
	{OP_SKIP,		"+DB   %02XH"},		{OP_SKIP,		"+DB   %02XH"},
	{OP_SKIP,		"+DB   %02XH"},		{OP_SKIP,		"+DB   %02XH"},
	{OP_SKIP,		"+DB   %02XH"},		{OP_SKIP,		"+DB   %02XH"},
	{OP_SKIP,		"+DB   %02XH"},		{OP_SKIP,		"+DB   %02XH"},
	{OP_SKIP,		"+DB   %02XH"},		{OP_SKIP,		"+DB   %02XH"},
	{OP_SKIP,		"+DB   %02XH"},		{OP_SKIP,		"+DB   %02XH"},
	
	{OP_SKIP,		"+DB   %02XH"},		{OP_NOTHING,	" POP  IX"},
	{OP_SKIP,		"+DB   %02XH"},		{OP_NOTHING,	" EX   IX,(SP)"},
	{OP_SKIP,		"+DB   %02XH"},		{OP_NOTHING,	" PUSH IX"},
	{OP_SKIP,		"+DB   %02XH"},		{OP_SKIP,		"+DB   %02XH"},
	{OP_SKIP,		"+DB   %02XH"},		{OP_NOTHING,	" JP   (IX)"},
	{OP_SKIP,		"+DB   %02XH"},		{OP_SKIP,		"+DB   %02XH"},
	{OP_SKIP,		"+DB   %02XH"},		{OP_SKIP,		"+DB   %02XH"},
	{OP_SKIP,		"+DB   %02XH"},		{OP_SKIP,		"+DB   %02XH"},
	
	{OP_SKIP,		"+DB   %02XH"},		{OP_SKIP,		"+DB   %02XH"},
	{OP_SKIP,		"+DB   %02XH"},		{OP_SKIP,		"+DB   %02XH"},
	{OP_SKIP,		"+DB   %02XH"},		{OP_SKIP,		"+DB   %02XH"},
	{OP_SKIP,		"+DB   %02XH"},		{OP_SKIP,		"+DB   %02XH"},
	{OP_SKIP,		"+DB   %02XH"},		{OP_NOTHING,	" LD   SP,IX"},
	{OP_SKIP,		"+DB   %02XH"},		{OP_SKIP,		"+DB   %02XH"},
	{OP_SKIP,		"+DB   %02XH"},		{OP_SKIP,		"+DB   %02XH"},
	{OP_SKIP,		"+DB   %02XH"},		{OP_SKIP,		"+DB   %02XH"},
};

static Mnemonics Instruction_DD_CB[256]=
{
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_INDEX,		" RLC  (IX%+d)"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_INDEX,		" RRC  (IX%+d)"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},

	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_INDEX,		" RL   (IX%+d)"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_INDEX,		" RR   (IX%+d)"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},

	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_INDEX,		" SLA  (IX%+d)"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_INDEX,		" SRA  (IX%+d)"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},

	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_INDEX,		"+SLL  (IX%+d)"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_INDEX,		" SRL   (IX%+d)"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},

	{OP_INDEX,		"+BIT  0,(IX%+d)"},
	{OP_INDEX,		"+BIT  0,(IX%+d)"},
	{OP_INDEX,		"+BIT  0,(IX%+d)"},
	{OP_INDEX,		"+BIT  0,(IX%+d)"},
	{OP_INDEX,		"+BIT  0,(IX%+d)"},
	{OP_INDEX,		"+BIT  0,(IX%+d)"},
	{OP_INDEX,		" BIT  0,(IX%+d)"},
	{OP_INDEX,		"+BIT  0,(IX%+d)"},
	{OP_INDEX,		"+BIT  1,(IX%+d)"},
	{OP_INDEX,		"+BIT  1,(IX%+d)"},
	{OP_INDEX,		"+BIT  1,(IX%+d)"},
	{OP_INDEX,		"+BIT  1,(IX%+d)"},
	{OP_INDEX,		"+BIT  1,(IX%+d)"},
	{OP_INDEX,		"+BIT  1,(IX%+d)"},
	{OP_INDEX,		" BIT  1,(IX%+d)"},
	{OP_INDEX,		"+BIT  1,(IX%+d)"},

	{OP_INDEX,		"+BIT  2,(IX%+d)"},
	{OP_INDEX,		"+BIT  2,(IX%+d)"},
	{OP_INDEX,		"+BIT  2,(IX%+d)"},
	{OP_INDEX,		"+BIT  2,(IX%+d)"},
	{OP_INDEX,		"+BIT  2,(IX%+d)"},
	{OP_INDEX,		"+BIT  2,(IX%+d)"},
	{OP_INDEX,		" BIT  2,(IX%+d)"},
	{OP_INDEX,		"+BIT  2,(IX%+d)"},
	{OP_INDEX,		"+BIT  3,(IX%+d)"},
	{OP_INDEX,		"+BIT  3,(IX%+d)"},
	{OP_INDEX,		"+BIT  3,(IX%+d)"},
	{OP_INDEX,		"+BIT  3,(IX%+d)"},
	{OP_INDEX,		"+BIT  3,(IX%+d)"},
	{OP_INDEX,		"+BIT  3,(IX%+d)"},
	{OP_INDEX,		" BIT  3,(IX%+d)"},
	{OP_INDEX,		"+BIT  3,(IX%+d)"},

	{OP_INDEX,		"+BIT  4,(IX%+d)"},
	{OP_INDEX,		"+BIT  4,(IX%+d)"},
	{OP_INDEX,		"+BIT  4,(IX%+d)"},
	{OP_INDEX,		"+BIT  4,(IX%+d)"},
	{OP_INDEX,		"+BIT  4,(IX%+d)"},
	{OP_INDEX,		"+BIT  4,(IX%+d)"},
	{OP_INDEX,		" BIT  4,(IX%+d)"},
	{OP_INDEX,		"+BIT  4,(IX%+d)"},
	{OP_INDEX,		"+BIT  5,(IX%+d)"},
	{OP_INDEX,		"+BIT  5,(IX%+d)"},
	{OP_INDEX,		"+BIT  5,(IX%+d)"},
	{OP_INDEX,		"+BIT  5,(IX%+d)"},
	{OP_INDEX,		"+BIT  5,(IX%+d)"},
	{OP_INDEX,		"+BIT  5,(IX%+d)"},
	{OP_INDEX,		" BIT  5,(IX%+d)"},
	{OP_INDEX,		"+BIT  5,(IX%+d)"},

	{OP_INDEX,		"+BIT  6,(IX%+d)"},
	{OP_INDEX,		"+BIT  6,(IX%+d)"},
	{OP_INDEX,		"+BIT  6,(IX%+d)"},
	{OP_INDEX,		"+BIT  6,(IX%+d)"},
	{OP_INDEX,		"+BIT  6,(IX%+d)"},
	{OP_INDEX,		"+BIT  6,(IX%+d)"},
	{OP_INDEX,		" BIT  6,(IX%+d)"},
	{OP_INDEX,		"+BIT  6,(IX%+d)"},
	{OP_INDEX,		"+BIT  7,(IX%+d)"},
	{OP_INDEX,		"+BIT  7,(IX%+d)"},
	{OP_INDEX,		"+BIT  7,(IX%+d)"},
	{OP_INDEX,		"+BIT  7,(IX%+d)"},
	{OP_INDEX,		"+BIT  7,(IX%+d)"},
	{OP_INDEX,		"+BIT  7,(IX%+d)"},
	{OP_INDEX,		" BIT  7,(IX%+d)"},
	{OP_INDEX,		"+BIT  7,(IX%+d)"},

	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_INDEX,		" RES  0,(IX%+d)"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_INDEX,		" RES  1,(IX%+d)"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},

	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_INDEX,		" RES  2,(IX%+d)"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_INDEX,		" RES  3,(IX%+d)"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},

	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_INDEX,		" RES  4,(IX%+d)"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_INDEX,		" RES  5,(IX%+d)"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},

	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_INDEX,		" RES  6,(IX%+d)"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_INDEX,		" RES  7,(IX%+d)"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},

	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_INDEX,		" SET  0,(IX%+d)"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_INDEX,		" SET  1,(IX%+d)"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},

	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_INDEX,		" SET  2,(IX%+d)"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_INDEX,		" SET  3,(IX%+d)"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},

	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_INDEX,		" SET  4,(IX%+d)"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_INDEX,		" SET  5,(IX%+d)"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},

	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_INDEX,		" SET  6,(IX%+d)"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_INDEX,		" SET  7,(IX%+d)"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
};

static Mnemonics Instruction_FD[256]=
{
	{OP_SKIP,		"+DB   %02XH"},		{OP_SKIP,		"+DB   %02XH"},
	{OP_SKIP,		"+DB   %02XH"},		{OP_SKIP,		"+DB   %02XH"},
	{OP_SKIP,		"+DB   %02XH"},		{OP_SKIP,		"+DB   %02XH"},
	{OP_SKIP,		"+DB   %02XH"},		{OP_SKIP,		"+DB   %02XH"},
	{OP_SKIP,		"+DB   %02XH"},		{OP_NOTHING,	" ADD  IY,BC"},
	{OP_SKIP,		"+DB   %02XH"},		{OP_SKIP,		"+DB   %02XH"},
	{OP_SKIP,		"+DB   %02XH"},		{OP_SKIP,		"+DB   %02XH"},
	{OP_SKIP,		"+DB   %02XH"},		{OP_SKIP,		"+DB   %02XH"},
	
	{OP_SKIP,		"+DB   %02XH"},		{OP_SKIP,		"+DB   %02XH"},
	{OP_SKIP,		"+DB   %02XH"},		{OP_SKIP,		"+DB   %02XH"},
	{OP_SKIP,		"+DB   %02XH"},		{OP_SKIP,		"+DB   %02XH"},
	{OP_SKIP,		"+DB   %02XH"},		{OP_SKIP,		"+DB   %02XH"},
	{OP_SKIP,		"+DB   %02XH"},		{OP_NOTHING,	" ADD  IY,DE"},
	{OP_SKIP,		"+DB   %02XH"},		{OP_SKIP,		"+DB   %02XH"},
	{OP_SKIP,		"+DB   %02XH"},		{OP_SKIP,		"+DB   %02XH"},
	{OP_SKIP,		"+DB   %02XH"},		{OP_SKIP,		"+DB   %02XH"},
	
	{OP_SKIP,		"+DB   %02XH"},			{OP_NUM_16,		" LD   IY,%04XH"},
	{OP_NUM_16,		" LD   (%04XH),IY"},	{OP_NOTHING,	" INC  IY"},
	{OP_NOTHING,	"+INC  IYH"},			{OP_NOTHING,	"+DEC  IYH"},
	{OP_NUM_8,		"+LD   IYH,%02XH"},		{OP_SKIP,		"+DB   %02XH"},
	{OP_SKIP,		"+DB   %02XH"},			{OP_NOTHING,	" ADD  IY,IY"},
	{OP_NUM_16,		" LD   IY,(%04XH)"},	{OP_NOTHING,	" DEC  IY"},
	{OP_NOTHING,	"+INC  IYL"},			{OP_NOTHING,	"+DEC  IYL"},
	{OP_NUM_8,		"+LD   IYL,%02XH"},		{OP_SKIP,		"+DB   %02XH"},
	
	{OP_SKIP,		"+DB   %02XH"},			{OP_SKIP,		"+DB   %02XH"},
	{OP_SKIP,		"+DB   %02XH"},			{OP_SKIP,		"+DB   %02XH"},
	{OP_INDEX,		" INC  (IY%+d)"},		{OP_INDEX,		" DEC  (IY%+d)"},
	{OP_IDX_NUM,	" LD   (IY%+d),%02XH"},	{OP_SKIP,		"+DB   %02XH"},
	{OP_SKIP,		"+DB   %02XH"},			{OP_NOTHING,	" ADD  IY,SP"},
	{OP_SKIP,		"+DB   %02XH"},			{OP_SKIP,		"+DB   %02XH"},
	{OP_SKIP,		"+DB   %02XH"},			{OP_SKIP,		"+DB   %02XH"},
	{OP_SKIP,		"+DB   %02XH"},			{OP_SKIP,		"+DB   %02XH"},
	
	{OP_SKIP,		"+DB   %02XH"},			{OP_SKIP,		"+DB   %02XH"},
	{OP_SKIP,		"+DB   %02XH"},			{OP_SKIP,		"+DB   %02XH"},
	{OP_NOTHING,	"+LD   B,IYH"},			{OP_NOTHING,	"+LD   B,IYL"},
	{OP_INDEX,		" LD   B,(IY%+d)"},		{OP_SKIP,		"+DB   %02XH"},
	{OP_SKIP,		"+DB   %02XH"},			{OP_SKIP,		"+DB   %02XH"},
	{OP_SKIP,		"+DB   %02XH"},			{OP_SKIP,		"+DB   %02XH"},
	{OP_NOTHING,	"+LD   C,IYH"},			{OP_NOTHING,	"+LD   C,IYL"},
	{OP_INDEX,		" LD   C,(IY%+d)"},		{OP_SKIP,		"+DB   %02XH"},
	
	{OP_SKIP,		"+DB   %02XH"},		{OP_SKIP,		"+DB   %02XH"},
	{OP_SKIP,		"+DB   %02XH"},		{OP_SKIP,		"+DB   %02XH"},
	{OP_NOTHING,	"+LD   D,IYH"},		{OP_NOTHING,	"+LD   D,IYL"},
	{OP_INDEX,		" LD   D,(IY%+d)"},	{OP_SKIP,		"+DB   %02XH"},
	{OP_SKIP,		"+DB   %02XH"},		{OP_SKIP,		"+DB   %02XH"},
	{OP_SKIP,		"+DB   %02XH"},		{OP_SKIP,		"+DB   %02XH"},
	{OP_NOTHING,	"+LD   E,IYH"},		{OP_NOTHING,	"+LD   E,IYL"},
	{OP_INDEX,		" LD   E,(IY%+d)"},	{OP_SKIP,		"+DB   %02XH"},
	
	{OP_NOTHING,	"+LD   IYH,B"},		{OP_NOTHING,	"+LD   IYH,C"},
	{OP_NOTHING,	"+LD   IYH,D"},		{OP_NOTHING,	"+LD   IYH,E"},
	{OP_NOTHING,	"+LD   IYH,IYH"},	{OP_NOTHING,	"+LD   IYH,IYL"},
	{OP_INDEX,		" LD   H,(IY%+d)"},	{OP_NOTHING,	"+LD   IYH,A"},
	{OP_NOTHING,	"+LD   IYL,B"},		{OP_NOTHING,	"+LD   IYL,C"},
	{OP_NOTHING,	"+LD   IYL,D"},		{OP_NOTHING,	"+LD   IYL,E"},
	{OP_NOTHING,	"+LD   IYL,IYH"},	{OP_NOTHING,	"+LD   IYL,IYL"},
	{OP_INDEX,		" LD   L,(IY%+d)"},	{OP_NOTHING,	"+LD   IYL,A"},
	
	{OP_INDEX,		" LD   (IY%+d),B"},	{OP_INDEX,		" LD   (IY%+d),C"},
	{OP_INDEX,		" LD   (IY%+d),D"},	{OP_INDEX,		" LD   (IY%+d),E"},
	{OP_INDEX,		" LD   (IY%+d),H"},	{OP_INDEX,		" LD   (IY%+d),L"},
	{OP_SKIP,		"+DB   %02XH"},		{OP_INDEX,		" LD   (IY%+d),A"},
	{OP_SKIP,		"+DB   %02XH"},		{OP_SKIP,		"+DB   %02XH"},
	{OP_SKIP,		"+DB   %02XH"},		{OP_SKIP,		"+DB   %02XH"},
	{OP_NOTHING,	"+LD   A,IYH"},		{OP_NOTHING,	"+LD   A,IYL"},
	{OP_INDEX,		" LD   A,(IY%+d)"},	{OP_SKIP,		"+DB   %02XH"},
	
	{OP_SKIP,		"+DB   %02XH"},		{OP_SKIP,		"+DB   %02XH"},
	{OP_SKIP,		"+DB   %02XH"},		{OP_SKIP,		"+DB   %02XH"},
	{OP_NOTHING,	"+ADD  A,IYH"},		{OP_NOTHING,	"+ADD  A,IYL"},
	{OP_INDEX,		" ADD  A,(IY%+d)"},	{OP_SKIP,		"+DB   %02XH"},
	{OP_SKIP,		"+DB   %02XH"},		{OP_SKIP,		"+DB   %02XH"},
	{OP_SKIP,		"+DB   %02XH"},		{OP_SKIP,		"+DB   %02XH"},
	{OP_NOTHING,	"+ADC  A,IYH"},		{OP_NOTHING,	"+ADC  A,IYL"},
	{OP_INDEX,		" ADC  A,(IY%+d)"},	{OP_SKIP,		"+DB   %02XH"},
	
	{OP_SKIP,		"+DB   %02XH"},		{OP_SKIP,		"+DB   %02XH"},
	{OP_SKIP,		"+DB   %02XH"},		{OP_SKIP,		"+DB   %02XH"},
	{OP_NOTHING,	"+SUB  IYH"},		{OP_NOTHING,	"+SUB  IYL"},
	{OP_INDEX,		" SUB  (IY%+d)"},	{OP_SKIP,		"+DB   %02XH"},
	{OP_SKIP,		"+DB   %02XH"},		{OP_SKIP,		"+DB   %02XH"},
	{OP_SKIP,		"+DB   %02XH"},		{OP_SKIP,		"+DB   %02XH"},
	{OP_NOTHING,	"+SBC  A,IYH"},		{OP_NOTHING,	"+SBC  A,IYL"},
	{OP_INDEX,		" SBC  A,(IY%+d)"},	{OP_SKIP,		"+DB   %02XH"},
	
	{OP_SKIP,		"+DB   %02XH"},		{OP_SKIP,		"+DB   %02XH"},
	{OP_SKIP,		"+DB   %02XH"},		{OP_SKIP,		"+DB   %02XH"},
	{OP_NOTHING,	"+AND  IYH"},		{OP_NOTHING,	"+AND  IYL"},
	{OP_INDEX,		" AND  (IY%+d)"},	{OP_SKIP,		"+DB   %02XH"},
	{OP_SKIP,		"+DB   %02XH"},		{OP_SKIP,		"+DB   %02XH"},
	{OP_SKIP,		"+DB   %02XH"},		{OP_SKIP,		"+DB   %02XH"},
	{OP_NOTHING,	"+XOR  IYH"},		{OP_NOTHING,	"+XOR  IYL"},
	{OP_INDEX,		" XOR  (IY%+d)"},	{OP_SKIP,		"+DB   %02XH"},
	
	{OP_SKIP,		"+DB   %02XH"},		{OP_SKIP,		"+DB   %02XH"},
	{OP_SKIP,		"+DB   %02XH"},		{OP_SKIP,		"+DB   %02XH"},
	{OP_NOTHING,	"+OR   IYH"},		{OP_NOTHING,	"+OR   IYL"},
	{OP_INDEX,		" OR   (IY%+d)"},	{OP_SKIP,		"+DB   %02XH"},
	{OP_SKIP,		"+DB   %02XH"},		{OP_SKIP,		"+DB   %02XH"},
	{OP_SKIP,		"+DB   %02XH"},		{OP_SKIP,		"+DB   %02XH"},
	{OP_NOTHING,	"+CP   IYH"},		{OP_NOTHING,	"+CP   IYL"},
	{OP_INDEX,		" CP   (IY%+d)"},	{OP_SKIP,		"+DB   %02XH"},
	
	{OP_SKIP,		"+DB   %02XH"},		{OP_SKIP,		"+DB   %02XH"},
	{OP_SKIP,		"+DB   %02XH"},		{OP_SKIP,		"+DB   %02XH"},
	{OP_SKIP,		"+DB   %02XH"},		{OP_SKIP,		"+DB   %02XH"},
	{OP_SKIP,		"+DB   %02XH"},		{OP_SKIP,		"+DB   %02XH"},
	{OP_SKIP,		"+DB   %02XH"},		{OP_SKIP,		"+DB   %02XH"},
	{OP_SKIP,		"+DB   %02XH"},		{OP_PREFIX,		nullptr},
	{OP_SKIP,		"+DB   %02XH"},		{OP_SKIP,		"+DB   %02XH"},
	{OP_SKIP,		"+DB   %02XH"},		{OP_SKIP,		"+DB   %02XH"},
	
	{OP_SKIP,		"+DB   %02XH"},		{OP_SKIP,		"+DB   %02XH"},
	{OP_SKIP,		"+DB   %02XH"},		{OP_SKIP,		"+DB   %02XH"},
	{OP_SKIP,		"+DB   %02XH"},		{OP_SKIP,		"+DB   %02XH"},
	{OP_SKIP,		"+DB   %02XH"},		{OP_SKIP,		"+DB   %02XH"},
	{OP_SKIP,		"+DB   %02XH"},		{OP_SKIP,		"+DB   %02XH"},
	{OP_SKIP,		"+DB   %02XH"},		{OP_SKIP,		"+DB   %02XH"},
	{OP_SKIP,		"+DB   %02XH"},		{OP_SKIP,		"+DB   %02XH"},
	{OP_SKIP,		"+DB   %02XH"},		{OP_SKIP,		"+DB   %02XH"},
	
	{OP_SKIP,		"+DB   %02XH"},		{OP_NOTHING,	" POP  IY"},
	{OP_SKIP,		"+DB   %02XH"},		{OP_NOTHING,	" EX   IY,(SP)"},
	{OP_SKIP,		"+DB   %02XH"},		{OP_NOTHING,	" PUSH IY"},
	{OP_SKIP,		"+DB   %02XH"},		{OP_SKIP,		"+DB   %02XH"},
	{OP_SKIP,		"+DB   %02XH"},		{OP_NOTHING,	" JP   (IY)"},
	{OP_SKIP,		"+DB   %02XH"},		{OP_SKIP,		"+DB   %02XH"},
	{OP_SKIP,		"+DB   %02XH"},		{OP_SKIP,		"+DB   %02XH"},
	{OP_SKIP,		"+DB   %02XH"},		{OP_SKIP,		"+DB   %02XH"},
	
	{OP_SKIP,		"+DB   %02XH"},		{OP_SKIP,		"+DB   %02XH"},
	{OP_SKIP,		"+DB   %02XH"},		{OP_SKIP,		"+DB   %02XH"},
	{OP_SKIP,		"+DB   %02XH"},		{OP_SKIP,		"+DB   %02XH"},
	{OP_SKIP,		"+DB   %02XH"},		{OP_SKIP,		"+DB   %02XH"},
	{OP_SKIP,		"+DB   %02XH"},		{OP_NOTHING,	" LD   SP,IY"},
	{OP_SKIP,		"+DB   %02XH"},		{OP_SKIP,		"+DB   %02XH"},
	{OP_SKIP,		"+DB   %02XH"},		{OP_SKIP,		"+DB   %02XH"},
	{OP_SKIP,		"+DB   %02XH"},		{OP_SKIP,		"+DB   %02XH"},
};

static Mnemonics Instruction_FD_CB[256]=
{
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_INDEX,		" RLC  (IY%+d)"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_INDEX,		" RRC  (IY%+d)"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},

	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_INDEX,		" RL   (IY%+d)"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_INDEX,		" RR   (IY%+d)"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},

	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_INDEX,		" SLA  (IY%+d)"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_INDEX,		" SRA  (IY%+d)"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},

	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_INDEX,		"+SLL  (IY%+d)"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_INDEX,		" SRL   (IY%+d)"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},

	{OP_INDEX,		"+BIT  0,(IY%+d)"},
	{OP_INDEX,		"+BIT  0,(IY%+d)"},
	{OP_INDEX,		"+BIT  0,(IY%+d)"},
	{OP_INDEX,		"+BIT  0,(IY%+d)"},
	{OP_INDEX,		"+BIT  0,(IY%+d)"},
	{OP_INDEX,		"+BIT  0,(IY%+d)"},
	{OP_INDEX,		" BIT  0,(IY%+d)"},
	{OP_INDEX,		"+BIT  0,(IY%+d)"},
	{OP_INDEX,		"+BIT  1,(IY%+d)"},
	{OP_INDEX,		"+BIT  1,(IY%+d)"},
	{OP_INDEX,		"+BIT  1,(IY%+d)"},
	{OP_INDEX,		"+BIT  1,(IY%+d)"},
	{OP_INDEX,		"+BIT  1,(IY%+d)"},
	{OP_INDEX,		"+BIT  1,(IY%+d)"},
	{OP_INDEX,		" BIT  1,(IY%+d)"},
	{OP_INDEX,		"+BIT  1,(IY%+d)"},

	{OP_INDEX,		"+BIT  2,(IY%+d)"},
	{OP_INDEX,		"+BIT  2,(IY%+d)"},
	{OP_INDEX,		"+BIT  2,(IY%+d)"},
	{OP_INDEX,		"+BIT  2,(IY%+d)"},
	{OP_INDEX,		"+BIT  2,(IY%+d)"},
	{OP_INDEX,		"+BIT  2,(IY%+d)"},
	{OP_INDEX,		" BIT  2,(IY%+d)"},
	{OP_INDEX,		"+BIT  2,(IY%+d)"},
	{OP_INDEX,		"+BIT  3,(IY%+d)"},
	{OP_INDEX,		"+BIT  3,(IY%+d)"},
	{OP_INDEX,		"+BIT  3,(IY%+d)"},
	{OP_INDEX,		"+BIT  3,(IY%+d)"},
	{OP_INDEX,		"+BIT  3,(IY%+d)"},
	{OP_INDEX,		"+BIT  3,(IY%+d)"},
	{OP_INDEX,		" BIT  3,(IY%+d)"},
	{OP_INDEX,		"+BIT  3,(IY%+d)"},

	{OP_INDEX,		"+BIT  4,(IY%+d)"},
	{OP_INDEX,		"+BIT  4,(IY%+d)"},
	{OP_INDEX,		"+BIT  4,(IY%+d)"},
	{OP_INDEX,		"+BIT  4,(IY%+d)"},
	{OP_INDEX,		"+BIT  4,(IY%+d)"},
	{OP_INDEX,		"+BIT  4,(IY%+d)"},
	{OP_INDEX,		" BIT  4,(IY%+d)"},
	{OP_INDEX,		"+BIT  4,(IY%+d)"},
	{OP_INDEX,		"+BIT  5,(IY%+d)"},
	{OP_INDEX,		"+BIT  5,(IY%+d)"},
	{OP_INDEX,		"+BIT  5,(IY%+d)"},
	{OP_INDEX,		"+BIT  5,(IY%+d)"},
	{OP_INDEX,		"+BIT  5,(IY%+d)"},
	{OP_INDEX,		"+BIT  5,(IY%+d)"},
	{OP_INDEX,		" BIT  5,(IY%+d)"},
	{OP_INDEX,		"+BIT  5,(IY%+d)"},

	{OP_INDEX,		"+BIT  6,(IY%+d)"},
	{OP_INDEX,		"+BIT  6,(IY%+d)"},
	{OP_INDEX,		"+BIT  6,(IY%+d)"},
	{OP_INDEX,		"+BIT  6,(IY%+d)"},
	{OP_INDEX,		"+BIT  6,(IY%+d)"},
	{OP_INDEX,		"+BIT  6,(IY%+d)"},
	{OP_INDEX,		" BIT  6,(IY%+d)"},
	{OP_INDEX,		"+BIT  6,(IY%+d)"},
	{OP_INDEX,		"+BIT  7,(IY%+d)"},
	{OP_INDEX,		"+BIT  7,(IY%+d)"},
	{OP_INDEX,		"+BIT  7,(IY%+d)"},
	{OP_INDEX,		"+BIT  7,(IY%+d)"},
	{OP_INDEX,		"+BIT  7,(IY%+d)"},
	{OP_INDEX,		"+BIT  7,(IY%+d)"},
	{OP_INDEX,		" BIT  7,(IY%+d)"},
	{OP_INDEX,		"+BIT  7,(IY%+d)"},

	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_INDEX,		" RES  0,(IY%+d)"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_INDEX,		" RES  1,(IY%+d)"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},

	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_INDEX,		" RES  2,(IY%+d)"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_INDEX,		" RES  3,(IY%+d)"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},

	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_INDEX,		" RES  4,(IY%+d)"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_INDEX,		" RES  5,(IY%+d)"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},

	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_INDEX,		" RES  6,(IY%+d)"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_INDEX,		" RES  7,(IY%+d)"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},

	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_INDEX,		" SET  0,(IY%+d)"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_INDEX,		" SET  1,(IY%+d)"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},

	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_INDEX,		" SET  2,(IY%+d)"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_INDEX,		" SET  3,(IY%+d)"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},

	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_INDEX,		" SET  4,(IY%+d)"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_INDEX,		" SET  5,(IY%+d)"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},

	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_INDEX,		" SET  6,(IY%+d)"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
	{OP_INDEX,		" SET  7,(IY%+d)"},
	{OP_UNEXIST,	"+DB   %02XH,%02XH,%02XH,%02XH"},
};



// アドレス／インストラクションコードの表示
void cZ80::PrintfHead( std::string& str, WORD pc, int num )
{
	str = Stringf( "%04X:", pc );
	
	switch( num ){
	case 1:
		str += Stringf( "%02X       ",       ReadMemNW( pc ) );
		break;
	case 2:
		str += Stringf( "%02X%02X     ",     ReadMemNW( pc ), ReadMemNW( pc+1 ) );
		break;
	case 3:
		str += Stringf( "%02X%02X%02X   ",   ReadMemNW( pc ), ReadMemNW( pc+1 ), ReadMemNW( pc+2 ) );
		break;
	case 4:
		str += Stringf( "%02X%02X%02X%02X ", ReadMemNW( pc ), ReadMemNW( pc+1 ), ReadMemNW( pc+2 ), ReadMemNW( pc+3 ) );
		break;
	default:
		str += Stringf( "Internal Error -" );
		break;
	}
}


/////////////////////////////////////////////////////////////////////////////
// 1ライン逆アセンブル
/////////////////////////////////////////////////////////////////////////////
int cZ80::Disasm( std::string& str, WORD pc )
{
	int num;
	Mnemonics* Inst;
	
	Inst = &Instruction[ ReadMemNW( pc ) ];
	switch( Inst->Type ){
	case OP_NOTHING:
		PrintfHead( str, pc, 1 );
		str += Inst->Str;
		return 1;
		
	case OP_NUM_8:
		num = ReadMemNW( pc+1 );
		PrintfHead( str, pc, 2 );
		str += Stringf( Inst->Str, num );
		return 2;
		
	case OP_NUM_16:
		num = ReadMemNW( pc+1 ) + ReadMemNW( pc+2 )*256;
		PrintfHead( str, pc, 3 );
		str += Stringf( Inst->Str, num );
		return 3;
		
	case OP_ADR_REL:
		num = pc +2 + (offset)ReadMemNW( pc+1 );
		PrintfHead( str, pc, 2 );
		str += Stringf( Inst->Str, num );
		return 2;
		
	case OP_PREFIX:
		switch( ReadMemNW( pc ) ){
		case 0xCB:	Inst = &Instruction_CB[ ReadMemNW( pc+1 ) ];	break;
		case 0xDD:	Inst = &Instruction_DD[ ReadMemNW( pc+1 ) ];	break;
		case 0xED:	Inst = &Instruction_ED[ ReadMemNW( pc+1 ) ];	break;
		case 0xFD:	Inst = &Instruction_FD[ ReadMemNW( pc+1 ) ];	break;
		default:
			str += " - Internal Error ! - ";
			return 2;
		}
		
		switch( Inst->Type ){
		case OP_NOTHING:
			PrintfHead( str, pc, 2 );
			str += Inst->Str;
			return 2;
			
		case OP_UNEXIST:
			PrintfHead( str, pc, 2 );
			str += Stringf( Inst->Str, ReadMemNW( pc ), ReadMemNW( pc+1 ) );
			return 2;
			
		case OP_SKIP:
			PrintfHead( str, pc, 1 );
			str += Stringf( Inst->Str, ReadMemNW( pc ) );
			return 1;
			
		case OP_NUM_8:
			num = ReadMemNW( pc+2 );
			PrintfHead( str, pc, 3 );
			str += Stringf( Inst->Str, num );
			return 3;
			
		case OP_NUM_16:
			num = ReadMemNW( pc+2 ) + ReadMemNW( pc+3 )*256;
			PrintfHead( str, pc, 4 );
			str += Stringf( Inst->Str, num );
			return 4;
		
		case OP_INDEX:
			PrintfHead( str, pc, 3 );
			str += Stringf( Inst->Str, (offset)ReadMemNW( pc+2 ) );
			return 3;
		
		case OP_IDX_NUM:
			num = (int)ReadMemNW( pc+3 );
			PrintfHead( str, pc, 4 );
			str += Stringf( Inst->Str, (int)ReadMemNW( pc+2 ), num );
			return 4;
			
		case OP_PREFIX:
			switch( ReadMemNW( pc ) ){
			case 0xDD:   Inst = &Instruction_DD_CB[ ReadMemNW( pc+3 ) ];	break;
			case 0xFD:   Inst = &Instruction_FD_CB[ ReadMemNW( pc+3 ) ];	break;
			default:
				str += " - Internal Error - ";
				return 4;
			}
			switch( Inst->Type ){
			case OP_INDEX:
				PrintfHead( str, pc, 4 );
				str += Stringf( Inst->Str, (offset)ReadMemNW( pc+2 ) );
				return 4;
				
			case OP_UNEXIST:
				PrintfHead( str, pc, 4 );
				str += Stringf( Inst->Str, ReadMemNW( pc ),  ReadMemNW( pc+1 ), ReadMemNW( pc+2 ),ReadMemNW( pc+3 ));
				return 4;
			}
			break;
		}
		break;
	}
	
	str += " - Internal Error - ";
	return 1;
}


/////////////////////////////////////////////////////////////////////////////
// レジスタ値取得
/////////////////////////////////////////////////////////////////////////////
void cZ80::GetRegister( Register& reg )
{
	// 汎用レジスタ
	reg.AF = AF;
	reg.BC = BC;
	reg.DE = DE;
	reg.HL = HL;
	
	// 専用レジスタ
	reg.IX = IX;
	reg.IY = IY;
	reg.PC = PC;
	reg.SP = SP;
	
	// 裏レジスタ
	reg.AF1 = AF1;
	reg.BC1 = BC1;
	reg.DE1 = DE1;
	reg.HL1 = HL1;
	
	// 特殊レジスタ
	reg.I = I;
	reg.R = R;
	
	// R reg 保存用
	reg.R_saved = R_saved;
	
	// IFF,IFF2
	reg.IFF  = IFF;
	reg.IFF2 = IFF2;
	
	// 割込モード
	reg.IM = IM;
	
	// HALT フラグ
	reg.Halt = Halt;
	
	// 割込みベクタ
	reg.IntVec = IntVec;
}


/////////////////////////////////////////////////////////////////////////////
// レジスタ値設定
/////////////////////////////////////////////////////////////////////////////
void cZ80::SetRegister( Register& reg )
{
	// 汎用レジスタ
	AF = reg.AF;
	BC = reg.BC;
	DE = reg.DE;
	HL = reg.HL;
	
	// 専用レジスタ
	IX = reg.IX;
	IY = reg.IY;
	PC = reg.PC;
	SP = reg.SP;
	
	// 裏レジスタ
	AF1 = reg.AF1;
	BC1 = reg.BC1;
	DE1 = reg.DE1;
	HL1 = reg.HL1;
	
	// 特殊レジスタ
	I = reg.I;
	R = reg.R;
	
	// R reg 保存用
	R_saved = reg.R_saved;
	
	// IFF,IFF2
	IFF  = reg.IFF;
	IFF2 = reg.IFF2;
	
	// 割込モード
	IM = reg.IM;
	
	// HALT フラグ
	Halt = reg.Halt;
	
	// 割込みベクタ
	IntVec = reg.IntVec;
}


/////////////////////////////////////////////////////////////////////////////
// PCレジスタ値取得
/////////////////////////////////////////////////////////////////////////////
WORD cZ80::GetPC( void )
{
	return PC.W;
}


/////////////////////////////////////////////////////////////////////////////
// 割込みベクタ取得
/////////////////////////////////////////////////////////////////////////////
BYTE cZ80::GetIntVec( void )
{
	return IntVec;
}

#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
