//2019/11/22

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
#include "lab27.h"



struct Lab27::Impl
{
	bool	bResetAll = true;
	bool	bResetParam = true;
	bool	bPause = false;
	bool	bStep = false;

	vector<shared_ptr<Obj>>	tbl_pObj;

	vector<int>	idxPoint;
};

Lab27::Lab27() : pImpl( new Lab27::Impl ){}


//------------------------------------------------------------------------------
void Lab27::Update( SysKeys& keys, SysMouse& mouse, SysSound& sound, SysGra& gra, Pers& pers, float delta, int& text_y, Cp& cp )
//------------------------------------------------------------------------------
{
	// 画面クリア
	gra.Clr(rgb(0.3,0.3,0.3));
	pers.grid.DrawGrid3d( gra, pers, vect3(0,0,0), mat33::mrotx(rad(90)), 10, 10, 1, rgb(0.2,0.2,0.2) );
	gra.Print(1,(double)text_y++,"27 : B-Spline(uniform)"); 

	//初期化
	if ( pImpl->bResetAll )
	{
		pImpl->bResetAll = false;

	}

	if ( pImpl->bResetParam )
	{
		pImpl->bResetParam = false;

		// カメラ
		pers.cam.pos	= vect3( 4.0, 4.0, -14.0 );
		pers.cam.at		= vect3( 4.0, 4.0, 0 );
		pers.cam.Update();
		pers.fovy=rad(45);
	}
	if ( keys.R.hi )	pImpl->bResetParam = true;
	
	
	// fa(t) 右肩上がり
	function<double(double,double)> fa = []( double t, double tmax )
	{
		return t/(tmax-1.0); 
	};
	// fb(t) 右肩下がり
	function<double(double,double)> fb = []( double t, double tmax )
	{
		return (tmax-t)/(tmax-1.0); 
	};

	// N1(t) 一次基底関数 山型波形
	function<double(double)> N1 = []( double t )
	{
		if ( t < 0 || t > 2.0 ) return 0.0;
		return ( t < 1.0 )?t:2.0-t; 
	};
	// N2(t) 二次基底関数 
	function<double(double)> N2 = [&]( double t )
	{
		return N1(t) * fa(t,3.0) + N1(t-1) * fb(t,3.0);
	};
	// N3(t) 三次基底関数 
	function<double(double)> N3 = [&]( double t )
	{
		return N2(t) * fa(t,4) + N2(t-1) * fb(t,4);
	};

	float wide = 1.0;
	// 二次B-スプライン
	{
		vect3 v(-4,0,0);
		for ( double t = 0 ; t <= 3  ; t+=0.01 )
		{

			// 左2/3
			pers.pen.pset3d( gra, pers, v+vect3(t, 4+ N1(t), 0), rgb(1,1,0), 1 );
			pers.pen.pset3d( gra, pers, v+vect3(t, 4+ fa(t,3), 0), rgb(1,0,0), 1 );
			pers.pen.pset3d( gra, pers, v+vect3(t, 4+ N1(t)*fa(t,3), 0), rgb(1,1,1), wide );

			// 右2/3
			pers.pen.pset3d( gra, pers, v+vect3(t, 2+ N1(t-1), 0), rgb(1,1,0), 1 );
			pers.pen.pset3d( gra, pers, v+vect3(t, 2+ fb(t,3), 0), rgb(1,0,0), 1 );
			pers.pen.pset3d( gra, pers, v+vect3(t, 2+ N1(t-1)*fb(t,3), 0 ), rgb(1,1,1), wide );

			// 左右合成
			pers.pen.pset3d( gra, pers, v+vect3(t, N2(t) ,0), rgb(1,1,1), wide );
		}
			pers.pen.print3d( gra, pers, vect3(-4,-0.5,0), 0,32, "B-Spline(2) " ); 
	}


	// 三次B-スプライン
	{
		vect3 v(0,0,0);
		for ( double t = 0 ; t <= 4.0  ; t+=0.01 )
		{
			// 左3/4
			v = vect3(0,4,0);
			pers.pen.pset3d( gra, pers, v+vect3(t, N2(t)			, 0), rgb(1,1,0), 1 );
			pers.pen.pset3d( gra, pers, v+vect3(t, fa(t,4)			, 0), rgb(1,0,0), 1 );
			pers.pen.pset3d( gra, pers, v+vect3(t, N2(t)*fa(t,4)	, 0), rgb(1,1,1), wide );


			// 右3/4
			v = vect3(0,2,0);
			pers.pen.pset3d( gra, pers, v+vect3(t, N2(t-1)			, 0), rgb(1,1,0), 1 );
			pers.pen.pset3d( gra, pers, v+vect3(t, fb(t,4)			, 0), rgb(1,0,0), 1 );
			pers.pen.pset3d( gra, pers, v+vect3(t, N2(t-1)*fb(t,4)	, 0), rgb(1,1,1), wide );
			
			// 左右合成
			v = vect3(0,0,0);
			pers.pen.pset3d( gra, pers, v+vect3(t, N2(t)*fa(t,4)	, 0), rgb(1,1,0), 1 );
			pers.pen.pset3d( gra, pers, v+vect3(t, N2(t-1)*fb(t,4)	, 0), rgb(1,1,0), 1 );
			pers.pen.pset3d( gra, pers, v+vect3(t, N3(t) ,0)			, rgb(1,1,1), wide );
		}
			pers.pen.print3d( gra, pers, vect3( 0,-0.5,0), 0,32, "B-Spline(3)" ); 
	}


	// 二次B-スプライン 合成グラフ
	{
		float tmax = 4+1;
		vect3 v0 = vect3(5,3,0);
		for ( double t = 0 ; t <= tmax  ; t+=0.01 )
		{
			vect3 v = v0;
			pers.pen.pset3d( gra, pers, v+vect3(t, N2(t  ) ,0), rgb(1,1,0), 1 );
			pers.pen.pset3d( gra, pers, v+vect3(t, N2(t-1) ,0), rgb(0,1,1), 1 );
			pers.pen.pset3d( gra, pers, v+vect3(t, N2(t-2) ,0), rgb(1,0,1), 1 );

			pers.pen.pset3d( gra, pers, v+vect3(t, N2(t-0)+N2(t-1)+N2(t-2) ,0), rgb(1,1,1), 1 );
		}
		for ( int t = 0 ; t <= tmax ; t++ )
		{
			pers.pen.print3d( gra, pers, v0+vect3( t,0,0), 0,32, to_string(t) ); 
		}
			pers.pen.print3d( gra, pers, v0+vect3( 2,0,0), 0,64, "B-Spline(2)" ); 
	}
	// 三次B-スプライン 合成グラフ
	{
		vect3 v0 = vect3(5,0,0);
		float tmax = 4+3;
		for ( double t = 0 ; t <= tmax  ; t+=0.01 )
		{
			vect3 v = v0;
			pers.pen.pset3d( gra, pers, v+vect3(t, N3(t  ) ,0), rgb(1,1,0), 1 );
			pers.pen.pset3d( gra, pers, v+vect3(t, N3(t-1) ,0), rgb(0,1,1), 1 );
			pers.pen.pset3d( gra, pers, v+vect3(t, N3(t-2) ,0), rgb(1,0,1), 1 );
			pers.pen.pset3d( gra, pers, v+vect3(t, N3(t-3) ,0), rgb(0,0,1), 1 );

			pers.pen.pset3d( gra, pers, v+vect3(t, N3(t-0)+N3(t-1)+N3(t-2)+N3(t-3) ,0), rgb(1,1,1), 1 );
		}
		for ( int t = 0 ; t <= tmax ; t++ )
		{
			pers.pen.print3d( gra, pers, v0+vect3( t,0,0), 0,32, to_string(t) ); 
		}
			pers.pen.print3d( gra, pers, v0+vect3( 2,0,0), 0,64, "B-Spline(3)" ); 
	}

}
