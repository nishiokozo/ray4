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
#include "lab2.h"
		
using namespace std;

struct Lab2::Impl
{
	bool	bResetAll = true;
	bool	bResetParam = true;
	bool	bPause = false;
	bool	bStep = false;

	vector<shared_ptr<Obj>>	tbl_pObj;
	vector<shared_ptr<Edge>>	tbl_pEdge;


	bool bShot = false;
	vect3	velocity;
	vect3	moment;
	vect3	to;
	vect3	add;
	float 	w;	//	角速度
};	
Lab2::Lab2() : pImpl( new Lab2::Impl ){}

//=================================
// 描画	角速度 実験
//=================================
//------------------------------------------------------------------------------
void Lab2::Update( SysKeys& keys, SysMouse& mouse, SysSound& sound, SysGra& gra, Pers& pers, float delta, int& text_y, Cp& cp )
//------------------------------------------------------------------------------
{
//	const float	G	= -9.80665;				// 重力加速度

	//画面クリア
	gra.Clr(rgb(0.3,0.3,0.3));
	pers.grid.DrawGrid3d( gra, pers, vect3(0,0,0), mat33::mrotx(rad(90)), 10, 10, 1, rgb(0.2,0.2,0.2) );

	gra.Print(1,(float)text_y++,string("<<lab2_kakusokudo>>")); 

//	const float	g = G *delta*delta;		// 重力加速度/frame

	// 初期化
	if ( pImpl->bResetAll )
	{
		pImpl->bResetAll = false;
		pers.cam.pos = vect3( 2.0, 2.5, -2.0 );
		pers.cam.at = vect3( 0,  0.0, 0 );
		pers.cam.Update();
	}

	if ( pImpl->bResetParam )
	{
		pImpl->bResetParam = false;

		pImpl->tbl_pObj.clear();
		pImpl->tbl_pObj.emplace_back( new Obj(vect3(0  ,0.1,0)) );
		pImpl->tbl_pObj.emplace_back( new Obj(vect3(1  ,0.1,0)) );
		pImpl->tbl_pObj.emplace_back( new Obj(vect3(1.5,0.1,0.2)) );
		// 線
		pImpl->tbl_pEdge.clear();
		pImpl->tbl_pEdge.emplace_back( new Edge(0,1) );
		pImpl->tbl_pEdge.emplace_back( new Edge(1,2,rgb(0,1,0),1) );

		//GUI登録
		cp.tbltbl_pObj.clear();
		cp.tbltbl_pEdge.clear();
		cp.tbltbl_pObj.emplace_back( pImpl->tbl_pObj );
		cp.tbltbl_pEdge.emplace_back( pImpl->tbl_pEdge );

		pImpl->bShot = false;
	}


	vect3&	v0 = pImpl->tbl_pObj[0]->pos;	//	barの根本
	vect3&	v1 = pImpl->tbl_pObj[1]->pos;	//	barの先端
	vect3&	v2 = pImpl->tbl_pObj[2]->pos;	// 速度指定


	//入力
	{
		// リセット
		if ( keys.R.hi )	pImpl->bResetParam = true ;

		if ( mouse.B.hi )	pImpl->bShot = !pImpl->bShot ;
	}

	// 計算
		
	// 描画
	{
		if ( pImpl->bShot )
		{
			vect3	bar = (v1-v0);							//	棒
//			float	radius = bar.abs();

			pImpl->to = mrotateByAxis( pImpl->moment, pImpl->w ) * bar;			//	移動計算

			vect3 add  = pImpl->to - bar;

			v1 += add;
			
			
		}
		else
		{
			vect3	bar = (v1-v0);							//	棒
			float	radius = bar.abs();
			pImpl->moment = cross(-bar,v2-v1);						//	回転モーメント
			pImpl->velocity = cross(bar/radius, pImpl->moment/radius );	//	ベロシティ

			pImpl->w = pImpl->velocity.abs()/radius;
			pImpl->to = mrotateByAxis( pImpl->moment, pImpl->w ) * bar;			//	移動計算

		}
/*

		{// 影 描画
			vect3	va = v0;va.y = 0;
			vect3	vb = v1;vb.y = 0;
			pers.pen.Line3d( gra, pers, va, vb, rgb(1,1,1)/4, 2 );
		}
		{// 影 描画
			vect3	va = v1;va.y = 0;
			vect3	vb = v2;vb.y = 0;
			pers.pen.Line3d( gra, pers, va, vb, rgb(1,1,1)/4, 2 );
		}

		pers.pen.Line3d( gra, pers, v0, v1, rgb(1,1,1), 2 );	//	棒
		pers.pen.Line3d( gra, pers, v1, v2, rgb(0,1,0), 1 );	// 外的な力
*/
		pers.prim.DrawArrow( gra, pers, text_y, v1, pImpl->velocity	,1	, rgb(1,1,0), "pImpl->velocity" );
		pers.prim.DrawArrow( gra, pers, text_y, v0, pImpl->moment		,1	, rgb(1,0,1), "pImpl->moment" );
		pers.prim.DrawArrow( gra, pers, text_y, v0, pImpl->to			,1	, rgb(0,1,1), "pImpl->to" );
			
	}

	// 1/60sec 待
	gra.Wait();

}
