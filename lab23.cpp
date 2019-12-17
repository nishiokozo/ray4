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
#include "lab23.h"

struct Lab23::Impl
{
	bool	bInitAll = false;
	bool	bInitParam = false;
	bool	bPause = false;
	bool	bStep = false;

	struct Box
	{
		float	pos;
		float	vel;
		float	weight;
		float	radius;
		float	new_pos;
		float	new_vel;
		bool	bHit;
		bool	bMove = true;
	} tblBox[4];
	
};
Lab23::Lab23() : pImpl( new Lab23::Impl ){}

//------------------------------------------------------------------------------
void Lab23::Update( SysKeys& keys, SysMouse& mouse, SysSound& sound, SysGra& gra, Pers& pers, float delta, int& text_y, Cp& cp )
//------------------------------------------------------------------------------
{
	pImpl->bStep = false;

	// 画面クリア
	gra.Clr(rgb(0.3,0.3,0.3));
	pers.grid.DrawGrid3d( gra, pers, vect3(0,0,0), mrotx(rad(90)), 26, 26, 1, rgb(0.2,0.2,0.2), false );
	gra.Print(1,(float)text_y++,"23 : 1d Boc mass collition" ); 

	//初期化
	if ( !pImpl->bInitAll )
	{
		pImpl->bInitAll = true;

		// カメラ
		pers.cam.pos	= vect3( 0.0, 1.0, -16.0 );
		pers.cam.at		= vect3( 0.0, 1.0, 0 );
	}

	// 初期化
	if ( !pImpl->bInitParam )
	{
		pImpl->bInitParam = true;

		pImpl->tblBox[0].pos = -2.5;
		pImpl->tblBox[0].vel = 0.0;

		pImpl->tblBox[1].pos =  2.5;
		pImpl->tblBox[1].vel = 0.0;

		pImpl->tblBox[0].weight = 2.0;
		pImpl->tblBox[1].weight = 1.0;
		pImpl->tblBox[0].radius = cbrt(pImpl->tblBox[0].weight);
		pImpl->tblBox[1].radius = cbrt(pImpl->tblBox[1].weight);

		// -- 壁
		pImpl->tblBox[2].pos = -7.5;
		pImpl->tblBox[3].pos =  7.5;
		pImpl->tblBox[2].bMove = false;
		pImpl->tblBox[3].bMove = false;
		pImpl->tblBox[2].radius = 0.5;
		pImpl->tblBox[3].radius = 0.5;

	}
	// 静止追突 1:0
	if ( keys._1.hi )
	{
		pImpl->tblBox[0].pos = -2.5-1;
		pImpl->tblBox[1].pos =  2.5;

		pImpl->tblBox[0].vel = 0.08;
		pImpl->tblBox[1].vel = 0.0;
	}
	// 背面追突 1:4
	if ( keys._2.hi )
	{
		pImpl->tblBox[0].pos = -2.5-1;
		pImpl->tblBox[1].pos =  2.5;

		pImpl->tblBox[0].vel = 0.08;
		pImpl->tblBox[1].vel = 0.02;
	}
	// 正面衝突 速度比 4:1
	if ( keys._3.hi )
	{
		pImpl->tblBox[0].pos = -2.5-1;
		pImpl->tblBox[1].pos =  2.5;

		pImpl->tblBox[0].vel = 0.08;
		pImpl->tblBox[1].vel = -0.02;
	}
	// 正面衝突 速度比 1:1
	if ( keys._4.hi )
	{
		pImpl->tblBox[0].pos = -2.5-1;
		pImpl->tblBox[1].pos =  2.5;

		pImpl->tblBox[0].vel = 0.08;
		pImpl->tblBox[1].vel = -0.08;
	}
	// 正面衝突 速度比 1:2
	if ( keys._5.hi )
	{
		pImpl->tblBox[0].pos = -2.5-1;
		pImpl->tblBox[1].pos =  2.5;

		pImpl->tblBox[0].vel = 0.08;
		pImpl->tblBox[1].vel = -0.16;
	}
	// 重量比 1:1
	if ( keys.Q.hi )
	{
		pImpl->tblBox[0].weight = 1.0;
		pImpl->tblBox[1].weight = 1.0;
		pImpl->tblBox[0].radius = cbrt(pImpl->tblBox[0].weight);
		pImpl->tblBox[1].radius = cbrt(pImpl->tblBox[1].weight);
	}
	// 重量比 1:2
	if ( keys.W.hi )
	{
		pImpl->tblBox[0].weight = 1.0;
		pImpl->tblBox[1].weight = 2.0;
		pImpl->tblBox[0].radius = cbrt(pImpl->tblBox[0].weight);
		pImpl->tblBox[1].radius = cbrt(pImpl->tblBox[1].weight);
	}
	// 重量比 2:1
	if ( keys.E.hi )
	{
		pImpl->tblBox[0].weight = 2.0;
		pImpl->tblBox[1].weight = 1.0;
		pImpl->tblBox[0].radius = cbrt(pImpl->tblBox[0].weight);
		pImpl->tblBox[1].radius = cbrt(pImpl->tblBox[1].weight);
	}
	// 重量比 1:1
	if ( keys.SHIFT.on && keys.Q.hi )
	{
		pImpl->tblBox[0].weight = 4.0;
		pImpl->tblBox[1].weight = 4.0;
		pImpl->tblBox[0].radius = cbrt(pImpl->tblBox[0].weight);
		pImpl->tblBox[1].radius = cbrt(pImpl->tblBox[1].weight);
	}
	// 重量比 1:2
	if ( keys.SHIFT.on && keys.W.hi )
	{
		pImpl->tblBox[0].weight = 1.0;
		pImpl->tblBox[1].weight = 4.0;
		pImpl->tblBox[0].radius = cbrt(pImpl->tblBox[0].weight);
		pImpl->tblBox[1].radius = cbrt(pImpl->tblBox[1].weight);
	}
	// 重量比 2:1
	if ( keys.SHIFT.on && keys.E.hi )
	{
		pImpl->tblBox[0].weight = 4.0;
		pImpl->tblBox[1].weight = 1.0;
		pImpl->tblBox[0].radius = cbrt(pImpl->tblBox[0].weight);
		pImpl->tblBox[1].radius = cbrt(pImpl->tblBox[1].weight);
	}


	// 入力
	if ( keys.R.hi )		pImpl->bInitParam = false;
	if ( keys.SPACE.hi )	{pImpl->bPause=!pImpl->bPause;}
	if ( keys.ENTER.rep )	{pImpl->bStep=true;pImpl->bPause=true;}


	// 衝突：運動準備
	for ( Impl::Box& box : pImpl->tblBox )
	{
		box.new_pos = box.pos;
		box.new_vel = box.vel;
		box.bHit = false;
	}

	// 移動：計算
	if ( !pImpl->bPause || pImpl->bStep ) for ( Impl::Box& box : pImpl->tblBox ) { box.new_pos += box.vel; }


	// 衝突：計算
	for ( Impl::Box& box1 : pImpl->tblBox )
	{
		for ( Impl::Box& box2 : pImpl->tblBox )
		{
//			if ( &box1 == &pImpl->tblBox[0] )
			if ( &box1 != &box2 )
			{
				if ( abs( box1.new_pos - box2.new_pos ) < box1.radius+box2.radius )
				{
					float w1 = box1.weight;
					float w2 = box2.weight;
					float v1 = box1.vel;
					float v2 = box2.vel;

					box1.bHit = true;
					box2.bHit = true;

					if ( box1.bMove && box2.bMove )
					{
						float w = min(w1,w2);		// より軽い方
						float f = (v1*w - v2*w);		// 衝突力の差
						box1.new_vel = v1 - f/w1;	// 反作用
						box2.new_vel = v2 + f/w2;	// 作用
					}
					else
					if ( box1.bMove )
					{
						box1.new_vel = -box1.vel;
					}
				}
			}
		}
	}

	// 衝突：運動反映
	if ( !pImpl->bPause || pImpl->bStep )
	for ( Impl::Box& box : pImpl->tblBox )
	{
		if ( box.bHit ) box.new_pos = box.pos;	// 衝突前の場所に戻す簡易処理

		box.pos = box.new_pos;
		box.vel = box.new_vel;
	}


	// 描画：箱
	{
		gra.SetZTest(false);
		for ( Impl::Box& box : pImpl->tblBox )
		{
			float 	r = box.radius;
			vect3	v0 = vect3( box.pos, r,0);
			float	wide = 1.0;
			pers.pen.line3d( gra, pers, v0+vect3(-r,-r,0), v0+vect3( r,-r,0),rgb(1,1,1), wide );
			pers.pen.line3d( gra, pers, v0+vect3( r,-r,0), v0+vect3( r, r,0),rgb(1,1,1), wide );
			pers.pen.line3d( gra, pers, v0+vect3( r, r,0), v0+vect3(-r, r,0),rgb(1,1,1), wide );
			pers.pen.line3d( gra, pers, v0+vect3(-r, r,0), v0+vect3(-r,-r,0),rgb(1,1,1), wide );

			// 箱情報表示
			if ( box.bMove )
			{
				vect3	v1 = vect3( box.vel, 0,0);
				stringstream ss ;
				ss << box.weight << "kg";
				pers.grid.print( gra, pers, 	v0.xy(), 0,-32, ss.str() ); 
				ss.str("");
				ss << box.vel << "m/s";
				m.drawVect( gra, pers, text_y, v0, v1 ,5	, rgb(1,0,1), ss.str(), false, false,false );
			}

		}
		gra.SetZTest(true);
	}

	//地面表示
	{
		gra.SetZTest(false);
		pers.pen.line3d( gra, pers, vect3(-8,0,0), vect3( 8,0,0), rgb(1,1,1),1);	
		gra.SetZTest(true);
	}
	
	// メッセージ表示
	if ( pImpl->bPause ) gra.Print(1,(float)text_y++,string("<Pause>")); 
}
