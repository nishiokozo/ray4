//2019/12/10

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

#include "SysKeys.h"
#include "SysMouse.h"
#include "SysGra.h"

#include "func.h"
#include "pers.h"

#include "lab.h"
#include "lab22.h"

struct Lab22::Impl
{
	struct Vt
	{
		vect2	pos;
		vect2	vel;
		vect2	new_pos;
		vect2	new_vel;
		Vt( vect2 p, vect2 v ) : pos(p), vel(v) {}
	};
	struct Ball
	{
		vector<Vt>		vt;
	} ball;

	vector<shared_ptr<Obj>>	tbl_pObj;

//	vector<unique_ptr<Obj>>	tbl;

};

Lab22::Lab22() : pImpl( new Lab22::Impl ){}

//------------------------------------------------------------------------------
void Lab22::Update( SysKeys& keys, SysMouse& mouse, SysGra& gra, Pers& pers, float delta, int& text_y, Cp& cp )
//------------------------------------------------------------------------------
{
	// 画面クリア
	gra.Clr(rgb(0.3,0.3,0.3));
	pers.grid.DrawGrid3d( gra, pers, vect3(0,0,0), mrotx(rad(90)), 26, 26, 1, rgb(0.2,0.2,0.2) );
	gra.Print(1,(float)text_y++,"22 : 2D Bar Spin" ); 

	//初期化
	if ( !m.bInitAll )
	{
		m.bInitAll = true;

		// カメラ
		pers.cam.pos	= vect3( 0.0, 1.0, -10.0 );
		pers.cam.at		= vect3( 0.0, 1.0, 0 );

		// GUIコントロールポイント生成
		pImpl->tbl_pObj.emplace_back( new Obj );
		pImpl->tbl_pObj.emplace_back( new Obj );
		cp.tbltbl_pObj.emplace_back( pImpl->tbl_pObj );

//		pImpl->tbl.emplace_back( new Obj );
//		cp.tbltbl_pObj.emplace_back( pImpl->tbl );

	}

	// 初期化
	if ( !m.bInitParam )
	{
		m.bInitParam = true;

		vect2 center(0,2);
		pImpl->ball.vt.clear();
		float th = rad(45);
		pImpl->ball.vt.emplace_back( vect2( cos(th), -sin(th) ) + center, vect2(0,0) );
		pImpl->ball.vt.emplace_back( vect2(-cos(th),  sin(th) ) + center, vect2(0,0) );

	}

	// 割当
	Impl::Vt& p0	 = pImpl->ball.vt[0];
	Impl::Vt& p1	 = pImpl->ball.vt[1];

	// 入力
	if ( keys.R.hi )	m.bInitParam = false;
	if ( mouse.B.on )	for ( Impl::Vt& v : pImpl->ball.vt ) { v.vel += vect2( 0.003 , 0.0 ); }

	// 落下
//	for ( Impl::Vt& vt : pImpl->ball.vt ) { vt.vel += vect2( 0, -0.001 ); }
	for ( Impl::Vt& vt : pImpl->ball.vt ) { vt.pos += vt.vel; }

	// 計算準備
	for ( Impl::Vt& vt : pImpl->ball.vt ) 
	{
		vt.new_pos = vt.pos;
		vt.new_vel = vt.vel;
	}

	// 回転
	

	// 反映
	for ( Impl::Vt& vt : pImpl->ball.vt ) 
	{
		vt.pos = vt.new_pos;
		vt.vel = vt.new_vel;
	}
	

	// 描画：棒
	pers.pen.line3d( gra, pers, vect3( pImpl->ball.vt[0].pos, 0 ) , vect3( pImpl->ball.vt[1].pos, 0 ) );

	

	
}
