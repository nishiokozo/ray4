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

struct Ball20 : Obj
{
	vect3	vel;
	float 	radius;
	float	spin;
	mat33	mat = midentity();

	Ball20() : Obj(vect3(0,0,0)){}
};

//------------------------------------------------------------------------------
void Lab20::Update( SysKeys& keys, SysMouse& mouse, SysGra& gra, Pers& pers, float delta, int& text_y, Cp& cp )
//------------------------------------------------------------------------------
{
	// 画面クリア
	gra.Clr(rgb(0.3,0.3,0.3));
	pers.grid.DrawGrid3d( gra, pers, vect3(0,0,0), mrotx(rad(90)), 10, 10, 1, rgb(0.2,0.2,0.2) );
	gra.Print(1,(float)text_y++,"20 : 2D Motor Spin" ); 

	//初期化
	if ( !m.bInitAll )
	{
		m.bInitAll = true;

		// カメラ
		pers.cam.pos	= vect3( 0.0, 1.0, -10.0 );
		pers.cam.at		= vect3( 0.0, 1.0, 0 );

		// 点
		m.tbl_pObj.emplace_back( new Ball20 );	//	球
		m.tbl_pObj.emplace_back( new Obj(vect3( 2,  0.2, 0)) );
		m.tbl_pObj.emplace_back( new Obj(vect3(-2, -0.2, 0)) );

		// 線
		m.tbl_pEdge.emplace_back( new Edge(1,2, rgb(1,1,1),1) );

		//GUI登録
		cp.tbltbl_pObj.clear();
		cp.tbltbl_pEdge.clear();
		cp.tbltbl_pObj.emplace_back( m.tbl_pObj );
		cp.tbltbl_pEdge.emplace_back( m.tbl_pEdge );

	}

	// 設定
	Ball20&	ball	= *dynamic_cast<Ball20*>(m.tbl_pObj[0]);
	vect3	p0		= m.tbl_pObj[1]->pos;
	vect3	p1		= m.tbl_pObj[2]->pos;
	vect3	plate_pos = p0;
	vect3	plate_nor = cross( p1-p0, vect3(0,0,1) ).normalize();



	// 初期化
	if ( !m.bInitParam )
	{
		m.bInitParam = true;

		ball.pos	= vect3(0,3,0);
		ball.vel	= vect3(0,0,0);
		ball.radius	= 1;
		ball.spin	= 0;
		ball.mat	= midentity();
	}

	// 入力
	if ( keys.R.hi )	m.bInitParam = false;
	if ( mouse.F.on )	ball.spin += 0.01;
	if ( mouse.B.on )	ball.vel += vect3(0.01,0,0);


	// 回転
	ball.mat = ball.mat *  mrotz( ball.spin );

	// 落下
	ball.vel += vect3(0,-9.8*delta*delta,0);

	// 移動
	ball.pos += ball.vel;

	if ( dot( ball.pos-plate_pos, plate_nor ) < ball.radius )
	{
		ball.pos -= ball.vel;

		ball.vel = func_reflect( ball.vel, plate_nor, 0.2 );
	}

	// 描画
	pers.prim.DrawCircle( gra, pers, ball.pos, ball.mat, 1.0, rgb(1,1,1) );
}
