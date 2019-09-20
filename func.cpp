#include <iostream>
#include <functional>
#include <tuple>

#include "geom.h"

#include "SysGra.h"

#include "obj.h"

#include "func.h"

using namespace std;



//------------------------------------------------------------------------------
tuple<bool,float,vect3,vect3>lengthLineLine_func( vect3 P0, vect3 I0, vect3 P1, vect3 I1 )
//------------------------------------------------------------------------------
{
	// 直線と直線の距離
	// 直線0: P0+I0
	// 直線1: P1+I1
	// 距離 : d = |Q1-Q0|
	// 戻り値 : 二本の直線が並行のとき、falseが変える。距離は求まる。

	vect3 Q0;
	vect3 Q1;
	float d;

	float d0 = dot( P1 - P0, I0 );
	float d1 = dot( P1 - P0, I1 );
	float d2 = dot( I0, I1 );

	if ( cross( I0, I1 ).abs() < 0.000001f ) 
	{
	    d = cross( (P1 - P0), I0 ).abs();
	//    return false;
		return {false,d,Q0,Q1};
	}

	float t0 = ( d0 - d1 * d2 ) / ( 1.0f - d2 * d2 );
	float t1 = ( d1 - d0 * d2 ) / ( d2 * d2 - 1.0f );

	Q0 = P0 + t0 * I0;
	Q1 = P1 + t1 * I1;
	d =  (Q1 - Q0).abs();

	return {true,d,Q0,Q1};
}

//------------------------------------------------------------------------------
bool IsIntersectPlate( vect3 plate_P, vect3 plate_N, vect3 P, vect3 I, vect3& Q)
//------------------------------------------------------------------------------
{
	// 球と変面殿衝突判定
	float	f = dot(plate_N, P - plate_P);
//	if ( f > 0 )
	{
		float	t = -f/dot( plate_N, I );

		if ( t >= 0 )
		{
			Q = I * t + P;
			return true;
		}
	}
	return false;
};

//------------------------------------------------------------------------------
bool IsIntersectSphereLine( vect3 sphere_P, float sphere_r, vect3 line_P , vect3 line_I )
//------------------------------------------------------------------------------
{
	//	球と直線の衝突判定
	vect3	OP = line_P - sphere_P;
	float	b = dot( line_I, OP );
	float	aa = sphere_r*sphere_r - dot(OP,OP)+ b*b;
	return ( aa>=0 ) ;
};

//------------------------------------------------------------------------------
vect3 bezier_func( float t, vect3 P0, vect3 P1, vect3 P2, vect3 P3 )
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
vect3 catmull3d_func( float t, const vect3 P0, const vect3 P1, const vect3 P2, const vect3 P3 )
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

//------------------------------------------------------------------------------
void g_line3d_scissor( SysGra& gra, Pers& pers, vect3 p0, vect3 p1, rgb col, float wide )
//------------------------------------------------------------------------------
{
	{
		vect3 a = p0* pers.cam.mat.invers();
		vect3 b = p1* pers.cam.mat.invers();
		vect3 v0;
		vect3 v1;
		bool flg = pers.calcScissorLine3d( a, b, v0, v1 );
//		if ( flg ) gra.Line( vect2(v0.x,v0.y), vect2(v1.x,v1.y), col );
		if ( flg ) gra.Line( v0, v1, col, wide );
	}
}
//------------------------------------------------------------------------------
void g_line3d( SysGra& gra, Pers& pers, vect3 p0, vect3 p1, rgb col, float wide )
//------------------------------------------------------------------------------
{
	{
		vect3 v0 = pers.calcWorldToScreen3( p0 );
		vect3 v1 = pers.calcWorldToScreen3( p1 );
		if ( v0.z > 0 && v1.z > 0 ) gra.Line( v0, v1, col, wide );
	}
}
//------------------------------------------------------------------------------
void g_pset3d( SysGra& gra, Pers& pers, vect3 p0, rgb col, float wide )
//------------------------------------------------------------------------------
{
	vect3 v0 = pers.calcWorldToScreen3( p0 );
	if ( v0.z > 0 ) gra.Pset( v0, col, wide);
}
