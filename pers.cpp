#include <iostream>
#include <functional>
#include <tuple>

#include "geom.h"

#include "SysGra.h"

#include "func.h"

#include "pers.h"


using namespace std;
//------------------------------------------------------------------------------
void Pers::line3d_scissor( SysGra& gra, Pers& pers, vect3 p0, vect3 p1, rgb col, float wide )
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
void Pers::line3d( SysGra& gra, Pers& pers, vect3 p0, vect3 p1, rgb col, float wide )
//------------------------------------------------------------------------------
{
	vect3 v0 = pers.calcWorldToScreen3( p0 );
	vect3 v1 = pers.calcWorldToScreen3( p1 );
	if ( v0.z > 0 && v1.z > 0 ) gra.Line( v0, v1, col, wide );
}

//------------------------------------------------------------------------------
void Pers::pset3d( SysGra& gra, Pers& pers, vect3 p0, rgb col, float wide )
//------------------------------------------------------------------------------
{
	vect3 v0 = pers.calcWorldToScreen3( p0 );
	if ( v0.z > 0 ) gra.Pset( v0, col, wide);
}

//------------------------------------------------------------------------------
void Pers::print3d( SysGra& gra, Pers& pers, vect3 p0, float x, float y, string str )
//------------------------------------------------------------------------------
{
	vect3 v0 = pers.calcWorldToScreen3( p0 );
	if ( v0.z > 0 ) gra.Print( v0.xy() + gra.Dot(x,y), str ); 
}

// ~2d関数はXY平面、z=0に描画する。
//------------------------------------------------------------------------------
void Pers::pset2d( SysGra& gra, Pers& pers, vect2 p0, rgb col, float wide )
//------------------------------------------------------------------------------
{
	(*this).pset3d( gra, pers, vect3(p0,0), col, wide );
}

//------------------------------------------------------------------------------
void Pers::print2d( SysGra& gra, Pers& pers, vect2 p0, float x, float y, string str )
//------------------------------------------------------------------------------
{
	(*this).print3d( gra, pers, vect3(p0,0), 0, 0, str );

}
//------------------------------------------------------------------------------
void Pers::line2d( SysGra& gra, Pers& pers, vect2 p0, vect2 p1, rgb col, float wide )
//------------------------------------------------------------------------------
{
	(*this).line3d( gra, pers, vect3(p0,0), vect3(p1,0), col, wide );

}


//------------------------------------------------------------------------------
void Pers::showMat33(  SysGra& gra, Pers& pers, vect3 v0, mat33 m )
//------------------------------------------------------------------------------
{
	vect3 a = {m.m[0][0],m.m[0][1],m.m[0][2]};
	vect3 b = {m.m[1][0],m.m[1][1],m.m[1][2]};
	vect3 c = {m.m[2][0],m.m[2][1],m.m[2][2]};

	(*this).line3d( gra, pers, v0		, v0+a/2, rgb(1,0,0), 1 );
	(*this).line3d( gra, pers, v0		, v0+b/2, rgb(0,1,0), 1 );
	(*this).line3d( gra, pers, v0		, v0+c/2, rgb(0,0,1), 1 );
	(*this).line3d( gra, pers, v0+a/2	, v0+c/2, rgb(0.5,0.5,0.5), 1 );
	(*this).line3d( gra, pers, v0+b/2	, v0+c/2, rgb(0.5,0.5,0.5), 1 );
	(*this).line3d( gra, pers, v0+b/2	, v0+a/2, rgb(0.5,0.5,0.5), 1 );

}


//------------------------------------------------------------------------------
void Pers::DrawBox( SysGra& gra, Pers& pers, vect3 pos, mat33 m , bool bAxis, bool bTri )
//------------------------------------------------------------------------------
{
	const float S = 0.04f;
	const float L = 0.03f;
	const float M = 0.02f;
	const float N = 0.02f;
	static vector<vect3> vert=
	{
		{	-S,		 S,		-S	},//0
		{	 S,		 S,		-S	},//1
		{	-S,		-S,		-S	},//2
		{	 S,		-S,		-S	},//3
		{	-S,		 S,		 S	},//4
		{	 S,		 S,		 S	},//5
		{	-S,		-S,		 S	},//6
		{	 S,		-S,		 S	},//7

		{	-L,		-S-L, 	-L	},//8	//yマーク
		{	 L,		-S-L, 	-L	},//9
		{	-L,		-S-L, 	 L	},//10
		{	 L,		-S-L, 	 L	},//11

		{	S+M,	+M, 	-M	},//	//xマーク
		{	S+M,	+M, 	+M	},//
		{	S+M,	-M, 	 0	},//

		{	-N,		 -N, 	S+N	},//	//zマーク
		{	+N,		 -N, 	S+N	},//
		{	-N,		 +N, 	S+N	},//
		{	+N,		 +N, 	S+N	},//
	};
	static vector<vect3> disp;

	static vector<ivect2>	edge
	{
		{	0,	1	},
		{	1,	3	},
		{	3,	2	},
		{	2,	0	},
		{	4,	5	},
		{	5,	7	},
		{	7,	6	},
		{	6,	4	},
		{	0,	4	},
		{	1,	5	},
		{	2,	6	},
		{	3,	7	},
		
		{	8,	9	},	//yマーク
		{	9,	11	},
		{	11,	10	},
		{	10,	8	},
#if 0

		{	12,	13	},	//xマーク
		{	13,	14	},
		{	14,	12	},


		{	15,	18	},	//zマーク
		{	17,	16	},
#endif
		
	};
	static vector<ivect3>	tri
	{
		{	8,10,9	},{	9,10,11	},	// yマーク
		{	14,	13, 12	},	// xマーク
	};


	disp.clear();

	for ( vect3 v : vert )
	{

		//	右手系座標系
		//	右手ねじ周り
		//	roll	:z	奥+
		//	pitch	:x	右+
		//	yaw		:y	下+
		v= v * m + pos;

		disp.emplace_back( v );

	}


	// 軸
	if( bAxis )
	{
		vect3	nx = vect3( m.m[0][0], m.m[0][1], m.m[0][2] );
		vect3	ny = vect3( m.m[1][0], m.m[1][1], m.m[1][2] );
		vect3	nz = vect3( m.m[2][0], m.m[2][1], m.m[2][2] );
		pers.line3d( gra, pers, pos, pos+nx*0.2, rgb(1,0,0) );
		pers.line3d( gra, pers, pos, pos+ny*0.2, rgb(0,1,0) );
		pers.line3d( gra, pers, pos, pos+nz*0.2, rgb(0,0,1) );
	}
	
	// Tri
	if ( bTri )
	{
		for ( ivect3 t : tri )
		{
			vect3 v0 = pers.calcWorldToScreen3( disp[t.n0] );
			vect3 v1 = pers.calcWorldToScreen3( disp[t.n1] );
			vect3 v2 = pers.calcWorldToScreen3( disp[t.n2] );
//					if ( v0.z>0 )
			{
				gra.Tri( v0,v1,v2, rgb(1,0,1));
				gra.Tri( v2,v1,v0, rgb(1,0,1)/2);
			}

		}
	}
	for ( ivect2 e : edge )
	{
		const vect3& a = disp[e.p];
		const vect3& b = disp[e.n];
		const rgb col = rgb(0,1,1);

		pers.line3d( gra, pers, a, b, col, false );

	}
}
	// ドラム
//------------------------------------------------------------------------------
void Pers::DrawDrum( SysGra& gra, Pers& pers,  vect3 pos, mat33 m  )
//------------------------------------------------------------------------------
{
	static const float S = 0.05f;
	static const float L = 0.03f;
	static const float M = 0.02f;
	static const float N = 0.02f;
	static vector<vect3> vert=
	{
		{	-S,		 S,		-S	},
		{	 S,		 S,		-S	},
		{	-S,		-S,		-S	},
		{	 S,		-S,		-S	},
		{	-S,		 S,		 S	},
		{	 S,		 S,		 S	},
		{	-S,		-S,		 S	},
		{	 S,		-S,		 S	},

		{	-L,		-S-L, 	-L	},	//yマーク
		{	 L,		-S-L, 	-L	},
		{	-L,		-S-L, 	 L	},
		{	 L,		-S-L, 	 L	},

		{	S+M,	+M, 	-M	},	//xマーク
		{	S+M,	+M, 	+M	},
		{	S+M,	-M, 	 0	},

		{	-N,	 -N, 	S+N	},	//zマーク
		{	+N,	 -N, 	S+N	},
		{	-N,	 +N, 	S+N	},
		{	+N,	 +N, 	S+N	},
	};
	static vector<vect3> disp;

	static vector<ivect2>	edge
	{
/*				{	0,	1	},
		{	1,	3	},
		{	3,	2	},
		{	2,	0	},
		{	4,	5	},
		{	5,	7	},
		{	7,	6	},
		{	6,	4	},
		{	0,	4	},
		{	1,	5	},
		{	2,	6	},
		{	3,	7	},
*/				
#if 0
		{	8,	9	},	//yマーク
		{	9,	11	},
		{	11,	10	},
		{	10,	8	},

//				{	12,	13	},	//xマーク
//				{	13,	14	},
//				{	14,	12	},

		{	15,	18	},	//zマーク
		{	17,	16	},
#endif
		
	};
	static vector<ivect3>	tri
	{
//				{	14,	13, 12	},	// xマーク
	};

	static bool bInit = false;
	if ( !bInit )
	{
		{
			int	ofs = (signed)vert.size();
			int cnt = 0;
			float r = 0.05;
			float w = 0.05;
			for ( int i = 0 ; i < 360 ; i+= 30 )
			{
				float z = r*cos(deg2rad((float)i));
				float y = r*sin(deg2rad((float)i));
				vert.emplace_back(-w,y,z);
				vert.emplace_back( w,y,z);
				vert.emplace_back( w,y*0.5,z*0.5);
				cnt++;
			}
			
			for ( int i = 0 ; i < cnt ; i++ )
			{
				
				int st = i;
				int en = i+1;
				if ( i+1 == cnt ) en = 0;
				
				const int n = 3;

				edge.emplace_back(ofs+st*n  ,ofs+en*n);
				edge.emplace_back(ofs+st*n+1,ofs+en*n+1);
				edge.emplace_back(ofs+st*n+2,ofs+en*n+2);
				if ( i%3 == 0 ) edge.emplace_back(ofs+st*n  ,ofs+st*n+1);
			}
		}
		bInit = true;
	}

	disp.clear();

	for ( vect3 v : vert )
	{

		//	右手系座標系
		//	右手ねじ周り
		//	roll	:z	奥+
		//	pitch	:x	右+
		//	yaw		:y	下+
		v= v * m + pos;

		disp.emplace_back( v );

	}


	// 軸
	if(0)
	{
		vect3	nx = vect3( m.m[0][0], m.m[0][1], m.m[0][2] );
		vect3	ny = vect3( m.m[1][0], m.m[1][1], m.m[1][2] );
		vect3	nz = vect3( m.m[2][0], m.m[2][1], m.m[2][2] );
		pers.line3d( gra, pers, pos, pos+nx*0.2, rgb(1,0,0) );
		pers.line3d( gra, pers, pos, pos+ny*0.2, rgb(0,1,0) );
		pers.line3d( gra, pers, pos, pos+nz*0.2, rgb(0,0,1) );
	}
	
	// Tri
	for ( ivect3 t : tri )
	{
		vect3 v0 = pers.calcWorldToScreen3( disp[t.n0] );
		vect3 v1 = pers.calcWorldToScreen3( disp[t.n1] );
		vect3 v2 = pers.calcWorldToScreen3( disp[t.n2] );
		if ( v0.z>0 )
		{
			gra.Tri( v0,v1,v2, rgb(1,0,1));
			gra.Tri( v2,v1,v0, rgb(1,0,1)/2);
		}

	}
	for ( ivect2 e : edge )
	{
		const vect3& a = disp[e.p];
		const vect3& b = disp[e.n];
		const rgb col = rgb(0,1,1);

		pers.line3d( gra, pers, a, b, col, false );
	}
}
