typedef struct
{
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
	bool	capslock;				// 14 VK_CAPITAL
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
	bool	_3A;				// 3A COLON
	bool	_3B;				// 3B SEMICOLON
	bool	_3C;				// 3C COMMMA
	bool	_3D;				// 3D PERIOD
	bool	_3E;				// 3E (reserved)
	bool	_3F;				// 3F (reserved)
	bool	_40;				// 40 (reserved)
	bool	a;					// 41 "A"
	bool	b;					// 42 "B"
	bool	c;					// 43 "C"
	bool	d;					// 44 "D"
	bool	e;					// 45 "E"
	bool	f;					// 46 "F"
	bool	g;					// 47 "G"
	bool	h;					// 48 "H"
	bool	i;					// 49 "I"
	bool	j;					// 4A "J"
	bool	k;					// 4B "K"
	bool	l;					// 4C "L"
	bool	m;					// 4D "M"
	bool	n;					// 4E "N"
	bool	o;					// 4F "O"
	bool	p;					// 50 "P"
	bool	q;					// 51 "Q"
	bool	r;					// 52 "R"
	bool	s;					// 53 "S"
	bool	t;					// 54 "T"
	bool	u;					// 55 "U"
	bool	v;					// 56 "V"
	bool	w;					// 57 "W"
	bool	x;					// 58 "X"
	bool	y;					// 59 "Y"
	bool	z;					// 5A "Z"
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
} KEY_TYPE;

typedef struct
{
	int	tbl_lim[256];
;					//
	union 
	{
		bool tbl_prev[256];
		KEY_TYPE	prev;
	};
	union 
	{
		bool tbl_on[256];
		KEY_TYPE	on;
	};
	union 
	{
		bool tbl_hi[256];
		KEY_TYPE	hi;
	};
	union 
	{
		bool tbl_lo[256];
		KEY_TYPE	lo;
	};
	union 
	{
		int	tbl_rep[256];
		KEY_TYPE	rep;
	};

} KEY_INF;


extern	KEY_INF	key;
int	key_update();
//int	key_isPuress( int num );
int key_init();
