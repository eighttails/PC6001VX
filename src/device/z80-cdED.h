/////////////////////////////////////////////////////////////////////////////
//  P C 6 0 0 1 V
//  Copyright 1999,2022 Yumitaro
/////////////////////////////////////////////////////////////////////////////
//***************************************************************************
// QUASI88 --- PC-8801 emulator
//	 Copyright (C) Showzoh Fukunaga 1998
//***************************************************************************
// 8ビット転送命令
	case LD_A_I:
		ACC  = I;
		FLAG = SZ_table[ACC] | (IFF ? P_FLAG : 0) | (FLAG & C_FLAG);
		break;
	case LD_A_R:
		ACC = (R & 0x7f) | (R_saved & 0x80);
		FLAG = SZ_table[ACC] | (IFF ? P_FLAG : 0) | (FLAG&C_FLAG);
		break;
	case LD_I_A: I = ACC; break;
	case LD_R_A: R = ACC; break;
	
// 16ビット転送命令
	case LD_x16x_HL:
		J.B.l = ReadMem(PC.W++);
		J.B.h = ReadMem(PC.W++);
		WriteMem(J.W++,HL.B.l);
		WriteMem(J.W,  HL.B.h);
		break;
	case LD_x16x_DE:
		J.B.l = ReadMem(PC.W++);
		J.B.h = ReadMem(PC.W++);
		WriteMem(J.W++,DE.B.l);
		WriteMem(J.W,  DE.B.h);
		break;
	case LD_x16x_BC:
		J.B.l = ReadMem(PC.W++);
		J.B.h = ReadMem(PC.W++);
		WriteMem(J.W++,BC.B.l);
		WriteMem(J.W,  BC.B.h);
		break;
	case LD_x16x_SP:
		J.B.l = ReadMem(PC.W++);
		J.B.h = ReadMem(PC.W++);
		WriteMem(J.W++,SP.B.l);
		WriteMem(J.W,  SP.B.h);
		break;
		
	case LD_HL_x16x:
		J.B.l = ReadMem(PC.W++);
		J.B.h = ReadMem(PC.W++);
		HL.B.l = ReadMem(J.W++);
		HL.B.h = ReadMem(J.W  );
		break;
	case LD_DE_x16x:
		J.B.l = ReadMem(PC.W++);
		J.B.h = ReadMem(PC.W++);
		DE.B.l = ReadMem(J.W++);
		DE.B.h = ReadMem(J.W  );
		break;
	case LD_BC_x16x:
		J.B.l = ReadMem(PC.W++);
		J.B.h = ReadMem(PC.W++);
		BC.B.l = ReadMem(J.W++);
		BC.B.h = ReadMem(J.W  );
		break;
	case LD_SP_x16x:
		J.B.l = ReadMem(PC.W++);
		J.B.h = ReadMem(PC.W++);
		SP.B.l = ReadMem(J.W++);
		SP.B.h = ReadMem(J.W  );
		break;
	
// 16ビット算術演算命令
	case ADC_HL_BC: M_ADCW(BC.W); break;
	case ADC_HL_DE: M_ADCW(DE.W); break;
	case ADC_HL_HL: M_ADCW(HL.W); break;
	case ADC_HL_SP: M_ADCW(SP.W); break;
	
	case SBC_HL_BC: M_SBCW(BC.W); break;
	case SBC_HL_DE: M_SBCW(DE.W); break;
	case SBC_HL_HL: M_SBCW(HL.W); break;
	case SBC_HL_SP: M_SBCW(SP.W); break;
	
// ローテート・シフト命令
	case RLD:
		i = ReadMem(HL.W);
		J.B.l = (i<<4)|(ACC&0x0f);
		WriteMem(HL.W,J.B.l);
		ACC  = (i>>4)|(ACC&0xf0);
		FLAG = SZP_table[ACC]|(FLAG&C_FLAG);
		break;
	case RRD:
		i = ReadMem(HL.W);
		J.B.l = (i>>4)|(ACC<<4);
		WriteMem(HL.W,J.B.l);
		ACC  = (i&0x0f)|(ACC&0xf0);
		FLAG = SZP_table[ACC]|(FLAG&C_FLAG);
		break;
	
// ＣＰＵ制御命令
	case IM_0:
	case IM_0_4E:
	case IM_0_66:
	case IM_0_6E: IM = 0; break;
	
	case IM_1:
	case IM_1_76: IM = 1; break;
	
	case IM_2:
	case IM_2_7E: IM = 2; break;
	
// アキュムレータ操作命令
	case NEG:
	case NEG_4C:
	case NEG_54:
	case NEG_5C:
	case NEG_64:
	case NEG_6C:
	case NEG_74:
	case NEG_7C: i=ACC; ACC=0; M_SUB(i); break;
	
// 分岐命令
	case RETI:		M_RET();	break;
	
	case RETN:
	case RETN_55:
	case RETN_5D:
	case RETN_65:
	case RETN_6D:
	case RETN_7D:  
	case RETN_75:	M_RET();	break;

//	case RETI:
//	case RETI_5D:
//	case RETI_6D:
//	case RETI_7D: M_RET(); break;
	
//	case RETN:
//	case RETN_55:
//	case RETN_65:
//	case RETN_75: IFF = IFF2; M_RET(); break;

	
// 入出力命令
	case IN_B_xC: M_IN_C(BC.B.h); break;
	case IN_C_xC: M_IN_C(BC.B.l); break;
	case IN_D_xC: M_IN_C(DE.B.h); break;
	case IN_E_xC: M_IN_C(DE.B.l); break;
	case IN_H_xC: M_IN_C(HL.B.h); break;
	case IN_L_xC: M_IN_C(HL.B.l); break;
	case IN_A_xC: M_IN_C(ACC);    break;
	case IN_F_xC: M_IN_C(J.B.l);       break;
	
	case OUT_xC_B: M_OUT_C(BC.B.h); break;
	case OUT_xC_C: M_OUT_C(BC.B.l); break;
	case OUT_xC_D: M_OUT_C(DE.B.h); break;
	case OUT_xC_E: M_OUT_C(DE.B.l); break;
	case OUT_xC_H: M_OUT_C(HL.B.h); break;
	case OUT_xC_L: M_OUT_C(HL.B.l); break;
	case OUT_xC_A: M_OUT_C(ACC);    break;
	case OUT_xC_F: M_OUT_C(0);           break;
	
	case INI:
		i = ReadIO(BC.W);
		WriteMem(HL.W++,i);
		BC.B.h--;
		FLAG = (BC.B.h? 0:Z_FLAG)|N_FLAG|(FLAG&C_FLAG);
		break;
	case INIR:
		i = ReadIO(BC.W);
		WriteMem(HL.W++,i);
		BC.B.h--;
		FLAG = (BC.B.h? 0:Z_FLAG)|N_FLAG|(FLAG&C_FLAG);
		if( BC.B.h ){
			state += 5;
			PC.W -= 2;
//			NotIntrCheck = true;
		}
		break;
	case IND:
		i = ReadIO(BC.W);
		WriteMem(HL.W--,i);
		BC.B.h--;
		FLAG = (BC.B.h? 0:Z_FLAG)|N_FLAG|(FLAG&C_FLAG);
		break;
	case INDR:
		i = ReadIO(BC.W);
		WriteMem(HL.W--,i);
		BC.B.h--;
		FLAG = (BC.B.h? 0:Z_FLAG)|N_FLAG|(FLAG&C_FLAG);
		if( BC.B.h ){
			state += 5;
			PC.W -= 2;
//			NotIntrCheck = true;
		}
		break;
		
	case OUTI:
		BC.B.h--;
		WriteIO(BC.W,ReadMem(HL.W));
		HL.W++;
		FLAG = (BC.B.h? 0:Z_FLAG)|N_FLAG|(FLAG&C_FLAG);
		break;
	case OTIR:
		BC.B.h--;
		WriteIO(BC.W,ReadMem(HL.W));
		HL.W++;
		FLAG = (BC.B.h? 0:Z_FLAG)|N_FLAG|(FLAG&C_FLAG);
		if( BC.B.h ){
			state += 5;
			PC.W -= 2;
//			NotIntrCheck = true;
		}
		break;
	case OUTD:
		BC.B.h--;
		WriteIO(BC.W,ReadMem(HL.W));
		HL.W--;
		FLAG = (BC.B.h? 0:Z_FLAG)|N_FLAG|(FLAG&C_FLAG);
		break;
	case OTDR:
		BC.B.h--;
		WriteIO(BC.W,ReadMem(HL.W));
		HL.W--;
		FLAG = (BC.B.h? 0:Z_FLAG)|N_FLAG|(FLAG&C_FLAG);
		if( BC.B.h ){
			state += 5;
			PC.W -= 2;
//			NotIntrCheck = true;
		}
		break;
	
// ブロック転送命令
	case LDI:
		WriteMem(DE.W++,ReadMem(HL.W++));
		BC.W--;
		FLAG = (FLAG&~(N_FLAG|H_FLAG|P_FLAG))|(BC.W? P_FLAG:0);
		break;
	case LDIR:
		WriteMem(DE.W++,ReadMem(HL.W++));
		BC.W--;
		FLAG = (FLAG&~(N_FLAG|H_FLAG|P_FLAG))|(BC.W? P_FLAG:0);
		if( BC.W ){
			state += 5;
			PC.W -= 2;
//			NotIntrCheck = true;
		}
		break;
	case LDD:
		WriteMem(DE.W--,ReadMem(HL.W--));
		BC.W--;
		FLAG = (FLAG&~(N_FLAG|H_FLAG|P_FLAG))|(BC.W? P_FLAG:0);
		break;
	case LDDR:
		WriteMem(DE.W--,ReadMem(HL.W--));
		BC.W--;
		FLAG = (FLAG&~(N_FLAG|H_FLAG|P_FLAG))|(BC.W? P_FLAG:0);
		if( BC.W ){
			state += 5;
			PC.W -= 2;
//			NotIntrCheck = true;
		}
		break;
	
// ブロックサーチ命令
	case CPI:
		i = ReadMem(HL.W++);
		J.B.l = ACC-i;
		BC.W--;
		FLAG = SZ_table[J.B.l] | ((ACC^i^J.B.l)&H_FLAG) | (BC.W? P_FLAG:0) | N_FLAG | (FLAG&C_FLAG);
		break;
	case CPIR:
		i = ReadMem(HL.W++);
		J.B.l = ACC-i;
		BC.W--;
		FLAG = SZ_table[J.B.l] | ((ACC^i^J.B.l)&H_FLAG) | (BC.W? P_FLAG:0) | N_FLAG | (FLAG&C_FLAG);
		if( BC.W && J.B.l ){
			state += 5;
			PC.W -= 2;
//			NotIntrCheck = true;
		}
		break;
	case CPD:
		i = ReadMem(HL.W--);
		J.B.l = ACC-i;
		BC.W--;
		FLAG = SZ_table[J.B.l] | ((ACC^i^J.B.l)&H_FLAG) | (BC.W? P_FLAG:0) | N_FLAG | (FLAG&C_FLAG);
		break;
	case CPDR:
		i = ReadMem(HL.W--);
		J.B.l = ACC-i;
		BC.W--;
		FLAG = SZ_table[J.B.l] | ((ACC^i^J.B.l)&H_FLAG) | (BC.W? P_FLAG:0) | N_FLAG | (FLAG&C_FLAG);
		if( BC.W && J.B.l ){
			state += 5;
			PC.W -= 2;
//			NotIntrCheck = true;
		}
		break;
