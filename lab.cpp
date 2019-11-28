//2019/10/17

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
#include "lab0.h"
#include "lab9.h"
#include "lab11.h"
#include "lab12.h"
#include "lab13.h"
#include "lab14.h"
#include "lab15.h"
#include "lab16.h"

struct Lab1 : LabObj
{
    struct Impl;
    std::unique_ptr<Impl> pImpl;
	Lab1();
	void Update( SysKeys& keys, SysMouse& mouse, SysGra& gra, Pers& pers, float delta, int& text_y, Cp& cp );
};
struct Lab1::Impl{};
Lab1::Lab1() : pImpl( new Lab1::Impl ){}

struct Lab2 : LabObj
{
	void Update( SysKeys& keys, SysMouse& mouse, SysGra& gra, Pers& pers, float delta, int& text_y, Cp& cp );
};
struct Lab3 : LabObj
{
	void Update( SysKeys& keys, SysMouse& mouse, SysGra& gra, Pers& pers, float delta, int& text_y, Cp& cp );
};
struct Lab4 : LabObj
{
	void Update( SysKeys& keys, SysMouse& mouse, SysGra& gra, Pers& pers, float delta, int& text_y, Cp& cp );
};
struct Lab5 : LabObj
{
	void Update( SysKeys& keys, SysMouse& mouse, SysGra& gra, Pers& pers, float delta, int& text_y, Cp& cp );
};
struct Lab6 : LabObj
{
	void Update( SysKeys& keys, SysMouse& mouse, SysGra& gra, Pers& pers, float delta, int& text_y, Cp& cp );
};
struct Lab7 : LabObj
{
	void Update( SysKeys& keys, SysMouse& mouse, SysGra& gra, Pers& pers, float delta, int& text_y, Cp& cp );
};
struct Lab8 : LabObj
{
	void Update( SysKeys& keys, SysMouse& mouse, SysGra& gra, Pers& pers, float delta, int& text_y, Cp& cp );
};
struct Lab10 : LabObj
{
	void Update( SysKeys& keys, SysMouse& mouse, SysGra& gra, Pers& pers, float delta, int& text_y, Cp& cp );
};



//------------------------------------------------------------------------------
void LabObj::M::drawVect( SysGra& gra, Pers& pers, int& text_y, vect3 v0, vect3 v, float sc, rgb col, string str )
//------------------------------------------------------------------------------
{
	static const rgb col0 = rgb( 0, 0, 0 );
	static const rgb col1 = rgb( 0, 0, 1 );
	static const rgb col2 = rgb( 1, 0, 0 );
	static const rgb col3 = rgb( 1, 0, 1 );
	static const rgb col4 = rgb( 0, 1, 0 );
	static const rgb col5 = rgb( 0, 1, 1 );
	static const rgb col6 = rgb( 1, 1, 0 );
	static const rgb col7 = rgb( 1, 1, 1 );

	gra.SetZTest(false);

	vect3 v1 = v0+v*sc;

	// 影
	{
		vect3	a = v0;	a.y=0;
		vect3	b = v1;	b.y=0;
		rgb		c = (col+rgb(0.75,0.75,0.75))/4;
		pers.pen.line3d( gra, pers, a, b, c, 1 );
		pers.pen.pset3d( gra, pers,    b, c, 5 );
	}

	// 線
	pers.pen.line3d( gra, pers, v0, v1, col, 1 );
	pers.pen.pset3d( gra, pers,     v1, col, 5 );
	pers.pen.print3d( gra, pers, 	v1,12,0, str ); 



	// 矢印
	if (0)
	{

		vect2 a = pers.calcWorldToScreen3( v0 ).xy();
		vect2 b = pers.calcWorldToScreen3( v1 ).xy();
//		if ( b.z > 0  ) 
		{
			vect3 c = vect3((b-a).normalize(),0);
			vect2 d = cross(c,vect3(0,0,1)).xy();
			vect2 e = (d+(b-a))*100;
//			gra.Line( b, b + gra.Dot(-e.x,e.y), col6, 2 );
			gra.Line( b, b + d*1, col6, 2 );
		}
	}

	gra.Print(1,(float)text_y++, str+": "+to_string(v.x)+" , "+to_string(v.y)+" , "+to_string(v.z));

	gra.SetZTest(true);
};

//------------------------------------------------------------------------------
void Lab10::Update( SysKeys& keys, SysMouse& mouse, SysGra& gra, Pers& pers, float delta, int& text_y, Cp& cp )
//------------------------------------------------------------------------------
{
	static bool bGrid = true;

//	if (bGrid==false)	
	gra.Clr(rgb(0.0,0.0,0.0));
	gra.Print(1,(float)text_y++,string("lab10_colors")); 

	// 初期化
	if ( !m.bInitParam )
	{
		m.bInitParam = true;
		pers.cam.pos = vect3( -4.0, 4.0, -1.5 );
		pers.cam.at = vect3( 0,  1.0, 0 );
		bGrid = true;
	}

	const float s = 0.1;
	vect3 vert[4] =
	{
		vect3(-s,-s,0),
		vect3( s,-s,0),
		vect3( s, s,0),
		vect3(-s, s,0),
	};

	// 入力
	if ( keys.R.hi ) m.bInitParam = false;
	if ( keys.G.hi ) bGrid = !bGrid;

	// 計算

	// 衝突

	// 移動

	// 計算チェック

	// 描画
	{
		// カラーバー
		auto func = [&]( float n, float th, float p, float q )
		{
			float y = n * 0.23;

			mat33 m = mrotateByAxis( vect3(0,1,0), th );
			vect3 p0 = m * vect3(-s,-s+y, p) ;
			vect3 p1 = m * vect3( s,-s+y, p) ;
			vect3 p2 = m * vect3( s, s+y, p) ;
			vect3 p3 = m * vect3(-s, s+y, p) ;

			vect3 q0 = m * vect3(-s,-s+y, q) ;
			vect3 q1 = m * vect3( s,-s+y, q) ;
			vect3 q2 = m * vect3( s, s+y, q) ;
			vect3 q3 = m * vect3(-s, s+y, q) ;


			// 面
			{
				auto suf = [&]( 
					vect3 v0, vect3 v1, vect3 v2, vect3 v3,
					vect3 w0, vect3 w1, vect3 w2, vect3 w3,
					 rgb col 
				)
				{
					rgb top = col;
					if ( n == 10 ) top = rgb(0.70,0.70,0.70);
				
					// 右面
					pers.pen.tri3d( gra, pers, v0, v1 ,v2 , col );
					pers.pen.tri3d( gra, pers, v0 ,v2, v3 , col );

					// 右面
					pers.pen.tri3d( gra, pers, w0, w1 ,w2 , col );
					pers.pen.tri3d( gra, pers, w0 ,w2, w3 , col );

					// 先
					pers.pen.tri3d( gra, pers, v1, v0, w0, col );
					pers.pen.tri3d( gra, pers, w1, w0, v0, col );

					// 上面
					pers.pen.tri3d( gra, pers, v0, v3 ,w2 , top );
					pers.pen.tri3d( gra, pers, w2, w1 ,v0 , top );

					// 蓋
					pers.pen.tri3d( gra, pers, w2, v3, vect3(0,y+s,0), top );
					
				};
			
				{
					auto colfunc = [&]( float th )
					{
						th = fmodf( th, deg2rad(360) );

						float f = 0; 
						if ( th < deg2rad(60) ) f = th/deg2rad(60);
						else
						if ( th < deg2rad(180) ) f = 1.0;
						else
						if ( th < deg2rad(240) ) f = (deg2rad(240)-th)/deg2rad(60);
						else
						f = 0.0;
						return f;
					};

					th += deg2rad(60);
					float r = colfunc(th);
					float g = colfunc(th + deg2rad(120));
					float b = colfunc(th + deg2rad(240));

					float alu[11][2] =
					{
						{0.0,	0.0},
						{0.1,	0.0},
						{0.2,	0.0},
						{0.4,	0.0},
						{0.6,	0.0},
						{0.8,	0.0},
						{1.0,	0.0},
						{1.0,	0.2},
						{1.0,	0.4},
						{1.0,	0.6},
						{1.0,	0.8},
					};
					vect3 col = rgb(r,g,b) * alu[int(n)%11][0] + alu[int(n)%11][1];

					suf(
						q3, q0, p0, p3, 
						q1, q2, p2, p1,
						col
					);
				}
			}

			// 線
			{
				rgb c1(1,1,1);
				float wide = 2;
				pers.pen.line3d( gra, pers, p0, q0, c1, wide );
				pers.pen.line3d( gra, pers, p1, q1, c1, wide );
				pers.pen.line3d( gra, pers, p2, q2, c1, wide );
				pers.pen.line3d( gra, pers, p3, q3, c1, wide );

				pers.pen.line3d( gra, pers, q0, q1, c1, wide );
				pers.pen.line3d( gra, pers, q1, q2, c1, wide );
				pers.pen.line3d( gra, pers, q2, q3, c1, wide );
				pers.pen.line3d( gra, pers, q3, q0, c1, wide );
			}

		};
		{
			mt19937    foo(0);
			for ( float th = 0 ; th < pi*2 ; th+=deg2rad(30) )
			{
				float n = 0;
				for ( float th2 = 0 ; th2 < pi ; th2+=deg2rad(18) )
				{
					float a = 1.75*sin(th2)-0.5;
					float r = (float)(foo()%10)/10.0*a/5;
					n++;
					func( n, th, 0.37, 0.7+a+r );
				}
			}
		}
	
		
	}
}


//------------------------------------------------------------------------------
void Lab8::Update( SysKeys& keys, SysMouse& mouse, SysGra& gra, Pers& pers, float delta, int& text_y, Cp& cp )
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

	gra.Print(1,(float)text_y++,string("lab8_vector_six_lab8")); 

	const float	g = G *delta*delta;			// 重力加速度/frame
	const vect3	vg = vect3(0,0, -g);	// 重力加速度/frame

	static	Graphs::Plot plot_moment( 0.02, rgb(1,0,1) );
	static vect3 vel;
	static bool		bPause = false;
	bool bStep = false;
	static float w = 0;

	// 初期化
	if ( !m.bInitParam )
	{
		if ( !m.bInitAll )
		{
			m.bInitAll = true;
			pers.cam.pos = vect3(  0.0, 5.0, -1.0 );
			pers.cam.at = vect3( 0,  0.0, 0 );
		}
	
		m.bInitParam = true;
		//点
		for ( Obj* p : m.tbl_pObj ) delete p;
		m.tbl_pObj.clear();
		m.tbl_pObj.emplace_back( new Obj(vect3( 0  , 0.1, 0 )) );
		m.tbl_pObj.emplace_back( new Obj(vect3( 1  , 0.1, 0 )) );
		// 線
		for ( Edge* p : m.tbl_pEdge ) delete p;
		m.tbl_pEdge.clear();
		m.tbl_pEdge.emplace_back( new Edge(0,1) );

		//GUI登録
		cp.tbltbl_pObj.clear();
		cp.tbltbl_pEdge.clear();
		cp.tbltbl_pObj.emplace_back( m.tbl_pObj );
		cp.tbltbl_pEdge.emplace_back( m.tbl_pEdge );

		pers.axis.bAxisX = true;
		pers.axis.bAxisY = false;
		pers.axis.bAxisZ = true;

		vel=vect3(0,0,0);	// 加速度
		w = 0;

		plot_moment.ResetPlot();
	}

	// 入力
	if ( keys.R.hi ) m.bInitParam = false;
	if ( keys.SPACE.hi )	bPause = !bPause ;
	if ( keys.ENTER.rep )	{bStep = true; bPause = true; }

	vect3&	v0 = m.tbl_pObj[0]->pos;
	vect3&	v1 = m.tbl_pObj[1]->pos;

	vect3	bar = (v1-v0);					//	棒
	vect3	moment = cross(-bar, vel+vg);	//	回転モーメント 仮
	vect3	F = cross(bar, moment );		//	力

	// 計算
	if ( !bPause || bStep )
	{
		plot_moment.WritePlot( moment.y*10 );

			
		// 衝突
		{
			vel = mrotateByAxis( moment, (vel+vg).abs() )*vel+vg;

			v1 += vel;

//			mat33 m = mrotateByAxis( moment, w );

//			v1 = m * (v1-v0) + v0;
//			w += F.abs();
//			vel += F;
		}
 		
	}

	m.drawVect( gra, pers, text_y, v0, moment ,10	, rgb(1,0,1), "moment" );
	m.drawVect( gra, pers, text_y, v1, vg		,100	, rgb(1,0,0), "vg" );
	m.drawVect( gra, pers, text_y, v1, F		,1		, rgb(0,1,0), "F" );
	plot_moment.DrawPlot( gra, pers );
	gra.Print(1,(float)text_y++,string("<<radius>>")+to_string(bar.abs())); 


}

//------------------------------------------------------------------------------
void Lab6::Update( SysKeys& keys, SysMouse& mouse, SysGra& gra, Pers& pers, float delta, int& text_y, Cp& cp )
//------------------------------------------------------------------------------
{
	//画面クリア
	gra.Clr(rgb(0.3,0.3,0.3));
	pers.grid.DrawGrid3d( gra, pers, vect3(0,0,0), midentity(), 10, 10, 1, rgb(0.2,0.2,0.2) );

	gra.Print(1,(float)text_y++,string("<<lab6_tire3d>>")); 

	const vect3 G_pos = vect3(0,2.0,0);
	const vect3 G_acc= vect3(0,0,0.0);
	const float G_radius = 0.5;
	const float G_head = deg2rad(35);	//	タイヤの方向
	const float G_bank = deg2rad(40);	//	回転角
	const float G_rspd = deg2rad(0);	//	角速度
	const float G = 9.80665;				// 重力加速度
	const float grate = 9.80665 / 60/60;	// 重力加速度

	static vect3 pos = G_pos;
	static vect3 acc1 = G_acc;
	static vect3 acc2 = G_acc;
	static float radius = G_radius;
	static float head = G_head;
	static float bank = G_bank;
	static float rspd = G_rspd;

	
	// 動き
	{
		// 強制
		{
			// リセット
			if ( keys.R.hi ) 
			{
				pos = G_pos;
				acc1 = G_acc;
				acc2 = G_acc;
				radius = G_radius;
				head = G_head;
				bank = G_bank;
				rspd = G_rspd;
			}
			if ( mouse.F.hi ) 
			{
				acc1 += vect3( sin(head), 0, cos(head) )*0.02;
				acc2 += vect3( sin(head), 0, cos(head) )*0.02;
			}
			if ( mouse.B.hi ) rspd += deg2rad(5);
			if ( mouse.R.hi ) rspd += deg2rad(-5);
		}

		// 重力
		{
			acc1.y -= grate;
			acc2.y -= grate;
//						pos += (acc1+acc2)/2 ;
		}

		// 回転
		{
			bank += rspd;
		}
	}

	float high = abs(radius*cos(bank));
	
	// 衝突	地面
	if ( high > pos.y ) 
	{

		pos.y = high;

		// バウンド
		if ( bank > 0 )
		{
		}
		else
		{
		}
//						vel.y = -vel.y * 0.5; 
//					vel.y = 0;
#if 0
		// 回転
		{
			float a = atan( vel.y * sin(bank) / radius );
			rspd = a;
		}
		// 衝突 bank
		if ( bank > deg2rad(90) ) bank -= deg2rad(180);
		if ( bank < deg2rad(-90) ) bank += deg2rad(180);
#endif
	}
	
	// 設置地面
	if ( high == pos.y ) 
	{
		// 衝突 bank
		if ( bank > deg2rad(90) ) bank -= deg2rad(180);
		if ( bank < deg2rad(-90) ) bank += deg2rad(180);

		// 回転
		{
			float a = atan( grate * sin(bank) / radius );
			rspd += a;
		}
	}

	// 減衰	空気抵抗
	{
//						rspd *= 0.99;
	}

	// 描画
	pers.prim.DrawTire( gra, pers, pos, head, bank, radius );
}

//------------------------------------------------------------------------------
void Lab5::Update( SysKeys& keys, SysMouse& mouse, SysGra& gra, Pers& pers, float delta, int& text_y, Cp& cp )
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

	gra.Print(1,(float)text_y++,string("<<lab5_furiko2d>>")); 

	const float grate = 9.80665 *delta*delta;		// 重力加速度/frame

	static float	rsp = 0;

	if ( !m.bInitParam )
	{
		if ( !m.bInitAll )
		{
			m.bInitAll = true;
			pers.cam.pos = vect3(  0.0, 2.0, -5.0 );
			pers.cam.at = vect3( 0,  2.0, 0 );
		}
	
		m.bInitParam = true;
		for ( Obj* p : m.tbl_pObj ) delete p;
		m.tbl_pObj.clear();
		m.tbl_pObj.emplace_back( new Obj(vect3(0, 2.0, 0)) );
		m.tbl_pObj.emplace_back( new Obj(vect3(1, 2.0, 0)) );
		// 線
		for ( Edge* p : m.tbl_pEdge ) delete p;
		m.tbl_pEdge.clear();
		m.tbl_pEdge.emplace_back( new Edge(0,1) );

		//GUI登録
		cp.tbltbl_pObj.clear();
		cp.tbltbl_pEdge.clear();
		cp.tbltbl_pObj.emplace_back( m.tbl_pObj );
		cp.tbltbl_pEdge.emplace_back( m.tbl_pEdge );

		rsp=0;
	}
	if ( keys.R.hi )	m.bInitParam = false;

	vect3&	v0 = m.tbl_pObj[0]->pos;	//	barの根本
	vect3&	v1 = m.tbl_pObj[1]->pos;	//	barの先端


//	pers.pen.line3d( gra, pers, v0, v1, rgb(1,1,1), 2 );

	// 縮む
	if ( mouse.F.hi )	v1 = (v1+v0)/2;

	// 伸びる
	if ( mouse.B.hi )	v1 = (v1-v0)*2+v0;

	{
		vect3 v = v1-v0;
		float b = atan2(v.x,v.y);

		// 角速度に変換
		float tsp = -grate * sin(b);	//	接線速度
		float r = tsp/2/pi/v.abs();		//	角加速度
		rsp +=r;						//	角速度

		// 回転
		float x = v.x *cos(rsp) - v.y*sin(rsp); 
		float y = v.x *sin(rsp) + v.y*cos(rsp); 
		v1 = v0+vect3(x,y,0);

	}
}
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

//------------------------------------------------------------------------------
void Lab4::Update( SysKeys& keys, SysMouse& mouse, SysGra& gra, Pers& pers, float delta, int& text_y, Cp& cp )
//------------------------------------------------------------------------------
{
	//画面クリア
	gra.Clr(rgb(0.3,0.3,0.3));
	pers.grid.DrawGrid3d( gra, pers, vect3(0,0,0), mrotx(deg2rad(90)), 100, 100, 1, vect3(0.2,0.2,0.2) );

//	pers.grid.DrawGrid3d( gra, pers, vect3(0,0,0), midentity(), 10, 10, 1, rgb(0.2,0.2,0.2) );
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

	static vect3	vel;			// 運動量
	static vect3	mov;			// 運動量
	static bool	bPause = false; 
	bool	bStep = false; 


	// 初期化
	if ( !m.bInitParam )
	{
		if ( !m.bInitAll )
		{
			m.bInitAll = true;
			pers.cam.pos = vect3(  0.0, 0.0, -7.0 );
			pers.cam.at = vect3( 0,  0.0, 0 );
		}

		m.bInitParam = true;
		for ( Obj* p : m.tbl_pObj ) delete p;
		m.tbl_pObj.clear();
		m.tbl_pObj.emplace_back( new Obj(vect3(0, 2.0, 0)) );
		m.tbl_pObj.emplace_back( new Obj(vect3(-1, 2.0, 0)) );

		//GUI登録
		cp.tbltbl_pObj.clear();
		cp.tbltbl_pEdge.clear();
		cp.tbltbl_pObj.emplace_back( m.tbl_pObj );
		
		vel = vect3(0,0,0);
	}

	vect3&	v0 = m.tbl_pObj[0]->pos;	//	barの根本
	vect3&	v1 = m.tbl_pObj[1]->pos;	//	barの先端

	// 入力
	{
		// ポーズ
		if ( keys.SPACE.hi )	bPause = !bPause;

		// ステップ再生
		if ( keys.ENTER.rep )	bStep = true;

		// リセット
		if ( keys.R.hi )	m.bInitParam = false ;

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
	if ( !bPause || bStep )
	{
		vel += F;

	 	float	w = vel.abs()/r;					//	角速度

mov =vel;
		vel = mrotateByAxis( moment, w ) * vel;		//	回転

		v1 += vel;
	}

	// 描画
	{


		pers.pen.line3d( gra, pers, v0, v1, rgb(1,1,1), 2 );

		m.drawVect( gra, pers, text_y, v1, vg	,100, rgb(1,0,0), "g" );
		m.drawVect( gra, pers, text_y, v0, moment,100, rgb(1,0,1), "moment" );
		m.drawVect( gra, pers, text_y, v1, F		,100, rgb(0,1,0), "F" );
		m.drawVect( gra, pers, text_y, v1, vel	,2	, rgb(1,1,0), "vel" );
		m.drawVect( gra, pers, text_y, v1, mov	,2	, rgb(0,0,1), "mov" );
	}


#if 0
	{
		static float	rsp = 0;
		static vect3	vv = 0;

		vect3 v = (v1-v0);

		// 角速度に変換
		{
			float b = atan2(v.x,v.y);
			float t = -g * sin(b);			//	接線速度
			float r = t/2/pi/v.abs();		//	角加速度
			rsp +=r;						//	角速度
		}

		// 回転
		{
			float x = v.x *cos(rsp) - v.y*sin(rsp); 
			float y = v.x *sin(rsp) + v.y*cos(rsp); 
			vect3 a = vect3(x,y,0);
			v1 = v0 + a;

			pers.pen.line3d( gra, pers, v1, v1+(a-v)*10, rgb(1,0,0),2 );
			
		}

		pers.pen.line3d( gra, pers, v0, v1, rgb(1,1,1), 2 );

		// 加速ベクトルの実験
		{
			vect3 vg = vect3(0,-g,0);	//	重力加速度ベクトル
			vect3 n0 = cross( v, vg );
			vect3 vt = cross( n0, v );

//						pers.pen.line3d( gra, pers, v1, v1+vt*100, rgb(0,1,0) );
			vect3 v2 = v*1.05+v0;
//						pers.pen.line3d( gra, pers, v2, v2+vv, rgb(0,1,0) );

//						pers.pen.line3d( gra, pers, v2, v2+vg*100, rgb(1,0,0) );
			pers.pen.line3d( gra, pers, v2, v2+vt*100, rgb(0,1,0) );
			pers.pen.line3d( gra, pers, v2, v2+n0*100, rgb(0,0,1) );					}

	}
#endif
#if 0
	{
		vect3 vg = vect3(0,-g,0);	//	重力加速度ベクトル
		vect3 v = (v2-v0).normalize();
		vect3 n0 = cross( v, vg );
		vect3 vt = cross( n0, v );

		vv += vt;
		v2 += vt*10;

//					pers.pen.line3d( gra, pers,  0,  vv*10, rgb(0,1,1), 2 );
		pers.pen.line3d( gra, pers,  vect3(1,0,0),  vect3(1,0,0)+vt*100, rgb(0,1,0), 1 );

		pers.pen.line3d( gra, pers, v2, v2+vt*100, rgb(0,1,0) );
		pers.pen.line3d( gra, pers, v0, v2, rgb(0,1,1), 2 );

	}
#endif

	
}

//=================================
// 描画	引力実験
//=================================

//------------------------------------------------------------------------------
void Lab3::Update( SysKeys& keys, SysMouse& mouse, SysGra& gra, Pers& pers, float delta, int& text_y, Cp& cp )
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
	gra.Print(1,(float)text_y++,string("<<lab3_gravityPlanet>>")); 

	static bool	bPause = false; 
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

		for ( Obj* p : m.tbl_pObj ) delete p;
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
		if ( keys.SPACE.hi )	bPause = !bPause;

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
		}

	}

	
	Planet& pl0 = *dynamic_cast<Planet*>(m.tbl_pObj[0]);	//	太陽
	Planet& pl1 = *dynamic_cast<Planet*>(m.tbl_pObj[1]);	//	地球

	// 計算
	if ( !bPause || bStep )
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
		for ( Obj* p : m.tbl_pObj ) 
		{
			Planet& pl = *dynamic_cast<Planet*>(p);
			for ( int i = 0 ; i < MAX_PREV ; i++ )
			{
				pers.pen.pset3d( gra, pers, pl.tblPrev[i], rgb(0,1,1),2 );
			}
		}
	}

}
		
//=================================
// 描画	角速度 実験
//=================================
//------------------------------------------------------------------------------
void Lab2::Update( SysKeys& keys, SysMouse& mouse, SysGra& gra, Pers& pers, float delta, int& text_y, Cp& cp )
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

	gra.Print(1,(float)text_y++,string("<<lab2_kakusokudo>>")); 

	const float	g = G *delta*delta;		// 重力加速度/frame

	static bool bShot = false;
	static vect3	velocity;
	static vect3	moment;
	static vect3	to;
	static vect3	add;
	
	static float 	w;	//	角速度

	// 初期化
	if ( !m.bInitParam )
	{
		if ( !m.bInitAll )
		{
			m.bInitAll = true;
			pers.cam.pos = vect3( 2.0, 2.5, -2.0 );
			pers.cam.at = vect3( 0,  0.0, 0 );
		}
		m.bInitParam = true;

		for ( Obj* p : m.tbl_pObj ) delete p;
		m.tbl_pObj.clear();
		m.tbl_pObj.emplace_back( new Obj(vect3(0  ,0.1,0)) );
		m.tbl_pObj.emplace_back( new Obj(vect3(1  ,0.1,0)) );
		m.tbl_pObj.emplace_back( new Obj(vect3(1.5,0.1,0.2)) );
		// 線
		for ( Edge* p : m.tbl_pEdge ) delete p;
		m.tbl_pEdge.clear();
		m.tbl_pEdge.emplace_back( new Edge(0,1) );
		m.tbl_pEdge.emplace_back( new Edge(1,2,rgb(0,1,0),1) );

		//GUI登録
		cp.tbltbl_pObj.clear();
		cp.tbltbl_pEdge.clear();
		cp.tbltbl_pObj.emplace_back( m.tbl_pObj );
		cp.tbltbl_pEdge.emplace_back( m.tbl_pEdge );

		bShot = false;
	}


	vect3&	v0 = m.tbl_pObj[0]->pos;	//	barの根本
	vect3&	v1 = m.tbl_pObj[1]->pos;	//	barの先端
	vect3&	v2 = m.tbl_pObj[2]->pos;	// 速度指定


	//入力
	{
		// リセット
		if ( keys.R.hi )	m.bInitParam = false ;

		if ( mouse.B.hi )	bShot = !bShot ;
	}

	// 計算
		
	// 描画
	{
		if ( bShot )
		{
			vect3	bar = (v1-v0);							//	棒
			float	radius = bar.abs();

			to = mrotateByAxis( moment, w ) * bar;			//	移動計算

			vect3 add  = to - bar;

			v1 += add;
			
			
		}
		else
		{
			vect3	bar = (v1-v0);							//	棒
			float	radius = bar.abs();
			moment = cross(-bar,v2-v1);						//	回転モーメント
			velocity = cross(bar/radius, moment/radius );	//	ベロシティ

			w = velocity.abs()/radius;
			to = mrotateByAxis( moment, w ) * bar;			//	移動計算

		}
/*

		{// 影 描画
			vect3	va = v0;va.y = 0;
			vect3	vb = v1;vb.y = 0;
			pers.pen.line3d( gra, pers, va, vb, rgb(1,1,1)/4, 2 );
		}
		{// 影 描画
			vect3	va = v1;va.y = 0;
			vect3	vb = v2;vb.y = 0;
			pers.pen.line3d( gra, pers, va, vb, rgb(1,1,1)/4, 2 );
		}

		pers.pen.line3d( gra, pers, v0, v1, rgb(1,1,1), 2 );	//	棒
		pers.pen.line3d( gra, pers, v1, v2, rgb(0,1,0), 1 );	// 外的な力
*/
		m.drawVect( gra, pers, text_y, v1, velocity	,1	, rgb(1,1,0), "velocity" );
		m.drawVect( gra, pers, text_y, v0, moment		,1	, rgb(1,0,1), "moment" );
		m.drawVect( gra, pers, text_y, v0, to			,1	, rgb(0,1,1), "to" );
			
	}

}

//------------------------------------------------------------------------------
void Lab1::Update( SysKeys& keys, SysMouse& mouse, SysGra& gra, Pers& pers, float delta, int& text_y, Cp& cp )
//------------------------------------------------------------------------------
{
	//画面クリア
	gra.Clr(rgb(0.3,0.3,0.3));
	pers.grid.DrawGrid3d( gra, pers, vect3(0,0,0), mrotx(deg2rad(90)), 100, 100, 1, vect3(0.2,0.2,0.2) );

	gra.Print(1,(float)text_y++,string("<<lab1_graph>>")); 

	rgb	col = vect3(0.2,0.2,0.2);

	// 初期化
	if ( !m.bInitAll )
	{
		m.bInitAll = true;
		pers.cam.pos = vect3( 0.0, 0.0, -5.0 );
		pers.cam.at = vect3( 0,  0.0, 0 );
	}

	if ( !m.bInitParam )
	{
		m.bInitParam = true;
	}


	//入力
	{
		// リセット
		if ( keys.R.hi )	m.bInitParam = false ;
	}


	float n = 10;
	{
		float dt = 0.1;
		float a = 1*dt*dt;
		float v = 0;
		float s = 0;

		for ( float t = 0 ; t < n ; t+=dt )
		{
			v += a;
			s += v;

			pers.pen.pset2d( gra, pers, vect2(t,s), rgb(1,0,0), 3 );
		}
	}

	{
		float a = 1;
		float v = 0;
		float s = 0;

		float t0 = 0;
		float v0 = v;
		float s0 = s;

		for ( float t = 0 ; t < n ; t+=0.1 )
		{
			v = a * t;
			s = v * t * 0.5f;

			pers.pen.line2d( gra, pers, vect2(t0,s0), vect2(t,s), rgb(0,0.5,1), 1 );
			pers.pen.line2d( gra, pers, vect2(t0,v0), vect2(t,v), rgb(0,1,0), 1 );

			pers.pen.pset2d( gra, pers, vect2(t,s), rgb(0,0.5,1), 3 );
			pers.pen.pset2d( gra, pers, vect2(t,v), rgb(0,1,0), 3 );
			pers.pen.print2d( gra, pers, vect2(t,s),0,0, to_string(s) );
		
			t0 = t;
			v0 = v;
			s0 = s;


		}
	}

}

//------------------------------------------------------------------------------
void Lab::SetIdx( int n, Cp& cp )
//------------------------------------------------------------------------------
{
cout << idx << endl;
	idx = max( n, 0 ); 

	cp.tbltbl_pObj.clear();
	cp.tbltbl_pEdge.clear();

	if ( pLab ) 
	{
		delete pLab;
		pLab=0;
	};

	switch( idx )
	{
		case 0:		pLab = new Lab0;	break;
		case 1:		pLab = new Lab1;	break;
		case 2:		pLab = new Lab2;	break;
		case 3:		pLab = new Lab3;	break;
		case 4:		pLab = new Lab4;	break;
		case 5:		pLab = new Lab5;	break;
		case 6:		pLab = new Lab6;	break;
		case 7:		pLab = new Lab7;	break;
		case 8:		pLab = new Lab8;	break;
		case 9:		pLab = new Lab9;	break;
		case 10:	pLab = new Lab10;	break;
		case 11:	pLab = new Lab11;	break;
		case 12:	pLab = new Lab12;	break;
		case 13:	pLab = new Lab13;	break;
		case 14:	pLab = new Lab14;	break;
		case 15:	pLab = new Lab15;	break;
		case 16:	pLab = new Lab16;	break;
		default: break;
	}
	

}

static	int	tblIdxLab[] = 
{
	0,
	1,
	2,
	3,
	4,
	5,
	6,
	7,
	8,
	9,
//	10,
	11,
	12,
	13,
	14,
	15,
	16,
};	
static const int sizeIdxLab = static_cast<signed>(sizeof(tblIdxLab)/sizeof(int));
//------------------------------------------------------------------------------
void Lab::SetNextIdx( int val, Cp& cp )
//------------------------------------------------------------------------------
{
	int num;
	for ( num = 0 ; num < sizeIdxLab ; num++ )
	{
		if ( tblIdxLab[num] == idx )
		{
			break;
		}
	}
	if ( num < sizeIdxLab ) 
	{
		num = max(num+val,0);
		num = min(num+val, sizeIdxLab-1 );
	}
	else
	{
		num = idx;
	}
	SetIdx( num, cp );
}

//------------------------------------------------------------------------------
void Lab::Update( SysKeys& keys, SysMouse& mouse, SysGra& gra, Pers& pers, float delta, int& text_y, Cp& cp )
//------------------------------------------------------------------------------
{
	if ( pLab ) pLab->Update( keys, mouse, gra, pers, delta, text_y, cp );
}

//------------------------------------------------------------------------------
Lab::Lab()
//------------------------------------------------------------------------------
{
	idx = 13;
}

