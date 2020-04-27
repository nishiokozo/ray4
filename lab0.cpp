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
#include "lab0.h"

using namespace std;

struct Lab0::Impl
{
	bool	bResetAll = true;
	bool	bResetParam = true;
	bool	bPause = false;
	bool	bStep = false;

	vector<shared_ptr<Obj>>	tbl_pObj;
};
Lab0::Lab0() : pImpl( new Lab0::Impl ){}

//------------------------------------------------------------------------------
void Lab0::Update( SysKeys& keys, SysMouse& mouse, SysSound& sound, SysGra& gra, Pers& pers, float delta, int& text_y, Cp& cp )
//------------------------------------------------------------------------------
{
	// 画面クリア
	gra.Clr(rgb(0.3,0.3,0.3));
	pers.grid.DrawGrid3d( gra, pers, vect3(0,0,0), mat33::mrotx(rad(90)), 10, 10, 1, rgb(0.2,0.2,0.2) );
	gra.Print(1,(float)text_y++,"0 : only grid" ); 

	//初期化
	if ( pImpl->bResetAll )
	{
		pImpl->bResetAll = false;

		// カメラ
		pers.cam.pos = vect3( 0.0, 2.0, -5.0 );
		pers.cam.at = vect3( 0,  1.0, 0 );
		pers.cam.Update();
	}


	vect3 vx = vect3(1,0,0);
	vect3 vy = vect3(0,1,0);
	vect3 vz = vect3(0,0,1);
	{
		vect3 p0 = vect3(-1,1,0);
		pers.prim.DrawArrow( gra, pers, text_y, p0, vx ,1	, rgb(1,0,0), "+x" );
		pers.prim.DrawArrow( gra, pers, text_y, p0, vy ,1	, rgb(0,1,0), "+y" );
		pers.prim.DrawArrow( gra, pers, text_y, p0, vz ,1	, rgb(0,0,1), "+z" );
	}

	{
		vect3 p0 = vect3(1,1,0);
		vect3 xy = cross(vx,vy);
		vect3 yz = cross(vy,vz);
		vect3 zx = cross(vz,vx);
		pers.prim.DrawArrow( gra, pers, text_y, p0, yz ,1	, rgb(1,0,0), "cross y*z" );
		pers.prim.DrawArrow( gra, pers, text_y, p0, zx ,1	, rgb(0,1,0), "cross z*x" );
		pers.prim.DrawArrow( gra, pers, text_y, p0, xy ,1	, rgb(0,0,1), "cross x*y" );

	}

}
