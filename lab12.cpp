//2019/11/17

//	平面と点の衝突実験


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
#include "lab12.h"

static const float	G	= -9.80665;				// 重力加速度
static const rgb col0 = rgb( 0, 0, 0 );
static const rgb col1 = rgb( 0, 0, 1 );
static const rgb col2 = rgb( 1, 0, 0 );
static const rgb col3 = rgb( 1, 0, 1 );
static const rgb col4 = rgb( 0, 1, 0 );
static const rgb col5 = rgb( 0, 1, 1 );
static const rgb col6 = rgb( 1, 1, 0 );
static const rgb col7 = rgb( 1, 1, 1 );


static	bool	bPause = false;

// 定義
struct Ball:Obj
{
	vect3	vel;	//	velocity 速度(m/s)
	float	radius = 0.1;
	mat33	mat = midentity();
	bool	flgOn = false;	// 接地フラグ

	Ball( vect3 v, vect3 _vel ) : Obj(v)
	{
		pos = v;
		vel = _vel;
	}
};

//------------------------------------------------------------------------------
void Lab12::Update( SysKeys& keys, SysMouse& mouse, SysGra& gra, Pers& pers, float delta, int& text_y, Cp& cp )
//------------------------------------------------------------------------------
{
delta = 1/60.0;
	bool bStep=false;

	//画面クリア
	gra.Clr(rgb(0.3,0.3,0.3));
//	pers.grid.DrawGrid3d( gra, pers, vect3(0,0,0), midentity(), 10, 10, 1, rgb(0.2,0.2,0.2) );

	gra.Print(1,(float)text_y++," : " + string(__func__ )); 

	//----
	// 変数
	vect3	vg	= vect3(0,G,0);		// 重力加速度ベクトル


	// 初期化：オール
	if ( !m.bInitAll )
	{
		m.bInitAll = true;
		//カメラ
		pers.cam.pos = vect3(	0.0,	2.0, -5.0 );
		pers.cam.at = vect3( 	0.0,	0.5, 0.0 );

		// カーソル
		pers.axis.bAxisX = true;
		pers.axis.bAxisY = true;
		pers.axis.bAxisZ = true;

		//点
		for ( Obj* p : m.tbl_pObj ) delete p;
		m.tbl_pObj.clear();
		m.tbl_pObj.emplace_back( new Ball(vect3(  0		, 1.0,  0.0 ), vect3(0,0,0)) );
		m.tbl_pObj.emplace_back( new Obj(vect3( -0.3	, 0.0,	0.0 )) );	// 平面原点
		m.tbl_pObj.emplace_back( new Obj(vect3( -0.3	, 0.2,  0.0 )) );	// 平面法線

		// 線
		for ( Edge* p : m.tbl_pEdge ) delete p;
		m.tbl_pEdge.clear();
		m.tbl_pEdge.emplace_back( new Edge(1,2, col7,1) );

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
		ball.pos = vect3(  0		, 1.0,  0.0 );
		ball.vel = vect3(  0		, 0.0,  0.0 );
		ball.flgOn = false;
	}

	// 入力
	{
		if ( keys.R.hi ) m.bInitParam = false;
		if ( keys.SPACE.hi )	bPause = !bPause ;
		if ( keys.ENTER.rep )	{bStep = true; bPause = true; }
	}


	// 設定
	vect3	plate_p = (*m.tbl_pObj[1]).pos;
	vect3	plate_n = ( (*m.tbl_pObj[2]).pos - plate_p ).normalize();
	Ball&	ball = *dynamic_cast<Ball*>(m.tbl_pObj[0]);

	vect3	p0 = ball.pos;
	vect3	v0 = ball.vel;
	vect3	pn;
	vect3	vn;


	{
		vect3	d = func_accelerationGetDistance_TVv( vg, delta, v0 );	// 移動距離(m)

		// 通常
		{
			pn = p0 + d;		// 仮移動
			vn = v0 + vg*delta;	// 仮速度
		}

		auto[flg,q0,t] = func_intersect_Plate_SegCurve( plate_p, plate_n, p0, -0.0001, delta, vg, v0 );

		// 衝突
		if ( flg )
		{
			// 衝突まで
			vect3 d1 = q0-p0;												// 衝突までの距離(m)
			float t1 = func_accelerationGetTime_DVv( vg.abs(), dot(d1,vg.normalize()), dot(v0,vg.normalize()) );	// 衝突までの時間(s)
			vect3 v1 = v0 + vg*t1;											// 衝突後の速度(m/s)

			float rate_r	= 0.6;		// 反射係数
			v1  = v1 - (1.0+rate_r)*dot( v1, plate_n ) * plate_n;

			// 衝突後
			float t2 = delta-t1;											// 衝突後の残り時間(s)
			vect3 d2 = func_accelerationGetDistance_TVv( vg, t2, v1 );		// 衝突後の移動距離(m)
			vect3 v2 = v1 + vg*t2;											// 衝突後の速度(m/s)

			// 接地
			if ( dot(d2,plate_n) < 0 )
			{
				d2 -= dot(d2,plate_n) * plate_n;
			}

			pn = p0 + d1 + d2;
			vn = v2 ;
			
		}

	}
	

	// 反映
	if  ( !bPause || bStep )
	{
		ball.pos = pn;
		ball.vel = vn;

	}


	// プレート表示
	pers.prim.DrawPlate( gra, pers, plate_p, plate_n, 32, col5/2.0 );

	// 軌跡表示
	for ( float t = 0 ; t < 1.0 ; t += 0.001 )
	{
		vect3 p = func_accelerationGetDistance_TVv( vg, t, v0 )+p0;	

		float d = dot( p-plate_p, plate_n );

		if ( d >0 ) 
		{
			pers.pen.pset3d( gra, pers, p, col5, 2 );
		}
		else
		{
			pers.pen.pset3d( gra, pers, p, col5/4, 1 );
		}

	}

}

/*
			if ( abs(dot(v0,plate_n)) < abs(dot((vg*delta),plate_n)) )
*/