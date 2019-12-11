//2019/11/20

// 

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
#include "lab13.h"

static const float	G	= -9.80665;				// 重力加速度
static const rgb col0 = rgb( 0, 0, 0 );
static const rgb col1 = rgb( 0, 0, 1 );
static const rgb col2 = rgb( 1, 0, 0 );
static const rgb col3 = rgb( 1, 0, 1 );
static const rgb col4 = rgb( 0, 1, 0 );
static const rgb col5 = rgb( 0, 1, 1 );
static const rgb col6 = rgb( 1, 1, 0 );
static const rgb col7 = rgb( 1, 1, 1 );


static	bool	bPause = false;


//------------------------------------------------------------------------------
void Lab13::Update( SysKeys& keys, SysMouse& mouse, SysGra& gra, Pers& pers, float delta, int& text_y, Cp& cp )
//------------------------------------------------------------------------------
{
	rgb col = rgb( 1, 1, 1 );

	//画面クリア
	gra.Clr(col/5);
	pers.grid.DrawGrid3d( gra, pers, vect3(0,0,0), midentity(), 10, 10, 1, col/3 );

	gra.Print(1,(float)text_y++," : intersect_plate_curve" ); 

	// 初期化：オール
	if ( !m.bInitAll )
	{
		m.bInitAll = true;

		//カメラ
		pers.cam.pos = vect3(	0.0,	4.3, -8.0 );
		pers.cam.at = vect3( 	0.0,	2.3, 0.0 );

		//点
		m.tbl_pObj.emplace_back( new Obj( vect3(  0,	1,	0	 ) ) );
		m.tbl_pObj.emplace_back( new Obj( vect3(  2, 4.0,  2.0 ) ) );
		m.tbl_pObj.emplace_back( new Obj( vect3(  0, G,  0 ) ) );
		//平面
		m.tbl_pObj.emplace_back( new Obj(vect3(  2.1	, 0.5,	2.0 )) );	// 平面原点
		m.tbl_pObj.emplace_back( new Obj(vect3(  2.0	, 0.7,  2.0 )) );	// 平面法線

		//GUI登録
		cp.tbltbl_pObj.clear();
		cp.tbltbl_pEdge.clear();
		cp.tbltbl_pObj.emplace_back( m.tbl_pObj );
	}

	// 設定
	int n = 0;
	vect3	p0 = m.tbl_pObj[n++]->pos ;
	vect3	v0 = m.tbl_pObj[n++]->pos -p0;
	vect3	vg = m.tbl_pObj[n++]->pos -p0;
	vect3	plate_p = (*m.tbl_pObj[n++]).pos;
	vect3	plate_n = ( (*m.tbl_pObj[n++]).pos - plate_p ).normalize();

	// 描画
	for ( float t = 0 ; t < 1.0 ; t += 0.001 )
	{
		vect3 p = func_accelerationGetDistance_TVv( vg, t, v0 )+p0;	

		float d = dot( p-plate_p, plate_n );

		if ( d >0 ) 
		{
			pers.pen.pset3d( gra, pers, p, col, 2 );
		}
		else
		{
			pers.pen.pset3d( gra, pers, p, col/2, 1 );
		}

	}

	//  平面と二次曲線の交差判定
	auto[flg,Q,t] = func_intersect_Plate_SegCurve( plate_p, plate_n, p0, 0, 1.0, vg, v0 );

	gra.Print(1,(float)text_y++, " t " + to_string(t)); 

	if ( flg )
	{
		pers.pen.pset3d( gra, pers, Q, col2, 7 );
	}
	else
	{
		pers.pen.pset3d( gra, pers, Q, col1, 7 );
	}
	
	
	
	m.drawVect( gra, pers, text_y, p0, v0	,1	, col, "v" );
	m.drawVect( gra, pers, text_y, p0, vg	,1	, col, "a" );
	pers.prim.DrawPlate( gra, pers, plate_p, plate_n, 5, col/2.0 );

}

