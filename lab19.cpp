//2019/12/01

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
#include "lab19.h"

static const float	G	= -9.80665;				// 重力加速度
static const rgb col0 = rgb( 0, 0, 0 );
static const rgb col1 = rgb( 0, 0, 1 );
static const rgb col2 = rgb( 1, 0, 0 );
static const rgb col3 = rgb( 1, 0, 1 );
static const rgb col4 = rgb( 0, 1, 0 );
static const rgb col5 = rgb( 0, 1, 1 );
static const rgb col6 = rgb( 1, 1, 0 );
static const rgb col7 = rgb( 1, 1, 1 );


struct Lab19::Impl
{
};

//------------------------------------------------------------------------------
Lab19::Lab19() : pImpl( new Lab19::Impl )
//------------------------------------------------------------------------------
{
}

static struct
{
	vect3	pos	= vect3(0,0,0);
	vect3	normal	= vect3(0,1,0);
} plate;


static struct
{
	float	power = 0.0;
} motor;

struct Ball : Obj
{
	vect3	vel = vect3(0,0,0);
	float 	radius = 1;
	vect3	vaxis = vect3(0,0,1);
	mat33	mat = midentity();	
	float	wspin;	//	角速度	

	Ball() : Obj(vect3(0,0,0)){}
};

//------------------------------------------------------------------------------
void Lab19::Update( SysKeys& keys, SysMouse& mouse, SysGra& gra, Pers& pers, float delta, int& text_y, Cp& cp )
//------------------------------------------------------------------------------
{
	// クリア
	gra.Clr(rgb(0.3,0.3,0.3));
	pers.grid.DrawGrid3d( gra, pers, vect3(0,0,0), midentity(), 16, 16, 1, rgb(0.2,0.2,0.2) );
	gra.Print(1,(float)text_y++,"19 : Motor Spin" ); 

	// 初期化：ALL
	if ( !m.bInitAll )
	{
		m.bInitAll = true;

		// カメラ
		pers.cam.pos = vect3(	0.0,	1.0, -10.0 );
		pers.cam.at = vect3( 	0.0,	1.0,   0.0 );

		// 点
		m.tbl_pObj.emplace_back( new Ball );	// ボール

	}

	// 設定
	Ball&	ball	= *dynamic_cast<Ball*>(m.tbl_pObj[0]);

	// 初期化：パラメータ
	if ( !m.bInitParam )
	{
		m.bInitParam = true;

		ball.radius	= 1;
		ball.pos	= vect3(-4,1,0);
		ball.vaxis	= vect3(0,0,1);
		ball.mat	= midentity();	
		ball.vel	= vect3(0,0,0);
		ball.wspin	= 0;
		motor.power = 0;
	}

	// 入力
	if ( keys.R.hi )	m.bInitParam = false;
	if ( mouse.F.on )	motor.power += 0.01;
	if ( mouse.B.hi )	motor.power -= 0.04;


	// 伝達
	if ( ball.wspin < motor.power ) 
	{
		ball.wspin = ( ball.wspin + motor.power ) /2;
	}
	motor.power *= 0.95; // 減衰ロス

	ball.mat = mrotateByAxis( ball.vaxis, ball.wspin ) * ball.mat;

	// 角速度を速度に変換
	ball.vel = cross( plate.normal, ball.vaxis ) * ball.wspin;

	// 移動
	ball.pos += ball.vel;

	// ボール表示
	pers.prim.DrawSphere( gra, pers, ball.radius, ball.pos, ball.mat );

	// 速度表示	
	gra.Print(1,(float)text_y++, "motor" + to_string(motor.power) );
	gra.Print(1,(float)text_y++, "spin" + to_string(ball.wspin) );
	gra.Print(1,(float)text_y++, "vel " + to_string(ball.vel.x) );

	{
		int y = 0;
		{
			vect2 v0 = vect2(0.0,0.75)+gra.Dot(0,40*y++);
			gra.Line( v0, v0+ vect2( motor.power, 0 ), col7, 2 );
			gra.Print( v0+gra.Dot(0,26), "power "+to_string(motor.power), col7 );
		}
		{
			vect2 v0 = vect2(0.0,0.75)+gra.Dot(0,40*y++);
			gra.Line( v0, v0+ vect2( ball.wspin, 0 ), col7, 2 );
			gra.Print( v0+gra.Dot(0,26), "spin  "+to_string(ball.wspin), col7 );
		}
		{
			vect2 v0 = vect2(0.0,0.75)+gra.Dot(0,40*y++);
			gra.Line( v0, v0+ vect2( ball.vel.abs(), 0 ), col7, 2 );
			gra.Print( v0+gra.Dot(0,26), "vel  "+to_string(ball.vel.abs()), col7 );
		}
	}
}
