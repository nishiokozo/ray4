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
#include "lab7.h"
struct Lab7::Impl
{
	bool	bResetAll = true;
	bool	bResetParam = true;
	bool	bPause = false;
	bool	bStep = false;

	vector<shared_ptr<Obj>>	tbl_pObj;
	vector<shared_ptr<Edge>>	tbl_pEdge;

	 bool		bShot = false;
	 vect3	velocity;
	 vect3	moment;
	 vect3	to;
	 vect3	add;
	
	 float 	w;	//	角速度

	vect3	acc2;

};
Lab7::Lab7() : pImpl( new Lab7::Impl ){}
//------------------------------------------------------------------------------
void Lab7::Update( SysKeys& keys, SysMouse& mouse, SysSound& sound, SysGra& gra, Pers& pers, float delta, int& text_y, Cp& cp )
//------------------------------------------------------------------------------
{
	const float	G	= -9.80665;				// 重力加速度
	
	//画面クリア
	gra.Clr(rgb(0.3,0.3,0.3));
	pers.grid.DrawGrid3d( gra, pers, vect3(0,0,0), mat33::midentity(), 10, 10, 1, rgb(0.2,0.2,0.2) );

	gra.Print(1,(float)text_y++,string("<<lab7_kakusokudo7>>")); 

	const float	g = G *delta*delta;		// 重力加速度/frame
	const vect3	vg = vect3(0,0, -g);		// 重力加速度/frame

	bool bStep = false;

	// 初期化
	if ( pImpl->bResetParam )
	{
		if ( pImpl->bResetAll )
		{
			pImpl->bResetAll = false;
			pers.cam.pos = vect3( 0, 5.0, -0.5 );
			pers.cam.at = vect3( 0,  0, 0 );
			pers.cam.Update();
		}
		pImpl->bResetParam = false;

		// 点
		pImpl->tbl_pObj.clear();
		pImpl->tbl_pObj.emplace_back( new Obj(vect3( 0 ,0.1, 0)) );
		pImpl->tbl_pObj.emplace_back( new Obj(vect3(-1 ,0.1, 0)) );

		// 線
		pImpl->tbl_pEdge.clear();
		pImpl->tbl_pEdge.emplace_back( new Edge(0,1) );

		//GUI登録
		cp.tbltbl_pObj.clear();
		cp.tbltbl_pEdge.clear();
		cp.tbltbl_pObj.emplace_back( pImpl->tbl_pObj );
		cp.tbltbl_pEdge.emplace_back( pImpl->tbl_pEdge );

		pImpl->bShot = false;
	}


	vect3&	v0 = pImpl->tbl_pObj[0]->pos;	//	barの根本
	vect3&	v1 = pImpl->tbl_pObj[1]->pos;	//	barの先端


	//入力
	{
		// リセット
		if ( keys.R.hi )	pImpl->bResetParam = true;

		if ( keys.SPACE.hi )	pImpl->bPause = !pImpl->bPause ;

		if ( keys.ENTER.hi )	{bStep = true; pImpl->bPause = true; }
		if ( mouse.B.hi )		pImpl->bShot = !pImpl->bShot ;
	}

	// 計算
	{
		if ( keys.R.hi ) pImpl->acc2 = vect3(0,0,0);



		if(0)
		{
			pImpl->acc2+=vg;

			v1+= pImpl->acc2;
			
			vect3	bar = (v1-v0);							//	棒
			float	radius = bar.abs();

			vect3 prev = v1;
			v1 = v0+bar/radius;
			pImpl->acc2 += v1-prev;
		}
		else
		{

			vect3	bar = (v1-v0);							//	棒

			vect3 moment = cross(-bar,vg);				//	回転モーメント

			vect3 vt = cross(bar, moment );	//	タンジェント

			if ( !pImpl->bPause || bStep )
			{

				pImpl->acc2+=vt;

				moment = cross(-bar,pImpl->acc2);				//	回転モーメント


				pImpl->acc2 = mrotateByAxis( moment, pImpl->acc2.abs() )*  pImpl->acc2;

				v1+=pImpl->acc2;
				vect3 prev = v1;
				v1 = (v1-v0)/(v1-v0).abs()+v0;
				pImpl->acc2 += v1-prev;
			}
			pers.prim.DrawVect( gra, pers, text_y, v1, vt			,100	, rgb(1,0,0), "vt" );
			gra.Print(1,(float)text_y++,string("radius ")+to_string(bar.abs())); 
		}

				

	}
if (0)
		
	// 描画
	{
		if ( pImpl->bShot )
		{
			vect3	bar = (v1-v0);							//	棒
		//	float	radius = bar.abs();
pImpl->w=deg2rad(2);
			pImpl->to = mrotateByAxis( pImpl->moment, pImpl->w ) * bar;			//	移動計算

			vect3 add  = pImpl->to - bar;

			v1 += add;
			
			
		}
		else
		{
			vect3	bar = (v1-v0);							//	棒
			float	radius = bar.abs();
			pImpl->moment = cross(-bar,vg);						//	回転モーメント
			pImpl->velocity = cross(bar/radius, pImpl->moment/radius );	//	ベロシティ

			pImpl->w = pImpl->velocity.abs()/radius;
			pImpl->to = mrotateByAxis( pImpl->moment, pImpl->w ) * bar;			//	移動計算

//			vect3 pImpl->add  = pImpl->to - bar;

		}

		// 補助線
		pers.prim.DrawVect( gra, pers, text_y, v1, vg			,100	, rgb(1,0,0), "vg" );
			
	}

}
