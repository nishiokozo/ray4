//2019/12/08

#include <iostream>
#include <memory>
#include <vector>
#include <map>
#include <thread>
#include <chrono>
#include <cmath>
#include <functional>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iomanip>
#include <tuple>
#include <random>

#include "geom.h"

#include "SysSound.h"
#include "SysKeys.h"
#include "SysMouse.h"
#include "SysGra.h"

#include "func.h"
#include "pers.h"

#include "lab.h"
#include "lab20.h"

struct Lab20::Impl
{
	bool	bResetAll = true;
	bool	bResetParam = true;
	bool	bPause = false;
	bool	bStep = false;

	vector<shared_ptr<Obj>>	tbl_pObj;

	 struct
	{
		float 	radius;
		vect2	pos;
		vect2	vel;
		float	spin;
		float	rot;	
	} ball;

	 struct
	{
		vect2	p0;
		vect2	p1;
		vect2	tan;
		vect2	nor;
	} wall;

	 struct
	{
		float power;
	} motor;

	 	float	valMasatu = 1.0;
};

Lab20::Lab20() : pImpl( new Lab20::Impl ){}

//------------------------------------------------------------------------------
void Lab20::Update( SysKeys& keys, SysMouse& mouse, SysSound& sound, SysGra& gra, Pers& pers, float delta, int& text_y, Cp& cp )
//------------------------------------------------------------------------------
{
	int m_y = 0;

	auto funcShowBar = []( SysGra& gra, int y, float val, string str, rgb col )
	{
		vect2 v0 = vect2(0.0,0.75)+gra.Dot(0,42.0*y);
		gra.Line( v0, v0+ vect2( val, 0 ), col, 2 );
		gra.Print( v0+gra.Dot(0,-6), str + to_string(val), col );
	};

	// 画面クリア
	gra.Clr(rgb(0.3,0.3,0.3));
	pers.grid.DrawGrid3d( gra, pers, vect3(0,0,0), mat33::mrotx(rad(90)), 26, 26, 1, rgb(0.2,0.2,0.2) );
	gra.Print(1,(float)text_y++,"20 : 2D Motor Spin" ); 

	//初期化
	if ( pImpl->bResetAll )
	{
		pImpl->bResetAll = false;

		// カメラ
		pers.cam.pos	= vect3( 0.0, 1.0, -10.0 );
		pers.cam.at		= vect3( 0.0, 1.0, 0 );
		pers.cam.Update();
	}

	// 初期化
	if ( pImpl->bResetParam )
	{
		pImpl->bResetParam = false;

		pImpl->ball.radius	= 1.0;
		pImpl->ball.pos	= vect2( 0, 3.1 );
		pImpl->ball.vel	= vect2( 0, 0 );
		pImpl->ball.spin	= 0;
		pImpl->ball.rot	= 0;

		pImpl->wall.p0 = vect2(-4,-0.2 );
		pImpl->wall.p1 = vect2( 4, 0.2 );
		pImpl->wall.tan = ( pImpl->wall.p1 - pImpl->wall.p0 ).normalize();
		pImpl->wall.nor = vect2( -pImpl->wall.tan.y, pImpl->wall.tan.x );

		pImpl->motor.power = 0;
	}

	// 入力
	if ( keys.R.hi )	pImpl->bResetParam = true;
	if ( mouse.F.on )	pImpl->motor.power += 0.0002;
	if ( mouse.B.on )	pImpl->ball.vel += vect2( 0.003 , 0.0 );
	if ( keys._1.rep )	pImpl->valMasatu -= 0.1;
	if ( keys._2.rep )	pImpl->valMasatu += 0.1;
	pImpl->valMasatu = min( max( pImpl->valMasatu, 0 ), 1.0 );

	// モーター回転伝達
	pImpl->ball.spin += pImpl->motor.power;

	// 落下
	pImpl->ball.vel += vect2( 0, -0.004 );
	pImpl->ball.pos += pImpl->ball.vel;

	if ( dot( pImpl->ball.pos-pImpl->wall.p0, pImpl->wall.nor ) < pImpl->ball.radius )
	{
		// 衝突前に戻す
		pImpl->ball.pos -= pImpl->ball.vel;

		// 移動ベクトルの反射
		pImpl->ball.vel = 	func_reflect( pImpl->ball.vel, pImpl->wall.nor, 0.0 );
		pImpl->ball.pos += pImpl->ball.vel;

		// 回転量を求める
		pImpl->ball.spin = dot( pImpl->ball.vel, pImpl->wall.tan );

	}

	// 描画：ボール
	pImpl->ball.rot += pImpl->ball.spin;
	pers.prim.DrawCircle( gra, pers, vect3( pImpl->ball.pos, 0 ), mat33::mrotz(-pImpl->ball.rot), 1.0, rgb(1,1,1) );

	// 描画：壁
	pers.pen.line3d( gra, pers, vect3(pImpl->wall.p0,0), vect3(pImpl->wall.p1,0) );

	// メーター表示
	funcShowBar( gra, m_y++, pImpl->motor.power,	"pImpl->power    ", rgb(1,1,1) );
	funcShowBar( gra, m_y++, pImpl->ball.vel.abs(),"vel      ", rgb(1,1,1) );
	funcShowBar( gra, m_y++, pImpl->ball.spin, 	"spin     ", rgb(1,1,1) );
	funcShowBar( gra, m_y++, pImpl->valMasatu, 		"pImpl->valMasatu ", rgb(1,1,1) );
	
}
