//2019/10/17

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
#include "lab8.h"
struct Lab8::Impl
{
	Graphs::Plot plot_moment = Graphs::Plot( 100, 0.02, rgb(1,0,1) );
	vect3 		vel;
	bool		bPause = false;
	float		w = 0;

};
Lab8::Lab8() : pImpl( new Lab8::Impl ){}
//------------------------------------------------------------------------------
void Lab8::Update( SysKeys& keys, SysMouse& mouse, SysGra& gra, Pers& pers, float delta, int& text_y, Cp& cp )
//------------------------------------------------------------------------------
{
	const float	G	= -9.80665;				// 重力加速度

	//画面クリア
	gra.Clr(rgb(0.3,0.3,0.3));
	pers.grid.DrawGrid3d( gra, pers, vect3(0,0,0), midentity(), 10, 10, 1, rgb(0.2,0.2,0.2) );

	gra.Print(1,(float)text_y++,string("lab8_vector_six_lab8")); 

	const float	g = G *delta*delta;			// 重力加速度/frame
	const vect3	vg = vect3(0,0, -g);	// 重力加速度/frame

	bool bStep = false;

	// 初期化
	if ( !m.bInitParam )
	{
		if ( !m.bInitAll )
		{
			m.bInitAll = true;
			pers.cam.pos = vect3(  0.0, 5.0, -1.0 );
			pers.cam.at = vect3( 0,  0.0, 0 );
		}
	
		m.bInitParam = true;
		//点
		m.tbl_pObj.clear();
		m.tbl_pObj.emplace_back( new Obj(vect3( 0  , 0.1, 0 )) );
		m.tbl_pObj.emplace_back( new Obj(vect3( 1  , 0.1, 0 )) );
		// 線
		m.tbl_pEdge.clear();
		m.tbl_pEdge.emplace_back( new Edge(0,1) );

		//GUI登録
		cp.tbltbl_pObj.clear();
		cp.tbltbl_pEdge.clear();
		cp.tbltbl_pObj.emplace_back( m.tbl_pObj );
		cp.tbltbl_pEdge.emplace_back( m.tbl_pEdge );

		pers.axis.bAxisX = true;
		pers.axis.bAxisY = false;
		pers.axis.bAxisZ = true;

		pImpl->vel=vect3(0,0,0);	// 加速度
		pImpl->w = 0;

		pImpl->plot_moment.ResetPlot();
	}

	// 入力
	if ( keys.R.hi ) m.bInitParam = false;
	if ( keys.SPACE.hi )	pImpl->bPause = !pImpl->bPause ;
	if ( keys.ENTER.rep )	{bStep = true; pImpl->bPause = true; }

	vect3&	v0 = m.tbl_pObj[0]->pos;
	vect3&	v1 = m.tbl_pObj[1]->pos;

	vect3	bar = (v1-v0);					//	棒
	vect3	moment = cross(-bar, pImpl->vel+vg);	//	回転モーメント 仮
	vect3	F = cross(bar, moment );		//	力

	// 計算
	if ( !pImpl->bPause || bStep )
	{
		pImpl->plot_moment.WritePlot( moment.y*10 );

			
		// 衝突
		{
			pImpl->vel = mrotateByAxis( moment, (pImpl->vel+vg).abs() )*pImpl->vel+vg;

			v1 += pImpl->vel;

//			mat33 m = mrotateByAxis( moment, pImpl->w );

//			v1 = m * (v1-v0) + v0;
//			pImpl->w += F.abs();
//			pImpl->vel += F;
		}
 		
	}

	m.drawVect( gra, pers, text_y, v0, moment ,10	, rgb(1,0,1), "moment" );
	m.drawVect( gra, pers, text_y, v1, vg		,100	, rgb(1,0,0), "vg" );
	m.drawVect( gra, pers, text_y, v1, F		,1		, rgb(0,1,0), "F" );
	pImpl->plot_moment.DrawPlot( gra, pers );
	gra.Print(1,(float)text_y++,string("<<radius>>")+to_string(bar.abs())); 


}
