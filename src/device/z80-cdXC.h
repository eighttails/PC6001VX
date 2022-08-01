/////////////////////////////////////////////////////////////////////////////
//  P C 6 0 0 1 V
//  Copyright 1999,2022 Yumitaro
/////////////////////////////////////////////////////////////////////////////
//***************************************************************************
// QUASI88 --- PC-8801 emulator
//	 Copyright (C) Showzoh Fukunaga 1998
//***************************************************************************
// ローテート・シフト命令
	case RLC_xHL:	i = ReadMem( J.W );	M_RLC( i );	WriteMem( J.W, i );	break;
	case RRC_xHL:	i = ReadMem( J.W );	M_RRC( i );	WriteMem( J.W, i );	break;
	case RL_xHL:	i = ReadMem( J.W );	M_RL( i );	WriteMem( J.W, i );	break;
	case RR_xHL:	i = ReadMem( J.W );	M_RR( i );	WriteMem( J.W, i );	break;
	case SLA_xHL:	i = ReadMem( J.W );	M_SLA( i );	WriteMem( J.W, i );	break;
	case SRA_xHL:	i = ReadMem( J.W );	M_SRA( i );	WriteMem( J.W, i );	break;
	case SLL_xHL:	i = ReadMem( J.W );	M_SLL( i );	WriteMem( J.W, i );	break;
	case SRL_xHL:	i = ReadMem( J.W );	M_SRL( i );	WriteMem( J.W, i );	break;
	
// ビット演算命令
	case BIT_0_B:
	case BIT_0_C:
	case BIT_0_D:
	case BIT_0_E:
	case BIT_0_H:
	case BIT_0_L:
	case BIT_0_xHL:
	case BIT_0_A:	i = ReadMem( J.W );	M_BIT( 0, i );	break;
	
	case BIT_1_B:
	case BIT_1_C:
	case BIT_1_D:
	case BIT_1_E:
	case BIT_1_H:
	case BIT_1_L:
	case BIT_1_xHL:
	case BIT_1_A:	i = ReadMem( J.W );	M_BIT( 1, i );	break;
	
	case BIT_2_B:
	case BIT_2_C:
	case BIT_2_D:
	case BIT_2_E:
	case BIT_2_H:
	case BIT_2_L:
	case BIT_2_xHL:
	case BIT_2_A:	i = ReadMem( J.W );	M_BIT( 2, i );	break;
	
	case BIT_3_B:
	case BIT_3_C:
	case BIT_3_D:
	case BIT_3_E:
	case BIT_3_H:
	case BIT_3_L:
	case BIT_3_xHL:
	case BIT_3_A:	i = ReadMem( J.W );	M_BIT( 3, i );	break;
	
	case BIT_4_B:
	case BIT_4_C:
	case BIT_4_D:
	case BIT_4_E:
	case BIT_4_H:
	case BIT_4_L:
	case BIT_4_xHL:
	case BIT_4_A:	i = ReadMem( J.W );	M_BIT( 4, i );	break;
	
	case BIT_5_B:
	case BIT_5_C:
	case BIT_5_D:
	case BIT_5_E:
	case BIT_5_H:
	case BIT_5_L:
	case BIT_5_xHL:
	case BIT_5_A:	i = ReadMem( J.W );	M_BIT( 5, i );	break;
	
	case BIT_6_B:
	case BIT_6_C:
	case BIT_6_D:
	case BIT_6_E:
	case BIT_6_H:
	case BIT_6_L:
	case BIT_6_xHL:
	case BIT_6_A:	i = ReadMem( J.W );	M_BIT( 6, i );	break;
	
	case BIT_7_B:
	case BIT_7_C:
	case BIT_7_D:
	case BIT_7_E:
	case BIT_7_H:
	case BIT_7_L:
	case BIT_7_xHL:
	case BIT_7_A:	i = ReadMem( J.W );	M_BIT( 7, i );	break;
	
	case RES_0_xHL:	i = ReadMem( J.W );	M_RES( 0, i );	WriteMem( J.W, i );	break;
	case RES_1_xHL:	i = ReadMem( J.W );	M_RES( 1, i );	WriteMem( J.W, i );	break;
	case RES_2_xHL:	i = ReadMem( J.W );	M_RES( 2, i );	WriteMem( J.W, i );	break;
	case RES_3_xHL:	i = ReadMem( J.W );	M_RES( 3, i );	WriteMem( J.W, i );	break;
	case RES_4_xHL:	i = ReadMem( J.W );	M_RES( 4, i );	WriteMem( J.W, i );	break;
	case RES_5_xHL:	i = ReadMem( J.W );	M_RES( 5, i );	WriteMem( J.W, i );	break;
	case RES_6_xHL:	i = ReadMem( J.W );	M_RES( 6, i );	WriteMem( J.W, i );	break;
	case RES_7_xHL:	i = ReadMem( J.W );	M_RES( 7, i );	WriteMem( J.W, i );	break;
	
	case SET_0_xHL:	i = ReadMem( J.W );	M_SET( 0, i );	WriteMem( J.W, i );	break;
	case SET_1_xHL:	i = ReadMem( J.W );	M_SET( 1, i );	WriteMem( J.W, i );	break;
	case SET_2_xHL:	i = ReadMem( J.W );	M_SET( 2, i );	WriteMem( J.W, i );	break;
	case SET_3_xHL:	i = ReadMem( J.W );	M_SET( 3, i );	WriteMem( J.W, i );	break;
	case SET_4_xHL:	i = ReadMem( J.W );	M_SET( 4, i );	WriteMem( J.W, i );	break;
	case SET_5_xHL:	i = ReadMem( J.W );	M_SET( 5, i );	WriteMem( J.W, i );	break;
	case SET_6_xHL:	i = ReadMem( J.W );	M_SET( 6, i );	WriteMem( J.W, i );	break;
	case SET_7_xHL:	i = ReadMem( J.W );	M_SET( 7, i );	WriteMem( J.W, i );	break;






// 未定義命令追加
	case RLC_B:	i = ReadMem( J.W ); M_RLC( i ); WriteMem( J.W, i ); BC.B.h = i;	break;
	case RLC_C:	i = ReadMem( J.W ); M_RLC( i ); WriteMem( J.W, i ); BC.B.l = i;	break;
	case RLC_D:	i = ReadMem( J.W ); M_RLC( i ); WriteMem( J.W, i ); DE.B.h = i;	break;
	case RLC_E:	i = ReadMem( J.W ); M_RLC( i ); WriteMem( J.W, i ); DE.B.l = i;	break;
	case RLC_H:	i = ReadMem( J.W ); M_RLC( i ); WriteMem( J.W, i ); HL.B.h = i;	break;
	case RLC_L:	i = ReadMem( J.W ); M_RLC( i ); WriteMem( J.W, i ); HL.B.l = i;	break;
	case RLC_A:	i = ReadMem( J.W ); M_RLC( i ); WriteMem( J.W, i ); AF.B.h = i;	break;
	
	case RRC_B:	i = ReadMem( J.W ); M_RRC( i ); WriteMem( J.W, i ); BC.B.h = i;	break;
	case RRC_C:	i = ReadMem( J.W ); M_RRC( i ); WriteMem( J.W, i ); BC.B.l = i;	break;
	case RRC_D:	i = ReadMem( J.W ); M_RRC( i ); WriteMem( J.W, i ); DE.B.h = i;	break;
	case RRC_E:	i = ReadMem( J.W ); M_RRC( i ); WriteMem( J.W, i ); DE.B.l = i;	break;
	case RRC_H:	i = ReadMem( J.W ); M_RRC( i ); WriteMem( J.W, i ); HL.B.h = i;	break;
	case RRC_L:	i = ReadMem( J.W ); M_RRC( i ); WriteMem( J.W, i ); HL.B.l = i;	break;
	case RRC_A:	i = ReadMem( J.W ); M_RRC( i ); WriteMem( J.W, i ); AF.B.h = i;	break;
	
	case RL_B:	i = ReadMem( J.W ); M_RL( i ); WriteMem( J.W, i ); BC.B.h = i;	break;
	case RL_C:	i = ReadMem( J.W ); M_RL( i ); WriteMem( J.W, i ); BC.B.l = i;	break;
	case RL_D:	i = ReadMem( J.W ); M_RL( i ); WriteMem( J.W, i ); DE.B.h = i;	break;
	case RL_E:	i = ReadMem( J.W ); M_RL( i ); WriteMem( J.W, i ); DE.B.l = i;	break;
	case RL_H:	i = ReadMem( J.W ); M_RL( i ); WriteMem( J.W, i ); HL.B.h = i;	break;
	case RL_L:	i = ReadMem( J.W ); M_RL( i ); WriteMem( J.W, i ); HL.B.l = i;	break;
	case RL_A:	i = ReadMem( J.W ); M_RL( i ); WriteMem( J.W, i ); AF.B.h = i;	break;
	
	case RR_B:	i = ReadMem( J.W ); M_RR( i ); WriteMem( J.W, i ); BC.B.h = i;	break;
	case RR_C:	i = ReadMem( J.W ); M_RR( i ); WriteMem( J.W, i ); BC.B.l = i;	break;
	case RR_D:	i = ReadMem( J.W ); M_RR( i ); WriteMem( J.W, i ); DE.B.h = i;	break;
	case RR_E:	i = ReadMem( J.W ); M_RR( i ); WriteMem( J.W, i ); DE.B.l = i;	break;
	case RR_H:	i = ReadMem( J.W ); M_RR( i ); WriteMem( J.W, i ); HL.B.h = i;	break;
	case RR_L:	i = ReadMem( J.W ); M_RR( i ); WriteMem( J.W, i ); HL.B.l = i;	break;
	case RR_A:	i = ReadMem( J.W ); M_RR( i ); WriteMem( J.W, i ); AF.B.h = i;	break;
	
	case SLA_B:	i = ReadMem( J.W ); M_SLA( i ); WriteMem( J.W, i ); BC.B.h = i;	break;
	case SLA_C:	i = ReadMem( J.W ); M_SLA( i ); WriteMem( J.W, i ); BC.B.l = i;	break;
	case SLA_D:	i = ReadMem( J.W ); M_SLA( i ); WriteMem( J.W, i ); DE.B.h = i;	break;
	case SLA_E:	i = ReadMem( J.W ); M_SLA( i ); WriteMem( J.W, i ); DE.B.l = i;	break;
	case SLA_H:	i = ReadMem( J.W ); M_SLA( i ); WriteMem( J.W, i ); HL.B.h = i;	break;
	case SLA_L:	i = ReadMem( J.W ); M_SLA( i ); WriteMem( J.W, i ); HL.B.l = i;	break;
	case SLA_A:	i = ReadMem( J.W ); M_SLA( i ); WriteMem( J.W, i ); AF.B.h = i;	break;
	
	case SRA_B:	i = ReadMem( J.W ); M_SRA( i ); WriteMem( J.W, i ); BC.B.h = i;	break;
	case SRA_C:	i = ReadMem( J.W ); M_SRA( i ); WriteMem( J.W, i ); BC.B.l = i;	break;
	case SRA_D:	i = ReadMem( J.W ); M_SRA( i ); WriteMem( J.W, i ); DE.B.h = i;	break;
	case SRA_E:	i = ReadMem( J.W ); M_SRA( i ); WriteMem( J.W, i ); DE.B.l = i;	break;
	case SRA_H:	i = ReadMem( J.W ); M_SRA( i ); WriteMem( J.W, i ); HL.B.h = i;	break;
	case SRA_L:	i = ReadMem( J.W ); M_SRA( i ); WriteMem( J.W, i ); HL.B.l = i;	break;
	case SRA_A:	i = ReadMem( J.W ); M_SRA( i ); WriteMem( J.W, i ); AF.B.h = i;	break;
	
	case SLL_B:	i = ReadMem( J.W ); M_SLL( i ); WriteMem( J.W, i ); BC.B.h = i;	break;
	case SLL_C:	i = ReadMem( J.W ); M_SLL( i ); WriteMem( J.W, i ); BC.B.l = i;	break;
	case SLL_D:	i = ReadMem( J.W ); M_SLL( i ); WriteMem( J.W, i ); DE.B.h = i;	break;
	case SLL_E:	i = ReadMem( J.W ); M_SLL( i ); WriteMem( J.W, i ); DE.B.l = i;	break;
	case SLL_H:	i = ReadMem( J.W ); M_SLL( i ); WriteMem( J.W, i ); HL.B.h = i;	break;
	case SLL_L:	i = ReadMem( J.W ); M_SLL( i ); WriteMem( J.W, i ); HL.B.l = i;	break;
	case SLL_A:	i = ReadMem( J.W ); M_SLL( i ); WriteMem( J.W, i ); AF.B.h = i;	break;
	
	case SRL_B:	i = ReadMem( J.W ); M_SRL( i ); WriteMem( J.W, i ); BC.B.h = i;	break;
	case SRL_C:	i = ReadMem( J.W ); M_SRL( i ); WriteMem( J.W, i ); BC.B.l = i;	break;
	case SRL_D:	i = ReadMem( J.W ); M_SRL( i ); WriteMem( J.W, i ); DE.B.h = i;	break;
	case SRL_E:	i = ReadMem( J.W ); M_SRL( i ); WriteMem( J.W, i ); DE.B.l = i;	break;
	case SRL_H:	i = ReadMem( J.W ); M_SRL( i ); WriteMem( J.W, i ); HL.B.h = i;	break;
	case SRL_L:	i = ReadMem( J.W ); M_SRL( i ); WriteMem( J.W, i ); HL.B.l = i;	break;
	case SRL_A:	i = ReadMem( J.W ); M_SRL( i ); WriteMem( J.W, i ); AF.B.h = i;	break;
	
	case RES_0_B:	i = ReadMem( J.W );	M_RES( 0, i );	WriteMem( J.W, i );	BC.B.h = i;	break;
	case RES_0_C:	i = ReadMem( J.W );	M_RES( 0, i );	WriteMem( J.W, i );	BC.B.l = i;	break;
	case RES_0_D:	i = ReadMem( J.W );	M_RES( 0, i );	WriteMem( J.W, i );	DE.B.h = i;	break;
	case RES_0_E:	i = ReadMem( J.W );	M_RES( 0, i );	WriteMem( J.W, i );	DE.B.l = i;	break;
	case RES_0_H:	i = ReadMem( J.W );	M_RES( 0, i );	WriteMem( J.W, i );	HL.B.h = i;	break;
	case RES_0_L:	i = ReadMem( J.W );	M_RES( 0, i );	WriteMem( J.W, i );	HL.B.l = i;	break;
	case RES_0_A:	i = ReadMem( J.W );	M_RES( 0, i );	WriteMem( J.W, i );	AF.B.h = i;	break;
	
	case RES_1_B:	i = ReadMem( J.W );	M_RES( 1, i );	WriteMem( J.W, i );	BC.B.h = i;	break;
	case RES_1_C:	i = ReadMem( J.W );	M_RES( 1, i );	WriteMem( J.W, i );	BC.B.l = i;	break;
	case RES_1_D:	i = ReadMem( J.W );	M_RES( 1, i );	WriteMem( J.W, i );	DE.B.h = i;	break;
	case RES_1_E:	i = ReadMem( J.W );	M_RES( 1, i );	WriteMem( J.W, i );	DE.B.l = i;	break;
	case RES_1_H:	i = ReadMem( J.W );	M_RES( 1, i );	WriteMem( J.W, i );	HL.B.h = i;	break;
	case RES_1_L:	i = ReadMem( J.W );	M_RES( 1, i );	WriteMem( J.W, i );	HL.B.l = i;	break;
	case RES_1_A:	i = ReadMem( J.W );	M_RES( 1, i );	WriteMem( J.W, i );	AF.B.h = i;	break;
	
	case RES_2_B:	i = ReadMem( J.W );	M_RES( 2, i );	WriteMem( J.W, i );	BC.B.h = i;	break;
	case RES_2_C:	i = ReadMem( J.W );	M_RES( 2, i );	WriteMem( J.W, i );	BC.B.l = i;	break;
	case RES_2_D:	i = ReadMem( J.W );	M_RES( 2, i );	WriteMem( J.W, i );	DE.B.h = i;	break;
	case RES_2_E:	i = ReadMem( J.W );	M_RES( 2, i );	WriteMem( J.W, i );	DE.B.l = i;	break;
	case RES_2_H:	i = ReadMem( J.W );	M_RES( 2, i );	WriteMem( J.W, i );	HL.B.h = i;	break;
	case RES_2_L:	i = ReadMem( J.W );	M_RES( 2, i );	WriteMem( J.W, i );	HL.B.l = i;	break;
	case RES_2_A:	i = ReadMem( J.W );	M_RES( 2, i );	WriteMem( J.W, i );	AF.B.h = i;	break;
	
	case RES_3_B:	i = ReadMem( J.W );	M_RES( 3, i );	WriteMem( J.W, i );	BC.B.h = i;	break;
	case RES_3_C:	i = ReadMem( J.W );	M_RES( 3, i );	WriteMem( J.W, i );	BC.B.l = i;	break;
	case RES_3_D:	i = ReadMem( J.W );	M_RES( 3, i );	WriteMem( J.W, i );	DE.B.h = i;	break;
	case RES_3_E:	i = ReadMem( J.W );	M_RES( 3, i );	WriteMem( J.W, i );	DE.B.l = i;	break;
	case RES_3_H:	i = ReadMem( J.W );	M_RES( 3, i );	WriteMem( J.W, i );	HL.B.h = i;	break;
	case RES_3_L:	i = ReadMem( J.W );	M_RES( 3, i );	WriteMem( J.W, i );	HL.B.l = i;	break;
	case RES_3_A:	i = ReadMem( J.W );	M_RES( 3, i );	WriteMem( J.W, i );	AF.B.h = i;	break;
	
	case RES_4_B:	i = ReadMem( J.W );	M_RES( 4, i );	WriteMem( J.W, i );	BC.B.h = i;	break;
	case RES_4_C:	i = ReadMem( J.W );	M_RES( 4, i );	WriteMem( J.W, i );	BC.B.l = i;	break;
	case RES_4_D:	i = ReadMem( J.W );	M_RES( 4, i );	WriteMem( J.W, i );	DE.B.h = i;	break;
	case RES_4_E:	i = ReadMem( J.W );	M_RES( 4, i );	WriteMem( J.W, i );	DE.B.l = i;	break;
	case RES_4_H:	i = ReadMem( J.W );	M_RES( 4, i );	WriteMem( J.W, i );	HL.B.h = i;	break;
	case RES_4_L:	i = ReadMem( J.W );	M_RES( 4, i );	WriteMem( J.W, i );	HL.B.l = i;	break;
	case RES_4_A:	i = ReadMem( J.W );	M_RES( 4, i );	WriteMem( J.W, i );	AF.B.h = i;	break;
	
	case RES_5_B:	i = ReadMem( J.W );	M_RES( 5, i );	WriteMem( J.W, i );	BC.B.h = i;	break;
	case RES_5_C:	i = ReadMem( J.W );	M_RES( 5, i );	WriteMem( J.W, i );	BC.B.l = i;	break;
	case RES_5_D:	i = ReadMem( J.W );	M_RES( 5, i );	WriteMem( J.W, i );	DE.B.h = i;	break;
	case RES_5_E:	i = ReadMem( J.W );	M_RES( 5, i );	WriteMem( J.W, i );	DE.B.l = i;	break;
	case RES_5_H:	i = ReadMem( J.W );	M_RES( 5, i );	WriteMem( J.W, i );	HL.B.h = i;	break;
	case RES_5_L:	i = ReadMem( J.W );	M_RES( 5, i );	WriteMem( J.W, i );	HL.B.l = i;	break;
	case RES_5_A:	i = ReadMem( J.W );	M_RES( 5, i );	WriteMem( J.W, i );	AF.B.h = i;	break;
	
	case RES_6_B:	i = ReadMem( J.W );	M_RES( 6, i );	WriteMem( J.W, i );	BC.B.h = i;	break;
	case RES_6_C:	i = ReadMem( J.W );	M_RES( 6, i );	WriteMem( J.W, i );	BC.B.l = i;	break;
	case RES_6_D:	i = ReadMem( J.W );	M_RES( 6, i );	WriteMem( J.W, i );	DE.B.h = i;	break;
	case RES_6_E:	i = ReadMem( J.W );	M_RES( 6, i );	WriteMem( J.W, i );	DE.B.l = i;	break;
	case RES_6_H:	i = ReadMem( J.W );	M_RES( 6, i );	WriteMem( J.W, i );	HL.B.h = i;	break;
	case RES_6_L:	i = ReadMem( J.W );	M_RES( 6, i );	WriteMem( J.W, i );	HL.B.l = i;	break;
	case RES_6_A:	i = ReadMem( J.W );	M_RES( 6, i );	WriteMem( J.W, i );	AF.B.h = i;	break;
	
	case RES_7_B:	i = ReadMem( J.W );	M_RES( 7, i );	WriteMem( J.W, i );	BC.B.h = i;	break;
	case RES_7_C:	i = ReadMem( J.W );	M_RES( 7, i );	WriteMem( J.W, i );	BC.B.l = i;	break;
	case RES_7_D:	i = ReadMem( J.W );	M_RES( 7, i );	WriteMem( J.W, i );	DE.B.h = i;	break;
	case RES_7_E:	i = ReadMem( J.W );	M_RES( 7, i );	WriteMem( J.W, i );	DE.B.l = i;	break;
	case RES_7_H:	i = ReadMem( J.W );	M_RES( 7, i );	WriteMem( J.W, i );	HL.B.h = i;	break;
	case RES_7_L:	i = ReadMem( J.W );	M_RES( 7, i );	WriteMem( J.W, i );	HL.B.l = i;	break;
	case RES_7_A:	i = ReadMem( J.W );	M_RES( 7, i );	WriteMem( J.W, i );	AF.B.h = i;	break;
	
	case SET_0_B:	i = ReadMem( J.W );	M_SET( 0, i );	WriteMem( J.W, i );	BC.B.h = i;	break;
	case SET_0_C:	i = ReadMem( J.W );	M_SET( 0, i );	WriteMem( J.W, i );	BC.B.l = i;	break;
	case SET_0_D:	i = ReadMem( J.W );	M_SET( 0, i );	WriteMem( J.W, i );	DE.B.h = i;	break;
	case SET_0_E:	i = ReadMem( J.W );	M_SET( 0, i );	WriteMem( J.W, i );	DE.B.l = i;	break;
	case SET_0_H:	i = ReadMem( J.W );	M_SET( 0, i );	WriteMem( J.W, i );	HL.B.h = i;	break;
	case SET_0_L:	i = ReadMem( J.W );	M_SET( 0, i );	WriteMem( J.W, i );	HL.B.l = i;	break;
	case SET_0_A:	i = ReadMem( J.W );	M_SET( 0, i );	WriteMem( J.W, i );	AF.B.h = i;	break;
	
	case SET_1_B:	i = ReadMem( J.W );	M_SET( 1, i );	WriteMem( J.W, i );	BC.B.h = i;	break;
	case SET_1_C:	i = ReadMem( J.W );	M_SET( 1, i );	WriteMem( J.W, i );	BC.B.l = i;	break;
	case SET_1_D:	i = ReadMem( J.W );	M_SET( 1, i );	WriteMem( J.W, i );	DE.B.h = i;	break;
	case SET_1_E:	i = ReadMem( J.W );	M_SET( 1, i );	WriteMem( J.W, i );	DE.B.l = i;	break;
	case SET_1_H:	i = ReadMem( J.W );	M_SET( 1, i );	WriteMem( J.W, i );	HL.B.h = i;	break;
	case SET_1_L:	i = ReadMem( J.W );	M_SET( 1, i );	WriteMem( J.W, i );	HL.B.l = i;	break;
	case SET_1_A:	i = ReadMem( J.W );	M_SET( 1, i );	WriteMem( J.W, i );	AF.B.h = i;	break;
	
	case SET_2_B:	i = ReadMem( J.W );	M_SET( 2, i );	WriteMem( J.W, i );	BC.B.h = i;	break;
	case SET_2_C:	i = ReadMem( J.W );	M_SET( 2, i );	WriteMem( J.W, i );	BC.B.l = i;	break;
	case SET_2_D:	i = ReadMem( J.W );	M_SET( 2, i );	WriteMem( J.W, i );	DE.B.h = i;	break;
	case SET_2_E:	i = ReadMem( J.W );	M_SET( 2, i );	WriteMem( J.W, i );	DE.B.l = i;	break;
	case SET_2_H:	i = ReadMem( J.W );	M_SET( 2, i );	WriteMem( J.W, i );	HL.B.h = i;	break;
	case SET_2_L:	i = ReadMem( J.W );	M_SET( 2, i );	WriteMem( J.W, i );	HL.B.l = i;	break;
	case SET_2_A:	i = ReadMem( J.W );	M_SET( 2, i );	WriteMem( J.W, i );	AF.B.h = i;	break;
	
	case SET_3_B:	i = ReadMem( J.W );	M_SET( 3, i );	WriteMem( J.W, i );	BC.B.h = i;	break;
	case SET_3_C:	i = ReadMem( J.W );	M_SET( 3, i );	WriteMem( J.W, i );	BC.B.l = i;	break;
	case SET_3_D:	i = ReadMem( J.W );	M_SET( 3, i );	WriteMem( J.W, i );	DE.B.h = i;	break;
	case SET_3_E:	i = ReadMem( J.W );	M_SET( 3, i );	WriteMem( J.W, i );	DE.B.l = i;	break;
	case SET_3_H:	i = ReadMem( J.W );	M_SET( 3, i );	WriteMem( J.W, i );	HL.B.h = i;	break;
	case SET_3_L:	i = ReadMem( J.W );	M_SET( 3, i );	WriteMem( J.W, i );	HL.B.l = i;	break;
	case SET_3_A:	i = ReadMem( J.W );	M_SET( 3, i );	WriteMem( J.W, i );	AF.B.h = i;	break;
	
	case SET_4_B:	i = ReadMem( J.W );	M_SET( 4, i );	WriteMem( J.W, i );	BC.B.h = i;	break;
	case SET_4_C:	i = ReadMem( J.W );	M_SET( 4, i );	WriteMem( J.W, i );	BC.B.l = i;	break;
	case SET_4_D:	i = ReadMem( J.W );	M_SET( 4, i );	WriteMem( J.W, i );	DE.B.h = i;	break;
	case SET_4_E:	i = ReadMem( J.W );	M_SET( 4, i );	WriteMem( J.W, i );	DE.B.l = i;	break;
	case SET_4_H:	i = ReadMem( J.W );	M_SET( 4, i );	WriteMem( J.W, i );	HL.B.h = i;	break;
	case SET_4_L:	i = ReadMem( J.W );	M_SET( 4, i );	WriteMem( J.W, i );	HL.B.l = i;	break;
	case SET_4_A:	i = ReadMem( J.W );	M_SET( 4, i );	WriteMem( J.W, i );	AF.B.h = i;	break;
	
	case SET_5_B:	i = ReadMem( J.W );	M_SET( 5, i );	WriteMem( J.W, i );	BC.B.h = i;	break;
	case SET_5_C:	i = ReadMem( J.W );	M_SET( 5, i );	WriteMem( J.W, i );	BC.B.l = i;	break;
	case SET_5_D:	i = ReadMem( J.W );	M_SET( 5, i );	WriteMem( J.W, i );	DE.B.h = i;	break;
	case SET_5_E:	i = ReadMem( J.W );	M_SET( 5, i );	WriteMem( J.W, i );	DE.B.l = i;	break;
	case SET_5_H:	i = ReadMem( J.W );	M_SET( 5, i );	WriteMem( J.W, i );	HL.B.h = i;	break;
	case SET_5_L:	i = ReadMem( J.W );	M_SET( 5, i );	WriteMem( J.W, i );	HL.B.l = i;	break;
	case SET_5_A:	i = ReadMem( J.W );	M_SET( 5, i );	WriteMem( J.W, i );	AF.B.h = i;	break;
	
	case SET_6_B:	i = ReadMem( J.W );	M_SET( 6, i );	WriteMem( J.W, i );	BC.B.h = i;	break;
	case SET_6_C:	i = ReadMem( J.W );	M_SET( 6, i );	WriteMem( J.W, i );	BC.B.l = i;	break;
	case SET_6_D:	i = ReadMem( J.W );	M_SET( 6, i );	WriteMem( J.W, i );	DE.B.h = i;	break;
	case SET_6_E:	i = ReadMem( J.W );	M_SET( 6, i );	WriteMem( J.W, i );	DE.B.l = i;	break;
	case SET_6_H:	i = ReadMem( J.W );	M_SET( 6, i );	WriteMem( J.W, i );	HL.B.h = i;	break;
	case SET_6_L:	i = ReadMem( J.W );	M_SET( 6, i );	WriteMem( J.W, i );	HL.B.l = i;	break;
	case SET_6_A:	i = ReadMem( J.W );	M_SET( 6, i );	WriteMem( J.W, i );	AF.B.h = i;	break;
	
	case SET_7_B:	i = ReadMem( J.W );	M_SET( 7, i );	WriteMem( J.W, i );	BC.B.h = i;	break;
	case SET_7_C:	i = ReadMem( J.W );	M_SET( 7, i );	WriteMem( J.W, i );	BC.B.l = i;	break;
	case SET_7_D:	i = ReadMem( J.W );	M_SET( 7, i );	WriteMem( J.W, i );	DE.B.h = i;	break;
	case SET_7_E:	i = ReadMem( J.W );	M_SET( 7, i );	WriteMem( J.W, i );	DE.B.l = i;	break;
	case SET_7_H:	i = ReadMem( J.W );	M_SET( 7, i );	WriteMem( J.W, i );	HL.B.h = i;	break;
	case SET_7_L:	i = ReadMem( J.W );	M_SET( 7, i );	WriteMem( J.W, i );	HL.B.l = i;	break;
	case SET_7_A:	i = ReadMem( J.W );	M_SET( 7, i );	WriteMem( J.W, i );	AF.B.h = i;	break;
	
