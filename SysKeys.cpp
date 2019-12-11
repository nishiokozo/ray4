#include <iostream>
#include <memory>
#include "SysKeys.h"

#include <windows.h>

using namespace std;

// WinAPI由来の情報。表に出さないためにここで定義
struct SysKeys::Impl
{
	struct Winapi_key
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

};

//-----------------------------------------------------------------------------
SysKeys::SysKeys() : pImpl( new SysKeys::Impl )
//-----------------------------------------------------------------------------
{
	Update(); // キー情報の正確な取得には一つ前の情報が必要なので、初期化時にUpdateを行う
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
	GetKeyboardState( pImpl->g.state );

	for ( int i = 0 ; i < 255 ; i++ )
	{
		bool prev	= pImpl->g.tbl_on[i];
		bool on		= ((pImpl->g.state[i] & 0x80)!=0);
		bool hi		= ( on && !prev);
		bool lo		= (!on &&  prev);
		bool rep	= hi;
		int	lim	= pImpl->g.tbl_lim[i];

		if ( hi )
		{
			lim=16;
		}
		if ( --lim <= 0 && on )
		{
			lim = 4;
			rep = true;
		}

		pImpl->g.tbl_prev[i]	= prev;
		pImpl->g.tbl_on[i]		= on;
		pImpl->g.tbl_hi[i]		= hi;
		pImpl->g.tbl_lo[i]		= lo;
		pImpl->g.tbl_rep[i]	= rep;
		pImpl->g.tbl_lim[i]	= lim;
	}



	//	 標準で使えるキー（US配列、日本語配列、コンパクトキーボード等、大抵のキーボードで対応しているキーのみアサイン
	//	
	//	
	//		使用		:	VKキーマップ
	//	----------------+-----------------------------------------
	//		-			:	00 
	//		-			:	01 VK_LBUTTON
	//		-			:	02 VK_RBUTTON
	//		-			:	03 VK_CANCEL
	//		-			:	04 VK_MBUTTON
	//		-			:	05 VK_XBUTTON1
	//		-			:	06 VK_XBUTTON2
	//		-			:	07 
	//		BACKSPACE	:	08 VK_BACK
	//		TAB			:	09 VK_TAB
	//		-			:	0A 
	//		-			:	0B 
	//		-			:	0C VK_CLEAR
	//		ENTER		:	0D VK_RETURN
	//		-			:	0E 
	//		-			:	0F 
	//		SHIFT		:	10 VK_SHIFT
	//		CTRL		:	11 VK_CONTROL
	//		ALT			:	12 VK_MENU
	//		-			:	13 VK_PAUSE
	//		CAPSLOCK	:	14 VK_CAPITAL
	//		-			:	15 VK_KANA
	//		-			:	16 
	//		-			:	17 VK_JUNJA
	//		-			:	18 VK_FINAL
	//		-			:	19 VK_KANJI 何故か常時0X80が入る
	//		-			:	1A 
	//		ESC			:	1B VK_ESCAPE
	//		-			:	1C VK_CONVERT
	//		-			:	1D VK_NONCONVERT
	//		-			:	1E VK_ACCEPT
	//		-			:	1F VK_MODECHANGE
	//		SPACE		:	20 VK_SPACE
	//		PAGEUP		:	21 VK_PRIOR
	//		PAGEDOWN	:	22 VK_NEXT
	//		END			:	23 VK_END
	//		HOME		:	24 VK_HOME
	//		LEFT		:	25 VK_LEFT
	//		UP			:	26 VK_UP
	//		RIGHT		:	27 VK_RIGHT
	//		DOWN		:	28 VK_DOWN
	//		-			:	29 VK_SELECT
	//		-			:	2A VK_PRINT
	//		-			:	2B VK_EXECUTE
	//		-			:	2C VK_SNAPSHOT
	//		-			:	2D VK_INSERT
	//		-			:	2E VK_DELETE
	//		-			:	2F VK_HELP
	//		_0			:	30 "0"
	//		_1			:	31 "1"
	//		_2			:	32 "2"
	//		_3			:	33 "3"
	//		_4			:	34 "4"
	//		_5			:	35 "5"
	//		_6			:	36 "6"
	//		_7			:	37 "7"
	//		_8			:	38 "8"
	//		_9			:	39 "9"
	//		-			:	3A VK_COLON
	//		-			:	3B VK_SEMICOLON
	//		-			:	3C VK_COMMMA
	//		-			:	3D VK_PERIOD
	//		-			:	3E 
	//		-			:	3F 
	//		-			:	40 
	//		A			:	41 'A'
	//		B			:	42 'B'
	//		C			:	43 'C'
	//		D			:	44 'D'
	//		E			:	45 'E'
	//		F			:	46 'F'
	//		G			:	47 'G'
	//		H			:	48 'H'
	//		I			:	49 'I'
	//		J			:	4A 'J'
	//		K			:	4B 'K'
	//		L			:	4C 'L'
	//		M			:	4D 'M'
	//		N			:	4E 'N'
	//		O			:	4F 'O'
	//		P			:	50 'P'
	//		Q			:	51 'Q'
	//		R			:	52 'R'
	//		S			:	53 'S'
	//		T			:	54 'T'
	//		U			:	55 'U'
	//		V			:	56 'V'
	//		W			:	57 'W'
	//		X			:	58 'X'
	//		Y			:	59 'Y'
	//		Z			:	5A 'Z'
	//		-			:	5B VK_LWIN
	//		-			:	5C VK_RWIN
	//		-			:	5D VK_APPS
	//		-			:	5E 
	//		-			:	5F VK_SLEEP
	//		-			:	60 VK_NUMPAD0
	//		-			:	61 VK_NUMPAD1
	//		-			:	62 VK_NUMPAD2
	//		-			:	63 VK_NUMPAD3
	//		-			:	64 VK_NUMPAD4
	//		-			:	65 VK_NUMPAD5
	//		-			:	66 VK_NUMPAD6
	//		-			:	67 VK_NUMPAD7
	//		-			:	68 VK_NUMPAD8
	//		-			:	69 VK_NUMPAD9
	//		-			:	6A VK_MULTIPLY
	//		-			:	6B VK_ADD
	//		-			:	6C VK_SEPARATOR
	//		-			:	6D VK_SUBTRACT
	//		-			:	6E VK_DECIMAL
	//		-			:	6F VK_DIVIDE
	//		F1			:	70 VK_F1
	//		F2			:	71 VK_F2
	//		F3			:	72 VK_F3
	//		F4			:	73 VK_F4
	//		F5			:	74 VK_F5
	//		F6			:	75 VK_F6
	//		F7			:	76 VK_F7
	//		F8			:	77 VK_F8
	//		F9			:	78 VK_F9
	//		F10			:	79 VK_F10
	//		-			:	7A VK_F11
	//		-			:	7B VK_F12
	//		-			:	7C VK_F13
	//		-			:	7D VK_F14
	//		-			:	7E VK_F15
	//		-			:	7F VK_F16
	//		-			:	80 VK_F17
	//		-			:	81 VK_F18
	//		-			:	82 VK_F19
	//		-			:	83 VK_F20
	//		-			:	84 VK_F21
	//		-			:	85 VK_F22
	//		-			:	86 VK_F23
	//		-			:	87 VK_F24
	//		-			:	88 
	//		-			:	89 
	//		-			:	8A 
	//		-			:	8B 
	//		-			:	8C 
	//		-			:	8D 
	//		-			:	8E 
	//		-			:	8F 
	//		-			:	90 VK_NUMLOCK
	//		-			:	91 VK_SCROLL
	//		-			:	92 VK_OEM_FJ_JISHO
	//		-			:	93 VK_OEM_FJ_MASSHOU
	//		-			:	94 VK_OEM_FJ_TOUROKU
	//		-			:	95 VK_OEM_FJ_LOYA
	//		-			:	96 VK_OEM_FJ_ROYA
	//		-			:	97 
	//		-			:	98 
	//		-			:	99 
	//		-			:	9A 
	//		-			:	9B 
	//		-			:	9C 
	//		-			:	9D 
	//		-			:	9E 
	//		-			:	9F 
	//		-			:	A0 VK_LSHIFT
	//		-			:	A1 VK_RSHIFT
	//		-			:	A2 VK_LCONTROL
	//		-			:	A3 VK_RCONTROL
	//		-			:	A4 VK_LMENU
	//		-			:	A5 VK_RMENU
	//		-			:	A6 VK_BROWSER_BACK
	//		-			:	A7 VK_BROWSER_FORWARD
	//		-			:	A8 VK_BROWSER_REFRESH
	//		-			:	A9 VK_BROWSER_STOP
	//		-			:	AA VK_BROWSER_SEARCH
	//		-			:	AB VK_BROWSER_FAVORITES
	//		-			:	AC VK_BROWSER_HOME
	//		-			:	AD VK_VOLUME_MUTE
	//		-			:	AE VK_VOLUME_DOWN
	//		-			:	AF VK_VOLUME_UP
	//		-			:	B0 VK_MEDIA_NEXT_TRACK
	//		-			:	B1 VK_MEDIA_PREV_TRACK
	//		-			:	B2 VK_MEDIA_STOP
	//		-			:	B3 VK_MEDIA_PLAY_PAUSE
	//		-			:	B4 VK_LAUNCH_MAIL
	//		-			:	B5 VK_LAUNCH_MEDIA_SELECT
	//		-			:	B6 VK_LAUNCH_APP1
	//		-			:	B7 VK_LAUNCH_APP2
	//		-			:	B8 
	//		-			:	B9 
	//		SEMICOLON	:	BA VK_OEM_1
	//		-			:	BB VK_OEM_PLUS
	//		COMMA		:	BC VK_OEM_COMMA
	//		-			:	BD VK_OEM_MINUS
	//		PERIOD		:	BE VK_OEM_PERIOD
	//		-			:	BF VK_OEM_2
	//		-			:	C0 VK_OEM_3
	//		-			:	C1 
	//		-			:	C2 
	//		-			:	C3 
	//		-			:	C4 
	//		-			:	C5 
	//		-			:	C6 
	//		-			:	C7 
	//		-			:	C8 
	//		-			:	C9 
	//		-			:	CA 
	//		-			:	CB 
	//		-			:	CC 
	//		-			:	CD 
	//		-			:	CE 
	//		-			:	CF 
	//		-			:	D0 
	//		-			:	D1 
	//		-			:	D2 
	//		-			:	D3 
	//		-			:	D4 
	//		-			:	D5 
	//		-			:	D6 
	//		-			:	D7 
	//		-			:	D8 
	//		-			:	D9 
	//		-			:	DA 
	//		-			:	DB VK_OEM_4
	//		-			:	DC VK_OEM_5
	//		-			:	DD VK_OEM_6
	//		-			:	DE VK_OEM_7
	//		-			:	DF VK_OEM_8
	//		-			:	E0 
	//		-			:	E1 VK_OEM_AX
	//		-			:	E2 VK_OEM_102
	//		-			:	E3 VK_ICO_HELP
	//		-			:	E4 VK_ICO_00
	//		-			:	E5 VK_PROCESSKEY
	//		-			:	E6 VK_ICO_CLEAR
	//		-			:	E7 VK_PACKET
	//		-			:	E8 
	//		-			:	E9 VK_OEM_RESET
	//		-			:	EA VK_OEM_JUMP
	//		-			:	EB VK_OEM_PA1
	//		-			:	EC VK_OEM_PA2
	//		-			:	ED VK_OEM_PA3
	//		-			:	EE VK_OEM_WSCTRL
	//		-			:	EF VK_OEM_CUSEL
	//		-			:	F0 VK_OEM_ATTN 何故か常時0X80が入る
	//		-			:	F1 VK_OEM_FINISH
	//		-			:	F2 VK_OEM_COPY
	//		-			:	F3 VK_OEM_AUTO 何故か常時0X80が入る
	//		-			:	F4 VK_OEM_ENLW
	//		-			:	F5 VK_OEM_BACKTAB
	//		-			:	F6 VK_ATTN 何故か常時0X80が入る
	//		-			:	F7 VK_CRSEL
	//		-			:	F8 VK_EXSEL
	//		-			:	F9 VK_EREOF
	//		-			:	FA VK_PLAY
	//		-			:	FB VK_ZOOM 何故か常時0X80が入る
	//		-			:	FC VK_NONAME
	//		-			:	FD VK_PA1
	//		-			:	FE VK_OEM_CLEAR
	//		-			:	FF 
	
	this->BACKSPACE.on	= pImpl->g.tbl_on[ VK_BACK ];			// 08
	this->BACKSPACE.hi	= pImpl->g.tbl_hi[ VK_BACK ];			// 08
	this->BACKSPACE.lo	= pImpl->g.tbl_lo[ VK_BACK ];			// 08
	this->BACKSPACE.rep	= pImpl->g.tbl_rep[ VK_BACK ];			// 08

	this->TAB.on		= pImpl->g.tbl_on[ VK_TAB ];			// 09
	this->TAB.hi		= pImpl->g.tbl_hi[ VK_TAB ];			// 09
	this->TAB.lo		= pImpl->g.tbl_lo[ VK_TAB ];			// 09
	this->TAB.rep		= pImpl->g.tbl_rep[ VK_TAB ];			// 09

	this->ENTER.on		= pImpl->g.tbl_on[ VK_RETURN ];		// 0D
	this->ENTER.hi		= pImpl->g.tbl_hi[ VK_RETURN ];		// 0D
	this->ENTER.lo		= pImpl->g.tbl_lo[ VK_RETURN ];		// 0D
	this->ENTER.rep		= pImpl->g.tbl_rep[ VK_RETURN ];		// 0D

	this->SHIFT.on		= pImpl->g.tbl_on[ VK_SHIFT ];			// 10
	this->SHIFT.hi		= pImpl->g.tbl_hi[ VK_SHIFT ];			// 10
	this->SHIFT.lo		= pImpl->g.tbl_lo[ VK_SHIFT ];			// 10
	this->SHIFT.rep		= pImpl->g.tbl_rep[ VK_SHIFT ];		// 10

	this->CTRL.on		= pImpl->g.tbl_on[ VK_CONTROL ];		// 11
	this->CTRL.hi		= pImpl->g.tbl_hi[ VK_CONTROL ];		// 11
	this->CTRL.lo		= pImpl->g.tbl_lo[ VK_CONTROL ];		// 11
	this->CTRL.rep		= pImpl->g.tbl_rep[ VK_CONTROL ];		// 11

	this->ALT.on		= pImpl->g.tbl_on[ VK_MENU ];			// 12
	this->ALT.hi		= pImpl->g.tbl_hi[ VK_MENU ];			// 12
	this->ALT.lo		= pImpl->g.tbl_lo[ VK_MENU ];			// 12
	this->ALT.rep		= pImpl->g.tbl_rep[ VK_MENU ];			// 12

	this->CAPSLOCK.on	= pImpl->g.tbl_on[ VK_CAPITAL ];		// 14
	this->CAPSLOCK.hi	= pImpl->g.tbl_hi[ VK_CAPITAL ];		// 14
	this->CAPSLOCK.lo	= pImpl->g.tbl_lo[ VK_CAPITAL ];		// 14
	this->CAPSLOCK.rep	= pImpl->g.tbl_rep[ VK_CAPITAL ];		// 14

	this->ESC.on		= pImpl->g.tbl_on[ VK_ESCAPE ];		// 1B
	this->ESC.hi		= pImpl->g.tbl_hi[ VK_ESCAPE ];		// 1B
	this->ESC.lo		= pImpl->g.tbl_lo[ VK_ESCAPE ];		// 1B
	this->ESC.rep		= pImpl->g.tbl_rep[ VK_ESCAPE ];		// 1B

	this->SPACE.on		= pImpl->g.tbl_on[ VK_SPACE ];			// 20
	this->SPACE.hi		= pImpl->g.tbl_hi[ VK_SPACE ];			// 20
	this->SPACE.lo		= pImpl->g.tbl_lo[ VK_SPACE ];			// 20
	this->SPACE.rep		= pImpl->g.tbl_rep[ VK_SPACE ];		// 20

	this->PAGEUP.on		= pImpl->g.tbl_on[ VK_PRIOR ];			// 21
	this->PAGEUP.hi		= pImpl->g.tbl_hi[ VK_PRIOR ];			// 21
	this->PAGEUP.lo		= pImpl->g.tbl_lo[ VK_PRIOR ];			// 21
	this->PAGEUP.rep	= pImpl->g.tbl_rep[ VK_PRIOR ];		// 21

	this->PAGEDOWN.on	= pImpl->g.tbl_on[ VK_NEXT ];			// 22
	this->PAGEDOWN.hi	= pImpl->g.tbl_hi[ VK_NEXT ];			// 22
	this->PAGEDOWN.lo	= pImpl->g.tbl_lo[ VK_NEXT ];			// 22
	this->PAGEDOWN.rep	= pImpl->g.tbl_rep[ VK_NEXT ];			// 22

	this->END.on		= pImpl->g.tbl_on[ VK_END ];			// 23
	this->END.hi		= pImpl->g.tbl_hi[ VK_END ];			// 23
	this->END.lo		= pImpl->g.tbl_lo[ VK_END ];			// 23
	this->END.rep		= pImpl->g.tbl_rep[ VK_END ];			// 23

	this->HOME.on		= pImpl->g.tbl_on[ VK_HOME ];			// 24
	this->HOME.hi		= pImpl->g.tbl_hi[ VK_HOME ];			// 24
	this->HOME.lo		= pImpl->g.tbl_lo[ VK_HOME ];			// 24
	this->HOME.rep		= pImpl->g.tbl_rep[ VK_HOME ];			// 24

	this->LEFT.on		= pImpl->g.tbl_on[ VK_LEFT ];			// 25
	this->LEFT.hi		= pImpl->g.tbl_hi[ VK_LEFT ];			// 25
	this->LEFT.lo		= pImpl->g.tbl_lo[ VK_LEFT ];			// 25
	this->LEFT.rep		= pImpl->g.tbl_rep[ VK_LEFT ];			// 25

	this->UP.on			= pImpl->g.tbl_on[ VK_UP ];			// 26
	this->UP.hi			= pImpl->g.tbl_hi[ VK_UP ];			// 26
	this->UP.lo			= pImpl->g.tbl_lo[ VK_UP ];			// 26
	this->UP.rep		= pImpl->g.tbl_rep[ VK_UP ];			// 26

	this->RIGHT.on		= pImpl->g.tbl_on[ VK_RIGHT ];			// 27
	this->RIGHT.hi		= pImpl->g.tbl_hi[ VK_RIGHT ];			// 27
	this->RIGHT.lo		= pImpl->g.tbl_lo[ VK_RIGHT ];			// 27
	this->RIGHT.rep		= pImpl->g.tbl_rep[ VK_RIGHT ];		// 27

	this->DOWN.on		= pImpl->g.tbl_on[ VK_DOWN ];			// 28
	this->DOWN.hi		= pImpl->g.tbl_hi[ VK_DOWN ];			// 28
	this->DOWN.lo		= pImpl->g.tbl_lo[ VK_DOWN ];			// 28
	this->DOWN.rep		= pImpl->g.tbl_rep[ VK_DOWN ];			// 28

	this->A.on			= pImpl->g.tbl_on[ (INT)'A'];			// 41
	this->A.hi			= pImpl->g.tbl_hi[ (INT)'A'];			// 41
	this->A.lo			= pImpl->g.tbl_lo[ (INT)'A'];			// 41
	this->A.rep			= pImpl->g.tbl_rep[ (INT)'A'];			// 41

	this->B.on			= pImpl->g.tbl_on[ (INT)'B'];			// 42
	this->B.hi			= pImpl->g.tbl_hi[ (INT)'B'];			// 42
	this->B.lo			= pImpl->g.tbl_lo[ (INT)'B'];			// 42
	this->B.rep			= pImpl->g.tbl_rep[ (INT)'B'];			// 42

	this->C.on			= pImpl->g.tbl_on[ (INT)'C'];			// 43
	this->C.hi			= pImpl->g.tbl_hi[ (INT)'C'];			// 43
	this->C.lo			= pImpl->g.tbl_lo[ (INT)'C'];			// 43
	this->C.rep			= pImpl->g.tbl_rep[ (INT)'C'];			// 43

	this->D.on			= pImpl->g.tbl_on[ (INT)'D'];			// 44
	this->D.hi			= pImpl->g.tbl_hi[ (INT)'D'];			// 44
	this->D.lo			= pImpl->g.tbl_lo[ (INT)'D'];			// 44
	this->D.rep			= pImpl->g.tbl_rep[ (INT)'D'];			// 44

	this->E.on			= pImpl->g.tbl_on[ (INT)'E'];			// 45
	this->E.hi			= pImpl->g.tbl_hi[ (INT)'E'];			// 45
	this->E.lo			= pImpl->g.tbl_lo[ (INT)'E'];			// 45
	this->E.rep			= pImpl->g.tbl_rep[ (INT)'E'];			// 45

	this->F.on			= pImpl->g.tbl_on[ (INT)'F'];			// 46
	this->F.hi			= pImpl->g.tbl_hi[ (INT)'F'];			// 46
	this->F.lo			= pImpl->g.tbl_lo[ (INT)'F'];			// 46
	this->F.rep			= pImpl->g.tbl_rep[ (INT)'F'];			// 46

	this->G.on			= pImpl->g.tbl_on[ (INT)'G'];			// 47
	this->G.hi			= pImpl->g.tbl_hi[ (INT)'G'];			// 47
	this->G.lo			= pImpl->g.tbl_lo[ (INT)'G'];			// 47
	this->G.rep			= pImpl->g.tbl_rep[ (INT)'G'];			// 47

	this->H.on			= pImpl->g.tbl_on[ (INT)'H'];			// 48
	this->H.hi			= pImpl->g.tbl_hi[ (INT)'H'];			// 48
	this->H.lo			= pImpl->g.tbl_lo[ (INT)'H'];			// 48
	this->H.rep			= pImpl->g.tbl_rep[ (INT)'H'];			// 48

	this->I.on			= pImpl->g.tbl_on[ (INT)'I'];			// 49
	this->I.hi			= pImpl->g.tbl_hi[ (INT)'I'];			// 49
	this->I.lo			= pImpl->g.tbl_lo[ (INT)'I'];			// 49
	this->I.rep			= pImpl->g.tbl_rep[ (INT)'I'];			// 49

	this->J.on			= pImpl->g.tbl_on[ (INT)'J'];			// 4A
	this->J.hi			= pImpl->g.tbl_hi[ (INT)'J'];			// 4A
	this->J.lo			= pImpl->g.tbl_lo[ (INT)'J'];			// 4A
	this->J.rep			= pImpl->g.tbl_rep[ (INT)'J'];			// 4A

	this->K.on			= pImpl->g.tbl_on[ (INT)'K'];			// 4B
	this->K.hi			= pImpl->g.tbl_hi[ (INT)'K'];			// 4B
	this->K.lo			= pImpl->g.tbl_lo[ (INT)'K'];			// 4B
	this->K.rep			= pImpl->g.tbl_rep[ (INT)'K'];			// 4B

	this->L.on			= pImpl->g.tbl_on[ (INT)'L'];			// 4C
	this->L.hi			= pImpl->g.tbl_hi[ (INT)'L'];			// 4C
	this->L.lo			= pImpl->g.tbl_lo[ (INT)'L'];			// 4C
	this->L.rep			= pImpl->g.tbl_rep[ (INT)'L'];			// 4C

	this->M.on			= pImpl->g.tbl_on[ (INT)'M'];			// 4D
	this->M.hi			= pImpl->g.tbl_hi[ (INT)'M'];			// 4D
	this->M.lo			= pImpl->g.tbl_lo[ (INT)'M'];			// 4D
	this->M.rep			= pImpl->g.tbl_rep[ (INT)'M'];			// 4D

	this->N.on			= pImpl->g.tbl_on[ (INT)'N'];			// 4E
	this->N.hi			= pImpl->g.tbl_hi[ (INT)'N'];			// 4E
	this->N.lo			= pImpl->g.tbl_lo[ (INT)'N'];			// 4E
	this->N.rep			= pImpl->g.tbl_rep[ (INT)'N'];			// 4E

	this->O.on			= pImpl->g.tbl_on[ (INT)'O'];			// 4F
	this->O.hi			= pImpl->g.tbl_hi[ (INT)'O'];			// 4F
	this->O.lo			= pImpl->g.tbl_lo[ (INT)'O'];			// 4F
	this->O.rep			= pImpl->g.tbl_rep[ (INT)'O'];			// 4F

	this->P.on			= pImpl->g.tbl_on[ (INT)'P'];			// 50
	this->P.hi			= pImpl->g.tbl_hi[ (INT)'P'];			// 50
	this->P.lo			= pImpl->g.tbl_lo[ (INT)'P'];			// 50
	this->P.rep			= pImpl->g.tbl_rep[ (INT)'P'];			// 50

	this->Q.on			= pImpl->g.tbl_on[ (INT)'Q'];			// 51
	this->Q.hi			= pImpl->g.tbl_hi[ (INT)'Q'];			// 51
	this->Q.lo			= pImpl->g.tbl_lo[ (INT)'Q'];			// 51
	this->Q.rep			= pImpl->g.tbl_rep[ (INT)'Q'];			// 51

	this->R.on			= pImpl->g.tbl_on[ (INT)'R'];			// 52
	this->R.hi			= pImpl->g.tbl_hi[ (INT)'R'];			// 52
	this->R.lo			= pImpl->g.tbl_lo[ (INT)'R'];			// 52
	this->R.rep			= pImpl->g.tbl_rep[ (INT)'R'];			// 52

	this->S.on			= pImpl->g.tbl_on[ (INT)'S'];			// 53
	this->S.hi			= pImpl->g.tbl_hi[ (INT)'S'];			// 53
	this->S.lo			= pImpl->g.tbl_lo[ (INT)'S'];			// 53
	this->S.rep			= pImpl->g.tbl_rep[ (INT)'S'];			// 53

	this->T.on			= pImpl->g.tbl_on[ (INT)'T'];			// 54
	this->T.hi			= pImpl->g.tbl_hi[ (INT)'T'];			// 54
	this->T.lo			= pImpl->g.tbl_lo[ (INT)'T'];			// 54
	this->T.rep			= pImpl->g.tbl_rep[ (INT)'T'];			// 54

	this->U.on			= pImpl->g.tbl_on[ (INT)'U'];			// 55
	this->U.hi			= pImpl->g.tbl_hi[ (INT)'U'];			// 55
	this->U.lo			= pImpl->g.tbl_lo[ (INT)'U'];			// 55
	this->U.rep			= pImpl->g.tbl_rep[ (INT)'U'];			// 55

	this->V.on			= pImpl->g.tbl_on[ (INT)'V'];			// 56
	this->V.hi			= pImpl->g.tbl_hi[ (INT)'V'];			// 56
	this->V.lo			= pImpl->g.tbl_lo[ (INT)'V'];			// 56
	this->V.rep			= pImpl->g.tbl_rep[ (INT)'V'];			// 56

	this->W.on			= pImpl->g.tbl_on[ (INT)'W'];			// 57
	this->W.hi			= pImpl->g.tbl_hi[ (INT)'W'];			// 57
	this->W.lo			= pImpl->g.tbl_lo[ (INT)'W'];			// 57
	this->W.rep			= pImpl->g.tbl_rep[ (INT)'W'];			// 57

	this->X.on			= pImpl->g.tbl_on[ (INT)'X'];			// 58
	this->X.hi			= pImpl->g.tbl_hi[ (INT)'X'];			// 58
	this->X.lo			= pImpl->g.tbl_lo[ (INT)'X'];			// 58
	this->X.rep			= pImpl->g.tbl_rep[ (INT)'X'];			// 58

	this->Y.on			= pImpl->g.tbl_on[ (INT)'Y'];			// 59
	this->Y.hi			= pImpl->g.tbl_hi[ (INT)'Y'];			// 59
	this->Y.lo			= pImpl->g.tbl_lo[ (INT)'Y'];			// 59
	this->Y.rep			= pImpl->g.tbl_rep[ (INT)'Y'];			// 59

	this->Z.on			= pImpl->g.tbl_on[ (INT)'Z'];			// 5A
	this->Z.hi			= pImpl->g.tbl_hi[ (INT)'Z'];			// 5A
	this->Z.lo			= pImpl->g.tbl_lo[ (INT)'Z'];			// 5A
	this->Z.rep			= pImpl->g.tbl_rep[ (INT)'Z'];			// 5A

	this->F1.on			= pImpl->g.tbl_on[ VK_F1 ];			// 70
	this->F1.hi			= pImpl->g.tbl_hi[ VK_F1 ];			// 70
	this->F1.lo			= pImpl->g.tbl_lo[ VK_F1 ];			// 70
	this->F1.rep		= pImpl->g.tbl_rep[ VK_F1 ];			// 70

	this->F2.on			= pImpl->g.tbl_on[ VK_F2 ];			// 71
	this->F2.hi			= pImpl->g.tbl_hi[ VK_F2 ];			// 71
	this->F2.lo			= pImpl->g.tbl_lo[ VK_F2 ];			// 71
	this->F2.rep		= pImpl->g.tbl_rep[ VK_F2 ];			// 71

	this->F3.on			= pImpl->g.tbl_on[ VK_F3 ];			// 72
	this->F3.hi			= pImpl->g.tbl_hi[ VK_F3 ];			// 72
	this->F3.lo			= pImpl->g.tbl_lo[ VK_F3 ];			// 72
	this->F3.rep		= pImpl->g.tbl_rep[ VK_F3 ];			// 72

	this->F4.on			= pImpl->g.tbl_on[ VK_F4 ];			// 73
	this->F4.hi			= pImpl->g.tbl_hi[ VK_F4 ];			// 73
	this->F4.lo			= pImpl->g.tbl_lo[ VK_F4 ];			// 73
	this->F4.rep		= pImpl->g.tbl_rep[ VK_F4 ];			// 73

	this->F5.on			= pImpl->g.tbl_on[ VK_F5 ];			// 74
	this->F5.hi			= pImpl->g.tbl_hi[ VK_F5 ];			// 74
	this->F5.lo			= pImpl->g.tbl_lo[ VK_F5 ];			// 74
	this->F5.rep		= pImpl->g.tbl_rep[ VK_F5 ];			// 74

	this->F6.on			= pImpl->g.tbl_on[ VK_F6 ];			// 75
	this->F6.hi			= pImpl->g.tbl_hi[ VK_F6 ];			// 75
	this->F6.lo			= pImpl->g.tbl_lo[ VK_F6 ];			// 75
	this->F6.rep		= pImpl->g.tbl_rep[ VK_F6 ];			// 75

	this->F7.on			= pImpl->g.tbl_on[ VK_F7 ];			// 76
	this->F7.hi			= pImpl->g.tbl_hi[ VK_F7 ];			// 76
	this->F7.lo			= pImpl->g.tbl_lo[ VK_F7 ];			// 76
	this->F7.rep		= pImpl->g.tbl_rep[ VK_F7 ];			// 76

	this->F8.on			= pImpl->g.tbl_on[ VK_F8 ];			// 77
	this->F8.hi			= pImpl->g.tbl_hi[ VK_F8 ];			// 77
	this->F8.lo			= pImpl->g.tbl_lo[ VK_F8 ];			// 77
	this->F8.rep		= pImpl->g.tbl_rep[ VK_F8 ];			// 77

	this->F9.on			= pImpl->g.tbl_on[ VK_F9 ];			// 78
	this->F9.hi			= pImpl->g.tbl_hi[ VK_F9 ];			// 78
	this->F9.lo			= pImpl->g.tbl_lo[ VK_F9 ];			// 78
	this->F9.rep		= pImpl->g.tbl_rep[ VK_F9 ];			// 78

	this->F10.on		= pImpl->g.tbl_on[ VK_F10 ];			// 79
	this->F10.hi		= pImpl->g.tbl_hi[ VK_F10 ];			// 79
	this->F10.lo		= pImpl->g.tbl_lo[ VK_F10 ];			// 79
	this->F10.rep		= pImpl->g.tbl_rep[ VK_F10 ];			// 79

	this->_1.on			= pImpl->g.tbl_on[ (int)'1' ];			// 
	this->_1.hi			= pImpl->g.tbl_hi[ (int)'1' ];			// 
	this->_1.lo			= pImpl->g.tbl_lo[ (int)'1' ];			// 
	this->_1.rep		= pImpl->g.tbl_rep[ (int)'1' ];			// 

	this->_2.on			= pImpl->g.tbl_on[ (int)'2' ];			// 
	this->_2.hi			= pImpl->g.tbl_hi[ (int)'2' ];			// 
	this->_2.lo			= pImpl->g.tbl_lo[ (int)'2' ];			// 
	this->_2.rep		= pImpl->g.tbl_rep[ (int)'2' ];			// 

	this->_3.on			= pImpl->g.tbl_on[ (int)'3' ];			// 
	this->_3.hi			= pImpl->g.tbl_hi[ (int)'3' ];			// 
	this->_3.lo			= pImpl->g.tbl_lo[ (int)'3' ];			// 
	this->_3.rep		= pImpl->g.tbl_rep[ (int)'3' ];			// 

	this->_4.on			= pImpl->g.tbl_on[ (int)'4' ];			// 
	this->_4.hi			= pImpl->g.tbl_hi[ (int)'4' ];			// 
	this->_4.lo			= pImpl->g.tbl_lo[ (int)'4' ];			// 
	this->_4.rep		= pImpl->g.tbl_rep[ (int)'4' ];			// 

	this->_5.on			= pImpl->g.tbl_on[ (int)'5' ];			// 
	this->_5.hi			= pImpl->g.tbl_hi[ (int)'5' ];			// 
	this->_5.lo			= pImpl->g.tbl_lo[ (int)'5' ];			// 
	this->_5.rep		= pImpl->g.tbl_rep[ (int)'5' ];			// 

	this->_6.on			= pImpl->g.tbl_on[ (int)'6' ];			// 
	this->_6.hi			= pImpl->g.tbl_hi[ (int)'6' ];			// 
	this->_6.lo			= pImpl->g.tbl_lo[ (int)'6' ];			// 
	this->_6.rep		= pImpl->g.tbl_rep[ (int)'6' ];			// 

	this->_7.on			= pImpl->g.tbl_on[ (int)'7' ];			// 
	this->_7.hi			= pImpl->g.tbl_hi[ (int)'7' ];			// 
	this->_7.lo			= pImpl->g.tbl_lo[ (int)'7' ];			// 
	this->_7.rep		= pImpl->g.tbl_rep[ (int)'7' ];			// 

	this->_8.on			= pImpl->g.tbl_on[ (int)'8' ];			// 
	this->_8.hi			= pImpl->g.tbl_hi[ (int)'8' ];			// 
	this->_8.lo			= pImpl->g.tbl_lo[ (int)'8' ];			// 
	this->_8.rep		= pImpl->g.tbl_rep[ (int)'8' ];			// 

	this->_9.on			= pImpl->g.tbl_on[ (int)'9' ];			// 
	this->_9.hi			= pImpl->g.tbl_hi[ (int)'9' ];			// 
	this->_9.lo			= pImpl->g.tbl_lo[ (int)'9' ];			// 
	this->_9.rep		= pImpl->g.tbl_rep[ (int)'9' ];			// 

	this->_0.on			= pImpl->g.tbl_on[ (int)'0' ];			// 
	this->_0.hi			= pImpl->g.tbl_hi[ (int)'0' ];			// 
	this->_0.lo			= pImpl->g.tbl_lo[ (int)'0' ];			// 
	this->_0.rep		= pImpl->g.tbl_rep[ (int)'0' ];			// 

	this->SEMICOLON.on	= pImpl->g.tbl_on[ VK_OEM_1 ];			// BA
	this->SEMICOLON.hi	= pImpl->g.tbl_hi[ VK_OEM_1 ];			// BA
	this->SEMICOLON.lo	= pImpl->g.tbl_lo[ VK_OEM_1 ];			// BA
	this->SEMICOLON.rep	= pImpl->g.tbl_rep[ VK_OEM_1 ];		// BA

	this->COMMA.on		= pImpl->g.tbl_on[ VK_OEM_COMMA ];		// BC
	this->COMMA.hi		= pImpl->g.tbl_hi[ VK_OEM_COMMA ];		// BC
	this->COMMA.lo		= pImpl->g.tbl_lo[ VK_OEM_COMMA ];		// BC
	this->COMMA.rep		= pImpl->g.tbl_rep[ VK_OEM_COMMA ];	// BC

	this->PERIOD.on		= pImpl->g.tbl_on[ VK_OEM_PERIOD ];	// BE
	this->PERIOD.hi		= pImpl->g.tbl_hi[ VK_OEM_PERIOD ];	// BE
	this->PERIOD.lo		= pImpl->g.tbl_lo[ VK_OEM_PERIOD ];	// BE
	this->PERIOD.rep	= pImpl->g.tbl_rep[ VK_OEM_PERIOD ];	// BE

#if 0
	 // キーコードチェック用
	int i;
	for ( i = 0 ; i < 255 ; i++ )
	{
		if ( pImpl->g.state[i] & 0x80 )
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
