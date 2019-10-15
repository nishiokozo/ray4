#include <iostream>
#include <functional>
#include <tuple>

#include "geom.h"

#include "SysGra.h"

#include "func.h"

#include "pers.h"


using namespace std;
//------------------------------------------------------------------------------
void g_line3d_scissor( SysGra& gra, Pers& pers, vect3 p0, vect3 p1, rgb col, float wide )
//------------------------------------------------------------------------------
{
	vect3 a = p0* pers.cam.mat.invers();
	vect3 b = p1* pers.cam.mat.invers();
	vect3 v0;
	vect3 v1;
	bool flg = pers.calcScissorLine3d( a, b, v0, v1 );
	if ( flg ) gra.Line( v0, v1, col, wide );
}
//------------------------------------------------------------------------------
void g_line3d( SysGra& gra, Pers& pers, vect3 p0, vect3 p1, rgb col, float wide )
//------------------------------------------------------------------------------
{
	vect3 v0 = pers.calcWorldToScreen3( p0 );
	vect3 v1 = pers.calcWorldToScreen3( p1 );
	if ( v0.z > 0 && v1.z > 0 ) gra.Line( v0, v1, col, wide );
}

//------------------------------------------------------------------------------
void g_pset3d( SysGra& gra, Pers& pers, vect3 p0, rgb col, float wide )
//------------------------------------------------------------------------------
{
	vect3 v0 = pers.calcWorldToScreen3( p0 );
	if ( v0.z > 0 ) gra.Pset( v0, col, wide);
}

//------------------------------------------------------------------------------
void g_print3d( SysGra& gra, Pers& pers, vect3 p0, float x, float y, string str )
//------------------------------------------------------------------------------
{
	vect3 v0 = pers.calcWorldToScreen3( p0 );
	if ( v0.z > 0 ) gra.Print( v0.xy() + gra.Dot(x,y), str ); 
}

// ~2d関数はXY平面、z=0に描画する。
//------------------------------------------------------------------------------
void g_pset2d( SysGra& gra, Pers& pers, vect2 p0, rgb col, float wide )
//------------------------------------------------------------------------------
{
	g_pset3d( gra, pers, vect3(p0,0), col, wide );
}

//------------------------------------------------------------------------------
void g_print2d( SysGra& gra, Pers& pers, vect2 p0, string str )
//------------------------------------------------------------------------------
{
	g_print3d( gra, pers, vect3(p0,0), 0, 0, str );

}
//------------------------------------------------------------------------------
void g_line2d( SysGra& gra, Pers& pers, vect2 p0, vect2 p1, rgb col, float wide )
//------------------------------------------------------------------------------
{
	g_line3d( gra, pers, vect3(p0,0), vect3(p1,0), col, wide );

}
