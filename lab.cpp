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
#include "Sys.h"

#include "func.h"
#include "pers.h"

#include "raytrace.h"
#include "skeleton.h"
#include "gui.h"

#include "lab.h"


struct	Graphs
{
	struct Plot
	{
		//	y = 0面に座標(0,0,0)を中心にグラフ（ドット）を打ってゆく

		static const int MaxPlot = 100;
		float tblPlot[MaxPlot];
		int cntPlot = 0;
		bool	bScroll = false;
		rgb col;
		float step;
		
		//------------------------------------------------------------------------------
		Plot( 
		//------------------------------------------------------------------------------
			float _step, 			// 送りステップ
			rgb _col = rgb(1,1,1) 	// 色
		)
		{
			col = _col;
			step = _step;
			for ( int i = 0 ; i < MaxPlot ; i++ )
			{
				tblPlot[i] = 0;
			}
			Reset();
		}

		//------------------------------------------------------------------------------
		void Reset()
		//------------------------------------------------------------------------------
		{
			bScroll = false;
			cntPlot = 0 ;
		}
		//------------------------------------------------------------------------------
		void Write( float val )
		//------------------------------------------------------------------------------
		{
			if ( cntPlot >= MaxPlot ) 
			{
				bScroll = true;
				cntPlot = 0;
			}

			tblPlot[ cntPlot++ ] = val;
		}
		//------------------------------------------------------------------------------
		void Draw( SysGra& gra, Pers& pers )
		//------------------------------------------------------------------------------
		{
			gra.SetZTest( false );
			
			float x = 0;
			
			if ( bScroll )
			for ( int i = cntPlot ; i < MaxPlot ; i++ )
			{
				pers.pset3d( gra, pers, vect3( x, 0, tblPlot[i] ),  col, 2 );
				x += step;
			}
			for ( int i = 0 ; i < cntPlot-1 ; i++ )
			{
				pers.pset3d( gra, pers, vect3( x, 0, tblPlot[i] ),  col, 2 );
				x += step;
			}
			if ( cntPlot > 0 ) pers.pset3d( gra, pers, vect3( x, 0, tblPlot[cntPlot-1] ),  rgb(1,1,1), 2 );

			gra.SetZTest( true );
		}

	};

	vector<Plot>	plots;

	//------------------------------------------------------------------------------
	int Add(
		float _step, 			// 送りステップ
		rgb _col = rgb(1,1,1) 	// 色
	)
	//------------------------------------------------------------------------------
	{
		int idx = (signed)plots.size(); 
		plots.emplace_back( _step, _col );
		return idx;
	}

	//------------------------------------------------------------------------------
	void Clear()
	//------------------------------------------------------------------------------
	{
		plots.clear();
	}
	
	//------------------------------------------------------------------------------
	void Request( int idx, float val )
	//------------------------------------------------------------------------------
	{
		if ( idx >= (signed)plots.size() ) return;
		plots[ idx ].Write( val );
	}

	//------------------------------------------------------------------------------
	void Draw( SysGra& gra, Pers& pers )
	//------------------------------------------------------------------------------
	{
		for ( Plot& p : plots )
		{
			p.Draw( gra, pers );
		}
	}
};


//------------------------------------------------------------------------------
static void drawVect( SysGra& gra, Pers& pers, int& text_y, vect3 v0, vect3 v, float sc, rgb col, string str )
//------------------------------------------------------------------------------
{
	vect3 v1 = v0+v*sc;

	// 影
	{
		vect3	a = v0;	a.y=0;
		vect3	b = v1;	b.y=0;
		rgb		c = (col+rgb(0.75,0.75,0.75))/4;
		pers.line3d( gra, pers, a, b, c, 1 );
		pers.pset3d( gra, pers,    b, c, 5 );
	}

	// 線
	pers.line3d( gra, pers, v0, v1, col, 1 );
	pers.pset3d( gra, pers,     v1, col, 5 );
	pers.print3d( gra, pers, 	v1, 0,0, str ); 

	gra.Print(1,(float)text_y++, str+": "+to_string(v.x)+" , "+to_string(v.y)+" , "+to_string(v.z));
};

//------------------------------------------------------------------------------
static void lab10_colors( Lab& lab, SysKeys& keys, SysMouse& mouse, SysGra& gra, Pers& pers, int& text_y )
//------------------------------------------------------------------------------
{
	static bool bGrid = true;
	if (bGrid==false)	gra.Clr(rgb(0.0,0.0,0.0));

	gra.Print(1,(float)text_y++,string("lab10_colors")+to_string(lab.idx)); 

	// 初期化
	if ( !lab.bInit )
	{
		lab.bInit = true;
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
	if ( keys.R.hi ) lab.bInit = false;
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
					pers.tri3d( gra, pers, v0, v1 ,v2 , col );
					pers.tri3d( gra, pers, v0 ,v2, v3 , col );

					// 右面
					pers.tri3d( gra, pers, w0, w1 ,w2 , col );
					pers.tri3d( gra, pers, w0 ,w2, w3 , col );

					// 先
					pers.tri3d( gra, pers, v1, v0, w0, col );
					pers.tri3d( gra, pers, w1, w0, v0, col );

					// 上面
					pers.tri3d( gra, pers, v0, v3 ,w2 , top );
					pers.tri3d( gra, pers, w2, w1 ,v0 , top );

					// 蓋
					pers.tri3d( gra, pers, w2, v3, vect3(0,y+s,0), top );
					
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
				pers.line3d( gra, pers, p0, q0, c1, wide );
				pers.line3d( gra, pers, p1, q1, c1, wide );
				pers.line3d( gra, pers, p2, q2, c1, wide );
				pers.line3d( gra, pers, p3, q3, c1, wide );

				pers.line3d( gra, pers, q0, q1, c1, wide );
				pers.line3d( gra, pers, q1, q2, c1, wide );
				pers.line3d( gra, pers, q2, q3, c1, wide );
				pers.line3d( gra, pers, q3, q0, c1, wide );
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
		
		//円柱
if(0)		{
			int i = 0;
			vect3 v1;
			for ( float th = 0 ; th < pi*2 ; th+=deg2rad(30) )
			{
				float r = 0.5;
				vect3 v0 =
				{
					r * cos( th ),
					3,
					r * sin( th )
				};
				
				if ( i > 0 )
				{
					pers.tri3d( gra, pers, v1, v0, vect3(0,v0.y,0), rgb(1,1,1)/10 );
				}
				
//				pers.pset3d( gra, pers, v0, rgb(1,1,1)/10, 10 );
				v1 = v0;
				i++;
			}
		}		
		
	}
}
//------------------------------------------------------------------------------
static void lab9_2dRidge( Lab& lab, SysKeys& keys, SysMouse& mouse, SysGra& gra, Pers& pers, int& text_y )
//------------------------------------------------------------------------------
{
	static	Graphs graphs;

	gra.Print(1,(float)text_y++,string("lab9_2dRidge")+to_string(lab.idx)); 

	const float	G	= -9.80665;				// 重力加速度
	const float	T	= 1.0/60.0;				// 時間/frame
	const vect3	gv	= vect3(0,0, G*T*T);	// 重力加速度/frame
	static bool		bPause = false;
	static float time = 0;

	typedef float Float;
	static Float p0;// = car.pos.z;
	static Float v0;// = car.vel.z;
	bool bStep = false;
	struct Car:Obj
	{
		vect3	vel;	//	velocity 速度(m/frame)

		bool	bReq = false;
//		vect3	req_pos;
//		vect3	req_vel;

		Car( vect3 v, vect3 _vel ) : Obj(v)
		{
			pos = v;
			vel = _vel;
		}
	};

	// 初期化
	if ( !lab.bInit )
	{
		if ( !lab.bInitCam )
		{
			lab.bInitCam = true;
			pers.cam.pos = vect3(	0.0,	1.2, 0.1 );
			pers.cam.at = vect3( 	0.0,	0.0, 0.2 );
		}
	
		lab.bInit = true;
		//点
		for ( Obj* p : lab.tblObj ) delete p;
		lab.tblObj.clear();
		lab.tblObj.emplace_back( new Car(vect3(  0		, 0.0,  1.0 ), vect3(0,0,0)) );
//		lab.tblObj.emplace_back( new Obj(vect3( -0.5	, 0.1,	0.0 )) );
//		lab.tblObj.emplace_back( new Obj(vect3(  0.5	, 0.1,	0.0 )) );
		// 線
		for ( Edge* p : lab.tblEdge ) delete p;
		lab.tblEdge.clear();
//		lab.tblEdge.emplace_back( new Edge(0,1) );

		pers.axis.bAxisX = true;
		pers.axis.bAxisY = false;
		pers.axis.bAxisZ = true;

		graphs.Clear();
		graphs.Add( 0.02, rgb(0,0,1) );
		graphs.Add( 0.02, rgb(0,1,1) );
		graphs.Add( 0.02, rgb(1,1,0) );

		time = 0;

		p0 = lab.tblObj[0]->pos.z;
		v0 = 0;

		cout << endl;
	}

	// 入力
	{
		if ( keys.R.hi ) lab.bInit = false;
		if ( keys.SPACE.hi )	bPause = !bPause ;
		if ( keys.ENTER.rep )	{bStep = true; bPause = true; }
	}
	
	Car&	car = *dynamic_cast<Car*>(lab.tblObj[0]);
	vect3&	st = lab.tblObj[1]->pos;
	vect3&	en = lab.tblObj[2]->pos;

	// 計算チェック
	{
		static bool b = false;
		static Float prev = 0;
/*
		if ( car.pos.z < prev && b == false ) 
		{
			cout << "top : " << prev << endl;
			b = true;
		}
		if ( car.pos.z > prev ) b = false;
		prev = car.pos.z;
*/
		if ( p0 < prev && b == false ) 
		{
			cout << "top : " << prev << endl;
			b = true;
		}
		if ( p0 > prev ) b = false;
		prev = p0;
	}

	//-----

	time += T;


#if 1

	// 移動量s,初速v0の時間を求める。
	auto func_t =[]( Float g, Float s, Float v0 )
	{
		Float a = 0.5*g;
		Float c = v0/(2*a);
		Float t = std::sqrt( std::abs( s/a + c*c ) ) - c; 
		return t;
	};

	// 時間t,初速v0の移動量sを求める。
	auto func_s =[]( Float g, Float t, Float v0 )
	{
		Float a = 0.5*g;
		Float s = a*t*t + v0*t;
		return s;
	};

	// 時間t,移動量sの、速度を求める。
	auto func_v =[]( Float g, Float t, Float s )
	{
		Float a = 0.5*g;
		Float v = (s-a*t*t)/t;
		return v;
	};

	//初期値
//	Float p0 = car.pos.z;
//	Float v0 = car.vel.z;

	// 差分
	Float s1; 	// 移動量

	// 結果
	Float p1;
	Float v1;

	{
		// 計算
		{
			s1 = func_s( G, T, v0 );	// 移動距離 m

			p1 = p0 + s1;	// 仮想移動
			v1 = v0 + G*T;	// 仮想速度
		}

		// 衝突
		{
			if ( p1 <= 0 )
			{
			#if 0
				// 簡易実装
				p1 = p0;
				v1 = -v0;
			#elif 0
				// 簡易実装2
				Float s = 0-p0;					// 衝突までの距離(m)
				Float t = func_t( G, s, v0 );	// 衝突までの時間(s)
				Float v = (v0 + G*t);			// 衝突後の速度(m/s)

				p1 = p0 + s;	
				v1 = -v;
			#elif 1
				// １バウンド実装
				Float s = 0-p0;					// 衝突までの距離(m)
				Float t = func_t( G, s, v0 );	// 衝突までの時間(s)
				Float v = (v0 + G*t);			// 衝突後の速度(m/s)

				{
					Float t2 = T-t;					// 衝突後の残り時間(s)
					Float s2 = func_s( G, t2, -v );	// 衝突後の移動距離(m)
					Float v2 = (-v + G*t2);			// 衝突後の速度(m/s)
				
					p1 = p0 + s + s2;	
					v1 = v2;
				}

			#endif

			}
		}
		
		// 更新
//		if(time<1.0)
		if  ( !bPause || bStep )
		{
			p0 = p1;
			v0 = v1;
		}

		// 描画
		{
			drawVect( gra, pers, text_y, vect3(0,0.0,p0), vect3(0,0.0,s1)	,1		, rgb(1,0,0), "s1" );
		}
	}

	car.pos.z = p0;
	car.vel.z = v0;
#endif

#if 0
	float p0 = car.pos.z;	//初期値
	float v0 = car.vel.z;

	float p1;	//増分
	float v1;

	float p2;	//結果
	float v2;

	float p3;	//衝突
	float v3;

	{
		// 計算
		{
			v1 = G*T;				// 増分・速度/frame

			p1 = v0*T +v1*T*0.5;	// 増分・移動/frame

			p2 = p0 + p1;	// 仮想移動
			v2 = v0 + v1;	// 仮想速度
		}

		// 衝突
		{


			if ( p2<=0 )
			{
	float new_p0 = p0;
	float new_v0 = v0;
				p3 = 0;				// 衝突点の位置
				float s = p3-p0;	// 衝突点までの移動
				{

					float t = func_t( G, s, v0 );
//					float s = func_s( t, v0 );
					float v = func_v( G, t, s );

					float t0 = t; 
					{
						float t2 = T-t0;	// 衝突後残り時間
						float v0 = -v;		// 衝突後速度
						//--
						float s = func_s( G, t2, v0 );
						float v = func_v( G, t2, s );

						cout << " t : " << t  << "t+t2 : " << +t+t2 << " v : " << v << endl ;
new_p0 = s;
new_v0 = v;
						pers.pset3d( gra, pers, vect3(0,0.1,s), rgb(1,1,0), 7 );
					}
				}

				p2 = new_p0;
				v2 = new_v0;
//				pers.pset3d( gra, pers, vect3(0,0.1,p2), rgb(1,1,0), 7 );
			}
		}

		// 更新
		if  ( !bPause || bStep )
		{
			p0 = p2;
			v0 = v2;
		}

		// 描画
		{
			drawVect( gra, pers, text_y, vect3(0,0.1,p0), vect3(0,0.1,p1)	,1		, rgb(1,0,0), "p1" );
		}
	}
	car.pos.z = p0;
	car.vel.z = v0;
#endif

#if 0
	float p0 = car.pos.z;	//初期値
	float v0 = car.vel.z;

	float p1;	//増分
	float v1;

	float p2;	//結果
	float v2;

	float p3;	//衝突
	float v3;

	{
		// 計算
		{
			v1 = G*T;				// 増分・速度/frame

			p1 = v0*T +v1*T*0.5;	// 増分・移動/frame

			p2 = p0 + p1;	// 仮想移動
			v2 = v0 + v1;	// 仮想速度
		}

		// 衝突
		{
			if ( p2<=0 )
			{
				p3 = 0;				// 衝突点の位置
				float p = p3-p0;	// 衝突点までの移動

				// p = 0.5*G*t^2 + v0 * t

if(0)
{
	float t = 1.0;
	float p = G/2*t*t;

	cout << endl;
	cout << "p : "<< p << endl;

	{
		float t = sqrt( abs( 2 * p / G ) );
		cout << "t : "<< t << endl;

		float s = 0.5*G*t*t;
		cout << "s : "<< s << endl;
		
	}

	
}

if (1)
{

	p = -G/2;
	float v = -G/8;

		float a = 0.5*G;
		float b = sqrt(abs(a));
		float c = v/(2*b);


					float t = ( sqrt( abs( p - c*c ) ) + c )/b; 

	cout << endl;
	cout << "v : " << v << endl;			
	cout << "t : " << t << endl;			
	{
//		float s  = 0.5*G*t*t + v*t;
		float s  = a*t*t + v*t;
		float s2 = -pow( b*t - c, 2 ) + c*c;
//		float s2 = pow( b*t + c, 2 ) - c*c;
		cout << " s : " << s  << " p : " << p  << " s2 : "<< s2 << endl ;
	}

}


				p2 = p0;
				v2 = -v0;
				pers.pset3d( gra, pers, vect3(0,0.1,p2), rgb(1,1,0), 7 );
			}
		}

		// 更新
		if  ( !bPause || bStep )
		{
			p0 = p2;
			v0 = v2;
		}

		// 描画
		{
			drawVect( gra, pers, text_y, vect3(0,0.1,p0), vect3(0,0.1,p1)	,1		, rgb(1,0,0), "p1" );
		}
	}
	car.pos.z = p0;
	car.vel.z = v0;
#endif

#if 0
	vect3 p0 = car.pos;
	vect3 v0 = car.vel;
	vect3 p1;
	vect3 v1;
	vect3 p2;
	vect3 v2;
	{
		// 計算
		{
			v1 = vect3( 0, 0, G*T );	// 加速量/frame

			p1 = v0*T +v1*T*0.5;		// 移動量/frame

			p2 = p0 + p1;	// 仮想移動
			v2 = v0 + v1;	// 仮想速度
		}

		// 衝突
		{
			auto[b,d,q0,q1,s0,s1] = func_distance_Segline_Segline( p0, p2, st, en );

			if ( b )
			{
				p2 = p0;
				v2 = -v0;
				pers.pset3d( gra, pers, q0, rgb(1,0,0), 7 );
				pers.pset3d( gra, pers, p2, rgb(1,1,0), 7 );
			}
		}

		// 更新
		if  ( !bPause || bStep )
		{
			p0 = p2;
			v0 = v2;
		}

		// 描画
		{
			drawVect( gra, pers, text_y, p0, p1	,1		, rgb(1,0,0), "p1" );
		}
	}
	car.pos = p0;
	car.vel = v0;
#endif

	
#if 0
	// 計算
	{
		car.req_vel = car.vel + gv;	//	落下
		car.req_pos = car.pos + car.req_vel;
	}

	// 衝突
	{
		auto[b,d,q0,q1,s0,s1] = func_distance_Segline_Segline( car.pos, car.pos + car.req_vel, st, en );


		if ( b )
		{
			float a0 = car.req_vel.abs() * 2.0 / (T*T);	// 加速度
			float t0 = sqrt( 2.0 * s0 / a0 );			// 衝突までの時間
			float t2 = T-t0;							// 衝突後の時間

//			( car.pos + car.req_vel.normalize() * 0.5*a*t0*t0 ).dump("qx");
//			q0.dump("q0");

			float a2 = 1;							// 衝突後の時間

			float v2 = 0.5*a0*t0*t0;

			float s0 = 0.5*a0*t0*t0;	// s0と同じ

			float s2 = -v2+0.5*a2*t2*t2;

			float s3 = 0.5*a0*T*T;
#if 1
cout << endl;
cout << "s0 :" << s0    << endl;
cout << "a0 :" << a0    << endl;
cout << "t0 :" << t0    << endl;
//cout << "at :" << t0*g  << endl;
 #else
cout << "a0 :" << a0 << endl;
cout << "s2 :" << s2 << endl;
cout << "vel:" << car.req_vel.abs() << endl;
#endif
	
//			float v2 = 
	
			car.req_pos = q0 - car.req_vel;
			car.req_vel = -car.req_vel;
		}
	}

	// 移動
	if ( !bPause || bStep )
	{
//		graphs.Request( 2, car.req_pos.z- car.pos.z  );
//cout << car.req_pos.z- car.pos.z << endl;	
		car.pos = car.req_pos;
		car.vel = car.req_vel;

		graphs.Request( 0, car.vel.z );
		graphs.Request( 1, car.pos.z );
	}

	// 描画
	{
		graphs.Draw( gra, pers );

		drawVect( gra, pers, text_y, car.pos, car.vel	,1		, rgb(1,0,0), "vel" );
	}
#endif
}

//------------------------------------------------------------------------------
static void lab8_vector_six_lab8( Lab& lab, SysKeys& keys, SysMouse& mouse, SysGra& gra, Pers& pers, int& text_y )
//------------------------------------------------------------------------------
{
	gra.Print(1,(float)text_y++,string("lab8_vector_six_lab8")+to_string(lab.idx)); 

	const float	G = 9.80665;				// 重力加速度
	const float	T = 1.0/60.0;			// 時間/frame
	const float	g = G *T*T;			// 重力加速度/frame
	const vect3	gv = vect3(0,0, -g);	// 重力加速度/frame

	static	Graphs::Plot plot_moment( 0.02, rgb(1,0,1) );
	static vect3 vel;
	static bool		bPause = false;
	bool bStep = false;
	static float w = 0;

	// 初期化
	if ( !lab.bInit )
	{
		if ( !lab.bInitCam )
		{
			lab.bInitCam = true;
			pers.cam.pos = vect3(  0.0, 5.0, -1.0 );
			pers.cam.at = vect3( 0,  0.0, 0 );
		}
	
		lab.bInit = true;
		//点
		for ( Obj* p : lab.tblObj ) delete p;
		lab.tblObj.clear();
		lab.tblObj.emplace_back( new Obj(vect3( 0  , 0.1, 0 )) );
		lab.tblObj.emplace_back( new Obj(vect3( 1  , 0.1, 0 )) );
		// 線
		for ( Edge* p : lab.tblEdge ) delete p;
		lab.tblEdge.clear();
		lab.tblEdge.emplace_back( new Edge(0,1) );

		pers.axis.bAxisX = true;
		pers.axis.bAxisY = false;
		pers.axis.bAxisZ = true;

		vel=vect3(0,0,0);	// 加速度
		w = 0;

		plot_moment.Reset();
	}

	// 入力
	if ( keys.R.hi ) lab.bInit = false;
	if ( keys.SPACE.hi )	bPause = !bPause ;
	if ( keys.ENTER.rep )	{bStep = true; bPause = true; }

	vect3&	v0 = lab.tblObj[0]->pos;
	vect3&	v1 = lab.tblObj[1]->pos;

	vect3	bar = (v1-v0);					//	棒
	vect3	moment = cross(-bar, vel+gv);	//	回転モーメント 仮
	vect3	F = cross(bar, moment );		//	力

	// 計算
	if ( !bPause || bStep )
	{
		plot_moment.Write( moment.y*10 );

			
		// 衝突
		{
			vel = mrotateByAxis( moment, (vel+gv).abs() )*vel+gv;

			v1 += vel;

//			mat33 m = mrotateByAxis( moment, w );

//			v1 = m * (v1-v0) + v0;
//			w += F.abs();
//			vel += F;
		}
 		
	}

	drawVect( gra, pers, text_y, v0, moment ,10	, rgb(1,0,1), "moment" );
	drawVect( gra, pers, text_y, v1, gv		,100	, rgb(1,0,0), "gv" );
	drawVect( gra, pers, text_y, v1, F		,1		, rgb(0,1,0), "F" );
	plot_moment.Draw( gra, pers );
	gra.Print(1,(float)text_y++,string("<<radius>>")+to_string(bar.abs())); 


}

//------------------------------------------------------------------------------
static void lab6_tire3d( Lab& lab, SysKeys& keys, SysMouse& mouse, SysGra& gra, Pers& pers, int& text_y )
//------------------------------------------------------------------------------
{
	gra.Print(1,(float)text_y++,string("<<lab6_tire3d>>")+to_string(lab.idx)); 

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
	pers.tire.DrawTire( gra, pers, pos, head, bank, radius );
}

//------------------------------------------------------------------------------
static void lab5_furiko2d( Lab& lab, SysKeys& keys, SysMouse& mouse, SysGra& gra, Pers& pers, int& text_y )
//------------------------------------------------------------------------------
{
	gra.Print(1,(float)text_y++,string("<<lab5_furiko2d>>")+to_string(lab.idx)); 

	const float G = 9.80665;				// 重力加速度
	const float T = 1.0/60.0;			// 時間/frame
	const float grate = 9.80665 *T*T;		// 重力加速度/frame

	static float	rsp = 0;

	if ( !lab.bInit )
	{
		if ( !lab.bInitCam )
		{
			lab.bInitCam = true;
			pers.cam.pos = vect3(  0.0, 2.0, -5.0 );
			pers.cam.at = vect3( 0,  2.0, 0 );
		}
	
		lab.bInit = true;
		for ( Obj* p : lab.tblObj ) delete p;
		lab.tblObj.clear();
		lab.tblObj.emplace_back( new Obj(vect3(0, 2.0, 0)) );
		lab.tblObj.emplace_back( new Obj(vect3(1, 2.0, 0)) );
		// 線
		for ( Edge* p : lab.tblEdge ) delete p;
		lab.tblEdge.clear();
		lab.tblEdge.emplace_back( new Edge(0,1) );

		rsp=0;
	}
	if ( keys.R.hi )	lab.bInit = false;

	vect3&	v0 = lab.tblObj[0]->pos;	//	barの根本
	vect3&	v1 = lab.tblObj[1]->pos;	//	barの先端


//	pers.line3d( gra, pers, v0, v1, rgb(1,1,1), 2 );

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
static void lab7_kakusokudo7( Lab& lab, SysKeys& keys, SysMouse& mouse, SysGra& gra, Pers& pers, int& text_y )
//------------------------------------------------------------------------------
{
	gra.Print(1,(float)text_y++,string("<<lab7_kakusokudo7>>")+to_string(lab.idx)); 

	const float	G = 9.80665;			// 重力加速度
	const float	T = 1.0/60.0;		// 時間/frame
	const float	g = G *T*T;		// 重力加速度/frame
	const vect3	gv = vect3(0,0, -g);		// 重力加速度/frame

	static bool		bShot = false;
	static vect3	velocity;
	static vect3	moment;
	static vect3	to;
	static vect3	add;
	
	static float 	w;	//	角速度
	static bool		bPause = false;
	bool bStep = false;

	// 初期化
	if ( !lab.bInit )
	{
		if ( !lab.bInitCam )
		{
			lab.bInitCam = true;
			pers.cam.pos = vect3( 0, 5.0, -0.5 );
			pers.cam.at = vect3( 0,  0, 0 );
		}
		lab.bInit = true;

		// 点
		for ( Obj* p : lab.tblObj ) delete p;
		lab.tblObj.clear();
		lab.tblObj.emplace_back( new Obj(vect3( 0 ,0.1, 0)) );
		lab.tblObj.emplace_back( new Obj(vect3(-1 ,0.1, 0)) );

		// 線
		for ( Edge* p : lab.tblEdge ) delete p;
		lab.tblEdge.clear();
		lab.tblEdge.emplace_back( new Edge(0,1) );

		bShot = false;
	}


	vect3&	v0 = lab.tblObj[0]->pos;	//	barの根本
	vect3&	v1 = lab.tblObj[1]->pos;	//	barの先端


	//入力
	{
		// リセット
		if ( keys.R.hi )		lab.bInit = false ;

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
			acc2+=gv;

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

			vect3 moment = cross(-bar,gv);				//	回転モーメント

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
			drawVect( gra, pers, text_y, v1, vt			,100	, rgb(1,0,0), "vt" );
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
			moment = cross(-bar,gv);						//	回転モーメント
			velocity = cross(bar/radius, moment/radius );	//	ベロシティ

			w = velocity.abs()/radius;
			to = mrotateByAxis( moment, w ) * bar;			//	移動計算

			vect3 add  = to - bar;

		}

		// 補助線
		drawVect( gra, pers, text_y, v1, gv			,100	, rgb(1,0,0), "gv" );
			
	}

}

//------------------------------------------------------------------------------
static void lab4_furiko3d( Lab& lab, SysKeys& keys, SysMouse& mouse, SysGra& gra, Pers& pers, int& text_y )
//------------------------------------------------------------------------------
{
		gra.Print(1,(float)text_y++,string("<<lab4_furiko3d>>")+to_string(lab.idx)); 
		// T=J dω/dt
		// T：トルク J：慣性モーメント ω：回転角速度[rad]
		//
		// t=rF
		// r:位置ベクトル t:力のモーメント F:力
		//
		// L=rp
		// r:位置ベクトル L:角運動量(運動量のモーメント) p:運動量

	const float G = 9.80665;			// 重力加速度
	const float T = 1.0/60.0;		// 時間/frame
	const float g = G *T*T;		// 重力加速度/frame
	const vect3 vg = vect3(0,-g,0);	// 重力加速度ベクトル/frame

	static vect3	vel;			// 運動量
	static vect3	mov;			// 運動量
	static bool	bPause = false; 
	bool	bStep = false; 


	// 初期化
	if ( !lab.bInit )
	{
		if ( !lab.bInitCam )
		{
			lab.bInitCam = true;
			pers.cam.pos = vect3(  0.0, 0.0, -7.0 );
			pers.cam.at = vect3( 0,  0.0, 0 );
		}

		lab.bInit = true;
		for ( Obj* p : lab.tblObj ) delete p;
		lab.tblObj.clear();
		lab.tblObj.emplace_back( new Obj(vect3(0, 2.0, 0)) );
		lab.tblObj.emplace_back( new Obj(vect3(-1, 2.0, 0)) );
		
		vel = vect3(0,0,0);
	}

	vect3&	v0 = lab.tblObj[0]->pos;	//	barの根本
	vect3&	v1 = lab.tblObj[1]->pos;	//	barの先端

	// 入力
	{
		// ポーズ
		if ( keys.SPACE.hi )	bPause = !bPause;

		// ステップ再生
		if ( keys.ENTER.rep )	bStep = true;

		// リセット
		if ( keys.R.hi )	lab.bInit = false ;

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
		gra.Clr(rgb(0.3,0.3,0.3));
		pers.grid.DrawGrid3d( gra, pers, vect3(0,0,0), mrotx(deg2rad(90)), 100, 100, 1, vect3(0.2,0.2,0.2) );

		pers.line3d( gra, pers, v0, v1, rgb(1,1,1), 2 );

		drawVect( gra, pers, text_y, v1, vg	,100, rgb(1,0,0), "g" );
		drawVect( gra, pers, text_y, v0, moment,100, rgb(1,0,1), "moment" );
		drawVect( gra, pers, text_y, v1, F		,100, rgb(0,1,0), "F" );
		drawVect( gra, pers, text_y, v1, vel	,2	, rgb(1,1,0), "vel" );
		drawVect( gra, pers, text_y, v1, mov	,2	, rgb(0,0,1), "mov" );
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

			pers.line3d( gra, pers, v1, v1+(a-v)*10, rgb(1,0,0),2 );
			
		}

		pers.line3d( gra, pers, v0, v1, rgb(1,1,1), 2 );

		// 加速ベクトルの実験
		{
			vect3 vg = vect3(0,-g,0);	//	重力加速度ベクトル
			vect3 n0 = cross( v, vg );
			vect3 vt = cross( n0, v );

//						pers.line3d( gra, pers, v1, v1+vt*100, rgb(0,1,0) );
			vect3 v2 = v*1.05+v0;
//						pers.line3d( gra, pers, v2, v2+vv, rgb(0,1,0) );

//						pers.line3d( gra, pers, v2, v2+vg*100, rgb(1,0,0) );
			pers.line3d( gra, pers, v2, v2+vt*100, rgb(0,1,0) );
			pers.line3d( gra, pers, v2, v2+n0*100, rgb(0,0,1) );					}

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

//					pers.line3d( gra, pers,  0,  vv*10, rgb(0,1,1), 2 );
		pers.line3d( gra, pers,  vect3(1,0,0),  vect3(1,0,0)+vt*100, rgb(0,1,0), 1 );

		pers.line3d( gra, pers, v2, v2+vt*100, rgb(0,1,0) );
		pers.line3d( gra, pers, v0, v2, rgb(0,1,1), 2 );

	}
#endif

	
}

//=================================
// 描画	引力実験
//=================================

//------------------------------------------------------------------------------
static void lab3_gravityPlanet( Lab& lab, SysKeys& keys, SysMouse& mouse, SysGra& gra, Pers& pers, int& text_y )
//------------------------------------------------------------------------------
{
	gra.Print(1,(float)text_y++,string("<<lab3_gravityPlanet>>")+to_string(lab.idx)); 

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
	if ( !lab.bInit )
	{
		lab.bInit = true;
		if ( !lab.bInitCam )
		{
			lab.bInitCam = true;
			//上から
			pers.cam.pos = vect3(  0.0, 0.5, -2.0 );
			pers.cam.at = vect3( 0,  0.0, 0 );
		}

		for ( Obj* p : lab.tblObj ) delete p;
		lab.tblObj.clear();
		lab.tblObj.emplace_back( new Planet(vect3(-0.5,0.1,0),vect3(0, 0, -0.02)) );
		lab.tblObj.emplace_back( new Planet(vect3( 0.5,0.1,0),vect3(0, 0, 0.02)) );
	}

	const float	G = 9.80665;			// 重力加速度
	const float	T = 1.0/60.0;		// 時間/frame
	const float	g = G *T*T;		// 重力加速度/frame

	// 入力
	{
		// ポーズ
		if ( keys.SPACE.hi )	bPause = !bPause;

		// ステップ再生
		if ( keys.ENTER.rep )	bStep = true;

		// リセット
		if ( keys.R.hi )	lab.bInit = false ;

		// 生成
		if ( mouse.B.hi )
		{
			vect3 P = pers.calcScreenToWorld3( vect3(mouse.pos,0) );
			vect3 I = pers.calcRayvect( P );
			lab.tblObj.emplace_back( new Planet( P, I/100.0 ) );
		}

	}

	
	Planet& pl0 = *dynamic_cast<Planet*>(lab.tblObj[0]);	//	太陽
	Planet& pl1 = *dynamic_cast<Planet*>(lab.tblObj[1]);	//	地球

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
		for ( Obj* p : lab.tblObj ) 
		{
			Planet& pl = *dynamic_cast<Planet*>(p);
			for ( int i = 0 ; i < MAX_PREV ; i++ )
			{
				pers.pset3d( gra, pers, pl.tblPrev[i], rgb(0,1,1),2 );
			}
		}
	}

}
		
//=================================
// 描画	角速度 実験
//=================================
//------------------------------------------------------------------------------
static void lab2_kakusokudo( Lab& lab, SysKeys& keys, SysMouse& mouse, SysGra& gra, Pers& pers, int& text_y )
//------------------------------------------------------------------------------
{
	gra.Print(1,(float)text_y++,string("<<lab2_kakusokudo>>")+to_string(lab.idx)); 

	const float	G = 9.80665;			// 重力加速度
	const float	T = 1.0/60.0;		// 時間/frame
	const float	g = G *T*T;		// 重力加速度/frame

	static bool bShot = false;
	static vect3	velocity;
	static vect3	moment;
	static vect3	to;
	static vect3	add;
	
	static float 	w;	//	角速度

	// 初期化
	if ( !lab.bInit )
	{
		if ( !lab.bInitCam )
		{
			lab.bInitCam = true;
			pers.cam.pos = vect3( 2.0, 2.5, -2.0 );
			pers.cam.at = vect3( 0,  0.0, 0 );
		}
		lab.bInit = true;

		for ( Obj* p : lab.tblObj ) delete p;
		lab.tblObj.clear();
		lab.tblObj.emplace_back( new Obj(vect3(0  ,0.1,0)) );
		lab.tblObj.emplace_back( new Obj(vect3(1  ,0.1,0)) );
		lab.tblObj.emplace_back( new Obj(vect3(1.5,0.1,0.2)) );
		// 線
		for ( Edge* p : lab.tblEdge ) delete p;
		lab.tblEdge.clear();
		lab.tblEdge.emplace_back( new Edge(0,1) );
		lab.tblEdge.emplace_back( new Edge(1,2,rgb(0,1,0),1) );

		bShot = false;
	}


	vect3&	v0 = lab.tblObj[0]->pos;	//	barの根本
	vect3&	v1 = lab.tblObj[1]->pos;	//	barの先端
	vect3&	v2 = lab.tblObj[2]->pos;	// 速度指定


	//入力
	{
		// リセット
		if ( keys.R.hi )	lab.bInit = false ;

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
			pers.line3d( gra, pers, va, vb, rgb(1,1,1)/4, 2 );
		}
		{// 影 描画
			vect3	va = v1;va.y = 0;
			vect3	vb = v2;vb.y = 0;
			pers.line3d( gra, pers, va, vb, rgb(1,1,1)/4, 2 );
		}

		pers.line3d( gra, pers, v0, v1, rgb(1,1,1), 2 );	//	棒
		pers.line3d( gra, pers, v1, v2, rgb(0,1,0), 1 );	// 外的な力
*/
		drawVect( gra, pers, text_y, v1, velocity	,1	, rgb(1,1,0), "velocity" );
		drawVect( gra, pers, text_y, v0, moment		,1	, rgb(1,0,1), "moment" );
		drawVect( gra, pers, text_y, v0, to			,1	, rgb(0,1,1), "to" );
			
	}

}

//------------------------------------------------------------------------------
static void lab1_graph( Lab& lab, SysKeys& keys, SysMouse& mouse, SysGra& gra, Pers& pers, int& text_y )
//------------------------------------------------------------------------------
{
	gra.Clr(rgb(0.3,0.3,0.3));
	pers.grid.DrawGrid3d( gra, pers, vect3(0,0,0), mrotx(deg2rad(90)), 100, 100, 1, vect3(0.2,0.2,0.2) );

	gra.Print(1,(float)text_y++,string("<<lab1_graph>>")+to_string(lab.idx)); 

	rgb	col = vect3(0.2,0.2,0.2);

	// 初期化
	if ( !lab.bInit )
	{
		if ( !lab.bInitCam )
		{
			lab.bInitCam = true;
			pers.cam.pos = vect3( 0.0, 0.0, -5.0 );
			pers.cam.at = vect3( 0,  0.0, 0 );
		}
		lab.bInit = true;
	}


	//入力
	{
		// リセット
		if ( keys.R.hi )	lab.bInit = false ;
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

			pers.pset2d( gra, pers, vect2(t,s), rgb(1,0,0), 3 );
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

			pers.line2d( gra, pers, vect2(t0,s0), vect2(t,s), rgb(0,0.5,1), 1 );
			pers.line2d( gra, pers, vect2(t0,v0), vect2(t,v), rgb(0,1,0), 1 );

			pers.pset2d( gra, pers, vect2(t,s), rgb(0,0.5,1), 3 );
			pers.pset2d( gra, pers, vect2(t,v), rgb(0,1,0), 3 );
			pers.print2d( gra, pers, vect2(t,s),0,0, to_string(s) );
		
			t0 = t;
			v0 = v;
			s0 = s;


		}
	}

}

//------------------------------------------------------------------------------
void Lab::Update( SysKeys& keys, SysMouse& mouse, SysGra& gra, Pers& pers, int& text_y )
//------------------------------------------------------------------------------
{
	switch( idx )
	{
		case 10:	// 描画	色見本
			lab10_colors( (*this), keys, mouse, gra, pers, text_y );
			break;

		case 9:	// 描画	2d剛体
			lab9_2dRidge( (*this), keys, mouse, gra, pers, text_y );
			break;

		case 8:	// 描画	位相空間
			lab8_vector_six_lab8( (*this), keys, mouse, gra, pers, text_y );
			break;

		case 1:	// 描画	グラフ実験
			lab1_graph( (*this), keys, mouse, gra, pers, text_y );
			break;

		case 2:// 描画	角速度 実験
			lab2_kakusokudo( (*this), keys, mouse, gra, pers, text_y );
			break;

		case 7:// 描画	角速度 実験
			lab7_kakusokudo7( (*this), keys, mouse, gra, pers, text_y );
			break;

		case 3:	// 描画	引力実験
			lab3_gravityPlanet( (*this), keys, mouse, gra, pers, text_y );
			break;

		case 4:	// 描画	振り子3D実験
			lab4_furiko3d( (*this), keys, mouse, gra, pers, text_y );
			break;

		case 5:	// 描画	振り子2D実験
			lab5_furiko2d( (*this), keys, mouse, gra, pers, text_y );
			break;

		case 6:	// 描画	タイヤ実験実験
			lab6_tire3d( (*this), keys, mouse, gra, pers, text_y );
			break;

		default:break;
	}
//	gra.Print(1,(float)text_y++,string("lab: ")+to_string(idx)); 
}

//------------------------------------------------------------------------------
Lab::Lab()
//------------------------------------------------------------------------------
{
	idx = 9;
}

