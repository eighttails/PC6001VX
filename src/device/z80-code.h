/////////////////////////////////////////////////////////////////////////////
//  P C 6 0 0 1 V
//  Copyright 1999,2022 Yumitaro
/////////////////////////////////////////////////////////////////////////////
//***************************************************************************
// QUASI88 --- PC-8801 emulator
//	 Copyright (C) Showzoh Fukunaga 1998
//***************************************************************************
// 8ビット転送命令
	case LD_A_A:	ACC=ACC;	break;
	case LD_A_B:	ACC=BC.B.h;	break;
	case LD_A_C:	ACC=BC.B.l;	break;
	case LD_A_D:	ACC=DE.B.h;	break;
	case LD_A_E:	ACC=DE.B.l;	break;
	case LD_A_H:	ACC=HL.B.h;	break;
	case LD_A_L:	ACC=HL.B.l;	break;
	case LD_A_xHL:	ACC=ReadMem(HL.W);		break;
	case LD_A_8:	ACC=ReadMem(PC.W++);	break;
	
	case LD_B_A:	BC.B.h=ACC;		break;
	case LD_B_B:	BC.B.h=BC.B.h;	break;
	case LD_B_C:	BC.B.h=BC.B.l;	break;
	case LD_B_D:	BC.B.h=DE.B.h;	break;
	case LD_B_E:	BC.B.h=DE.B.l;	break;
	case LD_B_H:	BC.B.h=HL.B.h;	break;
	case LD_B_L:	BC.B.h=HL.B.l;	break;
	case LD_B_xHL:	BC.B.h=ReadMem(HL.W);	break;
	case LD_B_8:	BC.B.h=ReadMem(PC.W++);	break;
	
	case LD_C_A:	BC.B.l=ACC;		break;
	case LD_C_B:	BC.B.l=BC.B.h;	break;
	case LD_C_C:	BC.B.l=BC.B.l;	break;
	case LD_C_D:	BC.B.l=DE.B.h;	break;
	case LD_C_E:	BC.B.l=DE.B.l;	break;
	case LD_C_H:	BC.B.l=HL.B.h;	break;
	case LD_C_L:	BC.B.l=HL.B.l;	break;
	case LD_C_xHL:	BC.B.l=ReadMem(HL.W);	break;
	case LD_C_8:	BC.B.l=ReadMem(PC.W++);	break;
	
	case LD_D_A:	DE.B.h=ACC;		break;
	case LD_D_B:	DE.B.h=BC.B.h;	break;
	case LD_D_C:	DE.B.h=BC.B.l;	break;
	case LD_D_D:	DE.B.h=DE.B.h;	break;
	case LD_D_E:	DE.B.h=DE.B.l;	break;
	case LD_D_H:	DE.B.h=HL.B.h;	break;
	case LD_D_L:	DE.B.h=HL.B.l;	break;
	case LD_D_xHL:	DE.B.h=ReadMem(HL.W);	break;
	case LD_D_8:	DE.B.h=ReadMem(PC.W++);	break;
	
	case LD_E_A:	DE.B.l=ACC;		break;
	case LD_E_B:	DE.B.l=BC.B.h;	break;
	case LD_E_C:	DE.B.l=BC.B.l;	break;
	case LD_E_D:	DE.B.l=DE.B.h;	break;
	case LD_E_E:	DE.B.l=DE.B.l;	break;
	case LD_E_H:	DE.B.l=HL.B.h;	break;
	case LD_E_L:	DE.B.l=HL.B.l;	break;
	case LD_E_xHL:	DE.B.l=ReadMem(HL.W);	break;
	case LD_E_8:	DE.B.l=ReadMem(PC.W++);	break;
	
	case LD_H_A:	HL.B.h=ACC;		break;
	case LD_H_B:	HL.B.h=BC.B.h;	break;
	case LD_H_C:	HL.B.h=BC.B.l;	break;
	case LD_H_D:	HL.B.h=DE.B.h;	break;
	case LD_H_E:	HL.B.h=DE.B.l;	break;
	case LD_H_H:	HL.B.h=HL.B.h;	break;
	case LD_H_L:	HL.B.h=HL.B.l;	break;
	case LD_H_xHL:	HL.B.h=ReadMem(HL.W);	break;
	case LD_H_8:	HL.B.h=ReadMem(PC.W++);	break;
	
	case LD_L_A:	HL.B.l=ACC;		break;
	case LD_L_B:	HL.B.l=BC.B.h;	break;
	case LD_L_C:	HL.B.l=BC.B.l;	break;
	case LD_L_D:	HL.B.l=DE.B.h;	break;
	case LD_L_E:	HL.B.l=DE.B.l;	break;
	case LD_L_H:	HL.B.l=HL.B.h;	break;
	case LD_L_L:	HL.B.l=HL.B.l;	break;
	case LD_L_xHL:	HL.B.l=ReadMem(HL.W);	break;
	case LD_L_8:	HL.B.l=ReadMem(PC.W++);	break;
	
	case LD_xHL_A:	WriteMem(HL.W,ACC);		break;
	case LD_xHL_B:	WriteMem(HL.W,BC.B.h);	break;
	case LD_xHL_C:	WriteMem(HL.W,BC.B.l);	break;
	case LD_xHL_D:	WriteMem(HL.W,DE.B.h);	break;
	case LD_xHL_E:	WriteMem(HL.W,DE.B.l);	break;
	case LD_xHL_H:	WriteMem(HL.W,HL.B.h);	break;
	case LD_xHL_L:	WriteMem(HL.W,HL.B.l);	break;
	case LD_xHL_8:	WriteMem(HL.W,ReadMem(PC.W++));	break;
	
	case LD_A_xBC:	ACC=ReadMem(BC.W);	break;
	case LD_A_xDE:	ACC=ReadMem(DE.W);	break;
	case LD_A_x16:
		J.B.l = ReadMem(PC.W++);
		J.B.h = ReadMem(PC.W++);
		ACC = ReadMem(J.W);
		break;
	
	case LD_xBC_A:	WriteMem(BC.W,ACC);	break;
	case LD_xDE_A:	WriteMem(DE.W,ACC);	break;
	case LD_x16_A:
		J.B.l = ReadMem(PC.W++);
		J.B.h = ReadMem(PC.W++);
		WriteMem(J.W,ACC);
		break;
	
// 16ビット転送命令
	case LD_BC_16:	M_LDWORD(BC);	break;
	case LD_DE_16:	M_LDWORD(DE);	break;
	case LD_HL_16:	M_LDWORD(HL);	break;
	case LD_SP_16:	M_LDWORD(SP);	break;
	
	case LD_SP_HL:	SP.W=HL.W;	break;
	
	case LD_x16_HL:
		J.B.l = ReadMem(PC.W++);
		J.B.h = ReadMem(PC.W++);
		WriteMem(J.W++,HL.B.l);
		WriteMem(J.W, HL.B.h);
		break;
	case LD_HL_x16:
		J.B.l = ReadMem(PC.W++);
		J.B.h = ReadMem(PC.W++);
		HL.B.l = ReadMem(J.W++);
		HL.B.h = ReadMem(J.W);
		break;
	
	case PUSH_BC:	M_PUSH(BC);	break;
	case PUSH_DE:	M_PUSH(DE);	break;
	case PUSH_HL:	M_PUSH(HL);	break;
	case PUSH_AF:	M_PUSH(AF);	break;

	case POP_BC:	M_POP(BC);	break;
	case POP_DE:	M_POP(DE);	break;
	case POP_HL:	M_POP(HL);	break;
	case POP_AF:	M_POP(AF);	break;
	
// 8ビット算術論理演算命令
	case ADD_A_A:	M_ADD_A(ACC);		break;
	case ADD_A_B:	M_ADD_A(BC.B.h);	break;
	case ADD_A_C:	M_ADD_A(BC.B.l);	break;
	case ADD_A_D:	M_ADD_A(DE.B.h);	break;
	case ADD_A_E:	M_ADD_A(DE.B.l);	break;
	case ADD_A_H:	M_ADD_A(HL.B.h);	break;
	case ADD_A_L:	M_ADD_A(HL.B.l);	break;
	case ADD_A_xHL:	i=ReadMem(HL.W); M_ADD_A(i);	break;
	case ADD_A_8:	i=ReadMem(PC.W++); M_ADD_A(i);	break;
	
	case ADC_A_A:	M_ADC_A(ACC);		break;
	case ADC_A_B:	M_ADC_A(BC.B.h);	break;
	case ADC_A_C:	M_ADC_A(BC.B.l);	break;
	case ADC_A_D:	M_ADC_A(DE.B.h);	break;
	case ADC_A_E:	M_ADC_A(DE.B.l);	break;
	case ADC_A_H:	M_ADC_A(HL.B.h);	break;
	case ADC_A_L:	M_ADC_A(HL.B.l);	break;
	case ADC_A_xHL:	i=ReadMem(HL.W); M_ADC_A(i);	break;
	case ADC_A_8:	i=ReadMem(PC.W++); M_ADC_A(i);	break;
	
	case SUB_A:	M_SUB(ACC);		break;
	case SUB_B:	M_SUB(BC.B.h);	break;
	case SUB_C:	M_SUB(BC.B.l);	break;
	case SUB_D:	M_SUB(DE.B.h);	break;
	case SUB_E:	M_SUB(DE.B.l);	break;
	case SUB_H:	M_SUB(HL.B.h);	break;
	case SUB_L:	M_SUB(HL.B.l);	break;
	case SUB_xHL:	i=ReadMem(HL.W); M_SUB(i);	break;
	case SUB_8:	i=ReadMem(PC.W++); M_SUB(i);	break;
	
	case SBC_A_A:	M_SBC_A(ACC);		break;
	case SBC_A_B:	M_SBC_A(BC.B.h);	break;
	case SBC_A_C:	M_SBC_A(BC.B.l);	break;
	case SBC_A_D:	M_SBC_A(DE.B.h);	break;
	case SBC_A_E:	M_SBC_A(DE.B.l);	break;
	case SBC_A_H:	M_SBC_A(HL.B.h);	break;
	case SBC_A_L:	M_SBC_A(HL.B.l);	break;
	case SBC_A_xHL:	i=ReadMem(HL.W); M_SBC_A(i);	break;
	case SBC_A_8:	i=ReadMem(PC.W++); M_SBC_A(i);	break;
	
	case AND_A:	M_AND(ACC);		break;
	case AND_B:	M_AND(BC.B.h);	break;
	case AND_C:	M_AND(BC.B.l);	break;
	case AND_D:	M_AND(DE.B.h);	break;
	case AND_E:	M_AND(DE.B.l);	break;
	case AND_H:	M_AND(HL.B.h);	break;
	case AND_L:	M_AND(HL.B.l);	break;
	case AND_xHL:	i=ReadMem(HL.W); M_AND(i);	break;
	case AND_8:	i=ReadMem(PC.W++); M_AND(i);	break;
	
	case OR_A:	M_OR(ACC);		break;
	case OR_B:	M_OR(BC.B.h);	break;
	case OR_C:	M_OR(BC.B.l);	break;
	case OR_D:	M_OR(DE.B.h);	break;
	case OR_E:	M_OR(DE.B.l);	break;
	case OR_H:	M_OR(HL.B.h);	break;
	case OR_L:	M_OR(HL.B.l);	break;
	case OR_xHL:	i=ReadMem(HL.W); M_OR(i);	break;
	case OR_8:	i=ReadMem(PC.W++); M_OR(i);		break;
	
	case XOR_A:	M_XOR(ACC);		break;
	case XOR_B:	M_XOR(BC.B.h);	break;
	case XOR_C:	M_XOR(BC.B.l);	break;
	case XOR_D:	M_XOR(DE.B.h);	break;
	case XOR_E:	M_XOR(DE.B.l);	break;
	case XOR_H:	M_XOR(HL.B.h);	break;
	case XOR_L:	M_XOR(HL.B.l);	break;
	case XOR_xHL:	i=ReadMem(HL.W); M_XOR(i);	break;
	case XOR_8:	i=ReadMem(PC.W++); M_XOR(i);	break;
	
	case CP_A:	M_CP(ACC);		break;
	case CP_B:	M_CP(BC.B.h);	break;
	case CP_C:	M_CP(BC.B.l);	break;
	case CP_D:	M_CP(DE.B.h);	break;
	case CP_E:	M_CP(DE.B.l);	break;
	case CP_H:	M_CP(HL.B.h);	break;
	case CP_L:	M_CP(HL.B.l);	break;
	case CP_xHL:	i=ReadMem(HL.W); M_CP(i);	break;
	case CP_8:	i=ReadMem(PC.W++); M_CP(i);		break;
	
	case INC_A:	M_INC(ACC);		break;
	case INC_B:	M_INC(BC.B.h);	break;
	case INC_C:	M_INC(BC.B.l);	break;
	case INC_D:	M_INC(DE.B.h);	break;
	case INC_E:	M_INC(DE.B.l);	break;
	case INC_H:	M_INC(HL.B.h);	break;
	case INC_L:	M_INC(HL.B.l);	break;
	case INC_xHL:	i=ReadMem(HL.W); M_INC(i); WriteMem(HL.W,i);	break;
	
	case DEC_A:	M_DEC(ACC);	break;
	case DEC_B:	M_DEC(BC.B.h);	break;
	case DEC_C:	M_DEC(BC.B.l);	break;
	case DEC_D:	M_DEC(DE.B.h);	break;
	case DEC_E:	M_DEC(DE.B.l);	break;
	case DEC_H:	M_DEC(HL.B.h);	break;
	case DEC_L:	M_DEC(HL.B.l);	break;
	case DEC_xHL:	i=ReadMem(HL.W); M_DEC(i); WriteMem(HL.W,i);	break;
	
// 16ビット算術演算命令
	case ADD_HL_BC:	M_ADDW(HL.W,BC.W);	break;
	case ADD_HL_DE:	M_ADDW(HL.W,DE.W);	break;
	case ADD_HL_HL:	M_ADDW(HL.W,HL.W);	break;
	case ADD_HL_SP:	M_ADDW(HL.W,SP.W);	break;
	
	case INC_BC:	BC.W++;	break;
	case INC_DE:	DE.W++;	break;
	case INC_HL:	HL.W++;	break;
	case INC_SP:	SP.W++;	break;
	
	case DEC_BC:	BC.W--;	break;
	case DEC_DE:	DE.W--;	break;
	case DEC_HL:	HL.W--;	break;
	case DEC_SP:	SP.W--;	break;
	
// レジスタ交換命令
	case EX_AF_AF:
		J.W=AF.W; AF.W=AF1.W; AF1.W=J.W;
		break;
	case EX_DE_HL:
		J.W=DE.W; DE.W=HL.W; HL.W=J.W;
		break;
	case EX_xSP_HL:
		J.B.l = ReadMem(SP.W); WriteMem(SP.W++,HL.B.l);
		J.B.h = ReadMem(SP.W); WriteMem(SP.W--,HL.B.h);
		HL.W = J.W;
		break;
	case EXX:
		J.W=BC.W; BC.W=BC1.W; BC1.W=J.W;
		J.W=DE.W; DE.W=DE1.W; DE1.W=J.W;
		J.W=HL.W; HL.W=HL1.W; HL1.W=J.W;
		break;
	
// 分岐命令
	case JP:	M_JP();	break;
	case JP_NZ:	if( M_NZ() ) M_JP(); else M_JP_SKIP();	break;
	case JP_NC:	if( M_NC() ) M_JP(); else M_JP_SKIP();	break;
	case JP_PO:	if( M_PO() ) M_JP(); else M_JP_SKIP();	break;
	case JP_P:	if( M_P()  ) M_JP(); else M_JP_SKIP();	break;
	case JP_Z:	if( M_Z()  ) M_JP(); else M_JP_SKIP();	break;
	case JP_C:	if( M_C()  ) M_JP(); else M_JP_SKIP();	break;
	case JP_PE:	if( M_PE() ) M_JP(); else M_JP_SKIP();	break;
	case JP_M:	if( M_M()  ) M_JP(); else M_JP_SKIP();	break;
	
	case JR:	M_JR();	break;
	case JR_NZ:	if( M_NZ() ) M_JR(); else M_JR_SKIP();	break;
	case JR_NC:	if( M_NC() ) M_JR(); else M_JR_SKIP();	break;
	case JR_Z:	if( M_Z()  ) M_JR(); else M_JR_SKIP();	break;
	case JR_C:	if( M_C()  ) M_JR(); else M_JR_SKIP();	break;
	
	case CALL:		M_CALL();	break;
	case CALL_NZ:	if( M_NZ() ) M_CALL(); else M_CALL_SKIP();	break;
	case CALL_NC:	if( M_NC() ) M_CALL(); else M_CALL_SKIP();	break;
	case CALL_PO:	if( M_PO() ) M_CALL(); else M_CALL_SKIP();	break;
	case CALL_P:	if( M_P()  ) M_CALL(); else M_CALL_SKIP();	break;
	case CALL_Z:	if( M_Z()  ) M_CALL(); else M_CALL_SKIP();	break;
	case CALL_C:	if( M_C()  ) M_CALL(); else M_CALL_SKIP();	break;
	case CALL_PE:	if( M_PE() ) M_CALL(); else M_CALL_SKIP();	break;
	case CALL_M:	if( M_M()  ) M_CALL(); else M_CALL_SKIP();	break;
	
	case RET:		M_RET();	break;
	case RET_NZ:	if( M_NZ() ) M_RET(); else M_RET_SKIP();	break;
	case RET_NC:	if( M_NC() ) M_RET(); else M_RET_SKIP();	break;
	case RET_PO:	if( M_PO() ) M_RET(); else M_RET_SKIP();	break;
	case RET_P:		if( M_P()  ) M_RET(); else M_RET_SKIP();	break;
	case RET_Z:		if( M_Z()  ) M_RET(); else M_RET_SKIP();	break;
	case RET_C:		if( M_C()  ) M_RET(); else M_RET_SKIP();	break;
	case RET_PE:	if( M_PE() ) M_RET(); else M_RET_SKIP();	break;
	case RET_M:		if( M_M()  ) M_RET(); else M_RET_SKIP();	break;
	
	case JP_xHL:	PC.W = HL.W;	break;
	case DJNZ:	if( --BC.B.h ) M_JR(); else M_JR_SKIP();	break;
	
	case RST00:	M_RST(0x0000);	break;
	case RST08:	M_RST(0x0008);	break;
	case RST10:	M_RST(0x0010);	break;
	case RST18:	M_RST(0x0018);	break;
	case RST20:	M_RST(0x0020);	break;
	case RST28:	M_RST(0x0028);	break;
	case RST30:	M_RST(0x0030);	break;
	case RST38:	M_RST(0x0038);	break;
	
// ローテート／シフト命令
	case RLCA:
		i = ACC>>7;
		ACC = (ACC<<1)|i;
		FLAG = (FLAG&~(H_FLAG|N_FLAG|C_FLAG))|i;
		break;
	case RLA:
		i = ACC>>7;
		ACC = (ACC<<1)|(FLAG&C_FLAG);
		FLAG = (FLAG&~(H_FLAG|N_FLAG|C_FLAG))|i;
		break;
	case RRCA:
		i = ACC&0x01;
		ACC = (ACC>>1)|(i<<7);
		FLAG = (FLAG&~(H_FLAG|N_FLAG|C_FLAG))|i;
		break;
	case RRA:
		i = ACC&0x01;
		ACC = (ACC>>1)|(FLAG<<7);
		FLAG = (FLAG&~(H_FLAG|N_FLAG|C_FLAG))|i;
		break;
	
// 入出力命令
	case IN_A_x8:
		i = ReadIO( (ACC<<8)|ReadMem(PC.W++) );
		ACC = i;
		break;
	case OUT_x8_A:
		WriteIO( (ACC<<8)|ReadMem(PC.W++), ACC );
		break;
	
// その他の命令
	case NOP:	break;
	
	case DI:
		IFF = IFF2 = false;
		break;
	
	case EI:
		if( !IFF ){
			IFF = IFF2 = true;
			NotIntrCheck = true;
		}
		break;
	
	case SCF:
		FLAG=(FLAG&~(H_FLAG|N_FLAG))|C_FLAG;
		break;
	case CCF:
		FLAG ^= C_FLAG;
		FLAG = (FLAG&~(H_FLAG|N_FLAG))|(FLAG&C_FLAG? 0:H_FLAG);
		break;
	
	case CPL:
		ACC = ~ACC;
		FLAG |= (H_FLAG|N_FLAG);
		break;
	case DAA:
		J.W = ACC;
		if( FLAG & C_FLAG ) J.W |= 256;
		if( FLAG & H_FLAG ) J.W |= 512;
		if( FLAG & N_FLAG ) J.W |= 1024;
		AF.W = DAA_table[ J.W ];
		break;
	
	case HALT:
		Halt = true;
		PC.W --;
		break;
