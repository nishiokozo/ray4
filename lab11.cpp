//2019/11/25

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
#include "lab11.h"

using namespace std;

struct Lab11::Impl
{
	bool	bResetAll = true;
	bool	bResetParam = true;
	bool	bPause = false;
	bool	bStep = false;

	vector<shared_ptr<Obj>>	tbl_pObj;
	vector<shared_ptr<Edge>>	tbl_pEdge;
//	Graphs graphs;
};
Lab11::Lab11() : pImpl( new Lab11::Impl ){}

struct Ball11:Obj
{
	vect3	vel;	//	velocity 速度(m/s)
	float	radius = 1.0;
	mat33	mat = mat33::midentity();
//	mat33	mspin = mat33::midentity();
//	vect3	vcenter;
	vect3	vaxis;
	float	fspin;
	Ball11( vect3 v, vect3 _vel ) : Obj(v)
	{
		pos = v;
		vel = _vel;
	}
};


//------------------------------------------------------------------------------
void Lab11::Update( SysKeys& keys, SysMouse& mouse, SysSound& sound, SysGra& gra, Pers& pers, float delta, int& text_y, Cp& cp )
//------------------------------------------------------------------------------
{
	pImpl->bStep = false;

	//画面クリア
	gra.Clr(rgb(0.3,0.3,0.3));

	gra.Print(1,(float)text_y++,string("11 : Plate & Ball11")); 

	// 定数
	const float	G	= -9.80665;				// 重力加速度
	vect3	vg	= vect3(0,G,0);		// 重力加速度ベクトル

	// 初期化：オール
	if ( pImpl->bResetAll )
	{
		pImpl->bResetAll = false;

		// カメラ
		pers.cam.pos = vect3(	0.0,	1.0, -10.0 );
		pers.cam.at = vect3( 	0.0,	1.0, 0.0 );
		pers.cam.Update();

		//点
		pImpl->tbl_pObj.clear();
		pImpl->tbl_pObj.emplace_back( new Ball11(vect3(  0		, 0.62,  0.0 ), vect3(0,0,0)) );
		pImpl->tbl_pObj.emplace_back( new Obj(vect3( -0.0	, 0.0,	0.0 )) );	// 平面原点
		pImpl->tbl_pObj.emplace_back( new Obj(vect3( -0.05	, 0.5,  0.0 )) );	// 平面法線

		// 線
		pImpl->tbl_pEdge.clear();
		pImpl->tbl_pEdge.emplace_back( new Edge(1,2, rgb(1,1,1),1) );

		// 移動軸
		pers.axis.bAxisX = true;
		pers.axis.bAxisY = false;
		pers.axis.bAxisZ = false;

		//GUI登録
		cp.tbltbl_pObj.clear();
		cp.tbltbl_pEdge.clear();
		cp.tbltbl_pObj.emplace_back( pImpl->tbl_pObj );
		cp.tbltbl_pEdge.emplace_back( pImpl->tbl_pEdge );

//		pImpl->graphs.Clear();
//		pImpl->graphs.Create( 0.02, rgb(0,0,1) );
//		pImpl->graphs.Create( 0.02, rgb(0,1,1) );
//		pImpl->graphs.Create( 0.02, rgb(1,1,0) );
	}

	// 初期化：パラメータ
	if ( pImpl->bResetParam )
	{
		pImpl->bResetParam = false;
		Ball11&	ball = *dynamic_cast<Ball11*>(pImpl->tbl_pObj[0].get());
		ball.pos = vect3(  0		, 3.0,  0.0 );
		ball.vel = vect3(  0		, 0.0,  0.0 );
		ball.mat = mat33::midentity();
		ball.vaxis		= vect3(0,0,1);
		ball.fspin		= 0.0;

	}

	// 入力
	{
		if ( keys.R.hi )	pImpl->bResetParam = true;
		if ( keys.SPACE.hi )	pImpl->bPause = !pImpl->bPause ;
		if ( keys.ENTER.rep )	{pImpl->bStep = true; pImpl->bPause = true; }

		if ( keys.G.hi )	{if ( vg.z==G ) vg=vect3(0,0,0); else vg=vect3(0,G,0);}
		if ( keys.A.hi )	{pImpl->tbl_pObj[2]->pos.z+=-1.0;}
		if ( keys.O.hi )	{pers.bOrtho = !pers.bOrtho;}
	
	}
	
	//変数
	Ball11&	ball	= *dynamic_cast<Ball11*>(pImpl->tbl_pObj[0].get());
	vect3	plate_p	= pImpl->tbl_pObj[1]->pos;
	vect3	plate_n	= (pImpl->tbl_pObj[2]->pos-plate_p).normalize();
	vect3	p0		= ball.pos;
	vect3	v0		= ball.vel;
	vect3	pn;
	vect3	vn;

	//-----
	vect3	d = func_accelerationGetDistance_TVv( vg, delta, v0 );	// 移動距離(m)

	auto[flg,q0,t] = func_intersect_Plate_SegCurve_ball( plate_p, plate_n, p0, ball.radius, -0.0001, delta, dot(vg,plate_n)*plate_n, dot(v0,plate_n)*plate_n );


	// 衝突計算
	if ( flg )
	{	
		// 衝突まで
		float t1 = t;													// 衝突までの時間(s)
		vect3 d1 = func_accelerationGetDistance_TVv( vg, t1, v0 );		// 衝突までの移動距離(m)
		vect3 v1 = v0 + vg*t1;											// 衝突後の速度(m/s)

		float rate_r	= 0.6;		// 反射係数
		v1  = func_reflect( v1, plate_n, rate_r );

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

		vect3 d3 = d1 + d2;

		pn = p0 + d3;
		vn = v2 ;

		// 移動量から回転量を求める
		{
			vect3	axis = cross(d3,plate_n);
			vect3	mov = d3 - dot(d3,plate_n)*plate_n;
			float	th = mov.abs()/ball.radius;
			ball.vaxis = axis;
			ball.fspin = th;
		}

	}
	else
	// 通常加速計算
	{
		pn = p0 + d;		// 仮移動
		vn = v0 + vg*delta;	// 仮速度
	}

	// 反映
//	if  ( !pImpl->bPause || pImpl->bStep )
	{
		ball.pos = pn;
		ball.vel = vn;
		ball.mat *= mrotateByAxis( ball.vaxis, ball.fspin );

	}



//	pImpl->graphs.Draw( gra, pers );


	// 平面表示
	{
		pers.prim.DrawPlate( gra, pers, plate_p, plate_n, 28, rgb(0.5,1,1)*0.55 );
	}


	// ボール表示
	pers.prim.DrawSphere( gra, pers, ball.pos, ball.mat, ball.radius );

	// 接点表示
	if ( flg )	pers.pen.Pset3d( gra, pers, q0 , rgb(1,1,0), 9);


	// 1/60sec 待
	gra.Wait();

}
