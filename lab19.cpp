//2019/12/01

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
#include "lab19.h"

using namespace std;


#define	PLATE_MAT	mat33::mrotz(rad(15))
struct Lab19::Impl
{
	bool	bResetAll = true;
	bool	bResetParam = true;
	bool	bPause = false;
	bool	bStep = false;

	vector<shared_ptr<Obj>>	tbl_pObj;

	struct
	{
		vect3	pos	= vect3(0,-0.3,0);
		vect3	nor	= vect3(0,1,0)*PLATE_MAT;
	} plate;

	struct
	{
		float	power = 0.0;
	} motor;

	struct Ball : Obj
	{
		vect3	vel;
		float 	radius;
		vect3	moment;
		mat33	mat = mat33::midentity();

		Ball() : Obj(vect3(0,0,0)){}
	};
};

//------------------------------------------------------------------------------
Lab19::Lab19() : pImpl( new Lab19::Impl )
//------------------------------------------------------------------------------
{
}



//------------------------------------------------------------------------------
void Lab19::Update( SysKeys& keys, SysMouse& mouse, SysSound& sound, SysGra& gra, Pers& pers, float delta, int& text_y, Cp& cp )
//------------------------------------------------------------------------------
{
	int m_y = 0;

	auto funcShowBar = []( SysGra& gra, int y, float val, string str, rgb col )
	{
		vect2 v0 = vect2(0.0,0.75)+gra.Dot(0,42.0*y);
		gra.Line2d( v0, v0+ vect2( val, 0 ), col, 2 );
		gra.Print( v0+gra.Dot(0,-6), str + to_string(val), col );
	};


	// クリア
	gra.Clr(rgb(0.3,0.3,0.3));
	gra.Print(1,(float)text_y++,"19 : ball Moment" );

	// 初期化：ALL
	if ( pImpl->bResetAll )
	{
		pImpl->bResetAll = false;

		// カメラ
		pers.cam.pos = vect3(	0.0,	1.0, -10.0 );
		pers.cam.at = vect3( 	0.0,	1.0,   0.0 );
		pers.cam.Update();

		// 点
		pImpl->tbl_pObj.emplace_back( new Impl::Ball );	// ボール

	}

	// 設定
	Impl::Ball&	ball	= *dynamic_cast<Impl::Ball*>(pImpl->tbl_pObj[0].get());

	// 初期化：パラメータ
	if ( pImpl->bResetParam )
	{
		pImpl->bResetParam = false;

		ball.radius	= 1;
		ball.pos	= vect3(-4,2,0);
		ball.moment	= vect3(0,0,0);
		ball.mat	= mat33::midentity();
		ball.vel	= vect3(0,0,0);
		pImpl->motor.power = 0;
	}

	// 入力
	if ( keys.R.hi )	pImpl->bResetParam = true;
	if ( mouse.F.on )	pImpl->motor.power += 0.01;
	if ( mouse.B.on )	ball.vel += vect3(0.01,0,0);

	// 落下
	ball.vel += vect3(0,-9.8*delta*delta,0);

	// 移動
	ball.pos += ball.vel;

	// 回転
//if ( mouse.F.on )
	{
		vect3 axis = ball.mat.GetVectZ();
		vect3 v = axis * pImpl->motor.power;
		ball.moment += (v - ball.moment);
	}
	


	// 接地
	if ( dot(ball.pos-pImpl->plate.pos,pImpl->plate.nor)-ball.radius < 0)
	{
		// 斜面に沿って球の位置と速度を補正
		ball.pos += -( dot( ball.pos - pImpl->plate.pos , pImpl->plate.nor ) - ball.radius ) * pImpl->plate.nor;
		ball.vel = 	ball.vel - dot( ball.vel , pImpl->plate.nor ) * pImpl->plate.nor;


	#if 1
		// モーメントを速度に変換
		vect3 vel = cross( pImpl->plate.nor, ball.moment );

		ball.vel +=  vel;

		// 斜面の速度を球のモーメントに変換
		vect3 moment = cross( ball.vel, pImpl->plate.nor );

		ball.moment = moment;

	#else
	#if 0
		// 斜面の速度を球の角速度に変換
		ball.moment = cross( ball.vel, pImpl->plate.nor );
	#else
		// モーメントを速度に変換
		ball.vel = cross( pImpl->plate.nor, ball.moment );
	#endif
	#endif
	}
	else
	{
	}
	

	// モデルの回転
	ball.mat = mrotateByAxis( ball.moment , ball.moment.abs() ) * ball.mat;

	// 床表示
	pers.grid.DrawGrid3d( gra, pers, pImpl->plate.pos, PLATE_MAT, 16, 16, 1, rgb(0.2,0.2,0.2) );

	// ボール表示
	pers.prim.DrawSphere( gra, pers, ball.pos, ball.mat, ball.radius );

	// メーター表示
	funcShowBar( gra, m_y++, pImpl->motor.power, 		"power  ", rgb(1,1,1) );
	funcShowBar( gra, m_y++, ball.moment.abs(),	"moment ", rgb(1,1,1) );
	funcShowBar( gra, m_y++, ball.vel.abs(),	"vel    ", rgb(1,1,1) );

	// 1/60sec 待
	gra.Wait();
}
