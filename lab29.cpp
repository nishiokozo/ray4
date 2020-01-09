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

#include <GL/gl.h>

using namespace std;

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

		vect3	Q;	//  Q point
		vect3	N;	//  Normal
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


	struct PrimTriangle : public Material
	{
		vect3	v0;
		vect3	v1;
		vect3	v2;

		PrimTriangle( vect3 _v0, vect3 _v1, vect3 _v2, vect3 _c, float _valReflection, float _valRefractive, float _valPower, float _valEmissive, float _valTransmittance )
		{
			v0 = _v0;
			v1 = _v1;
			v2 = _v2;
			C = max(0.0,min(1.0,_c));
			valReflectance	= _valReflection;
			valRefractive	= _valRefractive;
			valPower= _valPower;
			valEmissive	= _valEmissive;
			valTransmittance	= _valTransmittance;
		};

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
		vector<PrimTriangle>	m_tblTriangle;
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
			
			//	三角形
			for ( int i = 0 ; i < (signed)m_tblTriangle.size() ; i++ )
			{
				PrimTriangle&	obj = m_tblTriangle[i];

				const vect3& v0 = obj.v0;
				const vect3& v1 = obj.v1;
				const vect3& v2 = obj.v2;

				// ①平面(v0,v1,v2)の表（CW)の法線Nを求める
				vect3 N = cross(v1-v0,v2-v0).normalize();

				// ②視点Pと平面との距離f を求める
				float f = dot( N, P-obj.v0 );  
			
				// ③表面（視点Pが平面の表側にあるとき）だけ衝突判定を行う
				if ( f > 0 ) 
				{
					// ④衝突距離t（視線I方向の距離）を求める
					float	t = f / -dot( N, I );

					// ⑤衝突座標Q（平面上の座標）を求める
					vect3	Q = I * t + P;

					// ⑥衝突座標Q がポリゴン(v0,v1,v2)内かどうかの判定する
					float z = dot( cross(Q-v0,v1-v0), I );
//					float z = (Q.x-v0.x)*(v1.y-v0.y)-(Q.y-v0.y)*(v1.x-v0.x);
					if ( z > 0 )
					{
//						float z = (Q.x-v1.x)*(v2.y-v1.y)-(Q.y-v1.y)*(v2.x-v1.x);
						float z = dot( cross(Q-v1,v2-v1), I );
						if ( z >0 )
						{
//							float z = (Q.x-v2.x)*(v0.y-v2.y)-(Q.y-v2.y)*(v0.x-v2.x);
							float z = dot( cross(Q-v2,v0-v2), I );
							if ( z >0 )
							{
								sur.stat = Surface::STAT_FRONT;

								sur.t = t; 

								sur.Q = Q;

								sur.N = N;

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

			}
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
							sur.stat = Surface::STAT_FRONT;

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

			vect3	P,C,N;

		#define	SCENE 1
		#if SCENE==6
			float pw,e,em, tm,rl,rr;
			m_tblPlate.push_back( PrimPlate( P=vect3( 0  ,  0 ,0.0),N=vect3(0,1,0),C=vect3(0.8,0.8,0.8),rl=0.5,rr=1.0 ,pw=20,e= 0.0,tm=0.0 ) );

			mat33 m = mat33::mroty(rad(23))*mat33::mrotx(rad(-15));

			vect3	tblCube[8]=
			{
				vect3(-0.5, 0.5+1,-0.5),
				vect3( 0.5, 0.5+1,-0.5),
				vect3(-0.5,-0.5+1,-0.5),
				vect3( 0.5,-0.5+1,-0.5),
				vect3(-0.5, 0.5+1, 0.5),
				vect3( 0.5, 0.5+1, 0.5),
				vect3(-0.5,-0.5+1, 0.5),
				vect3( 0.5,-0.5+1, 0.5),
			};
			
			struct Suf
			{
				ivect3	idx1;
				ivect3	idx2;
				vect3	C;
			};
			Suf	tblQuad[]=
			{
				// 時計回りが表
				{{0,1,2},{2,1,3}, rgb(1, 0.2, 0.2)},
				{{5,4,6},{6,7,5}, rgb(1, 1.0, 0.2)},
				{{4,5,0},{0,5,1}, rgb(1, 0.2, 1.0)},
				{{2,3,6},{6,3,7}, rgb(0.2, 1.0, 0.2)},
				{{4,0,6},{6,0,2}, rgb(0.2, 0.2, 1.0)},
				{{1,5,3},{3,5,7}, rgb(0.2, 1.0, 1.0)},

			};
			for ( Suf i : tblQuad )
			{
				m_tblTriangle.push_back( 
					PrimTriangle(
						tblCube[ i.idx1.n0 ] * m,   
						tblCube[ i.idx1.n1 ] * m,   
						tblCube[ i.idx1.n2 ] * m,   
						C = i.C, rl = 0.0,rr = 0.0,pw = 20, em = 0.0, tm = 0.0
					) 
				);
				m_tblTriangle.push_back( 
					PrimTriangle(
						tblCube[ i.idx2.n0 ] * m,   
						tblCube[ i.idx2.n1 ] * m,   
						tblCube[ i.idx2.n2 ] * m,   
						C = i.C, rl = 0.0,rr = 0.0,pw = 20, em = 0.0, tm = 0.0
					) 
				);
				
			}
			/*
			m_tblTriangle.push_back( 
				PrimTriangle(
					vect3( 0.0 ,         0.0 ,  0) * m,   
					vect3(-0.5 , cos(rad(30)),  0) * m,   
					vect3( 0.5 , cos(rad(30)),  0) * m,   
					C = vect3(1  , 0.2, 0.2), rl = 0.0,rr = 0.0,pw = 20, em = 0.0, tm = 0.0
				) 
			);
			*/
			m_tblLight.push_back( PrimLight( vect3( 18   ,  18,  -18 ), vect3(1,1,1)*1300 )  );


		#endif
		#if SCENE==1	// 3 Balls Pilamid
			float pw,e,tm,rl,rr;
			m_tblPlate.push_back( PrimPlate( P=vect3( 0  ,  0 ,0.0),N=vect3(0,1,0),C=vect3(0.8,0.8,0.8),rl=0.5,rr=1.0 ,pw=20,e= 0.0,tm=0.0 ) );
			m_tblSphere.push_back( PrimSphere(vect3( 0.0 , 1.25, -2       ),   0.5 , vect3(1  , 0.2, 0.2), 0.5, 1.0, 20, 0.0, 0.0 ) );
			m_tblSphere.push_back( PrimSphere(vect3( 0.0 , 0.5 , -2-0.433 ),   0.5 , vect3(0.0, 0.0, 0.0), 1.0, 1.0, 20, 0.0, 0.0 ) );
			m_tblSphere.push_back( PrimSphere(vect3( 0.5 , 0.5 , -2+0.433 ),   0.5 , vect3(0.2, 0.2, 1.0), 0.5, 1.0, 20, 0.0, 0.0 ) );
			m_tblSphere.push_back( PrimSphere(vect3(-0.5 , 0.5 , -2+0.433 ),   0.5 , vect3(0.0, 1.0, 0.0), 0.5, 1.0, 20, 0.0, 0.0 ) );
			m_tblLight.push_back( PrimLight( vect3( 0   ,  30 ,  0 ), vect3(1,1,1)*1300 )  );
		#endif
		#if SCENE==2 //5 balls
			float pw,e,tm,rl,rr;
			m_tblPlate.push_back( PrimPlate( P=vect3( 0  ,  0 ,0.0),N=vect3(0,1,0),C=vect3(0.8,0.8,0.8),rl=0.5,rr=1.0 ,pw=20,e= 0.0,tm=0.0 ) );
			m_tblSphere.push_back( PrimSphere(vect3(-2.0 , 0.5 , -2 ),   0.5 , vect3(0.0, 0.0, 0.0), 1.0 , 1.0, 20, 0.0, 0.0 ) );
			m_tblSphere.push_back( PrimSphere(vect3(-1.0 , 0.5 , -2 ),   0.5 , vect3(0.0, 0.0, 0.0), 0.75, 1.0, 20, 0.0, 0.0 ) );
			m_tblSphere.push_back( PrimSphere(vect3( 0.0 , 0.5 , -2 ),   0.5 , vect3(0.0, 0.0, 0.0), 0.5 , 1.0, 20, 0.0, 0.0 ) );
			m_tblSphere.push_back( PrimSphere(vect3( 1.0 , 0.5 , -2 ),   0.5 , vect3(0.0, 0.0, 0.0), 0.25, 1.0, 20, 0.0, 0.0 ) );
			m_tblSphere.push_back( PrimSphere(vect3( 2.0 , 0.5 , -2 ),   0.5 , vect3(0.0, 0.0, 0.0), 0.0 , 1.0, 20, 0.0, 0.0 ) );
			m_tblLight.push_back( PrimLight( vect3( 0   ,  20 ,  -2 ), vect3(1.0, 1.0, 1.0)*800 ) );
		#endif
		#if SCENE==3 // ring
			float pw,e,tm,rl,rr,r;
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
		#endif
		#if SCENE==4 //twin balls
			m_tblPlate.push_back( PrimPlate( vect3( 0   ,  0 ,  0    ), normalize(vect3(0, 1,0))  , vect3(0.8, 0.8, 0.8), 0.5, 1.0, 20, 0.0, 0.0 ) );
			m_tblSphere.push_back( PrimSphere(vect3(-1.0 , 1.0 , -2 ),   1.0 , vect3(1.0, 0.5, 0.5), 0.2, 1.0, 20, 0.0, 0.0 ) );
			m_tblSphere.push_back( PrimSphere(vect3( 1.0 , 1.0 , -2 ),   1.0 , vect3(0.0, 0.0, 0.0), 0.2, 1.0, 20, 0.0, 0.0 ) );
			m_tblLight.push_back( PrimLight( vect3( 0   ,  20 ,  -2 ), vect3(1.0, 1.0, 1.0)*1800 ) );
		#endif
		#if SCENE==5//2 balls
			float pw,e,tm,rl,rr,r;
			m_tblPlate.push_back( PrimPlate( P=vect3(0  , 0 ,0.0),N=vect3(0,1,0),C=vect3(0.8,0.8,0.8),rl=0.5,rr=1.0 ,pw=20,e= 0.0,tm=0.0 ) );
			m_tblSphere.push_back( PrimSphere(P=vect3( 0.5,1.0,3.0),r=0.5       ,C=vect3(0.0,0.0,1.0),rl=0.5,rr=1.0 ,pw=20,e=10.0,tm=0.0 ) );
			m_tblSphere.push_back( PrimSphere(P=vect3(-0.5,1.0,3.0),r=0.5       ,C=vect3(0.0,1.0,0.0),rl=0.5,rr=1.0 ,pw=20,e=10.0,tm=0.0 ) );
			m_tblSphere.push_back( PrimSphere(P=vect3( 0.0,1.5,3.0),r=0.5       ,C=vect3(1.0,0.0,0.0),rl=0.5,rr=1.0 ,pw=20,e=10.0,tm=0.0 ) );
			m_tblSphere.push_back( PrimSphere(P=vect3( 0.0,0.5,3.0),r=0.5       ,C=vect3(1.0,1.0,0.0),rl=0.5,rr=1.0 ,pw=20,e=10.0,tm=0.0 ) );
			m_tblSphere.push_back( PrimSphere(P=vect3( 0.0,1.0,2.75),r=0.25      ,C=vect3(1.0,1.0,1.0),rl=0.5,rr=1.0 ,pw=20,e=10.0,tm=0.0 ) );
			m_tblLight.push_back( PrimLight( P=vect3( 1.0 ,15, 0 )          ,C=vect3(1,1,1)*360 )  );
		#endif
		}

		//------------------------------------------------------------------------------
		vect3 Raytrace( vect3 P, vect3 I, int nest )
		//------------------------------------------------------------------------------g
		{
			vect3 ret = vect3(0,0,0);

			if ( nest<=0 ) return ret;

			Surface sur;
			for ( PrimLight& lgt : m_tblLight )
			{
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
			}

			return ret;
		}


	};


	struct	Texture
	{
		GLuint	texname;
		int	width;
		int height;

		Texture( int _width, int _height ) : width(_width), height(_height)
		{
			glGenTextures( 1, &texname );
			glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
			glBindTexture( GL_TEXTURE_2D, texname );
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0 );
			glBindTexture( GL_TEXTURE_2D, 0 );	// Bindを開放
		}

		~Texture()
		{
			glDeleteTextures( 1, &texname );
		}

		void loadRGBA( void* pBuf )
		{
			glBindTexture( GL_TEXTURE_2D, texname );
			glTexSubImage2D( GL_TEXTURE_2D, 0, 0,0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pBuf );
			glBindTexture( GL_TEXTURE_2D, 0 );	// Bindを外す
		}

		void Enable()
		{
			glEnable(GL_TEXTURE_2D);
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
			glBindTexture(GL_TEXTURE_2D, texname);
		}
		void Disable()
		{
			//glBindTexture( GL_TEXTURE_2D, 0 );	// Bindを外す
			glDisable(GL_TEXTURE_2D);
		}
	};

	unique_ptr<Texture>pTexture = 0;

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

	static GLubyte* pImage = 0;

	//初期化
	if ( pImpl->bResetAll )
	{
		pImpl->bResetAll = false;

		// カメラ
		pers.cam.pos = vect3( 2.0, 2.0, -5.0 );
		pers.cam.at = vect3( 0,  1.0, 0 );
		pers.cam.Update();

//		pImpl->pTexture = new Impl::Texture(64,64);
		unique_ptr<Impl::Texture> pTmp(new Impl::Texture(gra.GetWidth(),gra.GetHeight()));
		pImpl->pTexture = move(pTmp);

		int size= gra.GetWidth()*gra.GetHeight()*4;
		pImage = new GLubyte[size];
		for ( int i = 0 ; i < size ; i++ )
		{
			pImage[i]=0;
		}
		

	}

	if ( pImpl->bResetParam )
	{
		pImpl->bResetParam = false;
	}


static float py = 0;

	{
		Impl::Renderer ren;


		float step = 3.0;
		{
			float width		= gra.GetWidth(); 
			float height	= gra.GetHeight(); 

			float	aspect = width / height;
		
			vect3	posScr = vect3(0,0.0,0);
			vect3	posView = vect3(0,0.0,-5);


			for( float py = 0 ; py < height ; py += step )
			{
				for( float px = 0 ; px < width ; px += step )
				{
					float	x = (px /  width) *2.0-1.0;
					float	y = (py / height) *2.0-1.0;
					vect3	P = vect3( x*aspect, y, 0 ) + posScr;	// P : 投影面
					vect3	I = normalize(P - posView);				// I : 視線ベクトル

					// カメラマトリクスでビューを回転
					P = P* pers.cam.mat;
					I = I* pers.cam.mat.GetRotate();

			 		rgb C = ren.Raytrace( P, I, 5 );

#define TST	1
				
#if TST
					gra.Pset( vect2(x,y) ,C);
#else
					unsigned char cr = min(255,C.r*255.0);
					unsigned char cg = min(255,C.g*255.0);
					unsigned char cb = min(255,C.b*255.0);
				
					int tx = px;
					int ty = py;
					int idx= (ty*width+tx)*4;
				
					unsigned char* adr = &pImage[idx];
					adr[0] = cr;
					adr[1] = cg;
					adr[2] = cb;
					adr[3] = 255;
#endif
				}
			}
			py += step;
			if ( py >= height ) py = 0;
			
//		gra.Print( 0,10, to_string(width)+" x "+to_string(height) );
		}


	}

#if TST
#else
	// リアルタイムにテクスチャ転送
	pImpl->pTexture->loadRGBA( pImage );


	vect2 v0=vect2(-0.5,-0.5)*2;
	vect2 v1=vect2( 0.5, 0.5)*2;
	{
		glDepthFunc( GL_ALWAYS );	// ENABLEのときだけ 必ず書く
	
		pImpl->pTexture->Enable();

	    glBegin(GL_QUADS);
	    glTexCoord2f(0.0, 0.0); glVertex2f(v0.x, v0.y);
	    glTexCoord2f(1.0, 0.0); glVertex2f(v1.x, v0.y);
	    glTexCoord2f(1.0, 1.0); glVertex2f(v1.x, v1.y);
	    glTexCoord2f(0.0, 1.0); glVertex2f(v0.x, v1.y);
	    glEnd();

		pImpl->pTexture->Disable();

	}
#endif

}
