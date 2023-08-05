/////////////////////////////////////////////////////////////////////////////
//  P C 6 0 0 1 V
//  Copyright 1999 Yumitaro
/////////////////////////////////////////////////////////////////////////////
//***************************************************************************
// QUASI88 --- PC-8801 emulator
//	 Copyright (C) Showzoh Fukunaga 1998
//***************************************************************************
// ローテート・シフト命令
	case RLC_B:		M_RLC( BC.B.h );	break;
	case RLC_C:		M_RLC( BC.B.l );	break;
	case RLC_D:		M_RLC( DE.B.h );	break;
	case RLC_E:		M_RLC( DE.B.l );	break;
	case RLC_H:		M_RLC( HL.B.h );	break;
	case RLC_L:		M_RLC( HL.B.l );	break;
	case RLC_xHL:	i = ReadMem( HL.W ); M_RLC( i ); WriteMem( HL.W, i );	break;
	case RLC_A:		M_RLC( AF.B.h );	break;
	
	case RRC_B:		M_RRC( BC.B.h );	break;
	case RRC_C:		M_RRC( BC.B.l );	break;
	case RRC_D:		M_RRC( DE.B.h );	break;
	case RRC_E:		M_RRC( DE.B.l );	break;
	case RRC_H:		M_RRC( HL.B.h );	break;
	case RRC_L:		M_RRC( HL.B.l );	break;
	case RRC_xHL:	i = ReadMem( HL.W ); M_RRC( i ); WriteMem( HL.W, i );	break;
	case RRC_A:		M_RRC( AF.B.h );	break;
	
	case RL_B:		M_RL( BC.B.h );		break;
	case RL_C:		M_RL( BC.B.l );		break;
	case RL_D:		M_RL( DE.B.h );		break;
	case RL_E:		M_RL( DE.B.l );		break;
	case RL_H:		M_RL( HL.B.h );		break;
	case RL_L:		M_RL( HL.B.l );		break;
	case RL_xHL:	i = ReadMem( HL.W ); M_RL( i ); WriteMem( HL.W, i );	break;
	case RL_A:		M_RL( AF.B.h );		break;
	
	case RR_B:		M_RR( BC.B.h );		break;
	case RR_C:		M_RR( BC.B.l );		break;
	case RR_D:		M_RR( DE.B.h );		break;
	case RR_E:		M_RR( DE.B.l );		break;
	case RR_H:		M_RR( HL.B.h );		break;
	case RR_L:		M_RR( HL.B.l );		break;
	case RR_xHL:	i = ReadMem( HL.W ); M_RR( i ); WriteMem( HL.W, i );	break;
	case RR_A:		M_RR( AF.B.h );		break;
	
	case SLA_B:		M_SLA( BC.B.h );	break;
	case SLA_C:		M_SLA( BC.B.l );	break;
	case SLA_D:		M_SLA( DE.B.h );	break;
	case SLA_E:		M_SLA( DE.B.l );	break;
	case SLA_H:		M_SLA( HL.B.h );	break;
	case SLA_L:		M_SLA( HL.B.l );	break;
	case SLA_xHL:	i = ReadMem( HL.W ); M_SLA( i ); WriteMem( HL.W, i );	break;
	case SLA_A:		M_SLA( AF.B.h );	break;
	
	case SRA_B:		M_SRA( BC.B.h );	break;
	case SRA_C:		M_SRA( BC.B.l );	break;
	case SRA_D:		M_SRA( DE.B.h );	break;
	case SRA_E:		M_SRA( DE.B.l );	break;
	case SRA_H:		M_SRA( HL.B.h );	break;
	case SRA_L:		M_SRA( HL.B.l );	break;
	case SRA_xHL:	i = ReadMem( HL.W ); M_SRA( i ); WriteMem( HL.W, i );	break;
	case SRA_A:		M_SRA( AF.B.h );	break;
	
	case SLL_B:		M_SLL( BC.B.h );	break;
	case SLL_C:		M_SLL( BC.B.l );	break;
	case SLL_D:		M_SLL( DE.B.h );	break;
	case SLL_E:		M_SLL( DE.B.l );	break;
	case SLL_H:		M_SLL( HL.B.h );	break;
	case SLL_L:		M_SLL( HL.B.l );	break;
	case SLL_xHL:	i = ReadMem( HL.W ); M_SLL( i ); WriteMem( HL.W, i );	break;
	case SLL_A:		M_SLL( AF.B.h );	break;
	
	case SRL_B:		M_SRL( BC.B.h );	break;
	case SRL_C:		M_SRL( BC.B.l );	break;
	case SRL_D:		M_SRL( DE.B.h );	break;
	case SRL_E:		M_SRL( DE.B.l );	break;
	case SRL_H:		M_SRL( HL.B.h );	break;
	case SRL_L:		M_SRL( HL.B.l );	break;
	case SRL_xHL:	i = ReadMem( HL.W ); M_SRL( i ); WriteMem( HL.W, i );	break;
	case SRL_A:		M_SRL( AF.B.h );	break;
	
// ビット操作命令
	case BIT_0_B:	M_BIT( 0, BC.B.h );	break;
	case BIT_0_C:	M_BIT( 0, BC.B.l );	break;
	case BIT_0_D:	M_BIT( 0, DE.B.h );	break;
	case BIT_0_E:	M_BIT( 0, DE.B.l );	break;
	case BIT_0_H:	M_BIT( 0, HL.B.h );	break;
	case BIT_0_L:	M_BIT( 0, HL.B.l );	break;
	case BIT_0_xHL: i = ReadMem( HL.W ); M_BIT( 0, i );	break;
	case BIT_0_A:	M_BIT( 0, AF.B.h );	break;
	
	case BIT_1_B:	M_BIT( 1, BC.B.h );	break;
	case BIT_1_C:	M_BIT( 1, BC.B.l );	break;
	case BIT_1_D:	M_BIT( 1, DE.B.h );	break;
	case BIT_1_E:	M_BIT( 1, DE.B.l );	break;
	case BIT_1_H:	M_BIT( 1, HL.B.h );	break;
	case BIT_1_L:	M_BIT( 1, HL.B.l );	break;
	case BIT_1_xHL: i = ReadMem( HL.W ); M_BIT( 1, i );	break;
	case BIT_1_A:	M_BIT( 1, AF.B.h );	break;
	
	case BIT_2_B:	M_BIT( 2, BC.B.h );	break;
	case BIT_2_C:	M_BIT( 2, BC.B.l );	break;
	case BIT_2_D:	M_BIT( 2, DE.B.h );	break;
	case BIT_2_E:	M_BIT( 2, DE.B.l );	break;
	case BIT_2_H:	M_BIT( 2, HL.B.h );	break;
	case BIT_2_L:	M_BIT( 2, HL.B.l );	break;
	case BIT_2_xHL: i = ReadMem( HL.W ); M_BIT( 2, i );	break;
	case BIT_2_A:	M_BIT( 2, AF.B.h );	break;
	
	case BIT_3_B:	M_BIT( 3, BC.B.h );	break;
	case BIT_3_C:	M_BIT( 3, BC.B.l );	break;
	case BIT_3_D:	M_BIT( 3, DE.B.h );	break;
	case BIT_3_E:	M_BIT( 3, DE.B.l );	break;
	case BIT_3_H:	M_BIT( 3, HL.B.h );	break;
	case BIT_3_L:	M_BIT( 3, HL.B.l );	break;
	case BIT_3_xHL: i = ReadMem( HL.W ); M_BIT( 3, i );	break;
	case BIT_3_A:	M_BIT( 3, AF.B.h );	break;
	
	case BIT_4_B:	M_BIT( 4, BC.B.h );	break;
	case BIT_4_C:	M_BIT( 4, BC.B.l );	break;
	case BIT_4_D:	M_BIT( 4, DE.B.h );	break;
	case BIT_4_E:	M_BIT( 4, DE.B.l );	break;
	case BIT_4_H:	M_BIT( 4, HL.B.h );	break;
	case BIT_4_L:	M_BIT( 4, HL.B.l );	break;
	case BIT_4_xHL: i = ReadMem( HL.W ); M_BIT( 4, i );	break;
	case BIT_4_A:	M_BIT( 4, AF.B.h );	break;
	
	case BIT_5_B:	M_BIT( 5, BC.B.h );	break;
	case BIT_5_C:	M_BIT( 5, BC.B.l );	break;
	case BIT_5_D:	M_BIT( 5, DE.B.h );	break;
	case BIT_5_E:	M_BIT( 5, DE.B.l );	break;
	case BIT_5_H:	M_BIT( 5, HL.B.h );	break;
	case BIT_5_L:	M_BIT( 5, HL.B.l );	break;
	case BIT_5_xHL: i = ReadMem( HL.W ); M_BIT( 5, i );	break;
	case BIT_5_A:	M_BIT( 5, AF.B.h );	break;
	
	case BIT_6_B:	M_BIT( 6, BC.B.h );	break;
	case BIT_6_C:	M_BIT( 6, BC.B.l );	break;
	case BIT_6_D:	M_BIT( 6, DE.B.h );	break;
	case BIT_6_E:	M_BIT( 6, DE.B.l );	break;
	case BIT_6_H:	M_BIT( 6, HL.B.h );	break;
	case BIT_6_L:	M_BIT( 6, HL.B.l );	break;
	case BIT_6_xHL: i = ReadMem( HL.W ); M_BIT( 6, i );	break;
	case BIT_6_A:	M_BIT( 6, AF.B.h );	break;
	
	case BIT_7_B:	M_BIT( 7, BC.B.h );	break;
	case BIT_7_C:	M_BIT( 7, BC.B.l );	break;
	case BIT_7_D:	M_BIT( 7, DE.B.h );	break;
	case BIT_7_E:	M_BIT( 7, DE.B.l );	break;
	case BIT_7_H:	M_BIT( 7, HL.B.h );	break;
	case BIT_7_L:	M_BIT( 7, HL.B.l );	break;
	case BIT_7_xHL: i = ReadMem( HL.W ); M_BIT( 7, i );	break;
	case BIT_7_A:	M_BIT( 7, AF.B.h );	break;
	
	case RES_0_B:	M_RES( 0, BC.B.h );	break;
	case RES_0_C:	M_RES( 0, BC.B.l );	break;
	case RES_0_D:	M_RES( 0, DE.B.h );	break;
	case RES_0_E:	M_RES( 0, DE.B.l );	break;
	case RES_0_H:	M_RES( 0, HL.B.h );	break;
	case RES_0_L:	M_RES( 0, HL.B.l );	break;
	case RES_0_xHL:	i = ReadMem( HL.W ); M_RES( 0, i ); WriteMem( HL.W, i );	break;
	case RES_0_A:	M_RES( 0, AF.B.h );	break;
	
	case RES_1_B:	M_RES( 1, BC.B.h );	break;
	case RES_1_C:	M_RES( 1, BC.B.l );	break;
	case RES_1_D:	M_RES( 1, DE.B.h );	break;
	case RES_1_E:	M_RES( 1, DE.B.l );	break;
	case RES_1_H:	M_RES( 1, HL.B.h );	break;
	case RES_1_L:	M_RES( 1, HL.B.l );	break;
	case RES_1_xHL:	i = ReadMem( HL.W ); M_RES( 1, i ); WriteMem( HL.W, i );	break;
	case RES_1_A:	M_RES( 1, AF.B.h );	break;
	
	case RES_2_B:	M_RES( 2, BC.B.h );	break;
	case RES_2_C:	M_RES( 2, BC.B.l );	break;
	case RES_2_D:	M_RES( 2, DE.B.h );	break;
	case RES_2_E:	M_RES( 2, DE.B.l );	break;
	case RES_2_H:	M_RES( 2, HL.B.h );	break;
	case RES_2_L:	M_RES( 2, HL.B.l );	break;
	case RES_2_xHL:	i = ReadMem( HL.W ); M_RES( 2, i ); WriteMem( HL.W, i );	break;
	case RES_2_A:	M_RES( 2, AF.B.h );	break;
	
	case RES_3_B:	M_RES( 3, BC.B.h );	break;
	case RES_3_C:	M_RES( 3, BC.B.l );	break;
	case RES_3_D:	M_RES( 3, DE.B.h );	break;
	case RES_3_E:	M_RES( 3, DE.B.l );	break;
	case RES_3_H:	M_RES( 3, HL.B.h );	break;
	case RES_3_L:	M_RES( 3, HL.B.l );	break;
	case RES_3_xHL:	i = ReadMem( HL.W ); M_RES( 3, i ); WriteMem( HL.W, i );	break;
	case RES_3_A:	M_RES( 3, AF.B.h );	break;
	
	case RES_4_B:	M_RES( 4, BC.B.h );	break;
	case RES_4_C:	M_RES( 4, BC.B.l );	break;
	case RES_4_D:	M_RES( 4, DE.B.h );	break;
	case RES_4_E:	M_RES( 4, DE.B.l );	break;
	case RES_4_H:	M_RES( 4, HL.B.h );	break;
	case RES_4_L:	M_RES( 4, HL.B.l );	break;
	case RES_4_xHL:	i = ReadMem( HL.W ); M_RES( 4, i ); WriteMem( HL.W, i );	break;
	case RES_4_A:	M_RES( 4, AF.B.h );	break;
	
	case RES_5_B:	M_RES( 5, BC.B.h );	break;
	case RES_5_C:	M_RES( 5, BC.B.l );	break;
	case RES_5_D:	M_RES( 5, DE.B.h );	break;
	case RES_5_E:	M_RES( 5, DE.B.l );	break;
	case RES_5_H:	M_RES( 5, HL.B.h );	break;
	case RES_5_L:	M_RES( 5, HL.B.l );	break;
	case RES_5_xHL:	i = ReadMem( HL.W ); M_RES( 5, i ); WriteMem( HL.W, i );	break;
	case RES_5_A:	M_RES( 5, AF.B.h );	break;
	
	case RES_6_B:	M_RES( 6, BC.B.h );	break;
	case RES_6_C:	M_RES( 6, BC.B.l );	break;
	case RES_6_D:	M_RES( 6, DE.B.h );	break;
	case RES_6_E:	M_RES( 6, DE.B.l );	break;
	case RES_6_H:	M_RES( 6, HL.B.h );	break;
	case RES_6_L:	M_RES( 6, HL.B.l );	break;
	case RES_6_xHL:	i = ReadMem( HL.W ); M_RES( 6, i ); WriteMem( HL.W, i );	break;
	case RES_6_A:	M_RES( 6, AF.B.h );	break;
	
	case RES_7_B:	M_RES( 7, BC.B.h );	break;
	case RES_7_C:	M_RES( 7, BC.B.l );	break;
	case RES_7_D:	M_RES( 7, DE.B.h );	break;
	case RES_7_E:	M_RES( 7, DE.B.l );	break;
	case RES_7_H:	M_RES( 7, HL.B.h );	break;
	case RES_7_L:	M_RES( 7, HL.B.l );	break;
	case RES_7_xHL:	i = ReadMem( HL.W ); M_RES( 7, i ); WriteMem( HL.W, i );	break;
	case RES_7_A:	M_RES( 7, AF.B.h );	break;
	
	case SET_0_B:	M_SET( 0, BC.B.h );	break;
	case SET_0_C:	M_SET( 0, BC.B.l );	break;
	case SET_0_D:	M_SET( 0, DE.B.h );	break;
	case SET_0_E:	M_SET( 0, DE.B.l );	break;
	case SET_0_H:	M_SET( 0, HL.B.h );	break;
	case SET_0_L:	M_SET( 0, HL.B.l );	break;
	case SET_0_xHL:	i = ReadMem( HL.W ); M_SET( 0, i ); WriteMem( HL.W, i );	break;
	case SET_0_A:	M_SET( 0, AF.B.h );	break;
	
	case SET_1_B:	M_SET( 1, BC.B.h );	break;
	case SET_1_C:	M_SET( 1, BC.B.l );	break;
	case SET_1_D:	M_SET( 1, DE.B.h );	break;
	case SET_1_E:	M_SET( 1, DE.B.l );	break;
	case SET_1_H:	M_SET( 1, HL.B.h );	break;
	case SET_1_L:	M_SET( 1, HL.B.l );	break;
	case SET_1_xHL:	i = ReadMem( HL.W ); M_SET( 1, i ); WriteMem( HL.W, i );	break;
	case SET_1_A:	M_SET( 1, AF.B.h );	break;
	
	case SET_2_B:	M_SET( 2, BC.B.h );	break;
	case SET_2_C:	M_SET( 2, BC.B.l );	break;
	case SET_2_D:	M_SET( 2, DE.B.h );	break;
	case SET_2_E:	M_SET( 2, DE.B.l );	break;
	case SET_2_H:	M_SET( 2, HL.B.h );	break;
	case SET_2_L:	M_SET( 2, HL.B.l );	break;
	case SET_2_xHL:	i = ReadMem( HL.W ); M_SET( 2, i ); WriteMem( HL.W, i );	break;
	case SET_2_A:	M_SET( 2, AF.B.h );	break;
	
	case SET_3_B:	M_SET( 3, BC.B.h );	break;
	case SET_3_C:	M_SET( 3, BC.B.l );	break;
	case SET_3_D:	M_SET( 3, DE.B.h );	break;
	case SET_3_E:	M_SET( 3, DE.B.l );	break;
	case SET_3_H:	M_SET( 3, HL.B.h );	break;
	case SET_3_L:	M_SET( 3, HL.B.l );	break;
	case SET_3_xHL:	i = ReadMem( HL.W ); M_SET( 3, i ); WriteMem( HL.W, i );	break;
	case SET_3_A:	M_SET( 3, AF.B.h );	break;
	
	case SET_4_B:	M_SET( 4, BC.B.h );	break;
	case SET_4_C:	M_SET( 4, BC.B.l );	break;
	case SET_4_D:	M_SET( 4, DE.B.h );	break;
	case SET_4_E:	M_SET( 4, DE.B.l );	break;
	case SET_4_H:	M_SET( 4, HL.B.h );	break;
	case SET_4_L:	M_SET( 4, HL.B.l );	break;
	case SET_4_xHL:	i = ReadMem( HL.W ); M_SET( 4, i ); WriteMem( HL.W, i );	break;
	case SET_4_A:	M_SET( 4, AF.B.h );	break;
	
	case SET_5_B:	M_SET( 5, BC.B.h );	break;
	case SET_5_C:	M_SET( 5, BC.B.l );	break;
	case SET_5_D:	M_SET( 5, DE.B.h );	break;
	case SET_5_E:	M_SET( 5, DE.B.l );	break;
	case SET_5_H:	M_SET( 5, HL.B.h );	break;
	case SET_5_L:	M_SET( 5, HL.B.l );	break;
	case SET_5_xHL:	i = ReadMem( HL.W ); M_SET( 5, i ); WriteMem( HL.W, i );	break;
	case SET_5_A:	M_SET( 5, AF.B.h );	break;
	
	case SET_6_B:	M_SET( 6, BC.B.h );	break;
	case SET_6_C:	M_SET( 6, BC.B.l );	break;
	case SET_6_D:	M_SET( 6, DE.B.h );	break;
	case SET_6_E:	M_SET( 6, DE.B.l );	break;
	case SET_6_H:	M_SET( 6, HL.B.h );	break;
	case SET_6_L:	M_SET( 6, HL.B.l );	break;
	case SET_6_xHL:	i = ReadMem( HL.W ); M_SET( 6, i ); WriteMem( HL.W, i );	break;
	case SET_6_A:	M_SET( 6, AF.B.h );	break;
	
	case SET_7_B:	M_SET( 7, BC.B.h );	break;
	case SET_7_C:	M_SET( 7, BC.B.l );	break;
	case SET_7_D:	M_SET( 7, DE.B.h );	break;
	case SET_7_E:	M_SET( 7, DE.B.l );	break;
	case SET_7_H:	M_SET( 7, HL.B.h );	break;
	case SET_7_L:	M_SET( 7, HL.B.l );	break;
	case SET_7_xHL:	i = ReadMem( HL.W ); M_SET( 7, i ); WriteMem( HL.W, i );	break;
	case SET_7_A:	M_SET( 7, AF.B.h );	break;
