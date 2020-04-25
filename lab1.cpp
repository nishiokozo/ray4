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

#include "SysSound.h"
#include "SysKeys.h"
#include "SysMouse.h"
#include "SysGra.h"

#include "func.h"
#include "pers.h"

#include "lab.h"
#include "lab1.h"

using namespace std;

struct Lab1::Impl
{
	bool	bResetAll = true;
	bool	bResetParam = true;
	bool	bPause = false;
	bool	bStep = false;

	vector<shared_ptr<Obj>>	tbl_pObj;
};
Lab1::Lab1() : pImpl( new Lab1::Impl ){}
//------------------------------------------------------------------------------
void Lab1::Update( SysKeys& keys, SysMouse& mouse, SysSound& sound, SysGra& gra, Pers& pers, float delta, int& text_y, Cp& cp )
//------------------------------------------------------------------------------
{
	//画面クリア
	gra.Clr(rgb(0.3,0.3,0.3));
	pers.grid.DrawGrid3d( gra, pers, vect3(0,0,0), mat33::mrotx(rad(0)), 100, 100, 1, vect3(0.2,0.2,0.2) );

	gra.Print(1,(float)text_y++,string("<<lab1_graph>>")); 

//	rgb	col = vect3(0.2,0.2,0.2);

	// 初期化
	if ( pImpl->bResetAll )
	{
		pImpl->bResetAll = false;
	}

	if ( pImpl->bResetParam )
	{
		pImpl->bResetParam = false;
		pers.cam.pos = vect3( 2.0, 0.0, -5.0 );
		pers.cam.at = vect3( 2,  0.0, 0 );
		pers.cam.Update();
	}


#if 0	// 当加速グラフ
	//入力
	{
		// リセット
		if ( keys.R.hi )	pImpl->bResetParam = true;
	}


	float n = 10;
	{
		float dt = 0.1;
		float a = 1*dt*dt;
		float v = 0;
		float s = 0;

		for ( float t = 0 ; t < n ; t+=dt )
		{
			v += a;
			s += v;

			pers.pen.Pset2d( gra, pers, vect2(t,s), rgb(1,0,0), 3 );
		}
	}

	{
		float a = 1;
		float v = 0;
		float s = 0;

		float t0 = 0;
		float v0 = v;
		float s0 = s;

		for ( float t = 0 ; t < n ; t+=0.1 )
		{
			v = a * t;
			s = v * t * 0.5f;

			pers.pen.Line2d( gra, pers, vect2(t0,s0), vect2(t,s), rgb(0,0.5,1), 1 );
			pers.pen.Line2d( gra, pers, vect2(t0,v0), vect2(t,v), rgb(0,1,0), 1 );

			pers.pen.Pset2d( gra, pers, vect2(t,s), rgb(0,0.5,1), 3 );
			pers.pen.Pset2d( gra, pers, vect2(t,v), rgb(0,1,0), 3 );
			pers.pen.Print2d( gra, pers, vect2(t,s),0,0, to_string(s) );
		
			t0 = t;
			v0 = v;
			s0 = s;


		}
	}
#endif
#if 1

	// cosh グラフ
	for ( float t = 0 ; t < pi*2 ; t+=0.1 )
	{
		float y1 = cosh(t);
		float y2 = sinh(t);
		pers.pen.Pset2d( gra, pers, vect2(t,y1), rgb(1,1,0), 3 );
		pers.pen.Pset2d( gra, pers, vect2(t,y2), rgb(1,0,1), 3 );
//		pers.pen.Pset2d( gra, pers, vect2(t,y2/y1), rgb(1,1,1), 3 );

		pers.pen.Print3d( gra, pers, vect3(0,0,0), 0, -32  , "cosh()", rgb(1,1,0) ); 
		pers.pen.Print3d( gra, pers, vect3(0,0,0), 0, -32*2, "sin()", rgb(1,0,1) ); 
//		pers.pen.Print3d( gra, pers, vect3(0,0,0), 0, -32*3, "cos()/cosh()", rgb(1,1,1) ); 

	}
#endif	
}
