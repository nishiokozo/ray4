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
		m_tblSphere.push_back( a );

	}

	//------------------------------------------------------------------------------
	void	Entry( PrimPlate* a )
	//------------------------------------------------------------------------------
	{
		m_tblPlate.push_back( a );
	}

	//------------------------------------------------------------------------------
	void	Entry( PrimLight* a )
	//------------------------------------------------------------------------------
	{
		m_tblLight.push_back( a );
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

#if 0
//------------------------------------------------------------------------------
void	raytrace( SysWin& win, int py )
//------------------------------------------------------------------------------
{
	Renderer ren;

	{
		int height	= m.height; 
		int width	= m.width; 
	
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

				win.(*this).Pset(px,height-py,gra.Rgb(C.r,C.g,C.b));
			}
		}
		
	}

}
#endif

struct Apr : public Sys
{
	Apr( const char* name, int pos_x, int pos_y, int width, int height ) : Sys( name, pos_x, pos_y, width, height ) 
	{}
	int main();
} ;

//------------------------------------------------------------------------------
int main()
//------------------------------------------------------------------------------
{
	Apr	apr("Ray4 " __DATE__, 300,300,768, 512 );
	return apr.main();
}

//------------------------------------------------------------------------------
Apr::main()
//------------------------------------------------------------------------------
{
	struct Figure
	{
		SysGra&	gra;
		vector<vect2> vert;
		vector<ivect2> edge;
		int	col;
		Figure( SysGra& _gra ) : gra(_gra) {}
//		void draw( function<void(double,double,double,double,int)> line , double ofs_x, double ofs_y, double th, int col )
//		{
//			for ( ivect2 e : edge )
//			{
//				vect2& p = vert[e.p];
//				vect2& n = vert[e.n];
//
//				double x0=p.x*cos(th) - p.y*sin(th) + ofs_x;
//				double y0=p.x*sin(th) + p.y*cos(th) + ofs_y;
//				double x1=n.x*cos(th) - n.y*sin(th) + ofs_x;
//				double y1=n.x*sin(th) + n.y*cos(th) + ofs_y;
//
//				line(x0,y0,x1,y1,col);
//			}
//		}
		void draw( double ofs_x, double ofs_y, double th, int col )
		{
			for ( ivect2 e : edge )
			{
				vect2& p = vert[e.p];
				vect2& n = vert[e.n];

				double x0=p.x*cos(th) - p.y*sin(th) + ofs_x;
				double y0=p.x*sin(th) + p.y*cos(th) + ofs_y;
				double x1=n.x*cos(th) - n.y*sin(th) + ofs_x;
				double y1=n.x*sin(th) + n.y*cos(th) + ofs_y;

				gra.Line(x0,y0,x1,y1,col);
			}
		}

	};

	Figure triangle(gra);
	triangle.vert.push_back( (vect2){   0,100*tan(rad(60))	-100*tan(rad(30)) } );
	triangle.vert.push_back( (vect2){-100,  0 	    	   	-100*tan(rad(30)) } );
	triangle.vert.push_back( (vect2){ 100,  0 				-100*tan(rad(30)) } );
	triangle.edge.push_back( (ivect2){ 0,1 } );
	triangle.edge.push_back( (ivect2){ 1,2 } );
	triangle.edge.push_back( (ivect2){ 2,0 } );
	triangle.col = gra.Rgb(0,1,1);

#if 1
	Figure fig(gra);
	fig.vert.push_back( (vect2){   0,20*tan(rad(60))	-10*tan(rad(30)) } );
	fig.vert.push_back( (vect2){-10,  0 	    	   	-10*tan(rad(30)) } );
	fig.vert.push_back( (vect2){ 10,  0 				-10*tan(rad(30)) } );
	fig.edge.push_back( (ivect2){ 0,1 } );
	fig.edge.push_back( (ivect2){ 1,2 } );
	fig.edge.push_back( (ivect2){ 2,0 } );
	fig.col = gra.Rgb(0,0.5,1);
#endif	

	
	vector<vect3> boxvert=
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
	vector<vect3> boxdl;

	vector<ivect2>	boxedge
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


		gra.Clr(gra.Rgb(0.3,0.3,0.3));
		
//		raytrace( win, py++ );
		if ( py >= m.height ) py=0;

		//	move
//		rx += rad(0.2);	
//		rz += rad(1);	
//		ry += rad(0.5);	

		double pz =4;


		//calc rotate
		boxdl.clear();
		for ( vect3 v : boxvert )
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

			boxdl.push_back( v );

		}
		
		
		static	double	val=45;
		if (keys.Q.rep) {val--;cout << val <<" "<<1/tan(rad(val)) << endl; }
		if (keys.A.rep) {val++;cout << val <<" "<<1/tan(rad(val)) << endl; }
		if (keys.W.rep) {val-=5;cout << val <<" "<<1/tan(rad(val)) << endl; }
		if (keys.S.rep) {val+=5;cout << val <<" "<<1/tan(rad(val)) << endl; }

		val += mouse.wheel/30;
//cout << mouse.wheel << endl;
		//calc pers 
		for ( ivect2 e : boxedge )
		{
			vect3& p = boxdl[e.p];
			vect3& n = boxdl[e.n];

			double	x,y,z;
			
			double	fovy = rad(val);	//	画角
			//画角から投影面パラメータを求める
			double	sc = m.height/2;
			double	sz = 1/tan(fovy/2);

			//pers
			double x0 = p.x/(p.z+sz)	*sc	+256;
			double y0 = p.y/(p.z+sz)	*sc	+256;
			double x1 = n.x/(n.z+sz)	*sc	+256;
			double y1 = n.y/(n.z+sz)	*sc	+256;
			gra.Line(x0,y0,x1,y1,gra.Rgb(0,1,1));

		}
#if 1
			gra.Tri(55,10, 10,100, 100,100,gra.Rgb(1,1,0));

			gra.Tri(55,10, 10,100, 100,100,gra.Rgb(1,1,0));

			double a = 80;
			gra.Tri(55+a,10, 10+a,100, 100+a,100,gra.Rgb(1,1,0));

			a=40;
			double	b = 120;
			gra.Bezier(10+a,10+b, 100+a,100+b, 200+a,10+b, 300+a,100+b,gra.Rgb(0,1,0));

			gra.Circle( 10+a, 10+b, 10, gra.Rgb(1,0,0));
			gra.Circle(100+a,100+b, 10, gra.Rgb(1,0,0));
			gra.Circle(200+a, 10+b, 10, gra.Rgb(1,0,0));
			gra.Circle(300+a,100+b, 10, gra.Rgb(1,0,0));
//cout << "circle " << gra.m.tblCircle.size() << endl;
		

#endif

		auto catmull = []( double t, const vect2 Pm, const vect2 P0, const vect2 P1, const vect2 P2 )
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

		struct Mark : vect2
		{
			SysGra&	gra;
			Figure&	fig;
			bool 	bMouseoverSelected;
			bool 	bMouseover;
			bool 	bSelected;
			double	th;
			Mark( SysGra& _gra, Figure& _fig, vect2 v, double _th ) : gra(_gra), fig(_fig)
			{
				x=v.x;
				y=v.y;
				bSelected=false;
				bMouseover=false;
				bMouseoverSelected=false;
				th = _th;
			}
			
			void draw( bool bDrag )
			{
				bool flg =  bSelected;
				
				if ( bDrag ) 
				{
					if ( bMouseover )
					{
						flg = bMouseoverSelected;
					}
				}
				
				if ( flg )			
				{
	//				fig.draw( func, m.x,m.y,m.th, gra.Rgb(1,0,0) );
					fig.draw( x,y,th, gra.Rgb(1,0,0) );
				}
				else
				{
	//				fig.draw( func, m.x,m.y,m.th, gra.Rgb(1,1,0) );
					fig.draw( x,y,th, gra.Rgb(1,1,0) );
				}
			
			}
			

		};
		

		static vector<Mark>	tblMark =
		{
			Mark( gra, fig, vect2(500,200  +0), rad(-90) ),
			Mark( gra, fig, vect2(500,200+ 20), rad(-90) ),
			Mark( gra, fig, vect2(550,200+100), rad(-90) ),
			Mark( gra, fig, vect2(500,200+180), rad(-90) ),
			Mark( gra, fig, vect2(500,200+200), rad(-90) ),
		};
		
		static vect2 drag_start(0,0);
		static bool bDrag = false;
	
		vect2 mpos( mouse.sx, mouse.sy );


		// マーカー追加
		if ( mouse.M.hi )
		{
			tblMark.push_back( Mark( gra, fig, mpos, rad(0) ) );
		}

		// マーカー選択
		if ( mouse.L.on )
		{
			struct
			{
				double	len;
				Mark*	pmark;
				int		cnt;
			} a = {99999,0,0};

			// 最近マーカーを検索
			for ( Mark& m : tblMark )
			{
				double len = (m-mpos).length();
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
					drag_start = mpos;
				}

				// マーカー全解除
				if ( keys.CTRL.on ){}
				else
				if ( keys.SHIFT.on ){}
				else
				if ( a.pmark && a.pmark->bSelected == true ){}
				else
				{
					for ( Mark& m : tblMark )
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
				// 矩形表示
				if ( bDrag )
				{
					double x0 = min( drag_start.x, mpos.x);
					double y0 = min( drag_start.y, mpos.y);
					double x1 = max( drag_start.x, mpos.x);
					double y1 = max( drag_start.y, mpos.y);

					gra.Line( x0,y0,x1,y0, gra.Rgb(0,0.5,1));
					gra.Line( x0,y1,x1,y1, gra.Rgb(0,0.5,1));
					gra.Line( x0,y0,x0,y1, gra.Rgb(0,0.5,1));
					gra.Line( x1,y0,x1,y1, gra.Rgb(0,0.5,1));

					for ( Mark& m : tblMark )
					{
						m.bMouseover = false;
					}

					// 矩形内マーカーを検索
					for ( Mark& m : tblMark )
					{
						double len = (m-mpos).length();
						if ( m.x > x0 && m.x < x1 && m.y > y0 && m.y < y1 )
						{
							m.bMouseover = true;
							if ( keys.CTRL.on )
							{
								m.bMouseoverSelected = !m.bSelected;
							}
							else
							{
								m.bMouseoverSelected = true;
							}
						}
					}

				}
				else
				// マーカー移動
				for ( Mark& m : tblMark )
				{
					if ( m.bSelected )
					{
						m.x += mouse.mx;
						m.y += mouse.my;
					}
				}
			}
		}
		else
		{
			if ( bDrag )
			{
				bDrag = false;
				for ( Mark& m : tblMark )
				{
					if ( m.bMouseover )
					{
						m.bSelected = m.bMouseoverSelected;
					}
					m.bMouseover = false;
					m.bMouseoverSelected = false;
				}
			}
		}
		
		// マーカー表示
		for ( Mark m : tblMark )
		{
//			auto func = [&]( double x0, double y0, double x1, double y1, int col)
//			{
//				gra.Line(x0,y0,x1,y1,col);
//			};

			bool flg =  m.bSelected;
			
			if ( bDrag ) 
			{
				if ( m.bMouseover )
				{
					flg = m.bMouseoverSelected;
				}
			}
			
			if ( flg )			
			{
//				fig.draw( func, m.x,m.y,m.th, gra.Rgb(1,0,0) );
				fig.draw( m.x,m.y,m.th, gra.Rgb(1,0,0) );
			}
			else
			{
//				fig.draw( func, m.x,m.y,m.th, gra.Rgb(1,1,0) );
				fig.draw( m.x,m.y,m.th, gra.Rgb(1,1,0) );
			}
		}
		
		// マーカースプライン変換表示
		if ( tblMark.size() >=4 )
		{
			for ( unsigned int i = 0 ; i < tblMark.size()-3 ; i++ )
			{
				double st = 0.1;

				vect2 Q = catmull(0, tblMark[i], tblMark[i+1], tblMark[i+2], tblMark[i+3] );

				for ( double t = st ; t < 1.0 ; t+=st)
				{
					vect2 P = catmull(t, tblMark[i], tblMark[i+1], tblMark[i+2], tblMark[i+3] );
					gra.Line( P.x, P.y, Q.x, Q.y, gra.Rgb(1,1,1));
					Q=P;
				}	
					vect2 P = catmull(1, tblMark[i], tblMark[i+1], tblMark[i+2], tblMark[i+3] );
					gra.Line( P.x, P.y, Q.x, Q.y, gra.Rgb(1,1,1));
					
			}
		}

		// fig
		{
//			auto func = [&]( double x0, double y0, double x1, double y1, int col)
//			{
//				gra.Line(x0,y0,x1,y1,col);
//			};
			fig.draw( 200,200,rad(-45), gra.Rgb(1,0,0) );
		}

		// triangle
		{
			static int cnt = 0;
#if 0
			auto func = [&]( double x0, double y0, double x1, double y1, int col)
			{
				gra.Line(x0,y0,x1,y1,col);
			};
			triangle.draw( func, 256,256,rad(cnt), gra.Rgb(0,1,1) );
#else
			triangle.draw( 256,256,rad(cnt), gra.Rgb(0,1,1) );
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
			if (chrono::duration_cast<chrono::microseconds>(chrono::system_clock::now()-time_sec).count() > 1*1000*1000 ) // 1sec毎表示
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
	

