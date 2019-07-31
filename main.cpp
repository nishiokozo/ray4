// 2017/07/07 ray3
// 2019/06/25 ray4

#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <cmath>
#include <functional>
using namespace std;

#include "geom.h"


#include "SysKeys.h"
#include "SysMouse.h"
#include "SysGra.h"
#include "Sys.h"




const	static	double INFINIT =  numeric_limits<double>::max();	//DBL_MAX

struct  Material
{
	vect3	C;					//	ベースカラー
	double	valReflectance;		//	反射率
	double	valRefractive;		//	屈折率
	double	valPower;			//	スペキュラ強度
	double	valEmissive;		//	エミッシブ
	double	valTransmittance;	//	透過率
};


struct	Surface : Material
{
	enum
	{
		STAT_NONE,
		STAT_OUT,
		STAT_FRONT,
		STAT_BACK,
	};

	double	t;
	bool	flg; 
	int		stat; 
		// 0:none
		// 1:back
		// 2:front
		// 3:inside

	vect3	Q;
	vect3	N;
	vect3	R;	//	Reflectionion
	vect3	Rr;	//	Refractive	

	Surface()
	{
		t		= INFINIT;
		flg		= false;
		stat	= 0;
		Q = vect3(0,0,0);
		N = vect3(0,0,0);
		R = vect3(0,0,0);
		Rr = vect3(0,0,0);
	}
};



struct PrimSphere : public Material
{
	vect3	P;
	double	r;

	PrimSphere( const vect3& _p, double _r, vect3 _c, double _valReflection, double _valRefractive, double _valPower, double _valEmissive, double _valTransmittance )
	{
		P = _p;
		r = _r;
		C = max(0.0,min(1.0,_c));
		valReflectance	= _valReflection;
		valRefractive	= _valRefractive;
		valPower= _valPower;
		valEmissive	= _valEmissive;
		valTransmittance	= _valTransmittance;
	};
};
struct PrimPlate : public Material
{
	vect3	P;
	vect3	N;

	PrimPlate( const vect3& _p, const vect3& _n, vect3 _c, double _valReflection, double _valRefractive, double _valPower, double _valEmissive, double _valTransmittance )
	{
		P = _p;
		N = _n;
		C = _c;
		valReflectance = _valReflection;
		valRefractive	= _valRefractive;
		valPower = _valPower;
		valEmissive = _valEmissive;
		valTransmittance = _valTransmittance;
	};
};

struct	PrimLight
{
	vect3	P;
	vect3	C;

	PrimLight( const vect3& _p, const vect3& _c )
	{
		P = _p;
		C = _c;
	}
};

////////////////////////////////////////////////////////////////////////////////
class	Renderer 
////////////////////////////////////////////////////////////////////////////////
{
	vector<PrimSphere*>	m_tblSphere;
	vector<PrimPlate*>	m_tblPlate;
	vector<PrimLight*>	m_tblLight;
	vect3	A;

public:
	int m_cntRay;


	//------------------------------------------------------------------------------
	Surface raycast( vect3& P, const vect3& I )
	//------------------------------------------------------------------------------
	{
		Surface sur;

		P += I*2e-10;

		sur.flg = false;

		sur.t  = INFINIT;
		sur.stat  = Surface::STAT_NONE;
		
		//	球
		for ( int i = 0 ; i < (signed)m_tblSphere.size() ; i++ )
		{
			PrimSphere&	obj = *m_tblSphere[i];

			vect3	O = obj.P;
			double	r = obj.r;

			vect3	OP = P-O;
			double	b = dot( I, OP );
			double	aa = r*r - dot(OP,OP)+ b*b;

			int	stat = 0;

			if ( aa >= 0 )
			{
				double t = - sqrt( aa ) - b;

				if ( t < 0 )
				{
					t = + sqrt( aa ) - b;
					stat++;
				}

				if ( sur.t >= t && t >= 0 )
				{
					stat += 2;

					sur.stat = stat;

					sur.t = t; 

					sur.Q = I * t + P;

					if ( stat == Surface::STAT_BACK )
					{
						sur.N = -normalize(sur.Q - O);
					}
					else
					{
						sur.N = normalize(sur.Q - O);
					}

					sur.C = obj.C;

					sur.R = reflect( I, sur.N );

					sur.valReflectance = obj.valReflectance;

					sur.valRefractive   = obj.valRefractive;

					sur.valPower = obj.valPower;

					sur.valEmissive = obj.valEmissive;

					sur.valTransmittance = obj.valTransmittance;

					sur.flg = true;
				}
			}
		}

		//	床
		for ( int i = 0 ; i < (signed)m_tblPlate.size() ; i++ )
		{
			PrimPlate&	obj = *m_tblPlate[i];

			double	f = dot(obj.N,P - obj.P);
			if ( f > 0 )
			{
				double	t = -f/dot(obj.N,I);

				if ( sur.t >= t && t >= 0 )
				{
					sur.stat = Surface::STAT_FRONT;

					sur.t = t; 

					sur.Q = I * t + P;

					sur.N = obj.N;

					if (   (fmod(sur.Q.x+10e3, 1.0) < 0.5 && fmod(sur.Q.z+10e3, 1.0) < 0.5 )
						|| (fmod(sur.Q.x+10e3, 1.0) > 0.5 && fmod(sur.Q.z+10e3, 1.0) > 0.5 ) )
					{
						sur.C = obj.C;
					}
					else
					{
						sur.C = obj.C * 0.5;
					}
		
					sur.R = reflect( I, obj.N );
		
					sur.valReflectance = obj.valReflectance;

					sur.valRefractive   = obj.valRefractive;

					sur.valPower = obj.valPower;

					sur.valEmissive = obj.valEmissive;

					sur.valTransmittance = obj.valTransmittance;


					sur.flg = true;
				}
				
			}

		}

		return sur;

	}

	//------------------------------------------------------------------------------
	void	Entry( PrimSphere* a )
	//------------------------------------------------------------------------------
	{
		m_tblSphere.emplace_back( a );

	}

	//------------------------------------------------------------------------------
	void	Entry( PrimPlate* a )
	//------------------------------------------------------------------------------
	{
		m_tblPlate.emplace_back( a );
	}

	//------------------------------------------------------------------------------
	void	Entry( PrimLight* a )
	//------------------------------------------------------------------------------
	{
		m_tblLight.emplace_back( a );
	}

	//------------------------------------------------------------------------------
	~Renderer()
	//------------------------------------------------------------------------------
	{
	}

	//------------------------------------------------------------------------------
	Renderer()
	//------------------------------------------------------------------------------
	{
		m_cntRay = 0;
		A = vect3(0,0,0);


		float r,s,pw,e,tm,rl,rr;
		vect3	P,C,N;

	#define	SCENE 1
	#if SCENE==1
		Entry( new PrimPlate( P=vect3( 0  ,  0 ,0.0),N=vect3(0,1,0),C=vect3(0.8,0.8,0.8),rl=0.5,rr=1.0 ,pw=20,e= 0.0,tm=0.0 ) );
		Entry( new PrimSphere(vect3( 0.0 , 1.25, -2       ),   0.5 , vect3(1  , 0.2, 0.2), 0.5, 1.0, 20, 0.0, 0.0 ) );
		Entry( new PrimSphere(vect3( 0.0 , 0.5 , -2-0.433 ),   0.5 , vect3(0.0, 0.0, 0.0), 1.0, 1.0, 20, 0.0, 0.0 ) );
		Entry( new PrimSphere(vect3( 0.5 , 0.5 , -2+0.433 ),   0.5 , vect3(0.2, 0.2, 1.0), 0.5, 1.0, 20, 0.0, 0.0 ) );
		Entry( new PrimSphere(vect3(-0.5 , 0.5 , -2+0.433 ),   0.5 , vect3(0.0, 1.0, 0.0), 0.5, 1.0, 20, 0.0, 0.0 ) );
		Entry( new PrimLight( vect3( 0   ,  30 ,  0 ), vect3(1,1,1)*1300 )  );
		A = vect3( 0.2,0.4,0.6)*0.0;
	#endif
	#if SCENE==2 //5 balls
		Entry( new PrimPlate( P=vect3( 0  ,  0 ,0.0),N=vect3(0,1,0),C=vect3(0.8,0.8,0.8),rl=0.5,rr=1.0 ,pw=20,e= 0.0,tm=0.0 ) );
		Entry( new PrimSphere(vect3(-2.0 , 0.5 , -2 ),   0.5 , vect3(0.0, 0.0, 0.0), 1.0 , 1.0, 20, 0.0, 0.0 ) );
		Entry( new PrimSphere(vect3(-1.0 , 0.5 , -2 ),   0.5 , vect3(0.0, 0.0, 0.0), 0.75, 1.0, 20, 0.0, 0.0 ) );
		Entry( new PrimSphere(vect3( 0.0 , 0.5 , -2 ),   0.5 , vect3(0.0, 0.0, 0.0), 0.5 , 1.0, 20, 0.0, 0.0 ) );
		Entry( new PrimSphere(vect3( 1.0 , 0.5 , -2 ),   0.5 , vect3(0.0, 0.0, 0.0), 0.25, 1.0, 20, 0.0, 0.0 ) );
		Entry( new PrimSphere(vect3( 2.0 , 0.5 , -2 ),   0.5 , vect3(0.0, 0.0, 0.0), 0.0 , 1.0, 20, 0.0, 0.0 ) );
		Entry( new PrimLight( vect3( 0   ,  20 ,  -2 ), vect3(1.0, 1.0, 1.0)*800 ) );
		A = vect3( 0.2,0.4,0.6)*1.0;
	#endif
	#if SCENE==3 // ring
		Entry( new PrimPlate( P=vect3( 0  ,  0 ,0.0),N=vect3(0,1,0),C=vect3(0.8,0.8,0.8),rl=0.5,rr=1.0 ,pw=20,e= 0.0,tm=0.0 ) );
		Entry( new PrimSphere(  vect3( 0 , 1.0 , 0 ),   0.5 ,  vect3(0.0, 0.0, 0.0),   0.5,   1.0 ,  100,  0.0,  0.0 ) );
		int	max = 16*3;
		for ( int i = 0 ; i < max ; i++ )
		{
			double	th  = (double)i *(pi/360)*16 * 3;
			double	th2 = (double)i *(pi/360)*16 * 0.5;
			double	x = cos(th);
			double	z = sin(th) ;
			double	y = cos(th2) +1.2;
			Entry( new PrimSphere(P=vect3( x , y , z ),r=0.2 ,C=vect3( x, y,  z) ,rl=0.2,rr=0.0 ,pw=100,e= 0.0,tm=0.0 ) );

		}
		Entry( new PrimLight( vect3( 0   ,  30 ,  0 ), vect3(1,1,1)*1800 )  );
		Entry( new PrimLight( vect3(-30   ,  30 ,  0 ), vect3(0.5,1,1)*1800 )  );
		Entry( new PrimLight( vect3(60   ,  80 ,  0 ), vect3(1,1,0.5)*4800 )  );
		Entry( new PrimLight( vect3(-60   ,  80 , 0 ), vect3(1,0.5,1)*4800 )  );
		A = vect3( 0.2,0.4,0.6)*0.0;
	#endif
	#if SCENE==4 //twin balls
		Entry( new PrimPlate( vect3( 0   ,  0 ,  0    ), normalize(vect3(0, 1,0))  , vect3(0.8, 0.8, 0.8), 0.5, 1.0, 20, 0.0, 0.0 ) );
		Entry( new PrimSphere(vect3(-1.0 , 1.0 , -2 ),   1.0 , vect3(1.0, 0.5, 0.5), 0.2, 1.0, 20, 0.0, 0.0 ) );
		Entry( new PrimSphere(vect3( 1.0 , 1.0 , -2 ),   1.0 , vect3(0.0, 0.0, 0.0), 0.2, 1.0, 20, 0.0, 0.0 ) );
		Entry( new PrimLight( vect3( 0   ,  20 ,  -2 ), vect3(1.0, 1.0, 1.0)*1800 ) );
		A = vect3( 0.2,0.4,0.6)*1.0;
	#endif
	#if SCENE==5//2 balls
		Entry( new PrimPlate( P=vect3(0  , 0 ,0.0),N=vect3(0,1,0),C=vect3(0.8,0.8,0.8),rl=0.5,rr=1.0 ,pw=20,e= 0.0,tm=0.0 ) );
		Entry( new PrimSphere(P=vect3( 0.5,1.0,3.0),r=0.5       ,C=vect3(0.0,0.0,1.0),rl=0.5,rr=1.0 ,pw=20,e=10.0,tm=0.0 ) );
		Entry( new PrimSphere(P=vect3(-0.5,1.0,3.0),r=0.5       ,C=vect3(0.0,1.0,0.0),rl=0.5,rr=1.0 ,pw=20,e=10.0,tm=0.0 ) );
		Entry( new PrimSphere(P=vect3( 0.0,1.5,3.0),r=0.5       ,C=vect3(1.0,0.0,0.0),rl=0.5,rr=1.0 ,pw=20,e=10.0,tm=0.0 ) );
		Entry( new PrimSphere(P=vect3( 0.0,0.5,3.0),r=0.5       ,C=vect3(1.0,1.0,0.0),rl=0.5,rr=1.0 ,pw=20,e=10.0,tm=0.0 ) );
		Entry( new PrimSphere(P=vect3( 0.0,1.0,2.75),r=0.25      ,C=vect3(1.0,1.0,1.0),rl=0.5,rr=1.0 ,pw=20,e=10.0,tm=0.0 ) );
		Entry( new PrimLight( P=vect3( 1.0 ,15, 0 )          ,C=vect3(1,1,1)*360 )  );
		A = vect3( 0.2,0.4,0.6)*0.5;
	#endif
	}

	//------------------------------------------------------------------------------
	vect3 Raycast( vect3 P, vect3 I )
	//------------------------------------------------------------------------------g
	{
		vect3 ret = vect3(0,0,0);

		if ( m_cntRay > 5 ) return ret;
		m_cntRay++;

		Surface sur;
		PrimLight&	lgt = *m_tblLight[0];
		vect3	Lc;
		vect3	L;
		float	d;
		float	s=0;
		float	r=0;
		float	t=0;
		
		if ( (sur = raycast( P, I )).flg )
		{
			L	= normalize(sur.Q - lgt.P);
			Lc	= lgt.C / dot(sur.Q - lgt.P, sur.Q - lgt.P);
			d	= max( 0.0, dot( sur.N, -L ) );
			s	= (sur.valPower+2)/(8*pi)*pow( max( 0.0, dot( sur.R, -L ) ), sur.valPower );
			r	= sur.valReflectance;
			ret += r* (Raycast( sur.Q, sur.R )+s) * Lc;

			if ( sur.valTransmittance == 0.0 )
			{
				ret += (1-r)*( d * sur.C ) * Lc;
			}
			else
			{
				I = refract( I, sur.N, sur.valRefractive/1.0 );
				sur = raycast( sur.Q, I );



				I = refract( I, sur.N, 1.0/sur.valRefractive );
				ret += (1-r)*Raycast( sur.Q, I );
			}

		}
		else
		{
			L = normalize(sur.Q - lgt.P);
			Lc = lgt.C / dot(sur.Q - lgt.P, sur.Q - lgt.P);
			int n = 20;
			ret += s;
		}

		return ret;
	}
	//------------------------------------------------------------------------------
	void	Paint( unsigned char* to, int height, int width, int psize )
	//------------------------------------------------------------------------------
	{

		vect3	posScr = vect3(0,1.0,-12+8);
		vect3	posEye = vect3(0,1.0,-17+8);

		float r,s,p,e,t,rl,rr;
		vect3	C;

		int	cntMax = 0;
		int	cntRay = 0;
		for( int py = 0 ; py < height ; py++ )
		{
			for( int px = 0 ; px < width ; px++ )
			{
				double x = ((double)px / width) *2.0-1.0;
				double y = ((double)py / height) *2.0-1.0;
				vect3	P = vect3( x, y, 0 ) + posScr;
				vect3	I = normalize(P - posEye);

				double	valRefractive = 1.0;

				m_cntRay = 0;
				int	cntNext = 0;
		 		C = Raycast( P, I );
				if ( m_cntRay > cntMax ) cntMax = m_cntRay;
				cntRay+= m_cntRay;

				int r = min( 255, (int)(255*C.r) );
				int g = min( 255, (int)(255*C.g) );
				int b = min( 255, (int)(255*C.b) );

				to[(py*width + px)*psize +0] = b;
				to[(py*width + px)*psize +1] = r;
				to[(py*width + px)*psize +2] = g;
			}
		}
		
	}

};

#if 1
//------------------------------------------------------------------------------
void	raytrace( SysGra& gra, int py )
//------------------------------------------------------------------------------
{
	Renderer ren;

	{
		int width	= gra.GetWidth(); 
		int height	= gra.GetHeight(); 
	
		vect3	posScr = vect3(0,1.0,-12+8);
		vect3	posEye = vect3(0,1.0,-17+8);

		float r,s,p,e,t,rl,rr;
		vect3	C;

		int	cntMax = 0;
		int	cntRay = 0;
//		for( int py = 0 ; py < height ; py++ )
		{
			for( int px = 0 ; px < width ; px++ )
			{
				double x = ((double)px / width) *2.0-1.0;
				double y = ((double)py / height) *2.0-1.0;
				vect3	P = vect3( x, y, 0 ) + posScr;
				vect3	I = normalize(P - posEye);

				double	valRefractive = 1.0;

				ren.m_cntRay = 0;
				int	cntNext = 0;
		 		C = ren.Raycast( P, I );
				if ( ren.m_cntRay > cntMax ) cntMax = ren.m_cntRay;
				cntRay+= ren.m_cntRay;

				gra.Pset( vect2(px,height-py) ,rgb(C.r,C.g,C.b));
			}
		}
		
	}

}
#endif

struct Apr : public Sys
{
	struct Figure
	{
		SysGra&	gra;
		vector<vect2> vert;
		vector<ivect2> edge;
		int	col;
		Figure( SysGra& _gra ) : gra(_gra) {}

		void draw( vect2 ofs, double th, int col ) const 
		{
			for ( ivect2 e : edge )
			{
				const vect2& p = vert[e.p];
				const vect2& n = vert[e.n];

				double x0=p.x*cos(th) - p.y*sin(th) + ofs.x;
				double y0=p.x*sin(th) + p.y*cos(th) + ofs.y;
				double x1=n.x*cos(th) - n.y*sin(th) + ofs.x;
				double y1=n.x*sin(th) + n.y*cos(th) + ofs.y;

				gra.Line(vect2(x0,y0), vect2(x1,y1),col);
			}
		}

	};

	struct Marker2
	{
		const SysGra&	gra;
		const Figure&	fig;
		vect2&	pos;
		bool 	bRectSelected;		//	矩形選択中、選択＆非選択
		bool 	bRectIn;			//	矩形選択中、矩形選択対象
		bool 	bSelected;			//	選択
		bool 	bAffectable;		//	削除対象
		double	th;
		int		colNormal = rgb(1,1,0);
		int		colSelected = rgb(1,0,0);

		Marker2( SysGra& _gra, Figure& _fig, vect2& v, double _th ) : gra(_gra), fig(_fig), pos(v)
		{
			bSelected		= false;
			bRectIn			= false;
			bRectSelected	= false;
			bAffectable		= false;
			th				= _th;
		}
		Marker2(const Marker2& a) : gra(a.gra), fig(a.fig), pos(a.pos)
		{
			bSelected		= a.bSelected;
			bRectIn			= a.bRectIn;
			bRectSelected	= a.bRectSelected;
			bAffectable 	= a.bAffectable;
			th 				= a.th;
		}	
		const Marker2&	operator=(Marker2&& a){return a;}	
		void draw()
		{
			bool flg =  bSelected;
			
			if ( bRectIn )
			{
				flg = bRectSelected;
			}
			
			if ( flg )			
			{
				fig.draw( pos,th, colSelected );
			}
			else
			{
				fig.draw( pos,th, colNormal );
			}
		
		}
	};


	
	~Apr()
	{
	}

	struct MarkerController
	{
		vector<Marker2>	tblMarker2;
		//------------------------------------------------------------------------------
		void funcMarkerDraw2()
		//------------------------------------------------------------------------------
		{
			for ( Marker2 m : tblMarker2 )
			{
				m.draw();
			}
		}
		//------------------------------------------------------------------------------
		void funcMarkerController2( Figure& fig, SysMouse& mouse, SysKeys& keys, SysGra& gra )
		//------------------------------------------------------------------------------
		{
			static vect2 drag_start(0,0);
			static bool bDrag = false;

			if  ( keys.ALT.on ) return;

			// マーカー追加
			if ( keys.M.hi )
			{
				tblMarker2.emplace_back( gra, fig, mouse.pos, rad(0) );
			}


			// マーカー削除
			if  ( keys.CTRL.on && keys.X.hi )
			{
				for ( Marker2& m : tblMarker2 )
				{
					if ( m.bSelected )
					{
						m.bAffectable = true;
					}
				}

				for ( int i = tblMarker2.size()-1 ; i >= 0 ; i-- )
				{
					if ( tblMarker2[i].bAffectable )
					{
							   tblMarker2.erase(tblMarker2.begin() +i);	
					}
				}
			}
			
			// マーカー選択
			if ( mouse.L.on )
			{
				struct
				{
					double	len;
					Marker2*	pmark;
					int		cnt;
				} a = {99999,0,0};

				// 最近マーカーを検索
				for ( Marker2& m : tblMarker2 )
				{
					double len = (m.pos-mouse.pos).length();
					if ( len < 20.0 && a.len > len )
					{
						a.len = len;
						a.pmark = &m;
						a.cnt++;
					}
				}

				// マーカー選択＆解除
				if ( mouse.L.hi )
				{
					// 矩形選択
					if ( a.pmark == 0 ) 
					{
						bDrag = true;
						drag_start = mouse.pos;
					}

					// マーカー全解除
					if ( keys.CTRL.on ){}
					else
					if ( keys.SHIFT.on ){}
					else
					if ( a.pmark && a.pmark->bSelected == true ){}
					else
					{
						for ( Marker2& m : tblMarker2 )
						{
							m.bSelected = false;
						}
					}
					
					//	マーカー選択
					if ( a.pmark )
					{
						if ( keys.CTRL.on )
						{
							a.pmark->bSelected = !a.pmark->bSelected;
						}
						else
						{
							a.pmark->bSelected = true;
						}
					}
				}
				else
				{
					if ( bDrag )
					{
						// 矩形カーソル表示
						vect2 v0 = min( drag_start, mouse.pos );
						vect2 v1 = max( drag_start, mouse.pos );
						gra.Box( v0,v1, rgb(0,0.5,1));

						for ( Marker2& m : tblMarker2 )
						{
							m.bRectIn = false;
						}

						// 矩形内マーカーを検索
						for ( Marker2& m : tblMarker2 )
						{
							double len = (m.pos-mouse.pos).length();
							if ( m.pos.x > v0.x && m.pos.x < v1.x && m.pos.y > v0.y && m.pos.y < v1.y )
							{
								m.bRectIn = true;
								if ( keys.CTRL.on )
								{
									m.bRectSelected = !m.bSelected;
								}
								else
								{
									m.bRectSelected = true;
								}
							}
						}

					}
					else
					// マーカー移動
					for ( Marker2& m : tblMarker2 )
					{
						if ( m.bSelected )
						{
							m.pos += mouse.mov;
						}
					}
				}
			}
			else
			{
				if ( bDrag )
				{
					bDrag = false;
					for ( Marker2& m : tblMarker2 )
					{
						if ( m.bRectIn )
						{
							m.bSelected = m.bRectSelected;
						}
						m.bRectIn = false;
						m.bRectSelected = false;
					}
				}
			}
			

		}

	} mc;


	//------------------------------------------------------------------------------
	Apr( const char* name, int pos_x, int pos_y, int width, int height ) : Sys( name, pos_x, pos_y, width, height )
	//------------------------------------------------------------------------------
	{
	}

	//------------------------------------------------------------------------------
	main()
	//------------------------------------------------------------------------------
	{
		Figure figArrow=Figure(gra);
		figArrow.vert.emplace_back( (vect2){   0,20*tan(rad(60))	-10*tan(rad(30)) } );
		figArrow.vert.emplace_back( (vect2){-10,  0 	    	   	-10*tan(rad(30)) } );
		figArrow.vert.emplace_back( (vect2){ 10,  0 				-10*tan(rad(30)) } );
		figArrow.edge.emplace_back( (ivect2){ 0,1 } );
		figArrow.edge.emplace_back( (ivect2){ 1,2 } );
		figArrow.edge.emplace_back( (ivect2){ 2,0 } );
		figArrow.col = rgb(0,0.5,1);

		Figure figTriangle=Figure(gra);
		{
			const double R=50;
			figTriangle.vert.emplace_back( (vect2){   0,R*tan(rad(60))	-R*tan(rad(30)) } );
			figTriangle.vert.emplace_back( (vect2){-R,  0 	    	   	-R*tan(rad(30)) } );
			figTriangle.vert.emplace_back( (vect2){ R,  0 				-R*tan(rad(30)) } );
			figTriangle.edge.emplace_back( (ivect2){ 0,1 } );
			figTriangle.edge.emplace_back( (ivect2){ 1,2 } );
			figTriangle.edge.emplace_back( (ivect2){ 2,0 } );
			figTriangle.col = rgb(0,1,1);
		}

		Figure figCircle=Figure(gra);
		{
			int s=0;
			for ( int i = 0 ; i < 360 ; i+=45 )
			{
				double th = i*pi/180.0;
				double r = 7;
				vect2 v( r*cos(th), r*sin(th) );
				figCircle.vert.emplace_back( v );
				s++;
			}
			for ( int i = 0 ; i < s-1 ; i++ )
			{
				figCircle.edge.emplace_back( (ivect2){ i,i+1 } );
			}
			figCircle.edge.emplace_back( (ivect2){ s-1,0 } );
		}

		// カトマル曲線
		auto catmull_func = []( double t, const vect2 P0, const vect2 P1, const vect2 P2, const vect2 P3 )
		{
			//Catmull-Rom 曲線
			// P(t)=P1*(2t^3-3t^2+1)+m0*(t^3-2t^2+t)+P2*(-2t^3+3t^2)+m1*(t^3-t^2)
			// m0=(P2-P0)/2
			// m1=(P3-P1)/2

			vect2 m0 = (P2-P0)/2.0;
			vect2 m1 = (P3-P1)/2.0;
			vect2 P =  P1*(  2*t*t*t - 3*t*t +1) + m0*( t*t*t -2*t*t +t )
					 + P2*( -2*t*t*t + 3*t*t   ) + m1*( t*t*t - t*t );

			return P;
		};
		vector<vect2> catmull_tbl =
		{
			#define X 600
			#define Y 50
			vect2(X+ 0,Y  +0),
//			vect2(X+ 0,Y+ 20),
			vect2(X+50,Y+100),
			vect2(X-50,Y+140),
//			vect2(X+ 0,Y+180),
			vect2(X+ 0,Y+200),
			#undef X
			#undef Y
		};
		for ( vect2& v : catmull_tbl )	// マーカー対象に位置を登録
		{
			mc.tblMarker2.emplace_back( gra, figCircle, v, rad(-90) );
		}
		
		//3字曲線
		auto bezier_func = [] ( double t, vect2 P0, vect2 P1, vect2 P2, vect2 P3 )
		{
		#if 1
			vect2 L0=(P1-P0)*t+P0;
			vect2 L1=(P2-P1)*t+P1;
			vect2 L2=(P3-P2)*t+P2;

			vect2 M0=(L1-L0)*t+L0;
			vect2 M1=(L2-L1)*t+L1;

			vect2 Q=(M1-M0)*t+M0;
		#else
			double tt = t*t;
			double ttt = tt*t;
			vect2 Q=
				 P3*  ttt
				+P2*(-ttt*3 +tt*3)
				+P1*( ttt*3 -tt*6 +t*3)
				+P0*(-ttt   +tt*3 -t*3 +1)
				;
		#endif

			return Q;
		};

		vector<vect2> bezier_tbl =
		{
			#define X 400
			#define Y 350
				vect2(X+ 00,Y+90),
				vect2(X+ 00,Y+20),
				vect2(X+100,Y+90),
				vect2(X+100,Y+20),
				vect2(X+100,Y-20),
				vect2(X+200,Y+20),
				vect2(X+200,Y+90),
				vect2(X+200,Y+120),
				vect2(X+300,Y+20),
				vect2(X+300,Y+90),
			#undef X
			#undef Y
		};
		for ( vect2& v : bezier_tbl )	// マーカー対象に位置を登録
		{
			mc.tblMarker2.emplace_back( gra, figCircle, v, rad(-90) );
		}

		//骨---------------------------------------
		struct Joint2
		{
			vect2 pos;
			vect2 tension;
//			vect2 accell;
//			vect2 mov;
//			vect2 prev;
			double len;
			Joint2( vect2 v )
			{
				pos = v;
				tension = 0;
	//			prev = v;
				len = 0;
			}
		};
		struct Bone2
		{
			Joint2& j0;
			Joint2& j1;
			double length;
			Bone2( Joint2& _j0, Joint2& _j1 ) :j0(_j0), j1(_j1){}
			Bone2( Joint2&& _j0, Joint2&& _j1 ) :j0(_j0), j1(_j1){}
		};
		vector<Joint2> tblJoint;
		tblJoint.reserve(1000);
		vector<Bone2> tblBone;
		if(1)
		{	//	対△
			int ox=200,oy=300;

			tblJoint.emplace_back( vect2( ox,oy) );
			tblJoint.emplace_back( vect2( ox-30,oy+80) );
			tblJoint.emplace_back( vect2( ox+30,oy+80) );
			tblJoint.emplace_back( vect2( ox+0 ,oy+160) );

			tblBone.emplace_back( tblJoint[0], tblJoint[1] );
			tblBone.emplace_back( tblJoint[0], tblJoint[2] );
			tblBone.emplace_back( tblJoint[1], tblJoint[2] );
			tblBone.emplace_back( tblJoint[1], tblJoint[3] );
			tblBone.emplace_back( tblJoint[2], tblJoint[3] );
		}
		if(0)
		{	//	四角格子メッシュ
			int ox=200,oy=300;
			const int  W=40;
			const int  H=80;
			const int  X=2;
			const int  Y=3;
			for ( int x = 0 ; x < X ; x++ )
			{
				for ( int y = 0 ; y < Y ; y++ )
				{
					tblJoint.emplace_back( vect2( x*W+ox,y*H+oy) );
				}
			}
			for ( int y=0 ; y < Y-1 ; y++ )
			{
				for ( int x=0 ; x < X ; x++ )
				{
					tblBone.emplace_back( tblJoint[x*Y+y], tblJoint[x*Y+y+1] );
				}
			}
			for ( int y=0 ; y < Y ; y++ )
			{
				for ( int x=0 ; x < X-1 ; x++ )
				{
					tblBone.emplace_back( tblJoint[x*Y+y], tblJoint[(x+1)*Y+y] );
				}
			}
		}
		if(0)
		{	//	三角形メッシュ
			function<void(int,int,int,int)> func = [&]( int v0, int v1, int v2, int n )
			{ 
				if ( n > 0 )
				{
					int ix = tblJoint.size();
					tblJoint.emplace_back( vect2( (tblJoint[v0].pos + tblJoint[v1].pos )/2 ) );
					tblJoint.emplace_back( vect2( (tblJoint[v1].pos + tblJoint[v2].pos )/2 ) );
					tblJoint.emplace_back( vect2( (tblJoint[v2].pos + tblJoint[v0].pos )/2 ) );
					
					func( v0, ix+0, ix+2, n-1 );
					func( v1, ix+1, ix+0, n-1 );
					func( v2, ix+2, ix+1, n-1 );
				}
				else
				{
					tblBone.emplace_back( tblJoint[v0], tblJoint[v1] );
					tblBone.emplace_back( tblJoint[v1], tblJoint[v2] );
					tblBone.emplace_back( tblJoint[v2], tblJoint[v0] );
				}
			};

			double R=80;
			tblJoint.emplace_back( vect2(200+0, 500-R*tan(rad(60))	-R*tan(rad(30)) ));
			tblJoint.emplace_back( vect2(200-R, 500+  	    	   	-R*tan(rad(30))) );
			tblJoint.emplace_back( vect2(200+R, 500+  				-R*tan(rad(30))) );

			func( 0, 1, 2, 3 );
			cout << tblJoint.size() << endl;
		}
		for ( Bone2& b : tblBone )
		{
			b.length = (b.j1.pos - b.j0.pos).length();
		}
		for ( Joint2& j : tblJoint )	//マーカー対象に位置を登録
		{
			mc.tblMarker2.emplace_back( gra, figCircle, j.pos, rad(-90) );
		}

		//人
		struct Joint3
		{
			vect3 pos;
			vect3 tension;
			vect3 world;
			vect3 disp;
			double len;
			Joint3( vect3 v )
			{
				pos = v;
				tension = 0;
				len = 0;
			}
		};

		struct Bone3
		{
			Joint3& j0;
			Joint3& j1;
			double length;
			Bone3( Joint3& _j0, Joint3& _j1 ) :j0(_j0), j1(_j1){}
			Bone3( Joint3&& _j0, Joint3&& _j1 ) :j0(_j0), j1(_j1){}
		};
		vector<Joint3> human_tblJoint;
		human_tblJoint.reserve(1000);
		vector<Bone3> human_tblBone;
		{	//	人
			double ox=0,oy=-160,os=0.01;

			human_tblJoint.emplace_back( os*vect3( ox-10,	oy- 20,	0 )	);//0
			human_tblJoint.emplace_back( os*vect3( ox+10,	oy- 20,	0 )	);//1
			human_tblJoint.emplace_back( os*vect3( ox+ 0,	oy+  0,	0 )	);//2
			human_tblJoint.emplace_back( os*vect3( ox-20,	oy+  0,	0 )	);//3
			human_tblJoint.emplace_back( os*vect3( ox+20,	oy+  0,	0 )	);//4
			human_tblJoint.emplace_back( os*vect3( ox+ 0,	oy+ 50,	0 )	);//5
			human_tblJoint.emplace_back( os*vect3( ox-15,	oy+ 70,	0 )	);//6
			human_tblJoint.emplace_back( os*vect3( ox+15,	oy+ 70,	0 )	);//7
			human_tblJoint.emplace_back( os*vect3( ox-15,	oy+115,	0 )	);//8
			human_tblJoint.emplace_back( os*vect3( ox-15,	oy+160,	0 )	);//9
			human_tblJoint.emplace_back( os*vect3( ox+15,	oy+115,	0 )	);//10
			human_tblJoint.emplace_back( os*vect3( ox+15,	oy+160,	0 )	);//11
			human_tblJoint.emplace_back( os*vect3( ox-20,	oy+ 40,	0 )	);//12
			human_tblJoint.emplace_back( os*vect3( ox-20,	oy+ 80,	0 )	);//13
			human_tblJoint.emplace_back( os*vect3( ox+20,	oy+ 40,	0 )	);//14
			human_tblJoint.emplace_back( os*vect3( ox+20,	oy+ 80,	0 )	);//15

			human_tblBone.emplace_back( human_tblJoint[0], human_tblJoint[1] );	//head
			human_tblBone.emplace_back( human_tblJoint[1], human_tblJoint[2] );
			human_tblBone.emplace_back( human_tblJoint[2], human_tblJoint[0] );

			human_tblBone.emplace_back( human_tblJoint[2], human_tblJoint[5] ); //chest
			human_tblBone.emplace_back( human_tblJoint[5], human_tblJoint[3] );
			human_tblBone.emplace_back( human_tblJoint[3], human_tblJoint[2] );
			human_tblBone.emplace_back( human_tblJoint[2], human_tblJoint[4] );
			human_tblBone.emplace_back( human_tblJoint[4], human_tblJoint[5] );

			human_tblBone.emplace_back( human_tblJoint[5], human_tblJoint[7] ); //hip
			human_tblBone.emplace_back( human_tblJoint[7], human_tblJoint[6] );
			human_tblBone.emplace_back( human_tblJoint[6], human_tblJoint[5] );

			human_tblBone.emplace_back( human_tblJoint[6], human_tblJoint[8] ); //leg
			human_tblBone.emplace_back( human_tblJoint[8], human_tblJoint[9] );

			human_tblBone.emplace_back( human_tblJoint[7], human_tblJoint[10] ); //leg
			human_tblBone.emplace_back( human_tblJoint[10], human_tblJoint[11] );

			human_tblBone.emplace_back( human_tblJoint[3], human_tblJoint[12] ); //arm
			human_tblBone.emplace_back( human_tblJoint[12], human_tblJoint[13] );

			human_tblBone.emplace_back( human_tblJoint[4], human_tblJoint[14] ); //arm
			human_tblBone.emplace_back( human_tblJoint[14], human_tblJoint[15] );
		}
		for ( Bone3& b : human_tblBone )
		{
			b.length = (b.j1.pos - b.j0.pos).length();
		}
		for ( Joint3& j : human_tblJoint )	//マーカー対象に位置を登録
		{
//			mc.tblMarker2.emplace_back( gra, figCircle, j.disp, rad(-90) );
		}
		vector<vect3> human_disp;


		// 人
		struct
		{
			vect3 rot = vect3(0,0,0);
			vect3 pos = vect3(-2,0.0,0);
		} human;
		


		// 箱
		struct
		{
			double	rx = rad(0);
			double	ry = rad(0);
			double	rz = rad(0);

			vect3 pos = {2,-1,0};

			vector<vect3> vert=
			{
				{	-1,	 1,	-1	},
				{	 1,	 1,	-1	},
				{	-1,	-1,	-1	},
				{	 1,	-1,	-1	},
				{	-1,	 1,	 1	},
				{	 1,	 1,	 1	},
				{	-1,	-1,	 1	},
				{	 1,	-1,	 1	},
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
			};

		} box;



		// カメラ
		struct
		{
			vect3	pos = vect3( 0,-1, -5);
			vect3 	at = vect3( 0, 0, 0 );
			vect3	up = vect3( 0, 1, 0);
		  		
		} cam ;
		
		struct Pers
		{
			double	val;
			double	fovy;		// 画角
			double	sz;			// 投影面までの距離
			double	ox;			// 描画画面の中心W
			double	oy;			// 描画画面の中心H
			double	w;			// 描画画面の解像度W/2
			double	h;			// 描画画面の解像度H/2
			double	aspect;		// 描画画面のアスペクト比
		
			Pers()
			{
				val=90/2;
				fovy = rad(val);				// 画角
				sz = 1/tan(fovy/2);				// 投影面までの距離
			}
		
			void Update( vect2 screensize )
			{
				fovy = rad(val);				// 画角
				sz = 1/tan(fovy/2);				// 投影面までの距離

				ox	= screensize.x/2;			// 描画画面の中心W
				oy	= screensize.y/2;			// 描画画面の中心H
				w	= screensize.x/2;			// 描画画面の解像度W/2
				h	= screensize.y/2;			// 描画画面の解像度H/2
				aspect	= screensize.y/screensize.x;	// 描画画面のアスペクト比

			} 

			vect3 calc( vect3 p ) const
			{
				vect3 ret;
				ret.x = p.x/(p.z+sz)	*sz*w*aspect	+ox;
				ret.y = p.y/(p.z+sz)	*sz*h			+oy;
				ret.z = 1/(p.z+sz);
				return ret;
			}
	
		};
		Pers pers;

		//===========================================================================
		while( Update() )
		{
			pers.Update( vect2( m.width, m.height ) );

if ( keys.H.hi ) cout << "fovy=" << pers.val << endl;

	 		static int py=0;


			gra.Clr(rgb(0.3,0.3,0.3));
//			gra.Fill(vect2(0,0),vect2(m.width,m.height),rgb(0.3,0.3,0.3));


			// レイトレ
			//raytrace( gra, py++ );
			if ( py >= m.height ) py=0;


			// パースペクティブ
			pers.val += -mouse.wheel/30;
			if (keys.R.rep) {pers.val-=1;cout << pers.val <<" "<<1/tan(rad(pers.val)) << endl; }
			if (keys.F.rep) {pers.val+=1;cout << pers.val <<" "<<1/tan(rad(pers.val)) << endl; }
//cout << pers.val << endl;
			// カメラ移動
			if ( keys.ALT.on )
			{
				if ( mouse.R.on )
				{
					cam.pos.z += mouse.mov.y/100.0;
				}
				if ( mouse.M.on )
				{
					cam.pos.x += mouse.mov.x/100.0;
					cam.pos.y += mouse.mov.y/100.0;
				}
			}


			// グリッドgrid
			if(0)
			{	// ドットグリッド
				const double NUM = 4;
				{
					for ( int i = -NUM ; i <= NUM ; i++ )
					{
						for ( int j = -NUM ; j <= NUM ; j++ )
						{
							vect3 v0 = pers.calc( vect3(i,0,j) -cam.pos);
							if ( v0.z > 0 ) 
							{
								double r = 5* v0.z;
								if ( r < 1.0 )
								{
									gra.Pset( vect2(v0.x,v0.y), rgb(0,1,1 ));
								}
								else
								{
									gra.Circle( vect2(v0.x,v0.y), r,rgb(0,1,1 ));
								}
							}
						}
					}
				}
			}

			if(1)
			{	// 格子グリッド
				const double NUM = 4;
				{
					vect3 a(-NUM, 0,-NUM);
					vect3 b( NUM, 0,-NUM);
					a += -cam.pos;
					b += -cam.pos;
					for ( int i = 0 ; i < NUM*2+1 ; i++ )
					{
						vect3 va = pers.calc(a);
						vect3 vb = pers.calc(b);


						if ( va.z > 0 && vb.z > 0 )
						{
							gra.Line( vect2(va.x,va.y), vect2(vb.x,vb.y), rgb(0,0,1));
						}
						

						a.z+=1.0;
						b.z+=1.0;
					}
				}			
				{
					vect3 a(-NUM, 0, NUM);
					vect3 b(-NUM, 0,-NUM);
					a += -cam.pos;
					b += -cam.pos;
					for ( int i = 0 ; i < NUM*2+1 ; i++ )

					{
						vect3 va = pers.calc(a);
						vect3 vb = pers.calc(b);

						{//シザリング
							function<vect3(vect3,vect3,int)>nearclip = [ pers , &nearclip ]( vect3 a, vect3 b, int n )
							{
								if (n <=0 ) return b;

								vect3 c =  (a+b)/2;
						
								if ( c.z <= -pers.sz )
								{
									c = nearclip( a, c, n-1 );
								}
								if ( c.z > 1.0-pers.sz )
								{
									c = nearclip( c, b, n-1 );
								}
								return c;
							};
							if ( va.z > 0|| vb.z > 0 )
							{
								if ( va.z < 0 )
								{
									vect3 c = nearclip(b,a,4);
									va = pers.calc(c);
								}
								if ( vb.z < 0 )
								{
									vect3 c = nearclip(a,b,4);
									vb = pers.calc(c);
								}
							}
						}
						if ( va.z > 0 && vb.z > 0 )
						{
							gra.Line( vect2(va.x,va.y), vect2(vb.x,vb.y), rgb(0,0,1));
						}
						a.x+=1.0;
						b.x+=1.0;
					}
				}			
			}
			// 箱
			//calc rotate
			box.disp.clear();
			for ( vect3 v : box.vert )
			{

	#if 0
				double	x,y,z;
				//box.rz
				x=v.x*cos(box.rz) - v.y*sin(box.rz) + v.z *0;
				y=v.x*sin(box.rz) + v.y*cos(box.rz) + v.z *0;
				z=v.x* 0      + v.y*0       + v.z *1;
				v.x=x;
				v.y=y;
				v.z=z;

				//box.rx
				x=v.x;
				y=v.y*cos(box.rx) - v.z*sin(box.rx);
				z=v.y*sin(box.rx) + v.z*cos(box.rx);
				v.x=x;
				v.y=y;
				v.z=z;

				//box.ry
				x=v.x*cos(box.ry) - v.z*sin(box.ry);
				y=v.y;
				z=v.x*sin(box.ry) + v.z*cos(box.ry);
				v.x=x;
				v.y=y;
				v.z=z;
	#else
				//	右手系座標系
				//	右手ねじ周り
				//	roll	:z	奥+
				//	pitch	:x	右+
				//	yaw		:y	下+

//				box.rx+=rad(0.03);
//				box.ry+=rad(0.05);
//				box.rz+=rad(0.1);
	/*
				mat44 rotx(
					1.0			,	0.0			,	0.0			,	0.0	,
					0.0			,	 cos(box.rx)	,	-sin(box.rx)	,	0.0	,
					0.0			,	 sin(box.rx)	,	 cos(box.rx)	,	0.0	,
					0.0			,	0.0			,	0.0			,	0.0	
				);

				mat44 roty(
					 cos(box.ry)	,	0.0			,	 sin(box.ry)	,	0.0	,
					 0.0		,	1.0			,	0.0			,	0.0	,
					-sin(box.ry)	,	0.0			,	 cos(box.ry)	,	0.0	,
					 0.0		,	0.0			,	0.0			,	0.0	
				);

				mat44 rotz(
					 cos(box.rz)	,	-sin(box.rz)	,	0.0			,	0.0	,
					 sin(box.rz)	,	 cos(box.rz)	,	0.0			,	0.0	,
					0.0			,	0.0			,	1.0			,	0.0	,
					0.0			,	0.0			,	0.0			,	0.0	
				);
	*/
				mat44	rotx;
				mat44	roty;
				mat44	rotz;
				rotx.setRotateX(box.rx);
				roty.setRotateY(box.ry);
				rotz.setRotateZ(box.rz);
		

	#if 0
				v= rotx *v ;
				v= roty *v ;
				v= rotz *v ;
	#else
				v= rotx * roty * rotz *v + box.pos ;
	#endif


				
	#endif

				v += -cam.pos;

				box.disp.emplace_back( v );

			}
			
			
			// 箱
			for ( ivect2 e : box.edge )
			{
				const vect3& a = box.disp[e.p];
				const vect3& b = box.disp[e.n];


				vect3 v0 = pers.calc(a);
				vect3 v1 = pers.calc(b);
				if ( v0.z > 0 && v1.z > 0 )
				{
					gra.Line( vect2(v0.x,v0.y), vect2(v1.x,v1.y), rgb(0,1,1));
				}

			}


			// 点
			gra.Pset(vect2(10,10),rgb(1,1,1));

			// 塗りつぶし三角
			gra.Tri( vect2(55,10), vect2(10,100), vect2(100,100),rgb(1,1,0));
			gra.Tri( vect2(55+80,10), vect2(10+80,100), vect2(100+80,100),rgb(0.5,0.3,0.2));

			
			// カトマル
			{
				// マーカースプライン変換表示
				{
					double div = 10;
					double st = 1/div;

					for ( int n = -1 ; n < (signed)catmull_tbl.size()-3+1 ; n++ )
					{
						int n0 = n;
						int n1 = n+1;
						int n2 = n+2;
						int n3 = n+3;
						if ( n0 < 0 ) n0 = 0;
						if ( n3 >= (signed)catmull_tbl.size() ) n3 = n2;
					
						double t = st;
						vect2 v0 = catmull_func(0, catmull_tbl[n0], catmull_tbl[n1], catmull_tbl[n2], catmull_tbl[n3] );
						for ( int i = 0 ; i <div ; i++ )
						{
							vect2 v1 = catmull_func(t, catmull_tbl[n0], catmull_tbl[n1], catmull_tbl[n2], catmull_tbl[n3] );
							gra.Line( v1, v0, rgb(1,1,1));
							gra.Fill( v1-1,v1+3, rgb(1,1,1));
							v0=v1;
							t+=st;

						}	
							
					}
				}
			}

			// ベジェ 三次曲線
			{
				{//ベジェ計算＆描画
					double div = 20;
					double st = 1/div;

					for ( int n = 0 ; n < (signed)bezier_tbl.size()-3 ; n+=3 )
					{
						double t  = st;
						vect2 v0 = bezier_tbl[n+0];
						for ( int i = 0 ; i < div ; i++ )
						{
							vect2 v1 = bezier_func( t, bezier_tbl[n+0], bezier_tbl[n+1], bezier_tbl[n+2], bezier_tbl[n+3] );
							gra.Line( v0,v1, rgb(1,1,1));
							gra.Fill( v1-1,v1+3, rgb(1,1,1));
							v0=v1;
							t+=st;
						}
					}

				}

				{// 補助ライン描画
					int cnt = 0;
					vect2 v0 = bezier_tbl[0];
					for ( int i = 1 ; i < (signed)bezier_tbl.size() ; i++ )
					{ 
						vect2 v1 = bezier_tbl[i];
						if ( cnt != 1 ) 
						{
							gra.Line( v0, v1, rgb(0,1,0));
						}
						v0 = v1;
						cnt = (cnt+1)%3;
					}
				}

			}

			// キーフレーム
			struct Keyframe
			{
				vect2 pos;
				Keyframe( vect2 _pos )
				{
					pos = _pos;
				}
			};
			static vector<Keyframe> tblKeyframe;

			static vect2 gv;
			if(0)
			{//ベジェアニメーション
		
				static	double t = 0;
				static	bool	dir = true;

				static int n = 0;
				gv = bezier_func( t, bezier_tbl[n+0], bezier_tbl[n+1], bezier_tbl[n+2], bezier_tbl[n+3] );

				gra.Circle( gv, 6,rgb(1,0,0));

				if ( dir ) t+=0.01; else t-=0.01;


				if ( t >= 1.0 ) 
				{
					if ( n+3 < (signed)bezier_tbl.size()-3 )
					{
						t = 0;
						n+=3;
					}
					else
					{
						t = 1.0;
						dir = !dir;
					}
				}
				else
				if ( t <= 0.0 ) 
				{
					if ( n >= 3 )
					{
						t = 1.0;
						n-=3;
					}
					else
					{
						t = 0.0;
						dir = !dir;
					}
				}
			}

			if(1)
			{//カトマルアニメーション
		
				static	double t = 0;
				static	bool	dir = true;

				static int n = 0;
				int n0 = n-1;
				int n1 = n;
				int n2 = n+1;
				int n3 = n+2;
				if ( n0<0 ) n0 = 0;
				if ( n3>=(signed)catmull_tbl.size() ) n3 =n2;

				gv = catmull_func( t, catmull_tbl[n0], catmull_tbl[n1], catmull_tbl[n2], catmull_tbl[n3] );

				gra.Circle( gv, 6,rgb(1,0,0));

				if ( dir ) t+=0.01; else t-=0.01;


				if ( t >= 1.0 ) 
				{
					if ( n+1 < (signed)catmull_tbl.size()-1 )
					{
						t = 0;
						n+=1;
					}
					else
					{
						t = 1.0;
						dir = !dir;
					}
				}
				else
				if ( t <= 0.0 ) 
				{
					if ( n >= 1 )
					{
						t = 1.0;
						n-=1;
					}
					else
					{
						t = 0.0;
						dir = !dir;
					}
				}
			}


			//=================================
			// 入力
			//=================================

			// マーカー操作	
			mc.funcMarkerController2( figCircle, mouse, keys, gra );

Joint2& tar = tblJoint[0];

			// キーフレーム追加
			if (keys.K.hi) 
			{
				tblKeyframe.emplace_back( mouse.pos );
			}
			for ( Keyframe k : tblKeyframe )
			{
				gra.Circle( k.pos, 5, rgb(0,1,1));
			}
			if (keys.SPACE.hi) 
			{
//				tar.pos = tblKeyframe[0].pos;
			}
				tar.pos = gv;



			//=================================
			// 2D骨力
			//=================================
			// 慣性移動
			for ( Joint2& a : tblJoint )
			{
//				a.pos += a.accell;
//				a.accell*=0.90;
			}

			for ( Joint2& j : tblJoint )
			{
//				j.mov = ( j.pos - j.prev );
			}

			for ( int i = 0 ; i < 1 ; i++ )
			{
				// 骨コリジョン 張力計算
				for ( Bone2 b : tblBone )
				{
					vect2 v = b.j1.pos - b.j0.pos;
					double l = v.length() - b.length;
					double w = 0;
	//				w = b.j0.waitht / ( b.j0.waitht + b.j1.waitht);
					//cout << w << endl;
					vect2 va  =	v.normalize()*l;
if ( &tar == &b.j0 )
{
					b.j1.tension -= va/3;
}
else
if ( &tar == &b.j1 )
{
					b.j0.tension -= va/3;
}
else
{
					b.j0.tension += va/3;
					b.j1.tension -= va/3;
}

				}

				// 張力解消
				for ( Joint2& a : tblJoint )
				{
					a.pos += a.tension;
					//a.accell += a.tension;
					a.tension=0;
				}
			}

			// 保管	
			for ( Joint2& a : tblJoint )
			{
//				a.prev = a.pos;
			}

			// 骨描画
			for ( Bone2 b : tblBone )
			{
				vect2 v0 = b.j0.pos;
				vect2 v1 = b.j1.pos;

				gra.Line( v0, v1, rgb( 1,1,1 ) );
			}


			//=================================
			// Human
			//=================================
			for ( int i = 0 ; i < 1 ; i++ )
			{
				// 骨コリジョン 張力計算
				for ( Bone3 b : human_tblBone )
				{
					vect3 v = b.j1.pos - b.j0.pos;
					double l = v.length() - b.length;
					double w = 0;
					vect3 va  =	v.normalize()*l;
					b.j0.tension += va/3;
					b.j1.tension -= va/3;

				}

				// 張力解消
				for ( Joint3& a : human_tblJoint )
				{
					a.pos += a.tension;
					a.tension=0;
				}
			}

//			if ( keys.W.rep ) cam.mat.AddTranslate( vect3( 0,0,0.5) );
//			if ( keys.S.rep ) cam.mat.AddTranslate( vect3( 0,0,-0.5) );
//			if ( keys.A.rep ) cam.mat.AddTranslate( vect3(-0.5,0,0) );
//			if ( keys.D.rep ) cam.mat.AddTranslate( vect3( 0.5,0,0) );



			


			// Human pers

			for ( Joint3& j : human_tblJoint )
			{
				//	右手系座標系
				//	右手ねじ周り
				//	roll	:z	奥+
				//	pitch	:x	右+
				//	yaw		:y	下+

				mat44	rotx;
				mat44	roty;
				mat44	rotz;
				mat44	trans;
				rotx.setRotateX(human.rot.x);
				roty.setRotateY(human.rot.y);
				rotz.setRotateZ(human.rot.z);
				trans.SetTranslate(human.pos);
				vect3 v= rotx * roty * rotz *  j.pos + human.pos;

				v += -cam.pos;

				j.world = v;

				j.disp = pers.calc(v);
//				vect3 v0 = pers.calc(v);
//				j.disp = v0;//vect2(v0.x,v0.y);


			}

			// Human 描画
			for ( Bone3 b : human_tblBone )
			{
				if ( b.j0.disp.z > 0 && b.j0.disp.z > 0 )
				{
					vect2 v0(b.j0.disp.x,b.j0.disp.y);
					vect2 v1(b.j1.disp.x,b.j1.disp.y);
					gra.Line( v0,v1, rgb(1,1,1));
				}
			}
			for ( const Joint3& j : human_tblJoint )
			{
//				gra.Fill( j.disp-3, j.disp+3,rgb(1,1,1));

				if ( j.disp.z > 0 )
				{
					vect2 v(j.disp.x,j.disp.y);
					gra.Fill( v-3,v+3, rgb(1,1,1));
				}

			}



			// マーカー表示
			mc.funcMarkerDraw2();
//			for ( Joint2 j : tblJoint ) gra.Circle( j.pos, 4, rgb(0,1,0));



			// figTriangle
			{
				int ox=256,oy=64;
				int sx =ox-128;
				int sy =oy-128;
				int ex =ox+128;
				int ey =oy+128;
//				gra.Fill(vect2(sx,sy),vect2(ex,ey),rgb(0.3,0.3,0.3));

				static int cnt = 0;
	#if 0
				auto func = [&]( double x0, double y0, double x1, double y1, int col)
				{
					gra.Line(x0,y0,x1,y1,col);
				};
				figTriangle.draw( func, 256,256,rad(cnt), rgb(0,1,1) );
	#else
				figTriangle.draw( vect2(ox,oy),rad(cnt), rgb(0,1,1) );
	#endif
				cnt++;
			}
			{
				static chrono::system_clock::time_point time_a;
				static chrono::system_clock::time_point time_b;
				static chrono::system_clock::time_point time_sec;
				static chrono::system_clock::time_point time_amt;

				time_b = chrono::system_clock::now(); 
				time_amt += time_b-time_a;
				while( chrono::duration_cast<chrono::microseconds>(chrono::system_clock::now()-time_a).count() < 16.6667*1000 )	// 60fps同期処理
				{
	 				this_thread::sleep_for (chrono::microseconds(100));
				}
				if (chrono::duration_cast<chrono::microseconds>(chrono::system_clock::now()-time_sec).count() > 1*1000*1000 ) // n sec毎表示
				{
					time_sec = chrono::system_clock::now();
					double f = chrono::duration_cast<chrono::microseconds>(time_b-time_a).count();
					cout << "time " << f/1000 << " msec" << endl;
				}
				time_a = chrono::system_clock::now();  
			}

		}
		return 0;
	}


} ;


//------------------------------------------------------------------------------
int main()
//------------------------------------------------------------------------------
{
	Apr	apr("Ray4 " __DATE__, 300,300,768, 512 );
	return apr.main();
}




	

