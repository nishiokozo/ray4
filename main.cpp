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
		for ( unsigned int i = 0 ; i < m_tblSphere.size() ; i++ )
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
		for ( unsigned int i = 0 ; i < m_tblPlate.size() ; i++ )
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

	struct Marker// : vect2
	{
		const SysGra*	pgra;
		const Figure*	pfig;
		vect2&	pos;
		bool 	bRectSelected;		//	矩形選択中、選択＆非選択
		bool 	bRectIn;			//	矩形選択中、矩形選択対象
		bool 	bSelected;			//	選択
		bool 	bAffectable;		//	削除対象
		double	th;
		int		colNormal;
		int		colSelected;

		Marker( SysGra* _gra, Figure* _fig, vect2& v, double _th, int _colNormal, int _colSelected ) : pgra(_gra), pfig(_fig), pos(v)
		{
//			x=v.x;
//			y=v.y;
			bSelected=false;
			bRectIn=false;
			bRectSelected=false;
			bAffectable = false;
			th = _th;
			colNormal = _colNormal;
			colSelected = _colSelected;
		}
		
		void draw()
		{
			bool flg =  bSelected;
			
			if ( bRectIn )
			{
				flg = bRectSelected;
			}
			
			if ( flg )			
			{
				pfig->draw( pos,th, colSelected );
			}
			else
			{
				pfig->draw( pos,th, colNormal );
			}
		
		}
	};

	Apr( const char* name, int pos_x, int pos_y, int width, int height ) : Sys( name, pos_x, pos_y, width, height )
	{
	}
	~Apr()
	{
	}

	struct MarkerController
	{
		vector<Marker>	tblMarker;

		//------------------------------------------------------------------------------
		void funcMarkerDraw()
		//------------------------------------------------------------------------------
		{
			for ( Marker m : tblMarker )
			{
				m.draw();
			}
		}
		//------------------------------------------------------------------------------
		void funcMarkerController( Figure* pFigAdder, SysMouse& mouse, SysKeys& keys, SysGra& gra )
		//------------------------------------------------------------------------------
		{
			static vect2 drag_start(0,0);
			static bool bDrag = false;


			// マーカー追加
			if ( mouse.M.hi )
			{
				tblMarker.emplace_back( Marker( &gra, pFigAdder, mouse.pos, rad(0), rgb(1,0,0), rgb(1,1,0) ) );
			}


			// マーカー削除
			if  ( keys.CTRL.on && keys.X.hi )
			{
				for ( Marker& m : tblMarker )
				{
					if ( m.bSelected )
					{
						m.bAffectable = true;
					}
				}

				for ( int i = tblMarker.size()-1 ; i >= 0 ; i-- )
				{
					if ( tblMarker[i].bAffectable )
					{
//							   tblMarker.erase(tblMarker.begin() +i);	
					}
				}
			}
			
			// マーカー選択
			if ( mouse.L.on )
			{
				struct
				{
					double	len;
					Marker*	pmark;
					int		cnt;
				} a = {99999,0,0};

				// 最近マーカーを検索
				for ( Marker& m : tblMarker )
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
						for ( Marker& m : tblMarker )
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

						for ( Marker& m : tblMarker )
						{
							m.bRectIn = false;
						}

						// 矩形内マーカーを検索
						for ( Marker& m : tblMarker )
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
					for ( Marker& m : tblMarker )
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
					for ( Marker& m : tblMarker )
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
	main()
	//------------------------------------------------------------------------------
	{
		Figure figArrow(gra);
		figArrow.vert.emplace_back( (vect2){   0,20*tan(rad(60))	-10*tan(rad(30)) } );
		figArrow.vert.emplace_back( (vect2){-10,  0 	    	   	-10*tan(rad(30)) } );
		figArrow.vert.emplace_back( (vect2){ 10,  0 				-10*tan(rad(30)) } );
		figArrow.edge.emplace_back( (ivect2){ 0,1 } );
		figArrow.edge.emplace_back( (ivect2){ 1,2 } );
		figArrow.edge.emplace_back( (ivect2){ 2,0 } );
		figArrow.col = rgb(0,0.5,1);


		Figure figTriangle(gra);
		figTriangle.vert.emplace_back( (vect2){   0,100*tan(rad(60))	-100*tan(rad(30)) } );
		figTriangle.vert.emplace_back( (vect2){-100,  0 	    	   	-100*tan(rad(30)) } );
		figTriangle.vert.emplace_back( (vect2){ 100,  0 				-100*tan(rad(30)) } );
		figTriangle.edge.emplace_back( (ivect2){ 0,1 } );
		figTriangle.edge.emplace_back( (ivect2){ 1,2 } );
		figTriangle.edge.emplace_back( (ivect2){ 2,0 } );
		figTriangle.col = rgb(0,1,1);

		Figure figCircle(gra);
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

		// ベジェ
		vector<vect2> bezier_pos=
		{
			#define X 40
			#define Y 120
			vect2( 10+X, 10+Y),
			vect2(100+X,100+Y),
			vect2(200+X, 10+Y),
			vect2(300+X,100+Y),
			#undef X
			#undef Y
		};
		for ( vect2& v : bezier_pos )	// マーカー対象に位置を登録
		{
			mc.tblMarker.emplace_back( Marker( &gra, &figCircle, v, rad(-90), rgb(1,1,0), rgb(1,0,0) ) );
		}

		// カトマル曲線
		vector<vect2> catmul_tblVert =
		{
			vect2(500,200  +0),
			vect2(500,200+ 20),
			vect2(550,200+100),
			vect2(500,200+180),
			vect2(500,200+200),
		};
		vector<int>	catmul_tblConnect
		{
			0, 1, 2, 3, 4
		};
		for ( vect2& v : catmul_tblVert )	// マーカー対象に位置を登録
		{
			mc.tblMarker.emplace_back( Marker( &gra, &figArrow, v, rad(-90), rgb(1,1,0), rgb(1,0,0) ) );
		}

		
		//骨
		struct Joint
		{
			vect2 pos;
			vect2 tension;
			vect2 accell;
			vect2 mov;
			vect2 prev;
			double len;
			Joint( vect2 v )
			{
				pos = v;
				tension = 0;
				prev = v;
				len = 0;
			}
		};
		struct Bone
		{
			Joint& j0;
			Joint& j1;
			double length;
			Bone( Joint& _j0, Joint& _j1 ) :j0(_j0), j1(_j1){}
		};
		vector<Joint> tblJoint;
		tblJoint.reserve(1000);
		vector<Bone> tblBone;
		if(1)
		{	//	四角格子メッシュ
			int ox=200,oy=300;
			const int  R=40;
			const int  X=4;
			const int  Y=5;
			for ( int x = 0 ; x < X ; x++ )
			{
				for ( int y = 0 ; y < Y ; y++ )
				{
					tblJoint.emplace_back( vect2( x*R+ox,y*R+oy) );
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
		for ( Bone& b : tblBone )
		{
			b.length = (b.j1.pos - b.j0.pos).length();
		}
		for ( Joint& j : tblJoint )	//マーカー対象に位置を登録
		{
			mc.tblMarker.emplace_back( Marker( &gra, &figCircle, j.pos, rad(-90), rgb(1,1,0), rgb(1,0,0) ) );
		}


		// 箱
		vector<vect3> box_vert=
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
		vector<vect3> box_disp;

		vector<ivect2>	box_edge
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



		double	rx = rad(0);
		double	ry = rad(0);
		double	rz = rad(0);

		while( Update() )
		{
	 		static int py=0;


			gra.Clr(rgb(0.3,0.3,0.3));
			
			//raytrace( gra, py++ );
			if ( py >= m.height ) py=0;

			//	move
	//		rx += rad(0.2);	
	//		rz += rad(1);	
	//		ry += rad(0.5);	

			double pz =4;


			//calc rotate
			box_disp.clear();
			for ( vect3 v : box_vert )
			{
	//			double x = v.x;
	//			double y = v.y;
	//			double z = v.z;

				double	x,y,z;


	#if 0
				//rz
				x=v.x*cos(rz) - v.y*sin(rz) + v.z *0;
				y=v.x*sin(rz) + v.y*cos(rz) + v.z *0;
				z=v.x* 0      + v.y*0       + v.z *1;
				v.x=x;
				v.y=y;
				v.z=z;

				//rx
				x=v.x;
				y=v.y*cos(rx) - v.z*sin(rx);
				z=v.y*sin(rx) + v.z*cos(rx);
				v.x=x;
				v.y=y;
				v.z=z;

				//ry
				x=v.x*cos(ry) - v.z*sin(ry);
				y=v.y;
				z=v.x*sin(ry) + v.z*cos(ry);
				v.x=x;
				v.y=y;
				v.z=z;
	#else
				//	右手系座標系
				//	右手ねじ周り
				//	roll	:z	奥+
				//	pitch	:x	右+
				//	yaw		:y	下+

				rx+=rad(0.03);
				ry+=rad(0.05);
				rz+=rad(0.1);
	/*
				mat44 rotx(
					1.0			,	0.0			,	0.0			,	0.0	,
					0.0			,	 cos(rx)	,	-sin(rx)	,	0.0	,
					0.0			,	 sin(rx)	,	 cos(rx)	,	0.0	,
					0.0			,	0.0			,	0.0			,	0.0	
				);

				mat44 roty(
					 cos(ry)	,	0.0			,	 sin(ry)	,	0.0	,
					 0.0		,	1.0			,	0.0			,	0.0	,
					-sin(ry)	,	0.0			,	 cos(ry)	,	0.0	,
					 0.0		,	0.0			,	0.0			,	0.0	
				);

				mat44 rotz(
					 cos(rz)	,	-sin(rz)	,	0.0			,	0.0	,
					 sin(rz)	,	 cos(rz)	,	0.0			,	0.0	,
					0.0			,	0.0			,	1.0			,	0.0	,
					0.0			,	0.0			,	0.0			,	0.0	
				);
	*/
				mat44	rotx;
				mat44	roty;
				mat44	rotz;
				rotx.setRotateX(rx);
				roty.setRotateY(ry);
				rotz.setRotateZ(rz);
		

	#if 1
				v= rotx *v ;
				v= roty *v ;
				v= rotz *v ;
	#else
				v= rotx * roty * rotz *v ;
	#endif


				
	#endif

				v.z+=pz;

				box_disp.emplace_back( v );

			}
			
			
			static	double	val=45;
			if (keys.Q.rep) {val--;cout << val <<" "<<1/tan(rad(val)) << endl; }
			if (keys.A.rep) {val++;cout << val <<" "<<1/tan(rad(val)) << endl; }
			if (keys.W.rep) {val-=5;cout << val <<" "<<1/tan(rad(val)) << endl; }
			if (keys.S.rep) {val+=5;cout << val <<" "<<1/tan(rad(val)) << endl; }

			val += -mouse.wheel/30;

			//calc pers 

			// 点
			gra.Pset(vect2(10,10),rgb(1,1,1));

			// 箱
			for ( ivect2 e : box_edge )
			{
				vect3& p = box_disp[e.p];
				vect3& n = box_disp[e.n];

				double	x,y,z;
				
				double	fovy = rad(val);	//	画角
				//画角から投影面パラメータを求める
				double	sc = m.height/2;
				double	sz = 1/tan(fovy/2);

				//pers
				double x0 = p.x/(p.z+sz)	*sc	+384;
				double y0 = p.y/(p.z+sz)	*sc	+128;
				double x1 = n.x/(n.z+sz)	*sc	+384;
				double y1 = n.y/(n.z+sz)	*sc	+128;
				gra.Line( vect2(x0,y0), vect2(x1,y1),rgb(0,1,1));

			}

			gra.Tri( vect2(55,10), vect2(10,100), vect2(100,100),rgb(1,1,0));

			gra.Tri( vect2(55,10), vect2(10,100), vect2(100,100),rgb(1,1,0));

			double a = 80;
			gra.Tri( vect2(55+a,10), vect2(10+a,100), vect2(100+a,100),rgb(1,1,0));

			//ベジェ
			gra.Bezier(bezier_pos[0],bezier_pos[1],bezier_pos[2],bezier_pos[3],rgb(1,1,1));
			vect2 v0 = bezier_pos[0];
			for ( unsigned int i = 1 ; i < bezier_pos.size() ; i++ )
			{
				vect2 v1 = bezier_pos[i];
				gra.Line(v1,v0,rgb(0,1,0));
				v0=v1;
			}
	
			
			// カトマル
			static auto catmull = []( double t, const vect2 Pm, const vect2 P0, const vect2 P1, const vect2 P2 )
			{
				//Catmull-Rom 曲線
				// P(t)=P0*(2t^3-3t^2+1)+m0*(t^3-2t^2+t)+P1*(-2t^3+3t^2)+m1*(t^3-t^2)
				// m0=(P1-Pm)/2
				// m1=(P2-P0)/2

				vect2 m0 = (P1-Pm)/2.0;
				vect2 m1 = (P2-P0)/2.0;
				vect2 P = P0*(2*t*t*t - 3*t*t +1) + m0*( t*t*t -2*t*t +t ) + P1*( -2*t*t*t + 3*t*t ) + m1*( t*t*t - t*t );

				return P;
			};

			// マーカースプライン変換表示
			if ( catmul_tblConnect.size() >=4 )
			{
				for ( unsigned int i = 0 ; i < catmul_tblConnect.size()-3 ; i++ )
				{
					double st = 0.1;

					int n = catmul_tblConnect[i];
					vect2 Q = catmull(0, catmul_tblVert[n], catmul_tblVert[n+1], catmul_tblVert[n+2], catmul_tblVert[n+3] );

					for ( double t = st ; t < 1.0 ; t+=st)
					{
						vect2 P = catmull(t, catmul_tblVert[n], catmul_tblVert[n+1], catmul_tblVert[n+2], catmul_tblVert[n+3] );
						gra.Line( P, Q, rgb(1,1,1));
						Q=P;
					}	
						vect2 P = catmull(1, catmul_tblVert[n], catmul_tblVert[n+1], catmul_tblVert[n+2], catmul_tblVert[n+3] );
						gra.Line( P, Q, rgb(1,1,1));
						
				}
			}


			// 入力

			// 慣性移動
			for ( Joint& a : tblJoint )
			{
//				a.pos += a.accell;
//				a.accell*=0.90;
			}

			// マーカー操作	
			mc.funcMarkerController( &figCircle, mouse, keys, gra );

			// 関節速度
			for ( Joint& j : tblJoint )
			{
//				j.mov = ( j.pos - j.prev );
			}

			for ( int i = 0 ; i < 1 ; i++ )
			{
				// 骨コリジョン 張力計算
				for ( Bone b : tblBone )
				{
					vect2 v = b.j1.pos - b.j0.pos;
					double l = v.length() - b.length;
					double w = 0;
	//				w = b.j0.waitht / ( b.j0.waitht + b.j1.waitht);
					//cout << w << endl;
					vect2 va  =	v.normalize()*l;
					b.j0.tension += va/3;
					b.j1.tension -= va/3;
				}

				// 張力解消
				for ( Joint& a : tblJoint )
				{
					a.pos += a.tension;
					a.accell += a.tension;
					a.tension=0;
				}
			}

			// 保管	
			for ( Joint& a : tblJoint )
			{
//				a.prev = a.pos;
			}

			// 骨描画
			for ( Bone b : tblBone )
			{
				vect2 v0 = b.j0.pos;
				vect2 v1 = b.j1.pos;

				gra.Line( v0, v1, rgb( 1,1,1 ) );
			}

			// マーカー表示
			mc.funcMarkerDraw();
//			for ( Joint j : tblJoint ) gra.Circle( j.pos, 4, rgb(0,1,0));


			// figTriangle
			{
				static int cnt = 0;
	#if 0
				auto func = [&]( double x0, double y0, double x1, double y1, int col)
				{
					gra.Line(x0,y0,x1,y1,col);
				};
				figTriangle.draw( func, 256,256,rad(cnt), rgb(0,1,1) );
	#else
				figTriangle.draw( vect2(384,128),rad(cnt), rgb(0,1,1) );
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




	

