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
#include "lab14.h"

using namespace std;


struct Lab14::Impl
{
	bool	bResetAll = true;
	bool	bResetParam = true;
	bool	bPause = false;
	bool	bStep = false;

	vector<shared_ptr<Obj>>	tbl_pObj;
};
Lab14::Lab14() : pImpl( new Lab14::Impl ){}

//------------------------------------------------------------------------------
void Lab14::Update( SysKeys& keys, SysMouse& mouse, SysSound& sound, SysGra& gra, Pers& pers, float delta, int& text_y, Cp& cp )
//------------------------------------------------------------------------------
{
	// 画面クリア
	gra.Clr(rgb(0.3,0.3,0.3));
	pers.grid.DrawGrid3d( gra, pers, vect3(0,0,0), mat33::midentity(), 10, 10, 1, rgb(0.2,0.2,0.2) );
	gra.Print(1,(float)text_y++,"14 : Catmull Rom Spline"); 

	//初期化
	if ( pImpl->bResetAll )
	{
		pImpl->bResetAll = false;

		// カメラ
		pers.cam.pos = vect3( 0.0, 2.0, -5.0 );
		pers.cam.at = vect3( 0,  1.0, 0 );
		pers.cam.Update();

		// 点
		pImpl->tbl_pObj.emplace_back( new Obj( vect3(	-0.5,	0.12,	-0.5) ) );
		pImpl->tbl_pObj.emplace_back( new Obj( vect3(	+0.5,	0.12,	-0.5) ) );
		pImpl->tbl_pObj.emplace_back( new Obj( vect3(	+0.9,	0.12,	 0.5) ) );
		pImpl->tbl_pObj.emplace_back( new Obj( vect3(	 0.0,	0.12,	 0.5) ) );
		pImpl->tbl_pObj.emplace_back( new Obj( vect3(	-0.9,	0.12,	 0.5) ) );

		//GUI登録
		cp.tbltbl_pObj.clear();
		cp.tbltbl_pEdge.clear();
		cp.tbltbl_pObj.emplace_back( pImpl->tbl_pObj );
	}

	// 描画
	vector<int>	tblIdx =
	{
		0,
		1,
		2,
		3,
		4,
	};

	vector<shared_ptr<Obj>> tbl = pImpl->tbl_pObj; 
	{
		// 描画 カーブ
		function<void()> func = [&]()
		{
			rgb	col(1,1,1);
			int size = (signed)tblIdx.size();
			vect3	v0;
			vect3	v2;
			for ( int i = 0 ; i < size ; i++ )
			{
				int n0 = tblIdx[i];
				int n1 = tblIdx[(i+1)%size];
				int n2 = tblIdx[(i+2)%size];
				int n3 = tblIdx[(i+3)%size];

				vect3 P0 = tbl[n0]->pos;
				vect3 P1 = tbl[n1]->pos;
				vect3 P2 = tbl[n2]->pos;
				vect3 P3 = tbl[n3]->pos;

				for ( float t = 0.0 ; t < 1.0 ; t+=0.1 )
				{
					vect3 v1 = func_catmull_rom_spline(t, P0,P1,P2,P3 );
					if ( (i==0 && t==0) ) 
					{
						v2=v1;
					}
					else 
					{
						pers.pen.line3d( gra, pers, v0, v1, col);
						{
							vect3 a = v0;a.y=0;
							vect3 b = v1;b.y=0;
							pers.pen.line3d( gra, pers, a, b, rgb(0.2,0.2,0.2));
						}

					}

					{
						vect3 nx,ny,nz;
						nz = (v1-v0).normalize();
						ny = vect3(0,1,0);
						nx = cross(nz,ny).normalize();
						ny = cross(nx,nz).normalize();

						mat33	m(
							nx.x,	nx.y,	nx.z,
							ny.x,	ny.y,	ny.z,
							nz.x,	nz.y,	nz.z
						);

					}


					v0 = v1;
				}
			}
			pers.pen.line3d( gra, pers, v0, v2, col);
			{
				vect3 a = v0;a.y=0;
				vect3 b = v2;b.y=0;
				pers.pen.line3d( gra, pers, a, b, rgb(0.2,0.2,0.2));
			}
		};
		func();
	}


}
