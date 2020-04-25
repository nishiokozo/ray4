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
#include "lab4.h"

using namespace std;

struct Lab4::Impl
{
	bool	bResetAll = true;
	bool	bResetParam = true;
	bool	bPause = false;
	bool	bStep = false;

	vector<shared_ptr<Obj>>	tbl_pObj;

	 vect3	vel;			// 運動量
	 vect3	mov;			// 運動量

	 float	rsp = 0;
	 vect3	vv = vect3(0,0,0);
};

Lab4::Lab4() : pImpl( new Lab4::Impl ){}

//------------------------------------------------------------------------------
void Lab4::Update( SysKeys& keys, SysMouse& mouse, SysSound& sound, SysGra& gra, Pers& pers, float delta, int& text_y, Cp& cp )
//------------------------------------------------------------------------------
{
	//画面クリア
	gra.Clr(rgb(0.3,0.3,0.3));
	pers.grid.DrawGrid3d( gra, pers, vect3(0,0,0), mat33::mrotx(rad(0)), 100, 100, 1, vect3(0.2,0.2,0.2) );

//	pers.grid.DrawGrid3d( gra, pers, vect3(0,0,0), mat33::mrotx(rad(90)), 10, 10, 1, rgb(0.2,0.2,0.2) );
		gra.Print(1,(float)text_y++,string("<<lab4_furiko3d>>")); 
		// delta=J dω/dt
		// delta：トルク J：慣性モーメント ω：回転角速度[rad]
		//
		// t=rF
		// r:位置ベクトル t:力のモーメント F:力
		//
		// L=rp
		// r:位置ベクトル L:角運動量(運動量のモーメント) p:運動量

	const float G = 9.80665;			// 重力加速度
	const float g = G *delta*delta;		// 重力加速度/frame
	const vect3 vg = vect3(0,-g,0);	// 重力加速度ベクトル/frame

	bool	bStep = false; 


	// 初期化
	if ( pImpl->bResetParam )
	{
		if ( pImpl->bResetAll )
		{
			pImpl->bResetAll = false;
			pers.cam.pos = vect3(  0.0, 0.0, -7.0 );
			pers.cam.at = vect3( 0,  0.0, 0 );
			pers.cam.Update();
		}

		pImpl->bResetParam = false;
		pImpl->tbl_pObj.clear();
		pImpl->tbl_pObj.emplace_back( new Obj(vect3(0, 2.0, 0)) );
		pImpl->tbl_pObj.emplace_back( new Obj(vect3(-1, 2.0, 0)) );

		//GUI登録
		cp.tbltbl_pObj.clear();
		cp.tbltbl_pEdge.clear();
		cp.tbltbl_pObj.emplace_back( pImpl->tbl_pObj );
		
		pImpl->vel = vect3(0,0,0);
	}

	vect3&	v0 = pImpl->tbl_pObj[0]->pos;	//	barの根本
	vect3&	v1 = pImpl->tbl_pObj[1]->pos;	//	barの先端

	// 入力
	{
		// ポーズ
		if ( keys.SPACE.hi )	pImpl->bPause = !pImpl->bPause;

		// ステップ再生
		if ( keys.ENTER.rep )	bStep = true;

		// リセット
		if ( keys.R.hi )	pImpl->bResetParam = true;

		// 縮む
		if ( mouse.F.hi )	v1 = (v1+v0)/2;

		// 伸びる
		if ( mouse.B.hi )	v1 = (v1-v0)*2+v0;
	}

	vect3	vr = (v1-v0);				//	位置ベクトル
	float	r = vr.abs();				//	位置スカラー
	vect3	moment = cross(-vr,vg);		//	モーメント
	vect3	F = cross(vr/r, moment/r );	//	力

	// 計算
	if ( !pImpl->bPause || bStep )
	{
		pImpl->vel += F;

	 	float	w = pImpl->vel.abs()/r;					//	角速度

pImpl->mov =pImpl->vel;
		pImpl->vel = mrotateByAxis( moment, w ) * pImpl->vel;		//	回転

		v1 += pImpl->vel;
	}

	// 描画
	{


		pers.pen.Line3d( gra, pers, v0, v1, rgb(1,1,1), 2 );

		pers.prim.DrawVect( gra, pers, text_y, v1, vg	,100, rgb(1,0,0), "g" );
		pers.prim.DrawVect( gra, pers, text_y, v0, moment,100, rgb(1,0,1), "moment" );
		pers.prim.DrawVect( gra, pers, text_y, v1, F		,100, rgb(0,1,0), "F" );
		pers.prim.DrawVect( gra, pers, text_y, v1, pImpl->vel	,2	, rgb(1,1,0), "pImpl->vel" );
		pers.prim.DrawVect( gra, pers, text_y, v1, pImpl->mov	,2	, rgb(0,0,1), "pImpl->mov" );
	}


#if 0
	{

		vect3 v = (v1-v0);

		// 角速度に変換
		{
			float b = atan2(v.x,v.y);
			float t = -g * sin(b);			//	接線速度
			float r = t/2/pi/v.abs();		//	角加速度
			pImpl->rsp +=r;						//	角速度
		}

		// 回転
		{
			float x = v.x *cos(pImpl->rsp) - v.y*sin(pImpl->rsp); 
			float y = v.x *sin(pImpl->rsp) + v.y*cos(pImpl->rsp); 
			vect3 a = vect3(x,y,0);
			v1 = v0 + a;

			pers.pen.Line3d( gra, pers, v1, v1+(a-v)*10, rgb(1,0,0),2 );
			
		}

		pers.pen.Line3d( gra, pers, v0, v1, rgb(1,1,1), 2 );

		// 加速ベクトルの実験
		{
			vect3 vg = vect3(0,-g,0);	//	重力加速度ベクトル
			vect3 n0 = cross( v, vg );
			vect3 vt = cross( n0, v );

//						pers.pen.Line3d( gra, pers, v1, v1+vt*100, rgb(0,1,0) );
			vect3 v2 = v*1.05+v0;
//						pers.pen.Line3d( gra, pers, v2, v2+pImpl->vv, rgb(0,1,0) );

//						pers.pen.Line3d( gra, pers, v2, v2+vg*100, rgb(1,0,0) );
			pers.pen.Line3d( gra, pers, v2, v2+vt*100, rgb(0,1,0) );
			pers.pen.Line3d( gra, pers, v2, v2+n0*100, rgb(0,0,1) );					}

	}
#endif
#if 0
	{
		vect3 vg = vect3(0,-g,0);	//	重力加速度ベクトル
		vect3 v = (v2-v0).normalize();
		vect3 n0 = cross( v, vg );
		vect3 vt = cross( n0, v );

		pImpl->vv += vt;
		v2 += vt*10;

//					pers.pen.Line3d( gra, pers,  0,  pImpl->vv*10, rgb(0,1,1), 2 );
		pers.pen.Line3d( gra, pers,  vect3(1,0,0),  vect3(1,0,0)+vt*100, rgb(0,1,0), 1 );

		pers.pen.Line3d( gra, pers, v2, v2+vt*100, rgb(0,1,0) );
		pers.pen.Line3d( gra, pers, v0, v2, rgb(0,1,1), 2 );

	}
#endif

	
	// 1/60sec 待
	gra.Wait();
}
