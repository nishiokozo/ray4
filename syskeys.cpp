#include <iostream>
using namespace std;
#include "SysKeys.h"

#include <windows.h>

// WinAPI由来の情報。表に出さないためにここで定義
static struct Winapi_key
{
	BYTE	state[256];
	bool	tbl_prev[256];
	bool	tbl_on[256];
	bool	tbl_hi[256];
	bool	tbl_lo[256];
	bool	tbl_rep[256];
	int		tbl_lim[256];

	Winapi_key()
	{
		for ( int i = 0 ; i < 255 ; i++ )
		{
			tbl_prev[i]	= 0;
			tbl_on[i]	= 0;
			tbl_hi[i]	= 0;
			tbl_lo[i]	= 0;
			tbl_rep[i]	= 0;
			tbl_lim[i]	= 0;
		}
	}
} g;


//-----------------------------------------------------------------------------
SysKeys& SysKeys::GetInstance()
//-----------------------------------------------------------------------------
{
	static SysKeys	g_instance;
	return	g_instance;
}

//-----------------------------------------------------------------------------
SysKeys::SysKeys()
//-----------------------------------------------------------------------------
{
}

//-----------------------------------------------------------------------------
SysKeys::~SysKeys()
//-----------------------------------------------------------------------------
{
}

//-----------------------------------------------------------------------------
void SysKeys::Update()
//-----------------------------------------------------------------------------
{


//最上位ビットが 1 のときはキーが押されていることを、0 のときはキーが押されていないことを示します。最下位ビットが 1 のときはキーがトグル状態にあることを、0 のときはトグルが解除されていることを示します。たとえば、CapsLock キーが ON になっているときは、トグル状態になります。
	GetKeyboardState( g.state );

	for ( int i = 0 ; i < 255 ; i++ )
	{
		bool prev	= g.tbl_on[i];
		bool on		= ((g.state[i] & 0x80)!=0);
		bool hi		= ( on && !prev);
		bool lo		= (!on &&  prev);
		bool rep	= hi;
		int	lim	= g.tbl_lim[i];

		if ( hi )
		{
			lim=16;
		}
		if ( --lim <= 0 && on )
		{
			lim = 4;
			rep = true;
		}

		g.tbl_prev[i]	= prev;
		g.tbl_on[i]		= on;
		g.tbl_hi[i]		= hi;
		g.tbl_lo[i]		= lo;
		g.tbl_rep[i]	= rep;
		g.tbl_lim[i]	= lim;
	}

	// Windows-APIの仕様を上に挙げないように一つ一つアサインする

/*
	// 標準で使えるキー（US配列、日本語配列、コンパクトキーボード等、大抵のキーボードで対応しているキーのみアサイン


	使用		:	VKキーマップ
----------------+-----------------------------------------
	-			:	00 
	-			:	01 VK_LBUTTON
	-			:	02 VK_RBUTTON
	-			:	03 VK_CANCEL
	-			:	04 VK_MBUTTON
	-			:	05 VK_XBUTTON1
	-			:	06 VK_XBUTTON2
	-			:	07 
	BACKSPACE	:	08 VK_BACK
	TAB			:	09 VK_TAB
	-			:	0A 
	-			:	0B 
	-			:	0C VK_CLEAR
	ENTER		:	0D VK_RETURN
	-			:	0E 
	-			:	0F 
	SHIFT		:	10 VK_SHIFT
	CTRL		:	11 VK_CONTROL
	ALT			:	12 VK_MENU
	-			:	13 VK_PAUSE
	CAPSLOCK	:	14 VK_CAPITAL
	-			:	15 VK_KANA
	-			:	16 
	-			:	17 VK_JUNJA
	-			:	18 VK_FINAL
	-			:	19 VK_KANJI 何故か常時0X80が入る
	-			:	1A 
	ESC			:	1B VK_ESCAPE
	-			:	1C VK_CONVERT
	-			:	1D VK_NONCONVERT
	-			:	1E VK_ACCEPT
	-			:	1F VK_MODECHANGE
	SPACE		:	20 VK_SPACE
	PAGEUP		:	21 VK_PRIOR
	PAGEDOWN	:	22 VK_NEXT
	END			:	23 VK_END
	HOME		:	24 VK_HOME
	LEFT		:	25 VK_LEFT
	UP			:	26 VK_UP
	RIGHT		:	27 VK_RIGHT
	DOWN		:	28 VK_DOWN
	-			:	29 VK_SELECT
	-			:	2A VK_PRINT
	-			:	2B VK_EXECUTE
	-			:	2C VK_SNAPSHOT
	-			:	2D VK_INSERT
	-			:	2E VK_DELETE
	-			:	2F VK_HELP
	_0			:	30 "0"
	_1			:	31 "1"
	_2			:	32 "2"
	_3			:	33 "3"
	_4			:	34 "4"
	_5			:	35 "5"
	_6			:	36 "6"
	_7			:	37 "7"
	_8			:	38 "8"
	_9			:	39 "9"
	-			:	3A VK_COLON
	-			:	3B VK_SEMICOLON
	-			:	3C VK_COMMMA
	-			:	3D VK_PERIOD
	-			:	3E 
	-			:	3F 
	-			:	40 
	A			:	41 'A'
	B			:	42 'B'
	C			:	43 'C'
	D			:	44 'D'
	E			:	45 'E'
	F			:	46 'F'
	G			:	47 'G'
	H			:	48 'H'
	I			:	49 'I'
	J			:	4A 'J'
	K			:	4B 'K'
	L			:	4C 'L'
	M			:	4D 'M'
	N			:	4E 'N'
	O			:	4F 'O'
	P			:	50 'P'
	Q			:	51 'Q'
	R			:	52 'R'
	S			:	53 'S'
	T			:	54 'T'
	U			:	55 'U'
	V			:	56 'V'
	W			:	57 'W'
	X			:	58 'X'
	Y			:	59 'Y'
	Z			:	5A 'Z'
	-			:	5B VK_LWIN
	-			:	5C VK_RWIN
	-			:	5D VK_APPS
	-			:	5E 
	-			:	5F VK_SLEEP
	-			:	60 VK_NUMPAD0
	-			:	61 VK_NUMPAD1
	-			:	62 VK_NUMPAD2
	-			:	63 VK_NUMPAD3
	-			:	64 VK_NUMPAD4
	-			:	65 VK_NUMPAD5
	-			:	66 VK_NUMPAD6
	-			:	67 VK_NUMPAD7
	-			:	68 VK_NUMPAD8
	-			:	69 VK_NUMPAD9
	-			:	6A VK_MULTIPLY
	-			:	6B VK_ADD
	-			:	6C VK_SEPARATOR
	-			:	6D VK_SUBTRACT
	-			:	6E VK_DECIMAL
	-			:	6F VK_DIVIDE
	F1			:	70 VK_F1
	F2			:	71 VK_F2
	F3			:	72 VK_F3
	F4			:	73 VK_F4
	F5			:	74 VK_F5
	F6			:	75 VK_F6
	F7			:	76 VK_F7
	F8			:	77 VK_F8
	F9			:	78 VK_F9
	F10			:	79 VK_F10
	-			:	7A VK_F11
	-			:	7B VK_F12
	-			:	7C VK_F13
	-			:	7D VK_F14
	-			:	7E VK_F15
	-			:	7F VK_F16
	-			:	80 VK_F17
	-			:	81 VK_F18
	-			:	82 VK_F19
	-			:	83 VK_F20
	-			:	84 VK_F21
	-			:	85 VK_F22
	-			:	86 VK_F23
	-			:	87 VK_F24
	-			:	88 
	-			:	89 
	-			:	8A 
	-			:	8B 
	-			:	8C 
	-			:	8D 
	-			:	8E 
	-			:	8F 
	-			:	90 VK_NUMLOCK
	-			:	91 VK_SCROLL
	-			:	92 VK_OEM_FJ_JISHO
	-			:	93 VK_OEM_FJ_MASSHOU
	-			:	94 VK_OEM_FJ_TOUROKU
	-			:	95 VK_OEM_FJ_LOYA
	-			:	96 VK_OEM_FJ_ROYA
	-			:	97 
	-			:	98 
	-			:	99 
	-			:	9A 
	-			:	9B 
	-			:	9C 
	-			:	9D 
	-			:	9E 
	-			:	9F 
	-			:	A0 VK_LSHIFT
	-			:	A1 VK_RSHIFT
	-			:	A2 VK_LCONTROL
	-			:	A3 VK_RCONTROL
	-			:	A4 VK_LMENU
	-			:	A5 VK_RMENU
	-			:	A6 VK_BROWSER_BACK
	-			:	A7 VK_BROWSER_FORWARD
	-			:	A8 VK_BROWSER_REFRESH
	-			:	A9 VK_BROWSER_STOP
	-			:	AA VK_BROWSER_SEARCH
	-			:	AB VK_BROWSER_FAVORITES
	-			:	AC VK_BROWSER_HOME
	-			:	AD VK_VOLUME_MUTE
	-			:	AE VK_VOLUME_DOWN
	-			:	AF VK_VOLUME_UP
	-			:	B0 VK_MEDIA_NEXT_TRACK
	-			:	B1 VK_MEDIA_PREV_TRACK
	-			:	B2 VK_MEDIA_STOP
	-			:	B3 VK_MEDIA_PLAY_PAUSE
	-			:	B4 VK_LAUNCH_MAIL
	-			:	B5 VK_LAUNCH_MEDIA_SELECT
	-			:	B6 VK_LAUNCH_APP1
	-			:	B7 VK_LAUNCH_APP2
	-			:	B8 
	-			:	B9 
	SEMICOLON	:	BA VK_OEM_1
	-			:	BB VK_OEM_PLUS
	COMMA		:	BC VK_OEM_COMMA
	-			:	BD VK_OEM_MINUS
	PERIOD		:	BE VK_OEM_PERIOD
	-			:	BF VK_OEM_2
	-			:	C0 VK_OEM_3
	-			:	C1 
	-			:	C2 
	-			:	C3 
	-			:	C4 
	-			:	C5 
	-			:	C6 
	-			:	C7 
	-			:	C8 
	-			:	C9 
	-			:	CA 
	-			:	CB 
	-			:	CC 
	-			:	CD 
	-			:	CE 
	-			:	CF 
	-			:	D0 
	-			:	D1 
	-			:	D2 
	-			:	D3 
	-			:	D4 
	-			:	D5 
	-			:	D6 
	-			:	D7 
	-			:	D8 
	-			:	D9 
	-			:	DA 
	-			:	DB VK_OEM_4
	-			:	DC VK_OEM_5
	-			:	DD VK_OEM_6
	-			:	DE VK_OEM_7
	-			:	DF VK_OEM_8
	-			:	E0 
	-			:	E1 VK_OEM_AX
	-			:	E2 VK_OEM_102
	-			:	E3 VK_ICO_HELP
	-			:	E4 VK_ICO_00
	-			:	E5 VK_PROCESSKEY
	-			:	E6 VK_ICO_CLEAR
	-			:	E7 VK_PACKET
	-			:	E8 
	-			:	E9 VK_OEM_RESET
	-			:	EA VK_OEM_JUMP
	-			:	EB VK_OEM_PA1
	-			:	EC VK_OEM_PA2
	-			:	ED VK_OEM_PA3
	-			:	EE VK_OEM_WSCTRL
	-			:	EF VK_OEM_CUSEL
	-			:	F0 VK_OEM_ATTN 何故か常時0X80が入る
	-			:	F1 VK_OEM_FINISH
	-			:	F2 VK_OEM_COPY
	-			:	F3 VK_OEM_AUTO 何故か常時0X80が入る
	-			:	F4 VK_OEM_ENLW
	-			:	F5 VK_OEM_BACKTAB
	-			:	F6 VK_ATTN 何故か常時0X80が入る
	-			:	F7 VK_CRSEL
	-			:	F8 VK_EXSEL
	-			:	F9 VK_EREOF
	-			:	FA VK_PLAY
	-			:	FB VK_ZOOM 何故か常時0X80が入る
	-			:	FC VK_NONAME
	-			:	FD VK_PA1
	-			:	FE VK_OEM_CLEAR
	-			:	FF 
*/
	
	this->BACKSPACE.on	= g.tbl_on[ VK_BACK ];			// 08
	this->BACKSPACE.hi	= g.tbl_hi[ VK_BACK ];			// 08
	this->BACKSPACE.lo	= g.tbl_lo[ VK_BACK ];			// 08
	this->BACKSPACE.rep	= g.tbl_rep[ VK_BACK ];			// 08

	this->TAB.on		= g.tbl_on[ VK_TAB ];			// 09
	this->TAB.hi		= g.tbl_hi[ VK_TAB ];			// 09
	this->TAB.lo		= g.tbl_lo[ VK_TAB ];			// 09
	this->TAB.rep		= g.tbl_rep[ VK_TAB ];			// 09

	this->ENTER.on		= g.tbl_on[ VK_RETURN ];		// 0D
	this->ENTER.hi		= g.tbl_hi[ VK_RETURN ];		// 0D
	this->ENTER.lo		= g.tbl_lo[ VK_RETURN ];		// 0D
	this->ENTER.rep		= g.tbl_rep[ VK_RETURN ];		// 0D

	this->SHIFT.on		= g.tbl_on[ VK_SHIFT ];			// 10
	this->SHIFT.hi		= g.tbl_hi[ VK_SHIFT ];			// 10
	this->SHIFT.lo		= g.tbl_lo[ VK_SHIFT ];			// 10
	this->SHIFT.rep		= g.tbl_rep[ VK_SHIFT ];		// 10

	this->CTRL.on		= g.tbl_on[ VK_CONTROL ];		// 11
	this->CTRL.hi		= g.tbl_hi[ VK_CONTROL ];		// 11
	this->CTRL.lo		= g.tbl_lo[ VK_CONTROL ];		// 11
	this->CTRL.rep		= g.tbl_rep[ VK_CONTROL ];		// 11

	this->ALT.on		= g.tbl_on[ VK_MENU ];			// 12
	this->ALT.hi		= g.tbl_hi[ VK_MENU ];			// 12
	this->ALT.lo		= g.tbl_lo[ VK_MENU ];			// 12
	this->ALT.rep		= g.tbl_rep[ VK_MENU ];			// 12

	this->CAPSLOCK.on	= g.tbl_on[ VK_CAPITAL ];		// 14
	this->CAPSLOCK.hi	= g.tbl_hi[ VK_CAPITAL ];		// 14
	this->CAPSLOCK.lo	= g.tbl_lo[ VK_CAPITAL ];		// 14
	this->CAPSLOCK.rep	= g.tbl_rep[ VK_CAPITAL ];		// 14

	this->ESC.on		= g.tbl_on[ VK_ESCAPE ];		// 1B
	this->ESC.hi		= g.tbl_hi[ VK_ESCAPE ];		// 1B
	this->ESC.lo		= g.tbl_lo[ VK_ESCAPE ];		// 1B
	this->ESC.rep		= g.tbl_rep[ VK_ESCAPE ];		// 1B

	this->SPACE.on		= g.tbl_on[ VK_SPACE ];			// 20
	this->SPACE.hi		= g.tbl_hi[ VK_SPACE ];			// 20
	this->SPACE.lo		= g.tbl_lo[ VK_SPACE ];			// 20
	this->SPACE.rep		= g.tbl_rep[ VK_SPACE ];		// 20

	this->PAGEUP.on		= g.tbl_on[ VK_PRIOR ];			// 21
	this->PAGEUP.hi		= g.tbl_hi[ VK_PRIOR ];			// 21
	this->PAGEUP.lo		= g.tbl_lo[ VK_PRIOR ];			// 21
	this->PAGEUP.rep	= g.tbl_rep[ VK_PRIOR ];		// 21

	this->PAGEDOWN.on	= g.tbl_on[ VK_NEXT ];			// 22
	this->PAGEDOWN.hi	= g.tbl_hi[ VK_NEXT ];			// 22
	this->PAGEDOWN.lo	= g.tbl_lo[ VK_NEXT ];			// 22
	this->PAGEDOWN.rep	= g.tbl_rep[ VK_NEXT ];			// 22

	this->END.on		= g.tbl_on[ VK_END ];			// 23
	this->END.hi		= g.tbl_hi[ VK_END ];			// 23
	this->END.lo		= g.tbl_lo[ VK_END ];			// 23
	this->END.rep		= g.tbl_rep[ VK_END ];			// 23

	this->HOME.on		= g.tbl_on[ VK_HOME ];			// 24
	this->HOME.hi		= g.tbl_hi[ VK_HOME ];			// 24
	this->HOME.lo		= g.tbl_lo[ VK_HOME ];			// 24
	this->HOME.rep		= g.tbl_rep[ VK_HOME ];			// 24

	this->LEFT.on		= g.tbl_on[ VK_LEFT ];			// 25
	this->LEFT.hi		= g.tbl_hi[ VK_LEFT ];			// 25
	this->LEFT.lo		= g.tbl_lo[ VK_LEFT ];			// 25
	this->LEFT.rep		= g.tbl_rep[ VK_LEFT ];			// 25

	this->UP.on			= g.tbl_on[ VK_UP ];			// 26
	this->UP.hi			= g.tbl_hi[ VK_UP ];			// 26
	this->UP.lo			= g.tbl_lo[ VK_UP ];			// 26
	this->UP.rep		= g.tbl_rep[ VK_UP ];			// 26

	this->RIGHT.on		= g.tbl_on[ VK_RIGHT ];			// 27
	this->RIGHT.hi		= g.tbl_hi[ VK_RIGHT ];			// 27
	this->RIGHT.lo		= g.tbl_lo[ VK_RIGHT ];			// 27
	this->RIGHT.rep		= g.tbl_rep[ VK_RIGHT ];		// 27

	this->DOWN.on		= g.tbl_on[ VK_DOWN ];			// 28
	this->DOWN.hi		= g.tbl_hi[ VK_DOWN ];			// 28
	this->DOWN.lo		= g.tbl_lo[ VK_DOWN ];			// 28
	this->DOWN.rep		= g.tbl_rep[ VK_DOWN ];			// 28

	this->A.on			= g.tbl_on[ (INT)'A'];			// 41
	this->A.hi			= g.tbl_hi[ (INT)'A'];			// 41
	this->A.lo			= g.tbl_lo[ (INT)'A'];			// 41
	this->A.rep			= g.tbl_rep[ (INT)'A'];			// 41

	this->B.on			= g.tbl_on[ (INT)'B'];			// 42
	this->B.hi			= g.tbl_hi[ (INT)'B'];			// 42
	this->B.lo			= g.tbl_lo[ (INT)'B'];			// 42
	this->B.rep			= g.tbl_rep[ (INT)'B'];			// 42

	this->C.on			= g.tbl_on[ (INT)'C'];			// 43
	this->C.hi			= g.tbl_hi[ (INT)'C'];			// 43
	this->C.lo			= g.tbl_lo[ (INT)'C'];			// 43
	this->C.rep			= g.tbl_rep[ (INT)'C'];			// 43

	this->D.on			= g.tbl_on[ (INT)'D'];			// 44
	this->D.hi			= g.tbl_hi[ (INT)'D'];			// 44
	this->D.lo			= g.tbl_lo[ (INT)'D'];			// 44
	this->D.rep			= g.tbl_rep[ (INT)'D'];			// 44

	this->E.on			= g.tbl_on[ (INT)'E'];			// 45
	this->E.hi			= g.tbl_hi[ (INT)'E'];			// 45
	this->E.lo			= g.tbl_lo[ (INT)'E'];			// 45
	this->E.rep			= g.tbl_rep[ (INT)'E'];			// 45

	this->F.on			= g.tbl_on[ (INT)'F'];			// 46
	this->F.hi			= g.tbl_hi[ (INT)'F'];			// 46
	this->F.lo			= g.tbl_lo[ (INT)'F'];			// 46
	this->F.rep			= g.tbl_rep[ (INT)'F'];			// 46

	this->G.on			= g.tbl_on[ (INT)'G'];			// 47
	this->G.hi			= g.tbl_hi[ (INT)'G'];			// 47
	this->G.lo			= g.tbl_lo[ (INT)'G'];			// 47
	this->G.rep			= g.tbl_rep[ (INT)'G'];			// 47

	this->H.on			= g.tbl_on[ (INT)'H'];			// 48
	this->H.hi			= g.tbl_hi[ (INT)'H'];			// 48
	this->H.lo			= g.tbl_lo[ (INT)'H'];			// 48
	this->H.rep			= g.tbl_rep[ (INT)'H'];			// 48

	this->I.on			= g.tbl_on[ (INT)'I'];			// 49
	this->I.hi			= g.tbl_hi[ (INT)'I'];			// 49
	this->I.lo			= g.tbl_lo[ (INT)'I'];			// 49
	this->I.rep			= g.tbl_rep[ (INT)'I'];			// 49

	this->J.on			= g.tbl_on[ (INT)'J'];			// 4A
	this->J.hi			= g.tbl_hi[ (INT)'J'];			// 4A
	this->J.lo			= g.tbl_lo[ (INT)'J'];			// 4A
	this->J.rep			= g.tbl_rep[ (INT)'J'];			// 4A

	this->K.on			= g.tbl_on[ (INT)'K'];			// 4B
	this->K.hi			= g.tbl_hi[ (INT)'K'];			// 4B
	this->K.lo			= g.tbl_lo[ (INT)'K'];			// 4B
	this->K.rep			= g.tbl_rep[ (INT)'K'];			// 4B

	this->L.on			= g.tbl_on[ (INT)'L'];			// 4C
	this->L.hi			= g.tbl_hi[ (INT)'L'];			// 4C
	this->L.lo			= g.tbl_lo[ (INT)'L'];			// 4C
	this->L.rep			= g.tbl_rep[ (INT)'L'];			// 4C

	this->M.on			= g.tbl_on[ (INT)'M'];			// 4D
	this->M.hi			= g.tbl_hi[ (INT)'M'];			// 4D
	this->M.lo			= g.tbl_lo[ (INT)'M'];			// 4D
	this->M.rep			= g.tbl_rep[ (INT)'M'];			// 4D

	this->N.on			= g.tbl_on[ (INT)'N'];			// 4E
	this->N.hi			= g.tbl_hi[ (INT)'N'];			// 4E
	this->N.lo			= g.tbl_lo[ (INT)'N'];			// 4E
	this->N.rep			= g.tbl_rep[ (INT)'N'];			// 4E

	this->O.on			= g.tbl_on[ (INT)'O'];			// 4F
	this->O.hi			= g.tbl_hi[ (INT)'O'];			// 4F
	this->O.lo			= g.tbl_lo[ (INT)'O'];			// 4F
	this->O.rep			= g.tbl_rep[ (INT)'O'];			// 4F

	this->P.on			= g.tbl_on[ (INT)'P'];			// 50
	this->P.hi			= g.tbl_hi[ (INT)'P'];			// 50
	this->P.lo			= g.tbl_lo[ (INT)'P'];			// 50
	this->P.rep			= g.tbl_rep[ (INT)'P'];			// 50

	this->Q.on			= g.tbl_on[ (INT)'Q'];			// 51
	this->Q.hi			= g.tbl_hi[ (INT)'Q'];			// 51
	this->Q.lo			= g.tbl_lo[ (INT)'Q'];			// 51
	this->Q.rep			= g.tbl_rep[ (INT)'Q'];			// 51

	this->R.on			= g.tbl_on[ (INT)'R'];			// 52
	this->R.hi			= g.tbl_hi[ (INT)'R'];			// 52
	this->R.lo			= g.tbl_lo[ (INT)'R'];			// 52
	this->R.rep			= g.tbl_rep[ (INT)'R'];			// 52

	this->S.on			= g.tbl_on[ (INT)'S'];			// 53
	this->S.hi			= g.tbl_hi[ (INT)'S'];			// 53
	this->S.lo			= g.tbl_lo[ (INT)'S'];			// 53
	this->S.rep			= g.tbl_rep[ (INT)'S'];			// 53

	this->T.on			= g.tbl_on[ (INT)'T'];			// 54
	this->T.hi			= g.tbl_hi[ (INT)'T'];			// 54
	this->T.lo			= g.tbl_lo[ (INT)'T'];			// 54
	this->T.rep			= g.tbl_rep[ (INT)'T'];			// 54

	this->U.on			= g.tbl_on[ (INT)'U'];			// 55
	this->U.hi			= g.tbl_hi[ (INT)'U'];			// 55
	this->U.lo			= g.tbl_lo[ (INT)'U'];			// 55
	this->U.rep			= g.tbl_rep[ (INT)'U'];			// 55

	this->V.on			= g.tbl_on[ (INT)'V'];			// 56
	this->V.hi			= g.tbl_hi[ (INT)'V'];			// 56
	this->V.lo			= g.tbl_lo[ (INT)'V'];			// 56
	this->V.rep			= g.tbl_rep[ (INT)'V'];			// 56

	this->W.on			= g.tbl_on[ (INT)'W'];			// 57
	this->W.hi			= g.tbl_hi[ (INT)'W'];			// 57
	this->W.lo			= g.tbl_lo[ (INT)'W'];			// 57
	this->W.rep			= g.tbl_rep[ (INT)'W'];			// 57

	this->X.on			= g.tbl_on[ (INT)'X'];			// 58
	this->X.hi			= g.tbl_hi[ (INT)'X'];			// 58
	this->X.lo			= g.tbl_lo[ (INT)'X'];			// 58
	this->X.rep			= g.tbl_rep[ (INT)'X'];			// 58

	this->Y.on			= g.tbl_on[ (INT)'Y'];			// 59
	this->Y.hi			= g.tbl_hi[ (INT)'Y'];			// 59
	this->Y.lo			= g.tbl_lo[ (INT)'Y'];			// 59
	this->Y.rep			= g.tbl_rep[ (INT)'Y'];			// 59

	this->Z.on			= g.tbl_on[ (INT)'Z'];			// 5A
	this->Z.hi			= g.tbl_hi[ (INT)'Z'];			// 5A
	this->Z.lo			= g.tbl_lo[ (INT)'Z'];			// 5A
	this->Z.rep			= g.tbl_rep[ (INT)'Z'];			// 5A

	this->F1.on			= g.tbl_on[ VK_F1 ];			// 70
	this->F1.hi			= g.tbl_hi[ VK_F1 ];			// 70
	this->F1.lo			= g.tbl_lo[ VK_F1 ];			// 70
	this->F1.rep		= g.tbl_rep[ VK_F1 ];			// 70

	this->F2.on			= g.tbl_on[ VK_F2 ];			// 71
	this->F2.hi			= g.tbl_hi[ VK_F2 ];			// 71
	this->F2.lo			= g.tbl_lo[ VK_F2 ];			// 71
	this->F2.rep		= g.tbl_rep[ VK_F2 ];			// 71

	this->F3.on			= g.tbl_on[ VK_F3 ];			// 72
	this->F3.hi			= g.tbl_hi[ VK_F3 ];			// 72
	this->F3.lo			= g.tbl_lo[ VK_F3 ];			// 72
	this->F3.rep		= g.tbl_rep[ VK_F3 ];			// 72

	this->F4.on			= g.tbl_on[ VK_F4 ];			// 73
	this->F4.hi			= g.tbl_hi[ VK_F4 ];			// 73
	this->F4.lo			= g.tbl_lo[ VK_F4 ];			// 73
	this->F4.rep		= g.tbl_rep[ VK_F4 ];			// 73

	this->F5.on			= g.tbl_on[ VK_F5 ];			// 74
	this->F5.hi			= g.tbl_hi[ VK_F5 ];			// 74
	this->F5.lo			= g.tbl_lo[ VK_F5 ];			// 74
	this->F5.rep		= g.tbl_rep[ VK_F5 ];			// 74

	this->F6.on			= g.tbl_on[ VK_F6 ];			// 75
	this->F6.hi			= g.tbl_hi[ VK_F6 ];			// 75
	this->F6.lo			= g.tbl_lo[ VK_F6 ];			// 75
	this->F6.rep		= g.tbl_rep[ VK_F6 ];			// 75

	this->F7.on			= g.tbl_on[ VK_F7 ];			// 76
	this->F7.hi			= g.tbl_hi[ VK_F7 ];			// 76
	this->F7.lo			= g.tbl_lo[ VK_F7 ];			// 76
	this->F7.rep		= g.tbl_rep[ VK_F7 ];			// 76

	this->F8.on			= g.tbl_on[ VK_F8 ];			// 77
	this->F8.hi			= g.tbl_hi[ VK_F8 ];			// 77
	this->F8.lo			= g.tbl_lo[ VK_F8 ];			// 77
	this->F8.rep		= g.tbl_rep[ VK_F8 ];			// 77

	this->F9.on			= g.tbl_on[ VK_F9 ];			// 78
	this->F9.hi			= g.tbl_hi[ VK_F9 ];			// 78
	this->F9.lo			= g.tbl_lo[ VK_F9 ];			// 78
	this->F9.rep		= g.tbl_rep[ VK_F9 ];			// 78

	this->F10.on		= g.tbl_on[ VK_F10 ];			// 79
	this->F10.hi		= g.tbl_hi[ VK_F10 ];			// 79
	this->F10.lo		= g.tbl_lo[ VK_F10 ];			// 79
	this->F10.rep		= g.tbl_rep[ VK_F10 ];			// 79

	this->SEMICOLON.on	= g.tbl_on[ VK_OEM_1 ];			// BA
	this->SEMICOLON.hi	= g.tbl_hi[ VK_OEM_1 ];			// BA
	this->SEMICOLON.lo	= g.tbl_lo[ VK_OEM_1 ];			// BA
	this->SEMICOLON.rep	= g.tbl_rep[ VK_OEM_1 ];		// BA

	this->COMMA.on		= g.tbl_on[ VK_OEM_COMMA ];		// BC
	this->COMMA.hi		= g.tbl_hi[ VK_OEM_COMMA ];		// BC
	this->COMMA.lo		= g.tbl_lo[ VK_OEM_COMMA ];		// BC
	this->COMMA.rep		= g.tbl_rep[ VK_OEM_COMMA ];	// BC

	this->PERIOD.on		= g.tbl_on[ VK_OEM_PERIOD ];	// BE
	this->PERIOD.hi		= g.tbl_hi[ VK_OEM_PERIOD ];	// BE
	this->PERIOD.lo		= g.tbl_lo[ VK_OEM_PERIOD ];	// BE
	this->PERIOD.rep	= g.tbl_rep[ VK_OEM_PERIOD ];	// BE

#if 0
	 // キーコードチェック用
	int i;
	for ( i = 0 ; i < 255 ; i++ )
	{
		if ( g.state[i] & 0x80 )
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
}
