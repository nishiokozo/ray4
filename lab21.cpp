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
#include "lab21.h"

struct Lab21::Impl
{
};

//------------------------------------------------------------------------------
Lab21::Lab21() : pImpl( new Lab21::Impl )
//------------------------------------------------------------------------------
{

}

static struct Ball21
{
	struct Vt
	{
		vect2	pos;
		vect2	vel;
		Vt( vect2 p, vect2 v ) : pos(p), vel(v) {}
	};
	vector<vect2>	pos{vect2(0,0),vect2(0,0),vect2(0,0),vect2(0,0)};
	vector<vect2>	vel{vect2(0,0),vect2(0,0),vect2(0,0),vect2(0,0)};
	vector<Vt>		vt;
	vect2	pos3;
	vect2	pos1;
	vect2	pos2;
	vect2	pos0;
	vect2	vel3;
	vect2	vel1;
	vect2	vel2;
	vect2	vel0;
} ball;

static struct
{
	vect2	p0;
	vect2	p1;
	vect2	tan;
	vect2	nor;
} wall;

static struct
{
	float power;
} motor;

static 	float	g_masatu = 1.0;
//------------------------------------------------------------------------------
void Lab21::Update( SysKeys& keys, SysMouse& mouse, SysGra& gra, Pers& pers, float delta, int& text_y, Cp& cp )
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
	gra.Print(1,(float)text_y++,"21 : 2D Square Spin" ); 

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

		vect2 center(0,3);

		ball.vt.clear();
		ball.vt.emplace_back( vect2( 1, 0) + center, vect2(0,0) );
		ball.vt.emplace_back( vect2( 0, 1) + center, vect2(0,0) );
		ball.vt.emplace_back( vect2(-1, 0) + center, vect2(0,0) );
		ball.vt.emplace_back( vect2( 0,-1) + center, vect2(0,0) );

		wall.p0 = vect2(-4,-0.2 );
		wall.p1 = vect2( 4, 0.2 );
		wall.tan = ( wall.p1 - wall.p0 ).normalize();
		wall.nor = vect2( -wall.tan.y, wall.tan.x );

		motor.power = 0;
	}

	// 入力
	if ( keys.R.hi )	m.bInitParam = false;
	if ( mouse.F.on )	motor.power += 0.0002;
	if ( mouse.B.on )	for ( Ball21::Vt& v : ball.vt ) { v.vel += vect2( 0.003 , 0.0 ); }
	if ( keys._1.rep )	g_masatu -= 0.1;
	if ( keys._2.rep )	g_masatu += 0.1;
	g_masatu = min( max( g_masatu, 0 ), 1.0 );

	// モーター回転伝達

	// 落下
	for ( Ball21::Vt& vt : ball.vt ) { vt.vel += vect2( 0, -0.004 ); }
	for ( Ball21::Vt& vt : ball.vt ) { vt.pos += vt.vel; }

	// 衝突
	for ( Ball21::Vt& vt : ball.vt ) 
	{
		if ( dot( vt.pos - wall.p0, wall.nor ) < 0 )
		{
			vt.pos -= vt.vel;
			vt.vel = func_reflect( vt.vel, wall.nor, 0.3 );
			vt.pos += vt.vel;

			// 他の頂点に伝達
			for ( Ball21::Vt& vt2 : ball.vt ) 
			{
				if ( &vt != &vt2 )
				{
					vt2.pos -= vt2.vel;
					vt2.vel /= 2;
				
				}
			}


		}
	}

	

	// 描画：ボール
	pers.pen.line3d( gra, pers, vect3( ball.vt[0].pos, 0 ) , vect3( ball.vt[1].pos, 0 ) );
	pers.pen.line3d( gra, pers, vect3( ball.vt[1].pos, 0 ) , vect3( ball.vt[2].pos, 0 ) );
	pers.pen.line3d( gra, pers, vect3( ball.vt[2].pos, 0 ) , vect3( ball.vt[3].pos, 0 ) );
	pers.pen.line3d( gra, pers, vect3( ball.vt[3].pos, 0 ) , vect3( ball.vt[0].pos, 0 ) );

	

	// 描画：壁
	pers.pen.line3d( gra, pers, vect3(wall.p0,0), vect3(wall.p1,0) );

	// メーター表示
	funcShowBar( gra, m_y++, motor.power,	"power    ", rgb(1,1,1) );
	funcShowBar( gra, m_y++, (ball.vel0+ball.vel1+ball.vel2+ball.vel3).abs(),"vel      ", rgb(1,1,1) );
	funcShowBar( gra, m_y++, g_masatu, 		"g_masatu ", rgb(1,1,1) );
	
}
