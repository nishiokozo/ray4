#include	"geom.h"
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

	SysMouse();		//	間違って直接生成しないようにprivateで定義しておく
	~SysMouse();	//	間違って直接生成しないようにprivateで定義しておく
#if 0
	vect2	prev;	//	マウスカーソル位置旧
	vect2	pos;	//	マウスカーソル位置
	vect2	mov;	//	マウスカーソル移動量
#endif

public:
	static SysMouse& GetInstance();

	Keyobj	L;	//左ボタン
	Keyobj	R;	//右ボタン
	Keyobj	M;	//中ボタン
	Keyobj	F;	//フォワードボタン
	Keyobj	B;	//バックボタン


	vect2	prev;	//	マウスカーソル位置旧 GL座標
	vect2	pos;	//	マウスカーソル位置 GL座標
	vect2	mov;	//	マウスカーソル移動量 GL座標

//	int	sx;	//	マウスカーソル位置
//	int	sy;
//	int	mx;	//	マウスカーソル移動量
//	int	my;

	float	wheel;
/*
	struct 
	{
		int	pos_x;	//画面左上座標	
		int	pos_y;	//画面左上座標
		int	width;
		int	height;
	} m;
*/
	void Update();

//	void OnSize( int width, int height );
//	void OnMove( int pos_x, int pos_y );

};


