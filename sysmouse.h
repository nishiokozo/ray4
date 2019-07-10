//int mouse_update();
//int mouse_init( );
//int mouse_mouse(int button, int state, int x, int y);
//int	 mosue_resize(int w, int h);

typedef struct 
{
	int	l;
	int	r;
} MOUSE_TYPE;

class SysMouse
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

	SysMouse();	//	間違って直接生成しないようにprivateで定義しておく
	~SysMouse();	//	間違って直接生成しないようにprivateで定義しておく

public:
	static SysMouse& GetInstance();

	Keyobj	L;
	Keyobj	R;
	Keyobj	M;

	float	sx;	//	マウスカーソル位置
	float	sy;
	float	mx;	//	マウスカーソル移動量
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
	vector	vm;	//	移動量
	vector	vw;	//	位置
	vector	vf;	//	位置（アンチチャタリング）
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
//	int	state;

	void Update();

};

//extern	MOUSE_INF	mouse;


