#include <iostream>
#include <functional>
#include <tuple>

#include "geom.h"

//#include "SysGra.h"

//#include "pers.h"

#include "func.h"

using namespace std;

//------------------------------------------------------------------------------
static tuple<bool,float>func_intersect_Plate_Curve_t( vect3 plate_P, vect3 plate_N, vect3 curve_A, vect3 curve_V )
//------------------------------------------------------------------------------
{
	vect3 P = plate_P;
	vect3 N = plate_N;
	vect3 A = curve_A;
	vect3 V = curve_V;

    ////////////////////////////////////////////////////////////////////////
    //                                                                    //
    //  平面と二次曲線の交差判定                                          //
    //                                                                    //
    //  P :平面原点ベクトル                                               //
    //  N :平面法線ベクトル                                               //
    //  A :曲線加速度ベクトル                                             //
    //  V :曲線初速度ベクトル                                             //
    //  Q :交差点ベクトル                                                 //
    //  t :時間                                                           //
    //                                                                    //
    //  P = P-P0                                                          //
    //                                                                    //
    //  N･(Q - P) = 0     ...①                                           //
    //                                                                    //
    //      w = 0.5t^2                                                    //
    //                                                                    //
    //  Q = Aw + Vt       ...②                                           //
    //                                                                    //
    //  ①に②を代入                                                      //
    //                                                                    //
    //  N･(Aw + Vt - P) = 0                                               //
    //                                                                    //
    //  N.x*((A.x*w + V.x*t)-P.x) +                                       //
    //  N.y*((A.y*w + V.y*t)-P.y) +                                       //
    //  N.z*((A.z*w + V.z*t)-P.z) = 0                                     //
    //                                                                    //
    //  (N･A)0.5t^2 + (N･V)t - (N･P) = 0                                  //
    //                                                                    //
    //      c=(N･A)0.5                                                    //
    //      d=(N･V)                                                       //
    //      e=-(N･P)                                                      //
    //                                                                    //
    //  ct^2 + dt + e = 0                                                 //
    //                                                                    //
    //  c(t+(d/2c))^2 + e -c(d/(2c))^2 = 0                                //
    //                                                                    //
    //      f = (d/2c)                                                    //
    //                                                                    //
    //  c(t+f)^2 + e -cf^2 = 0                                            //
    //                                                                    //
    //                                                                    //
    //  t = ±√( f^2 - e/c ) - f;                                        //
    //                                                                    //
    //                                                                    //
    ////////////////////////////////////////////////////////////////////////

	float c = dot(N,A)*0.5;
	float d = dot(N,V);
	float e = -dot(N,P);
	float f = d/(2*c);		
	float g = sqrt( f*f - e/c );
	float t = 0;

	bool flg = false;
	{
		//////////////////////////////////////
	    //                                  //
		//   A･V | A･N || +√  | -√        //
		//  -----+-----++------+------      //
		//    +  |  +  ||  裏  |  ✕        //
		//  -----+-----++------+------      //
		//    +  |  -  ||【表】|  ✕        //
		//  -----+-----++------+------      //
		//    -  |  +  ||  裏  |【表】      //
		//  -----+-----++------+------      //
		//    -  |  -  ||【表】|  裏        //
	    //                                  //
	    //  ※ A･V<0 は山型（答えが２つ）   //
	    //  ※ 平面の表との衝突のみ検出     //
	    //                                  //
		//////////////////////////////////////

		float av = dot(A,V);
		float an = c;

		if ( av > 0)
		{
			if ( an < 0 ) 
			{
				t =  g - f;
				flg = true;
			}
		}
		else
		{
			if ( an < 0 ) 
			{
				t =  g - f;
				flg = true;
			}
			else
			{
				t = -g - f;
				flg = true;
			}
		}		
	}

	return {flg,t};

}
	
//------------------------------------------------------------------------------
tuple<bool,vect3,float>func_intersect_Plate_Curve( vect3 plate_P, vect3 plate_N, vect3 curve_P0, vect3 curve_A, vect3 curve_V )
//------------------------------------------------------------------------------
{
	// 平面と曲線の交差判定

	auto[flg,t] = func_intersect_Plate_Curve_t( plate_P-curve_P0, plate_N, curve_A, curve_V );
	if ( flg )
	{
		vect3 Q = func_accelerationGetDistance_TVv( curve_A, t, curve_V ) + curve_P0;
		return {true,Q,t};
	}
	return {false,vect3(0,0,0),t};
}

//------------------------------------------------------------------------------
tuple<bool,vect3,float>func_intersect_Plate_SegCurve( vect3 plate_P, vect3 plate_N, vect3 curve_P0, float curve_t0, float curve_t1, vect3 curve_A, vect3 curve_V )
//------------------------------------------------------------------------------
{
	// 平面と曲線分の交差判定

	auto[flg,t] = func_intersect_Plate_Curve_t( plate_P-curve_P0, plate_N, curve_A, curve_V );

	if ( flg )
	{
		// 原点(curve_t0)以上、curve_t1未満のみ
		if ( t >= curve_t0 && t < curve_t1 )
		{
			vect3 Q = func_accelerationGetDistance_TVv( curve_A, t, curve_V ) + curve_P0;

			return {flg,Q,t};
		}
	}
	return {false,vect3(0,0,0),t};
}

//------------------------------------------------------------------------------
tuple<bool,vect3,float>func_intersect_Plate_SegCurve_ball( vect3 plate_P, vect3 plate_N, vect3 curve_P0, float r, float curve_t0, float curve_t1, vect3 curve_A, vect3 curve_V )
//------------------------------------------------------------------------------
{
	// 平面と曲線分の交差判定
	vect3 vr = plate_N*r;
	auto[flg,q0,t] = func_intersect_Plate_SegCurve( plate_P+vr, plate_N, curve_P0, curve_t0, curve_t1, curve_A, curve_V );

	q0 -= vr;
	return {flg,q0,t};
}

//------------------------------------------------------------------------------
float func_intersect_Line_Point( vect3 P0, vect3 I0, vect3 P1 )
//------------------------------------------------------------------------------
{
	// 直線/点距離
	// 直線 : P0+I0
	// 点   : P1

	float d = cross( (P1 - P0), I0 ).abs();
	return d;
}

//------------------------------------------------------------------------------
tuple<bool,float,vect3,vect3,float,float>func_intersect_Line_Line( vect3 P0, vect3 I0, vect3 P1, vect3 I1 )
//------------------------------------------------------------------------------
{
	if ( I0.isZero() || I1.isZero() ) return {false,0,vect3(0,0,0),vect3(0,0,0),0,0};

	//    P0       P1
	//    |        |
	//    |}t0     |}t1(時間:Iベクトル方向、負の数ならP1より前)
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


	float t0 = ( d0 - d1 * d2 ) / ( 1.0f - d2 * d2 );
	float t1 = ( d1 - d0 * d2 ) / ( d2 * d2 - 1.0f );

	vect3	Q0 = P0 + t0 * I0;
	vect3	Q1 = P1 + t1 * I1;
	float	d =  (Q1 - Q0).abs();

	return {true,d,Q0,Q1,t0,t1};
}

//------------------------------------------------------------------------------
tuple<bool,float,vect3,vect3,float,float>func_intersect_Line_SegLine( vect3 P0, vect3 I0, vect3 p1, vect3 q1 )
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

	auto[flg,d,Q0,Q1,t0,t1] = func_intersect_Line_Line( P0, I0, P1, I1 );

	if ( flg )
	{
		// 線分処理
		if ( t1 < 0 ) flg = false;
		if ( t1 > (q1-p1).abs() ) flg = false;

	}

	return {flg,d,Q0,Q1,t0,t1};
}

//------------------------------------------------------------------------------
tuple<bool,float,vect3,vect3,float,float>func_intersect_HarfLine_SegLine( vect3 p0, vect3 I0, vect3 p1, vect3 q1 )
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

	auto[flg,d,Q0,Q1,t0,t1] = func_intersect_Line_Line( P0, I0, P1, I1 );

	if ( flg )
	{
		// 線分処理
		if ( t1 < 0 ) flg = false;
		if ( t1 > (q1-p1).abs() ) flg = false;

		// 半直線
		if ( t0 < 0 ) flg = false;
	}

	return {flg,d,Q0,Q1,t0,t1};
}

//------------------------------------------------------------------------------
tuple<bool,float,vect3,vect3,float,float>func_intersect_SegLine_SegLine( vect3 p0, vect3 q0, vect3 p1, vect3 q1 )
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

	auto[flg,d,Q0,Q1,t0,t1] = func_intersect_Line_Line( P0, I0, P1, I1 );

	if ( flg )
	{
		// 線分処理
		if ( t1 < 0 ) flg = false;
		if ( t1 > (q1-p1).abs() ) flg = false;

		// 線分処理
		if ( t0 < 0 ) flg = false;
		if ( t0 > (q0-p0).abs() ) flg = false;

	}

	return {flg,d,Q0,Q1,t0,t1};
}

//------------------------------------------------------------------------------
tuple<bool,vect3,float> func_intersect_Plate_Line( vect3 plate_P, vect3 plate_N, vect3 line_P, vect3 line_I )
//------------------------------------------------------------------------------
{
	// 平面と直線の距離
	float	f = dot(plate_N, line_P - plate_P);
	if ( f > 0 )	// 平面の表面のみ
	{
		float	t = -f/dot( plate_N, line_I );

		vect3	Q = line_I * t + line_P;
		return {true,Q,t};
	}
	return {false,vect3(0,0,0),0};
};

//------------------------------------------------------------------------------
tuple<bool,vect3,float> func_intersect_Plate_HarfLine( vect3 plate_P, vect3 plate_N, vect3 p0, vect3 i0 )
//------------------------------------------------------------------------------
{
	// 平面と直線の距離
	// p0 : line start
	// i0 : line vector

	auto[flg,Q,t] = func_intersect_Plate_Line( plate_P, plate_N, p0, i0 );

	if ( flg )
	{
		if ( t < 0 ) flg = false;
	}


	return {flg,Q,t};

};

//------------------------------------------------------------------------------
tuple<bool,vect3,float> func_intersect_Plate_SegLine( vect3 plate_P, vect3 plate_N, vect3 p0, vect3 q0 )
//------------------------------------------------------------------------------
{
	// 平面と線分の距離
	// p0 : line start
	// q0 : line end

	vect3 i0 = (q0 - p0).normalize();

	auto[flg,Q,t] = func_intersect_Plate_Line( plate_P, plate_N, p0, i0 );

	if ( flg )
	{
		if ( t < 0 ) flg = false;
		if ( t > (q0-p0).abs() ) flg = false;
	}


	return {flg,Q,t};
}

//------------------------------------------------------------------------------
bool func_IsIntersectSphereLine( vect3 sphere_P, float sphere_r, vect3 p0 , vect3 i0 )
//------------------------------------------------------------------------------
{
	// 球と直線の衝突判定
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
	// ベジェ 曲線	接線を求める
	// func_bezier3を一回微分した物

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

//------------------------------------------------------------------------------
float func_accelerationGetTime_DVv( float a, float p, float v )	// DV : Distance / Velocity
//------------------------------------------------------------------------------
{
	// 二次曲線：加速度a,位置p,初速vの時の時間tを求める。
	// p = 1/2*a*t^2 + v*t からの変形
	 
	float	b = 0.5*a;
	float	t = 0;

	if ( b == 0 && v == 0 )
	{
		// 加速度＆速度がない
		t = 0;
	}
	else
	if ( v == 0 )
	{
		// 加速のみ
		t =  sqrt( p/b ); 
	}
	else
	if ( b == 0 )
	{
		// 速度のみ
		t = p/v;
	}
	else
	{
		float	c = v/a;
		t =  sqrt( p/b + c*c ) - c; 
	}

	return t;
};

//------------------------------------------------------------------------------
vect3 func_accelerationGetDistance_TVv( vect3 A, float t, vect3 V )	// TV : Time / Velocity
//------------------------------------------------------------------------------
{
	// 二次曲線：加速度a,時間t,初速vの時の位置pを求める。

	return 0.5*A*t*t + V*t;
};

//------------------------------------------------------------------------------
vect3 func_reflect( const vect3& I, const vect3& N, float rate )
//------------------------------------------------------------------------------
{
	//  I 入射ベクトル
	//  N 法線ベクトル（単位ベクトル）
	//  rate 反射レート 1.0:正反射 0.0:法線と直行 -n:屈折反射
	return I - (1.0+rate)*dot( I, N ) * N;
}
