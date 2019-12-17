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
		float	prev_pos;
		float	new_pos;
		float	new_vel;
		bool	bMove = true;
	} tblBox[2];
	
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

		pImpl->tblBox[0].weight = 1.0;
		pImpl->tblBox[1].weight = 1.0;
		pImpl->tblBox[0].radius = cbrt(pImpl->tblBox[0].weight);
		pImpl->tblBox[1].radius = cbrt(pImpl->tblBox[1].weight);

	}

	// 正面衝突
	if ( keys._1.hi )
	{
		pImpl->tblBox[0].pos = -2.5;
		pImpl->tblBox[0].vel = 0.08;

		pImpl->tblBox[1].pos =  2.5;
		pImpl->tblBox[1].vel = -0.01;
	}
	// 静止追突
	if ( keys._2.hi )
	{
		pImpl->tblBox[0].pos = -2.5;
		pImpl->tblBox[0].vel = 0.08;

		pImpl->tblBox[1].pos =  2.5;
		pImpl->tblBox[1].vel = 0.0;
	}
	// 追突
	if ( keys._3.hi )
	{
		pImpl->tblBox[0].pos = -2.5;
		pImpl->tblBox[0].vel = 0.08;

		pImpl->tblBox[1].pos =  2.5;
		pImpl->tblBox[1].vel = 0.01;
	}
	// 重要 1:1
	if ( keys._4.hi )
	{
		pImpl->tblBox[0].weight = 1.0;
		pImpl->tblBox[1].weight = 1.0;
		pImpl->tblBox[0].radius = cbrt(pImpl->tblBox[0].weight);
		pImpl->tblBox[1].radius = cbrt(pImpl->tblBox[1].weight);
	}
	// 重要 1:2
	if ( keys._5.hi )
	{
		pImpl->tblBox[0].weight = 1.0;
		pImpl->tblBox[1].weight = 2.0;
		pImpl->tblBox[0].radius = cbrt(pImpl->tblBox[0].weight);
		pImpl->tblBox[1].radius = cbrt(pImpl->tblBox[1].weight);
	}
	// 重要 2:1
	if ( keys._6.hi )
	{
		pImpl->tblBox[0].weight = 2.0;
		pImpl->tblBox[1].weight = 1.0;
		pImpl->tblBox[0].radius = cbrt(pImpl->tblBox[0].weight);
		pImpl->tblBox[1].radius = cbrt(pImpl->tblBox[1].weight);
	}


	// 入力
	if ( keys.R.hi )		pImpl->bInitParam = false;
	if ( keys.SPACE.hi )	{pImpl->bPause=!pImpl->bPause;}
	if ( keys.ENTER.rep )	{pImpl->bStep=true;pImpl->bPause=true;}


	// 移動：計算
	if ( !pImpl->bPause || pImpl->bStep ) for ( Impl::Box& box : pImpl->tblBox ) { box.pos += box.vel; }

	// 衝突：運動準備
	for ( Impl::Box& box : pImpl->tblBox )
	{
		box.new_pos = box.pos;
		box.new_vel = box.vel;
	}

	// 衝突：計算
	for ( Impl::Box& pa : pImpl->tblBox )
	{
		for ( Impl::Box& pb : pImpl->tblBox )
		{
			if ( &pa == &pImpl->tblBox[0] )
			if ( &pa != &pb )
			{
				if ( abs( pa.pos - pb.pos ) < pa.radius+pb.radius )
				{
					float wa = pa.weight;
					float wb = pb.weight;
					float va = pa.vel;
					float vb = pb.vel;

					pa.new_pos = pa.prev_pos;	// 衝突前の場所に戻す簡易処理
					pb.new_pos = pb.prev_pos;	// 衝突前の場所に戻す簡易処理

					if ( pa.bMove && pb.bMove )
					{
					#if 0
						pa.new_vel = vb*wb/wa;
						pb.new_vel = va*wa/wb;
					#else
						float	f = wa/wb;
					
						pa.new_vel = -va*(1.0 -f) + vb*f;
						pb.new_vel =  vb*(1.0 -f) + va*f;
					#endif

					}
					else
					if ( pa.bMove )
					{
						pa.new_vel = -pa.vel;
					}
				}
			}
		}
	}

	// 衝突：運動反映
	if ( !pImpl->bPause || pImpl->bStep )
	for ( Impl::Box& box : pImpl->tblBox )
	{
		box.pos = box.new_pos;
		box.vel = box.new_vel;
	}

	// 保存
	for ( Impl::Box& box : pImpl->tblBox )
	{
		box.prev_pos = box.pos;
	}

	// 描画：箱
	{
//		gra.SetZTest(false);
		for ( Impl::Box& box : pImpl->tblBox )
		{
			float 	r = cbrt(box.weight);
			vect2	v0 = vect2( box.pos, r);
			float	wide = 1.0;
//			pers.pen.line3d( gra, pers, v0+vect3(-r,-r,0), v0+vect3( r,-r,0),rgb(1,1,1), wide );
//			pers.pen.line3d( gra, pers, v0+vect3( r,-r,0), v0+vect3( r, r,0),rgb(1,1,1), wide );
//			pers.pen.line3d( gra, pers, v0+vect3( r, r,0), v0+vect3(-r, r,0),rgb(1,1,1), wide );
//			pers.pen.line3d( gra, pers, v0+vect3(-r, r,0), v0+vect3(-r,-r,0),rgb(1,1,1), wide );
			pers.grid.line( gra, pers, v0+vect2(-r,-r), v0+vect2( r,-r),rgb(1,1,1), wide );
			pers.grid.line( gra, pers, v0+vect2( r,-r), v0+vect2( r, r),rgb(1,1,1), wide );
			pers.grid.line( gra, pers, v0+vect2( r, r), v0+vect2(-r, r),rgb(1,1,1), wide );
			pers.grid.line( gra, pers, v0+vect2(-r, r), v0+vect2(-r,-r),rgb(1,1,1), wide );

			// 箱情報表示
			{
				vect2	v1 = vect2( box.vel, 0);
				stringstream ss ;
				ss << box.weight << "kg";
				pers.grid.print( gra, pers, 	v0, 0,-32, ss.str() ); 
				ss.str("");
				ss << box.vel << "m/s";
				m.drawVect( gra, pers, text_y, vect3(v0,0)+vect3(-0.5,0,0), vect3(v1,0) ,10	, rgb(1,0,1), ss.str(), false, false,false );
			}

		}
//		gra.SetZTest(true);
	}

	//地面表示
//	pers.pen.line3d( gra, pers, vect3(-8,0,0), vect3( 8,0,0), rgb(1,1,1),1);	
	pers.grid.line( gra, pers, vect2(-8,0), vect2(8,0) );
	
	// メッセージ表示
	if ( pImpl->bPause ) gra.Print(1,(float)text_y++,string("<Pause>")); 
}
