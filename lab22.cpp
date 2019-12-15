//2019/12/10

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
#include "lab22.h"

struct Lab22::Impl
{

	struct Vt : Obj
	{
		vect3	vel = vect3(0,0,0);
		vect3	new_pos = vect3(0,0,0);
		vect3	new_vel = vect3(0,0,0);
		bool	bMove;
		vect3	prev_pos = vect3(0,0,0);
		float	weight = 0.0;
		float	radius = 0;
		Vt( bool b, float w, vect3 p = vect3(0,0,0) ) : Obj(p), bMove(b), weight(w) 
		{
			radius = cbrt(w);
		}
	};
	struct box
	{
		vector<shared_ptr<Vt>>		vt;
	} ball;

	vector<shared_ptr<Obj>>	tbl_pObj;

	bool bPause = false;
	bool bStep = false;
};

Lab22::Lab22() : pImpl( new Lab22::Impl ){}

//------------------------------------------------------------------------------
void Lab22::Update( SysKeys& keys, SysMouse& mouse, SysSound& sound, SysGra& gra, Pers& pers, float delta, int& text_y, Cp& cp )
//------------------------------------------------------------------------------
{
	pImpl->bStep = false;
	auto funcShowBar = []( SysGra& gra, int y, float val, string str, rgb col )
	{
		vect2 v0 = vect2(0.0,0.75)+gra.Dot(0,42.0*y);
		gra.Line( v0, v0+ vect2( val, 0 ), col, 2 );
		gra.Print( v0+gra.Dot(0,-6), str + to_string(val), col );
	};

	// 画面クリア
	gra.Clr(rgb(0.3,0.3,0.3));
	gra.Print(1,(float)text_y++,"22 : 2D twin box" ); 

	// グリッド表示
	pers.grid.DrawGrid3d( gra, pers, vect3(0,0,0), mrotx(rad(90)), 26, 26, 1, rgb(0.2,0.2,0.2), true );

	//初期化
	if ( !m.bInitAll )
	{
		m.bInitAll = true;

		// カメラ
		pers.cam.pos	= vect3( 0.0, 1.0, -16.0 );
		pers.cam.at		= vect3( 0.0, 1.0, 0 );

		pImpl->ball.vt.clear();
		pImpl->ball.vt.emplace_back( new Impl::Vt(true,1) );
		pImpl->ball.vt.emplace_back( new Impl::Vt(true,4) );
		pImpl->ball.vt.emplace_back( new Impl::Vt(false,0.2,vect3(-8,cbrt(0.2),0)) );
		pImpl->ball.vt.emplace_back( new Impl::Vt(false,0.2,vect3( 8,cbrt(0.2),0) ) );

		// プロット
		pers.grid.plot.ResetPlot();
	}

	// 初期化
	if ( !m.bInitParam )
	{
		m.bInitParam = true;

		pImpl->ball.vt[0]->pos = vect3( -3, pImpl->ball.vt[0]->radius, 0 );
		pImpl->ball.vt[0]->vel = vect3(0,0,0);

		pImpl->ball.vt[1]->pos = vect3(  3, pImpl->ball.vt[1]->radius, 0 );
		pImpl->ball.vt[1]->vel = vect3(-0.05,0,0);
	}

	Impl::Vt& t0 = *pImpl->ball.vt[0];
	Impl::Vt& t1 = *pImpl->ball.vt[1];

	// 入力
	if ( keys.R.hi )	m.bInitParam = false;
	if ( keys.SPACE.hi )	{pImpl->bPause=!pImpl->bPause;}
	if ( keys.ENTER.rep )	{pImpl->bStep=true;pImpl->bPause=true;}
	
	// 加速
	if ( mouse.F.on )	t0.vel += vect3( 0.005 , 0 , 0 );
	if ( mouse.B.on )	t1.vel += vect3(-0.01 , 0 , 0 );

	// 移動：計算
	if ( !pImpl->bPause || pImpl->bStep )
	for ( shared_ptr<Impl::Vt> vt : pImpl->ball.vt ) { vt->pos += vt->vel; }

	// 衝突：運動準備
	for ( shared_ptr<Impl::Vt> vt : pImpl->ball.vt ) 
	{
		vt->new_pos = vt->pos;
		vt->new_vel = vt->vel;
	}

	// 衝突：計算
	for ( shared_ptr<Impl::Vt>pa : pImpl->ball.vt )
	{
		for ( shared_ptr<Impl::Vt>pb : pImpl->ball.vt )
		{
			if ( pa != pb )
			{
				if ( abs( pa->pos.x - pb->pos.x ) < pa->radius+pb->radius )
				{
					float wa = pa->weight;
					float wb = pb->weight;
					vect3 va = pa->vel;
					vect3 vb = pb->vel;

					pa->new_pos = pa->prev_pos;	// 衝突前の場所に戻す簡易処理

					if ( pa->bMove && pb->bMove )
					{
						pa->new_vel = vb*wb/wa;


						sound.mml_play( "T1800O5V10#cdr");
					}
					else
					if ( pa->bMove )
					{
						pa->new_vel = -pa->vel;

						sound.mml_play( "T1800O5V10c#cr");
					}
				}
			}
		}
	}

	// 衝突：運動反映
	if ( !pImpl->bPause || pImpl->bStep )
	for ( shared_ptr<Impl::Vt> vt : pImpl->ball.vt ) 
	{
		vt->pos = vt->new_pos;
		vt->vel = vt->new_vel;
	}
	
	// プロット
	pers.grid.plot.WritePlot( t0.vel.x);

	// 描画：箱
	{
		gra.SetZTest(false);
		for ( shared_ptr<Impl::Vt>p : pImpl->ball.vt )
		{
			vect3 v0 = p->pos;
			float 	r = p->radius;
			float	wide = 1.0;
			pers.pen.line3d( gra, pers, v0+vect3(-r,-r,0), v0+vect3( r,-r,0),rgb(1,1,1), wide );
			pers.pen.line3d( gra, pers, v0+vect3( r,-r,0), v0+vect3( r, r,0),rgb(1,1,1), wide );
			pers.pen.line3d( gra, pers, v0+vect3( r, r,0), v0+vect3(-r, r,0),rgb(1,1,1), wide );
			pers.pen.line3d( gra, pers, v0+vect3(-r, r,0), v0+vect3(-r,-r,0),rgb(1,1,1), wide );
		}
		gra.SetZTest(true);
	}

	// 保存
	for ( shared_ptr<Impl::Vt> vt : pImpl->ball.vt ) 
	{
		vt->prev_pos = vt->pos;
	}
	
	// メーター表示
	int m_y = 0;
	funcShowBar( gra, m_y++, t0.vel.x,	"0 vel    ", rgb(1,1,1) );
	funcShowBar( gra, m_y++, t1.vel.x,	"1 vel    ", rgb(1,1,1) );

	// プロット表示
//	pImpl->plot_moment.DrawPlot( gra, pers );

	gra.Print(1,(float)text_y++,string("<ENTER on>")+to_string(keys.ENTER.on)); 
	if ( pImpl->bPause ) gra.Print(1,(float)text_y++,string("<Pause>")); 
	if ( pImpl->bStep ) gra.Print(1,(float)text_y++,string("<Step>")); 

}
