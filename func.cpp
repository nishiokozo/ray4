#include <iostream>
#include <functional>
#include <tuple>

#include "geom.h"

//#include "SysGra.h"

//#include "pers.h"

#include "func.h"

using namespace std;

//------------------------------------------------------------------------------
//	定数
//------------------------------------------------------------------------------
/*
const float	G	= -9.80665;				// 重力加速度
const rgb col0( 0, 0, 0 );
const rgb col1( 0, 0, 1 );
const rgb col2( 1, 0, 0 );
const rgb col3( 1, 0, 1 );
const rgb col4( 0, 1, 0 );
const rgb col5( 0, 1, 1 );
const rgb col6( 1, 1, 0 );
const rgb col7( 1, 1, 1 );
*/


//------------------------------------------------------------------------------
float func_distanceLinePoint( vect3 P0, vect3 I0, vect3 P1 )
//------------------------------------------------------------------------------
{
	// 直線/点距離
	// 直線 : P0+I0
	// 点   : P1

	float d = cross( (P1 - P0), I0 ).abs();
	return d;
}

//------------------------------------------------------------------------------
tuple<bool,float,vect3,vect3,float,float>func_distance_Line_Line( vect3 P0, vect3 I0, vect3 P1, vect3 I1 )
//------------------------------------------------------------------------------
{
	if ( I0.isZero() || I1.isZero() ) return {false,0,vect3(0,0,0),vect3(0,0,0),0,0};

	//    P0       P1
	//    |        |
	//    |}s0     |}s1(距離ベクトル:Iベクトル方向、負の数ならP1より前)
	//    |        |
	// Q0 +--------+ Q1(衝突位置)
	//    |        |
	//    v        v
	//    I0       I1(単位ベクトル)
	//
	//	交点ができたときは、Q0=Q1 , d=0 になる

	// 直線/線分距離
	if ( cross( I0, I1 ).isZero() )
	{
		vect3 Q0;
		vect3 Q1;
		float d = cross( (P1 - P0), I0 ).abs();
		return {false,d,Q0,Q1,0,0};
	}

	float d0 = dot( P1 - P0, I0 );
	float d1 = dot( P1 - P0, I1 );
	float d2 = dot( I0, I1 );


	float s0 = ( d0 - d1 * d2 ) / ( 1.0f - d2 * d2 );
	float s1 = ( d1 - d0 * d2 ) / ( d2 * d2 - 1.0f );

	vect3	Q0 = P0 + s0 * I0;
	vect3	Q1 = P1 + s1 * I1;
	float	d =  (Q1 - Q0).abs();

	return {true,d,Q0,Q1,s0,s1};
}

//------------------------------------------------------------------------------
tuple<bool,float,vect3,vect3,float,float>func_distance_Line_Segline( vect3 P0, vect3 I0, vect3 p1, vect3 q1 )
//------------------------------------------------------------------------------
{
	if ( q1 == p1 ) return {false,0,vect3(0,0,0),vect3(0,0,0),0,0};

	// 直線と線分の距離
	// 直線     : P0+I0
	// 線分開始 : p1
	// 線分終了 : q1
	// 距離     : d = |Q1-Q0|
	// 戻り値   : d距離 Q0,Q1	※false でもdだけは取得できる
	
	vect3	P1 = p1;
	vect3	I1 = (q1-p1).normalize();

	auto[flg,d,Q0,Q1,s0,s1] = func_distance_Line_Line( P0, I0, P1, I1 );

	if ( flg )
	{
		// 線分処理
		if ( s1 < 0 ) flg = false;
		if ( s1 > (q1-p1).abs() ) flg = false;

	}

	return {flg,d,Q0,Q1,s0,s1};
}

//------------------------------------------------------------------------------
tuple<bool,float,vect3,vect3,float,float>func_distance_Harfline_Segline( vect3 p0, vect3 I0, vect3 p1, vect3 q1 )
//------------------------------------------------------------------------------
{
	if ( I0.isZero() || q1 == p1 ) return {false,0,vect3(0,0,0),vect3(0,0,0),0,0};

	// 半直線と線分の距離
	// 半直線   : p0+I0
	// 線分開始 : p1
	// 線分終了 : q1
	// 距離     : d = |Q1-Q0|
	// 戻り値   : d距離 Q0,Q1	※false でもdだけは取得できる
	
	vect3	P0 = p0;
	vect3	P1 = p1;
	vect3	I1 = (q1-p1).normalize();

	auto[flg,d,Q0,Q1,s0,s1] = func_distance_Line_Line( P0, I0, P1, I1 );

	if ( flg )
	{
		// 線分処理
		if ( s1 < 0 ) flg = false;
		if ( s1 > (q1-p1).abs() ) flg = false;

		// 半直線
		if ( s0 < 0 ) flg = false;
	}

	return {flg,d,Q0,Q1,s0,s1};
}

//------------------------------------------------------------------------------
tuple<bool,float,vect3,vect3,float,float>func_distance_Segline_Segline( vect3 p0, vect3 q0, vect3 p1, vect3 q1 )
//------------------------------------------------------------------------------
{
	if ( q0 == p0 || q1 == p1 ) return {false,0,vect3(0,0,0),vect3(0,0,0),0,0};

	// 線分と線分の距離
	// 線分0開始: p1
	// 線分0終了: q1
	// 線分1開始: p1
	// 線分1終了: q1
	// 距離     : d = |Q1-Q0|
	// 戻り値   : d距離 Q0,Q1	※false でもdだけは取得できる
	
	vect3	P0 = p0;
	vect3	I0 = (q0-p0).normalize();
	vect3	P1 = p1;
	vect3	I1 = (q1-p1).normalize();

	auto[flg,d,Q0,Q1,s0,s1] = func_distance_Line_Line( P0, I0, P1, I1 );

	if ( flg )
	{
		// 線分処理
		if ( s1 < 0 ) flg = false;
		if ( s1 > (q1-p1).abs() ) flg = false;

		// 線分処理
		if ( s0 < 0 ) flg = false;
		if ( s0 > (q0-p0).abs() ) flg = false;

	}

	return {flg,d,Q0,Q1,s0,s1};
}

//------------------------------------------------------------------------------
tuple<bool,vect3,float> func_distance_Plate_Line( vect3 plate_P, vect3 plate_N, vect3 line_P, vect3 line_I )
//------------------------------------------------------------------------------
{
	// 平面と直線の距離
	float	f = dot(plate_N, line_P - plate_P);
	if ( f > 0 )	// 平面の表面のみ
	{
		float	s = -f/dot( plate_N, line_I );

		vect3	Q = line_I * s + line_P;
		return {true,Q,s};
	}
	return {false,vect3(0,0,0),0};
};

//------------------------------------------------------------------------------
tuple<bool,vect3,float> func_distance_Plate_Harfline( vect3 plate_P, vect3 plate_N, vect3 p0, vect3 i0 )
//------------------------------------------------------------------------------
{
	// 平面と直線の距離
	// p0 : line start
	// i0 : line vector

	auto[flg,Q,s] = func_distance_Plate_Line( plate_P, plate_N, p0, i0 );

	if ( flg )
	{
		if ( s < 0 ) flg = false;
	}


	return {flg,Q,s};

};

//------------------------------------------------------------------------------
tuple<bool,vect3,float> func_distance_Plate_Segline( vect3 plate_P, vect3 plate_N, vect3 p0, vect3 q0 )
//------------------------------------------------------------------------------
{
	// 平面と線分の距離
	// p0 : line start
	// q0 : line end

	vect3 i0 = (q0 - p0).normalize();

	auto[flg,Q,s] = func_distance_Plate_Line( plate_P, plate_N, p0, i0 );

	if ( flg )
	{
		if ( s < 0 ) flg = false;
		if ( s > (q0-p0).abs() ) flg = false;
	}


	return {flg,Q,s};
}

//------------------------------------------------------------------------------
bool func_IsIntersectSphereLine( vect3 sphere_P, float sphere_r, vect3 p0 , vect3 i0 )
//------------------------------------------------------------------------------
{
	//	球と直線の衝突判定
	// p0 : line start
	// i0 : line vector

	vect3	OP = p0 - sphere_P;
	float	b = dot( i0, OP );
	float	aa = sphere_r*sphere_r - dot(OP,OP)+ b*b;
	return ( aa>=0 ) ;
};

//------------------------------------------------------------------------------
vect3 func_bezier3( float t, vect3 P0, vect3 P1, vect3 P2, vect3 P3 )
//------------------------------------------------------------------------------
{
	//ベジェ 曲線
#if 1
	vect3 L0=(P1-P0)*t+P0;
	vect3 L1=(P2-P1)*t+P1;
	vect3 L2=(P3-P2)*t+P2;

	vect3 M0=(L1-L0)*t+L0;
	vect3 M1=(L2-L1)*t+L1;

	vect3 Q=(M1-M0)*t+M0;
#else
	float tt = t*t;
	float ttt = tt*t;
	vect3 Q=
		 P3*  ttt
		+P2*(-ttt*3 +tt*3)
		+P1*( ttt*3 -tt*6 +t*3)
		+P0*(-ttt   +tt*3 -t*3 +1)
		;
#endif

	return Q;
};

//------------------------------------------------------------------------------
vect3 func_bezier3_delta( float t, vect3 P0, vect3 P1, vect3 P2, vect3 P3 )
//------------------------------------------------------------------------------
{
	//ベジェ 曲線	接線	bezier3_funcを一回微分した物
	float tt = t*t;
	vect3 Q=
		 P3*  3*tt
		+P2*(-3*tt*3 +2*t*3)
		+P1*( 3*tt*3 -2*t*6 +3)
		+P0*(-3*tt   +2*t*3 -3)
		;
/*
	Q=
		 P3*  tt
		+P2*(-tt*3 +2*t)
		+P1*( tt*3 -2*t*2 +1)
		+P0*(-tt   +2*t -1)
		;
*/
	return Q;
};

//------------------------------------------------------------------------------
vect3 func_catmull3( float t, const vect3 P0, const vect3 P1, const vect3 P2, const vect3 P3 )
//------------------------------------------------------------------------------
{
	//catmull-Rom 曲線
	// P(t)=P1*(2t^3-3t^2+1)+m0*(t^3-2t^2+t)+P2*(-2t^3+3t^2)+m1*(t^3-t^2)
	// m0=(P2-P0)/2
	// m1=(P3-P1)/2

	vect3 m0 = (P2-P0)/2.0;
	vect3 m1 = (P3-P1)/2.0;
	vect3 P =  P1*(  2*t*t*t - 3*t*t +1) + m0*( t*t*t -2*t*t +t )
			 + P2*( -2*t*t*t + 3*t*t   ) + m1*( t*t*t - t*t );

	return P;
};


// 移動量d,初速v0の時の時間tを求める。
//------------------------------------------------------------------------------
float func_accelerationGetTime_DVv( vect3 gv, float d, vect3 v0 )	// DV : Distance / Velocity
//------------------------------------------------------------------------------
{

	float	a = 0.5*gv.abs();
	float	t = 0;

	if ( a == 0 && v0.abs() == 0 )
	{
		// 加速度＆速度がない
		t = 0;
	}
	else
	if ( v0.abs() == 0 )
	{
		// 加速のみ
		t =  sqrt( d/a ); 
	}
	else
	if ( a == 0 )
	{
		// 速度のみ
		t = d/v0.abs();
	}
	else
	{
		float	c = v0.abs()/gv.abs();
		t =  sqrt( d/a + c*c ) - c; 
	}

	return t;
};

// 時間t,初速v0の時の移動量dを求める。
//------------------------------------------------------------------------------
vect3 func_accelerationGetDistance_TVv( vect3 gv, float t, vect3 v0 )	// TV : Time / Velocity
//------------------------------------------------------------------------------
{
	vect3 a = 0.5*gv;
	vect3 d = a*t*t + v0*t;
	return d;
};

