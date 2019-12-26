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
#include "lab27.h"



struct Lab27::Impl
{
	bool	bResetAll = true;
	bool	bResetParam = true;
	bool	bPause = false;
	bool	bStep = false;

	vector<shared_ptr<Obj>>	tbl_pObj;

	vector<int>	idxPoint;
};

Lab27::Lab27() : pImpl( new Lab27::Impl ){}


//------------------------------------------------------------------------------
void Lab27::Update( SysKeys& keys, SysMouse& mouse, SysSound& sound, SysGra& gra, Pers& pers, float delta, int& text_y, Cp& cp )
//------------------------------------------------------------------------------
{
	// 画面クリア
	gra.Clr(rgb(0.3,0.3,0.3));
	pers.grid.DrawGrid3d( gra, pers, vect3(0,0,0), midentity(), 10, 10, 1, rgb(0.2,0.2,0.2) );
	gra.Print(1,(float)text_y++,"27 :  B-Spline"); 

	//初期化
	if ( pImpl->bResetAll )
	{
		pImpl->bResetAll = false;

		// カメラ
		pers.cam.pos = vect3( 0.0, 2.0, -5.0 );
		pers.cam.at = vect3( 0,  1.0, 0 );

		// 点
		pImpl->tbl_pObj.emplace_back( new Point3(vect3(-1.0, 0.0, 0.0 ),vect3( 0.0, 0.0, 0.5 ),vect3( 0.0, 0.0,-0.5 )) );
		pImpl->tbl_pObj.emplace_back( new Point3(vect3( 0.0, 0.0,-1.0 ),vect3(-0.5, 0.0, 0.0 ),vect3( 0.5, 0.0, 0.0 )) );
		pImpl->tbl_pObj.emplace_back( new Point3(vect3( 1.0, 0.0, 0.0 ),vect3( 0.0, 0.0,-0.5 ),vect3( 0.0, 0.0, 0.5 )) );
		pImpl->tbl_pObj.emplace_back( new Point3(vect3( 0.0, 0.0, 1.0 ),vect3( 0.5, 0.0, 0.0 ),vect3(-0.5, 0.0, 0.0 )) );

		// インデックス
		pImpl->idxPoint.emplace_back( 0 );
		pImpl->idxPoint.emplace_back( 1 );
		pImpl->idxPoint.emplace_back( 2 );
		pImpl->idxPoint.emplace_back( 3 );
		pImpl->idxPoint.emplace_back( 0 );

		//GUI登録
		cp.tbltbl_pObj.clear();
		cp.tbltbl_pEdge.clear();
		cp.tbltbl_pObj.emplace_back( pImpl->tbl_pObj );



	}
	{

		{
			//ベジェ計算＆描画
			int div = 10;
			float dt = 1/(float)div;

			int size = static_cast<signed>(pImpl->idxPoint.size());

			for ( int n = 0 ; n < size-1 ; n++ )
			{
				int n0 = pImpl->idxPoint[n];
				int n1 = pImpl->idxPoint[n+1];

				Point3* p0 = dynamic_cast<Point3*>(pImpl->tbl_pObj[n0].get());
				Point3* p1 = dynamic_cast<Point3*>(pImpl->tbl_pObj[n1].get());

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


					pos0=pos1;
					t+=dt;
				}

			}

		}



	}


	
}
