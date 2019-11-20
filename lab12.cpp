//2019/11/17

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
void Lab::lab12_RidgePlateDot( Lab& lab, SysKeys& keys, SysMouse& mouse, SysGra& gra, Pers& pers, float delta, int& text_y )
//------------------------------------------------------------------------------
{
delta = 1/60.0;
	bool bStep=false;
	gra.Print(1,(float)text_y++,to_string(lab.idx)+" : " + string(__func__ )); 

	//----
	// 変数
	vect3	gv	= vect3(0,G,0);		// 重力加速度ベクトル


	// 初期化：オール
	if ( !lab.bInitAll )
	{
		lab.bInitAll = true;
		//カメラ
		pers.cam.pos = vect3(	0.0,	0.3, -3.0 );
		pers.cam.at = vect3( 	0.0,	0.3, 0.0 );

		//点
		for ( Obj* p : lab.tblObj ) delete p;
		lab.tblObj.clear();
		lab.tblObj.emplace_back( new Ball(vect3(  0		, 1.0,  0.0 ), vect3(0,0,0)) );
		lab.tblObj.emplace_back( new Obj(vect3( -0.3	, 0.5,	0.0 )) );	// 平面原点
		lab.tblObj.emplace_back( new Obj(vect3( -0.3	, 0.7,  0.0 )) );	// 平面法線

		// 線
		for ( Edge* p : lab.tblEdge ) delete p;
		lab.tblEdge.clear();
		lab.tblEdge.emplace_back( new Edge(1,2, col7,1) );

		// カーソル
		pers.axis.bAxisX = true;
		pers.axis.bAxisY = true;
		pers.axis.bAxisZ = true;
	}

	// 初期化：パラメータ
	if ( !lab.bInitParam )
	{
		lab.bInitParam = true;

		Ball&	ball = *dynamic_cast<Ball*>(lab.tblObj[0]);
		ball.pos = vect3(  0		, 1.0,  0.0 );
		ball.vel = vect3(  0		, 0.0,  0.0 );
		ball.flgOn = false;
	}

	// 入力
	{
		if ( keys.R.hi ) lab.bInitParam = false;
		if ( keys.SPACE.hi )	bPause = !bPause ;
		if ( keys.ENTER.rep )	{bStep = true; bPause = true; }
	}


	// 設定
	vect3	plate_p = (*lab.tblObj[1]).pos;
	vect3	plate_n = ( (*lab.tblObj[2]).pos - plate_p ).normalize();
	Ball&	ball = *dynamic_cast<Ball*>(lab.tblObj[0]);

	vect3	p0 = ball.pos;
	vect3	v0 = ball.vel;
	vect3	pn;
	vect3	vn;


	{
		vect3	d = func_accelerationGetDistance_TVv( gv, delta, v0 );	// 移動距離(m)

		// 通常
		{
			pn = p0 + d;		// 仮移動
			vn = v0 + gv*delta;	// 仮速度
		}

		auto[flg,q0,s] = func_distance_Plate_Segline( plate_p, plate_n, p0+plate_n/1024.0, p0 + d ); // 『+plate_n/1024.0』は線上を突き抜け内容にする処置。


		// 衝突
		if ( flg )
		{
			// 衝突まで
			vect3 d1 = q0-p0;												// 衝突までの距離(m)
			float t1 = func_accelerationGetTime_DVv( gv, d1.abs(), v0 );	// 衝突までの時間(s)
			vect3 v1 = v0 + gv*t1;											// 衝突後の速度(m/s)

			float rate_r	= 0.8;		// 反射係数
			v1  = v1 - (1.0+rate_r)*dot( v1, plate_n ) * plate_n;

			// 衝突後
			float t2 = delta-t1;											// 衝突後の残り時間(s)
			vect3 d2 = func_accelerationGetDistance_TVv( gv, t2, v1 );		// 衝突後の移動距離(m)
			vect3 v2 = v1 + gv*t2;											// 衝突後の速度(m/s)

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


	// 更新
	pers.prim.DrawPlate( gra, pers, plate_p, plate_n, col5/2.0 );
}

/*
			if ( abs(dot(v0,plate_n)) < abs(dot((gv*delta),plate_n)) )
*/