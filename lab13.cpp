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
static void drawVect( SysGra& gra, Pers& pers, int& text_y, vect3 v0, vect3 v, float sc, rgb col, string str )
//------------------------------------------------------------------------------
{
	gra.SetZTest(false);

	vect3 v1 = v0+v*sc;

	// 影
	{
		vect3	a = v0;	a.y=0;
		vect3	b = v1;	b.y=0;
		rgb		c = (col+rgb(0.75,0.75,0.75))/4;
		pers.pen.line3d( gra, pers, a, b, c, 1 );
		pers.pen.pset3d( gra, pers,    b, c, 5 );
	}

	// 線
	pers.pen.line3d( gra, pers, v0, v1, col, 1 );
	pers.pen.pset3d( gra, pers,     v1, col, 5 );
	pers.pen.print3d( gra, pers, 	v1, 0,0, str ); 

	gra.Print(1,(float)text_y++, str+": "+to_string(v.x)+" , "+to_string(v.y)+" , "+to_string(v.z));

	gra.SetZTest(true);
};

//------------------------------------------------------------------------------
void Lab::lab13( Lab& lab, SysKeys& keys, SysMouse& mouse, SysGra& gra, Pers& pers, float delta, int& text_y )
//------------------------------------------------------------------------------
{
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
		//平面
		lab.tblObj.emplace_back( new Obj(vect3(  2.1	, 0.5,	2.0 )) );	// 平面原点
		lab.tblObj.emplace_back( new Obj(vect3(  2.0	, 0.7,  2.0 )) );	// 平面法線
	}

	// 設定
	vect3	plate_p = (*lab.tblObj[1]).pos;
	vect3	plate_n = ( (*lab.tblObj[2]).pos - plate_p ).normalize();
	v0 = lab.tblObj[0]->pos ;

	// 描画
	for ( float t = 0 ; t < 1.0 ; t += 0.01 )
	{
		vect3 d = func_accelerationGetDistance_TVv( vg, t, v0 );	

		pers.pen.pset3d( gra, pers, d, col7, 2 );
	}
	
	
	drawVect( gra, pers, text_y, vect3(0,0,0), v0	,1	, col4, "" );
	pers.prim.DrawPlate( gra, pers, plate_p, plate_n, col5/2.0 );



}

