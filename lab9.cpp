
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
#include "lab9.h"

static const float	G	= -9.80665;				// 重力加速度
static const rgb col0 = rgb( 0, 0, 0 );
static const rgb col1 = rgb( 0, 0, 1 );
static const rgb col2 = rgb( 1, 0, 0 );
static const rgb col3 = rgb( 1, 0, 1 );
static const rgb col4 = rgb( 0, 1, 0 );
static const rgb col5 = rgb( 0, 1, 1 );
static const rgb col6 = rgb( 1, 1, 0 );
static const rgb col7 = rgb( 1, 1, 1 );

//------------------------------------------------------------------------------
void Lab9::Update( SysKeys& keys, SysMouse& mouse, SysGra& gra, Pers& pers, float delta, int& text_y, Cp& cp )
//------------------------------------------------------------------------------
{
	//画面クリア
	gra.Clr(rgb(0.3,0.3,0.3));
	pers.grid.DrawGrid3d( gra, pers, vect3(0,0,0), midentity(), 10, 10, 1, rgb(0.2,0.2,0.2) );

	gra.Print(1,(float)text_y++,string("lab9_2dRidge")); 

	static	Graphs graphs;

	static vect3	vg	= vect3(0.0,0,G);		// 重力加速度ベクトル
	static bool		bPause = false;
	static float time = 0;
	bool bStep = false;
	struct Car:Obj
	{
		vect3	vel;	//	velocity 速度(m/s)
		Car( vect3 v, vect3 _vel ) : Obj(v)
		{
			pos = v;
			vel = _vel;
		}
	};

	// 初期化
	if ( !m.bInitParam )
	{
		if ( !m.bInitAll )
		{
			m.bInitAll = true;
			pers.cam.pos = vect3(	0.0,	4.2, 0.1 );
			pers.cam.at = vect3( 	0.0,	0.0, 0.2 );
		}
	
		m.bInitParam = true;
		//点
		for ( Obj* p : m.tbl_pObj ) delete p;
		m.tbl_pObj.clear();
		m.tbl_pObj.emplace_back( new Car(vect3(  0		, 0.0,  1.0 ), vect3(0,0,0)) );
		m.tbl_pObj.emplace_back( new Obj(vect3( -0.5	, 0.0,	0.0 )) );
		m.tbl_pObj.emplace_back( new Obj(vect3(  0.5	, 0.0, 0.0 )) );

		// 線
		for ( Edge* p : m.tbl_pEdge ) delete p;
		m.tbl_pEdge.clear();
		m.tbl_pEdge.emplace_back( new Edge(1,2) );

		//GUI登録
		cp.tbltbl_pObj.clear();
		cp.tbltbl_pEdge.clear();
		cp.tbltbl_pObj.emplace_back( m.tbl_pObj );
		cp.tbltbl_pEdge.emplace_back( m.tbl_pEdge );

		pers.axis.bAxisX = true;
		pers.axis.bAxisY = false;
		pers.axis.bAxisZ = true;

		graphs.Clear();
		graphs.Create( 0.02, rgb(0,0,1) );
		graphs.Create( 0.02, rgb(0,1,1) );
		graphs.Create( 0.02, rgb(1,1,0) );

		time = 0;

		cout << endl;
	}

	// 入力
	{
		if ( keys.R.hi ) m.bInitParam = false;
		if ( keys.SPACE.hi )	bPause = !bPause ;
		if ( keys.ENTER.rep )	{bStep = true; bPause = true; }

		if ( keys.G.hi )	{if ( vg.z==G ) vg=vect3(0,0,0); else vg=vect3(0,0,G);}
		if ( keys.A.hi )	{m.tbl_pObj[2]->pos.z+=-1.0;}
	
	}
	
	Car&	car = *dynamic_cast<Car*>(m.tbl_pObj[0]);
	vect3	st = m.tbl_pObj[1]->pos;
	vect3	en = m.tbl_pObj[2]->pos;

	//-----

	time += delta;



	//初期値
	vect3 p0 = car.pos;
	vect3 v0 = car.vel;


	// 結果
	vect3 pn;
	vect3 vn;

	{
			vect3 d = func_accelerationGetDistance_TVv( vg, delta, v0 );	// 移動距離 m
		// 計算
		{

			pn = p0 + d;	// 仮想移動
			vn = v0 + vg*delta;	// 仮想速度

			gra.SetZTest(false);
			pers.pen.line3d_scissor( gra, pers, p0, pn, col4, 1 );
			gra.SetZTest(true);
		}

		// 衝突
		{
			vect3 n = cross( (en-st).normalize(), vect3(0,1,0) );
			pers.pen.line3d( gra, pers, (en+st)/2, (en+st)/2+n, col2, 1 );


			auto[flg,dis,q0,q1,s0,s1] = func_intersect_SegLine_SegLine( p0-d/1024.0, pn, st, en );

			static vect3 p0x,nx,rx,bx;

			if ( flg )
			{
			

				// 衝突点での速度を求める
				vect3 d1 = q0-p0;												// 衝突までの距離(m)
				float t1 = func_accelerationGetTime_DVv( vg.abs(), d1.abs(), v0.abs() );	// 衝突までの時間(s)
				vect3 v1 = v0 + vg*t1;											// 衝突後の速度(m/s)

				// 速度の反射
				if ( v1.abs() > 0 )
				{
					vect3 n = cross( (en-st).normalize(), vect3(0,1,0) );
					vect3 b = (v1).normalize();
					vect3 r = reflect( b, n );
					v1 = r * v1.abs();
					nx=n;rx=r;bx=b;
				}

				{
					float t2 = delta-t1;											// 衝突後の残り時間(s)
					vect3 d2 = func_accelerationGetDistance_TVv( vg, t2, v1 );	// 衝突後の移動距離(m)
					vect3 v2 = v1 + vg*t2;										// 衝突後の速度(m/s)
				
					// 設置
					if ( dot(n,v2) < 0 )
					{
						d2 = vect3(0,0,0);
						v2 = vect3(0,0,0);
					}
					
					// 減衰
					v2 *= 0.2;

					pn = q0 + d2;
					vn = v2 ;

					pers.pen.pset3d( gra, pers, p0+d1, col4, 5 );	//	衝突点
					pers.pen.pset3d( gra, pers, pn, col4, 5 );		//	バウンド先
					p0x=p0+d1;
				}
			}

					pers.pen.line3d( gra, pers, p0x, p0x+nx , col6, 1 );
					pers.pen.line3d( gra, pers, p0x, p0x+rx , col4, 1 );
					pers.pen.line3d( gra, pers, p0x, p0x-bx , col5, 1 );

		}

		if  ( !bPause || bStep )
		{
			p0 = pn;
			v0 = vn;
		}
	}

	car.pos = p0;
	car.vel = v0;

}
