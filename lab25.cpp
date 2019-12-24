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

	vector<int>	idxPoint;

	struct Puv : Obj
	{
		vect3	va;
		vect3	vb;
		vect3	u;
		Puv( vect3 _pos, vect3 _a, vect3 _b, vect3 _u ) :Obj(_pos) { pos=_pos; va=_a; vb=_b;u=_u;}
	};

	vector<ivect4> tblSurface;
	
};

Lab25::Lab25() : pImpl( new Lab25::Impl )
{
}


//------------------------------------------------------------------------------
void Lab25::Update( SysKeys& keys, SysMouse& mouse, SysSound& sound, SysGra& gra, Pers& pers, float delta, int& text_y, Cp& cp )
//------------------------------------------------------------------------------
{
	// 画面クリア
	gra.Clr(rgb(0.3,0.3,0.3));
	pers.grid.DrawGrid3d( gra, pers, vect3(0,0,0), midentity(), 10, 10, 1, rgb(0.2,0.2,0.2) );
	gra.Print(1,(float)text_y++,"25 : Nurbus cource"); 

	//初期化
	if ( pImpl->bResetAll )
	{
		pImpl->bResetAll = false;

		// カメラ
		pers.cam.pos = vect3(-5.0, 8.0, -10.0 );
		pers.cam.at = vect3(0,  0.0, 0 );

		// 点
		{
			float f = 4.0;
			// 内形
/* 0*/		pImpl->tbl_pObj.emplace_back( new Impl::Puv(vect3(-(f+1), 0.1, 0.0 ),vect3( 0.0, 0.0, 0.5 ),vect3( 0.0, 0.0,-0.5 ), vect3(-0.4, 0.0, 0.0)) );
/* 1*/		pImpl->tbl_pObj.emplace_back( new Impl::Puv(vect3(  -(f), 0.1,-1.0 ),vect3(-0.5, 0.0, 0.0 ),vect3( 0.5, 0.0, 0.0 ), vect3( 0.0, 0.0,-0.4)) );
/* 2*/		pImpl->tbl_pObj.emplace_back( new Impl::Puv(vect3(   (f), 0.1,-1.0 ),vect3(-0.5, 0.0, 0.0 ),vect3( 0.5, 0.0, 0.0 ), vect3( 0.0, 0.0,-0.5)) );
/* 3*/		pImpl->tbl_pObj.emplace_back( new Impl::Puv(vect3( (f+1), 0.1, 0.0 ),vect3( 0.0, 0.0,-0.5 ),vect3( 0.0, 0.0, 0.5 ), vect3( 0.5, 0.0, 0.0)) );
/* 4*/		pImpl->tbl_pObj.emplace_back( new Impl::Puv(vect3(   (f), 0.1, 1.0 ),vect3( 0.5, 0.0, 0.0 ),vect3(-0.5, 0.0, 0.0 ), vect3( 0.0, 0.0, 0.5)) );
/* 5*/		pImpl->tbl_pObj.emplace_back( new Impl::Puv(vect3(  -(f), 0.1, 1.0 ),vect3( 0.5, 0.0, 0.0 ),vect3(-0.5, 0.0, 0.0 ), vect3( 0.0, 0.0, 0.4)) );
			// 外形
/* 6*/		pImpl->tbl_pObj.emplace_back( new Impl::Puv(vect3(-(f+2), 0.1, 0.0 ),vect3( 0.0, 0.0, 1.0 ),vect3( 0.0, 0.0,-1.0 ), vect3( 0.4, 0.0, 0.0)) );
/* 7*/		pImpl->tbl_pObj.emplace_back( new Impl::Puv(vect3(  -(f), 0.1,-2.0 ),vect3(-1.0, 0.0, 0.0 ),vect3( 1.0, 0.0, 0.0 ), vect3( 0.0, 0.0, 0.4)) );
/* 8*/		pImpl->tbl_pObj.emplace_back( new Impl::Puv(vect3(   (f), 0.5,-2.0 ),vect3(-1.0, 0.0, 0.0 ),vect3( 1.0, 0.0, 0.0 ), vect3( 0.0,-0.3, 0.0)) );
/* 9*/		pImpl->tbl_pObj.emplace_back( new Impl::Puv(vect3( (f+2), 0.6, 0.0 ),vect3( 0.0, 0.0,-1.0 ),vect3( 0.0, 0.0, 1.0 ), vect3( 0.0,-0.5, 0.0)) );
/*10*/		pImpl->tbl_pObj.emplace_back( new Impl::Puv(vect3(   (f), 0.5, 2.0 ),vect3( 1.0, 0.0, 0.0 ),vect3(-1.0, 0.0, 0.0 ), vect3( 0.0,-0.3, 0.0)) );
/*11*/		pImpl->tbl_pObj.emplace_back( new Impl::Puv(vect3(  -(f), 0.1, 2.0 ),vect3( 1.0, 0.0, 0.0 ),vect3(-1.0, 0.0, 0.0 ), vect3( 0.0, 0.0,-0.4)) );
		}

		// インデックス
		{
			// 内形
			pImpl->idxPoint.emplace_back( 0 );
			pImpl->idxPoint.emplace_back( 1 );
			pImpl->idxPoint.emplace_back( 2 );
			pImpl->idxPoint.emplace_back( 3 );
			pImpl->idxPoint.emplace_back( 4 );
			pImpl->idxPoint.emplace_back( 5 );
			pImpl->idxPoint.emplace_back( 0 );

			// セパレータ
			pImpl->idxPoint.emplace_back( -1 );

			// 外形
			pImpl->idxPoint.emplace_back( 6+0 );
			pImpl->idxPoint.emplace_back( 6+1 );
			pImpl->idxPoint.emplace_back( 6+2 );
			pImpl->idxPoint.emplace_back( 6+3 );
			pImpl->idxPoint.emplace_back( 6+4 );
			pImpl->idxPoint.emplace_back( 6+5 );
			pImpl->idxPoint.emplace_back( 6+0 );
		}

		// サーフェイス
		pImpl->tblSurface.emplace_back(  2, 8, 1, 7 );
		pImpl->tblSurface.emplace_back(  3, 9, 2, 8 );
		pImpl->tblSurface.emplace_back(  4,10, 3, 9 );
		pImpl->tblSurface.emplace_back(  5,11, 4,10 );
		pImpl->tblSurface.emplace_back(  0, 6, 5,11 );
		pImpl->tblSurface.emplace_back(  1, 7, 0, 6 );

		//GUI登録
		cp.tbltbl_pObj.clear();
		cp.tbltbl_pEdge.clear();
		cp.tbltbl_pObj.emplace_back( pImpl->tbl_pObj );



	}
	
/*
	for ( shared_ptr<Obj> po : pImpl->tbl_pObj )
	{
		Impl::Puv* p = dynamic_cast<Impl::Puv*>(po.get());
		pers.pen.pset3d( gra, pers, p->pos+p->u, rgb(0,1,1), 2);
	}
*/

	auto funcDrawSurface = [](
		SysGra& gra, Pers& pers,
		vect3 p0 , vect3 p1 , vect3 p2 , vect3 p3,
		vect3 p0u, vect3 p1u, vect3 p2u, vect3 p3u,
		vect3 p0v, vect3 p1v, vect3 p2v, vect3 p3v
	)
	{
		// 0--1
		// |  |
		// 2--3
	
		int udiv = pow(2,5.0);	// floatで計算誤差が出ないよう2^nにしておく
		int vdiv = pow(2,5.0);	// floatで計算誤差が出ないよう2^nにしておく

		// U
		for ( float ut = 0 ; ut <= 1.0 ; ut += 1.0/udiv )
		{
			vect3 vP0 = func_bezier3( ut, p0, p0+p0u, p1+p1u, p1 );
			vect3 vP3 = func_bezier3( ut, p2, p2+p2u, p3+p3u, p3 );
			vect3 vP1 = vP0 + (p0v)*(1.0-ut) + (p1v)*ut;
			vect3 vP2 = vP3 + (p2v)*(1.0-ut) + (p3v)*ut;

			// V
			for ( float vt = 0 ; vt <= 1.0 ; vt +=1.0/vdiv )
			{
				vect3 vpos = func_bezier3( vt, vP0, vP1, vP2, vP3 );
				pers.pen.pset3d( gra, pers, vpos, rgb(1,1,1), 1 );
			}
//				pers.pen.pset3d( gra, pers, vP0, rgb(1,1,0), 3 );
		}
	};

#if 0
	{
		//ベジェ計算＆描画
		int div = 16;
		float dt = 1/(float)div;

		int size = static_cast<signed>(pImpl->idxPoint.size());

		for ( int n = 0 ; n < size-1 ; n++ )
		{
			int n0 = pImpl->idxPoint[n];
			int n1 = pImpl->idxPoint[n+1];

			if ( n0 == -1 ) continue;	// -1はセパレータ
			if ( n1 == -1 ) continue;

			Impl::Puv* p0 = dynamic_cast<Impl::Puv*>(pImpl->tbl_pObj[n0].get());
			Impl::Puv* p1 = dynamic_cast<Impl::Puv*>(pImpl->tbl_pObj[n1].get());

			vect3 P0 =     p0->pos;
			vect3 P1 = P0 +p0->vb;
			vect3 P3 =     p1->pos;
			vect3 P2 = P3 +p1->va;

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
#else
	for ( ivect4 o : pImpl->tblSurface )
	{
		Impl::Puv* p0 = dynamic_cast<Impl::Puv*>(pImpl->tbl_pObj[o.n0].get());
		Impl::Puv* p1 = dynamic_cast<Impl::Puv*>(pImpl->tbl_pObj[o.n1].get());
		Impl::Puv* p2 = dynamic_cast<Impl::Puv*>(pImpl->tbl_pObj[o.n2].get());
		Impl::Puv* p3 = dynamic_cast<Impl::Puv*>(pImpl->tbl_pObj[o.n3].get());

			

		funcDrawSurface( gra, pers, 
			p0->pos,	p1->pos,	p2->pos,	p3->pos,
			p0->u,		p1->u,		p2->u,		p3->u,
			p0->va,		p1->va,		p2->vb,		p3->vb
		);

	}

	
#endif



	
}
