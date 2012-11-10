#ifndef PIO_H_INCLUDED
#define PIO_H_INCLUDED

#include "typedef.h"
#include "device.h"
#include "p6device.h"
#include "device/pd8255.h"


////////////////////////////////////////////////////////////////
// クラス定義
////////////////////////////////////////////////////////////////
class cPRT {
private:
	char FilePath[PATH_MAX];		// PRINTERファイルフルパス
	
	FILE *fp;						// FILE ポインタ
	BYTE pdata;						// パラレルポートから受け取ったデータ
	
	bool strb;						// ストローブ
	
public:
	cPRT();							// コンストラクタ
	~cPRT();						// デストラクタ
	
	void Init( char * );			// 初期化
	void SetData( BYTE );			// 印刷するデータを受付
	void Strobe( bool );			// ストローブ受付
};




class PIO6 : public P6DEVICE, public Device, public cD8255, public cPRT, public IDoko {
private:
	// 入出力処理関数
	void JobWriteA( BYTE );
	void JobWriteB( BYTE );
	void JobWriteC1( BYTE );
	void JobWriteD( BYTE );
	
	// デバイス定義
	static const Descriptor descriptor;
	static const InFuncPtr  indef[];
	static const OutFuncPtr outdef[];
	const Descriptor* GetDesc() const { return &descriptor; }
	
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
	PIO6( VM6 *, const ID& );		// コンストラクタ
	~PIO6();						// デストラクタ
	
	// デバイスID
	enum IDOut{ out90H=0, out91H, out92H, out93H, outPBH                 };
	enum IDIn {  in90H=0,          in92H,  in93H,  inPBH,  inIBF,  inOBF };
	
	// ------------------------------------------
	bool DokoSave( cIni * );	// どこでもSAVE
	bool DokoLoad( cIni * );	// どこでもLOAD
	// ------------------------------------------
};

#endif	// PIO_H_INCLUDED
