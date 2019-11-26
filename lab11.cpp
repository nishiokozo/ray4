//2019/11/25

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
#include "lab11.h"

static	const float	G	= -9.80665;				// 重力加速度
static	const rgb col0 = rgb( 0, 0, 0 );
static	const rgb col1 = rgb( 0, 0, 1 );
static	const rgb col2 = rgb( 1, 0, 0 );
static	const rgb col3 = rgb( 1, 0, 1 );
static	const rgb col4 = rgb( 0, 1, 0 );
static	const rgb col5 = rgb( 0, 1, 1 );
static	const rgb col6 = rgb( 1, 1, 0 );
static	const rgb col7 = rgb( 1, 1, 1 );

struct Lab11::Impl{};
Lab11::Lab11() : pImpl( new Lab11::Impl ){}

struct Ball:Obj
{
	vect3	vel;	//	velocity 速度(m/s)
	float	radius = 1.0;
	mat33	mat = midentity();
	mat33	mspin = midentity();
	Ball( vect3 v, vect3 _vel ) : Obj(v)
	{
		pos = v;
		vel = _vel;
	}
};

//------------------------------------------------------------------------------
void Lab11::Update( SysKeys& keys, SysMouse& mouse, SysGra& gra, Pers& pers, float delta, int& text_y, Cp& cp )
//------------------------------------------------------------------------------
{
	m.bStep = false;

	//画面クリア
	gra.Clr(rgb(0.3,0.3,0.3));
//	pers.grid.DrawGrid3d( gra, pers, vect3(0,0,0), midentity(), 10, 10, 1, rgb(0.2,0.2,0.2) );

	gra.Print(1,(float)text_y++,string("11 : Plate & Ball")); 

	// 定数
	vect3	vg	= vect3(0,G,0);		// 重力加速度ベクトル

	// 初期化：オール
	if ( !m.bInitAll )
	{
		m.bInitAll = true;

		// カメラ
		pers.cam.pos = vect3(	0.0,	1.0, -8.0 );
		pers.cam.at = vect3( 	0.0,	1.0, 0.0 );

		//点
		for ( Obj* p : m.tbl_pObj ) delete p;
		m.tbl_pObj.clear();
		m.tbl_pObj.emplace_back( new Ball(vect3(  0		, 0.62,  0.0 ), vect3(0,0,0)) );
		m.tbl_pObj.emplace_back( new Obj(vect3( -0.0	, 0.0,	0.0 )) );	// 平面原点
		m.tbl_pObj.emplace_back( new Obj(vect3( -0.05	, 0.5,  0.0 )) );	// 平面法線

		// 線
		for ( Edge* p : m.tbl_pEdge ) delete p;
		m.tbl_pEdge.clear();
		m.tbl_pEdge.emplace_back( new Edge(1,2, col7,1) );

		// 移動軸
		pers.axis.bAxisX = true;
		pers.axis.bAxisY = false;
		pers.axis.bAxisZ = false;

		//GUI登録
		cp.tbltbl_pObj.clear();
		cp.tbltbl_pEdge.clear();
		cp.tbltbl_pObj.emplace_back( m.tbl_pObj );
		cp.tbltbl_pEdge.emplace_back( m.tbl_pEdge );
	}

	// 初期化：パラメータ
	if ( !m.bInitParam )
	{
		m.bInitParam = true;
		Ball&	ball = *dynamic_cast<Ball*>(m.tbl_pObj[0]);
		ball.pos = vect3(  0		, 2.0,  0.0 );
		ball.vel = vect3(  0		, 0.0,  0.0 );
		ball.mat = midentity();
		ball.mspin = midentity();
	}

	// 入力
	{
		if ( keys.R.hi ) m.bInitParam = false;
		if ( keys.SPACE.hi )	m.bPause = !m.bPause ;
		if ( keys.ENTER.rep )	{m.bStep = true; m.bPause = true; }

		if ( keys.G.hi )	{if ( vg.z==G ) vg=vect3(0,0,0); else vg=vect3(0,G,0);}
		if ( keys.A.hi )	{m.tbl_pObj[2]->pos.z+=-1.0;}
		if ( keys.O.hi )	{pers.bOrtho = !pers.bOrtho;}
	
	}
	
	//変数
	Ball&	ball = *dynamic_cast<Ball*>(m.tbl_pObj[0]);
	vect3	plate_p = m.tbl_pObj[1]->pos;
	vect3	plate_n = (m.tbl_pObj[2]->pos-plate_p).normalize();
	vect3	p0 = ball.pos;
	vect3	v0 = ball.vel;
	vect3	pn;
	vect3	vn;
	mat33	mn = midentity();

	//-----

	vect3	d = func_accelerationGetDistance_TVv( vg, delta, v0 );	// 移動距離(m)

	// 通常加速計算
	{
		pn = p0 + d;		// 仮移動
		vn = v0 + vg*delta;	// 仮速度
	}

	vect3 vr = plate_n*ball.radius;
	auto[flg,q0,t] = func_intersect_Plate_SegCurve( plate_p+vr, plate_n, p0, -0.0001, delta, dot(vg,plate_n)*plate_n, dot(v0,plate_n)*plate_n );


	// 衝突計算
	if ( flg )
	{
		// 衝突まで
		float t1 = t;													// 衝突までの時間(s)
		vect3 d1 = func_accelerationGetDistance_TVv( vg, t1, v0 );		// 衝突までの移動距離(m)
		vect3 v1 = v0 + vg*t1;											// 衝突後の速度(m/s)

		float rate_r	= 0.6;		// 反射係数
		v1  = v1 - (1.0+rate_r)*dot( v1, plate_n ) * plate_n;

		// 衝突後
		float t2 = delta-t1;											// 衝突後の残り時間(s)
		vect3 d2 = func_accelerationGetDistance_TVv( vg, t2, v1 );		// 衝突後の移動距離(m)
		vect3 v2 = v1 + vg*t2;											// 衝突後の速度(m/s)

		// 極小衝突
		if ( dot(d2,plate_n) < 0 )
		{
			// 近似：本来はplate面での無限衝突
			d2 -= dot(d2,plate_n) * plate_n;
		}

		pn = p0 + d1 + d2;
		vn = v2 ;
		

		// 回転量を移動量計算で求める
		{
			vect3	axis = cross(d,plate_n);
			vect3	mov = d - dot(d,plate_n)*plate_n;
			float	th = mov.abs()/ball.radius;
			mn = mrotateByAxis( axis, th );
			ball.mspin = mn;
		}


	}


	// 反映
	if  ( !m.bPause || m.bStep )
	{
		ball.pos = pn;
		ball.vel = vn;
		ball.mat *= ball.mspin;
	}

	
	// 平面表示
	{
		pers.prim.DrawPlate( gra, pers, plate_p, plate_n, 28, rgb(0.5,1,1)*0.55 );
	}


	// ボール表示
	pers.prim.DrawSphere( gra, pers, ball.radius, ball.pos, ball.mat );



}
