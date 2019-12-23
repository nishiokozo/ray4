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
#include "lab25.h"

struct Lab25::Impl
{
	bool	bResetAll = true;
	bool	bResetParam = true;
	bool	bPause = false;
	bool	bStep = false;

	vector<shared_ptr<Obj>>	tbl_pObj;

};
Lab25::Lab25() : pImpl( new Lab25::Impl ){}


//------------------------------------------------------------------------------
void Lab25::Update( SysKeys& keys, SysMouse& mouse, SysSound& sound, SysGra& gra, Pers& pers, float delta, int& text_y, Cp& cp )
//------------------------------------------------------------------------------
{
	// 画面クリア
	gra.Clr(rgb(0.3,0.3,0.3));
	pers.grid.DrawGrid3d( gra, pers, vect3(0,0,0), midentity(), 10, 10, 1, rgb(0.2,0.2,0.2) );
	gra.Print(1,(float)text_y++,"25 : Nurbs surfaces" ); 

	//初期化：All
	if ( pImpl->bResetAll )
	{
		pImpl->bResetAll = false;

		// カメラ
		pers.cam.pos = vect3( 2.0, 4.0, -5.0 );
		pers.cam.at = vect3( 0,  0.5, 0 );
	}

	//初期化：パラメータ
	if ( pImpl->bResetParam )
	{
		pImpl->bResetParam = false;

		// 点
		pImpl->tbl_pObj.clear();
		pImpl->tbl_pObj.emplace_back( new Obj(vect3(-1.0, 1.0, 1.0 ) ) );
		pImpl->tbl_pObj.emplace_back( new Obj(vect3(-0.5, 1.0, 1.0 ) ) );
		pImpl->tbl_pObj.emplace_back( new Obj(vect3(-1.0, 1.0, 0.5 ) ) );

		pImpl->tbl_pObj.emplace_back( new Obj(vect3( 1.0, 0.0, 1.0 ) ) );
		pImpl->tbl_pObj.emplace_back( new Obj(vect3( 1.0, 1.0, 1.0 ) ) );
		pImpl->tbl_pObj.emplace_back( new Obj(vect3( 1.0, 0.0, 0.5 ) ) );

		pImpl->tbl_pObj.emplace_back( new Obj(vect3( 1.0, 0.0, -1.0 ) ) );
		pImpl->tbl_pObj.emplace_back( new Obj(vect3( 0.5, 0.0, -1.0 ) ) );
		pImpl->tbl_pObj.emplace_back( new Obj(vect3( 1.0, 0.0, -0.5 ) ) );

		pImpl->tbl_pObj.emplace_back( new Obj(vect3(-1.0, 0.0, -1.0 ) ) );
		pImpl->tbl_pObj.emplace_back( new Obj(vect3(-0.5, 0.0, -1.0 ) ) );
		pImpl->tbl_pObj.emplace_back( new Obj(vect3(-1.0, 1.0, -1.0 ) ) );

		//GUI登録
		cp.tbltbl_pObj.clear();
		cp.tbltbl_pObj.emplace_back( pImpl->tbl_pObj );
	}

	// 入力
	if ( keys.R.hi )	pImpl->bResetParam = true;

	{
		vect3 p0  = pImpl->tbl_pObj[0]->pos;
		vect3 p0u = pImpl->tbl_pObj[1]->pos - p0;
		vect3 p0v = pImpl->tbl_pObj[2]->pos - p0;

		vect3 p1  = pImpl->tbl_pObj[3]->pos;
		vect3 p1u = pImpl->tbl_pObj[4]->pos - p1;
		vect3 p1v = pImpl->tbl_pObj[5]->pos - p1;

		vect3 p2  = pImpl->tbl_pObj[9]->pos;
		vect3 p2u = pImpl->tbl_pObj[10]->pos - p2;
		vect3 p2v = pImpl->tbl_pObj[11]->pos - p2;

		vect3 p3  = pImpl->tbl_pObj[6]->pos;
		vect3 p3u = pImpl->tbl_pObj[7]->pos - p3;
		vect3 p3v = pImpl->tbl_pObj[8]->pos - p3;



		auto funcDrawSurface = []( SysGra& gra, Pers& pers,
			vect3 p0,vect3 p1,vect3 p2,vect3 p3,
			vect3 ua10, vect3 ua23, vect3 ub10, vect3 ub23,
			vect3 va10, vect3 va23, vect3 vb10, vect3 vb23
		)
		{
			int udiv = pow(2,5.0);	// floatで計算誤差が出ないよう2^nにしておく
			int vdiv = pow(2,5.0);	// floatで計算誤差が出ないよう2^nにしておく

			// U
			for ( float ut = 0 ; ut <= 1.0 ; ut +=1.0/udiv )
			{
				vect3 ua = func_bezier3( ut, p0, p0+ua10, p1+ua23, p1 );
				vect3 ub = func_bezier3( ut, p2, p0+ub10, p3+ub23, p3 );

				{
					vect3 v10 = (va10)*(1.0-ut) + (vb10)*ut;
					vect3 v23 = (va23)*(1.0-ut) + (vb23)*ut;
					vect3 vP0 = ua;
					vect3 vP1 = vP0+v10;
					vect3 vP3 = ub;
					vect3 vP2 = vP3+v23;

					// V
					for ( float vt = 0 ; vt <= 1.0 ; vt +=1.0/vdiv )
					{
						vect3 vpos = func_bezier3( vt, vP0, vP1, vP2, vP3 );
						pers.pen.pset3d( gra, pers, vpos, rgb(1,1,1), 1 );

					}
				}


			}
		};

		funcDrawSurface( gra, pers, 
			p0,		p1,		p2,		p3,
			p0u,	p1u,	p2u,	p3u,
			p0v,	p1v,	p2v,	p3v
		);
	}
}
