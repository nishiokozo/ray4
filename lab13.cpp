//2019/11/20

// 

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
void Lab::lab13( Lab& lab, SysKeys& keys, SysMouse& mouse, SysGra& gra, Pers& pers, float delta, int& text_y )
//------------------------------------------------------------------------------
{
//	rgb col = rgb( 0.6, 1, 0.75 );
	rgb col = rgb( 1, 1, 1 );
	gra.Clr(col/5);
	pers.grid.DrawGrid3d( gra, pers, vect3(0,0,0), midentity(), 10, 10, 1, col/3 );


	gra.Print(1,(float)text_y++,to_string(lab.idx)+" : " + string(__func__ )); 


	vect3 vg = vect3(0,G,0);
	vect3 v0 = vect3(1,1,0);

	// 初期化：オール
	if ( !lab.bInitAll )
	{
		lab.bInitAll = true;

		//カメラ
		pers.cam.pos = vect3(	0.0,	4.3, -8.0 );
		pers.cam.at = vect3( 	0.0,	2.3, 0.0 );

		//点
		lab.tblObj.emplace_back( new Obj( vect3(  2, 4.0,  2.0 ) ) );
		lab.tblObj.emplace_back( new Obj( vect3(  0, G,  0 ) ) );
		//平面
		lab.tblObj.emplace_back( new Obj(vect3(  2.1	, 0.5,	2.0 )) );	// 平面原点
		lab.tblObj.emplace_back( new Obj(vect3(  2.0	, 0.7,  2.0 )) );	// 平面法線
	}

	// 設定
	vect3	plate_p = (*lab.tblObj[2]).pos;
	vect3	plate_n = ( (*lab.tblObj[3]).pos - plate_p ).normalize();
	v0 = lab.tblObj[0]->pos ;
	vg = lab.tblObj[1]->pos ;

	// 描画
	for ( float t = 0 ; t < 1.0 ; t += 0.001 )
	{
		vect3 p = func_accelerationGetDistance_TVv( vg, t, v0 );	

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
	auto[flg,Q,s] = func_intersect_Plate_Curve( plate_n, plate_p, vg, v0 );

	pers.pen.pset3d( gra, pers, Q, col2, 7 );
	
	
	lab.drawVect( gra, pers, text_y, vect3(0,0,0), v0	,1	, col, "v" );
	lab.drawVect( gra, pers, text_y, vect3(0,0,0), vg	,1	, col, "a" );
	pers.prim.DrawPlate( gra, pers, plate_p, plate_n, 5, col/2.0 );

}

