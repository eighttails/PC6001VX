/////////////////////////////////////////////////////////////////////////////
//  P C 6 0 0 1 V
//  Copyright 1999,2022 Yumitaro
/////////////////////////////////////////////////////////////////////////////
// Based on:
/////////////////////////////////////////////////////////////////////////////
/*
name is romaji.h

Copyright (c) 2020-2022 Windy
Version 2.0
This software is released under the MIT License.

https://github.com/windy6001/ROMAJI/

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/
/////////////////////////////////////////////////////////////////////////////


#ifndef _ROMAJI_H
#define _ROMAJI_H

#include <cstdint>
#include <map>
#include <string>

#define HENKAN_SUCCESS		1	// ローマ字変換成功
#define HENKAN_FAILED		0	// ローマ字変換失敗
#define HENKAN_DOING		-1	// ローマ字変換中
#define HENKAN_CANCEL		2	// ローマ字変換しない　& キャンセル



class Romaji{
protected:
	std::vector<P6KEYsym> buff;
	std::string outstr;
	
	bool isBoin( const P6KEYsym );
	bool isShiin( const P6KEYsym );
	void SetOutStr( const std::vector<uint8_t>& );	// 出力文字列設定

public:
	Romaji( void );
	~Romaji( void );
	
	int ConvertRomaji2kana( const P6KEYsym );	// ローマ字かな変換
	const std::string& GetResult( bool );		// ローマ字かな変換結果取得
};

#endif	// ROMAJI_H
