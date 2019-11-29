//2019/10/17

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
#include "lab4.h"
//------------------------------------------------------------------------------
void Lab4::Update( SysKeys& keys, SysMouse& mouse, SysGra& gra, Pers& pers, float delta, int& text_y, Cp& cp )
//------------------------------------------------------------------------------
{
	//画面クリア
	gra.Clr(rgb(0.3,0.3,0.3));
	pers.grid.DrawGrid3d( gra, pers, vect3(0,0,0), mrotx(deg2rad(90)), 100, 100, 1, vect3(0.2,0.2,0.2) );

//	pers.grid.DrawGrid3d( gra, pers, vect3(0,0,0), midentity(), 10, 10, 1, rgb(0.2,0.2,0.2) );
		gra.Print(1,(float)text_y++,string("<<lab4_furiko3d>>")); 
		// delta=J dω/dt
		// delta：トルク J：慣性モーメント ω：回転角速度[rad]
		//
		// t=rF
		// r:位置ベクトル t:力のモーメント F:力
		//
		// L=rp
		// r:位置ベクトル L:角運動量(運動量のモーメント) p:運動量

	const float G = 9.80665;			// 重力加速度
	const float g = G *delta*delta;		// 重力加速度/frame
	const vect3 vg = vect3(0,-g,0);	// 重力加速度ベクトル/frame

	static vect3	vel;			// 運動量
	static vect3	mov;			// 運動量
	static bool	bPause = false; 
	bool	bStep = false; 


	// 初期化
	if ( !m.bInitParam )
	{
		if ( !m.bInitAll )
		{
			m.bInitAll = true;
			pers.cam.pos = vect3(  0.0, 0.0, -7.0 );
			pers.cam.at = vect3( 0,  0.0, 0 );
		}

		m.bInitParam = true;
		for ( Obj* p : m.tbl_pObj ) delete p;
		m.tbl_pObj.clear();
		m.tbl_pObj.emplace_back( new Obj(vect3(0, 2.0, 0)) );
		m.tbl_pObj.emplace_back( new Obj(vect3(-1, 2.0, 0)) );

		//GUI登録
		cp.tbltbl_pObj.clear();
		cp.tbltbl_pEdge.clear();
		cp.tbltbl_pObj.emplace_back( m.tbl_pObj );
		
		vel = vect3(0,0,0);
	}

	vect3&	v0 = m.tbl_pObj[0]->pos;	//	barの根本
	vect3&	v1 = m.tbl_pObj[1]->pos;	//	barの先端

	// 入力
	{
		// ポーズ
		if ( keys.SPACE.hi )	bPause = !bPause;

		// ステップ再生
		if ( keys.ENTER.rep )	bStep = true;

		// リセット
		if ( keys.R.hi )	m.bInitParam = false ;

		// 縮む
		if ( mouse.F.hi )	v1 = (v1+v0)/2;

		// 伸びる
		if ( mouse.B.hi )	v1 = (v1-v0)*2+v0;
	}

	vect3	vr = (v1-v0);				//	位置ベクトル
	float	r = vr.abs();				//	位置スカラー
	vect3	moment = cross(-vr,vg);		//	モーメント
	vect3	F = cross(vr/r, moment/r );	//	力

	// 計算
	if ( !bPause || bStep )
	{
		vel += F;

	 	float	w = vel.abs()/r;					//	角速度

mov =vel;
		vel = mrotateByAxis( moment, w ) * vel;		//	回転

		v1 += vel;
	}

	// 描画
	{


		pers.pen.line3d( gra, pers, v0, v1, rgb(1,1,1), 2 );

		m.drawVect( gra, pers, text_y, v1, vg	,100, rgb(1,0,0), "g" );
		m.drawVect( gra, pers, text_y, v0, moment,100, rgb(1,0,1), "moment" );
		m.drawVect( gra, pers, text_y, v1, F		,100, rgb(0,1,0), "F" );
		m.drawVect( gra, pers, text_y, v1, vel	,2	, rgb(1,1,0), "vel" );
		m.drawVect( gra, pers, text_y, v1, mov	,2	, rgb(0,0,1), "mov" );
	}


#if 0
	{
		static float	rsp = 0;
		static vect3	vv = 0;

		vect3 v = (v1-v0);

		// 角速度に変換
		{
			float b = atan2(v.x,v.y);
			float t = -g * sin(b);			//	接線速度
			float r = t/2/pi/v.abs();		//	角加速度
			rsp +=r;						//	角速度
		}

		// 回転
		{
			float x = v.x *cos(rsp) - v.y*sin(rsp); 
			float y = v.x *sin(rsp) + v.y*cos(rsp); 
			vect3 a = vect3(x,y,0);
			v1 = v0 + a;

			pers.pen.line3d( gra, pers, v1, v1+(a-v)*10, rgb(1,0,0),2 );
			
		}

		pers.pen.line3d( gra, pers, v0, v1, rgb(1,1,1), 2 );

		// 加速ベクトルの実験
		{
			vect3 vg = vect3(0,-g,0);	//	重力加速度ベクトル
			vect3 n0 = cross( v, vg );
			vect3 vt = cross( n0, v );

//						pers.pen.line3d( gra, pers, v1, v1+vt*100, rgb(0,1,0) );
			vect3 v2 = v*1.05+v0;
//						pers.pen.line3d( gra, pers, v2, v2+vv, rgb(0,1,0) );

//						pers.pen.line3d( gra, pers, v2, v2+vg*100, rgb(1,0,0) );
			pers.pen.line3d( gra, pers, v2, v2+vt*100, rgb(0,1,0) );
			pers.pen.line3d( gra, pers, v2, v2+n0*100, rgb(0,0,1) );					}

	}
#endif
#if 0
	{
		vect3 vg = vect3(0,-g,0);	//	重力加速度ベクトル
		vect3 v = (v2-v0).normalize();
		vect3 n0 = cross( v, vg );
		vect3 vt = cross( n0, v );

		vv += vt;
		v2 += vt*10;

//					pers.pen.line3d( gra, pers,  0,  vv*10, rgb(0,1,1), 2 );
		pers.pen.line3d( gra, pers,  vect3(1,0,0),  vect3(1,0,0)+vt*100, rgb(0,1,0), 1 );

		pers.pen.line3d( gra, pers, v2, v2+vt*100, rgb(0,1,0) );
		pers.pen.line3d( gra, pers, v0, v2, rgb(0,1,1), 2 );

	}
#endif

	
}
