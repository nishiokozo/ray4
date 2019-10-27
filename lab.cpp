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

struct Plot
{
	static const int MaxPlot = 100;
	float tblPlot[MaxPlot];
	int cntPlot = 0;
	bool	bScroll = false;
	rgb col;
	float step;
	
	//------------------------------------------------------------------------------
	Plot( float _step, rgb _col = rgb(1,1,1) )
	//------------------------------------------------------------------------------
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
	void Update( float val )
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
			pers.pset3d( gra, pers, vect3( x, 0, tblPlot[cntPlot-1] ),  rgb(1,1,1), 2 );

		gra.SetZTest( true );
	}

};

//------------------------------------------------------------------------------
void drawVect( SysGra& gra, Pers& pers, int& text_y, vect3 v0, vect3 v, float sc, rgb col, string str )
//------------------------------------------------------------------------------
{
	vect3 v1 = v0+v*sc;

	// 影
	{
		vect3	a = v0;	a.y=0;
		vect3	b = v1;	b.y=0;
		rgb		c = (col+rgb(1,1,1))/4;
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
Lab::Lab()
//------------------------------------------------------------------------------
{
	idx = 8;
}

//------------------------------------------------------------------------------
void Lab::Update( SysKeys& keys, SysMouse& mouse, SysGra& gra, Pers& pers, int& text_y )
//------------------------------------------------------------------------------
{
	switch( idx )
	{
		case 8:	// 描画	位相空間
			vector_six_lab8( keys, mouse, gra, pers, text_y );
			break;

		case 1:	// 描画	グラフ実験
			graph( keys, mouse, gra, pers, text_y );
			break;

		case 2:// 描画	角速度 実験
			kakusokudo( keys, mouse, gra, pers, text_y );
			break;

		case 7:// 描画	角速度 実験
			kakusokudo7( keys, mouse, gra, pers, text_y );
			break;

		case 3:	// 描画	引力実験
			gravityPlanet( keys, mouse, gra, pers, text_y );
			break;

		case 4:	// 描画	振り子3D実験
			furiko3d( keys, mouse, gra, pers, text_y );
			break;

		case 5:	// 描画	振り子2D実験
			furiko2d( keys, mouse, gra, pers, text_y );
			break;

		case 6:	// 描画	タイヤ実験実験
			tire3d( keys, mouse, gra, pers, text_y );
			break;

		default:break;
	}
	gra.Print(1,(float)text_y++,string("lab: ")+to_string(idx)); 
}

//------------------------------------------------------------------------------
void Lab::vector_six_lab8( SysKeys& keys, SysMouse& mouse, SysGra& gra, Pers& pers, int& text_y )
//------------------------------------------------------------------------------
{
	gra.Print(1,(float)text_y++,string("vector_six_lab8")+to_string(idx)); 

	const float	G = 9.8;			// 重力加速度
	const float	T = 1.0/60.0;		// 時間/frame
	const float	g = 9.8 *T*T;		// 重力加速度/frame
	const vect3	gv = vect3(0,0, -g);		// 重力加速度/frame

	static	Plot plot_moment( 0.02, rgb(1,0,1) );
	static vect3 acc;
	static bool		bPause = false;
	bool bStep = false;
	static float w = 0;

	// 初期化
	if ( !bInit )
	{
		if ( !bInitCam )
		{
			bInitCam = true;
			pers.cam.pos = vect3(  0.0, 5.0, -1.0 );
			pers.cam.at = vect3( 0,  0.0, 0 );
		}
	
		bInit = true;
		//点
		for ( Obj* p : (*this).tblObj ) delete p;
		tblObj.clear();
		tblObj.emplace_back( new Obj(vect3( 0  , 0.1, 0 )) );
		tblObj.emplace_back( new Obj(vect3( 1  , 0.1, 0 )) );
		// 線
		for ( Edge* p : (*this).tblEdge ) delete p;
		tblEdge.clear();
		tblEdge.emplace_back( new Edge(0,1) );

		pers.axis.bAxisX = true;
		pers.axis.bAxisY = false;
		pers.axis.bAxisZ = true;

		acc=0;	// 加速度
		w = 0;

		plot_moment.Reset();
	}

	// 入力
	if ( keys.R.hi ) bInit = false;
	if ( keys.SPACE.hi )	bPause = !bPause ;
	if ( keys.ENTER.rep )	{bStep = true; bPause = true; }

	vect3&	v0 = tblObj[0]->pos;
	vect3&	v1 = tblObj[1]->pos;

	vect3	bar = (v1-v0);				//	棒
	vect3	moment = cross(-bar, gv);	//	回転モーメント
	vect3	F = cross(bar, moment );	//	力

	// 計算
	if ( !bPause || bStep )
	{
		plot_moment.Update( moment.y*100 );

			
		// 衝突
		{
			mat33 m = mrotateByAxis( moment, w );

			v1 = m * (v1-v0) + v0;
			w += F.abs();
		}
 		
	}

	drawVect( gra, pers, text_y, v0, moment ,100	, rgb(1,0,1), "moment" );
	drawVect( gra, pers, text_y, v1, gv		,100	, rgb(1,0,0), "gv" );
	drawVect( gra, pers, text_y, v1, F		,100	, rgb(1,0,0), "F" );
	plot_moment.Draw( gra, pers );
	gra.Print(1,(float)text_y++,string("<<radius>>")+to_string(bar.abs())); 


}

//------------------------------------------------------------------------------
void Lab::tire3d( SysKeys& keys, SysMouse& mouse, SysGra& gra, Pers& pers, int& text_y )
//------------------------------------------------------------------------------
{
	gra.Print(1,(float)text_y++,string("<<tire3d>>")+to_string(idx)); 

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
void Lab::furiko2d( SysKeys& keys, SysMouse& mouse, SysGra& gra, Pers& pers, int& text_y )
//------------------------------------------------------------------------------
{
	gra.Print(1,(float)text_y++,string("<<furiko2d>>")+to_string(idx)); 

	const float G = 9.8;				// 重力加速度
	const float T = 1.0/60.0;			// 時間/frame
	const float grate = 9.8 *T*T;		// 重力加速度/frame

	static float	rsp = 0;

	if ( !bInit )
	{
		if ( !bInitCam )
		{
			bInitCam = true;
			pers.cam.pos = vect3(  0.0, 2.0, -5.0 );
			pers.cam.at = vect3( 0,  2.0, 0 );
		}
	
		bInit = true;
		for ( Obj* p : (*this).tblObj ) delete p;
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
void Lab::kakusokudo7( SysKeys& keys, SysMouse& mouse, SysGra& gra, Pers& pers, int& text_y )
//------------------------------------------------------------------------------
{
	gra.Print(1,(float)text_y++,string("<<kakusokudo7>>")+to_string(idx)); 

	const float	G = 9.8;			// 重力加速度
	const float	T = 1.0/60.0;		// 時間/frame
	const float	g = 9.8 *T*T;		// 重力加速度/frame
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
	if ( !bInit )
	{
		if ( !bInitCam )
		{
			bInitCam = true;
			pers.cam.pos = vect3( 0, 5.0, -0.5 );
			pers.cam.at = vect3( 0,  0, 0 );
		}
		bInit = true;

		// 点
		for ( Obj* p : (*this).tblObj ) delete p;
		tblObj.clear();
		tblObj.emplace_back( new Obj(vect3( 0 ,0.1, 0)) );
		tblObj.emplace_back( new Obj(vect3(-1 ,0.1, 0)) );

		// 線
		for ( Edge* p : (*this).tblEdge ) delete p;
		tblEdge.clear();
		tblEdge.emplace_back( new Edge(0,1) );

		bShot = false;
	}


	vect3&	v0 = (*this).tblObj[0]->pos;	//	barの根本
	vect3&	v1 = (*this).tblObj[1]->pos;	//	barの先端


	//入力
	{
		// リセット
		if ( keys.R.hi )		bInit = false ;

		if ( keys.SPACE.hi )	bPause = !bPause ;

		if ( keys.ENTER.hi )	{bStep = true; bPause = true; }
		if ( mouse.B.hi )		bShot = !bShot ;
	}

	// 計算
	{
		static	vect3	acc2;
		if ( keys.R.hi ) acc2 = 0;



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
void Lab::furiko3d( SysKeys& keys, SysMouse& mouse, SysGra& gra, Pers& pers, int& text_y )
//------------------------------------------------------------------------------
{
		gra.Print(1,(float)text_y++,string("<<furiko3d>>")+to_string(idx)); 
		// T=J dω/dt
		// T：トルク J：慣性モーメント ω：回転角速度[rad]
		//
		// t=rF
		// r:位置ベクトル t:力のモーメント F:力
		//
		// L=rp
		// r:位置ベクトル L:角運動量(運動量のモーメント) p:運動量

	const float G = 9.8;			// 重力加速度
	const float T = 1.0/60.0;		// 時間/frame
	const float g = 9.8 *T*T;		// 重力加速度/frame
	const vect3 vg = vect3(0,-g,0);	// 重力加速度ベクトル/frame

	static vect3	acc;			// 運動量
	static vect3	mov;			// 運動量
	static bool	bPause = false; 
	bool	bStep = false; 


	// 初期化
	if ( !bInit )
	{
		if ( !bInitCam )
		{
			bInitCam = true;
			pers.cam.pos = vect3(  0.0, 0.0, -7.0 );
			pers.cam.at = vect3( 0,  0.0, 0 );
		}

		bInit = true;
		for ( Obj* p : (*this).tblObj ) delete p;
		tblObj.clear();
		tblObj.emplace_back( new Obj(vect3(0, 2.0, 0)) );
		tblObj.emplace_back( new Obj(vect3(-1, 2.0, 0)) );
		
		acc = 0;
	}

	vect3&	v0 = tblObj[0]->pos;	//	barの根本
	vect3&	v1 = tblObj[1]->pos;	//	barの先端

	// 入力
	{
		// ポーズ
		if ( keys.SPACE.hi )	bPause = !bPause;

		// ステップ再生
		if ( keys.ENTER.rep )	bStep = true;

		// リセット
		if ( keys.R.hi )	bInit = false ;

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
		acc += F;

	 	float	w = acc.abs()/r;					//	角速度

mov =acc;
		acc = mrotateByAxis( moment, w ) * acc;		//	回転

		v1 += acc;
	}

	// 描画
	{
		gra.Clr(rgb(0.3,0.3,0.3));
		pers.grid.DrawGrid3d( gra, pers, vect3(0,0,0), mrotx(deg2rad(90)), 100, 100, 1, vect3(0.2,0.2,0.2) );

		pers.line3d( gra, pers, v0, v1, rgb(1,1,1), 2 );

		drawVect( gra, pers, text_y, v1, vg	,100, rgb(1,0,0), "g" );
		drawVect( gra, pers, text_y, v0, moment,100, rgb(1,0,1), "moment" );
		drawVect( gra, pers, text_y, v1, F		,100, rgb(0,1,0), "F" );
		drawVect( gra, pers, text_y, v1, acc	,2	, rgb(1,1,0), "acc" );
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
void Lab::gravityPlanet( SysKeys& keys, SysMouse& mouse, SysGra& gra, Pers& pers, int& text_y )
//------------------------------------------------------------------------------
{
	gra.Print(1,(float)text_y++,string("<<gravityPlanet>>")+to_string(idx)); 

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
	if ( !bInit )
	{
		bInit = true;
		if ( !bInitCam )
		{
			bInitCam = true;
			//上から
			pers.cam.pos = vect3(  0.0, 0.5, -2.0 );
			pers.cam.at = vect3( 0,  0.0, 0 );
		}

		for ( Obj* p : (*this).tblObj ) delete p;
		tblObj.clear();
		tblObj.emplace_back( new Planet(vect3(-0.5,0.1,0),vect3(0, 0, -0.02)) );
		tblObj.emplace_back( new Planet(vect3( 0.5,0.1,0),vect3(0, 0, 0.02)) );
	}

	const float	G = 9.8;			// 重力加速度
	const float	T = 1.0/60.0;		// 時間/frame
	const float	g = 9.8 *T*T;		// 重力加速度/frame

	// 入力
	{
		// ポーズ
		if ( keys.SPACE.hi )	bPause = !bPause;

		// ステップ再生
		if ( keys.ENTER.rep )	bStep = true;

		// リセット
		if ( keys.R.hi )	bInit = false ;

		// 生成
		if ( mouse.B.hi )
		{
			vect3 P = pers.calcScreenToWorld3( vect3(mouse.pos,0) );
			vect3 I = pers.calcRayvect( P );
			tblObj.emplace_back( new Planet( P, I/100.0 ) );
		}

	}

	
	Planet& pl0 = *dynamic_cast<Planet*>((*this).tblObj[0]);	//	太陽
	Planet& pl1 = *dynamic_cast<Planet*>((*this).tblObj[1]);	//	地球

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
		for ( Obj* p : (*this).tblObj ) 
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
void Lab::kakusokudo( SysKeys& keys, SysMouse& mouse, SysGra& gra, Pers& pers, int& text_y )
//------------------------------------------------------------------------------
{
	gra.Print(1,(float)text_y++,string("<<kakusokudo>>")+to_string(idx)); 

	const float	G = 9.8;			// 重力加速度
	const float	T = 1.0/60.0;		// 時間/frame
	const float	g = 9.8 *T*T;		// 重力加速度/frame

	static bool bShot = false;
	static vect3	velocity;
	static vect3	moment;
	static vect3	to;
	static vect3	add;
	
	static float 	w;	//	角速度

	// 初期化
	if ( !bInit )
	{
		if ( !bInitCam )
		{
			bInitCam = true;
			pers.cam.pos = vect3( 2.0, 2.5, -2.0 );
			pers.cam.at = vect3( 0,  0.0, 0 );
		}
		bInit = true;

		for ( Obj* p : (*this).tblObj ) delete p;
		tblObj.clear();
		tblObj.emplace_back( new Obj(vect3(0  ,0.1,0)) );
		tblObj.emplace_back( new Obj(vect3(1  ,0.1,0)) );
		tblObj.emplace_back( new Obj(vect3(1.5,0.1,0.2)) );

		bShot = false;
	}


	vect3&	v0 = (*this).tblObj[0]->pos;	//	barの根本
	vect3&	v1 = (*this).tblObj[1]->pos;	//	barの先端
	vect3&	v2 = (*this).tblObj[2]->pos;	// 速度指定


	//入力
	{
		// リセット
		if ( keys.R.hi )	bInit = false ;

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

		drawVect( gra, pers, text_y, v1, velocity	,1	, rgb(1,1,0), "velocity" );
		drawVect( gra, pers, text_y, v0, moment		,1	, rgb(1,0,1), "moment" );
		drawVect( gra, pers, text_y, v0, to			,1	, rgb(0,1,1), "to" );
			
	}

}

//------------------------------------------------------------------------------
void Lab::graph( SysKeys& keys, SysMouse& mouse, SysGra& gra, Pers& pers, int& text_y )
//------------------------------------------------------------------------------
{
	gra.Print(1,(float)text_y++,string("<<graph>>")+to_string(idx)); 

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
