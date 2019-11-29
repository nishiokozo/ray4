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
#include "lab7.h"
//------------------------------------------------------------------------------
void Lab7::Update( SysKeys& keys, SysMouse& mouse, SysGra& gra, Pers& pers, float delta, int& text_y, Cp& cp )
//------------------------------------------------------------------------------
{
	const float	G	= -9.80665;				// 重力加速度
	const rgb col0 = rgb( 0, 0, 0 );
	const rgb col1 = rgb( 0, 0, 1 );
	const rgb col2 = rgb( 1, 0, 0 );
	const rgb col3 = rgb( 1, 0, 1 );
	const rgb col4 = rgb( 0, 1, 0 );
	const rgb col5 = rgb( 0, 1, 1 );
	const rgb col6 = rgb( 1, 1, 0 );
	const rgb col7 = rgb( 1, 1, 1 );
	
	//画面クリア
	gra.Clr(rgb(0.3,0.3,0.3));
	pers.grid.DrawGrid3d( gra, pers, vect3(0,0,0), midentity(), 10, 10, 1, rgb(0.2,0.2,0.2) );

	gra.Print(1,(float)text_y++,string("<<lab7_kakusokudo7>>")); 

	const float	g = G *delta*delta;		// 重力加速度/frame
	const vect3	vg = vect3(0,0, -g);		// 重力加速度/frame

	static bool		bShot = false;
	static vect3	velocity;
	static vect3	moment;
	static vect3	to;
	static vect3	add;
	
	static float 	w;	//	角速度
	static bool		bPause = false;
	bool bStep = false;

	// 初期化
	if ( !m.bInitParam )
	{
		if ( !m.bInitAll )
		{
			m.bInitAll = true;
			pers.cam.pos = vect3( 0, 5.0, -0.5 );
			pers.cam.at = vect3( 0,  0, 0 );
		}
		m.bInitParam = true;

		// 点
		for ( Obj* p : m.tbl_pObj ) delete p;
		m.tbl_pObj.clear();
		m.tbl_pObj.emplace_back( new Obj(vect3( 0 ,0.1, 0)) );
		m.tbl_pObj.emplace_back( new Obj(vect3(-1 ,0.1, 0)) );

		// 線
		for ( Edge* p : m.tbl_pEdge ) delete p;
		m.tbl_pEdge.clear();
		m.tbl_pEdge.emplace_back( new Edge(0,1) );

		//GUI登録
		cp.tbltbl_pObj.clear();
		cp.tbltbl_pEdge.clear();
		cp.tbltbl_pObj.emplace_back( m.tbl_pObj );
		cp.tbltbl_pEdge.emplace_back( m.tbl_pEdge );

		bShot = false;
	}


	vect3&	v0 = m.tbl_pObj[0]->pos;	//	barの根本
	vect3&	v1 = m.tbl_pObj[1]->pos;	//	barの先端


	//入力
	{
		// リセット
		if ( keys.R.hi )		m.bInitParam = false ;

		if ( keys.SPACE.hi )	bPause = !bPause ;

		if ( keys.ENTER.hi )	{bStep = true; bPause = true; }
		if ( mouse.B.hi )		bShot = !bShot ;
	}

	// 計算
	{
		static	vect3	acc2;
		if ( keys.R.hi ) acc2 = vect3(0,0,0);



		if(0)
		{
			acc2+=vg;

			v1+= acc2;
			
			vect3	bar = (v1-v0);							//	棒
			float	radius = bar.abs();

			vect3 prev = v1;
			v1 = v0+bar/radius;
			acc2 += v1-prev;
		}
		else
		{

			vect3	bar = (v1-v0);							//	棒

			vect3 moment = cross(-bar,vg);				//	回転モーメント

			vect3 vt = cross(bar, moment );	//	タンジェント

			if ( !bPause || bStep )
			{

				acc2+=vt;

				moment = cross(-bar,acc2);				//	回転モーメント


				acc2 = mrotateByAxis( moment, acc2.abs() )*  acc2;

				v1+=acc2;
				vect3 prev = v1;
				v1 = (v1-v0)/(v1-v0).abs()+v0;
				acc2 += v1-prev;
			}
			m.drawVect( gra, pers, text_y, v1, vt			,100	, rgb(1,0,0), "vt" );
			gra.Print(1,(float)text_y++,string("radius ")+to_string(bar.abs())); 
		}

				

	}
if (0)
		
	// 描画
	{
		if ( bShot )
		{
			vect3	bar = (v1-v0);							//	棒
			float	radius = bar.abs();
w=deg2rad(2);
			to = mrotateByAxis( moment, w ) * bar;			//	移動計算

			vect3 add  = to - bar;

			v1 += add;
			
			
		}
		else
		{
			vect3	bar = (v1-v0);							//	棒
			float	radius = bar.abs();
			moment = cross(-bar,vg);						//	回転モーメント
			velocity = cross(bar/radius, moment/radius );	//	ベロシティ

			w = velocity.abs()/radius;
			to = mrotateByAxis( moment, w ) * bar;			//	移動計算

			vect3 add  = to - bar;

		}

		// 補助線
		m.drawVect( gra, pers, text_y, v1, vg			,100	, rgb(1,0,0), "vg" );
			
	}

}
