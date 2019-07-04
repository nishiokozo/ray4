#include <iostream>
using namespace std;
#include <windows.h>
#include <stdio.h>
#include "key.h"

static	BYTE g_state[256];

static	bool g_tbl_prev[256];
static	bool g_tbl_on[256];
static	bool g_tbl_hi[256];
static	bool g_tbl_lo[256];
static	bool g_tbl_rep[256];
static	int	g_tbl_lim[256];

KEY_INF	key;

//static KEY_INF	g_key;
/*
//-----------------------------------------------------------------------------
KEY_INF& key_getInstance()
//-----------------------------------------------------------------------------
{
		cout << "key_getInstance()" << endl;
	return	(*this);
}
*/

//-----------------------------------------------------------------------------
KEY_INF& KEY_INF::key_getInstance()
//-----------------------------------------------------------------------------
{
		cout << "key_getInstance()" << endl;
	return	(*this);
}

/*
//-----------------------------------------------------------------------------
int key_init()
//-----------------------------------------------------------------------------
{
		cout << "key_init()" << endl;
	memset( &g_key, 0, sizeof((*this))) ;
	return	true;
}
*/
//-----------------------------------------------------------------------------
KEY_INF::KEY_INF()
//-----------------------------------------------------------------------------
{
	cout << "KEY_INF()" << endl;
}

//-----------------------------------------------------------------------------
void KEY_INF::Update()
//-----------------------------------------------------------------------------
{


//最上位ビットが 1 のときはキーが押されていることを、0 のときはキーが押されていないことを示します。最下位ビットが 1 のときはキーがトグル状態にあることを、0 のときはトグルが解除されていることを示します。たとえば、CapsLock キーが ON になっているときは、トグル状態になります。
	GetKeyboardState( g_state );

	for ( int i = 0 ; i < 255 ; i++ )
	{
		bool prev	= g_tbl_on[i];
		bool on		= ((g_state[i] & 0x80)!=0);
		bool hi		= ( on && !prev);
		bool lo		= (!on &&  prev);
		bool rep	= hi;
		int	lim	= g_tbl_lim[i];

		if ( hi )
		{
			lim=16;
		}
		if ( --lim <= 0 && on )
		{
			lim = 4;
			rep = true;
		}

		g_tbl_prev[i]	= prev;
		g_tbl_on[i]	= on;
		g_tbl_hi[i]	= hi;
		g_tbl_lo[i]	= lo;
		g_tbl_rep[i]	= rep;
		g_tbl_lim[i]	= lim;
	}

	// Windows-APIの仕様を上に挙げないように一つ一つアサインする

/*
	bool	_00;				// 00 (reserved)
	bool	_01;				// 01 VK_LBUTTON
	bool	_02;				// 02 VK_RBUTTON
	bool	_03;				// 03 VK_CANCEL
	bool	_04;				// 04 VK_MBUTTON
	bool	_05;				// 05 VK_XBUTTON1
	bool	_06;				// 06 VK_XBUTTON2
	bool	_07;				// 07 (reserved)
	bool	backspace;			// 08 VK_BACK
	bool	tab;				// 09 VK_TAB
	bool	_0A;				// 0A (reserved)
	bool	_0B;				// 0B (reserved)
	bool	_0C;				// 0C VK_CLEAR
	bool	enter;				// 0D VK_RETURN
	bool	_0E;				// 0E (reserved)
	bool	_0F;				// 0F (reserved)
	bool	shift;				// 10 VK_SHIFT
	bool	ctrl;				// 11 VK_CONTROL
	bool	alt;				// 12 VK_MENU
	bool	_13;				// 13 VK_PAUSE
	bool	capslock;			// 14 VK_CAPITAL
	bool	_15;				// 15 VK_KANA
	bool	_16;				// 16 (reserved)
	bool	_17;				// 17 VK_JUNJA
	bool	_18;				// 18 VK_FINAL
	bool	_19;				// 19 VK_KANJI 何故か常時0x80が入る
	bool	_1A;				// 1A (reserved)
	bool	esc;				// 1B VK_ESCAPE
	bool	_1C;				// 1C VK_CONVERT
	bool	_1D;				// 1D VK_NONCONVERT
	bool	_1E;				// 1E VK_ACCEPT
	bool	_1F;				// 1F VK_MODECHANGE
	bool	space;				// 20 VK_SPACE
	bool	pageup;				// 21 VK_PRIOR
	bool	pagedown;			// 22 VK_NEXT
	bool	end;				// 23 VK_END
	bool	home;				// 24 VK_HOME
	bool	left;				// 25 VK_LEFT
	bool	up;					// 26 VK_UP
	bool	right;				// 27 VK_RIGHT
	bool	down;				// 28 VK_DOWN
	bool	_29;				// 29 VK_SELECT
	bool	_2A;				// 2A VK_PRINT
	bool	_2B;				// 2B VK_EXECUTE
	bool	_2C;				// 2C VK_SNAPSHOT
	bool	_2D;				// 2D VK_INSERT
	bool	_2E;				// 2E VK_DELETE
	bool	_2F;				// 2F VK_HELP
	bool	_0;					// 30 "0"
	bool	_1;					// 31 "1"
	bool	_2;					// 32 "2"
	bool	_3;					// 33 "3"
	bool	_4;					// 34 "4"
	bool	_5;					// 35 "5"
	bool	_6;					// 36 "6"
	bool	_7;					// 37 "7"
	bool	_8;					// 38 "8"
	bool	_9;					// 39 "9"
	bool	_3A;				// 3A VK_COLON
	bool	_3B;				// 3B VK_SEMICOLON
	bool	_3C;				// 3C VK_COMMMA
	bool	_3D;				// 3D VK_PERIOD
	bool	_3E;				// 3E (reserved)
	bool	_3F;				// 3F (reserved)
	bool	_40;				// 40 (reserved)
	bool	a;					// 41 'A'
	bool	b;					// 42 'B'
	bool	c;					// 43 'C'
	bool	d;					// 44 'D'
	bool	e;					// 45 'E'
	bool	f;					// 46 'F'
	bool	g;					// 47 'G'
	bool	h;					// 48 'H'
	bool	i;					// 49 'I'
	bool	j;					// 4A 'J'
	bool	k;					// 4B 'K'
	bool	l;					// 4C 'L'
	bool	m;					// 4D 'M'
	bool	n;					// 4E 'N'
	bool	o;					// 4F 'O'
	bool	p;					// 50 'P'
	bool	q;					// 51 'Q'
	bool	r;					// 52 'R'
	bool	s;					// 53 'S'
	bool	t;					// 54 'T'
	bool	u;					// 55 'U'
	bool	v;					// 56 'V'
	bool	w;					// 57 'W'
	bool	x;					// 58 'X'
	bool	y;					// 59 'Y'
	bool	z;					// 5A 'Z'
	bool	_5B;				// 5B VK_LWIN
	bool	_5C;				// 5C VK_RWIN
	bool	_5D;				// 5D VK_APPS
	bool	_5E;				// 5E (reserved)
	bool	_5F;				// 5F VK_SLEEP
	bool	_60;				// 60 VK_NUMPAD0
	bool	_61;				// 61 VK_NUMPAD1
	bool	_62;				// 62 VK_NUMPAD2
	bool	_63;				// 63 VK_NUMPAD3
	bool	_64;				// 64 VK_NUMPAD4
	bool	_65;				// 65 VK_NUMPAD5
	bool	_66;				// 66 VK_NUMPAD6
	bool	_67;				// 67 VK_NUMPAD7
	bool	_68;				// 68 VK_NUMPAD8
	bool	_69;				// 69 VK_NUMPAD9
	bool	_6A;				// 6A VK_MULTIPLY
	bool	_6B;				// 6B VK_ADD
	bool	_6C;				// 6C VK_SEPARATOR
	bool	_6D;				// 6D VK_SUBTRACT
	bool	_6E;				// 6E VK_DECIMAL
	bool	_6F;				// 6F VK_DIVIDE
	bool	f1;					// 70 VK_F1
	bool	f2;					// 71 VK_F2
	bool	f3;					// 72 VK_F3
	bool	f4;					// 73 VK_F4
	bool	f5;					// 74 VK_F5
	bool	f6;					// 75 VK_F6
	bool	f7;					// 76 VK_F7
	bool	f8;					// 77 VK_F8
	bool	f9;					// 78 VK_F9
	bool	f10;				// 79 VK_F10
	bool	_7A;				// 7A VK_F11
	bool	_7B;				// 7B VK_F12
	bool	_7C;				// 7C VK_F13
	bool	_7D;				// 7D VK_F14
	bool	_7E;				// 7E VK_F15
	bool	_7F;				// 7F VK_F16
	bool	_80;				// 80 VK_F17
	bool	_81;				// 81 VK_F18
	bool	_82;				// 82 VK_F19
	bool	_83;				// 83 VK_F20
	bool	_84;				// 84 VK_F21
	bool	_85;				// 85 VK_F22
	bool	_86;				// 86 VK_F23
	bool	_87;				// 87 VK_F24
	bool	_88;				// 88 (reserved)
	bool	_89;				// 89 (reserved)
	bool	_8A;				// 8A (reserved)
	bool	_8B;				// 8B (reserved)
	bool	_8C;				// 8C (reserved)
	bool	_8D;				// 8D (reserved)
	bool	_8E;				// 8E (reserved)
	bool	_8F;				// 8F (reserved)
	bool	_90;				// 90 VK_NUMLOCK
	bool	_91;				// 91 VK_SCROLL
	bool	_92;				// 92 VK_OEM_FJ_JISHO
	bool	_93;				// 93 VK_OEM_FJ_MASSHOU
	bool	_94;				// 94 VK_OEM_FJ_TOUROKU
	bool	_95;				// 95 VK_OEM_FJ_LOYA
	bool	_96;				// 96 VK_OEM_FJ_ROYA
	bool	_97;				// 97 (reserved)
	bool	_98;				// 98 (reserved)
	bool	_99;				// 99 (reserved)
	bool	_9A;				// 9A (reserved)
	bool	_9B;				// 9B (reserved)
	bool	_9C;				// 9C (reserved)
	bool	_9D;				// 9D (reserved)
	bool	_9E;				// 9E (reserved)
	bool	_9F;				// 9F (reserved)
	bool	_A0;				// A0 VK_LSHIFT
	bool	_A1;				// A1 VK_RSHIFT
	bool	_A2;				// A2 VK_LCONTROL
	bool	_A3;				// A3 VK_RCONTROL
	bool	_A4;				// A4 VK_LMENU
	bool	_A5;				// A5 VK_RMENU
	bool	_A6;				// A6 VK_BROWSER_BACK
	bool	_A7;				// A7 VK_BROWSER_FORWARD
	bool	_A8;				// A8 VK_BROWSER_REFRESH
	bool	_A9;				// A9 VK_BROWSER_STOP
	bool	_AA;				// AA VK_BROWSER_SEARCH
	bool	_AB;				// AB VK_BROWSER_FAVORITES
	bool	_AC;				// AC VK_BROWSER_HOME
	bool	_AD;				// AD VK_VOLUME_MUTE
	bool	_AE;				// AE VK_VOLUME_DOWN
	bool	_AF;				// AF VK_VOLUME_UP
	bool	_B0;				// B0 VK_MEDIA_NEXT_TRACK
	bool	_B1;				// B1 VK_MEDIA_PREV_TRACK
	bool	_B2;				// B2 VK_MEDIA_STOP
	bool	_B3;				// B3 VK_MEDIA_PLAY_PAUSE
	bool	_B4;				// B4 VK_LAUNCH_MAIL
	bool	_B5;				// B5 VK_LAUNCH_MEDIA_SELECT
	bool	_B6;				// B6 VK_LAUNCH_APP1
	bool	_B7;				// B7 VK_LAUNCH_APP2
	bool	_B8;				// B8 (reserved)
	bool	_B9;				// B9 (reserved)
	bool	semicolon;			// BA VK_OEM_1
	bool	_BB;				// BB VK_OEM_PLUS
	bool	comma;				// BC VK_OEM_COMMA
	bool	_BD;				// BD VK_OEM_MINUS
	bool	period;				// BE VK_OEM_PERIOD
	bool	_BF;				// BF VK_OEM_2
	bool	_C0;				// C0 VK_OEM_3
	bool	_C1;				// C1 (reserved)
	bool	_C2;				// C2 (reserved)
	bool	_C3;				// C3 (reserved)
	bool	_C4;				// C4 (reserved)
	bool	_C5;				// C5 (reserved)
	bool	_C6;				// C6 (reserved)
	bool	_C7;				// C7 (reserved)
	bool	_C8;				// C8 (reserved)
	bool	_C9;				// C9 (reserved)
	bool	_CA;				// CA (reserved)
	bool	_CB;				// CB (reserved)
	bool	_CC;				// CC (reserved)
	bool	_CD;				// CD (reserved)
	bool	_CE;				// CE (reserved)
	bool	_CF;				// CF (reserved)
	bool	_D0;				// D0 (reserved)
	bool	_D1;				// D1 (reserved)
	bool	_D2;				// D2 (reserved)
	bool	_D3;				// D3 (reserved)
	bool	_D4;				// D4 (reserved)
	bool	_D5;				// D5 (reserved)
	bool	_D6;				// D6 (reserved)
	bool	_D7;				// D7 (reserved)
	bool	_D8;				// D8 (reserved)
	bool	_D9;				// D9 (reserved)
	bool	_DA;				// DA (reserved)
	bool	_DB;				// DB VK_OEM_4
	bool	_DC;				// DC VK_OEM_5
	bool	_DD;				// DD VK_OEM_6
	bool	_DE;				// DE VK_OEM_7
	bool	_DF;				// DF VK_OEM_8
	bool	_E0;				// E0 (reserved)
	bool	_E1;				// E1 VK_OEM_AX
	bool	_E2;				// E2 VK_OEM_102
	bool	_E3;				// E3 VK_ICO_HELP
	bool	_E4;				// E4 VK_ICO_00
	bool	_E5;				// E5 VK_PROCESSKEY
	bool	_E6;				// E6 VK_ICO_CLEAR
	bool	_E7;				// E7 VK_PACKET
	bool	_E8;				// E8 (reserved)
	bool	_E9;				// E9 VK_OEM_RESET
	bool	_EA;				// EA VK_OEM_JUMP
	bool	_EB;				// EB VK_OEM_PA1
	bool	_EC;				// EC VK_OEM_PA2
	bool	_ED;				// ED VK_OEM_PA3
	bool	_EE;				// EE VK_OEM_WSCTRL
	bool	_EF;				// EF VK_OEM_CUSEL
	bool	_F0;				// F0 VK_OEM_ATTN 何故か常時0x80が入る
	bool	_F1;				// F1 VK_OEM_FINISH
	bool	_F2;				// F2 VK_OEM_COPY
	bool	_F3;				// F3 VK_OEM_AUTO 何故か常時0x80が入る
	bool	_F4;				// F4 VK_OEM_ENLW
	bool	_F5;				// F5 VK_OEM_BACKTAB
	bool	_F6;				// F6 VK_ATTN 何故か常時0x80が入る
	bool	_F7;				// F7 VK_CRSEL
	bool	_F8;				// F8 VK_EXSEL
	bool	_F9;				// F9 VK_EREOF
	bool	_FA;				// FA VK_PLAY
	bool	_FB;				// FB VK_ZOOM 何故か常時0x80が入る
	bool	_FC;				// FC VK_NONAME
	bool	_FD;				// FD VK_PA1
	bool	_FE;				// FE VK_OEM_CLEAR
	bool	_FF;				// FF (reserved)
*/
	
	this->on.backspace	= g_tbl_on[ VK_BACK ];			// 08
	this->on.tab		= g_tbl_on[ VK_TAB ];			// 09
	this->on.enter		= g_tbl_on[ VK_RETURN ];		// 0D
	this->on.shift		= g_tbl_on[ VK_SHIFT ];			// 10
	this->on.ctrl		= g_tbl_on[ VK_CONTROL ];		// 11
	this->on.alt		= g_tbl_on[ VK_MENU ];			// 12
	this->on.capslock	= g_tbl_on[ VK_CAPITAL ];		// 14
	this->on.esc		= g_tbl_on[ VK_ESCAPE ];		// 1B
	this->on.space		= g_tbl_on[ VK_SPACE ];			// 20
	this->on.pageup		= g_tbl_on[ VK_PRIOR ];			// 21
	this->on.pagedown	= g_tbl_on[ VK_NEXT ];			// 22
	this->on.end		= g_tbl_on[ VK_END ];			// 23
	this->on.home		= g_tbl_on[ VK_HOME ];			// 24
	this->on.left		= g_tbl_on[ VK_LEFT ];			// 25
	this->on.up			= g_tbl_on[ VK_UP ];			// 26
	this->on.right		= g_tbl_on[ VK_RIGHT ];			// 27
	this->on.down		= g_tbl_on[ VK_DOWN ];			// 28
	this->on.a			= g_tbl_on[ (int)'A'];			// 41
	this->on.b			= g_tbl_on[ (int)'B'];			// 42
	this->on.c			= g_tbl_on[ (int)'C'];			// 43
	this->on.d			= g_tbl_on[ (int)'D'];			// 44
	this->on.e			= g_tbl_on[ (int)'E'];			// 45
	this->on.f			= g_tbl_on[ (int)'F'];			// 46
	this->on.g			= g_tbl_on[ (int)'G'];			// 47
	this->on.h			= g_tbl_on[ (int)'H'];			// 48
	this->on.i			= g_tbl_on[ (int)'I'];			// 49
	this->on.j			= g_tbl_on[ (int)'J'];			// 4A
	this->on.k			= g_tbl_on[ (int)'K'];			// 4B
	this->on.l			= g_tbl_on[ (int)'L'];			// 4C
	this->on.m			= g_tbl_on[ (int)'M'];			// 4D
	this->on.n			= g_tbl_on[ (int)'N'];			// 4E
	this->on.o			= g_tbl_on[ (int)'O'];			// 4F
	this->on.p			= g_tbl_on[ (int)'P'];			// 50
	this->on.q			= g_tbl_on[ (int)'Q'];			// 51
	this->on.r			= g_tbl_on[ (int)'R'];			// 52
	this->on.s			= g_tbl_on[ (int)'S'];			// 53
	this->on.t			= g_tbl_on[ (int)'T'];			// 54
	this->on.u			= g_tbl_on[ (int)'U'];			// 55
	this->on.v			= g_tbl_on[ (int)'V'];			// 56
	this->on.w			= g_tbl_on[ (int)'W'];			// 57
	this->on.x			= g_tbl_on[ (int)'X'];			// 58
	this->on.y			= g_tbl_on[ (int)'Y'];			// 59
	this->on.z			= g_tbl_on[ (int)'Z'];			// 5A
	this->on.f1			= g_tbl_on[ VK_F1 ];			// 70
	this->on.f2			= g_tbl_on[ VK_F2 ];			// 71
	this->on.f3			= g_tbl_on[ VK_F3 ];			// 72
	this->on.f4			= g_tbl_on[ VK_F4 ];			// 73
	this->on.f5			= g_tbl_on[ VK_F5 ];			// 74
	this->on.f6			= g_tbl_on[ VK_F6 ];			// 75
	this->on.f7			= g_tbl_on[ VK_F7 ];			// 76
	this->on.f8			= g_tbl_on[ VK_F8 ];			// 77
	this->on.f9			= g_tbl_on[ VK_F9 ];			// 78
	this->on.f10		= g_tbl_on[ VK_F10 ];			// 79
	this->on.semicolon	= g_tbl_on[ VK_OEM_1 ];			// BA
	this->on.comma		= g_tbl_on[ VK_OEM_COMMA ];		// BC
	this->on.period		= g_tbl_on[ VK_OEM_PERIOD ];	// BE


	this->hi.backspace	= g_tbl_hi[ VK_BACK ];			// 08
	this->hi.tab		= g_tbl_hi[ VK_TAB ];			// 09
	this->hi.enter		= g_tbl_hi[ VK_RETURN ];		// 0D
	this->hi.shift		= g_tbl_hi[ VK_SHIFT ];			// 10
	this->hi.ctrl		= g_tbl_hi[ VK_CONTROL ];		// 11
	this->hi.alt		= g_tbl_hi[ VK_MENU ];			// 12
	this->hi.capslock	= g_tbl_hi[ VK_CAPITAL ];		// 14
	this->hi.esc		= g_tbl_hi[ VK_ESCAPE ];		// 1B
	this->hi.space		= g_tbl_hi[ VK_SPACE ];			// 20
	this->hi.pageup		= g_tbl_hi[ VK_PRIOR ];			// 21
	this->hi.pagedown	= g_tbl_hi[ VK_NEXT ];			// 22
	this->hi.end		= g_tbl_hi[ VK_END ];			// 23
	this->hi.home		= g_tbl_hi[ VK_HOME ];			// 24
	this->hi.left		= g_tbl_hi[ VK_LEFT ];			// 25
	this->hi.up			= g_tbl_hi[ VK_UP ];			// 26
	this->hi.right		= g_tbl_hi[ VK_RIGHT ];			// 27
	this->hi.down		= g_tbl_hi[ VK_DOWN ];			// 28
	this->hi.a			= g_tbl_hi[ (int)'A'];			// 41
	this->hi.b			= g_tbl_hi[ (int)'B'];			// 42
	this->hi.c			= g_tbl_hi[ (int)'C'];			// 43
	this->hi.d			= g_tbl_hi[ (int)'D'];			// 44
	this->hi.e			= g_tbl_hi[ (int)'E'];			// 45
	this->hi.f			= g_tbl_hi[ (int)'F'];			// 46
	this->hi.g			= g_tbl_hi[ (int)'G'];			// 47
	this->hi.h			= g_tbl_hi[ (int)'H'];			// 48
	this->hi.i			= g_tbl_hi[ (int)'I'];			// 49
	this->hi.j			= g_tbl_hi[ (int)'J'];			// 4A
	this->hi.k			= g_tbl_hi[ (int)'K'];			// 4B
	this->hi.l			= g_tbl_hi[ (int)'L'];			// 4C
	this->hi.m			= g_tbl_hi[ (int)'M'];			// 4D
	this->hi.n			= g_tbl_hi[ (int)'N'];			// 4E
	this->hi.o			= g_tbl_hi[ (int)'O'];			// 4F
	this->hi.p			= g_tbl_hi[ (int)'P'];			// 50
	this->hi.q			= g_tbl_hi[ (int)'Q'];			// 51
	this->hi.r			= g_tbl_hi[ (int)'R'];			// 52
	this->hi.s			= g_tbl_hi[ (int)'S'];			// 53
	this->hi.t			= g_tbl_hi[ (int)'T'];			// 54
	this->hi.u			= g_tbl_hi[ (int)'U'];			// 55
	this->hi.v			= g_tbl_hi[ (int)'V'];			// 56
	this->hi.w			= g_tbl_hi[ (int)'W'];			// 57
	this->hi.x			= g_tbl_hi[ (int)'X'];			// 58
	this->hi.y			= g_tbl_hi[ (int)'Y'];			// 59
	this->hi.z			= g_tbl_hi[ (int)'Z'];			// 5A
	this->hi.f1			= g_tbl_hi[ VK_F1 ];			// 70
	this->hi.f2			= g_tbl_hi[ VK_F2 ];			// 71
	this->hi.f3			= g_tbl_hi[ VK_F3 ];			// 72
	this->hi.f4			= g_tbl_hi[ VK_F4 ];			// 73
	this->hi.f5			= g_tbl_hi[ VK_F5 ];			// 74
	this->hi.f6			= g_tbl_hi[ VK_F6 ];			// 75
	this->hi.f7			= g_tbl_hi[ VK_F7 ];			// 76
	this->hi.f8			= g_tbl_hi[ VK_F8 ];			// 77
	this->hi.f9			= g_tbl_hi[ VK_F9 ];			// 78
	this->hi.f10		= g_tbl_hi[ VK_F10 ];			// 79
	this->hi.semicolon	= g_tbl_hi[ VK_OEM_1 ];			// BA
	this->hi.comma		= g_tbl_hi[ VK_OEM_COMMA ];		// BC
	this->hi.period		= g_tbl_hi[ VK_OEM_PERIOD ];	// BE	


	this->lo.backspace	= g_tbl_lo[ VK_BACK ];			// 08
	this->lo.tab		= g_tbl_lo[ VK_TAB ];			// 09
	this->lo.enter		= g_tbl_lo[ VK_RETURN ];		// 0D
	this->lo.shift		= g_tbl_lo[ VK_SHIFT ];			// 10
	this->lo.ctrl		= g_tbl_lo[ VK_CONTROL ];		// 11
	this->lo.alt		= g_tbl_lo[ VK_MENU ];			// 12
	this->lo.capslock	= g_tbl_lo[ VK_CAPITAL ];		// 14
	this->lo.esc		= g_tbl_lo[ VK_ESCAPE ];		// 1B
	this->lo.space		= g_tbl_lo[ VK_SPACE ];			// 20
	this->lo.pageup		= g_tbl_lo[ VK_PRIOR ];			// 21
	this->lo.pagedown	= g_tbl_lo[ VK_NEXT ];			// 22
	this->lo.end		= g_tbl_lo[ VK_END ];			// 23
	this->lo.home		= g_tbl_lo[ VK_HOME ];			// 24
	this->lo.left		= g_tbl_lo[ VK_LEFT ];			// 25
	this->lo.up			= g_tbl_lo[ VK_UP ];			// 26
	this->lo.right		= g_tbl_lo[ VK_RIGHT ];			// 27
	this->lo.down		= g_tbl_lo[ VK_DOWN ];			// 28
	this->lo.a			= g_tbl_lo[ (int)'A'];			// 41
	this->lo.b			= g_tbl_lo[ (int)'B'];			// 42
	this->lo.c			= g_tbl_lo[ (int)'C'];			// 43
	this->lo.d			= g_tbl_lo[ (int)'D'];			// 44
	this->lo.e			= g_tbl_lo[ (int)'E'];			// 45
	this->lo.f			= g_tbl_lo[ (int)'F'];			// 46
	this->lo.g			= g_tbl_lo[ (int)'G'];			// 47
	this->lo.h			= g_tbl_lo[ (int)'H'];			// 48
	this->lo.i			= g_tbl_lo[ (int)'I'];			// 49
	this->lo.j			= g_tbl_lo[ (int)'J'];			// 4A
	this->lo.k			= g_tbl_lo[ (int)'K'];			// 4B
	this->lo.l			= g_tbl_lo[ (int)'L'];			// 4C
	this->lo.m			= g_tbl_lo[ (int)'M'];			// 4D
	this->lo.n			= g_tbl_lo[ (int)'N'];			// 4E
	this->lo.o			= g_tbl_lo[ (int)'O'];			// 4F
	this->lo.p			= g_tbl_lo[ (int)'P'];			// 50
	this->lo.q			= g_tbl_lo[ (int)'Q'];			// 51
	this->lo.r			= g_tbl_lo[ (int)'R'];			// 52
	this->lo.s			= g_tbl_lo[ (int)'S'];			// 53
	this->lo.t			= g_tbl_lo[ (int)'T'];			// 54
	this->lo.u			= g_tbl_lo[ (int)'U'];			// 55
	this->lo.v			= g_tbl_lo[ (int)'V'];			// 56
	this->lo.w			= g_tbl_lo[ (int)'W'];			// 57
	this->lo.x			= g_tbl_lo[ (int)'X'];			// 58
	this->lo.y			= g_tbl_lo[ (int)'Y'];			// 59
	this->lo.z			= g_tbl_lo[ (int)'Z'];			// 5A
	this->lo.f1			= g_tbl_lo[ VK_F1 ];			// 70
	this->lo.f2			= g_tbl_lo[ VK_F2 ];			// 71
	this->lo.f3			= g_tbl_lo[ VK_F3 ];			// 72
	this->lo.f4			= g_tbl_lo[ VK_F4 ];			// 73
	this->lo.f5			= g_tbl_lo[ VK_F5 ];			// 74
	this->lo.f6			= g_tbl_lo[ VK_F6 ];			// 75
	this->lo.f7			= g_tbl_lo[ VK_F7 ];			// 76
	this->lo.f8			= g_tbl_lo[ VK_F8 ];			// 77
	this->lo.f9			= g_tbl_lo[ VK_F9 ];			// 78
	this->lo.f10		= g_tbl_lo[ VK_F10 ];			// 79
	this->lo.semicolon	= g_tbl_lo[ VK_OEM_1 ];			// BA
	this->lo.comma		= g_tbl_lo[ VK_OEM_COMMA ];		// BC
	this->lo.period		= g_tbl_lo[ VK_OEM_PERIOD ];	// BE	


	this->rep.backspace	= g_tbl_rep[ VK_BACK ];			// 08
	this->rep.tab		= g_tbl_rep[ VK_TAB ];			// 09
	this->rep.enter		= g_tbl_rep[ VK_RETURN ];		// 0D
	this->rep.shift		= g_tbl_rep[ VK_SHIFT ];		// 10
	this->rep.ctrl		= g_tbl_rep[ VK_CONTROL ];		// 11
	this->rep.alt		= g_tbl_rep[ VK_MENU ];			// 12
	this->rep.capslock	= g_tbl_rep[ VK_CAPITAL ];		// 14
	this->rep.esc		= g_tbl_rep[ VK_ESCAPE ];		// 1B
	this->rep.space		= g_tbl_rep[ VK_SPACE ];		// 20
	this->rep.pageup	= g_tbl_rep[ VK_PRIOR ];		// 21
	this->rep.pagedown	= g_tbl_rep[ VK_NEXT ];			// 22
	this->rep.end		= g_tbl_rep[ VK_END ];			// 23
	this->rep.home		= g_tbl_rep[ VK_HOME ];			// 24
	this->rep.left		= g_tbl_rep[ VK_LEFT ];			// 25
	this->rep.up		= g_tbl_rep[ VK_UP ];			// 26
	this->rep.right		= g_tbl_rep[ VK_RIGHT ];		// 27
	this->rep.down		= g_tbl_rep[ VK_DOWN ];			// 28
	this->rep.a			= g_tbl_rep[ (int)'A'];			// 41
	this->rep.b			= g_tbl_rep[ (int)'B'];			// 42
	this->rep.c			= g_tbl_rep[ (int)'C'];			// 43
	this->rep.d			= g_tbl_rep[ (int)'D'];			// 44
	this->rep.e			= g_tbl_rep[ (int)'E'];			// 45
	this->rep.f			= g_tbl_rep[ (int)'F'];			// 46
	this->rep.g			= g_tbl_rep[ (int)'G'];			// 47
	this->rep.h			= g_tbl_rep[ (int)'H'];			// 48
	this->rep.i			= g_tbl_rep[ (int)'I'];			// 49
	this->rep.j			= g_tbl_rep[ (int)'J'];			// 4A
	this->rep.k			= g_tbl_rep[ (int)'K'];			// 4B
	this->rep.l			= g_tbl_rep[ (int)'L'];			// 4C
	this->rep.m			= g_tbl_rep[ (int)'M'];			// 4D
	this->rep.n			= g_tbl_rep[ (int)'N'];			// 4E
	this->rep.o			= g_tbl_rep[ (int)'O'];			// 4F
	this->rep.p			= g_tbl_rep[ (int)'P'];			// 50
	this->rep.q			= g_tbl_rep[ (int)'Q'];			// 51
	this->rep.r			= g_tbl_rep[ (int)'R'];			// 52
	this->rep.s			= g_tbl_rep[ (int)'S'];			// 53
	this->rep.t			= g_tbl_rep[ (int)'T'];			// 54
	this->rep.u			= g_tbl_rep[ (int)'U'];			// 55
	this->rep.v			= g_tbl_rep[ (int)'V'];			// 56
	this->rep.w			= g_tbl_rep[ (int)'W'];			// 57
	this->rep.x			= g_tbl_rep[ (int)'X'];			// 58
	this->rep.y			= g_tbl_rep[ (int)'Y'];			// 59
	this->rep.z			= g_tbl_rep[ (int)'Z'];			// 5A
	this->rep.f1		= g_tbl_rep[ VK_F1 ];			// 70
	this->rep.f2		= g_tbl_rep[ VK_F2 ];			// 71
	this->rep.f3		= g_tbl_rep[ VK_F3 ];			// 72
	this->rep.f4		= g_tbl_rep[ VK_F4 ];			// 73
	this->rep.f5		= g_tbl_rep[ VK_F5 ];			// 74
	this->rep.f6		= g_tbl_rep[ VK_F6 ];			// 75
	this->rep.f7		= g_tbl_rep[ VK_F7 ];			// 76
	this->rep.f8		= g_tbl_rep[ VK_F8 ];			// 77
	this->rep.f9		= g_tbl_rep[ VK_F9 ];			// 78
	this->rep.f10		= g_tbl_rep[ VK_F10 ];			// 79
	this->rep.semicolon	= g_tbl_rep[ VK_OEM_1 ];		// BA
	this->rep.comma		= g_tbl_rep[ VK_OEM_COMMA ];	// BC
	this->rep.period	= g_tbl_rep[ VK_OEM_PERIOD ];	// BE	

#if 0
	// 標準で使えるキー（US配列、日本語配列、コンパクトキーボード等、大抵のキーボードで対応しているキー
	if ( this->on.esc )	cout <<  "ok" << endl;
	if ( this->on.tab )	cout <<  "ok" << endl;
	if ( this->on.capslock )	cout <<  "ok" << endl;
	if ( this->on.shift )	cout <<  "ok" << endl;
	if ( this->on.ctrl )	cout <<  "ok" << endl;
	if ( this->on.alt )	cout <<  "ok" << endl;
	if ( this->on.up )	cout <<  "ok" << endl;
	if ( this->on.down )	cout <<  "ok" << endl;
	if ( this->on.left )	cout <<  "ok" << endl;
	if ( this->on.right )	cout <<  "ok" << endl;
	if ( this->on.pageup )	cout <<  "ok" << endl;
	if ( this->on.pagedown )	cout <<  "ok" << endl;
	if ( this->on.home )	cout <<  "ok" << endl;
	if ( this->on.end )	cout <<  "ok" << endl;
	if ( this->on.comma )	cout <<  "ok" << endl;
	if ( this->on.period )	cout <<  "ok" << endl;
	if ( this->on.semicolon )	cout <<  "ok" << endl;
	if ( this->on.space )	cout <<  "ok" << endl;
	if ( this->on.backspace )	cout <<  "ok" << endl;
	if ( this->on.enter )	cout <<  "ok" << endl;
	if ( this->on.f1 )	cout <<  "ok" << endl;
	if ( this->on.f2 )	cout <<  "ok" << endl;
	if ( this->on.f3 )	cout <<  "ok" << endl;
	if ( this->on.f4 )	cout <<  "ok" << endl;
	if ( this->on.f5 )	cout <<  "ok" << endl;
	if ( this->on.f6 )	cout <<  "ok" << endl;
	if ( this->on.f7 )	cout <<  "ok" << endl;
	if ( this->on.f8 )	cout <<  "ok" << endl;
	if ( this->on.f9 )	cout <<  "ok" << endl;
	if ( this->on.f10 )	cout <<  "ok" << endl;
	if ( this->on.a )	cout <<  "ok" << endl;
	if ( this->on.b )	cout <<  "ok" << endl;
	if ( this->on.c )	cout <<  "ok" << endl;
	if ( this->on.d )	cout <<  "ok" << endl;
	if ( this->on.e )	cout <<  "ok" << endl;
	if ( this->on.f )	cout <<  "ok" << endl;
	if ( this->on.g )	cout <<  "ok" << endl;
	if ( this->on.h )	cout <<  "ok" << endl;
	if ( this->on.i )	cout <<  "ok" << endl;
	if ( this->on.j )	cout <<  "ok" << endl;
	if ( this->on.k )	cout <<  "ok" << endl;
	if ( this->on.l )	cout <<  "ok" << endl;
	if ( this->on.m )	cout <<  "ok" << endl;
	if ( this->on.n )	cout <<  "ok" << endl;
	if ( this->on.o )	cout <<  "ok" << endl;
	if ( this->on.p )	cout <<  "ok" << endl;
	if ( this->on.q )	cout <<  "ok" << endl;
	if ( this->on.r )	cout <<  "ok" << endl;
	if ( this->on.s )	cout <<  "ok" << endl;
	if ( this->on.t )	cout <<  "ok" << endl;
	if ( this->on.u )	cout <<  "ok" << endl;
	if ( this->on.v )	cout <<  "ok" << endl;
	if ( this->on.w )	cout <<  "ok" << endl;
	if ( this->on.x )	cout <<  "ok" << endl;
	if ( this->on.y )	cout <<  "ok" << endl;
	if ( this->on.z )	cout <<  "ok" << endl;
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
		cout << hex << "(*this) hi " << i << endl;
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
		cout << hex << "(*this) toggle " << i << endl;
	}
}
#endif

//	return	true;
}
