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
		pers.cam.pos = vect3( 1.0, 2.0, -6.0 );
		pers.cam.at = vect3( 1.0,  2.0, 0 );
		pers.fovy=rad(45);
	}

	if ( pImpl->bResetParam )
	{
		pImpl->bResetParam = false;
	}
	
	
	// 山型波形 0.0~2.0
	function<float(float)> N0_1 = []( float t )
	{
		if ( t < 0.0 || t > 2.0 ) return 0.0;
		return ( t < 1.0 )?t:2.0-t; 
	};
	function<float(float)> f1 = []( float t )
	{
		return 0.5f * t; 
	};

	// 山型波形 1.0~3.0
	function<float(float)> N1_1 = []( float t )
	{
		if ( t < 1.0 || t > 3.0 ) return 0.0;
		return ( t < 2.0 )?t-1.0:3.0-t; 
	};
	function<float(float)> f2 = []( float t )
	{
		return (3.0-t)/2.0; 
	};

	for ( float t = 0 ; t <= 3.0  ; t+=0.01 )
	{

		float a1 = N0_1(t);
		float b1 = f1(t);
		float y1 = a1 * b1;

		float a2 = N1_1(t);
		float b2 = f2(t);
		float y2 = a2 * b2;

		float y3 = y1 + y2;

		// 左2/3
		pers.pen.pset3d( gra, pers, vect3(t, 4+a1,0), rgb(1,1,0), 1 );
		pers.pen.pset3d( gra, pers, vect3(t, 4+b1,0), rgb(1,0,0), 1 );
		pers.pen.pset3d( gra, pers, vect3(t, 4+y1,0), rgb(1,1,1), 2 );

		// 右2/3
		pers.pen.pset3d( gra, pers, vect3(t, 2+a2,0), rgb(1,1,0), 1 );
		pers.pen.pset3d( gra, pers, vect3(t, 2+b2,0), rgb(1,0,0), 1 );
		pers.pen.pset3d( gra, pers, vect3(t, 2+y2,0), rgb(1,1,1), 2 );

		// 左右合成
		pers.pen.pset3d( gra, pers, vect3(t, y3 ,0), rgb(1,1,1), 2 );
	}
	
	
}
