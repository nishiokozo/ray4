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
	pers.grid.DrawGrid3d( gra, pers, vect3(0,0,0), mrotx(rad(90)), 10, 10, 1, rgb(0.2,0.2,0.2) );
	gra.Print(1,(float)text_y++,"27 : 2D B-Spline"); 

	//初期化
	if ( pImpl->bResetAll )
	{
		pImpl->bResetAll = false;

		// カメラ
		pers.cam.pos = vect3( 1.0, 3.0, -9.0 );
		pers.cam.at = vect3( 1.0,  3.0, 0 );
		pers.fovy=rad(45);
	}

	if ( pImpl->bResetParam )
	{
		pImpl->bResetParam = false;
	}
	
	
	// N1a(t) 一次基底関数 山型波形 0.0~2.0 
	function<float(float)> N1a = []( float t )
	{
		if ( t < 0.0 || t > 2.0 ) return 0.0;
		return ( t < 1.0 )?t:2.0-t; 
	};
	// f1a(t) 右肩上がり
	function<float(float)> f1a = []( float t )
	{
		return t/2.0; 
	};

	// N1b(t) 一次基底関数 山型波形 1.0~3.0
	function<float(float)> N1b = []( float t )
	{
		if ( t < 1.0 || t > 3.0 ) return 0.0;
		return ( t < 2.0 )?t-1.0:3.0-t; 
	};
	// f1b(t) 右肩下がり
	function<float(float)> f1b = []( float t )
	{
		return (3.0-t)/2.0; 
	};

	// N2(t) 二次基底関数 
	function<float(float)> N2 = [&]( float t )
	{
		float a1 = N1a(t);
		float b1 = f1a(t);
		float y1 = a1 * b1;

		float a2 = N1b(t);
		float b2 = f1b(t);
		float y2 = a2 * b2;

		float y3 = y1 + y2;
		return y3; 
	};
	// f2a(t) 右肩上がり
	function<float(float)> f2a = []( float t )
	{
		return t/3.0; 
	};
	// f2b(t) 右肩下がり
	function<float(float)> f2b = []( float t )
	{
		return (4.0-t)/3.0; 
	};

	for ( float t = 0 ; t <= 3.0  ; t+=0.01 )
	{

		// 左2/3
		pers.pen.pset3d( gra, pers, vect3(t, 4+ N1a(t), 0), rgb(1,1,0), 1 );
		pers.pen.pset3d( gra, pers, vect3(t, 4+ f1a(t), 0), rgb(1,0,0), 1 );
		pers.pen.pset3d( gra, pers, vect3(t, 4+ N1a(t)*f1a(t), 0), rgb(1,1,1), 2 );

		// 右2/3
		pers.pen.pset3d( gra, pers, vect3(t, 2+ N1b(t), 0), rgb(1,1,0), 1 );
		pers.pen.pset3d( gra, pers, vect3(t, 2+ f1b(t), 0), rgb(1,0,0), 1 );
		pers.pen.pset3d( gra, pers, vect3(t, 2+ N1a(t)*f1b(t), 0 ), rgb(1,1,1), 2 );

		// 左右合成
		pers.pen.pset3d( gra, pers, vect3(t, N2(t) ,0), rgb(1,1,1), 2 );
	}
	
	
}
