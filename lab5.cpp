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
#include "lab5.h"

struct Lab5::Impl
{
	float	rsp = 0;

};

Lab5::Lab5() : pImpl( new Lab5::Impl ){}

//------------------------------------------------------------------------------
void Lab5::Update( SysKeys& keys, SysMouse& mouse, SysGra& gra, Pers& pers, float delta, int& text_y, Cp& cp )
//------------------------------------------------------------------------------
{
	const float	G	= -9.80665;				// 重力加速度
	
	//画面クリア
	gra.Clr(rgb(0.3,0.3,0.3));
	pers.grid.DrawGrid3d( gra, pers, vect3(0,0,0), midentity(), 10, 10, 1, rgb(0.2,0.2,0.2) );

	gra.Print(1,(float)text_y++,string("<<lab5_furiko2d>>")); 

	const float grate = 9.80665 *delta*delta;		// 重力加速度/frame


	if ( !m.bInitParam )
	{
		if ( !m.bInitAll )
		{
			m.bInitAll = true;
			pers.cam.pos = vect3(  0.0, 2.0, -5.0 );
			pers.cam.at = vect3( 0,  2.0, 0 );
		}
	
		m.bInitParam = true;
		m.tbl_pObj.clear();
		m.tbl_pObj.emplace_back( new Obj(vect3(0, 2.0, 0)) );
		m.tbl_pObj.emplace_back( new Obj(vect3(1, 2.0, 0)) );
		// 線
		m.tbl_pEdge.clear();
		m.tbl_pEdge.emplace_back( new Edge(0,1) );

		//GUI登録
		cp.tbltbl_pObj.clear();
		cp.tbltbl_pEdge.clear();
		cp.tbltbl_pObj.emplace_back( m.tbl_pObj );
		cp.tbltbl_pEdge.emplace_back( m.tbl_pEdge );

		pImpl->rsp=0;
	}
	if ( keys.R.hi )	m.bInitParam = false;

	vect3&	v0 = m.tbl_pObj[0]->pos;	//	barの根本
	vect3&	v1 = m.tbl_pObj[1]->pos;	//	barの先端


//	pers.pen.line3d( gra, pers, v0, v1, rgb(1,1,1), 2 );

	// 縮む
	if ( mouse.F.hi )	v1 = (v1+v0)/2;

	// 伸びる
	if ( mouse.B.hi )	v1 = (v1-v0)*2+v0;

	{
		vect3 v = v1-v0;
		float b = atan2(v.x,v.y);

		// 角速度に変換
		float tsp = -grate * sin(b);	//	接線速度
		float r = tsp/2/pi/v.abs();		//	角加速度
		pImpl->rsp +=r;						//	角速度

		// 回転
		float x = v.x *cos(pImpl->rsp) - v.y*sin(pImpl->rsp); 
		float y = v.x *sin(pImpl->rsp) + v.y*cos(pImpl->rsp); 
		v1 = v0+vect3(x,y,0);

	}
}
