//2020/04/21

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
#include "lab35.h"

using namespace std;

struct Lab35::Impl
{
	bool	bResetAll = true;
	bool	bResetParam = true;
	bool	bPause = false;
	bool	bStep = false;

//	vector<shared_ptr<Obj>>	tbl_pObj;
	struct Stone
	{
		vect2	prev_pos;
		vect2	pos;
		float	radius;
		vect2	vel;
		
		Stone( vect2 _pos, float _r ) :  pos(_pos), radius(_r) {}
		Stone( vect2 _pos, float _r, vect2 _mov ) :  pos(_pos), radius(_r), vel(_mov) {}
	};

	vector<Stone>	tblStones;
	
//	mat33	mat = mat33::mrotx(rad(90));
	mat33	mat = mat33::mrotx(rad(0));
};
Lab35::Lab35() : pImpl( new Lab35::Impl ){}

//------------------------------------------------------------------------------
void Lab35::Update( SysKeys& keys, SysMouse& mouse, SysSound& sound, SysGra& gra, Pers& pers, float delta, int& text_y, Cp& cp )
//------------------------------------------------------------------------------
{
	// 画面クリア
	gra.Clr(rgb(0.3,0.3,0.3));
	pers.grid.DrawGrid3d( gra, pers, vect3(0,0,0), pImpl->mat, 10, 10, 1, rgb(0.2,0.2,0.2) );
	gra.Print(1,(float)text_y++,"35 : Curing" ); 

	//初期化
	if ( pImpl->bResetAll )
	{
		pImpl->bResetAll = false;

		// カメラ
		pers.cam.pos = vect3( 0.0, 0.0, -15.0 );
		pers.cam.at = vect3( 0,  0.0, 0 );
		pers.cam.Update();

		// ストーン
		pImpl->tblStones.emplace_back( Impl::Stone(vect2(1,0),0.5) );
		pImpl->tblStones.emplace_back( Impl::Stone(vect2(0,1),0.5) );

	}
/*
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
*/
	if ( keys.R.hi )	pImpl->bResetAll = true;
	if ( keys.Q.hi )	pImpl->mat *= mat33::mrotx(rad(-30));
	if ( keys.A.hi )	pImpl->mat *= mat33::mrotx(rad(30));

	{
		vect3 P1 = pers.calcScreenToWorld3( vect3(mouse.pos,0) );
		vect3 I1 = pers.calcRayvect( P1 );

		auto[b,Q] = pers.grid.IntersectOn( P1, I1 );	// Grid空間座標を求める。

		pers.grid.Circle( gra, pers,Q, 0.5, 24, rgb(0,1,0) );
		pers.grid.Pset( gra, pers, Q, rgb(0,1,0), 12 );
		pers.grid.Print( gra, pers, Q, -26,-52, to_string(Q.x) + " " +to_string(Q.y)  ); 
//			pers.grid.Line( gra, pers, vect2(1,1), vect2(-1,-1), rgb(0,1,1), 2 );


		if ( mouse.L.lo ) pImpl->tblStones.emplace_back( Q, 0.5, vect2(0,0.01) );

	}


	// 移動
	for ( auto& o : pImpl->tblStones )
	{
		o.prev_pos = o.pos;
		o.pos += o.vel;
		o.vel *= 0.9999;
	}

	// 衝突
	for ( auto& b1 : pImpl->tblStones )
	{
		for ( auto& b2 : pImpl->tblStones )
		{
			if ( &b1 != &b2 )
			{
				vect2	v = (b2.pos-b1.pos);
				float	len = v.abs();
				float	bor = b1.radius + b2.radius;
				if ( len < bor )
				{
					b1.pos = b1.prev_pos;
					b2.pos = b2.prev_pos;

					v = v * (bor-len);
					vect2	N = v.normalize();
					vect2	vel1 = func_reflect( b1.vel, N, 1.0 );
					vect2	vel2 = func_reflect( b2.vel, N, 1.0 );

					vect2	v1 = dot(-vel2,N)*N;
					vect2	v2 = dot(-vel1,N)*N;

					b1.vel += v1 - v2;
					b2.vel += v2 - v1;

				}
			}
		}
	}

	// 減衰
	for ( auto& o : pImpl->tblStones )
	{
		o.vel *= 0.999;
	}

	// 表示
	for ( auto o : pImpl->tblStones )
	{
		pers.grid.Circle( gra, pers,o.pos, o.radius, 24, rgb(1,1,1) );
	}
//	pers.prim.DrawCircle( gra, pers, vect3( 0,0, 0 ), mat33::mrotz(0.0), 1.0, rgb(1,1,1) );


}
