#ifndef LOG_H_INCLUDED
#define LOG_H_INCLUDED


/////////////////////////////////////////////////////////////////////////////
// デバッグログ関係
/////////////////////////////////////////////////////////////////////////////
// ログを取る場合1にする
#define KEY_LOG		0	/* キー入力関係 */
#define MEM_LOG		0	/* メモリ関係 */
#define TAPE_LOG	0	/* TAPE関係 */
#define P6T2_LOG	0	/* P6T2関係 */
#define DISK_LOG	0	/* DISK関係 */
#define FDC_LOG		0	/* FDC関係 */
#define D88_LOG		0	/* D88関係 */
#define PSG_LOG		0	/* PSG関係 */
#define INTR_LOG	0	/* 割込み関係 */
#define CPU_LOG		0	/* CPU関係 */
#define SUB_LOG		0	/* サブCPU関係 */
#define PPI_LOG		0	/* 8255関係 */
#define SND_LOG		0	/* Sound関係 */
#define GRP_LOG		0	/* 描画関係 */
#define OSD_LOG		0	/* OS依存関係 */
#define TIC_LOG		0	/* スケジューラ関係 */
#define INI_LOG		0	/* INI関係 */
#define CON_LOG		0	/* コンストラクタ関係 */
#define IO_LOG		0	/* I/O関係 */
#define WIN_LOG		0	/* ウィンドウ関係 */
#define CONST_LOG	0	/* コンストラクタ・デストラクタ関係 */
#define VOI_LOG		0	/* 音声合成関係 */

#if KEY_LOG || MEM_LOG || TAPE_LOG || P6T2_LOG || DISK_LOG || D88_LOG || FDC_LOG || PSG_LOG || INTR_LOG || CPU_LOG || SUB_LOG || PPI_LOG || SND_LOG || GRP_LOG || OSD_LOG || TIC_LOG || INI_LOG || CON_LOG || IO_LOG || WIN_LOG || CONST_LOG || VOI_LOG

#include <stdio.h>
#define PRINTD(m,s)				{ if( m ){ fprintf( stdout, (s) );							fflush( stdout ); } }
#define PRINTD1(m,s,a)			{ if( m ){ fprintf( stdout, (s), (a) );						fflush( stdout ); } }
#define PRINTD2(m,s,a,b)		{ if( m ){ fprintf( stdout, (s), (a), (b) );				fflush( stdout ); } }
#define PRINTD3(m,s,a,b,c)		{ if( m ){ fprintf( stdout, (s), (a), (b), (c) );			fflush( stdout ); } }
#define PRINTD4(m,s,a,b,c,d)	{ if( m ){ fprintf( stdout, (s), (a), (b), (c), (d) );		fflush( stdout ); } }
#define PRINTD5(m,s,a,b,c,d,e)	{ if( m ){ fprintf( stdout, (s), (a), (b), (c), (d), (e) );	fflush( stdout ); } }

#else

#define PRINTD(m,s)
#define PRINTD1(m,s,a)
#define PRINTD2(m,s,a,b)
#define PRINTD3(m,s,a,b,c)
#define PRINTD4(m,s,a,b,c,d)
#define PRINTD5(m,s,a,b,c,d,e)

#endif


#endif	// LOG_H_INCLUDED
