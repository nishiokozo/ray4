typedef struct
{
	bool	backspace;			// 08 VK_BACK
	bool	tab;				// 09 VK_TAB
	bool	enter;				// 0D VK_RETURN
	bool	shift;				// 10 VK_SHIFT
	bool	ctrl;				// 11 VK_CONTROL
	bool	alt;				// 12 VK_MENU
	bool	capslock;				// 14 VK_CAPITAL
	bool	esc;				// 1B VK_ESCAPE
	bool	space;				// 20 VK_SPACE
	bool	pageup;				// 21 VK_PRIOR
	bool	pagedown;			// 22 VK_NEXT
	bool	end;				// 23 VK_END
	bool	home;				// 24 VK_HOME
	bool	left;				// 25 VK_LEFT
	bool	up;					// 26 VK_UP
	bool	right;				// 27 VK_RIGHT
	bool	down;				// 28 VK_DOWN
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
	bool	semicolon;			// BA VK_OEM_1
	bool	comma;				// BC VK_OEM_COMMA
	bool	period;				// BE VK_OEM_PERIOD

} KEY_MAP;

class KEY_INF
{
public:
	KEY_MAP	prev;
	KEY_MAP	on;
	KEY_MAP	hi;
	KEY_MAP	lo;
	KEY_MAP	rep;

	KEY_INF();
	void	Update();

	KEY_INF& key_getInstance();

} ;


int key_init();
KEY_INF& key_getInstance();

extern	KEY_INF	key;
