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
//------------------------------------------------------------------------------
void Lab::tire3d( SysKeys& keys, SysMouse& mouse, SysGra& gra, Pers& pers )
//------------------------------------------------------------------------------
{
	const vect3 G_pos = vect3(0,2.0,0);
	const vect3 G_acc= vect3(0,0,0.0);
	const float G_radius = 0.5;
	const float G_head = deg2rad(35);	//	タイヤの方向
	const float G_bank = deg2rad(40);	//	回転角
	const float G_rspd = deg2rad(0);	//	角速度
	const float G = 9.8;				// 重力加速度
	const float grate = 9.8 / 60/60;	// 重力加速度

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
//						acc.y = -acc.y * 0.5; 
//					acc.y = 0;
#if 0
		// 回転
		{
			float a = atan( acc.y * sin(bank) / radius );
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
void Lab::furiko2d( SysKeys& keys, SysMouse& mouse, SysGra& gra, Pers& pers )
//------------------------------------------------------------------------------
{
	const float G = 9.8;				// 重力加速度
	const float T = 1.0/60.0;			// 時間/frame
	const float grate = 9.8 *T*T;		// 重力加速度/frame

	static float	rsp = 0;

	static bool bInit = false;
	if ( !bInit )
	{
		pers.cam.pos = vect3(  0.0, 2.0, -5.0 );
		pers.cam.at = vect3( 0,  2.0, 0 );

		bInit = true;
		tblObj.clear();
		tblObj.emplace_back( new Obj(vect3(0, 2.0, 0)) );
		tblObj.emplace_back( new Obj(vect3(1, 2.0, 0)) );

		rsp=0;
	}
	if ( keys.R.hi )	bInit = false;

	vect3&	v0 = tblObj[0]->pos;	//	barの根本
	vect3&	v1 = tblObj[1]->pos;	//	barの先端


	pers.line3d( gra, pers, v0, v1, rgb(1,1,1), 2 );

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
void Lab::furiko3d( SysKeys& keys, SysMouse& mouse, SysGra& gra, Pers& pers )
//------------------------------------------------------------------------------
{
	const float G = 9.8;			// 重力加速度
	const float T = 1.0/60.0;		// 時間/frame
	const float g = 9.8 *T*T;		// 重力加速度/frame
	const vect3 vg = vect3(0,-g,0);		// 重力加速度/frame

	static vect3	acc;

	static bool bInit = false;
	if ( !bInit )
	{
		pers.cam.pos = vect3(  0.0, 0.0, -7.0 );
		pers.cam.at = vect3( 0,  0.0, 0 );

		bInit = true;
		tblObj.clear();
		tblObj.emplace_back( new Obj(vect3(0, 2.0, 0)) );
		tblObj.emplace_back( new Obj(vect3(1, 2.0, 0)) );
		
		acc = 0;
	}

	gra.Clr(rgb(0.3,0.3,0.3));
	pers.grid.DrawGrid3d( gra, pers, vect3(0,0,0), mrotx(deg2rad(90)), 100, 100, 1, vect3(0.2,0.2,0.2) );


	vect3&	v0 = tblObj[0]->pos;	//	barの根本
	vect3&	v1 = tblObj[1]->pos;	//	barの先端

	auto drawVect = [&]( vect3 v0, vect3 v, rgb col, string str )
	{
		pers.line3d( gra, pers, v0, v0+v, col, 1 );
		pers.pset3d( gra, pers,     v0+v, col, 5 );
		pers.print3d( gra, pers, v0+v, 0,0, str ); 

	};

	// 角度リセット
	if ( keys.R.hi )	bInit = false ;

	// 縮む
	if ( mouse.F.hi )	v1 = (v1+v0)/2;

	// 伸びる
	if ( mouse.B.hi )	v1 = (v1-v0)*2+v0;


#if 1
		vect3	bar = (v1-v0);									//	棒
		float	radius = bar.abs();								//	棒長さ
		vect3	moment = cross(-bar,vg);						//	回転モーメント
		vect3	velocity = cross(bar/radius, moment/radius );	//	ベロシティ

	 	float	th = velocity.abs()/radius;						//	角速度
//		v1 = mrotateByAxis( moment, th ) * v1;			//	移動計算
v1 += velocity;
		pers.line3d( gra, pers, v0, v1, rgb(1,1,1), 2 );


		drawVect( v0, moment*100, rgb(1,0,1), "moment" );
		drawVect( v1, velocity*100, rgb(0,1,0), "velocity" );
		drawVect( v1, vg*100, rgb(1,0,0), "g" );


#endif


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
void Lab::gravityPlanet( SysKeys& keys, SysMouse& mouse, SysGra& gra, Pers& pers )
//------------------------------------------------------------------------------
{
	#define MAX_PLANET 300
	struct Planet:Obj
	{
		 vect3	spd;
		 vect3	tblPlanet[MAX_PLANET];
		 int	cntPlanet=0;;

		Planet( vect3 v, vect3 _spd ) 
		{
			pos = v;
			spd = _spd;
		}
	};

	static bool bInit = false;
	if ( !bInit )
	{
		bInit = true;
		//上から
		pers.cam.pos = vect3(  0.0, 0.5, -2.0 );
		pers.cam.at = vect3( 0,  0.0, 0 );

		tblObj.clear();
		tblObj.emplace_back( new Planet(vect3(-0.5,0.1,0),vect3(0, 0, -0.02)) );
		tblObj.emplace_back( new Planet(vect3( 0.5,0.1,0),vect3(0, 0, 0.02)) );

	}

	const float	G = 9.8;			// 重力加速度
	const float	T = 1.0/60.0;		// 時間/frame
	const float	g = 9.8 *T*T;		// 重力加速度/frame

	function<void(Planet&, Planet&)> func = [&]( Planet& pl0, Planet& pl1 )
	{
		float 	dis = (pl0.pos-pl1.pos).abs();				// 距離
		vect3 	dir = (pl0.pos-pl1.pos).normalize();		// 方向

		if ( dis < 0.01 ) dis = 0.01;
		
		pl1.spd = (pl1.spd + dir/dis/1000);

		pl1.pos += pl1.spd; 
	
		pl1.tblPlanet[ pl1.cntPlanet++ ] = pl1.pos;	
		if ( pl1.cntPlanet >= MAX_PLANET ) pl1.cntPlanet = 0;

		for ( int i = 0 ; i < MAX_PLANET ; i++ )
		{
			pers.pset3d( gra, pers, pl1.tblPlanet[i], rgb(0,1,1),2 );
		}
	};
	
	Planet& pl0 = *dynamic_cast<Planet*>((*this).tblObj[0]);	//	太陽
	Planet& pl1 = *dynamic_cast<Planet*>((*this).tblObj[1]);	//	地球

	func( pl0, pl1 );
	func( pl1, pl0 );

	for ( int i = 1 ; i < (signed)tblObj.size() ; i++ )
	{
		Planet& pl1 = *dynamic_cast<Planet*>((*this).tblObj[i]);
//					func( pl0, pl1 );
	}

//				pers.line3d( gra, pers, pl1.pos, pl2.pos, rgb(0,1,1),1 );

	// 角度リセット
	if ( keys.R.hi )	bInit = false ;


	if ( keys.SPACE.hi )
	{
		vect3 P = pers.calcScreenToWorld3( vect3(mouse.pos,0) );
		vect3 I = pers.calcRayvect( P );
		tblObj.emplace_back( new Planet( P, I/100.0 ) );
	}

}
		
//=================================
// 描画	角速度 実験
//=================================
//------------------------------------------------------------------------------
void Lab::kakusokudo( SysKeys& keys, SysMouse& mouse, SysGra& gra, Pers& pers )
//------------------------------------------------------------------------------
{
	static bool bInit = false;
	if ( !bInit )
	{
		bInit = true;
		tblObj.clear();
		tblObj.emplace_back( new Obj(vect3(0,0.1,0)) );
		tblObj.emplace_back( new Obj(vect3(1,0.1,0)) );
		tblObj.emplace_back( new Obj(vect3(1,0.1,0.4)) );
	}

	const float	G = 9.8;			// 重力加速度
	const float	T = 1.0/60.0;		// 時間/frame
	const float	g = 9.8 *T*T;		// 重力加速度/frame

	vect3&	v0 = (*this).tblObj[0]->pos;	//	barの根本
	vect3&	v1 = (*this).tblObj[1]->pos;	//	barの先端
	vect3&	v2 = (*this).tblObj[2]->pos;	// 速度指定

	// 角度リセット
	if ( keys.R.hi )	bInit = false ;

	{
		vect3	bar = (v1-v0);									//	棒
		float	radius = bar.abs();
		vect3	moment = cross(-bar,v2-v1);						//	回転モーメント
		vect3	velocity = cross(bar/radius, moment/radius );	//	ベロシティ

	 	float	th = velocity.abs()/radius;						//	角速度
		vect3	to = mrotateByAxis( moment, th ) * bar;			//	移動計算

		{// 影 描画
			vect3	va = v0;;va.y = 0;
			vect3	vb = v0+to;vb.y = 0;
			pers.line3d( gra, pers, va, vb, rgb(1,1,1)/4, 2 );
			va = v0;va.y = 0;
			vb = v0+moment;vb.y = 0;
			pers.line3d( gra, pers, va, vb, rgb(1,1,1)/4, 2 );
			va = v1;va.y = 0;
			vb = v1+velocity;vb.y = 0;
			pers.line3d( gra, pers, va, vb, rgb(1,1,1)/4, 2 );
			va = v0;va.y = 0;
			vb = v1;vb.y = 0;
			pers.line3d( gra, pers, va, vb, rgb(1,1,1)/4, 2 );
		}
		pers.line3d( gra, pers, v0, v1, rgb(1,1,1), 2 );	//	棒
		pers.line3d( gra, pers, v1, v2, rgb(1,1,0), 1 );	// 外的な力
		pers.line3d( gra, pers, v0, v0+to, rgb(0,1,1), 1 );	// 回転先ベクトル
		pers.line3d( gra, pers, v0, v0+moment, rgb(1,0,1), 1 );	// モーメント/回転軸
		pers.line3d( gra, pers, v1, v1+velocity, rgb(0,1,0), 1 );	// 移動ベクトル

		pers.pset3d( gra, pers, v1+velocity, rgb(0,1,0),5 );pers.print3d( gra, pers, v1+velocity, 0,0, "velocity" ); 
		pers.pset3d( gra, pers, v0+moment, rgb(1,0,1),5 );pers.print3d( gra, pers, v0+moment, 0,0, "moment" ); 
		pers.pset3d( gra, pers, v0+to  , rgb(0,1,1),5 );pers.print3d( gra, pers, v0+to  , 0,0, "to" ); 
			
	}

}

//------------------------------------------------------------------------------
void Lab::graph( SysKeys& keys, SysMouse& mouse, SysGra& gra, Pers& pers )
//------------------------------------------------------------------------------
{

	rgb	col = vect3(0.2,0.2,0.2);

	gra.Clr(rgb(0.3,0.3,0.3));
	pers.grid.DrawGrid3d( gra, pers, vect3(0,0,0), mrotx(deg2rad(90)), 100, 100, 1, vect3(0.2,0.2,0.2) );


	float n = 10;
	{
		float dt = 0.2;
		float a = 1*dt*dt;
		float v = 0;
		float s = 0;

		for ( float t = 0 ; t < n ; t+=dt )
		{
			v += a;
			s += v;
		}
	}

	{
		float a = 0.5;
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
