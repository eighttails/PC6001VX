// 8ビット転送命令
	case LD_A_H:	ACC=XX.B.h;	break;
	case LD_A_L:	ACC=XX.B.l;	break;
	case LD_A_xHL:	ACC=ReadMem(XX.W+(offset)ReadMem(PC.W++));	break;
	
	case LD_B_H:	BC.B.h=XX.B.h;	break;
	case LD_B_L:	BC.B.h=XX.B.l;	break;
	case LD_B_xHL:	BC.B.h=ReadMem(XX.W+(offset)ReadMem(PC.W++));	break;
	
	case LD_C_H:	BC.B.l=XX.B.h;	break;
	case LD_C_L:	BC.B.l=XX.B.l;	break;
	case LD_C_xHL:	BC.B.l=ReadMem(XX.W+(offset)ReadMem(PC.W++));	break;
	
	case LD_D_H:	DE.B.h=XX.B.h;	break;
	case LD_D_L:	DE.B.h=XX.B.l;	break;
	case LD_D_xHL:	DE.B.h=ReadMem(XX.W+(offset)ReadMem(PC.W++));	break;
	
	case LD_E_H:	DE.B.l=XX.B.h;	break;
	case LD_E_L:	DE.B.l=XX.B.l;	break;
	case LD_E_xHL:	DE.B.l=ReadMem(XX.W+(offset)ReadMem(PC.W++));	break;
	
	case LD_H_A:	XX.B.h=ACC;	break;
	case LD_H_B:	XX.B.h=BC.B.h;	break;
	case LD_H_C:	XX.B.h=BC.B.l;	break;
	case LD_H_D:	XX.B.h=DE.B.h;	break;
	case LD_H_E:	XX.B.h=DE.B.l;	break;
	case LD_H_H:	XX.B.h=XX.B.h;	break;
	case LD_H_L:	XX.B.h=XX.B.l;	break;
	case LD_H_xHL:	HL.B.h=ReadMem(XX.W+(offset)ReadMem(PC.W++));	break;
	case LD_H_8:	XX.B.h=ReadMem(PC.W++);	break;
	
	case LD_L_A:	XX.B.l=ACC;	break;
	case LD_L_B:	XX.B.l=BC.B.h;	break;
	case LD_L_C:	XX.B.l=BC.B.l;	break;
	case LD_L_D:	XX.B.l=DE.B.h;	break;
	case LD_L_E:	XX.B.l=DE.B.l;	break;
	case LD_L_H:	XX.B.l=XX.B.h;	break;
	case LD_L_L:	XX.B.l=XX.B.l;	break;
	case LD_L_xHL:	HL.B.l=ReadMem(XX.W+(offset)ReadMem(PC.W++));	break;
	case LD_L_8:	XX.B.l=ReadMem(PC.W++);	break;
	
	case LD_xHL_A:
		J.W=XX.W+(offset)ReadMem(PC.W++);
		WriteMem(J.W,ACC);
		break;
	case LD_xHL_B:
		J.W=XX.W+(offset)ReadMem(PC.W++);
		WriteMem(J.W,BC.B.h);
		break;
	case LD_xHL_C:
		J.W=XX.W+(offset)ReadMem(PC.W++);
		WriteMem(J.W,BC.B.l);
		break;
	case LD_xHL_D:
		J.W=XX.W+(offset)ReadMem(PC.W++);
		WriteMem(J.W,DE.B.h);
		break;
	case LD_xHL_E:
		J.W=XX.W+(offset)ReadMem(PC.W++);
		WriteMem(J.W,DE.B.l);
		break;
	case LD_xHL_H:
		J.W=XX.W+(offset)ReadMem(PC.W++);
		WriteMem(J.W,HL.B.h);
		break;
	case LD_xHL_L:
		J.W=XX.W+(offset)ReadMem(PC.W++);
		WriteMem(J.W,HL.B.l);
		break;
	case LD_xHL_8:
		J.W=XX.W+(offset)ReadMem(PC.W++);
		WriteMem(J.W,ReadMem(PC.W++));
		break;
	
// 16ビット転送命令
	case LD_HL_16:	M_LDWORD(XX);	break;
	
	case LD_SP_HL:	SP.W=XX.W;	break;
	
	case LD_x16_HL:
		J.B.l=ReadMem(PC.W++);
		J.B.h=ReadMem(PC.W++);
		WriteMem(J.W++,XX.B.l);
		WriteMem(J.W,  XX.B.h);
		break;
	case LD_HL_x16:
		J.B.l=ReadMem(PC.W++);
		J.B.h=ReadMem(PC.W++);
		XX.B.l=ReadMem(J.W++);
		XX.B.h=ReadMem(J.W);
		break;
	
	case PUSH_HL:	M_PUSH(XX);	break;
	case POP_HL:	M_POP(XX);	break;
	
// 8ビット算術論理演算命令
	case ADD_A_H:	M_ADD_A(XX.B.h);	break;
	case ADD_A_L:	M_ADD_A(XX.B.l);	break;
	case ADD_A_xHL:
		i=ReadMem(XX.W+(offset)ReadMem(PC.W++));
 		M_ADD_A(i);
		break;
	
	case ADC_A_H:	M_ADC_A(XX.B.h);	break;
	case ADC_A_L:	M_ADC_A(XX.B.l);	break;
	case ADC_A_xHL:
		i=ReadMem(XX.W+(offset)ReadMem(PC.W++));
 		M_ADC_A(i);
		break;
	
	case SUB_H:	M_SUB(XX.B.h);	break;
	case SUB_L:	M_SUB(XX.B.l);	break;
	case SUB_xHL:
		i=ReadMem(XX.W+(offset)ReadMem(PC.W++));
 		M_SUB(i);
		break;
	
	case SBC_A_H:	M_SBC_A(XX.B.h);	break;
	case SBC_A_L:	M_SBC_A(XX.B.l);	break;
	case SBC_A_xHL:
		i=ReadMem(XX.W+(offset)ReadMem(PC.W++));
 		M_SBC_A(i);
		break;
	
	case AND_H:	M_AND(XX.B.h);	break;
	case AND_L:	M_AND(XX.B.l);	break;
	case AND_xHL:
		i=ReadMem(XX.W+(offset)ReadMem(PC.W++));
 		M_AND(i);
		break;
	
	case OR_H:	 M_OR(XX.B.h);	break;
	case OR_L:	 M_OR(XX.B.l);	break;
	case OR_xHL:
		i=ReadMem(XX.W+(offset)ReadMem(PC.W++));
 		M_OR(i);
		break;
	
	case XOR_H:	M_XOR(XX.B.h);	break;
	case XOR_L:	M_XOR(XX.B.l);	break;
	case XOR_xHL:
		i=ReadMem(XX.W+(offset)ReadMem(PC.W++));
 		M_XOR(i);
		break;
	
	case CP_H:	 M_CP(XX.B.h);	break;
	case CP_L:	 M_CP(XX.B.l);	break;
	case CP_xHL:
		i=ReadMem(XX.W+(offset)ReadMem(PC.W++));
 		M_CP(i);
		break;
	
	case INC_H:	M_INC(XX.B.h);	break;
	case INC_L:	M_INC(XX.B.l);	break;
	case INC_xHL:
		i=ReadMem(XX.W+(offset)ReadMem(PC.W));
 		M_INC(i);
 		WriteMem(XX.W+(offset)ReadMem(PC.W++),i);
		break;
	
	case DEC_H:	M_DEC(XX.B.h);	break;
	case DEC_L:	M_DEC(XX.B.l);	break;
	case DEC_xHL:
		i=ReadMem(XX.W+(offset)ReadMem(PC.W));
 		M_DEC(i);
 		WriteMem(XX.W+(offset)ReadMem(PC.W++),i);
		break;
	
// 16ビット算術演算命令
	case ADD_HL_BC:	M_ADDW(XX.W,BC.W);	break;
	case ADD_HL_DE:	M_ADDW(XX.W,DE.W);	break;
	case ADD_HL_HL:	M_ADDW(XX.W,XX.W);	break;
	case ADD_HL_SP:	M_ADDW(XX.W,SP.W);	break;
	
	case INC_HL:	XX.W++;	break;
	case DEC_HL:	XX.W--;	break;
	
// レジスタ交換命令
	case EX_xSP_HL:
		J.B.l=ReadMem(SP.W); WriteMem(SP.W++,XX.B.l);
		J.B.h=ReadMem(SP.W); WriteMem(SP.W--,XX.B.h);
		XX.W=J.W;
	break;
	
// 分岐命令
	case JP_xHL:	PC.W = XX.W;	break;
