///2019/11/22

#include <iostream>
#include <memory>
#include <vector>
#include <map>
#include <thread>
#include <chrono>
#include <cmath>
#include <functional>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iomanip>
#include <tuple>
#include <random>

#include "geom.h"

#include "SysSound.h"
#include "SysKeys.h"
#include "SysMouse.h"
#include "SysGra.h"

#include "func.h"
#include "pers.h"

#include "lab.h"
#include "lab29.h"

struct Lab29::Impl
{
	bool	bResetAll = true;
	bool	bResetParam = true;
	bool	bPause = false;
	bool	bStep = false;

	vector<shared_ptr<Obj>>	tbl_pObj;

	//--

	struct  Material
	{
		vect3	C;					//	ベースカラー
		float	valReflectance;		//	反射率
		float	valRefractive;		//	屈折率
		float	valPower;			//	スペキュラ強度
		float	valEmissive;		//	エミッシブ
		float	valTransmittance;	//	透過率
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

		float	t;
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
			t		= infinit;
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
		float	r;

		PrimSphere( const vect3& _p, float _r, vect3 _c, float _valReflection, float _valRefractive, float _valPower, float _valEmissive, float _valTransmittance )
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

		PrimPlate( const vect3& _p, const vect3& _n, vect3 _c, float _valReflection, float _valRefractive, float _valPower, float _valEmissive, float _valTransmittance )
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
		vector<PrimSphere>	m_tblSphere;
		vector<PrimPlate>	m_tblPlate;
		vector<PrimLight>	m_tblLight;
		vect3	A;

	public:


		//------------------------------------------------------------------------------
		Surface raycast( vect3& P, const vect3& I )
		//------------------------------------------------------------------------------
		{
			Surface sur;

		//	P += I*2e-10;	// double
			P += I*2e-5;	// float 
			
			sur.flg = false;

			sur.t  = infinit;
			sur.stat  = Surface::STAT_NONE;
			
			//	球
			for ( int i = 0 ; i < (signed)m_tblSphere.size() ; i++ )
			{
				PrimSphere&	obj = m_tblSphere[i];

				vect3	O = obj.P;
				float	r = obj.r;

				////////////////////////////////////////////////////////////////////////////////////
				//                                                                                //
				//                                                                                //
				//           O              O球の中心                                             //
				//          /|                                                                    //
				//        l/ |}r            r:球の半径                                            //
				//        /  |                                                                    //
				//       /   :}a            a:線と球表面までの距離                                //
				//      /    :                                                                    //
				//     P-----+--->I         P:Rayの照射元,I:Rayの単位ベクトル                     //
				//        b                 b:衝突位置                                            //
				//                                                                                //
				//                                                                                //
				//                                                                                //
				//  l = P-O                                                                       //
				//  b = I･l                                                                       //
				//  aa = r^2 - l･l + b^2   ※(r+a)^2 + b^2 - l^2 からの変形。aaが負の数なら衝突   //
				//  a = ±sqrt( aa )                                                              //
				//  t = a - b                                                                     //
				//                                                                                //
				//                                                                                //
				////////////////////////////////////////////////////////////////////////////////////




				vect3	l = P-O;
				float	b = dot( I, l );
				float	aa = r*r - dot(l,l)+ b*b;

				if ( aa >= 0 )
				{
					float t = - sqrt( aa ) - b;
					if ( t >= 0 )
					{
						if ( sur.t >= t )
						{
							int	stat = Surface::STAT_FRONT;
		
							sur.stat = stat;

							sur.t = t; 

							sur.Q = I * t + P;

							sur.N = normalize(sur.Q - O);

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

			}

			//	床
			for ( int i = 0 ; i < (signed)m_tblPlate.size() ; i++ )
			{
				PrimPlate&	obj = m_tblPlate[i];

				float	f = dot(obj.N,P - obj.P);
				if ( f > 0 )
				{
					float	t = -f/dot(obj.N,I);

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
		Renderer()
		//------------------------------------------------------------------------------
		{
			A = vect3(0,0,0);

			float pw,e,tm,rl,rr;
			vect3	P,C,N;

		#define	SCENE 1
		#if SCENE==1
			m_tblPlate.push_back( PrimPlate( P=vect3( 0  ,  0 ,0.0),N=vect3(0,1,0),C=vect3(0.8,0.8,0.8),rl=0.5,rr=1.0 ,pw=20,e= 0.0,tm=0.0 ) );
			m_tblSphere.push_back( PrimSphere(vect3( 0.0 , 1.25, -2       ),   0.5 , vect3(1  , 0.2, 0.2), 0.5, 1.0, 20, 0.0, 0.0 ) );
			m_tblSphere.push_back( PrimSphere(vect3( 0.0 , 0.5 , -2-0.433 ),   0.5 , vect3(0.0, 0.0, 0.0), 1.0, 1.0, 20, 0.0, 0.0 ) );
			m_tblSphere.push_back( PrimSphere(vect3( 0.5 , 0.5 , -2+0.433 ),   0.5 , vect3(0.2, 0.2, 1.0), 0.5, 1.0, 20, 0.0, 0.0 ) );
			m_tblSphere.push_back( PrimSphere(vect3(-0.5 , 0.5 , -2+0.433 ),   0.5 , vect3(0.0, 1.0, 0.0), 0.5, 1.0, 20, 0.0, 0.0 ) );
			m_tblLight.push_back( PrimLight( vect3( 0   ,  30 ,  0 ), vect3(1,1,1)*1300 )  );
			A = vect3( 0.2,0.4,0.6)*0.0;
		#endif
		#if SCENE==2 //5 balls
			m_tblPlate.push_back( PrimPlate( P=vect3( 0  ,  0 ,0.0),N=vect3(0,1,0),C=vect3(0.8,0.8,0.8),rl=0.5,rr=1.0 ,pw=20,e= 0.0,tm=0.0 ) );
			m_tblSphere.push_back( PrimSphere(vect3(-2.0 , 0.5 , -2 ),   0.5 , vect3(0.0, 0.0, 0.0), 1.0 , 1.0, 20, 0.0, 0.0 ) );
			m_tblSphere.push_back( PrimSphere(vect3(-1.0 , 0.5 , -2 ),   0.5 , vect3(0.0, 0.0, 0.0), 0.75, 1.0, 20, 0.0, 0.0 ) );
			m_tblSphere.push_back( PrimSphere(vect3( 0.0 , 0.5 , -2 ),   0.5 , vect3(0.0, 0.0, 0.0), 0.5 , 1.0, 20, 0.0, 0.0 ) );
			m_tblSphere.push_back( PrimSphere(vect3( 1.0 , 0.5 , -2 ),   0.5 , vect3(0.0, 0.0, 0.0), 0.25, 1.0, 20, 0.0, 0.0 ) );
			m_tblSphere.push_back( PrimSphere(vect3( 2.0 , 0.5 , -2 ),   0.5 , vect3(0.0, 0.0, 0.0), 0.0 , 1.0, 20, 0.0, 0.0 ) );
			m_tblLight.push_back( PrimLight( vect3( 0   ,  20 ,  -2 ), vect3(1.0, 1.0, 1.0)*800 ) );
			A = vect3( 0.2,0.4,0.6)*1.0;
		#endif
		#if SCENE==3 // ring
			m_tblPlate.push_back( PrimPlate( P=vect3( 0  ,  0 ,0.0),N=vect3(0,1,0),C=vect3(0.8,0.8,0.8),rl=0.5,rr=1.0 ,pw=20,e= 0.0,tm=0.0 ) );
			m_tblSphere.push_back( PrimSphere(  vect3( 0 , 1.0 , 0 ),   0.5 ,  vect3(0.0, 0.0, 0.0),   0.5,   1.0 ,  100,  0.0,  0.0 ) );
			int	max = 16*3;
			for ( int i = 0 ; i < max ; i++ )
			{
				float	th  = (float)i *(pi/360)*16 * 3;
				float	th2 = (float)i *(pi/360)*16 * 0.5;
				float	x = cos(th);
				float	z = sin(th) ;
				float	y = cos(th2) +1.2;
				m_tblSphere.push_back( PrimSphere(P=vect3( x , y , z ),r=0.2 ,C=vect3( x, y,  z) ,rl=0.2,rr=0.0 ,pw=100,e= 0.0,tm=0.0 ) );

			}
			m_tblLight.push_back( PrimLight( vect3( 0   ,  30 ,  0 ), vect3(1,1,1)*1800 )  );
			m_tblLight.push_back( PrimLight( vect3(-30   ,  30 ,  0 ), vect3(0.5,1,1)*1800 )  );
			m_tblLight.push_back( PrimLight( vect3(60   ,  80 ,  0 ), vect3(1,1,0.5)*4800 )  );
			m_tblLight.push_back( PrimLight( vect3(-60   ,  80 , 0 ), vect3(1,0.5,1)*4800 )  );
			A = vect3( 0.2,0.4,0.6)*0.0;
		#endif
		#if SCENE==4 //twin balls
			m_tblPlate.push_back( PrimPlate( vect3( 0   ,  0 ,  0    ), normalize(vect3(0, 1,0))  , vect3(0.8, 0.8, 0.8), 0.5, 1.0, 20, 0.0, 0.0 ) );
			m_tblSphere.push_back( PrimSphere(vect3(-1.0 , 1.0 , -2 ),   1.0 , vect3(1.0, 0.5, 0.5), 0.2, 1.0, 20, 0.0, 0.0 ) );
			m_tblSphere.push_back( PrimSphere(vect3( 1.0 , 1.0 , -2 ),   1.0 , vect3(0.0, 0.0, 0.0), 0.2, 1.0, 20, 0.0, 0.0 ) );
			m_tblLight.push_back( PrimLight( vect3( 0   ,  20 ,  -2 ), vect3(1.0, 1.0, 1.0)*1800 ) );
			A = vect3( 0.2,0.4,0.6)*1.0;
		#endif
		#if SCENE==5//2 balls
			m_tblPlate.push_back( PrimPlate( P=vect3(0  , 0 ,0.0),N=vect3(0,1,0),C=vect3(0.8,0.8,0.8),rl=0.5,rr=1.0 ,pw=20,e= 0.0,tm=0.0 ) );
			m_tblSphere.push_back( PrimSphere(P=vect3( 0.5,1.0,3.0),r=0.5       ,C=vect3(0.0,0.0,1.0),rl=0.5,rr=1.0 ,pw=20,e=10.0,tm=0.0 ) );
			m_tblSphere.push_back( PrimSphere(P=vect3(-0.5,1.0,3.0),r=0.5       ,C=vect3(0.0,1.0,0.0),rl=0.5,rr=1.0 ,pw=20,e=10.0,tm=0.0 ) );
			m_tblSphere.push_back( PrimSphere(P=vect3( 0.0,1.5,3.0),r=0.5       ,C=vect3(1.0,0.0,0.0),rl=0.5,rr=1.0 ,pw=20,e=10.0,tm=0.0 ) );
			m_tblSphere.push_back( PrimSphere(P=vect3( 0.0,0.5,3.0),r=0.5       ,C=vect3(1.0,1.0,0.0),rl=0.5,rr=1.0 ,pw=20,e=10.0,tm=0.0 ) );
			m_tblSphere.push_back( PrimSphere(P=vect3( 0.0,1.0,2.75),r=0.25      ,C=vect3(1.0,1.0,1.0),rl=0.5,rr=1.0 ,pw=20,e=10.0,tm=0.0 ) );
			m_tblLight.push_back( PrimLight( P=vect3( 1.0 ,15, 0 )          ,C=vect3(1,1,1)*360 )  );
			A = vect3( 0.2,0.4,0.6)*0.5;
		#endif
		}

		//------------------------------------------------------------------------------
		vect3 Raytrace( vect3 P, vect3 I, int nest )
		//------------------------------------------------------------------------------g
		{
			vect3 ret = vect3(0,0,0);

			if ( nest<=0 ) return ret;

			Surface sur;
			PrimLight&	lgt = m_tblLight[0];
			vect3	Lc;
			vect3	L;
			float	d;
			float	s=0;
			float	r=0;
			
			if ( (sur = raycast( P, I )).flg )
			{
				L	= normalize(sur.Q - lgt.P);
				Lc	= lgt.C / dot(sur.Q - lgt.P, sur.Q - lgt.P);
				d	= max( 0.0, dot( sur.N, -L ) );
				s	= (sur.valPower+2)/(8*pi)*pow( max( 0.0, dot( sur.R, -L ) ), sur.valPower );
				r	= sur.valReflectance;
				ret += r* (Raytrace( sur.Q, sur.R, nest-1 )+vect3(s,s,s)) * Lc;

				if ( sur.valTransmittance == 0.0 )
				{
					ret += (1-r)*( d * sur.C ) * Lc;
				}
				else
				{
					I = refract( I, sur.N, sur.valRefractive/1.0 );
					sur = raycast( sur.Q, I );



					I = refract( I, sur.N, 1.0/sur.valRefractive );
					ret += (1-r)*Raytrace( sur.Q, I, nest-1 );
				}

			}
			else
			{
				L = (sur.Q - lgt.P).normalize();
				Lc = lgt.C / dot(sur.Q - lgt.P, sur.Q - lgt.P);
				ret += vect3(s,s,s);
			}

			return ret;
		}


	};


};
Lab29::Lab29() : pImpl( new Lab29::Impl ){}



//------------------------------------------------------------------------------
void Lab29::Update( SysKeys& keys, SysMouse& mouse, SysSound& sound, SysGra& gra, Pers& pers, float delta, int& text_y, Cp& cp )
//------------------------------------------------------------------------------
{
	// 画面クリア
	gra.Clr(rgb(0.0,0.0,0.0));
//	pers.grid.DrawGrid3d( gra, pers, vect3(0,0,0), mat33::midentity(), 10, 10, 1, rgb(0.2,0.2,0.2) );
	gra.Print(1,(float)text_y++,"29 : raytrace" ); 

	//初期化
	if ( pImpl->bResetAll )
	{
		pImpl->bResetAll = false;

		// カメラ
		pers.cam.pos = vect3( 0.0, 1.0, -5.0 );
		pers.cam.at = vect3( 0,  1.0, 0 );
		pers.cam.Update();

	}

	if ( pImpl->bResetParam )
	{
		pImpl->bResetParam = false;
	}


static float py = 0;

	{
		Impl::Renderer ren;


		float step = 2.0;
		{
			float width		= gra.GetWidth(); 
			float height	= gra.GetHeight(); 

			float	aspect = width / height;
		
			vect3	posScr = vect3(0,0.0,0);
			vect3	posEye = vect3(0,0.0,-5);


			for( float py = 0 ; py < height ; py += step )
			{
				for( float px = 0 ; px < width ; px += step )
				{
					float	x = (px /  width) *2.0-1.0;
					float	y = (py / height) *2.0-1.0;
					vect3	P = vect3( x*aspect, y, 0 ) + posScr;	// P : 投影面
					vect3	I = normalize(P - posEye);				// I : 視線ベクトル

					P = P* pers.cam.mat;
					I = I* pers.cam.mat.GetRotate();

			 		rgb C = ren.Raytrace( P, I, 5 );
					gra.Pset( vect2(x,y) ,C);
				}
			}
			py += step;
			if ( py >= height ) py = 0;
			
		}


	}
}
