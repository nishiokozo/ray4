#include <iostream>
#include <memory>
#include <functional>
#include <tuple>

#include "geom.h"

#include "SysGra.h"

#include "func.h"

#include "pers.h"


using namespace std;
//--------------------------------------------------------------------------
Pers::Pers()
//--------------------------------------------------------------------------
{
	fovy=90/2;
	rate_w = 1.0f/64;	// wバッファに書き込むレート

}

//--------------------------------------------------------------------------
void Pers::Update( vect2 screensize )
//--------------------------------------------------------------------------
{
#if 0
	//投影面サイズ基準
	fy = 1;									// 投影面の高さ/2
	sz = fy/tan(rad(fovy)/2);				// 投影面までの距離
#else
	//投影面までの距離基準	こちらの方がニアクリップがしやすく扱いやすい。
	sz = 1.0;								// 投影面までの距離
	sz = 1.0/8.0;								// 投影面までの距離
	fy = sz*tan(rad(fovy)/2);				// 投影面の高さ/2
#endif
	aspect	= screensize.x/screensize.y;	// 描画画面のアスペクト比

} 

//--------------------------------------------------------------------------
float Pers::getW( float z ) const
//--------------------------------------------------------------------------
{
	if ( bOrtho )
	{
		// 平行投影
		return 1.0;
	}
	else
	{
		return 1/((fy/sz)*(z+sz));	// 投影面のz位置を0とした場合のzに対するw値を求める。
	}
}


//--------------------------------------------------------------------------
vect3 Pers::calcViewScreen3( vect3 v )	// 透視変換
//--------------------------------------------------------------------------
{
	vect3 ret;
	{
		float w = getW(v.z);
		ret.x = v.x* w / aspect;
		ret.y = v.y* w 	;
		ret.z = w*rate_w;	// 三次元ベクトルで返す都合上、ZにW値を入れている。
	}

	return ret;
}
//--------------------------------------------------------------------------
vect2 Pers::calcDisp2( vect3 v )	// 透視変換
//--------------------------------------------------------------------------
{
	v = calcViewScreen3(v);
	return vect2( v.x, v.y );
}

//--------------------------------------------------------------------------
vect3 Pers::calcWorldToScreen3( vect3 v )	// 透視変換
//--------------------------------------------------------------------------
{
	v = v * cam.mat.invers();
	return calcViewScreen3(v);
}

//--------------------------------------------------------------------------
vect2 Pers::calcWorldToScreen2( vect3 v )	// 透視変換
//--------------------------------------------------------------------------
{
	v = calcWorldToScreen3(v);
	return vect2(v.x,v.y);
}

//--------------------------------------------------------------------------
vect3 Pers::calcScreenToWorld3( vect3 q )	// 透視変換後の画面座標から空間座標を求める。
//--------------------------------------------------------------------------
{
	vect3 v;
	float w = getW(0);
	v.x = q.x/(w /aspect);
	v.y = q.y/(w);
	v.z = q.z/(w);
	return v * cam.mat;
}

//--------------------------------------------------------------------------
vect3 Pers::calcScreenToWorld2( vect2 q )	// 透視変換後の画面座標から空間座標を求める。
//--------------------------------------------------------------------------
{
	return calcScreenToWorld3( vect3(q,0) );
//	return calcScreenToWorld3( vect3(q,getW(0)) );
//	return calcScreenToWorld3( vect3(q,1.0/getW(0)) );
//	return calcScreenToWorld3( vect3(q,sz) );
}

//--------------------------------------------------------------------------
vect3 Pers::calcRayvect( vect3 v1 ) 
//--------------------------------------------------------------------------
{
	vect3 v0 = vect3(0,0,-sz) * cam.mat; // 投影原点

	vect3 v = (v1-v0).normalize(); // 正規化

	return v;
}

//--------------------------------------------------------------------------
tuple<bool,vect3> Pers::calcScreenToGround( vect2 q )	// 透視変換後の画面座標から『床』上の座標を求める。
//--------------------------------------------------------------------------
{
	vect3 P = calcScreenToWorld2( q );
	vect3 I = calcRayvect( P );
	auto[b,Q,s] = func_intersect_Plate_HarfLine( vect3(0,0,0), vect3(0,1,0), P, I );
	return {b,Q};
}

//--------------------------------------------------------------------------
bool Pers::calcScissorLine3d( vect3 v0, vect3 v1, vect3& va, vect3& vb )	// 3D 空間座標のままシザリングを行う。
//--------------------------------------------------------------------------
{
	va = calcViewScreen3(v0);
	vb = calcViewScreen3(v1);

	{//シザリング ニアクリップ
		const float NZ = 0;//getW(0)/rate_w;

		const float FZ = 0;

		function<vect3(vect3,vect3,int)>nearclip = [ &nearclip, NZ, FZ ]( vect3 a, vect3 b, int n )
		{
			if (n <=0 ) return b;

			vect3 c =  (a+b)/2;
	
			if ( c.z < NZ )
			{
				c = nearclip( a, c, n-1 );
			}
			if ( c.z > FZ )
			{
				c = nearclip( c, b, n-1 );
			}
			return c;
		};

		if ( va.z > 0|| vb.z > 0 )
		{
			if ( va.z < NZ )
			{
				vect3 c = nearclip(v1,v0,8);
				va = calcViewScreen3(c);
			}
			if ( vb.z < NZ )
			{
				vect3 c = nearclip(v0,v1,8);
				vb = calcViewScreen3(c);
			}
		}
	}
	return ( va.z > 0 && vb.z > 0 );
}

//------------------------------------------------------------------------------
void Pers::Focus( vector<vector<shared_ptr<Obj>>>& tbltbl_pObj )
//------------------------------------------------------------------------------
{
	vect3 b(-infinit,-infinit,-infinit);
	vect3 s(infinit,infinit,infinit);
	vect3 b2;
	vect3 s2;

	for ( vector<shared_ptr<Obj>>& tbl_pObj : tbltbl_pObj )
	{
		for ( shared_ptr<Obj> p : tbl_pObj )
		{
			if ( p->bSelected )
			{
				vect3 v = calcWorldToScreen3( p->pos );
				if ( b.x < v.x ) { b2.x = p->pos.x; b.x=v.x; }
				if ( b.y < v.y ) { b2.y = p->pos.y; b.y=v.y; }
				if ( b.z < v.z ) { b2.z = p->pos.z; b.z=v.z; }
				if ( s.x > v.x ) { s2.x = p->pos.x; s.x=v.x; }
				if ( s.y > v.y ) { s2.y = p->pos.y; s.y=v.y; }
				if ( s.z > v.z ) { s2.z = p->pos.z; s.z=v.z; }
			}
		}
	}
	vect3 v2 = ave(b2,s2);
	cam.pos = cam.pos - cam.at + v2;
	cam.at = v2;
}



////////////////
// Camera
////////////////
//------------------------------------------------------------------------------
void Pers::Cam::Rotation( vect3 mov )
//------------------------------------------------------------------------------
{
	float len = (pos-at).abs();
	float l = (pos-at).abs()/10;
	l=max(l,0.0000001*2);
	l=min(l,8);

	float eyeup = dot( (pos-at).normalize(), up ); // 視線ベクトルとupベクトルの織り成すcos th
	if ( eyeup > sin(rad(80)) )
	{
		if ( mov.y > 0 ) mov.y = 0;
	}

	{
		// 回転
		vect3	v = mov * l;
		mat33 mrot = mat.GetRotate();
		v = v* mrot;

		{
			pos += v;
			
			// 正規化
			{
				vect3 dir = (pos-at).normalize();
				pos = at+dir*len;
			}
		}
	}
}

//------------------------------------------------------------------------------
void Pers::Cam::Move( vect3 v )
//------------------------------------------------------------------------------
{
	mat33 mrot = mat.GetRotate();
	v = v* mrot;

v = v * vect3(1.5,1,1);
	at += v;
	pos += v;
}

//------------------------------------------------------------------------------
void Pers::Cam::Zoom( float step )
//------------------------------------------------------------------------------
{
	const float varNearist = 0.00000001f*100; 
#if 1
	vect3	v= vect3(0,0,step);
	mat33 mrot = mat.GetRotate();
	v = v * mrot;

	vect3 p = pos;
	pos += v;
	if( (pos-at).abs() <= v.abs() ) pos = (p-at).normalize()*varNearist+at;
#else
	vect3	v= vect3(0,0,step);
	mat33	mrot = mat.GetRotate();
	v = v * mrot;

//vect3 a = (pos-at).normalize() * step;

	if( (pos-at).abs() <= v.abs() ) 
	{
//		vect3 p = pos;
//		pos += v;
//		pos += v;
pos = (pos-at).normalize()*varNearist+at;
cout << "a" << endl;
pos.dump();
at.dump();
	}
	else
	{
		vect3 p = pos;
		pos += v;
	}


//	if( (pos-at).abs() <= v.abs() ) pos = (p-at).normalize()*varNearist+at;
#endif
}

//------------------------------------------------------------------------------
void Pers::Cam::Update()
//------------------------------------------------------------------------------
{
	mat.LookAt( pos, at, up );
}


//------------------------------------------------------------------------------
void Pers::Pen::line3d_scissor( SysGra& gra, Pers& pers, vect3 p0, vect3 p1, rgb col, float wide )
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
void Pers::Pen::line3d( SysGra& gra, Pers& pers, vect3 p0, vect3 p1, rgb col, float wide )
//------------------------------------------------------------------------------
{
	vect3 v0 = pers.calcWorldToScreen3( p0 );
	vect3 v1 = pers.calcWorldToScreen3( p1 );
	if ( v0.z > 0 && v1.z > 0 ) gra.Line( v0, v1, col, wide );
}

//------------------------------------------------------------------------------
void Pers::Pen::tri3d( SysGra& gra, Pers& pers, vect3 p0, vect3 p1, vect3 p2, rgb col )
//------------------------------------------------------------------------------
{
	vect3 v0 = pers.calcWorldToScreen3( p0 );
	vect3 v1 = pers.calcWorldToScreen3( p1 );
	vect3 v2 = pers.calcWorldToScreen3( p2 );
	if ( v0.z > 0 && v1.z > 0 && v2.z > 0 ) gra.Tri( v0, v1, v2, col );
}

//------------------------------------------------------------------------------
void Pers::Pen::pset3d( SysGra& gra, Pers& pers, vect3 p0, rgb col, float wide )
//------------------------------------------------------------------------------
{
	vect3 v0 = pers.calcWorldToScreen3( p0 );
	if ( v0.z > 0 ) gra.Pset( v0, col, wide);
}

//------------------------------------------------------------------------------
void Pers::Pen::print3d( SysGra& gra, Pers& pers, vect3 p0, float x, float y, string str )
//------------------------------------------------------------------------------
{
	vect3 v0 = pers.calcWorldToScreen3( p0 );
	if ( v0.z > 0 ) gra.Print( v0.xy() + gra.Dot(x,y), str ); 
}

//------------------------------------------------------------------------------
void Pers::Pen::circle3d( SysGra& gra, Pers& pers, vect3 p0, float radius, rgb col, float wide )
//------------------------------------------------------------------------------
{
	vect3 v0 = pers.calcWorldToScreen3( p0 );
	float r = radius*v0.z;

	if ( v0.z > 0 ) gra.Circle( v0, r, col, wide );

}

// ~2d関数はXY平面、z=0に描画する。
//------------------------------------------------------------------------------
void Pers::Pen::pset2d( SysGra& gra, Pers& pers, vect2 p0, rgb col, float wide )
//------------------------------------------------------------------------------
{
	(*this).pset3d( gra, pers, vect3(p0,0), col, wide );
}

//------------------------------------------------------------------------------
void Pers::Pen::print2d( SysGra& gra, Pers& pers, vect2 p0, float x, float y, string str )
//------------------------------------------------------------------------------
{
	(*this).print3d( gra, pers, vect3(p0,0), 0, 0, str );

}
//------------------------------------------------------------------------------
void Pers::Pen::line2d( SysGra& gra, Pers& pers, vect2 p0, vect2 p1, rgb col, float wide )
//------------------------------------------------------------------------------
{
	(*this).line3d( gra, pers, vect3(p0,0), vect3(p1,0), col, wide );

}

//------------------------------------------------------------------------------
void Pers::Pen::DrawBezier( SysGra& gra, Pers& pers, vect3 P0, vect3 P1, vect3 P2, vect3 P3 )
//------------------------------------------------------------------------------
{
	int div = 16;
	float dt = 1/(float)div;

	float t  = dt;
	vect3 pos0 = P0;
	for ( int i = 0 ; i < div ; i++ )
	{
		vect3 pos1 = func_bezier3( t, P0, P1, P2, P3 );
		pers.pen.line3d( gra, pers, pos0, pos1, rgb(1,1,1) );

		pos0=pos1;
		t+=dt;
	}
}

//------------------------------------------------------------------------------
void Pers::Pen::DrawBezierSurface(
//------------------------------------------------------------------------------
	SysGra& gra, Pers& pers,
	vect3 p0 , vect3 p1 , vect3 p2 , vect3 p3,
	vect3 p0u, vect3 p1u, vect3 p2u, vect3 p3u,
	vect3 p0v, vect3 p1v, vect3 p2v, vect3 p3v
)
{
	// 0--1
	// |  |
	// 2--3

	int udiv = pow(2,5.0);	// floatで計算誤差が出ないよう2^nにしておく
	int vdiv = pow(2,5.0);	// floatで計算誤差が出ないよう2^nにしておく

	// U
	for ( float ut = 0 ; ut <= 1.0 ; ut += 1.0/udiv )
	{
		vect3 vP0 = func_bezier3( ut, p0, p0+p0u, p1+p1u, p1 );
		vect3 vP3 = func_bezier3( ut, p2, p2+p2u, p3+p3u, p3 );
		vect3 vP1 = vP0 + (p0v)*(1.0-ut) + (p1v)*ut;
		vect3 vP2 = vP3 + (p2v)*(1.0-ut) + (p3v)*ut;

		// V
		for ( float vt = 0 ; vt <= 1.0 ; vt +=1.0/vdiv )
		{
			vect3 vpos = func_bezier3( vt, vP0, vP1, vP2, vP3 );
			pers.pen.pset3d( gra, pers, vpos, rgb(1,1,1), 1 );
		}
	}
}


/////////////////////
// Prim
/////////////////////
//------------------------------------------------------------------------------
void Pers::Prim::DrawPlate( SysGra& gra, Pers& pers, vect3 pos, vect3 normal, int n, rgb col )
//------------------------------------------------------------------------------
{
	mat33 m = mslerp( normal, 1.0 );

	//	仮想平面表示
	float r = 0.25;
	for ( int i = 0 ; i < n ; i++  )
	{
//		r = (float)(1+i)*0.25 + i*i/2.0;
		if ( i == 0 ) r = 0.05;
		else r = (float)(i)*0.5+i*i*0.25;
	
		vect3 a;
		for ( float th = 0 ; th <= rad(360) ; th+=rad(10) )
		{
			vect3 b = vect3( r*cos(th), r*sin(th), 0 ) ;
			if ( th > 0 ) 
			{
				vect3 v0 = a * m;
				vect3 v1 = b * m;
				pers.pen.line3d( gra, pers, v0+ pos, v1+ pos, col );
			}
			a = b;
		}
	}

		pers.pen.line3d( gra, pers, pos, pos+ normal*0.2, col, 3 );
		pers.pen.pset3d( gra, pers, pos+ normal*0.2, col, 5 );

}

/*
//------------------------------------------------------------------------------
void Pers::Prim::DrawCircle( SysGra& gra, Pers& pers, vect3 pos, vect3 normal, float radius, rgb col )
//------------------------------------------------------------------------------
{
	mat33 m = mslerp( normal, 1.0 );	// 法線方向を向いたマトリクスを作成

	vect3 a;
	for ( float th = 0 ; th <= rad(360) ; th+=rad(10) )
	{
		vect3 b = vect3( radius*cos(th), radius*sin(th), 0 ) ;
		if ( th > 0 ) 
		{
			vect3 v0 = a * m;
			vect3 v1 = b * m;
			pers.pen.line3d( gra, pers, v0+ pos, v1+ pos, col );
		}
		a = b;
	}
}
*/

//------------------------------------------------------------------------------
void Pers::Prim::DrawCircle( SysGra& gra, Pers& pers, vect3 pos, mat33 m, float radius, rgb col )
//------------------------------------------------------------------------------
{
//	mat33 m = mslerp( normal, 1.0 );	// 法線方向を向いたマトリクスを作成

	vect3 a;
	for ( float th = 0 ; th <= rad(360) ; th+=rad(10) )
	{
		vect3 b = vect3( radius*cos(th), radius*sin(th), 0 ) ;
		if ( th > 0 ) 
		{
			vect3 v0 = a * m;
			vect3 v1 = b * m;
			pers.pen.line3d( gra, pers, v0+ pos, v1+ pos, col );
		}
		a = b;
	}
	{
		vect3	v0 = vect3(-1,0,0) * m + pos;
		vect3	v1 = vect3( 1,0,0) * m + pos;
		pers.pen.line3d( gra, pers, v0, v1, col );
	}
	{
		vect3	v0 = vect3( 0,-1,0) * m + pos;
		vect3	v1 = vect3( 0, 1,0) * m + pos;
		pers.pen.line3d( gra, pers, v0, v1, col );
	}
}

//------------------------------------------------------------------------------
void Pers::Prim::DrawMat33(  SysGra& gra, Pers& pers, vect3 v0, mat33 m )
//------------------------------------------------------------------------------
{
	vect3 a = {m.m[0][0],m.m[0][1],m.m[0][2]};
	vect3 b = {m.m[1][0],m.m[1][1],m.m[1][2]};
	vect3 c = {m.m[2][0],m.m[2][1],m.m[2][2]};

	pers.pen.line3d( gra, pers, v0		, v0+a/2, rgb(1,0,0), 1 );
	pers.pen.line3d( gra, pers, v0		, v0+b/2, rgb(0,1,0), 1 );
	pers.pen.line3d( gra, pers, v0		, v0+c/2, rgb(0,0,1), 1 );
	pers.pen.line3d( gra, pers, v0+a/2	, v0+c/2, rgb(0.5,0.5,0.5), 1 );
	pers.pen.line3d( gra, pers, v0+b/2	, v0+c/2, rgb(0.5,0.5,0.5), 1 );
	pers.pen.line3d( gra, pers, v0+b/2	, v0+a/2, rgb(0.5,0.5,0.5), 1 );

}


//------------------------------------------------------------------------------
void Pers::Prim::DrawBox( SysGra& gra, Pers& pers, vect3 pos, mat33 m , bool bAxis, bool bTri )
//------------------------------------------------------------------------------
{
	const float S = 0.04f;
	const float L = 0.03f;
	const float M = 0.02f;
	const float N = 0.02f;
	vector<vect3> vert=
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
	vector<vect3> disp;

	vector<ivect2>	edge
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
	vector<ivect3>	tri
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
		pers.pen.line3d( gra, pers, pos, pos+nx*0.2, rgb(1,0,0) );
		pers.pen.line3d( gra, pers, pos, pos+ny*0.2, rgb(0,1,0) );
		pers.pen.line3d( gra, pers, pos, pos+nz*0.2, rgb(0,0,1) );
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

		pers.pen.line3d( gra, pers, a, b, col, false );

	}
}
	// ドラム
//------------------------------------------------------------------------------
void Pers::Prim::DrawDrum( SysGra& gra, Pers& pers,  vect3 pos, mat33 m  )
//------------------------------------------------------------------------------
{
	const float S = 0.05f;
	const float L = 0.03f;
	const float M = 0.02f;
	const float N = 0.02f;
	vector<vect3> vert=
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
	vector<vect3> disp;

	vector<ivect2>	edge
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
	vector<ivect3>	tri
	{
//				{	14,	13, 12	},	// xマーク
	};

	bool bInitParam = false;
	if ( !bInitParam )
	{
		{
			int	ofs = (signed)vert.size();
			int cnt = 0;
			float r = 0.05;
			float w = 0.05;
			for ( int i = 0 ; i < 360 ; i+= 30 )
			{
				float z = r*cos(rad((float)i));
				float y = r*sin(rad((float)i));
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
		bInitParam = true;
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
		pers.pen.line3d( gra, pers, pos, pos+nx*0.2, rgb(1,0,0) );
		pers.pen.line3d( gra, pers, pos, pos+ny*0.2, rgb(0,1,0) );
		pers.pen.line3d( gra, pers, pos, pos+nz*0.2, rgb(0,0,1) );
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

		pers.pen.line3d( gra, pers, a, b, col, false );
	}
}

// 球
/*
	tuple<vect3,vect3,vect3> func( float th, float r, mat33& m, vect3& pos )
	{
		float u = r*cos(th);
		float v = r*sin(th);
	
		vect3	vx1 = m*vect3(0,u,v)+pos;
		vect3	vy1 = m*vect3(u,0,v)+pos;
		vect3	vz1 = m*vect3(u,v,0)+pos;
		return {vx1, vy1, vz1};
	};
*/
//------------------------------------------------------------------------------
void Pers::Prim::DrawSphere( SysGra& gra, Pers& pers, vect3 pos, mat33 m, float r, rgb col  )
//------------------------------------------------------------------------------
{
//	float step = 2*pi/4/8;//48.0;
//	rgb col = rgb(0,1,1);


	vect3	vx0;
	vect3	vy0;
	vect3	vz0;
	float fmax = 48;
	for ( float f = 0 ; f <= fmax ; f++ )
	{
		float th = 	f*2*pi/fmax;
#if 0
		auto func = [&]( float th, float r, mat33& m, vect3& pos, vect3& vx1, vect3& vy1, vect3& vz1 )
		{
			float u = r*cos(th);
			float v = r*sin(th);
		
			vx1 = m*vect3(0,u,v)+pos;
			vy1 = m*vect3(u,0,v)+pos;
			vz1 = m*vect3(u,v,0)+pos;
		};
		vect3 vx1, vy1, vz1;
		func( th, r, m, pos, vx1, vy1, vz1 );
#else
		float u = r*cos(th);
		float v = r*sin(th);

		vect3	vx1 = m*vect3(0,u,v)+pos;
		vect3	vy1 = m*vect3(u,0,v)+pos;
		vect3	vz1 = m*vect3(u,v,0)+pos;
#endif
		if ( f > 0 )
		{
			pers.pen.line3d( gra, pers, vx0, vx1, col );
			pers.pen.line3d( gra, pers, vy0, vy1, col );
			pers.pen.line3d( gra, pers, vz0, vz1, col );
		}
		vx0 = vx1;
		vy0 = vy1;
		vz0 = vz1;
	}
	
}

//------------------------------------------------------------------------------
void Pers::Prim::DrawTire( SysGra& gra, Pers& pers, vect3 pos, float head, float bank, float radius )
//------------------------------------------------------------------------------
{
	vector<vect3> vert;
	vector<vect3> disp;
	bool bInitParam = false;
	if ( !bInitParam )
	{
		bInitParam = true;
		float r = 1.0;
		for ( float t = 0 ; t < 2*pi ; t+=rad(5) )
		{
			float x = 0;
			float y = r * sin(t);
			float z = r * cos(t);

			vert.emplace_back( x, y, z );
		}
	}
	
	mat33 m = mrotz(bank) * mroty(head);

	rgb col(0,1,1);
	int i = 0;
	vect3 v0;
	for ( vect3 v : vert )
	{
		vect3 v1 = v*radius * m + pos;

		if ( i > 0 ) 
		{
			pers.pen.line3d( gra, pers, v0, v1, col, 2 );

			// 影
			vect3 a0= v0;
			vect3 a1= v1;
			a0.y=0;
			a1.y=0;
			pers.pen.line3d( gra, pers, a0, a1, col/4, 2 );
		}

		v0 = v1;
		i++;
	}

}

//------------------------------------------------------------------------------
void Pers::Prim::DrawSquare( SysGra& gra, Pers& pers, vect3 pos, mat33 m , bool bAxis, bool bTri )
//------------------------------------------------------------------------------
{
	const float s = 0.04f;
	vector<vect3> vert=
	{
		{	-s,		 s,		0	},//0
		{	 s,		 s,		0	},//1
		{	-s,		-s,		0	},//2
		{	 s,		-s,		0	},//3

	};
	vector<vect3> disp;

	vector<ivect2>	edge
	{
		{	0,	1	},
		{	1,	3	},
		{	3,	2	},
		{	2,	0	},
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
		pers.pen.line3d( gra, pers, pos, pos+nx*0.2, rgb(1,0,0) );
		pers.pen.line3d( gra, pers, pos, pos+ny*0.2, rgb(0,1,0) );
		pers.pen.line3d( gra, pers, pos, pos+nz*0.2, rgb(0,0,1) );
	}
	
	int cnt = 0;
	for ( ivect2 e : edge )
	{
		const vect3& a = disp[e.p];
		const vect3& b = disp[e.n];
		rgb col = rgb(0,1,1);
		if ( cnt == 0 ) col = rgb(1,0,0);
		//if ( cnt == 1 ) col = rgb(0,1,0);
		cnt++;
		pers.pen.line3d( gra, pers, a, b, col, false );

	}
}


////////////////
// Axis
////////////////
//------------------------------------------------------------------------------
void Pers::Axis::DrawAxis( SysGra& gra, Pers& pers, vect2 mpos )
//------------------------------------------------------------------------------
{
	gra.SetZTest( false );

	vect3 pos = pers.calcScreenToWorld3( vect3(mpos,0) );

	vect3 v0 = pers.calcWorldToScreen3( pos );

	// 軸表示
	float l = 0.1;
	if ( bAxisX  )
	{
		vect3 v1 = v0 + vect3(
			pers.cam.mat.m[0][0] / pers.aspect,
			pers.cam.mat.m[1][0],
			pers.cam.mat.m[2][0]
		) * l;
		gra.Line( v0, v1, rgb(0.8,0.2,0.2), 2.0 );
	}
	if ( bAxisY  )
	{
		vect3 v1 = v0 + vect3(
			pers.cam.mat.m[0][1] / pers.aspect,
			pers.cam.mat.m[1][1],
			pers.cam.mat.m[2][1]
		) * l;
		gra.Line( v0, v1, rgb(0.2,0.8,0.2), 2.0 );
	}
	if ( bAxisZ )
	{
		vect3 v1 = v0 + vect3(
			pers.cam.mat.m[0][2] / pers.aspect,
			pers.cam.mat.m[1][2],
			pers.cam.mat.m[2][2]
		) * l;
		gra.Line( v0, v1, rgb(0.1,0.3,1), 2.0 );

	}

	// 軸名表示
	gra.Print( mpos+gra.Dot(16,-12),string("")+(bAxisX?"X":"")+(bAxisY?"Y":"")+(bAxisZ?"Z":"") ); 


	gra.Pset( v0, rgb(1,1,1),3 );

	gra.SetZTest( true );

}
		
////////////////
// Grid
////////////////
//------------------------------------------------------------------------------
void Pers::Grid::DrawGrid3d( SysGra& gra, Pers& pers, vect3 pos0, mat33 m, int NUM_U, int NUM_V, float dt, rgb col, bool bPlot  )
//----------------------------------------------------------------------------
{
	mat = m;

	vect3 pos = pos0;

	// 注視点中心に展開
	{
		vect3 tar = pers.cam.at;
		vect3 nor = m.GetVectY();

		auto[b0,q0,s0] = func_intersect_Dualplate_Line( pos, nor, tar, nor );

		vect3 q1 = (q0-pos) * m.invers();

		q1.x = (float)int( q1.x );
		q1.y = (float)int( q1.y );
		q1.z = (float)int( q1.z );
		pos += q1;	
	}


	// ミニグリッド
	vect3 vt = vect3(0,0,0);
	float du = (float)NUM_U*dt;
	float dv = (float)NUM_V*dt;
	vect3 a;
	vect3 b;
	{
		a = pos+vect3(-du, 0,-du);
		b = pos+vect3( du, 0,-du);
		vt = vect3(0,0,dt);
		for ( int i = 0 ; i < NUM_V*2+1 ; i++ )
		{
			vect3 v0 = a * m;
			vect3 v1 = b * m;
			pers.pen.line3d_scissor( gra, pers, v0, v1, col );
			a+=vt;
			b+=vt;
		}
	}			
	{

		a = pos+vect3(-dv, 0, dv);
		b = pos+vect3(-dv, 0,-dv);
		vt = vect3(dt,0,0);
		for ( int i = 0 ; i < NUM_U*2+1 ; i++ )
		{
			vect3 v0 = a * m;
			vect3 v1 = b * m;
			pers.pen.line3d_scissor( gra, pers, v0, v1, col );
			a+=vt;
			b+=vt;
		}
	}			

	{//原点円表示
		float r = 0.1;
		vect3 a;
		for ( float th = 0 ; th <= rad(360) ; th+=rad(20) )
		{
			vect3 b = vect3( r*cos(th), 0, r*sin(th) ) + pos0;
			if ( th > 0 ) 
			{
				vect3 v0 = a * m;
				vect3 v1 = b * m;
				pers.pen.line3d( gra, pers, v0,v1, col );
			}
			a = b;
		}
	}
	
	{// 10cmメモリ
		vect3 a;
		for ( float f = -1.0 ; f <= 1.0 ; f+=0.1 )
		{
			if ( abs(f)<=0.11 ) continue;
			{
				vect3 v0 = ( vect3( f, 0, -0.025 ) + pos0 ) * m;
				vect3 v1 = ( vect3( f, 0,  0.025 ) + pos0 ) * m;
				pers.pen.line3d( gra, pers, v0,v1, col );
			}
			{
				vect3 v0 = ( vect3( -0.025, 0, f ) + pos0 ) * m;
				vect3 v1 = ( vect3(  0.025, 0, f ) + pos0 ) * m;
				pers.pen.line3d( gra, pers, v0,v1, col );
			}
		}
	}

	// プロット表示
	if ( bPlot ) plot.DrawPlot( gra, pers, m );

}

//------------------------------------------------------------------------------
void Pers::Grid::DrawGrid( SysGra& gra, Pers& pers )
//----------------------------------------------------------------------------
{
	DrawGrid3d( gra, pers, vect3(0,0,0), mat, 10, 10, 1, col );
}
//------------------------------------------------------------------------------
void Pers::Grid::line( SysGra& gra, Pers& pers, vect2 p0, vect2 p1, rgb col, float wide )
//----------------------------------------------------------------------------
{
	gra.SetZTest(false);

	mat33 m = mat * mrotx(-rad(90));

	vect3 v0 = m * vect3(p0,0);	
	vect3 v1 = m * vect3(p1,0);	
	pers.pen.line3d( gra, pers, v0, v1, col, wide );

	gra.SetZTest(true);
}
//------------------------------------------------------------------------------
void Pers::Grid::print( SysGra& gra, Pers& pers, vect2 p0, float x, float y, string str )
//----------------------------------------------------------------------------
{
	gra.SetZTest(false);

	mat33 m = mat * mrotx(-rad(90));

	vect3 v0 = m * vect3(p0,0);	
	pers.pen.print3d( gra, pers, v0, x,y,str );


	gra.SetZTest(true);
}

//////////////////
// GRID::PLOT			
//////////////////
//------------------------------------------------------------------------------
Pers::Grid::Plot::Plot( 
//------------------------------------------------------------------------------
	int	_max,		// プロット数
	float _step,	// 送りステップ
	rgb _col 		// 色
)
{
	MaxPlot = _max;

	col = _col;
	step = _step;
	tblDot.clear();
	ResetPlot();
}

//------------------------------------------------------------------------------
void Pers::Grid::Plot::ResetPlot()
//------------------------------------------------------------------------------
{
	bScroll = false;
	cntPlot = 0 ;

	amtPlot = 0 ;
	tblDot.clear();
}
//------------------------------------------------------------------------------
void Pers::Grid::Plot::WritePlot( float val )
//------------------------------------------------------------------------------
{
	if ( cntPlot >= MaxPlot ) 
	{
		bScroll = true;
		cntPlot = 0;
	}


	if ( amtPlot >= MaxPlot ) 
	{
		int idx = amtPlot % MaxPlot;
		tblDot[idx] = val;
	}
	else
	{
		tblDot.emplace_back( val );
	}
	amtPlot++;
	cntPlot++;
}
//------------------------------------------------------------------------------
void Pers::Grid::Plot::DrawPlot( SysGra& gra, Pers& pers, mat33& m )
//------------------------------------------------------------------------------
{
	gra.SetZTest( false );
	
	float x = 0;
	
	if ( bScroll )
	for ( int i = cntPlot ; i < MaxPlot ; i++ )
	{
		vect3 v = m*vect3( x, 0, tblDot[i] );
		pers.pen.pset3d( gra, pers, v,  col, 2 );
		x += step;
	}
	for ( int i = 0 ; i < cntPlot-1 ; i++ )
	{
		vect3 v = m* vect3( x, 0, tblDot[i] );
		pers.pen.pset3d( gra, pers, v ,  col, 2 );
		x += step;
	}
	if ( cntPlot > 0 ) 
	{
		vect3 v = m*vect3( x, 0, tblDot[cntPlot-1] );

		pers.pen.pset3d( gra, pers, v,  rgb(1,1,1), 2 );
	}
	gra.SetZTest( true );
}
