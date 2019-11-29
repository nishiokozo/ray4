//2019/11/22

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
#include "lab15.h"



struct Lab15::Impl
{
	vector<int>	idxPoint;
};

//------------------------------------------------------------------------------
Lab15::Lab15() : pImpl( new Lab15::Impl )
//------------------------------------------------------------------------------
{
}


//------------------------------------------------------------------------------
void Lab15::Update( SysKeys& keys, SysMouse& mouse, SysGra& gra, Pers& pers, float delta, int& text_y, Cp& cp )
//------------------------------------------------------------------------------
{
	// 画面クリア
	gra.Clr(rgb(0.3,0.3,0.3));
	pers.grid.DrawGrid3d( gra, pers, vect3(0,0,0), midentity(), 10, 10, 1, rgb(0.2,0.2,0.2) );
	gra.Print(1,(float)text_y++," : " + string(__func__ )); 

	//初期化
	if ( !m.bInitAll )
	{
		m.bInitAll = true;

		// カメラ
		pers.cam.pos = vect3( 0.0, 2.0, -5.0 );
		pers.cam.at = vect3( 0,  1.0, 0 );

	#if 1
		// 点
		m.tbl_pObj.emplace_back( new Point3(vect3(-1.0, 0.0, 0.0 ),vect3( 0.0, 0.0, 0.5 ),vect3( 0.0, 0.0,-0.5 )) );
		m.tbl_pObj.emplace_back( new Point3(vect3( 0.0, 0.0,-1.0 ),vect3(-0.5, 0.0, 0.0 ),vect3( 0.5, 0.0, 0.0 )) );
		m.tbl_pObj.emplace_back( new Point3(vect3( 1.0, 0.0, 0.0 ),vect3( 0.0, 0.0,-0.5 ),vect3( 0.0, 0.0, 0.5 )) );
		m.tbl_pObj.emplace_back( new Point3(vect3( 0.0, 0.0, 1.0 ),vect3( 0.5, 0.0, 0.0 ),vect3(-0.5, 0.0, 0.0 )) );

		// インデックス
		pImpl->idxPoint.emplace_back( 0 );
		pImpl->idxPoint.emplace_back( 1 );
		pImpl->idxPoint.emplace_back( 2 );
		pImpl->idxPoint.emplace_back( 3 );
		pImpl->idxPoint.emplace_back( 0 );
	#else
		// 点
		m.tbl_pObj.emplace_back( new Point3(vect3(-1.0, 0.0, 0.0 ),vect3( 0.0, 0.0, 1.0 ),vect3( 0.0, 0.0,-1.0 )) );
		m.tbl_pObj.emplace_back( new Point3(vect3( 1.0, 0.0, 0.0 ),vect3( 0.0, 0.0,-1.0 ),vect3( 0.0, 0.0, 1.0 )) );

		// インデックス
		pImpl->idxPoint.emplace_back( 0 );
		pImpl->idxPoint.emplace_back( 1 );
		pImpl->idxPoint.emplace_back( 0 );
	#endif

		//GUI登録
		cp.tbltbl_pObj.clear();
		cp.tbltbl_pEdge.clear();
		cp.tbltbl_pObj.emplace_back( m.tbl_pObj );



	}
	{

		// マウスベクトル
		vect3 P = pers.calcScreenToWorld3( vect3(mouse.pos,0) );
		vect3 I = pers.calcRayvect( P );

		bool bDel = keys.D.hi;
		bool bCut = mouse.L.hi && keys.E.on;
		bool bSerch = keys.E.on;

		{
			//ベジェ計算＆描画
			int div = 10;
			float dt = 1/(float)div;

			int size = static_cast<signed>(pImpl->idxPoint.size());

			float	mind = infinit;
			vect3	minQ1;
			vect3	minQ0;
			vect3	minQ;
			int		minn;
			float	mint;
			vect3	mindt;
			bool	minb = false;
			vect3	minv;

			for ( int n = 0 ; n < size-1 ; n++ )
			{
				int n0 = pImpl->idxPoint[n];
				int n1 = pImpl->idxPoint[n+1];

				Point3* p0 = dynamic_cast<Point3*>(m.tbl_pObj[n0]);
				Point3* p1 = dynamic_cast<Point3*>(m.tbl_pObj[n1]);

				vect3 P0 =     p0->pos;
				vect3 P1 = P0 +p0->b;
				vect3 P3 =     p1->pos;
				vect3 P2 = P3 +p1->a;

				float t  = dt;
				vect3 pos0 = p0->pos;
				for ( int i = 0 ; i < div ; i++ )
				{
					vect3 pos1 = func_bezier3( t, P0, P1, P2, P3 );
					pers.pen.line3d( gra, pers, pos0, pos1, rgb(1,1,1) );

					// マウスベクトルとの最近点
					if ( bSerch )
					{
						auto[b,d,Q0,Q1,t0,t1] = func_intersect_Line_SegLine( P, I, pos0, pos1 );
						if ( b ) 
						{
							if ( mind > d && 0.2 > d )
							{
								minb	= true;
								mind	= d;
								minn	= n;

								minQ0 = Q0;
								minQ1 = Q1;

								mint	= t-(1/(float)div) + (t1/(pos1-pos0).abs())/((float)div);
								minQ	= func_bezier3( mint, P0, P1, P2, P3 );
								mindt	= func_bezier3_delta( mint, P0, P1, P2, P3 );

								minv = (minQ-Q0).normalize();
							}
						}

					}

					pos0=pos1;
					t+=dt;
				}

			}

			// マウスベクトルとの最近点表示
			gra.SetZTest( false );
			if ( minb ) 
			{
				pers.pen.pset3d( gra, pers, minQ, rgb(1,0,0), 5 );
			}
			gra.SetZTest( true );

			//--------
			// 切断
			//--------
			if ( minb )
			{
				if ( bCut )
				{
					vect3	q = minQ;
					vect3	v = mindt/3;
					float	t0 = mint;
					float	t1 = 1.0-mint;

					// 頂点生成
					{
						
						m.tbl_pObj.emplace_back( new Point3( q, -v*t0, v*t1 ) );
						pImpl->idxPoint.insert( pImpl->idxPoint.begin()+minn+1, (signed)m.tbl_pObj.size()-1);
					}

					//	接線計算
					{
						Point3* p0 = dynamic_cast<Point3*>(m.tbl_pObj[pImpl->idxPoint[minn+0]]);
						Point3* p2 = dynamic_cast<Point3*>(m.tbl_pObj[pImpl->idxPoint[minn+2]]);
						p0->b *= t0;
						p2->a *= t1;
					}
	/*
					gui.rect_mode = Gui::G_CALC::NONE;


					// 追加したポイントを選択状態にする
					{
						int a = pImpl->idxPoint[minn+1];
						vect3 v = pers.calcWorldToScreen3( minQ );
						//
						gui.one.w = v.z;
						gui.one.idxTbl = m.idxTbl;
						gui.one.idxObj = a;
						gui.one.bEnable = true;
						m.tbl_pObj[ a ]->bSelected = true;
						m.tbl_pObj[ a ]->bPreselect = false;
						gui.one.bSelected_a = false;
						gui.one.bSelected_b = false;
					}
	*/
	
					//GUI登録
					cp.tbltbl_pObj.clear();
					cp.tbltbl_pEdge.clear();
					cp.tbltbl_pObj.emplace_back( m.tbl_pObj );
				}

				{
					gra.SetZTest( false );
					pers.pen.print3d( gra, pers, minQ, -26,-52, to_string(mint) ); 
					gra.SetZTest( true );
				}
			}

			if ( bDel )
			{
				int a = 1;
				m.tbl_pObj.erase( m.tbl_pObj.begin() + a );

				//GUI登録
				cp.tbltbl_pObj.clear();
				cp.tbltbl_pEdge.clear();
				cp.tbltbl_pObj.emplace_back( m.tbl_pObj );
			}

		}



	}


	
}
