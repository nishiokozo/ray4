//2019/11/22

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

struct Cutmull14
{
	bool	bActive = false;	//	使用可能

	int idxTbl = 0;

	~Cutmull14()
	{
		for ( Obj* p : tblPoint ) delete p;
	}
	
	
	vector<Obj*>	tblPoint =
	{
		new Obj( vect3(	-0.5,	0.12,	-0.5) ),
		new Obj( vect3(	+0.5,	0.12,	-0.5) ),
		new Obj( vect3(	+0.9,	0.12,	 0.5) ),
		new Obj( vect3(	 0.0,	0.12,	 0.5) ),
		new Obj( vect3(	-0.9,	0.12,	 0.5) ),

		new Obj( vect3(	-0.5,	0.12,	-0.5)*1.2 ),
		new Obj( vect3(	+0.5,	0.12,	-0.5)*1.2 ),
		new Obj( vect3(	+0.9,	0.12,	 0.5)*1.2 ),
		new Obj( vect3(	0.0,	0.12,	 0.5)*1.2 ),
		new Obj( vect3(	-0.9,	0.12,	 0.5)*1.2 ),
	};
	

	vector<ivect2>	tblIdx =
	{
		{0,5},
		{1,6},
		{2,7},
		{3,8},
		{4,9},
	};


	vector<Edge*> tblDummy;		//	空のまま

	//------------------------------------------------------------------------------
	void drawPoint( SysGra& gra, Pers& pers, vector<Obj*>& tblPoint )
	//------------------------------------------------------------------------------
	{
		int cnt = 0 ;
		gra.SetZTest(false);
		for ( Obj* p : tblPoint )
		{
			vect3 v = pers.calcWorldToScreen3( p->pos );
			if ( v.z > 0 )
			{
				if ( p->bSelected ) 
						gra.Pset( v, rgb(1,0,0), 11 ); 
				else	gra.Pset( v, rgb(0,0,1), 11 ); 

				gra.Print( v.xy() + gra.Dot(16,-12), to_string(cnt) ); 
			}
			cnt++;
		}
		gra.SetZTest(true);
	}

	//------------------------------------------------------------------------------
	void drawCutmull( SysGra& gra, Pers& pers, vector<Obj*> tbl, vector<ivect2>idx )
	//------------------------------------------------------------------------------
	{
		// 描画 カーブ
		function<void()> func = [&]()
		{
			rgb	col(1,1,1);
			int size = (signed)idx.size();
			vect3	v0;
			vect3	v2;
			vect3	w0;
			vect3	w2;
			for ( int i = 0 ; i < size ; i++ )
			{
				int n0 = idx[i].n0;
				int n1 = idx[(i+1)%size].n0;
				int n2 = idx[(i+2)%size].n0;
				int n3 = idx[(i+3)%size].n0;

				int m0 = idx[i].n1;
				int m1 = idx[(i+1)%size].n1;
				int m2 = idx[(i+2)%size].n1;
				int m3 = idx[(i+3)%size].n1;
			
				vect3 P0 = tbl[n0]->pos;
				vect3 P1 = tbl[n1]->pos;
				vect3 P2 = tbl[n2]->pos;
				vect3 P3 = tbl[n3]->pos;

				vect3 Q0 = tbl[m0]->pos;
				vect3 Q1 = tbl[m1]->pos;
				vect3 Q2 = tbl[m2]->pos;
				vect3 Q3 = tbl[m3]->pos;
				
				for ( float t = 0.0 ; t < 1.0 ; t+=0.1 )
				{
					vect3 v1 = func_catmull3(t, P0,P1,P2,P3 );
					vect3 w1 = func_catmull3(t, Q0,Q1,Q2,Q3 );
					if ( (i==0 && t==0) ) 
					{
						v2=v1;
						w2=w1;
					}
					else 
					{
						pers.pen.line3d( gra, pers, v0, v1, col);
						pers.pen.line3d( gra, pers, w0, w1, col);
						pers.pen.line3d( gra, pers, w1, v1, col);

						{
							vect3 a = v0;a.y=0;
							vect3 b = v1;b.y=0;
							pers.pen.line3d( gra, pers, a, b, rgb(0.2,0.2,0.2));
						}
						{
							vect3 a = w0;a.y=0;
							vect3 b = w1;b.y=0;
							pers.pen.line3d( gra, pers, a, b, rgb(0.2,0.2,0.2));
						}

					}

						{
							vect3 nx,ny,nz;
							nz = (v1-v0).normalize();
							ny = vect3(0,1,0);
							nx = cross(nz,ny).normalize();
							ny = cross(nx,nz).normalize();

							mat33	m(
								nx.x,	nx.y,	nx.z,
								ny.x,	ny.y,	ny.z,
								nz.x,	nz.y,	nz.z
							);

							//square.DrawSquare( gra, pers, v1, m, false  );
						}


					v0 = v1;
					w0 = w1;
				}
			}
			pers.pen.line3d( gra, pers, v0, v2, col);
			pers.pen.line3d( gra, pers, w0, w2, col);
			pers.pen.line3d( gra, pers, w2, v2, col );
			{
				vect3 a = v0;a.y=0;
				vect3 b = v2;b.y=0;
				pers.pen.line3d( gra, pers, a, b, rgb(0.2,0.2,0.2));
			}
			{
				vect3 a = w0;a.y=0;
				vect3 b = w2;b.y=0;
				pers.pen.line3d( gra, pers, a, b, rgb(0.2,0.2,0.2));
			}
		};
		func();
	}
};

static	unique_ptr<Cutmull14> pCutmull(new Cutmull14);

//------------------------------------------------------------------------------
void Lab::lab14( Lab& lab, SysKeys& keys, SysMouse& mouse, SysGra& gra, Pers& pers, float delta, int& text_y )
//------------------------------------------------------------------------------
{
	// 画面クリア
	gra.Clr(rgb(0.3,0.3,0.3));
	pers.grid.DrawGrid3d( gra, pers, vect3(0,0,0), midentity(), 10, 10, 1, rgb(0.2,0.2,0.2) );
	gra.Print(1,(float)text_y++,to_string(lab.idx)+" : " + string(__func__ )); 

	//初期化
	if ( !lab.bInitAll )
	{
		lab.bInitAll = true;

		// カメラ
		pers.cam.pos = vect3( 0.0, 2.0, -5.0 );
		pers.cam.at = vect3( 0,  1.0, 0 );
	}

				(*pCutmull).bActive = true;


			//=================================
			// コース描画 Cutmull14
			//=================================
			if ( (*pCutmull).bActive )
			{
				// 描画
				(*pCutmull).drawCutmull( gra, pers, (*pCutmull).tblPoint, (*pCutmull).tblIdx );
			}

}
