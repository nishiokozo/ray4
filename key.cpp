#include <iostream>
using namespace std;
#include <windows.h>
#include <stdio.h>
#include "key.h"

static	BYTE g_state[256];

KEY_INF	key;

static	int	g_lim_on;

//-----------------------------------------------------------------------------
int key_init()
//-----------------------------------------------------------------------------
{
	memset( &key, 0, sizeof(key)) ;
	return	true;
}
//-----------------------------------------------------------------------------
int	key_update()
//-----------------------------------------------------------------------------
{


//最上位ビットが 1 のときはキーが押されていることを、0 のときはキーが押されていないことを示します。最下位ビットが 1 のときはキーがトグル状態にあることを、0 のときはトグルが解除されていることを示します。たとえば、CapsLock キーが ON になっているときは、トグル状態になります。
	GetKeyboardState( g_state );

	for ( int i = 0 ; i < 255 ; i++ )
	{
		bool prev	= key.tbl_on[i];
		bool on		= ((g_state[i] & 0x80)!=0);
		bool hi		= ( on && !prev);
		bool lo		= (!on &&  prev);
		bool rep	= hi;
		int	lim	= key.tbl_lim[i];

		if ( hi )
		{
			lim=16;
		}
		if ( lim-- <= 0 && on )
		{
			lim = 4;
			rep = true;
		}

		key.tbl_prev[i]	= prev;
		key.tbl_on[i]	= on;
		key.tbl_hi[i]	= hi;
		key.tbl_lo[i]	= lo;
		key.tbl_rep[i]	= rep;
		key.tbl_lim[i]	= lim;
	}




#if 0
	// 標準で使えるキー（US配列、日本語配列、コンパクトキーボード等、大抵のキーボードで対応しているキー
	if ( key.on.esc )	cout <<  "ok" << endl;
	if ( key.on.tab )	cout <<  "ok" << endl;
	if ( key.on.capslock )	cout <<  "ok" << endl;
	if ( key.on.shift )	cout <<  "ok" << endl;
	if ( key.on.ctrl )	cout <<  "ok" << endl;
	if ( key.on.alt )	cout <<  "ok" << endl;
	if ( key.on.up )	cout <<  "ok" << endl;
	if ( key.on.down )	cout <<  "ok" << endl;
	if ( key.on.left )	cout <<  "ok" << endl;
	if ( key.on.right )	cout <<  "ok" << endl;
	if ( key.on.pageup )	cout <<  "ok" << endl;
	if ( key.on.pagedown )	cout <<  "ok" << endl;
	if ( key.on.home )	cout <<  "ok" << endl;
	if ( key.on.end )	cout <<  "ok" << endl;
	if ( key.on.comma )	cout <<  "ok" << endl;
	if ( key.on.period )	cout <<  "ok" << endl;
	if ( key.on.semicolon )	cout <<  "ok" << endl;
	if ( key.on.space )	cout <<  "ok" << endl;
	if ( key.on.backspace )	cout <<  "ok" << endl;
	if ( key.on.enter )	cout <<  "ok" << endl;
	if ( key.on.f1 )	cout <<  "ok" << endl;
	if ( key.on.f2 )	cout <<  "ok" << endl;
	if ( key.on.f3 )	cout <<  "ok" << endl;
	if ( key.on.f4 )	cout <<  "ok" << endl;
	if ( key.on.f5 )	cout <<  "ok" << endl;
	if ( key.on.f6 )	cout <<  "ok" << endl;
	if ( key.on.f7 )	cout <<  "ok" << endl;
	if ( key.on.f8 )	cout <<  "ok" << endl;
	if ( key.on.f9 )	cout <<  "ok" << endl;
	if ( key.on.f10 )	cout <<  "ok" << endl;
	if ( key.on.a )	cout <<  "ok" << endl;
	if ( key.on.b )	cout <<  "ok" << endl;
	if ( key.on.c )	cout <<  "ok" << endl;
	if ( key.on.d )	cout <<  "ok" << endl;
	if ( key.on.e )	cout <<  "ok" << endl;
	if ( key.on.f )	cout <<  "ok" << endl;
	if ( key.on.g )	cout <<  "ok" << endl;
	if ( key.on.h )	cout <<  "ok" << endl;
	if ( key.on.i )	cout <<  "ok" << endl;
	if ( key.on.j )	cout <<  "ok" << endl;
	if ( key.on.k )	cout <<  "ok" << endl;
	if ( key.on.l )	cout <<  "ok" << endl;
	if ( key.on.m )	cout <<  "ok" << endl;
	if ( key.on.n )	cout <<  "ok" << endl;
	if ( key.on.o )	cout <<  "ok" << endl;
	if ( key.on.p )	cout <<  "ok" << endl;
	if ( key.on.q )	cout <<  "ok" << endl;
	if ( key.on.r )	cout <<  "ok" << endl;
	if ( key.on.s )	cout <<  "ok" << endl;
	if ( key.on.t )	cout <<  "ok" << endl;
	if ( key.on.u )	cout <<  "ok" << endl;
	if ( key.on.v )	cout <<  "ok" << endl;
	if ( key.on.w )	cout <<  "ok" << endl;
	if ( key.on.x )	cout <<  "ok" << endl;
	if ( key.on.y )	cout <<  "ok" << endl;
	if ( key.on.z )	cout <<  "ok" << endl;
#endif

#if 0
int i;
for ( i = 0 ; i < 255 ; i++ )
{
	if ( g_state[i] & 0x80 )
	{
		if ( 
			i!=0x19 &&
			i!=0xf0 &&
			i!=0xf3 &&
			i!=0xf6 &&
			i!=0xfb
		)
		cout << hex << "key hi " << i << endl;
	}
}
#endif
#if 0
int i;
for ( i = 0 ; i < 255 ; i++ )
{
	if ( g_state[i] & 0x01 )
	{
		if ( 1
			&& i!=0x1
			&& i!=0x2
			&& i!=0x5
			&& i!=0x8
			&& i!=0x9
			&& i!=0xd
			&& i!=0x10
			&& i!=0x11
			&& i!=0x1b
			&& i!=0x24
			&& i!=0xa0
			&& i!=0xa2
			&& i!=0xf0
			&& i!=0xf3
			&& i!=0xf6
			&& i!=0xfb

		)
		cout << hex << "key toggle " << i << endl;
	}
}
#endif

	return	true;
}
