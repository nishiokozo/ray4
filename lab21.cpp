//2019/12/08

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
#include "lab21.h"

using namespace std;

struct Lab21::Impl
{
	bool	bResetAll = true;
	bool	bResetParam = true;
	bool	bPause = false;
	bool	bStep = false;

	vector<shared_ptr<Obj>>	tbl_pObj;

	struct Vt
	{
		vect2	pos;
		vect2	vel;
		Vt( vect2 p, vect2 v ) : pos(p), vel(v) {}
	};

	struct Ball
	{
		vector<Vt>		vt;
	} ball;

	struct
	{
		vect2	p0;
		vect2	p1;
		vect2	tan;
		vect2	nor;
	} wall;

	struct
	{
		float power;
	} motor;

	float	masatu = 1.0;
};

Lab21::Lab21() : pImpl( new Lab21::Impl ){}

//------------------------------------------------------------------------------
void Lab21::Update( SysKeys& keys, SysMouse& mouse, SysSound& sound, SysGra& gra, Pers& pers, float delta, int& text_y, Cp& cp )
//------------------------------------------------------------------------------
{
	int m_y = 0;

	auto funcShowBar = []( SysGra& gra, int y, float val, string str, rgb col )
	{
		vect2 v0 = vect2(0.0,0.75)+gra.Dot(0,42.0*y);
		gra.Line2d( v0, v0+ vect2( val, 0 ), col, 2 );
		gra.Print( v0+gra.Dot(0,-6), str + to_string(val), col );
	};

	// 画面クリア
	gra.Clr(rgb(0.3,0.3,0.3));
	pers.grid.DrawGrid3d( gra, pers, vect3(0,0,0), mat33::mrotx(rad(90)), 26, 26, 1, rgb(0.2,0.2,0.2) );
	gra.Print(1,(float)text_y++,"21 : 2D Square Spin" ); 

	//初期化
	if ( pImpl->bResetAll )
	{
		pImpl->bResetAll = false;

		// カメラ
		pers.cam.pos	= vect3( 0.0, 1.0, -10.0 );
		pers.cam.at		= vect3( 0.0, 1.0, 0 );
		pers.cam.Update();
	}

	// 初期化
	if ( pImpl->bResetParam )
	{
		pImpl->bResetParam = false;

		vect2 center(0,3);

		pImpl->ball.vt.clear();
		pImpl->ball.vt.emplace_back( vect2( 1, 0) + center, vect2(0,0) );
		pImpl->ball.vt.emplace_back( vect2( 0, 1) + center, vect2(0,0) );
		pImpl->ball.vt.emplace_back( vect2(-1, 0) + center, vect2(0,0) );
		pImpl->ball.vt.emplace_back( vect2( 0,-1) + center, vect2(0,0) );

		pImpl->wall.p0 = vect2(-4,-0.2 );
		pImpl->wall.p1 = vect2( 4, 0.2 );
		pImpl->wall.tan = ( pImpl->wall.p1 - pImpl->wall.p0 ).normalize();
		pImpl->wall.nor = vect2( -pImpl->wall.tan.y, pImpl->wall.tan.x );

		pImpl->motor.power = 0;
	}

	// 入力
	if ( keys.R.hi )	pImpl->bResetParam = true;
	if ( mouse.F.on )	pImpl->motor.power += 0.0002;
	if ( mouse.B.on )	for ( Impl::Vt& v : pImpl->ball.vt ) { v.vel += vect2( 0.003 , 0.0 ); }
	if ( keys._1.rep )	pImpl->masatu -= 0.1;
	if ( keys._2.rep )	pImpl->masatu += 0.1;
	pImpl->masatu = min( max( pImpl->masatu, 0 ), 1.0 );

	// モーター回転伝達

	// 落下
	for ( Impl::Vt& vt : pImpl->ball.vt ) { vt.vel += vect2( 0, -0.004 ); }
	for ( Impl::Vt& vt : pImpl->ball.vt ) { vt.pos += vt.vel; }

	// 衝突
	for ( Impl::Vt& vt : pImpl->ball.vt ) 
	{
		if ( dot( vt.pos - pImpl->wall.p0, pImpl->wall.nor ) < 0 )
		{
			vt.pos -= vt.vel;
			vt.vel = func_reflect( vt.vel, pImpl->wall.nor, 0.3 );
			vt.pos += vt.vel;

			// 他の頂点に伝達
			for ( Impl::Vt& vt2 : pImpl->ball.vt ) 
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
	pers.pen.Line3d( gra, pers, vect3( pImpl->ball.vt[0].pos, 0 ) , vect3( pImpl->ball.vt[1].pos, 0 ) );
	pers.pen.Line3d( gra, pers, vect3( pImpl->ball.vt[1].pos, 0 ) , vect3( pImpl->ball.vt[2].pos, 0 ) );
	pers.pen.Line3d( gra, pers, vect3( pImpl->ball.vt[2].pos, 0 ) , vect3( pImpl->ball.vt[3].pos, 0 ) );
	pers.pen.Line3d( gra, pers, vect3( pImpl->ball.vt[3].pos, 0 ) , vect3( pImpl->ball.vt[0].pos, 0 ) );

	

	// 描画：壁
	pers.pen.Line3d( gra, pers, vect3(pImpl->wall.p0,0), vect3(pImpl->wall.p1,0) );

	// メーター表示
	funcShowBar( gra, m_y++, pImpl->motor.power,	"power    ", rgb(1,1,1) );
	funcShowBar( gra, m_y++, pImpl->masatu, 		"masatu ", rgb(1,1,1) );
	

	// 1/60sec 待
	gra.Wait();
}
