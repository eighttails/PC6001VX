#include <new>

#include "error.h"
#include "io.h"
#include "log.h"


// ---------------------------------------------------------------------------
//	IOBus : I/O空間を提供するクラス
//	  Original     : cisc
//	  Modification : Yumitaro
// ---------------------------------------------------------------------------
IOBus::DummyIO IOBus::dummyio;

IOBus::IOBus() : ins(NULL), outs(NULL), flags(NULL), devlist(NULL), banksize(0) {}

IOBus::~IOBus()
{
	if( ins ) delete [] ins;
	if( outs ) delete [] outs;
	if( flags ) delete [] flags;
}

////////////////////////////////////////////////////////////////
// 初期化
////////////////////////////////////////////////////////////////
bool IOBus::Init( DeviceList* dl, int bs )
{
	devlist  = dl;
	banksize = bs;
	
	// ご破算で願いましては
	if( ins ){   delete [] ins;   ins   = NULL; }
	if( outs ){  delete [] outs;  outs  = NULL; }
	if( flags ){ delete [] flags; flags = NULL; }
	
	// メモリ確保
	try{
		ins   = new InBank[banksize];
		outs  = new OutBank[banksize];
		flags = new BYTE[banksize];
	}
	catch( std::bad_alloc ){	// new に失敗した場合
		Error::SetError( Error::MemAllocFailed );
		if( ins ){   delete [] ins;   ins   = NULL; }
		if( outs ){  delete [] outs;  outs  = NULL; }
		return false;
	}
	
	ZeroMemory( flags, banksize );
	
	// とりあえず全てのポートにダミーデバイスを割り当てる
	for( int i=0; i<banksize; i++ ){
		ins[i].device  = &dummyio;
		ins[i].func    = STATIC_CAST( InFuncPtr, &DummyIO::dummyin );
		ins[i].next    = 0;
		outs[i].device = &dummyio;
		outs[i].func   = STATIC_CAST( OutFuncPtr, &DummyIO::dummyout );
		outs[i].next   = 0;
	}
	
	return true;
}


////////////////////////////////////////////////////////////////
// デバイス接続
////////////////////////////////////////////////////////////////
// IN/OUT -----------
bool IOBus::Connect( IDevice* device, const Connector* connector )
{
	if( devlist ) devlist->Add(device);
	
	const IDevice::Descriptor* desc = device->GetDesc();
	
	for( ; connector->rule; connector++ ){
		switch( connector->rule & 3 ){
		case portin:
			if( !ConnectIn(connector->bank, device, desc->indef[connector->id]) )
				return false;
			break;
			
		case portout:
			if( !ConnectOut(connector->bank, device, desc->outdef[connector->id]) )
				return false;
			break;
		}
	}
	return true;
}


// IN -----------
bool IOBus::ConnectIn( int bank, IDevice* device, InFuncPtr func )
{
	InBank* i = &ins[bank];
	if( i->func == STATIC_CAST( InFuncPtr, &DummyIO::dummyin ) ){
		// 最初の接続
		i->device = device;
		i->func   = func;
	}else{
		// 2回目以降の接続
		InBank *j;
		try{
			j = new InBank;
		}
		catch( std::bad_alloc ){	// new に失敗した場合
			Error::SetError( Error::MemAllocFailed );
			return false;
		}
		j->device = device;
		j->func   = func;
		j->next   = i->next;
		i->next   = j;
	}
	return true;
}


// OUT -----------
bool IOBus::ConnectOut( int bank, IDevice* device, OutFuncPtr func )
{
	OutBank* i = &outs[bank];
	if( i->func == STATIC_CAST( OutFuncPtr, &DummyIO::dummyout ) ){
		// 最初の接続
		i->device = device;
		i->func   = func;
	}else{
		// 2回目以降の接続
		OutBank *j;
		try{
			j = new OutBank;
		}
		catch( std::bad_alloc ){	// new に失敗した場合
			Error::SetError( Error::MemAllocFailed );
			return false;
		}
		j->device = device;
		j->func   = func;
		j->next   = i->next;
		i->next   = j;
	}
	return true;
}


////////////////////////////////////////////////////////////////
// デバイス切断
////////////////////////////////////////////////////////////////
bool IOBus::Disconnect( IDevice* device )
{
	if( devlist ) devlist->Del(device);
	
	// IN
  	for( int i=0; i<banksize; i++ ){
		InBank* current = &ins[i];
		InBank* referer = 0;
		while( current ){
			InBank* next = current->next;
			if( current->device == device ){
				if( referer ){
					referer->next = next;
					delete current;
				}else{
					// 削除するべきアイテムが最初にあった場合
					if( next ){
						// 次のアイテムの内容を複写して削除
						*current = *next;
						referer = 0;
						delete next;
						continue;
					}else{
						// このアイテムが唯一のアイテムだった場合
						current->func = STATIC_CAST( InFuncPtr, &DummyIO::dummyin );
					}
				}
			}
			current = next;
		}
	}
	
	// OUT
	for( int i=0; i<banksize; i++ ){
		OutBank* current = &outs[i];
		OutBank* referer = 0;
		while( current ){
			OutBank* next = current->next;
			if( current->device == device ){
				if( referer ){
					referer->next = next;
					delete current;
				}else{
					// 削除するべきアイテムが最初にあった場合
					if( next ){
						// 次のアイテムの内容を複写して削除
						*current = *next;
						referer = 0;
						delete next;
						continue;
					}else{
						// このアイテムが唯一のアイテムだった場合
						current->func = STATIC_CAST( OutFuncPtr, &DummyIO::dummyout );
					}
				}
			}
			current = next;
		}
	}
	return true;
}


////////////////////////////////////////////////////////////////
// IN関数
////////////////////////////////////////////////////////////////
BYTE IOBus::In( int port )
{
	InBank* list = &ins[port&0xff];
	
	BYTE data = 0xff;
	do{
		data &= (list->device->*list->func)( port );
		list = list->next;
	} while ( list );
	return data;
}


////////////////////////////////////////////////////////////////
// OUT関数
////////////////////////////////////////////////////////////////
void IOBus::Out( int port, BYTE data )
{
	OutBank* list = &outs[port&0xff];
	do{
		(list->device->*list->func)( port, data );
		list = list->next;
	} while ( list );
}


////////////////////////////////////////////////////////////////
// ダミーデバイス(IN)
////////////////////////////////////////////////////////////////
BYTE IOBus::DummyIO::dummyin( int )
{
	return 0xff;
}


////////////////////////////////////////////////////////////////
// ダミーデバイス(OUT)
////////////////////////////////////////////////////////////////
void IOBus::DummyIO::dummyout( int, BYTE )
{
	return;
}


////////////////////////////////////////////////////////////////
// コンストラクタ
////////////////////////////////////////////////////////////////
IO6::IO6( void ) : io(NULL), dl(NULL)
{
	INITARRAY( Iwait, 0 );
	INITARRAY( Owait, 0 );
}


////////////////////////////////////////////////////////////////
// デストラクタ
////////////////////////////////////////////////////////////////
IO6::~IO6( void )
{
	if( dl ) delete dl;
	if( io ) delete io;
}


////////////////////////////////////////////////////////////////
// 初期化
////////////////////////////////////////////////////////////////
bool IO6::Init( int banksize )
{
	// オブジェクト確保
	try{
		dl = new DeviceList;	// デバイスリスト
		io = new IOBus;			// I/O
		
		if( !io->Init( dl, banksize ) ) throw Error::InitFailed;
	}
	catch( std::bad_alloc ){	// new に失敗した場合
		Error::SetError( Error::MemAllocFailed );
		if( dl ){ delete dl; dl = NULL; }
		return false;
	}
	// 例外発生
	catch( Error::Errno i ){
		Error::SetError( i );
		return false;
	}
	
	for( int i=0; i<BANKSIZE; i++ ) Iwait[i] = Owait[i] = 0;
	
	return true;
}


////////////////////////////////////////////////////////////////
// デバイス接続
////////////////////////////////////////////////////////////////
bool IO6::Connect( IDevice* device, const IOBus::Connector* connector )
{
	return io->Connect( device, connector );
}


////////////////////////////////////////////////////////////////
// デバイス切断
////////////////////////////////////////////////////////////////
bool IO6::Disconnect( IDevice* device )
{
	return io->Disconnect( device );
}


////////////////////////////////////////////////////////////////
// IN関数
////////////////////////////////////////////////////////////////
BYTE IO6::In( int port, int *wcnt )
{
	PRINTD( IO_LOG, "[IO][In] port : %02X\n", port&0xff );
	
	if( wcnt ) (*wcnt) += Iwait[port&0xff];
	return io->In( port );
}


////////////////////////////////////////////////////////////////
// OUT関数
////////////////////////////////////////////////////////////////
void IO6::Out( int port, BYTE data, int *wcnt )
{
	PRINTD( IO_LOG, "[IO][Out] port : %02X  data : %02X\n", port&0xff, data );
	
	if( wcnt ) (*wcnt) += Owait[port&0xff];
	io->Out( port, data );
}


////////////////////////////////////////////////////////////////
// IN ウェイト設定
////////////////////////////////////////////////////////////////
void IO6::SetInWait( int port, int wait )
{
	Iwait[port&0xff] = wait;
}


////////////////////////////////////////////////////////////////
// OUTウェイト設定
////////////////////////////////////////////////////////////////
void IO6::SetOutWait( int port, int wait )
{
	Owait[port&0xff] = wait;
}


////////////////////////////////////////////////////////////////
// IN ウェイト取得
////////////////////////////////////////////////////////////////
int IO6::GetInWait( int port )
{
	return Iwait[port&0xff];
}


////////////////////////////////////////////////////////////////
// IN ウェイト取得
////////////////////////////////////////////////////////////////
int IO6::GetOutWait( int port )
{
	return Owait[port&0xff];
}
