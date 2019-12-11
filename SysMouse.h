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

public:

	Keyobj	L;	//左ボタン
	Keyobj	R;	//右ボタン
	Keyobj	M;	//中ボタン
	Keyobj	F;	//フォワードボタン
	Keyobj	B;	//バックボタン


	vect2	prev;	//	マウスカーソル位置旧 GL座標
	vect2	pos;	//	マウスカーソル位置 GL座標
	vect2	mov;	//	マウスカーソル移動量 GL座標

	float	wheel;
	void Update();


	SysMouse();
	~SysMouse();
    struct Impl;
    std::unique_ptr<Impl> pImpl;

};


