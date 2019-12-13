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
#include "lab3.h"

//=================================
// 描画	引力実験
//=================================

struct Lab3::Impl
{
	bool	bPause = false; 
};

Lab3::Lab3() : pImpl( new Lab3::Impl ){}

//------------------------------------------------------------------------------
void Lab3::Update( SysKeys& keys, SysMouse& mouse, SysSound& sound, SysGra& gra, Pers& pers, float delta, int& text_y, Cp& cp )
//------------------------------------------------------------------------------
{
	const float	G	= -9.80665;				// 重力加速度

	//画面クリア
	gra.Clr(rgb(0.3,0.3,0.3));
	pers.grid.DrawGrid3d( gra, pers, vect3(0,0,0), midentity(), 10, 10, 1, rgb(0.2,0.2,0.2) );
	gra.Print(1,(float)text_y++,string("<<lab3_gravityPlanet>>")); 

	bool	bStep = false; 

	#define MAX_PREV 300
	struct Planet:Obj
	{
		 vect3	spd;
		 vect3	tblPrev[MAX_PREV];
		 int	cntPrev=0;;

		Planet( vect3 v, vect3 _spd ) : Obj(v)
		{
			pos = v;
			spd = _spd;
		}
	};

	// 初期化
	if ( !m.bInitParam )
	{
		m.bInitParam = true;
		if ( !m.bInitAll )
		{
			m.bInitAll = true;
			//上から
			pers.cam.pos = vect3(  0.0, 0.5, -2.0 );
			pers.cam.at = vect3( 0,  0.0, 0 );
		}

		m.tbl_pObj.clear();
		m.tbl_pObj.emplace_back( new Planet(vect3(-0.5,0.1,0),vect3(0, 0, -0.02)) );
		m.tbl_pObj.emplace_back( new Planet(vect3( 0.5,0.1,0),vect3(0, 0, 0.02)) );

		//GUI登録
		cp.tbltbl_pObj.clear();
		cp.tbltbl_pEdge.clear();
		cp.tbltbl_pObj.emplace_back( m.tbl_pObj );
	}

	const float	g = G *delta*delta;		// 重力加速度/frame

	// 入力
	{
		// ポーズ
		if ( keys.SPACE.hi )	pImpl->bPause = !pImpl->bPause;

		// ステップ再生
		if ( keys.ENTER.rep )	bStep = true;

		// リセット
		if ( keys.R.hi )	m.bInitParam = false ;

		// 生成
		if ( mouse.B.hi )
		{
			vect3 P = pers.calcScreenToWorld3( vect3(mouse.pos,0) );
			vect3 I = pers.calcRayvect( P );
			m.tbl_pObj.emplace_back( new Planet( P, I/100.0 ) );
			cp.tbltbl_pObj.clear();
			cp.tbltbl_pObj.emplace_back( m.tbl_pObj );
		}

	}

	
	Planet& pl0 = *dynamic_cast<Planet*>(m.tbl_pObj[0].get());	//	太陽
	Planet& pl1 = *dynamic_cast<Planet*>(m.tbl_pObj[1].get());	//	地球

	// 計算
	if ( !pImpl->bPause || bStep )
	{
		function<void(Planet&, Planet&)> func = [&]( Planet& pl0, Planet& pl1 )
		{
			float 	dis = (pl0.pos-pl1.pos).abs();				// 距離
			vect3 	dir = (pl0.pos-pl1.pos).normalize();		// 方向

			if ( dis < 0.01 ) dis = 0.01;
			
			pl1.spd = (pl1.spd + dir/(dis*dis)/1000);

			pl1.pos += pl1.spd; 
		
			pl1.tblPrev[ pl1.cntPrev++ ] = pl1.pos;	
			if ( pl1.cntPrev >= MAX_PREV ) pl1.cntPrev = 0;

		};

		func( pl0, pl1 );
		func( pl1, pl0 );
	}

	// 描画
	{
		for ( shared_ptr<Obj> p : m.tbl_pObj ) 
		{
			Planet& pl = *dynamic_cast<Planet*>(p.get());
			for ( int i = 0 ; i < MAX_PREV ; i++ )
			{
				pers.pen.pset3d( gra, pers, pl.tblPrev[i], rgb(0,1,1),2 );
			}
		}
	}

}
