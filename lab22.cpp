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
//	Pers::Grid::Plot plot_moment = Pers::Grid::Plot( 100, 0.02, rgb(1,0,1) );

	struct Vt : Obj
	{
		vect3	vel = vect3(0,0,0);
		vect3	new_pos = vect3(0,0,0);
		vect3	new_vel = vect3(0,0,0);
		vect3	prev_pos = vect3(0,0,0);
		Vt() : Obj(vect3(0,0,0)){}
		Vt( vect3 p ) : Obj(p){}
		Vt( vect3 p, vect3 v ) : Obj(p), vel(v) {}
	};
	struct Ball
	{
		vector<shared_ptr<Vt>>		vt;
	} ball;

	vector<shared_ptr<Obj>>	tbl_pObj;

};

Lab22::Lab22() : pImpl( new Lab22::Impl ){}

//------------------------------------------------------------------------------
void Lab22::Update( SysKeys& keys, SysMouse& mouse, SysGra& gra, Pers& pers, float delta, int& text_y, Cp& cp )
//------------------------------------------------------------------------------
{
	auto funcShowBar = []( SysGra& gra, int y, float val, string str, rgb col )
	{
		vect2 v0 = vect2(0.0,0.75)+gra.Dot(0,42*y);
		gra.Line( v0, v0+ vect2( val, 0 ), col, 2 );
		gra.Print( v0+gra.Dot(0,-6), str + to_string(val), col );
	};

	// 画面クリア
	gra.Clr(rgb(0.3,0.3,0.3));
	gra.Print(1,(float)text_y++,"22 : 2D twin box" ); 

	// グリッド表示
	pers.grid.DrawGrid3d( gra, pers, vect3(0,0,0), mrotx(rad(90)), 26, 26, 1, rgb(0.2,0.2,0.2) );

	//初期化
	if ( !m.bInitAll )
	{
		m.bInitAll = true;

		// カメラ
		pers.cam.pos	= vect3( 0.0, 1.0, -10.0 );
		pers.cam.at		= vect3( 0.0, 1.0, 0 );

		pImpl->ball.vt.clear();
		pImpl->ball.vt.emplace_back( new Impl::Vt() );
		pImpl->ball.vt.emplace_back( new Impl::Vt() );

		// GUIコントロールポイントへの登録
		pImpl->tbl_pObj.clear();
		pImpl->tbl_pObj.emplace_back( pImpl->ball.vt[0] );
		pImpl->tbl_pObj.emplace_back( pImpl->ball.vt[1] );
		cp.tbltbl_pObj.clear();
		cp.tbltbl_pObj.emplace_back( pImpl->tbl_pObj );

		// プロット
//		pImpl->plot_moment.ResetPlot();
		pers.grid.plot.ResetPlot();
	}

	// 初期化
	if ( !m.bInitParam )
	{
		m.bInitParam = true;

		pImpl->ball.vt[0]->pos = vect3( -2, 1, 0 );
		pImpl->ball.vt[0]->vel = vect3(0,0,0);

		pImpl->ball.vt[1]->pos = vect3(  2, 1, 0 );
		pImpl->ball.vt[1]->vel = vect3(0,0,0);
	}

	// 入力
	if ( keys.R.hi )	m.bInitParam = false;
	if ( mouse.F.hi )	pImpl->ball.vt[0]->vel += vect3( 0.04 , 0 , 0 );
	if ( mouse.B.on )	pImpl->ball.vt[0]->vel += vect3( 0.01 , 0 , 0 );
	
	// 計算
	for ( shared_ptr<Impl::Vt> vt : pImpl->ball.vt ) { vt->pos += vt->vel; }

	Impl::Vt& p0 = *pImpl->ball.vt[0];
	Impl::Vt& p1 = *pImpl->ball.vt[1];

	// 衝突
	if ( ( p0.pos - p1.pos ).abs() < 2.0 )
	{
		vect3 vel = p0.vel;
		p0.pos -= vel;
		p0.vel = vel*0;
		p1.vel = vel;
	}
	
//	float suberi = 0.99;
//		p0.vel *= suberi;
//		p1.vel *= suberi;

	
	// プロット
//	pImpl->plot_moment.WritePlot( p0.vel.x );
	pers.grid.plot.WritePlot( p0.vel.x);
	


	// 描画：箱
	{
		vect3 v0 = pImpl->ball.vt[0]->pos;
		float 	w = 1.0;
		pers.pen.line3d( gra, pers, v0+vect3(-w,-w,0), v0+vect3( w,-w,0) );
		pers.pen.line3d( gra, pers, v0+vect3( w,-w,0), v0+vect3( w, w,0) );
		pers.pen.line3d( gra, pers, v0+vect3( w, w,0), v0+vect3(-w, w,0) );
		pers.pen.line3d( gra, pers, v0+vect3(-w, w,0), v0+vect3(-w,-w,0) );
	}
	{
		vect3 v0 = pImpl->ball.vt[1]->pos;
		float 	w = 1.0;
		pers.pen.line3d( gra, pers, v0+vect3(-w,-w,0), v0+vect3( w,-w,0) );
		pers.pen.line3d( gra, pers, v0+vect3( w,-w,0), v0+vect3( w, w,0) );
		pers.pen.line3d( gra, pers, v0+vect3( w, w,0), v0+vect3(-w, w,0) );
		pers.pen.line3d( gra, pers, v0+vect3(-w, w,0), v0+vect3(-w,-w,0) );
	}

	// 保存
	for ( shared_ptr<Impl::Vt> vt : pImpl->ball.vt ) 
	{
		vt->prev_pos = vt->pos;
	}
	
	// メーター表示
	int m_y = 0;
	funcShowBar( gra, m_y++, p0.vel.x,	"vel    ", rgb(1,1,1) );

	// プロット表示
//	pImpl->plot_moment.DrawPlot( gra, pers );



}
