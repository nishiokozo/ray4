//2019/11/28


#include <iostream>
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

#include "SysKeys.h"
#include "SysMouse.h"
#include "SysGra.h"

#include "func.h"
#include "pers.h"

#include "lab.h"
#include "lab9.h"

static const float	G	= -9.80665;				// 重力加速度
static const rgb col0 = rgb( 0, 0, 0 );
static const rgb col1 = rgb( 0, 0, 1 );
static const rgb col2 = rgb( 1, 0, 0 );
static const rgb col3 = rgb( 1, 0, 1 );
static const rgb col4 = rgb( 0, 1, 0 );
static const rgb col5 = rgb( 0, 1, 1 );
static const rgb col6 = rgb( 1, 1, 0 );
static const rgb col7 = rgb( 1, 1, 1 );



// 定義
struct Ball9:Obj
{
	vect3	vel;	//	velocity 速度(m/s)
	float	radius = 0.1;
	mat33	mat = midentity();
	bool	flgOn = false;	// 接地フラグ
	vect3	Q;
	vect3	vaxis;
	float	fspin;

	vect3	pn;
	vect3	vn;
	mat33	mn = midentity();

	Ball9() : Obj(vect3(0,0,0)) {}

	Ball9( vect3 v, vect3 _vel ) : Obj(v)
	{
		vel = _vel;
	}
	
};

//------------------------------------------------------------------------------
void Lab9::Update( SysKeys& keys, SysMouse& mouse, SysGra& gra, Pers& pers, float delta, int& text_y, Cp& cp )
//------------------------------------------------------------------------------
{
	m.bStep = false;

	//画面クリア
	gra.Clr(rgb(0.3,0.3,0.3));
//	pers.grid.DrawGrid3d( gra, pers, vect3(0,0,0), midentity(), 16, 16, 1, rgb(0.2,0.2,0.2) );
	gra.Print(1,(float)text_y++,string("9 : Ball & Ball & Gravity")); 

	if ( !m.bInitAll )
	{
		m.bInitAll = true;

		// カメラ
		pers.cam.pos = vect3(	0.0,	10.0, -30.0 );
		pers.cam.at = vect3( 	0.0,	1.0, 0.0 );

		//点
		for ( Obj* p : m.tbl_pObj ) delete p;
		m.tbl_pObj.clear();
		m.tbl_pObj.emplace_back( new Obj(vect3( 0.05	, 0.0,	0.01 )) );	// 平面原点
		m.tbl_pObj.emplace_back( new Obj(vect3( 0.0		, 0.5,  0.0 )) );	// 平面法線
		m.tbl_pObj.emplace_back( new Ball9 );
		m.tbl_pObj.emplace_back( new Ball9 );

		//GUI登録
		cp.tbltbl_pObj.emplace_back( m.tbl_pObj );

	}

	// 設定値
	vect3	vg	= vect3(0,G,0);		// 重力加速度ベクトル
	vect3	plate_p	= m.tbl_pObj[0]->pos;
	vect3	plate_n	= (m.tbl_pObj[1]->pos-plate_p).normalize();
	Ball9&	b1 = *dynamic_cast<Ball9*>(m.tbl_pObj[2]);
	Ball9&	b2 = *dynamic_cast<Ball9*>(m.tbl_pObj[3]);

	// 初期化：パラメータ
	if ( !m.bInitParam )
	{
	// b1 b2 正面衝突°
		m.bInitParam = true;
		{
			b1.pos		= vect3( -3	, 6.0,  0.0 );
			b1.vel		= vect3(  0	, 0.0,  0.0 );
			b1.radius 	= 1.0;
			b1.mat		= midentity();
			b1.vaxis	= vect3(0,0,1);
			b1.fspin	= 0.0;
		}
		{
			b2.pos		= vect3(  2	, 4.0,  0.0 );
			b2.vel		= vect3(  0	, 0.0,  0.0 );
			b2.radius 	= 1.0;
			b2.mat		= midentity();
			b2.vaxis	= vect3(0,0,1);
			b2.fspin	= 0.0;
		}

		// 転がり
		{
			float ene = 7;
			vect3 dir = vect3(1,0,0).normalize();
			b1.vel += ene * dir;
		}
		{
			float ene = 1;
			vect3 dir = vect3(1,0,0).normalize();
			b2.vel += ene * dir;
		}

	}
//static float time = 0;cout << time << " " ;b1.pos.dump();time += delta;

	// 入力
	{
		if ( keys.R.hi )		m.bInitParam = false;
		if ( keys.SPACE.hi )	m.bPause = !m.bPause ;
		if ( keys.ENTER.rep )	{m.bStep = true; m.bPause = true; }
		if ( keys.O.hi )	{pers.bOrtho = !pers.bOrtho;}
	}


	// 移動
	if ( keys.S.hi )
	{
		float ene = 6;
		vect3 dir = vect3(1,0,0);
		b2.vel += ene * dir;
	}

	// 移動
	b1.vn = b1.vel + vg * delta;
	b2.vn = b2.vel + vg * delta;
	
	b1.pn = b1.pos + b1.vn * delta;
	b2.pn = b2.pos + b2.vn * delta;

	//衝突
	// 回転量を移動量計算で求める
	auto funcSpin = []( Ball9& ball, vect3& N )
	{
		vect3	d = ball.pn - ball.pos;
		float	r = ball.radius;

		vect3	axis = cross(d,N);
		vect3	mov = d - dot(d,N)*N;
		float	th = mov.abs()/r;

		ball.vaxis = axis;
		ball.fspin = th;
	}; 

	// ボールの反射
	auto funcBound = [&]( Ball9& ball, vect3& N )
	{
		vect3 d = ball.vn * delta;
		ball.pn -= dot( d, N ) * N;
		ball.vn = func_reflect( ball.vn, N, (1.0-pow(1-0.3,2)) );
		funcSpin(ball, N);
	};

	// 床との衝突
	{
		auto[flg,q0,t] = func_intersect_Plate_SegCurve_ball( plate_p, plate_n, b1.pos, b1.radius, -0.0001, delta, dot(vg,plate_n)*plate_n, dot(b1.vn,plate_n)*plate_n );
		b1.flgOn = flg;
		b1.Q = q0;
		if ( b1.flgOn )
		{
			funcBound( b1, plate_n );
		}
	}
	{
		auto[flg,q0,t] = func_intersect_Plate_SegCurve_ball( plate_p, plate_n, b2.pos, b2.radius, -0.0001, delta, dot(vg,plate_n)*plate_n, dot(b2.vn,plate_n)*plate_n );
		b2.flgOn = flg;
		b2.Q = q0;
		if ( b2.flgOn )
		{
			funcBound( b2, plate_n );
		}
	}

	// ボールとの衝突
	{
		if ( (b1.pn-b2.pn).abs() <b1.radius+b2.radius )
		{

			vect3	N = (b1.pos-b2.pos).normalize();

			funcSpin(b1, N);
			funcSpin(b2, N);

			vect3	vel1 = func_reflect( b1.vn, N, 1.0 );
			vect3	vel2 = func_reflect( b2.vn, N, 1.0 );

			vect3 v1 = dot(-vel2,N)*N;
			vect3 v2 = dot(-vel1,N)*N;

			b1.pn = b1.pos;
			b2.pn = b2.pos;
			b1.vn += v1 - v2;
			b2.vn += v2 - v1;
		}
	}

	
	// 反映
	if  ( !m.bPause || m.bStep )
	{
		b1.pos = b1.pn;
		b2.pos = b2.pn;
		b1.vel = b1.vn;
		b2.vel = b2.vn;
		b1.mat = mrotateByAxis( b1.vaxis, b1.fspin ) * b1.mat;
		b2.mat = mrotateByAxis( b2.vaxis, b2.fspin ) * b2.mat;
	}
	
	m.drawVect( gra, pers, text_y, b1.pos, b1.vaxis.normalize() ,1	, col3, "axis" );
	m.drawVect( gra, pers, text_y, b2.pos, b2.vaxis.normalize() ,1	, col3, "axis" );


	// 平面表示
	{
		pers.prim.DrawPlate( gra, pers, plate_p, plate_n, 28, rgb(0.5,1,1)*0.55 );
	}
	
	
	// 表示
	pers.prim.DrawSphere( gra, pers, b1.radius, b1.pos, b1.mat );
	pers.prim.DrawSphere( gra, pers, b2.radius, b2.pos, b2.mat );

	// 接点表示
	if ( b1.flgOn )	pers.pen.pset3d( gra, pers, b1.Q , col6, 9);
	if ( b2.flgOn )	pers.pen.pset3d( gra, pers, b2.Q , col6, 9);


}
