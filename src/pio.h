/////////////////////////////////////////////////////////////////////////////
//  P C 6 0 0 1 V
//  Copyright 1999,2022 Yumitaro
/////////////////////////////////////////////////////////////////////////////
#ifndef PIO_H_INCLUDED
#define PIO_H_INCLUDED

#include <memory>
#include <string>

#include "typedef.h"
#include "device.h"
#include "device/pd8255.h"

/////////////////////////////////////////////////////////////////////////////
// クラス定義
/////////////////////////////////////////////////////////////////////////////
class cPRT {
private:
	P6VPATH FilePath;					// PRINTERファイルフルパス
	
	BYTE pdata;							// パラレルポートから受け取ったデータ
	bool strb;							// ストローブ
	
public:
	cPRT();
	~cPRT();
	
	void SetFile( const P6VPATH& );		// プリンタ出力ファイル名設定
	void SetData( BYTE );				// 印刷するデータを受付
	void Strobe( bool );				// ストローブ受付
};




class PIO6 : public Device, public cD8255, public cPRT, public IDoko {
private:
	// 入出力処理関数
	void JobWriteA( BYTE ) override;
	void JobWriteB( BYTE ) override;
	void JobWriteC1( BYTE ) override;
	void JobWriteD( BYTE ) override;
	
	// I/Oアクセス関数
	void Out90H( int, BYTE );
	void Out91H( int, BYTE );
	void Out92H( int, BYTE );
	void Out93H( int, BYTE );
	BYTE In90H( int );
	BYTE In92H( int );
	BYTE In93H( int );
	
	void OutPBH( int, BYTE );
	BYTE InPBH( int );
	BYTE InIBF( int );
	BYTE InOBF( int );
	
public:
	PIO6( VM6*, const ID& );
	~PIO6();
	
	// デバイスID
	enum IDOut{ out90H=0, out91H, out92H, out93H, outPBH                 };
	enum IDIn {  in90H=0,          in92H,  in93H,  inPBH,  inIBF,  inOBF };
	
	// ----------------------------------------------------------------------
	bool DokoSave( cIni* ) override;	// どこでもSAVE
	bool DokoLoad( cIni* ) override;	// どこでもLOAD
	// ----------------------------------------------------------------------
};

#endif	// PIO_H_INCLUDED
