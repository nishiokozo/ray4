//2019/11/22

#include <iostream>
#include <memory>
#include <vector>
#include <map>
#include <thread>
#include <chrono>
#include <cmath>
#include <functional>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iomanip>
#include <tuple>
#include <random>

#include "geom.h"

#include "SysSound.h"
#include "SysKeys.h"
#include "SysMouse.h"
#include "SysGra.h"

#include "func.h"
#include "pers.h"

#include "lab.h"
#include "lab32.h"

using namespace std;

struct Lab32::Impl
{
	bool	bResetAll = true;
	bool	bResetParam = true;
	bool	bPause = false;
	bool	bStep = false;

//	vector<shared_ptr<Obj>>	tbl_pObj;

};
Lab32::Lab32() : pImpl( new Lab32::Impl ){}

//------------------------------------------------------------------------------
void Lab32::Update( SysKeys& keys, SysMouse& mouse, SysSound& sound, SysGra& gra, Pers& pers, float delta, int& text_y, Cp& cp )
//------------------------------------------------------------------------------
{
	// 画面クリア
	gra.Clr(rgb(0.3,0.3,0.3));
	pers.grid.DrawGrid3d( gra, pers, vect3(0,0,0), mat33::mrotx(rad(90)), 10, 10, 1, rgb(0.2,0.2,0.2) );
	gra.Print(1,(float)text_y++,"32 : interpreter" ); 

	//初期化
	if ( pImpl->bResetAll )
	{
		pImpl->bResetAll = false;

		// カメラ
		pers.cam.pos = vect3( 0.0, 2.0, -5.0 );
		pers.cam.at = vect3( 0,  1.0, 0 );
		pers.cam.Update();


		string text;

		{
			fstream fi( "tst.cue", ios::in );
			string buf;
			while ( getline( fi, buf ) )
			{
				text += buf;
			}
			
		}
		cout << text << endl;
		cout << "--" << endl;

	
	
	
		enum Type
		{
			TYPE_NONE,
			TYPE_CTRL,	// /n /r	...
			TYPE_ALPHA,	// isalpha	abcd..
			TYPE_NUM,	// 1234..
			TYPE_SPACE,	// 0x20, \t, ..
			TYPE_CALC,	// ispunct	+,*,
			TYPE_STR,	// "..."
		};

				
		
		string token;
		Type	type = TYPE_NONE;
		
		for ( int i =0 ; i < (signed)text.length() ; i++ )
		{
			char c = text[i]; 

			auto func = []( char c )
			{
				Type	type = TYPE_NONE;
				     if ( c<=  8 )	type = TYPE_CTRL;
				else if ( c<=  9 )	type = TYPE_SPACE;
				else if ( c<= 31 )	type = TYPE_CTRL;
				else if ( c<= 32 )	type = TYPE_SPACE;
				else if ( c=='"' )	type = TYPE_STR;
				else if ( c<= 47 )	type = TYPE_CALC;
				else if ( c<= 57 )	type = TYPE_NUM;
				else if ( c<= 64 )	type = TYPE_CALC;
				else if ( c<= 90 )	type = TYPE_ALPHA;
				else if ( c<= 96 )	type = TYPE_CALC;
				else if ( c<=122 )	type = TYPE_ALPHA;
				else if ( c<=126 )	type = TYPE_CALC;
				else 				type = TYPE_CTRL;

				return type;
			};

			if ( token.length() == 0 )
			{
				type = func(c);
				if ( type == TYPE_STR ) continue;
				if ( type == TYPE_CTRL ) continue;
				if ( type == TYPE_SPACE ) continue;
				token = c;
				continue;
			}
			else
			{
				Type type2 = func(c);

				bool bAbandon = false;	// c 放棄フラグ
				bool bComp = false;		// トークン完成フラグ

				     if ( type2 == TYPE_STR )		{bComp = true;bAbandon = true;}	
				else if ( type2 == TYPE_CTRL )		{bComp = true;bAbandon = true;}
				else if ( type2 == TYPE_SPACE )		{bComp = true;bAbandon = true;}
				else if ( type2 == TYPE_CALC && type == TYPE_CALC )	{bComp = true;}
				else if ( type2 != type )			bComp = true;

				if ( bComp )
				{
					// トークン完成
					cout << type << ":" << token << endl;

					if ( bAbandon ) 
					{
						// 放棄
						token.clear();
						type = TYPE_NONE;
					}
					else
					{
						// 採用
						token = c;
						type = type2;
					}
				}
				else
				{
					// トークン継続
					token += c;
				}

			}
		}
	}
	

	if ( keys.R.hi ) pImpl->bResetAll = true;

}
