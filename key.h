typedef struct
{
	int	a;
	int	b;
	int	c;
	int	d;
	int	e;
	int	f;
	int	g;
	int	h;
	int	i;
	int	j;
	int	k;
	int	l;
	int	m;
	int	n;
	int	o;
	int	p;
	int	q;
	int	r;
	int	s;
	int	t;
	int	u;
	int	v;
	int	w;
	int	x;
	int	y;
	int	z;
	int	_1;
	int	_2;
	int	_3;
	int	_4;
	int	_5;
	int	_6;
	int	_7;
	int	_8;
	int	_9;
	int	_0;

	int	f1;
	int	f2;
	int	f3;
	int	f4;
	int	f5;
	int	f6;
	int	f7;
	int	f8;
	int	f9;
	int	f10;
	int	f11;
	int	f12;
	int	f13;
	int	f14;
	int	f15;
	int	f16;

	int	esc;
	int	tab;
	int	capsLock;
	int	enter;
	int	space;
	int	shift;
	int	shift_l;
	int	shift_r;
	int	ctrl;
	int	ctrl_l;
	int	ctrl_r;
	int	alt;
	int	alt_l;
	int	alt_r;

	int	scrollLock;
	int	pause;

	int	pageUp;
	int	pageDown;
	int	end;
	int	home;
	int	insert;
	int	del;

	int	left;
	int	up;
	int	right;
	int	down;

	int	numLock;
	int	num_0;
	int	num_1;
	int	num_2;
	int	num_3;
	int	num_4;
	int	num_5;
	int	num_6;
	int	num_7;
	int	num_8;
	int	num_9;
	int	num_multiply;
	int	num_add;
	int	num_enter;
	int	num_sub;
	int	num_dot;
	int	num_devide;

} KEY_TYPE;

typedef struct
{
	KEY_TYPE	on;
	KEY_TYPE	hi;
	KEY_TYPE	lo;
	KEY_TYPE	rep;
} KEY_INF;


extern	KEY_INF	key;
int	key_update();
//int	key_isPuress( int num );
int key_init();
