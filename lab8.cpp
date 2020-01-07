//2019/10/17

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
#include "lab8.h"
struct Lab8::Impl
{
	bool	bResetAll = true;
	bool	bResetParam = true;
	bool	bPause = false;
	bool	bStep = false;

	vector<shared_ptr<Obj>>	tbl_pObj;
	vector<shared_ptr<Edge>>	tbl_pEdge;

	struct	Graphs
	{
		struct Plot
		{
			//	y = 0面に座標(0,0,0)を中心にグラフ（ドット）を打ってゆく

			int MaxPlot = 100;
			int cntPlot = 0;
			bool	bScroll = false;
			rgb col;
			float step;

			vector<float> tblDot;
			int amtPlot = 0;
			
			//------------------------------------------------------------------------------
			Plot( 
			//------------------------------------------------------------------------------
				int	_max,
				float _step, 			// 送りステップ
				rgb _col = rgb(1,1,1) 	// 色
			)
			{
				MaxPlot = _max;

				col = _col;
				step = _step;
				tblDot.clear();
				ResetPlot();
			}

			//------------------------------------------------------------------------------
			void ResetPlot()
			//------------------------------------------------------------------------------
			{
				bScroll = false;
				cntPlot = 0 ;

				amtPlot = 0 ;
				tblDot.clear();
			}
			//------------------------------------------------------------------------------
			void WritePlot( float val )
			//------------------------------------------------------------------------------
			{
				if ( cntPlot >= MaxPlot ) 
				{
					bScroll = true;
					cntPlot = 0;
				}

		
				if ( amtPlot >= MaxPlot ) 
				{
					int idx = amtPlot % MaxPlot;
					tblDot[idx] = val;
				}
				else
				{
					tblDot.emplace_back( val );
				}
				amtPlot++;
				cntPlot++;
			}
			//------------------------------------------------------------------------------
			void DrawPlot( SysGra& gra, Pers& pers )
			//------------------------------------------------------------------------------
			{
				gra.SetZTest( false );
				
				float x = 0;
				
				if ( bScroll )
				for ( int i = cntPlot ; i < MaxPlot ; i++ )
				{
					pers.pen.pset3d( gra, pers, vect3( x, 0, tblDot[i] ),  col, 2 );
					x += step;
				}
				for ( int i = 0 ; i < cntPlot-1 ; i++ )
				{
					pers.pen.pset3d( gra, pers, vect3( x, 0, tblDot[i] ),  col, 2 );
					x += step;
				}
				if ( cntPlot > 0 ) pers.pen.pset3d( gra, pers, vect3( x, 0, tblDot[cntPlot-1] ),  rgb(1,1,1), 2 );
				gra.SetZTest( true );
			}

		};

		vector<Plot>	plots;

		//------------------------------------------------------------------------------
		int Create(
		//------------------------------------------------------------------------------
			float _step, 			// 送りステップ
			rgb _col = rgb(1,1,1) 	// 色
		)
		{
			int idx = (signed)plots.size(); 
			plots.emplace_back( 100,_step, _col );
			return idx;
		}

		//------------------------------------------------------------------------------
		void Clear()
		//------------------------------------------------------------------------------
		{
			plots.clear();
		}
		
		//------------------------------------------------------------------------------
		void Request( int idx, float val )
		//------------------------------------------------------------------------------
		{
			if ( idx >= (signed)plots.size() ) return;
			plots[ idx ].WritePlot( val );
		}

		//------------------------------------------------------------------------------
		void Draw( SysGra& gra, Pers& pers )
		//------------------------------------------------------------------------------
		{
			for ( Plot& p : plots )
			{
				p.DrawPlot( gra, pers );
			}
		}
	};

	Graphs::Plot plot_moment = Graphs::Plot( 100, 0.02, rgb(1,0,1) );
	vect3 		vel;
	float		w = 0;

};
Lab8::Lab8() : pImpl( new Lab8::Impl ){}
//------------------------------------------------------------------------------
void Lab8::Update( SysKeys& keys, SysMouse& mouse, SysSound& sound, SysGra& gra, Pers& pers, float delta, int& text_y, Cp& cp )
//------------------------------------------------------------------------------
{
	const float	G	= -9.80665;				// 重力加速度

	//画面クリア
	gra.Clr(rgb(0.3,0.3,0.3));
	pers.grid.DrawGrid3d( gra, pers, vect3(0,0,0), mat33::midentity(), 10, 10, 1, rgb(0.2,0.2,0.2) );

	gra.Print(1,(float)text_y++,string("lab8_vector_six_lab8")); 

	const float	g = G *delta*delta;			// 重力加速度/frame
	const vect3	vg = vect3(0,0, -g);	// 重力加速度/frame

	bool bStep = false;

	// 初期化
	if ( pImpl->bResetParam )
	{
		if ( pImpl->bResetAll )
		{
			pImpl->bResetAll = false;
			pers.cam.pos = vect3(  0.0, 5.0, -1.0 );
			pers.cam.at = vect3( 0,  0.0, 0 );
			pers.cam.Update();
		}
	
		pImpl->bResetParam = false;
		//点
		pImpl->tbl_pObj.clear();
		pImpl->tbl_pObj.emplace_back( new Obj(vect3( 0  , 0.1, 0 )) );
		pImpl->tbl_pObj.emplace_back( new Obj(vect3( 1  , 0.1, 0 )) );
		// 線
		pImpl->tbl_pEdge.clear();
		pImpl->tbl_pEdge.emplace_back( new Edge(0,1) );

		//GUI登録
		cp.tbltbl_pObj.clear();
		cp.tbltbl_pEdge.clear();
		cp.tbltbl_pObj.emplace_back( pImpl->tbl_pObj );
		cp.tbltbl_pEdge.emplace_back( pImpl->tbl_pEdge );

		pers.axis.bAxisX = true;
		pers.axis.bAxisY = false;
		pers.axis.bAxisZ = true;

		pImpl->vel=vect3(0,0,0);	// 加速度
		pImpl->w = 0;

		pImpl->plot_moment.ResetPlot();
	}

	// 入力
	if ( keys.R.hi )	pImpl->bResetParam = true;
	if ( keys.SPACE.hi )	pImpl->bPause = !pImpl->bPause ;
	if ( keys.ENTER.rep )	{bStep = true; pImpl->bPause = true; }

	vect3&	v0 = pImpl->tbl_pObj[0]->pos;
	vect3&	v1 = pImpl->tbl_pObj[1]->pos;

	vect3	bar = (v1-v0);					//	棒
	vect3	moment = cross(-bar, pImpl->vel+vg);	//	回転モーメント 仮
	vect3	F = cross(bar, moment );		//	力

	// 計算
	if ( !pImpl->bPause || bStep )
	{
		pImpl->plot_moment.WritePlot( moment.y*10 );

			
		// 衝突
		{
			pImpl->vel = mrotateByAxis( moment, (pImpl->vel+vg).abs() )*pImpl->vel+vg;

			v1 += pImpl->vel;

//			mat33 m = mrotateByAxis( moment, pImpl->w );

//			v1 = m * (v1-v0) + v0;
//			pImpl->w += F.abs();
//			pImpl->vel += F;
		}
 		
	}

	pers.prim.DrawVect( gra, pers, text_y, v0, moment ,10	, rgb(1,0,1), "moment" );
	pers.prim.DrawVect( gra, pers, text_y, v1, vg		,100	, rgb(1,0,0), "vg" );
	pers.prim.DrawVect( gra, pers, text_y, v1, F		,1		, rgb(0,1,0), "F" );
	pImpl->plot_moment.DrawPlot( gra, pers );
	gra.Print(1,(float)text_y++,string("<<radius>>")+to_string(bar.abs())); 

	// 1/60sec 待
	gra.Wait();

}
