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

//		pers.grid.SetCol( col/3 );

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
	for ( float t = 0 ; t < 1.0 ; t += 0.001 )
	{
		vect3 p = func_accelerationGetDistance_TVv( vg, t, v0 );	

		float d = dot( p-plate_p, plate_n );

if ( d >-0.01 && d < 0.01 ) 
{
//		pers.pen.pset3d( gra, pers, p, col2, 5 );
}
else
if ( d >0 ) 
{
		pers.pen.pset3d( gra, pers, p, col, 2 );
}
else
{
		pers.pen.pset3d( gra, pers, p, col/2, 1 );
}
//		pers.pen.pset3d( gra, pers, vect3(t,0,d), col5, 1 );

	}

	{

        ////////////////////////////////////////////////////////////////////////
        //                                                                    //
        //  平面と二次曲線の交差判定                                          //
        //                                                                    //
        //  N:平面法線ベクトル                                                //
        //  Q:平面原点ベクトル                                                //
        //  P:交差点ベクトル                                                  //
        //  A:加速度ベクトル                                                  //
        //  V:初速度ベクトル                                                  //
        //  t:時間                                                            //
        //                                                                    //
        //  N･(P - Q) = 0     ...①                                           //
        //                                                                    //
        //      w = 0.5t^2                                                    //
        //                                                                    //
        //  P = Aw + Vt       ...②                                           //
        //                                                                    //
        //  ①に②を代入                                                      //
        //                                                                    //
        //  N･(Aw + Vt - Q) = 0                                               //
        //                                                                    //
        //  N.x*((A.x*w + V.x*t)-Q.x) +                                       //
        //  N.y*((A.y*w + V.y*t)-Q.y) +                                       //
        //  N.z*((A.z*w + V.z*t)-Q.z) = 0                                     //
        //                                                                    //
        //  (N･A)0.5t^2 + (N･V)t - (N･Q) = 0                                  //
        //                                                                    //
        //      c=(N･A)0.5                                                    //
        //      d=(N･V)                                                       //
        //      e=-(N･Q)                                                      //
        //                                                                    //
        //  ct^2 + dt + e = 0                                                 //
        //                                                                    //
        //  c(t+(d/2c))^2 + e -c(d/(2c))^2 = 0                                //
        //                                                                    //
        //      f = (d/2c)                                                    //
        //                                                                    //
        //  c(t+f)^2 + e -cf^2 = 0                                            //
        //                                                                    //
        //  t = ±√( f^2 - e/c ) - f;                                        //
        //                                                                    //
        ////////////////////////////////////////////////////////////////////////


		vect3 N = plate_n;	// 平面法線ベクトル
		vect3 Q = plate_p;	// 平面位置ベクトル
		vect3 A = vg;		// 加速度ベクトル
		vect3 V = v0;		// 初速度ベクトル
		//---
		float c = dot(N,A)*0.5;
		float d = dot(N,V);
		float e = -dot(N,Q);
		float f = d/(2*c);		
		float t =sqrt( f*f - e/c ) - f;
		float t2 = -sqrt( f*f - e/c ) - f;

		// 交差点ベクトル
		vect3 P = func_accelerationGetDistance_TVv( vg, t, v0 );	
		vect3 P2 = func_accelerationGetDistance_TVv( vg, t2, v0 );	

		pers.pen.pset3d( gra, pers, P, col2, 7 );
		pers.pen.pset3d( gra, pers, P2, col3, 7 );

	}
	
	
	lab.drawVect( gra, pers, text_y, vect3(0,0,0), v0	,1	, col, "HHH" );
	pers.prim.DrawPlate( gra, pers, plate_p, plate_n, 5, col/2.0 );

}

