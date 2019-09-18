#include <iostream>
#include <functional>
using namespace std;

#include "geom.h"

#include "SysGra.h"

#include "obj.h"

#include "func.h"

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
void g_line3d( SysGra& gra, Pers& pers, vect3 p0, vect3 p1, rgb col, bool bScissor )
//------------------------------------------------------------------------------
{
	if ( bScissor )
	{
		vect3 a = p0* pers.cam.mat.invers();
		vect3 b = p1* pers.cam.mat.invers();
		vect3 v0;
		vect3 v1;
		bool flg = pers.calcScissorLine3d( a, b, v0, v1 );
		if ( flg ) gra.Line( vect2(v0.x,v0.y), vect2(v1.x,v1.y), col );
	}
	else
	{
		vect3 v0 = pers.calcWorldToScreen3( p0 );
		vect3 v1 = pers.calcWorldToScreen3( p1 );
		gra.Line( v0, v1, col );
	}
}
