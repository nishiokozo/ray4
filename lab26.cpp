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
#include "lab26.h"



struct Lab26::Impl
{
	void funcDrawCurve( SysGra& gra, Pers& pers, vect3 P0, vect3 P1, vect3 P2, vect3 P3 )
	{
		int div = 16;
		float dt = 1/(float)div;

		float t  = dt;
		vect3 pos0 = P0;
		for ( int i = 0 ; i < div ; i++ )
		{
			vect3 pos1 = func_bezier3( t, P0, P1, P2, P3 );
			pers.pen.line3d( gra, pers, pos0, pos1, rgb(1,1,1) );

			pos0=pos1;
			t+=dt;
		}
	}

	void funcDrawSurface(
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
		}
	}

	// 

	bool	bResetAll = true;
	bool	bResetParam = true;
	bool	bPause = false;
	bool	bStep = false;

	vector<shared_ptr<Obj>>	tbl_pObj;

	struct Curve
	{
		int		n0;	// Obj番号
		int		n1;
		vect3	t0;	// 接線ベクトル
		vect3	t1;
		Curve( int _n0, int _n1, vect3 _t0, vect3 _t1 ) : n0(_n0), n1(_n1), t0(_t0), t1(_t1) {}
	};

	struct Surface
	{
		int		u0;	// Curve番号
		int		u1;
		int		v0;
		int		v1;
		Surface( int _u0, int _u1, int _v0, int _v1 ) : u0(_u0), u1(_u1), v0(_v0), v1(_v1) {}
	};

	vector<Curve>	tblCurve;

	vector<Surface>	tblSurface;

};

Lab26::Lab26() : pImpl( new Lab26::Impl ){}


//------------------------------------------------------------------------------
void Lab26::Update( SysKeys& keys, SysMouse& mouse, SysSound& sound, SysGra& gra, Pers& pers, float delta, int& text_y, Cp& cp )
//------------------------------------------------------------------------------
{
	// 画面クリア
	gra.Clr(rgb(0.3,0.3,0.3));
	pers.grid.DrawGrid3d( gra, pers, vect3(0,0,0), midentity(), 10, 10, 1, rgb(0.2,0.2,0.2) );
	gra.Print(1,(float)text_y++,"26 : Nurbus cource Edit"); 

	//初期化
	if ( pImpl->bResetAll )
	{
		pImpl->bResetAll = false;

		// カメラ
		pers.cam.pos = vect3(-9.0, 3.0, 3.0 );
		pers.cam.at = vect3(0,  0.0, 0 );

		// カーブ線に接線データをもたせている。より自由な（複雑な）造形が可能
		// 点
		{
			float f = 4.0;
			// 内形
/* 0*/		pImpl->tbl_pObj.emplace_back( new Obj( vect3(-(f+1), 0.1, 0.0 ) ));
/* 1*/		pImpl->tbl_pObj.emplace_back( new Obj( vect3(  -(f), 0.1,-1.0 ) ));
/* 2*/		pImpl->tbl_pObj.emplace_back( new Obj( vect3(   (f), 0.1,-1.0 ) ));
/* 3*/		pImpl->tbl_pObj.emplace_back( new Obj( vect3( (f+1), 0.1, 0.0 ) ));
/* 4*/		pImpl->tbl_pObj.emplace_back( new Obj( vect3(   (f), 0.1, 1.0 ) ));
/* 5*/		pImpl->tbl_pObj.emplace_back( new Obj( vect3(  -(f), 0.1, 1.0 ) ));
			// 外形
/* 6*/		pImpl->tbl_pObj.emplace_back( new Obj( vect3(-(f+2), 0.1, 0.0 ) ));
/* 7*/		pImpl->tbl_pObj.emplace_back( new Obj( vect3(  -(f), 0.1,-2.0 ) ));
/* 8*/		pImpl->tbl_pObj.emplace_back( new Obj( vect3(   (f), 0.5,-2.0 ) ));
/* 9*/		pImpl->tbl_pObj.emplace_back( new Obj( vect3( (f+2), 0.6, 0.0 ) ));
/*10*/		pImpl->tbl_pObj.emplace_back( new Obj( vect3(   (f), 0.5, 2.0 ) ));
/*11*/		pImpl->tbl_pObj.emplace_back( new Obj( vect3(  -(f), 0.1, 2.0 ) ));
		}

		// カーブ
		{
			// 内形
/* 0*/		pImpl->tblCurve.emplace_back( 0, 1, vect3( 0.0, 0.0,-0.5), vect3(-0.5, 0.0, 0.0)  );
/* 1*/		pImpl->tblCurve.emplace_back( 1, 2, vect3( 0.5, 0.0, 0.0), vect3(-0.5, 0.0, 0.0)  );
/* 2*/		pImpl->tblCurve.emplace_back( 2, 3, vect3( 0.5, 0.0, 0.0), vect3( 0.0, 0.0,-0.5)  );
/* 3*/		pImpl->tblCurve.emplace_back( 3, 4, vect3( 0.0, 0.0, 0.5), vect3( 0.5, 0.0, 0.0)  );
/* 4*/		pImpl->tblCurve.emplace_back( 4, 5, vect3(-0.5, 0.0, 0.0), vect3( 0.5, 0.0, 0.0)  );
/* 5*/		pImpl->tblCurve.emplace_back( 5, 0, vect3(-0.5, 0.0, 0.0), vect3( 0.0, 0.0, 0.5)  );
			// 外形
/* 6*/		pImpl->tblCurve.emplace_back( 6, 7, vect3( 0.0, 0.0,-1.0), vect3(-1.0, 0.0, 0.0)  );
/* 7*/		pImpl->tblCurve.emplace_back( 7, 8, vect3( 1.0, 0.0, 0.0), vect3(-1.0, 0.0, 0.0)  );
/* 8*/		pImpl->tblCurve.emplace_back( 8, 9, vect3( 1.0, 0.0, 0.0), vect3( 0.0, 0.0,-1.0)  );
/* 9*/		pImpl->tblCurve.emplace_back( 9,10, vect3( 0.0, 0.0, 1.0), vect3( 1.0, 0.0, 0.0)  );
/*10*/		pImpl->tblCurve.emplace_back(10,11, vect3(-1.0, 0.0, 0.0), vect3( 1.0, 0.0, 0.0)  );
/*11*/		pImpl->tblCurve.emplace_back(11, 6, vect3(-1.0, 0.0, 0.0), vect3( 0.0, 0.0, 1.0)  );
			// バンク
/*12*/		pImpl->tblCurve.emplace_back( 0, 6, vect3(-0.5, 0.0, 0.0), vect3( 0.5, 0.0, 0.0)  );
/*13*/		pImpl->tblCurve.emplace_back( 1, 7, vect3( 0.0, 0.0,-0.5), vect3( 0.0, 0.0, 0.5)  );
/*14*/		pImpl->tblCurve.emplace_back( 2, 8, vect3( 0.0, 0.0,-0.4), vect3( 0.0,-0.3, 0.0)  );
/*15*/		pImpl->tblCurve.emplace_back( 3, 9, vect3( 0.4, 0.0, 0.0), vect3( 0.0,-0.4, 0.0)  );
/*16*/		pImpl->tblCurve.emplace_back( 4,10, vect3( 0.0, 0.0, 0.4), vect3( 0.0,-0.3, 0.0)  );
/*17*/		pImpl->tblCurve.emplace_back( 5,11, vect3( 0.0, 0.0, 0.5), vect3( 0.0, 0.0,-0.5)  );

		}

		// サーフェース
		{
			pImpl->tblSurface.emplace_back( 12,13, 0, 6 );
			pImpl->tblSurface.emplace_back( 13,14, 1, 7 );
			pImpl->tblSurface.emplace_back( 14,15, 2, 8 );
			pImpl->tblSurface.emplace_back( 15,16, 3, 9 );
			pImpl->tblSurface.emplace_back( 16,17, 4,10 );
			pImpl->tblSurface.emplace_back( 17,12, 5,11 );
		}

		//GUI登録
		cp.tbltbl_pObj.clear();
		cp.tbltbl_pEdge.clear();
		cp.tbltbl_pObj.emplace_back( pImpl->tbl_pObj );

	}
	

	// 接線表示
	for ( Impl::Curve& cv : pImpl->tblCurve )
	{
		Obj* p0 = pImpl->tbl_pObj[cv.n0].get();
		Obj* p1 = pImpl->tbl_pObj[cv.n1].get();
		
		pers.pen.line3d( gra, pers, p0->pos, p0->pos+cv.t0, rgb(0,1,0) );
		pers.pen.pset3d( gra, pers, p0->pos+cv.t0, rgb(0,1,0), 5 );

		pers.pen.line3d( gra, pers, p1->pos, p1->pos+cv.t1, rgb(0,1,0) );
		pers.pen.pset3d( gra, pers, p1->pos+cv.t1, rgb(0,1,0), 5 );

	}

	// サーフェス表示
	for ( Impl::Surface& suf : pImpl->tblSurface )
	{

		Impl::Curve& u0 = pImpl->tblCurve[suf.u0];
		Impl::Curve& u1 = pImpl->tblCurve[suf.u1];
		Impl::Curve& v0 = pImpl->tblCurve[suf.v0];
		Impl::Curve& v1 = pImpl->tblCurve[suf.v1];

		Obj* p0 = pImpl->tbl_pObj[u0.n0].get();
		Obj* p1 = pImpl->tbl_pObj[u0.n1].get();
		Obj* p2 = pImpl->tbl_pObj[u1.n0].get();
		Obj* p3 = pImpl->tbl_pObj[u1.n1].get();
			
		// サーフェス描画	
		pImpl->funcDrawSurface( gra, pers, 
			p0->pos,	p1->pos,	p2->pos,	p3->pos,
			u0.t0,		u0.t1,		u1.t0,		u1.t1,
			v0.t0,		v1.t0,		v0.t1,		v1.t1
		);

		// エッジ描画
		pImpl->funcDrawCurve( gra, pers, p0->pos, p0->pos +v0.t0, p2->pos +v0.t1, p2->pos );
		pImpl->funcDrawCurve( gra, pers, p1->pos, p1->pos +v1.t0, p3->pos +v1.t1, p3->pos );

	}






	
}
