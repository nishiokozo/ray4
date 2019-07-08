

class Keyboard
{

	struct Keyobj
	{
		bool	on;
		bool	hi;
		bool	lo;
		bool	rep;
		Keyobj()
		{
			on = 0;
			hi = 0;
			lo = 0;
			rep = 0;
		};
	};

	Keyboard() {};	//	間違って直接生成しないようにprivateで定義しておく

public:
	static Keyboard& GetInstance();

	void	Update();


	Keyobj	BACKSPACE;			// 08 VK_BACK
	Keyobj	TAB;				// 09 VK_TAB
	Keyobj	ENTER;				// 0D VK_RETURN
	Keyobj	SHIFT;				// 10 VK_SHIFT
	Keyobj	CTRL;				// 11 VK_CONTROL
	Keyobj	ALT;				// 12 VK_MENU
	Keyobj	CAPSLOCK;			// 14 VK_CAPITAL
	Keyobj	ESC;				// 1B VK_ESCAPE
	Keyobj	SPACE;				// 20 VK_SPACE
	Keyobj	PAGEUP;				// 21 VK_PRIOR
	Keyobj	PAGEDOWN;			// 22 VK_NEXT
	Keyobj	END;				// 23 VK_END
	Keyobj	HOME;				// 24 VK_HOME
	Keyobj	LEFT;				// 25 VK_LEFT
	Keyobj	UP;					// 26 VK_UP
	Keyobj	RIGHT;				// 27 VK_RIGHT
	Keyobj	DOWN;				// 28 VK_DOWN
	Keyobj	_0;					// 30 "0"
	Keyobj	_1;					// 31 "1"
	Keyobj	_2;					// 32 "2"
	Keyobj	_3;					// 33 "3"
	Keyobj	_4;					// 34 "4"
	Keyobj	_5;					// 35 "5"
	Keyobj	_6;					// 36 "6"
	Keyobj	_7;					// 37 "7"
	Keyobj	_8;					// 38 "8"
	Keyobj	_9;					// 39 "9"
	Keyobj	A;					// 41 'A'
	Keyobj	B;					// 42 'B'
	Keyobj	C;					// 43 'C'
	Keyobj	D;					// 44 'D'
	Keyobj	E;					// 45 'E'
	Keyobj	F;					// 46 'F'
	Keyobj	G;					// 47 'G'
	Keyobj	H;					// 48 'H'
	Keyobj	I;					// 49 'I'
	Keyobj	J;					// 4A 'J'
	Keyobj	K;					// 4B 'K'
	Keyobj	L;					// 4C 'L'
	Keyobj	M;					// 4D 'M'
	Keyobj	N;					// 4E 'N'
	Keyobj	O;					// 4F 'O'
	Keyobj	P;					// 50 'P'
	Keyobj	Q;					// 51 'Q'
	Keyobj	R;					// 52 'R'
	Keyobj	S;					// 53 'S'
	Keyobj	T;					// 54 'T'
	Keyobj	U;					// 55 'U'
	Keyobj	V;					// 56 'V'
	Keyobj	W;					// 57 'W'
	Keyobj	X;					// 58 'X'
	Keyobj	Y;					// 59 'Y'
	Keyobj	Z;					// 5A 'Z'
	Keyobj	F1;					// 70 VK_F1
	Keyobj	F2;					// 71 VK_F2
	Keyobj	F3;					// 72 VK_F3
	Keyobj	F4;					// 73 VK_F4
	Keyobj	F5;					// 74 VK_F5
	Keyobj	F6;					// 75 VK_F6
	Keyobj	F7;					// 76 VK_F7
	Keyobj	F8;					// 77 VK_F8
	Keyobj	F9;					// 78 VK_F9
	Keyobj	F10;				// 79 VK_F10
	Keyobj	SEMICOLON;			// BA VK_OEM_1
	Keyobj	COMMA;				// BC VK_OEM_COMMA
	Keyobj	PERIOD;				// BE VK_OEM_PERIOD



} ;
