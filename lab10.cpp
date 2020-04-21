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
#include "lab10.h"

using namespace std;

struct Lab10::Impl
{
	bool	bResetAll = true;
	bool	bResetParam = true;
	bool	bPause = false;
	bool	bStep = false;

	vector<shared_ptr<Obj>>	tbl_pObj;
};
Lab10::Lab10() : pImpl( new Lab10::Impl ){}

//------------------------------------------------------------------------------
void Lab10::Update( SysKeys& keys, SysMouse& mouse, SysSound& sound, SysGra& gra, Pers& pers, float delta, int& text_y, Cp& cp )
//------------------------------------------------------------------------------
{

	gra.Clr(rgb(0.0,0.0,0.0));
	gra.Print(1,(float)text_y++,string("lab10_colors")); 

	// 初期化
	if ( pImpl->bResetAll )
	{
		pImpl->bResetAll = false;
		pers.cam.pos = vect3( -4.0, 4.0, -1.5 );
		pers.cam.at = vect3( 0,  1.0, 0 );
		pers.cam.Update();
	}

	const float s = 0.1;
/*
	vect3 vert[4] =
	{
		vect3(-s,-s,0),
		vect3( s,-s,0),
		vect3( s, s,0),
		vect3(-s, s,0),
	};
*/
	// 入力
	if ( keys.R.hi ) pImpl->bResetAll = true;

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
					pers.pen.Tri3d( gra, pers, v0, v1 ,v2 , col );
					pers.pen.Tri3d( gra, pers, v0 ,v2, v3 , col );

					// 右面
					pers.pen.Tri3d( gra, pers, w0, w1 ,w2 , col );
					pers.pen.Tri3d( gra, pers, w0 ,w2, w3 , col );

					// 先
					pers.pen.Tri3d( gra, pers, v1, v0, w0, col );
					pers.pen.Tri3d( gra, pers, w1, w0, v0, col );

					// 上面
					pers.pen.Tri3d( gra, pers, v0, v3 ,w2 , top );
					pers.pen.Tri3d( gra, pers, w2, w1 ,v0 , top );

					// 蓋
					pers.pen.Tri3d( gra, pers, w2, v3, vect3(0,y+s,0), top );
					
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
					float a = alu[int(n)%11][1];
					vect3 col = rgb(r,g,b) * alu[int(n)%11][0] + vect3(a,a,a);

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
				pers.pen.Line3d( gra, pers, p0, q0, c1, wide );
				pers.pen.Line3d( gra, pers, p1, q1, c1, wide );
				pers.pen.Line3d( gra, pers, p2, q2, c1, wide );
				pers.pen.Line3d( gra, pers, p3, q3, c1, wide );

				pers.pen.Line3d( gra, pers, q0, q1, c1, wide );
				pers.pen.Line3d( gra, pers, q1, q2, c1, wide );
				pers.pen.Line3d( gra, pers, q2, q3, c1, wide );
				pers.pen.Line3d( gra, pers, q3, q0, c1, wide );
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
