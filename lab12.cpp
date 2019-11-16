//2019/11/17

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
#include "gui.h"

#include "lab.h"

//------------------------------------------------------------------------------
void lab12_RidgePlateDot( Lab& lab, SysKeys& keys, SysMouse& mouse, SysGra& gra, Pers& pers, Gui& gui, int& text_y )
//------------------------------------------------------------------------------
{
	gra.Print(1,(float)text_y++,to_string(lab.idx)+" : " + string(__func__ )); 

	static bool		bPause = false;
	bool bStep = false;

	struct Ball:Obj
	{
		vect3	vel;	//	velocity 速度(m/s)
		float	radius = 0.1;
		mat33	mat = midentity();
		Ball( vect3 v, vect3 _vel ) : Obj(v)
		{
			pos = v;
			vel = _vel;
		}
	};

	// 初期化
	if ( !lab.bInitCam )
	{
		lab.bInitCam = true;
		//カメラ
		pers.cam.pos = vect3(	0.0,	1.0, -3.0 );
		pers.cam.at = vect3( 	0.0,	1.0, 0.0 );

		//点
		for ( Obj* p : lab.tblObj ) delete p;
		lab.tblObj.clear();
		lab.tblObj.emplace_back( new Ball(vect3(  0		, 1.0,  0.0 ), vect3(0,0,0)) );
		lab.tblObj.emplace_back( new Obj(vect3( -0.4	, 0.5,	0.0 )) );	// 平面原点
		lab.tblObj.emplace_back( new Obj(vect3( -0.3	, 0.7,  0.0 )) );	// 平面法線

		// 線
		for ( Edge* p : lab.tblEdge ) delete p;
		lab.tblEdge.clear();
		lab.tblEdge.emplace_back( new Edge(1,2, col7,1) );

		// カーソル
		gui.axis.bAxisX = true;
		gui.axis.bAxisY = true;
		gui.axis.bAxisZ = false;
	}

	// リセット
	if ( !lab.bInit )
	{
		Ball&	ball = *dynamic_cast<Ball*>(lab.tblObj[0]);
		ball.pos = vect3(  0		, 1.0,  0.0 );
		ball.vel = vect3(  0		, 0.0,  0.0 );

		lab.bInit = true;


	}


}