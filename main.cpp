// ver 1.1 2017/07/07 
#include	<iostream>
using namespace std;
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <windows.h>
#include <float.h>

#include <cmath>
#include <vector>

#include "Win.h"
#include "geom.h"


const	static	double INFINIT = DBL_MAX;

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

	vect3	Q;	//	交差点(x,y,z)
	vect3	N;	//	Normal(x,y,z)
	vect3	R;	//	Reflectionion(x,y,z)
	vect3	Rr;	//	Refractive(x,y,z)


	Surface()
	{
		memset( this, 0, sizeof(*this) );

		t = INFINIT;
	}
};


struct PrimSphere : public Material
{
	vect3	P;
	double	r;

	PrimSphere( const vect3& _p, double _r, vect3 _c, double _valReflection, double _valRefractive, double _valPower, double _valEmissive, double _valTransmittance )
	{
		memset( this, 0, sizeof(*this));
		P = _p;
		r = _r;
		C = max(0.0,min(1.0,_c));
		valReflectance		= _valReflection;
		valRefractive		= _valRefractive;
		valPower			= _valPower;
		valEmissive			= _valEmissive;
		valTransmittance	= _valTransmittance;
	};
};

struct PrimPlate : public Material
{
	vect3	P;
	vect3	N;

	PrimPlate( const vect3& _p, const vect3& _n, vect3 _c, double _valReflection, double _valRefractive, double _valPower, double _valEmissive, double _valTransmittance )
	{
		memset( this, 0, sizeof(*this));
		P = _p;
		N = _n;
		C = _c;
		valReflectance		= _valReflection;
		valRefractive		= _valRefractive;
		valPower			= _valPower;
		valEmissive			= _valEmissive;
		valTransmittance	= _valTransmittance;
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
		for ( int i = 0 ; i < m_cntSphere ; i++ )
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
		for ( int i = 0 ; i < m_cntPlate ; i++ )
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
	vect3	getAsmosphereLight( const vect3& I )
	//------------------------------------------------------------------------------
	{
		double	p = 0.999999;
		double	b = I.y*p;
		double	t = min(1.0,sqrt(1.0 - p*p + b*b) - b);

		vect3	col = vect3(0);
		col = vect3(0.2,0.4,0.5);
		return	col;
	}

//	PrimSphere*	m_tblSphere[1000];
//	PrimPlate*	m_tblPlate[1000];
//	PrimLight*	m_tblLight[1000];
	vector<PrimSphere*>	m_tblSphere;
	vector<PrimPlate*>	m_tblPlate;
	vector<PrimLight*>	m_tblLight;
	int		m_cntSphere;
	int		m_cntPlate;
	int		m_cntLight;
	vect3	A;

	//------------------------------------------------------------------------------
	void	Entry( PrimSphere* a )
	//------------------------------------------------------------------------------
	{
		m_tblSphere[ m_cntSphere++ ] = a;
	}

	//------------------------------------------------------------------------------
	void	Entry( PrimPlate* a )
	//------------------------------------------------------------------------------
	{
		m_tblPlate[ m_cntPlate++ ] = a;
	}

	//------------------------------------------------------------------------------
	void	Entry( PrimLight* a )
	//------------------------------------------------------------------------------
	{
		m_tblLight[ m_cntLight++ ] = a;
	}

	//------------------------------------------------------------------------------
	~Renderer()
	//------------------------------------------------------------------------------
	{
		for ( int i = 0 ; i < m_cntSphere ; i++ ) delete m_tblSphere[i];
		for ( int i = 0 ; i < m_cntPlate  ; i++ ) delete m_tblPlate[i];
		for ( int i = 0 ; i < m_cntLight  ; i++ ) delete m_tblLight[i];
	}

	//------------------------------------------------------------------------------
	Renderer()
	//------------------------------------------------------------------------------
	{
		memset( this, 0, sizeof(*this) );
		float r,s,pw,e,tm,rl,rr;
		vect3	P,C,N;

	#define	SCENE	3
	#if SCENE==1
		Entry( new PrimPlate( P=vect3( 0  ,  0 ,0.0),N=vect3(0,1,0),C=vect3(0.8,0.8,0.8),rl=0.5,rr=1.0 ,pw=20,e= 0.0,tm=0.0 ) );
		Entry( new PrimSphere(vect3( 0.0 , 1.25, -2       ),   0.5 , vect3(1  , 0.2, 0.2), 0.5, 1.0, 20, 0.0, 0.0 ) );
		Entry( new PrimSphere(vect3( 0.0 , 0.5 , -2-0.433 ),   0.5 , vect3(0.0, 0.0, 0.0), 1.0, 1.0, 20, 0.0, 0.0 ) );
		Entry( new PrimSphere(vect3( 0.5 , 0.5 , -2+0.433 ),   0.5 , vect3(0.2, 0.2, 1.0), 0.5, 1.0, 20, 0.0, 0.0 ) );
		Entry( new PrimSphere(vect3(-0.5 , 0.5 , -2+0.433 ),   0.5 , vect3(0.0, 1.0, 0.0), 0.5, 1.0, 20, 0.0, 0.0 ) );
		Entry( new PrimLight( vect3( 4   ,  2 , -1 ), vect3(0.6, 0.8, 1.0)*40 ) );
		Entry( new PrimLight( vect3( -1  ,  2 ,  -3 ), vect3(1.0, 0.8, 0.6)*10 ) );
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
			double	th  = (double)i *(M_PI/360)*16 * 3;
			double	th2 = (double)i *(M_PI/360)*16 * 0.5;
			double	x = cos(th);
			double	z = sin(th) ;
			double	y = cos(th2) +1.2;
	//		double	y = (x+z)/2 +1.0;
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
	#if SCENE==5 //2 balls
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
	vect3 Raytrack( vect3 P, vect3 I )
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
			s	= (sur.valPower+2)/(8*M_PI)*pow( max( 0.0, dot( sur.R, -L ) ), sur.valPower );
			r	= sur.valReflectance;
			ret += r* (Raytrack( sur.Q, sur.R )+s) * Lc;

			if ( sur.valTransmittance == 0.0 )
			{
				ret += (1-r)*( d * sur.C ) * Lc;
			}
			else
			{
				I = refract( I, sur.N, sur.valRefractive/1.0 );
				sur = raycast( sur.Q, I );



				I = refract( I, sur.N, 1.0/sur.valRefractive );
				ret += (1-r)*Raytrack( sur.Q, I );
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
	void	Paint( unsigned char* to, int height, int width, int size )
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
		 		C = Raytrack( P, I );
				if ( m_cntRay > cntMax ) cntMax = m_cntRay;
				cntRay+= m_cntRay;

				int r = min( 255, (int)(255*C.r) );
				int g = min( 255, (int)(255*C.g) );
				int b = min( 255, (int)(255*C.b) );

				to[py*width*3 + px*3 +0] = b;
				to[py*width*3 + px*3 +1] = r;
				to[py*width*3 + px*3 +2] = g;
			}
		}
		
	}

};	


static LARGE_INTEGER nFreq;
//------------------------------------------------------------------------------
double	getPerformanceTime()
//------------------------------------------------------------------------------
{
	QueryPerformanceFrequency(&nFreq);

	LARGE_INTEGER time2;
	QueryPerformanceCounter(&time2);
	double time = ((double)(time2.QuadPart) / (double)nFreq.QuadPart);

	return time;
}
//------------------------------------------------------------------------------
int main()
//------------------------------------------------------------------------------
{
	Win	win(512,512,"Renderer   " __DATE__, RENDER_GDI24);

	Renderer ren;

	int cnt = 0;

	while( win.runMessage() )
	{
		double	time1 = getPerformanceTime();

		ren.Paint( win.GDI_bPixelBits, win.m_height, win.m_width, 3 );

		double	time2 = getPerformanceTime();

		if ( cnt++ < 3 ) printf("time %fsec\n", time2-time1 );
	}

	return 0;

}
