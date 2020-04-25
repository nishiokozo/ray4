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
#include "lab18.h"

using namespace std;


struct Lab18::Impl
{
	bool	bResetAll = true;
	bool	bResetParam = true;
	bool	bPause = false;
	bool	bStep = false;

	vector<shared_ptr<Obj>>	tbl_pObj;
};

//------------------------------------------------------------------------------
Lab18::Lab18() : pImpl( new Lab18::Impl )
//------------------------------------------------------------------------------
{
}

// 定義
struct Ball18:Obj
{
	vect3	vel;	//	velocity 速度(m/s)
	float	radius = 0.1;
	mat33	mat = mat33::midentity();
	bool	flgOn = false;	// 接地フラグ
	vect3	Q;
	vect3	vaxis;
	float	fspin;

	vect3	pn;
	vect3	vn;
	mat33	mn = mat33::midentity();

	Ball18() : Obj(vect3(0,0,0)) {}

	Ball18( vect3 v, vect3 _vel ) : Obj(v)
	{
		vel = _vel;
	}
	
};

//------------------------------------------------------------------------------
void Lab18::Update( SysKeys& keys, SysMouse& mouse, SysSound& sound, SysGra& gra, Pers& pers, float delta, int& text_y, Cp& cp )
//------------------------------------------------------------------------------
{
	pImpl->bStep = false;

	// 画面クリア
	gra.Clr(rgb(0.3,0.3,0.3));
//	pers.grid.DrawGrid3d( gra, pers, vect3(0,0,0), mat33::mrotx(rad(90)), 16, 16, 1, rgb(0.2,0.2,0.2) );
	gra.Print(1,(float)text_y++,"18 : Spin on Floor" ); 


	if ( pImpl->bResetAll )
	{
		pImpl->bResetAll = false;

		// カメラ
		pers.cam.pos = vect3(	0.0,	1.0, -10.0 );
		pers.cam.at = vect3( 	0.0,	1.0,   0.0 );
		pers.cam.Update();

		//点
		pImpl->tbl_pObj.clear();
		pImpl->tbl_pObj.emplace_back( new Ball18 );
		pImpl->tbl_pObj.emplace_back( new Obj(vect3( 0.15	, 0.0,	0.0 )) );	// 平面原点
		pImpl->tbl_pObj.emplace_back( new Obj(vect3( 0.0		, 1.0,  0.0 )) );	// 平面法線

		//GUI登録
		cp.tbltbl_pObj.emplace_back( pImpl->tbl_pObj );

	}

	// 設定値
	const float	G	= -9.80665;				// 重力加速度
	vect3	vg	= vect3(0,G,0);		// 重力加速度ベクトル
//	vect3	plate_p	= vect3(0,0,0);
//	vect3	plate_n	= vect3(0,1,0);
	Ball18&	b1 = *dynamic_cast<Ball18*>(pImpl->tbl_pObj[0].get());
	vect3	plate_p	= pImpl->tbl_pObj[1]->pos;
	vect3	plate_n	= (pImpl->tbl_pObj[2]->pos-plate_p).normalize();

	// 初期化：パラメータ
	if ( pImpl->bResetParam )
	{
	// b1 正面衝突°
		pImpl->bResetParam = false;
		{
			b1.pos		= vect3(  0	, 2.0,  0.0 );
			b1.vel		= vect3(  0	, 0.0,  0.0 );
			b1.radius 	= 1.0;
			b1.mat		= mat33::midentity();
			b1.vaxis	= vect3(0,0,1);
			b1.fspin	= 0.0;
		}
		// 転がり
		{
			float ene = 0;
			vect3 dir = vect3(1,0,0).normalize();
			b1.vel += ene * dir;
		}
		b1.vaxis = vect3(0,0,-1);
		b1.fspin = -0.1;

	}

	// 入力
	{
		if ( keys.R.hi )	pImpl->bResetParam = true;
		if ( keys.SPACE.hi )	pImpl->bPause = !pImpl->bPause ;
		if ( keys.ENTER.rep )	{pImpl->bStep = true; pImpl->bPause = true; }
		if ( keys.O.hi )	{pers.bOrtho = !pers.bOrtho;}
		if ( keys.S.hi )
		{
			b1.fspin += -0.1;
		}
	}

	// 移動
	b1.vn = b1.vel + vg * delta;
	b1.pn = b1.pos + b1.vn * delta;

	//衝突
	// 回転量を移動量計算で求める
/*
	auto funcSpin = []( Ball18& ball, vect3& N )
	{
		vect3	d = ball.pn - ball.pos;
		float	r = ball.radius;

		vect3	axis = cross(d,N);

		vect3	mov = d - dot(d,N)*N;
		float	th = mov.abs()/r;

		ball.vaxis = axis;
		ball.fspin = th;
	}; 
*/

	// 床との衝突
	{
		auto[flg,q0,t] = func_intersect_Plate_SegCurve_ball( plate_p, plate_n, b1.pos, b1.radius, -0.0001, delta, dot(vg,plate_n)*plate_n, dot(b1.vn,plate_n)*plate_n );
		b1.flgOn = flg;
		b1.Q = q0;
		if ( b1.flgOn )
		{
			// 回転量から移動量を求める。
			{
				Ball18& ball = b1;
				vect3& N = plate_n;
				vect3 d = ball.vn * delta;
				ball.pn -= dot( d, N ) * N;
				ball.vn = vect3(0,0,0);
			};
			{
				vect3	vr	= (q0-b1.pos);
//				float	r	= vr.abs();
				vect3	vf	= cross( vr, b1.vaxis.normalize() );
				b1.pn += vf * -b1.fspin;

			}


		}
	}


	
	// 反映
	if  ( !pImpl->bPause || pImpl->bStep )
	{
		b1.pos = b1.pn;
		b1.vel = b1.vn;
		b1.mat = mrotateByAxis( b1.vaxis, b1.fspin ) * b1.mat;
	}
	
	pers.prim.DrawVect( gra, pers, text_y, b1.pos, b1.vaxis.normalize() ,1	, rgb(1,0,1), "axis" );


	// 平面表示
	pers.prim.DrawPlate( gra, pers, plate_p, plate_n, 28, rgb(0.5,1,1)*0.55 );
	
	// ボール表示
	pers.prim.DrawSphere( gra, pers, b1.pos, b1.mat, b1.radius );

	// 接点表示
	if ( b1.flgOn )	pers.pen.Pset3d( gra, pers, b1.Q , rgb(1,1,0), 9);


	// 1/60sec 待
	gra.Wait();
}
