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



// 定義
struct Ball12:Obj
{
	vect3	vel;	//	velocity 速度(m/s)
	float	radius = 0.1;
	mat33	mat = midentity();
	bool	flgOn = false;	// 接地フラグ
	vect3	vaxis;
	float	fspin;

	vect3	pn;
	
	Ball12() : Obj(vect3(0,0,0)) {}

	Ball12( vect3 v, vect3 _vel ) : Obj(v)
	{
		pos = v;
		vel = _vel;
	}
	
	void Init(  vect3 _pos, vect3 _vel, float _radius, vect3 _vaxis, float _fspin )
	{
		pos		= _pos;
		vel		= _vel;
		radius	= _radius;
		vaxis	= _vaxis;
		fspin	= _fspin;
		
	}
	
};

//------------------------------------------------------------------------------
void Lab12::Update( SysKeys& keys, SysMouse& mouse, SysGra& gra, Pers& pers, float delta, int& text_y, Cp& cp )
//------------------------------------------------------------------------------
{
	m.bStep = false;

	//画面クリア
	gra.Clr(rgb(0.3,0.3,0.3));
	pers.grid.DrawGrid3d( gra, pers, vect3(0,0,0), midentity(), 16, 16, 1, rgb(0.2,0.2,0.2) );
	gra.Print(1,(float)text_y++,string("12 : Ball12 & Ball12")); 

	if ( !m.bInitAll )
	{
		m.bInitAll = true;

		// カメラ
		pers.cam.pos = vect3(	0.0,	20.0, -2.0 );
		pers.cam.at = vect3( 	0.0,	1.0, 0.0 );

		//点
		for ( Obj* p : m.tbl_pObj ) delete p;
		m.tbl_pObj.clear();
		m.tbl_pObj.emplace_back( new Ball12 );
		m.tbl_pObj.emplace_back( new Ball12 );

		//GUI登録
		cp.tbltbl_pObj.emplace_back( m.tbl_pObj );

	}

	Ball12&	b1 = *dynamic_cast<Ball12*>(m.tbl_pObj[0]);
	Ball12&	b2 = *dynamic_cast<Ball12*>(m.tbl_pObj[1]);

	// 初期化：パラメータ
	if ( !m.bInitParam )
	{
#define NUM 2
#if NUM==1
	// b2 斜め45°
		m.bInitParam = true;
		{
			b1.pos	= vect3( -2	, 1.0,  0.0 );
			b1.vel	= vect3(  0	, 0.0,  0.0 );
			b1.radius = 1.0;
			b1.mat	= midentity();
			b1.vaxis	= vect3(0,0,1);
			b1.fspin	= 0.0;
		}
		{
			b2.pos	= vect3(  3	, 1.0, -3.0 );
			b2.vel	= vect3(  0	, 0.0,  0.0 );
			b2.radius = 1.0;
			b2.mat	= midentity();
			b2.vaxis	= vect3(0,0,1);
			b2.fspin	= 0.0;
		}

		// 転がり
		if(0){
			float ene = 0.1;
			vect3 dir = vect3(1,0,0).normalize();
			b1.vel += ene * dir;
		}
		if(1){
			float ene = 0.1;
			vect3 dir = vect3(-1,0,1).normalize();
			b2.vel += ene * dir;
		}
#endif
#if NUM==2
	// b1 b2 斜め90°
		m.bInitParam = true;
		{
			b1.pos	= vect3( -3	, 1.0, -3.0 );
			b1.vel	= vect3(  0	, 0.0,  0.0 );
			b1.radius = 1.0;
			b1.mat	= midentity();
			b1.vaxis	= vect3(0,0,1);
			b1.fspin	= 0.0;
		}
		{
			b2.pos	= vect3(  3	, 1.0, -3.0 );
			b2.vel	= vect3(  0	, 0.0,  0.0 );
			b2.radius = 1.0;
			b2.mat	= midentity();
			b2.vaxis	= vect3(0,0,1);
			b2.fspin	= 0.0;
		}

		// 転がり
		{
			float ene = 0.1;
			vect3 dir = vect3(1,0,1).normalize();
			b1.vel += ene * dir;
		}
		{
			float ene = 0.1;
			vect3 dir = vect3(-1,0,1).normalize();
			b2.vel += ene * dir;
		}
#endif
#if NUM==3
	// b1 b2 正面衝突°
		m.bInitParam = true;
		{
			b1.pos	= vect3( -3	, 1.0,  0.0 );
			b1.vel	= vect3(  0	, 0.0,  0.0 );
			b1.radius = 1.0;
			b1.mat	= midentity();
			b1.vaxis	= vect3(0,0,1);
			b1.fspin	= 0.0;
		}
		{
			b2.pos	= vect3(  3	, 1.0,  0.0 );
			b2.vel	= vect3(  0	, 0.0,  0.0 );
			b2.radius = 1.0;
			b2.mat	= midentity();
			b2.vaxis	= vect3(0,0,1);
			b2.fspin	= 0.0;
		}

		// 転がり
		{
			float ene = 0.1;
			vect3 dir = vect3(1,0,0).normalize();
			b1.vel += ene * dir;
		}
		{
			float ene = 0.1;
			vect3 dir = vect3(-1,0,0).normalize();
			b2.vel += ene * dir;
		}
#endif

	}

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
		float ene = 0.1;
		vect3 dir = vect3(-1,0,0);
		b2.vel += ene * dir;
	}

	// 移動
	b1.pn = b1.pos + b1.vel;
	b2.pn = b2.pos + b2.vel;

	// 衝突
	{
		if ( (b1.pn-b2.pn).abs() <b1.radius+b2.radius )
		{
			b1.pn = b1.pos;
			b2.pn = b2.pos;

			vect3	N = (b1.pos-b2.pos).normalize();
			vect3	vel1 = func_reflect( b1.vel, N, 1.0 );
			vect3	vel2 = func_reflect( b2.vel, N, 1.0 );

			vect3 v1 = dot(-vel2,N)*N;
			vect3 v2 = dot(-vel1,N)*N;

			b1.vel += v1 - v2;
			b2.vel += v2 - v1;
		}
	}
	
	// 反映
	if  ( !m.bPause || m.bStep )
	{
		// 回転量を移動量計算で求める
		auto func = [&]( Ball12& ball )
		{
			vect3	plate_n(0,1,0);
			vect3	d3 = ball.pn - ball.pos;
			float	r = ball.radius;

			vect3	axis = cross(d3,plate_n);
			vect3	mov = d3 - dot(d3,plate_n)*plate_n;
			float	th = mov.abs()/r;

			ball.vaxis = axis;
			ball.fspin = th;
			ball.mat = mrotateByAxis( ball.vaxis, ball.fspin ) * ball.mat;
		}; 
		func(b1);
		func(b2);
		b1.pos = b1.pn;
		b2.pos = b2.pn;
	}
	
	m.drawVect( gra, pers, text_y, b2.pos, b2.vaxis.normalize() ,1	, col3, "axis" );

	
	// 表示
	pers.prim.DrawSphere( gra, pers, b1.radius, b1.pos, b1.mat );
	pers.prim.DrawSphere( gra, pers, b2.radius, b2.pos, b2.mat );

}
