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
#include "lab33.h"

using namespace std;

struct Lab33::Impl
{
	bool	bResetAll = true;
	bool	bResetParam = true;
	bool	bPause = false;
	bool	bStep = false;

//	vector<shared_ptr<Obj>>	tbl_pObj;

	struct TokenMQO
	{
		string text;

		enum Type
		{
			TYPE_NONE,	// 0:
			TYPE_ALPHA,	// 1:	isalpha	abcd..
			TYPE_NUM,	// 2:	1234..
			TYPE_SPACE,	// 3:	0x20, \t, ..
			TYPE_CALC,	// 4:	ispunct	+,*,
			TYPE_STR,	// 5:	"..."
		//	TYPE_CTRL,	// 6:	/n /r	...
		};

		string	word;
		Type	type;
		int		idx;
		bool	bComplete;

		//-----------------------------------------------------------------------------------------
		TokenMQO( string filename )
		//-----------------------------------------------------------------------------------------
		{
			fstream fi( filename, ios::in );
			string buf;
			while ( getline( fi, buf ) )
			{
				text += buf+" ";
			}

			idx	= 0;
			type = TYPE_NONE;
			word = "";
			bComplete = false;
		}
		
		//-----------------------------------------------------------------------------------------
		bool GetWord()
		//-----------------------------------------------------------------------------------------
		{
			if ( bComplete )
			{
				bComplete = false;
				type = TYPE_NONE;
				word = "";
			}
			
			while( idx < text.length() )
			{
				char c = text[idx++]; 

				auto func = []( char c )
				{
					Type	t = TYPE_NONE;
						 if ( c=='"' )				t = TYPE_STR;
					else if ( c==' ' || c=='\t')	t = TYPE_SPACE;
					else if ( c>='a' && c<='z')		t = TYPE_ALPHA;
					else if ( c>='A' && c<='Z')		t = TYPE_ALPHA;
					else if ( c>='0' && c<='9')		t = TYPE_NUM;
					else if ( c>='.')				t = TYPE_NUM;
					else if ( c>='-')				t = TYPE_NUM;
					else if ( c=='{' || c=='}') 	t = TYPE_CALC;
					else if ( c=='(' || c==')') 	t = TYPE_CALC;
					else t = TYPE_ALPHA;
					return t;
				};

				// トークンの始まり
				if ( type == TYPE_NONE )
				{
					Type t = func(c);
					if ( t==TYPE_SPACE ) {}
					else
					if ( t==TYPE_STR ) 
					{
						type=t;
					}
					else
					{
						type = t;
						word = c;
					}
				}
				else
				// 文字列トークン
				if ( type == TYPE_STR )
				{
					Type t = func(c);

					if ( t==TYPE_STR ) 
					{
						// トークン完成
						bComplete = true;
						break;
					}
					else
					{
						word += c;
					}
				}
				else
				// 通常トークン
				{
					Type t = func(c);
					if ( t==type )
					{
						word += c;
					}
					else
					{
						// トークン完成
						bComplete = true;
						idx--;
						break;
					}
				}
			}
		
			return ( idx < text.length() );
		}
	};

};
Lab33::Lab33() : pImpl( new Lab33::Impl ){}

//------------------------------------------------------------------------------
void Lab33::Update( SysKeys& keys, SysMouse& mouse, SysSound& sound, SysGra& gra, Pers& pers, float delta, int& text_y, Cp& cp )
//------------------------------------------------------------------------------
{
	// 画面クリア
	gra.Clr(rgb(0.3,0.3,0.3));
	pers.grid.DrawGrid3d( gra, pers, vect3(0,0,0), mat33::midentity(), 10, 10, 1, rgb(0.2,0.2,0.2) );
	gra.Print(1,(float)text_y++,"33 : .mqo reader" ); 

	//初期化
	if ( pImpl->bResetAll )
	{
		pImpl->bResetAll = false;

		// カメラ
		pers.cam.pos = vect3( 0.0, 2.0, -5.0 );
		pers.cam.at = vect3( 0,  1.0, 0 );
		pers.cam.Update();


		Impl::TokenMQO token( "tst.mqo" );
		
//		cout << text << endl;
		cout << "--" << endl;

		while( token.GetWord() )
		{
			cout << token.type << ":" << token.word << endl;
		}

	
	}
	

	if ( keys.R.hi ) pImpl->bResetAll = true;

}
