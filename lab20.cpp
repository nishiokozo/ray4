//2019/12/08

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
#include "lab20.h"

struct Lab20::Impl
{
};

//------------------------------------------------------------------------------
Lab20::Lab20() : pImpl( new Lab20::Impl )
//------------------------------------------------------------------------------
{

}

static struct
{
	float 	radius;
	vect2	pos;
	vect2	vel;
	float	spin;
	float	rot;	
} ball;

static struct
{
	vect2	p0;
	vect2	p1;
	vect2	tan;
	vect2	nor;
} wall;

//------------------------------------------------------------------------------
void Lab20::Update( SysKeys& keys, SysMouse& mouse, SysGra& gra, Pers& pers, float delta, int& text_y, Cp& cp )
//------------------------------------------------------------------------------
{
	int m_y = 0;

	auto funcShowBar = []( SysGra& gra, int y, float val, string str, rgb col )
	{
		vect2 v0 = vect2(0.0,0.75)+gra.Dot(0,42*y);
		gra.Line( v0, v0+ vect2( val, 0 ), col, 2 );
		gra.Print( v0+gra.Dot(0,-6), str + to_string(val), col );
	};

	// 画面クリア
	gra.Clr(rgb(0.3,0.3,0.3));
	pers.grid.DrawGrid3d( gra, pers, vect3(0,0,0), mrotx(rad(90)), 26, 26, 1, rgb(0.2,0.2,0.2) );
	gra.Print(1,(float)text_y++,"20 : 2D Motor Spin" ); 

	//初期化
	if ( !m.bInitAll )
	{
		m.bInitAll = true;

		// カメラ
		pers.cam.pos	= vect3( 0.0, 1.0, -10.0 );
		pers.cam.at		= vect3( 0.0, 1.0, 0 );
	}

	// 初期化
	if ( !m.bInitParam )
	{
		m.bInitParam = true;

		ball.radius	= 1.0;
		ball.pos	= vect2( 0, 3.1 );
		ball.vel	= vect2( 0, 0 );
		ball.spin	= 0;
		ball.rot	= 0;

		wall.p0 = vect2(-4,-0.2 );
		wall.p1 = vect2( 4, 0.2 );
		wall.tan = ( wall.p1 - wall.p0 ).normalize();
		wall.nor = vect2( -wall.tan.y, wall.tan.x );

	}

	// 入力
	if ( keys.R.hi )	m.bInitParam = false;
	if ( mouse.F.on )	ball.spin += 0.01;
	if ( mouse.B.on )	ball.vel += vect2( 0.003 , 0.0 );


	// 落下
	ball.vel += vect2( 0, -0.004 );
	ball.pos += ball.vel;

	if ( dot( ball.pos-wall.p0, wall.nor ) < ball.radius )
	{
		// 衝突前に戻す
		ball.pos -= ball.vel;

		// 床に並行な移動量
		ball.vel = 	func_reflect( ball.vel, wall.nor, 0.0 );
		ball.pos += ball.vel;

		// 回転量を求める
		ball.spin = dot( ball.vel, wall.tan );

	}
	

	// 描画：ボール
	ball.rot += ball.spin;
	pers.prim.DrawCircle( gra, pers, vect3( ball.pos, 0 ), mrotz(-ball.rot), 1.0, rgb(1,1,1) );

	// 描画：壁
	pers.pen.line3d( gra, pers, vect3(wall.p0,0), vect3(wall.p1,0) );


	funcShowBar( gra, m_y++, ball.spin, 		"spin  ", rgb(1,1,1) );
	
}
