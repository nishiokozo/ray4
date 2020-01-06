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
	bool	bResetAll = true;
	bool	bResetParam = true;
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
		bool	bWall = false;
		//
		float	def_pos;
		float	def_vel;
		float	def_weight;
		float	def_radius;
		bool	def_bWall = false;
	};
	vector<Box>tblBox;
	
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
	gra.Print(1,(float)text_y++,"23 : 1d Box mass collition" ); 

	//初期化
	if ( pImpl->bResetAll )
	{
		pImpl->bResetAll = false;

		// カメラ
		pers.cam.pos	= vect3( 0.0, 1.0, -16.0 );
		pers.cam.at		= vect3( 0.0, 1.0, 0 );
		pers.cam.Update();

		// 箱
		int n=0;
		pImpl->tblBox.emplace_back();
		pImpl->tblBox[n].def_pos = -4;
		pImpl->tblBox[n].def_vel = 0.09;
		pImpl->tblBox[n].def_weight = 1.0;
		n++;
		pImpl->tblBox.emplace_back();
		pImpl->tblBox[n].def_pos =  0;
		pImpl->tblBox[n].def_vel = 0.0;
		pImpl->tblBox[n].def_weight = 1.0;
		n++;
	#if 0
		pImpl->tblBox.emplace_back();
		pImpl->tblBox[n].def_pos =  3;
		pImpl->tblBox[n].def_vel = 0.0;
		pImpl->tblBox[n].def_weight = 1.0;
		n++;
	#endif
		// 壁
		pImpl->tblBox.emplace_back();
		pImpl->tblBox[n].def_bWall = true;
		pImpl->tblBox[n].def_pos = -7.5;
		pImpl->tblBox[n].def_radius = 0.5;
		n++;
		pImpl->tblBox.emplace_back();
		pImpl->tblBox[n].def_bWall = true;
		pImpl->tblBox[n].def_pos =  7.5;
		pImpl->tblBox[n].def_radius = 0.5;
		n++;
	}

	// 初期化
	if ( pImpl->bResetParam )
	{
		pImpl->bResetParam = false;

		for ( Impl::Box& box : pImpl->tblBox )
		{
			box.pos = box.def_pos;
			box.vel = box.def_vel;
			box.weight = box.def_weight;
			box.radius = box.def_radius;
			box.bWall = box.def_bWall;
			if ( !box.bWall )
			{
				box.radius = cbrt(box.weight);
			}
		}
	}
	// 静止追突 1:0
	if ( keys._1.hi )
	{
		pImpl->tblBox[0].def_vel = 0.09;
		pImpl->tblBox[1].def_vel = 0.0;
		pImpl->bResetParam = true;
		pImpl->bPause = true;
	}
	// 背面追突 1:4
	if ( keys._2.hi )
	{
		pImpl->tblBox[0].def_vel = 0.09;
		pImpl->tblBox[1].def_vel = 0.03;
		pImpl->bResetParam = true;
		pImpl->bPause = true;
	}
	// 正面衝突 速度比 4:1
	if ( keys._3.hi )
	{
		pImpl->tblBox[0].def_vel = 0.09;
		pImpl->tblBox[1].def_vel = -0.03;
		pImpl->bResetParam = true;
		pImpl->bPause = true;
	}
	// 正面衝突 速度比 1:1
	if ( keys._4.hi )
	{
		pImpl->tblBox[0].def_vel = 0.09;
		pImpl->tblBox[1].def_vel = -0.09;
		pImpl->bResetParam = true;
		pImpl->bPause = true;
	}
	// 正面衝突 速度比 1:2
	if ( keys._5.hi )
	{
		pImpl->tblBox[0].def_vel = 0.09;
		pImpl->tblBox[1].def_vel = -0.18;
		pImpl->bResetParam = true;
		pImpl->bPause = true;
	}
	// 重量比 1:1
	if ( keys.Q.hi )
	{
		pImpl->tblBox[0].def_weight = 1.0;
		pImpl->tblBox[1].def_weight = 1.0;
		pImpl->bResetParam = true;
		pImpl->bPause = true;
	}
	// 重量比 1:2
	if ( keys.W.hi )
	{
		pImpl->tblBox[0].def_weight = 2.0;
		pImpl->tblBox[1].def_weight = 1.0;
		pImpl->bResetParam = true;
		pImpl->bPause = true;
	}
	// 重量比 2:1
	if ( keys.E.hi )
	{
		pImpl->tblBox[0].def_weight = 1.0;
		pImpl->tblBox[1].def_weight = 2.0;
		pImpl->bResetParam = true;
		pImpl->bPause = true;
	}
	// 重量比 1:1
	if ( keys.SHIFT.on && keys.Q.hi )
	{
		pImpl->tblBox[0].def_weight = 4.0;
		pImpl->tblBox[1].def_weight = 4.0;
		pImpl->bResetParam = true;
		pImpl->bPause = true;
	}
	// 重量比 1:2
	if ( keys.SHIFT.on && keys.W.hi )
	{
		pImpl->tblBox[0].def_weight = 1.0;
		pImpl->tblBox[1].def_weight = 4.0;
		pImpl->bResetParam = true;
		pImpl->bPause = true;
	}
	// 重量比 2:1
	if ( keys.SHIFT.on && keys.E.hi )
	{
		pImpl->tblBox[0].def_weight = 4.0;
		pImpl->tblBox[1].def_weight = 1.0;
		pImpl->bResetParam = true;
		pImpl->bPause = true;
	}

	// 入力
	if ( keys.R.hi )		pImpl->bResetParam = true;
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

					if ( !box1.bWall && !box2.bWall )
					{
			if ( &box1 == &pImpl->tblBox[0] )
			{
					#if 0
						// 衝突で互いの速度が入れ替わる
						float f = (v2*w2 - v1*w1);	// 衝撃力
						box1.new_vel = v1 + f/w1;	// 反作用
						box2.new_vel = v2 - f/w2;	// 作用
						// 2kg→1kgに衝突したとき全エネルギーが渡ってしまうのが変
					#endif
					#if 0
						// より軽い質量を基準にする
						float w = min(w1,w2);		// より軽い方
						float f = (v2*w - v1*w);	// 衝撃力
						box1.new_vel = v1 + f/w1;	// 反作用
						box2.new_vel = v2 - f/w2;	// 作用
						// 正面衝突でエネルギーが減衰してしまうのが変
					#endif
					#if 0
						// 静止したボールA(質量1)に対し、ボールB(質量2)をぶつけたら、
						// Aは速度1で飛び出し、Bは残った質量1で速度0.5で更に動く。
						// 静止したボールB(質量2）に対し、ボールA(質量1)をぶつけたら、
						// Bは速度0.5で飛び出し、Aは静止する。
						float f = (v1*w1 - v2*w2);	// 衝突力
						float f2 = f     /(w1/w2);
						float f1 = (f-f2)/(w1);		// とりあえずbox1→box2の順の衝突のみ検証
						box1.new_vel = f1;
						box2.new_vel = f2;
						// 追突でエネルギーが消失してしまう。
						
					#endif
					#if 0
						// 衝突力を均等に割り振る
						float f = (v2*w2 - v1*w1);	// 衝撃力
						float f1 = f*w1/(w1+w2);	// 力配分
						float f2 = f*w2/(w1+w2);	// 力配分
						box1.new_vel =  + f1/w1;		// 速度変換
						box2.new_vel =  - f2/w2;		// 速度変換
						// 衝突前の状態に関わらず一定に分配サれてしまう。
					#endif

					#if 1
						// 衝突力を均等に割り振る
						float f = (v2*w2 - v1*w1);	// 衝撃力
						float f1 = f*w1/(w1+w2);	// 力配分
						float f2 = f*w2/(w1+w2);	// 力配分
						box1.new_vel =  + f1/w1;		// 速度変換
						box2.new_vel =  - f2/w2;		// 速度変換
						// 衝突前の状態に関わらず一定に分配サれてしまう。
					#endif

			}
					}
					else
					if ( !box1.bWall )
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
			if ( box.bWall == false )
			{
				vect3	v1 = vect3( box.vel, 0,0);
				stringstream ss ;
				ss << box.weight << "kg";
				pers.grid.print( gra, pers, 	v0.xy(), 0,-32, ss.str() ); 
				ss.str("");
				ss << box.vel << "m/s";
				pers.prim.DrawVect( gra, pers, text_y, v0, v1 ,5	, rgb(1,0,1), ss.str(), false, false,false );
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
	
	// 運動エネルギー表示
	{
		float f = 0;
		for ( Impl::Box& box : pImpl->tblBox )
		{
			f += abs(box.vel*box.weight);
		}

		gra.Print(1,(float)text_y++,string("F=")+to_string(f)); 
		for ( Impl::Box& box : pImpl->tblBox )
		{
			if ( box.bWall == false )
			gra.Print(1,(float)text_y++,string(":=")+to_string(box.vel*box.weight)); 
		}
		
	}
	
	// メッセージ表示
	if ( pImpl->bPause ) gra.Print(1,(float)text_y++,string("<Pause>")); 
}
