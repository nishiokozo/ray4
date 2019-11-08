#include <iostream>
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
	sz = fy/tan(deg2rad(fovy)/2);				// 投影面までの距離
#else
	//投影面までの距離基準	こちらの方がニアクリップがしやすく扱いやすい。
	sz = 1.0;								// 投影面までの距離
	sz = 1.0/8.0;								// 投影面までの距離
	fy = sz*tan(deg2rad(fovy)/2);				// 投影面の高さ/2
#endif
	aspect	= screensize.x/screensize.y;	// 描画画面のアスペクト比

} 

//--------------------------------------------------------------------------
float Pers::getW( float z ) const
//--------------------------------------------------------------------------
{
	return 1/((fy/sz)*(z+sz));	// 投影面のz位置を0とした場合のzに対するw値を求める。
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
	auto[b,t,Q] = func_distance_Plate_Line( vect3(0,0,0), vect3(0,1,0), P, I );
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

		function<vect3(vect3,vect3,int)>nearclip = [ this, &nearclip, NZ, FZ ]( vect3 a, vect3 b, int n )
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
void Pers::Focus( vector<vector<Obj*>>& tbls )
//------------------------------------------------------------------------------
{
	vect3 b(-infinit,-infinit,-infinit);
	vect3 s(infinit,infinit,infinit);
	vect3 b2;
	vect3 s2;

	for ( vector<Obj*>& tblObj : tbls )
	{
		for ( Obj* p : tblObj )
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
void Pers::tri3d( SysGra& gra, Pers& pers, vect3 p0, vect3 p1, vect3 p2, rgb col )
//------------------------------------------------------------------------------
{
	vect3 v0 = pers.calcWorldToScreen3( p0 );
	vect3 v1 = pers.calcWorldToScreen3( p1 );
	vect3 v2 = pers.calcWorldToScreen3( p2 );
	if ( v0.z > 0 && v1.z > 0 && v2.z > 0 ) gra.Tri( v0, v1, v2, col );
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

////////////////
// Camera
////////////////
const static float g_nearist = 0.00000001f*100; 
//------------------------------------------------------------------------------
void Pers::Cam::Rotation( vect3 mov )
//------------------------------------------------------------------------------
{
	float len = (pos-at).abs();
	float l = (pos-at).abs()/10;
	l=max(l,0.0000001*2);
	l=min(l,8);

	float eyeup = dot( (pos-at).normalize(), up ); // 視線ベクトルとupベクトルの織り成すcos th
	if ( eyeup > sin(deg2rad(80)) )
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

	at += v;
	pos += v;
}

//------------------------------------------------------------------------------
void Pers::Cam::Zoom( float step )
//------------------------------------------------------------------------------
{
#if 1
	vect3	v= vect3(0,0,step);
	mat33 mrot = mat.GetRotate();
	v = v * mrot;

	vect3 p = pos;
	pos += v;
	if( (pos-at).abs() <= v.abs() ) pos = (p-at).normalize()*g_nearist+at;
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
pos = (pos-at).normalize()*g_nearist+at;
cout << "a" << endl;
pos.dump();
at.dump();
	}
	else
	{
		vect3 p = pos;
		pos += v;
	}


//	if( (pos-at).abs() <= v.abs() ) pos = (p-at).normalize()*g_nearist+at;
#endif
}

//------------------------------------------------------------------------------
void Pers::Cam::Update()
//------------------------------------------------------------------------------
{
	mat.LookAt( pos, at, up );
}

////////////////
// Grid
////////////////
//------------------------------------------------------------------------------
void Pers::Grid::DrawGrid3d( SysGra& gra, Pers& pers,  vect3 pos, mat33 m, int NUM_U, int NUM_V, float dt, rgb col  )
//------------------------------------------------------------------------------
{	// ミニグリッド
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
			pers.line3d_scissor( gra, pers, v0, v1, col );
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
			pers.line3d_scissor( gra, pers, v0, v1, col );
			a+=vt;
			b+=vt;
		}
	}			

	if(1)
	{//原点表示
		float r = 0.1;
		vect3 a;
		for ( float th = 0 ; th <= deg2rad(360) ; th+=deg2rad(20) )
		{
			vect3 b = vect3( r*cos(th), 0, r*sin(th) ) + pos;
			if ( th > 0 ) 
			{
				vect3 v0 = a * m;
				vect3 v1 = b * m;
				pers.line3d( gra, pers, v0,v1, col );
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
				vect3 v0 = vect3( f, 0, -0.025 ) * m;
				vect3 v1 = vect3( f, 0,  0.025 ) * m;
				pers.line3d( gra, pers, v0,v1, col );
			}
			{
				vect3 v0 = vect3( -0.025, 0, f ) * m;
				vect3 v1 = vect3(  0.025, 0, f ) * m;
				pers.line3d( gra, pers, v0,v1, col );
			}
		}
	}
}
