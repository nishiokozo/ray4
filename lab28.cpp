//2019/12/26

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
#include "lab28.h"



struct Lab28::Impl
{
	bool	bResetAll = true;
	bool	bResetParam = true;
	bool	bPause = false;
	bool	bStep = false;

	vector<shared_ptr<Obj>>	tbl_pObj;

	vector<int>	idxPoint;
};

Lab28::Lab28() : pImpl( new Lab28::Impl ){}


//------------------------------------------------------------------------------
void Lab28::Update( SysKeys& keys, SysMouse& mouse, SysSound& sound, SysGra& gra, Pers& pers, float delta, int& text_y, Cp& cp )
//------------------------------------------------------------------------------
{
	// 画面クリア
	gra.Clr(rgb(0.3,0.3,0.3));
	pers.grid.DrawGrid3d( gra, pers, vect3(0,0,0), mat33::mrotx(rad(90)), 10, 10, 1, rgb(0.2,0.2,0.2) );
	gra.Print(1,(double)text_y++,"28 : B-Spline (open uniform)"); 

	//初期化
	if ( pImpl->bResetAll )
	{
		pImpl->bResetAll = false;

	}

	if ( pImpl->bResetParam )
	{
		pImpl->bResetParam = false;

		// カメラ
		pers.cam.pos	= vect3( 3.0, 3.0, -12.0 );
		pers.cam.at		= vect3( 3.0, 3.0, 0 );
		pers.cam.Update();
		pers.fovy=rad(45);
	}
	if ( keys.R.hi )	pImpl->bResetParam = true;


	// fa(t) 右肩上がり
	auto fa = []( double t, double tmax )
	{
		return t/(tmax-1.0); 
	};
	// fb(t) 右肩下がり
	auto fb = []( double t, double tmax )
	{
		return (tmax-t)/(tmax-1.0); 
	};

	// N1(t) 一次基底関数 山型波形 0<t<2.0
	auto N1 = []( double t )
	{
		if ( t < 0 || t > 2.0 ) return 0.0;
		return ( t < 1.0 )?t:2.0-t; 
	};
	// N2(t) 二次基底関数 なめらかな山型 0<t<3.0
	auto N2 = [&]( double t )
	{
		return N1(t) * fa(t,3) + N1(t-1) * fb(t,3);
	};
	// N3(t) 三次基底関数 よりなめらかな山型 0<t<4.0
	auto N3 = [&]( double t )
	{
		return N2(t) * fa(t,4) + N2(t-1) * fb(t,4);
	};

	float wide = 1.0;

	vector<vect3> pos =
	{
		{1,2+0,0},
		{1,2+2,0},
		{3,2+3,0},
		{4,2+1,0},
		{6,2+1,0},
		{6,2+3,0},
	};
	int cntVert = (signed)pos.size();


	// 二次B-スプライン 合成グラフ
if(0)
	{
		float tmax = 4+3;
		vect3 v0 = vect3(7,3,0);
		for ( double t = 0 ; t <= tmax  ; t+=0.01 )
		{
			vect3 v = v0;
			pers.pen.pset3d( gra, pers, v+vect3(t, N2(t  ) ,0), rgb(1,1,0), 1 );
			pers.pen.pset3d( gra, pers, v+vect3(t, N2(t-1) ,0), rgb(0,1,1), 1 );
			pers.pen.pset3d( gra, pers, v+vect3(t, N2(t-2) ,0), rgb(1,0,1), 1 );

			pers.pen.pset3d( gra, pers, v+vect3(t, N2(t-0)+N2(t-1)+N2(t-2) ,0), rgb(1,1,1), 1 );
		}
		for ( int t = 0 ; t <= tmax ; t++ )
		{
			pers.pen.print3d( gra, pers, v0+vect3( t,0,0), 0,32, to_string(t) ); 
		}
			pers.pen.print3d( gra, pers, v0+vect3( 2,0,0), 0,64, "B-Spline(2)" ); 
	}
	// 三次B-スプライン 合成グラフ
	{
		vect3 v0 = vect3(0,0,0);
		float tmax = 4+3;
		for ( double t = 0 ; t <= tmax  ; t+=0.01 )
		{
			float c = 1.0;
		//	c = ( t < 2.0 || t > cntVert+1 )?0.5:1.0;
		
			vect3 v = v0;
			pers.pen.pset3d( gra, pers, v+vect3(t, N3(t  ) ,0), rgb(1,1,0)*c, 1 );
			pers.pen.pset3d( gra, pers, v+vect3(t, N3(t-1) ,0), rgb(0,1,1)*c, 1 );
			pers.pen.pset3d( gra, pers, v+vect3(t, N3(t-2) ,0), rgb(1,0,1)*c, 1 );
			pers.pen.pset3d( gra, pers, v+vect3(t, N3(t-3) ,0), rgb(0,0,1)*c, 1 );

			pers.pen.pset3d( gra, pers, v+vect3(t, N3(t-0)+N3(t-1)+N3(t-2)+N3(t-3) ,0), rgb(1,1,1)*c, 1 );
		}
		for ( int t = 0 ; t <= tmax ; t++ )
		{
			pers.pen.print3d( gra, pers, v0+vect3( t,0,0), 0,32, to_string(t) ); 
		}
			pers.pen.print3d( gra, pers, v0+vect3( 0,0,0), 0,64, "B-Spline(3)" ); 
	}


	// ベースライン
	for ( int  i = 0 ; i < cntVert ; i++ )
	{
		pers.pen.pset3d( gra, pers, pos[i], rgb(0,1,0), 5 );
		if ( i > 0 ) pers.pen.line3d( gra, pers, pos[i-1], pos[i], rgb(0,1,0), 1 );
	}

	// 答え
	{ 
		auto Coefficent = [](double t) 
		{
			double r;
			double d;

			if(t < 0.0) t = -t;
			if(t < 1.0) 
			{
				r = (3.0 * t * t * t -6.0 * t * t + 4.0) / 6.0;
			}
			else if(t < 2.0) 
			{
				d = t - 2.0;
				r = -d * d * d / 6.0;
			}
			else 
			{
				r = 0.0;
			}
			return r;
		};

		double step = 0.04;
		for( double t = -1.0 ; t <= cntVert ; t += step) 
		{
			int j,k;

			vect3 v(0,0,0);
			for(j = -2 ; j < cntVert + 2 ; j++) 
			{
				k = j;
				if(j < 0  ) k = 0;
				if(j > cntVert-1) k = cntVert-1;
				v += pos[k] * Coefficent(t - j);
			}
			pers.pen.pset3d( gra, pers, v+vect3(0.1,0,0), rgb(0,1,1)/0.8, 1 );
		}
	}

	// 3次スプラインは、基底関数の山の幅が4(3+1)あるので、4つの山を1.0づつずらしてウェイトとして頂点に掛けて足し合わせる。
	// ノットベクトルが、[0,1,2,3,4,5,6,7] のように、等間隔の整数の時、ユニフォームBスプライン
//cout << endl;
	float step = 0.01/4;
	for( double t = 0.0 ; t < cntVert ; t += step ) 
	{
/*
		int n0 = 0;
		int n1 = 1;
		int n2 = 2;
		int n3 = 3;
	
		float ofs = 0.0;

		vect3 v0 = pos[n0] * N3( t-0 );
		vect3 v1 = pos[n1] * N3( t-1 );
		vect3 v2 = pos[n2] * N3( t-2 );
		vect3 v3 = pos[n3] * N3( t-3 );

		vect3 v = v0+v1+v2+v3;
*/

		vect3 v = vect3(0,0,0);
		for( int i = 0 ; i < cntVert ; i++ )
		{

			v += pos[i] * N3( t-i );
		}



		float f = t/5.0;
		pers.pen.pset3d( gra, pers, v, rgb(1-f,f,0), 2 );

		float l; 
		l = 1.0;
		if ( t>l-step/2 &&  t<l+step/2 ) pers.pen.pset3d( gra, pers, v, rgb(1,1,0), 8 );
		l=2.0;
		if ( t>l-step/2 &&  t<l+step/2 ) pers.pen.pset3d( gra, pers, v, rgb(1,1,0), 8 );
		l=3.0;
		if ( t>l-step/2 &&  t<l+step/2 ) pers.pen.pset3d( gra, pers, v, rgb(1,1,0), 8 );
		l=4.0;
		if ( t>l-step/2 &&  t<l+step/2 ) pers.pen.pset3d( gra, pers, v, rgb(1,1,0), 8 );
		l=5.0;
		if ( t>l-step/2 &&  t<l+step/2 ) pers.pen.pset3d( gra, pers, v, rgb(1,1,0), 8 );
		l=6.0;
		if ( t>l-step/2 &&  t<l+step/2 ) pers.pen.pset3d( gra, pers, v, rgb(1,1,0), 8 );
		l=7.0;
		if ( t>l-step/2 &&  t<l+step/2 ) pers.pen.pset3d( gra, pers, v, rgb(1,1,0), 8 );
		l=8.0;
		if ( t>l-step/2 &&  t<l+step/2 ) pers.pen.pset3d( gra, pers, v, rgb(1,1,0), 8 );
//cout << t << endl;
	}


}
