// 2017/07/07 ray3
// 2019/06/25 ray4

#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <cmath>
using namespace std;

#include "Win.h"
#include "geom.h"
#include "key.h"


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
chrono::system_clock::time_point time_a;
chrono::system_clock::time_point time_b;

//------------------------------------------------------------------------------
void	raytrace( Win& win, int py )
//------------------------------------------------------------------------------
{
	Renderer ren;

	{
		int height	= win.m.height; 
		int width	= win.m.width; 
	
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

				win.pset(px,height-py,win.rgb(C.r,C.g,C.b));
			}
		}
		
	}

}
//------------------------------------------------------------------------------
int main()
//------------------------------------------------------------------------------
{
	Win	win("Ray4 " __DATE__, 300,300,512, 512 );


	int cnt = 0;

	struct V2
	{
		double x,y;
	};
	vector<V2> triangle=
	{
		{   0,100*tan(rad(60)) -100*tan(rad(30)) },
		{-100,  0 	    	       -100*tan(rad(30)) },
		{ 100,  0   	           -100*tan(rad(30)) },
		{   0,100*tan(rad(60)) -100*tan(rad(30)) },
	};

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

	struct	E2
	{
		int	p,n;
	};
	vector<E2>	boxedge
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

	key_init();
	while( win.exec() )
	{
		key_update();
 		static int py=0;


		win.clr(win.rgb(0.3,0.3,0.3));
		
//		raytrace( win, py++ );
		if ( py >= win.m.height ) py=0;

		//	move
//		rx += rad(0.2);	
//		rz += rad(1);	
		ry += rad(0.5);	

double pz =4;
		//calc rotate
		boxdl.clear();
		for ( vect3 v : boxvert )
		{
			double x = v.x;
			double y = v.y;
			double z = v.z;

			double	vx,vy,vz;

			//rz
			vx=x*cos(rz) - y*sin(rz);
			vy=x*sin(rz) + y*cos(rz);
			vz=z;
			x=vx;
			y=vy;
			z=vz;

			//rx
			vx=x;
			vy=y*cos(rx) - z*sin(rx);
			vz=y*sin(rx) + z*cos(rx);
			x=vx;
			y=vy;
			z=vz;

			//ry
			vx=x*cos(ry) - z*sin(ry);
			vy=y;
			vz=x*sin(ry) + z*cos(ry);
			x=vx;
			y=vy;
			z=vz;

			z+=pz;

			boxdl.push_back( (vect3){x,y,z} );

		}
		
		
static	double	val=3;
if (key.rep.q) {val--;cout << val << endl; }
if (key.rep.a) {val++;cout << val << endl; }
if (key.rep.w) {val-=0.1;cout << val << endl; }
if (key.rep.s) {val+=0.1;cout << val << endl; }


		//calc pers 
		for ( E2 e : boxedge )
		{
			vect3& p = boxdl[e.p];
			vect3& n = boxdl[e.n];

			double	x,y,z;
			
			double	fovy = rad(45);	//	画角
			//画角から投影面パラメータを求める
			double	sc = win.m.height/2;
			double	sz = sc/tan(fovy);
//cout << sz << endl;

sz=val;	sc=win.m.height/2;//*sz;
			//pers
			double x0 = p.x/(p.z+sz)	*sc	+256;
			double y0 = p.y/(p.z+sz)	*sc	+256;
			double x1 = n.x/(n.z+sz)	*sc	+256;
			double y1 = n.y/(n.z+sz)	*sc	+256;


		

			win.line(x0,y0,x1,y1,win.rgb(0,1,1));

		}
		


		//	V2
		for ( unsigned int i = 0 ; i < triangle.size()-1 ; i++ )
		{
			double xa=triangle[i].x;
			double ya=triangle[i].y;
			double xb=triangle[i+1].x;
			double yb=triangle[i+1].y;

			double th=rad(cnt/1);
			
			double x0=xa*cos(th) - ya*sin(th);
			double y0=xa*sin(th) + ya*cos(th);
			double x1=xb*cos(th) - yb*sin(th);
			double y1=xb*sin(th) + yb*cos(th);

			x0+=256;
			y0+=256;
			x1+=256;
			y1+=256;

//			win.line(x0,y0,x1,y1,win.rgb(0,1,1));
		}
		
		{
			time_b = chrono::system_clock::now();  
			while( chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now()-time_a).count() < 16.6667 )
			{
 				this_thread::sleep_for (chrono::milliseconds(1));
			}
			if ( cnt < 10 )
			{
				double f = chrono::duration_cast<chrono::microseconds>(time_b-time_a).count();
				printf("time %fsec\n", f/1000/1000 );
			}
			cnt++;
			time_a = chrono::system_clock::now();  
		}
	}

	return 0;

}
