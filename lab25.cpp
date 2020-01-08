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

using namespace std;


struct Lab25::Impl
{
	bool	bResetAll = true;
	bool	bResetParam = true;
	bool	bPause = false;
	bool	bStep = false;

	vector<shared_ptr<Obj>>	tbl_pObj;

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
	pers.grid.DrawGrid3d( gra, pers, vect3(0,0,0), mat33::midentity(), 16, 16, 1, rgb(0.2,0.2,0.2) );
	gra.Print(1,(float)text_y++,"25 : Bezier Surface cource"); 

	//初期化
	if ( pImpl->bResetAll )
	{
		pImpl->bResetAll = false;

		// カメラ
		pers.cam.pos = vect3(-5.0, 8.0, -10.0 );
		pers.cam.at = vect3(0,  0.0, 0 );
		pers.cam.Update();

		// 頂点に接線データをもたせている。扱いやすくシンプル。ただし４接線(±UV)までに制限される。
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

		// サーフェイス 頂点番号を入れる
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
	
	// 接線表示 UVの接線を区別がある
	for ( shared_ptr<Obj>po : pImpl->tbl_pObj )
	{
		Impl::Puv* p = dynamic_cast<Impl::Puv*>(po.get());

		pers.pen.line3d( gra, pers, p->pos, p->pos+p->va, rgb(0,1,0) );
		pers.pen.pset3d( gra, pers, p->pos+p->va, rgb(0,1,0), 5 );
		pers.pen.line3d( gra, pers, p->pos, p->pos+p->vb, rgb(0,1,0) );
		pers.pen.pset3d( gra, pers, p->pos+p->vb, rgb(0,1,0), 5 );

		pers.pen.line3d( gra, pers, p->pos, p->pos+p->u, rgb(1,1,0) );
		pers.pen.pset3d( gra, pers, p->pos+p->u, rgb(1,1,0), 5 );

	}
	
	for ( ivect4 o : pImpl->tblSurface )
	{
		Impl::Puv* p0 = dynamic_cast<Impl::Puv*>(pImpl->tbl_pObj[o.n0].get());
		Impl::Puv* p1 = dynamic_cast<Impl::Puv*>(pImpl->tbl_pObj[o.n1].get());
		Impl::Puv* p2 = dynamic_cast<Impl::Puv*>(pImpl->tbl_pObj[o.n2].get());
		Impl::Puv* p3 = dynamic_cast<Impl::Puv*>(pImpl->tbl_pObj[o.n3].get());

		// サーフェス描画
		pers.pen.DrawBezierSurface( gra, pers, 
			p0->pos,	p1->pos,	p2->pos,	p3->pos,
			p0->u,		p1->u,		p2->u,		p3->u,
			p0->va,		p1->va,		p2->vb,		p3->vb
		);

		// エッジ描画
		{
			// V
			pers.pen.DrawBezier( gra, pers, p0->pos, p0->pos +p0->va, p2->pos +p2->vb, p2->pos );
			pers.pen.DrawBezier( gra, pers, p1->pos, p1->pos +p1->va, p3->pos +p3->vb, p3->pos );

			// U
			pers.pen.DrawBezier( gra, pers, p0->pos, p0->pos +p0->u, p1->pos +p1->u, p1->pos ); // どちらも同じ
	//		pers.pen.DrawBezier( gra, pers, p2->pos, p2->pos +p2->u, p3->pos +p3->u, p3->pos ); // どちらも同じ
		}
	

	}

}
