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
#include "lab24.h"

struct Lab24::Impl
{
	bool	bResetAll = true;
	bool	bResetParam = true;
	bool	bPause = false;
	bool	bStep = false;

	vector<shared_ptr<Obj>>	tbl_pObj;

	vector<int>	idxPoint;

};
Lab24::Lab24() : pImpl( new Lab24::Impl ){}

//------------------------------------------------------------------------------
void Lab24::Update( SysKeys& keys, SysMouse& mouse, SysSound& sound, SysGra& gra, Pers& pers, float delta, int& text_y, Cp& cp )
//------------------------------------------------------------------------------
{
	// 画面クリア
	gra.Clr(rgb(0.3,0.3,0.3));
	pers.grid.DrawGrid3d( gra, pers, vect3(0,0,0), midentity(), 10, 10, 1, rgb(0.2,0.2,0.2) );
	gra.Print(1,(float)text_y++,"24 : Nurbs surface(one)" ); 

	//初期化
	if ( pImpl->bResetAll )
	{
		pImpl->bResetAll = false;

		// カメラ
		pers.cam.pos = vect3( 2.0, 4.0, -5.0 );
		pers.cam.at = vect3( 0,  0.5, 0 );
	}

	if ( pImpl->bResetParam )
	{
		pImpl->bResetParam = false;

		// 点
//		for ( auto o : pImpl->tbl_pObj ) delete o;
		pImpl->tbl_pObj.clear();
		pImpl->tbl_pObj.emplace_back( new Obj(vect3(-1.0, 1.0, 1.0 ) ) );
		pImpl->tbl_pObj.emplace_back( new Obj(vect3(-0.5, 1.0, 1.0 ) ) );
		pImpl->tbl_pObj.emplace_back( new Obj(vect3(-1.0, 1.0,  0.5 ) ) );

		pImpl->tbl_pObj.emplace_back( new Obj(vect3( 1.0, 0.0, 1.0 ) ) );
		pImpl->tbl_pObj.emplace_back( new Obj(vect3( 1.0, 1.0, 1.0 ) ) );
		pImpl->tbl_pObj.emplace_back( new Obj(vect3( 1.0, 0.0,  0.5 ) ) );

		pImpl->tbl_pObj.emplace_back( new Obj(vect3( 1.0, 0.0, -1.0 ) ) );
		pImpl->tbl_pObj.emplace_back( new Obj(vect3( 0.5, 0.0, -1.0 ) ) );
		pImpl->tbl_pObj.emplace_back( new Obj(vect3( 1.0, 0.0, -0.5 ) ) );

		pImpl->tbl_pObj.emplace_back( new Obj(vect3(-1.0, 0.0, -1.0 ) ) );
		pImpl->tbl_pObj.emplace_back( new Obj(vect3(-0.5, 0.0, -1.0 ) ) );
		pImpl->tbl_pObj.emplace_back( new Obj(vect3(-1.0, 1.0, -1.0 ) ) );

		// U
		pImpl->idxPoint.clear();
		pImpl->idxPoint.emplace_back( 0 );
		pImpl->idxPoint.emplace_back( 1 );
		pImpl->idxPoint.emplace_back( 4 );
		pImpl->idxPoint.emplace_back( 3 );

		pImpl->idxPoint.emplace_back( 9 );
		pImpl->idxPoint.emplace_back( 10 );
		pImpl->idxPoint.emplace_back( 7 );
		pImpl->idxPoint.emplace_back( 6 );

		 // V
		pImpl->idxPoint.emplace_back( 0 );
		pImpl->idxPoint.emplace_back( 2 );
		pImpl->idxPoint.emplace_back( 11);
		pImpl->idxPoint.emplace_back( 9 );

		pImpl->idxPoint.emplace_back( 3 );
		pImpl->idxPoint.emplace_back( 5 );
		pImpl->idxPoint.emplace_back( 8 );
		pImpl->idxPoint.emplace_back( 6 );

		//GUI登録
		cp.tbltbl_pObj.clear();
		cp.tbltbl_pObj.emplace_back( pImpl->tbl_pObj );
	}
	// 入力
	if ( keys.R.hi )		pImpl->bResetParam = true;


//	for ( int i = 0 ; i < 4 ; i++ )
	{
		// U
		int uan = 0;
		int uan0 = pImpl->idxPoint[uan+0];
		int uan1 = pImpl->idxPoint[uan+1];
		int uan2 = pImpl->idxPoint[uan+2];
		int uan3 = pImpl->idxPoint[uan+3];

		int ubn = 4;
		int ubn0 = pImpl->idxPoint[ubn+0];
		int ubn1 = pImpl->idxPoint[ubn+1];
		int ubn3 = pImpl->idxPoint[ubn+3];
		int ubn2 = pImpl->idxPoint[ubn+2];

		vect3 uaP0 = pImpl->tbl_pObj[uan0]->pos;
		vect3 uaP1 = pImpl->tbl_pObj[uan1]->pos;
		vect3 uaP3 = pImpl->tbl_pObj[uan3]->pos;
		vect3 uaP2 = pImpl->tbl_pObj[uan2]->pos;

		vect3 ubP0 = pImpl->tbl_pObj[ubn0]->pos;
		vect3 ubP1 = pImpl->tbl_pObj[ubn1]->pos;
		vect3 ubP3 = pImpl->tbl_pObj[ubn3]->pos;
		vect3 ubP2 = pImpl->tbl_pObj[ubn2]->pos;

		// V
		int van = 8;
		int van0 = pImpl->idxPoint[van+0];
		int van1 = pImpl->idxPoint[van+1];
		int van3 = pImpl->idxPoint[van+3];
		int van2 = pImpl->idxPoint[van+2];

		int vbn = 12;
		int vbn0 = pImpl->idxPoint[vbn+0];
		int vbn1 = pImpl->idxPoint[vbn+1];
		int vbn3 = pImpl->idxPoint[vbn+3];
		int vbn2 = pImpl->idxPoint[vbn+2];

		vect3 vaP0 = pImpl->tbl_pObj[van0]->pos;
		vect3 vaP1 = pImpl->tbl_pObj[van1]->pos;
		vect3 vaP3 = pImpl->tbl_pObj[van3]->pos;
		vect3 vaP2 = pImpl->tbl_pObj[van2]->pos;

		vect3 vbP0 = pImpl->tbl_pObj[vbn0]->pos;
		vect3 vbP1 = pImpl->tbl_pObj[vbn1]->pos;
		vect3 vbP3 = pImpl->tbl_pObj[vbn3]->pos;
		vect3 vbP2 = pImpl->tbl_pObj[vbn2]->pos;

		vect3 va10 = vaP1-vaP0;
		vect3 va23 = vaP2-vaP3;
		vect3 vb10 = vbP1-vbP0;
		vect3 vb23 = vbP2-vbP3;

		{
			int udiv = pow(2,5.0);	// floatで計算誤差が出ないよう2^nにしておく
			int vdiv = pow(2,5.0);	// floatで計算誤差が出ないよう2^nにしておく

			for ( float ut = 0 ; ut <= 1.0 ; ut +=1.0/udiv )
			{
				vect3 ua = func_bezier3( ut, uaP0, uaP1, uaP2, uaP3 );
				vect3 ub = func_bezier3( ut, ubP0, ubP1, ubP2, ubP3 );

				// V
				{
					vect3 v10 = (va10)*(1.0-ut) + (vb10)*ut;
					vect3 v23 = (va23)*(1.0-ut) + (vb23)*ut;
					vect3 vP0 = ua;
					vect3 vP1 = vP0+v10;
					vect3 vP3 = ub;
					vect3 vP2 = vP3+v23;

					for ( float vt = 0 ; vt <= 1.0 ; vt +=1.0/vdiv )
					{
						vect3 vpos = func_bezier3( vt, vP0, vP1, vP2, vP3 );
						pers.pen.pset3d( gra, pers, vpos, rgb(1,1,1), 1 );

					}
				}


			}

			
		}
	}
}
