int mouse_update();
int mouse_init( );
int mouse_mouse(int button, int state, int x, int y);
int	 mosue_resize(int w, int h);

typedef struct 
{
	int	l;
	int	r;
} MOUSE_TYPE;

typedef struct
{
	float	sx;	//	�}�E�X�J�[�\���ʒu
	float	sy;
	float	mx;	//	�}�E�X�J�[�\���ړ���
	float	my;
/*
	float	cx;
	float	cy;
	float	dx;
	float	dy;
	float	screen_w;
	float	screen_h;
*/
//	int	flgDrag;
/*
	vector	vd;
	vector	vm;	//	�ړ���
	vector	vw;	//	�ʒu
	vector	vf;	//	�ʒu�i�A���`�`���^�����O�j
*/

	MOUSE_TYPE	on;
	MOUSE_TYPE	hi;
	MOUSE_TYPE	lo;
	MOUSE_TYPE	rep;
/*
	int	l;
	int	r;
	int	hi_l;
	int	hi_r;
	int	lo_l;
	int	lo_r;
*/
//	int	button;
	int	state;
} MOUSE_INF;

extern	MOUSE_INF	mouse;


