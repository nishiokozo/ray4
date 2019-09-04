// 2017/07/07 ray3
// 2019/06/25 ray4

#include <iostream>
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
using namespace std;

#include "geom.h"


#include "SysKeys.h"
#include "SysMouse.h"
#include "SysGra.h"
#include "Sys.h"

#include "raytrace.h"
#include "obj.h"
#include "skeleton.h"


struct Apr : public Sys
{
	//------------------------------------------------------------------------------
	Apr( const char* name, int pos_x, int pos_y, int width, int height ) : Sys( name, pos_x, pos_y, width, height )
	//------------------------------------------------------------------------------
	{}

	//------------------------------------------------------------------------------
	~Apr()
	//------------------------------------------------------------------------------
	{}

	long long	time_peak = 0;


	struct Manupirator
	{

		bool bAxisX = true;;
		bool bAxisY = true;;
		bool bAxisZ = true;;

		//------------------------------------------------------------------------------
		void DrawAxis( vect2 mpos, Apr& apr )
		//------------------------------------------------------------------------------
		{
			apr.gra.SetZTest( false );

			vect3 pos = apr.pers.calcScreenToWorld( vect3(mpos,0) );

			vect3 v0 = apr.pers.calcWorldToScreen3( pos  );



			// 軸表示
			float l = 0.1;
			if ( bAxisX  )
			{
				vect3 v1 = v0 + vect3(
					apr.pers.cam.mat.m[0][0] / apr.pers.aspect,
					apr.pers.cam.mat.m[1][0],
					apr.pers.cam.mat.m[2][0]
				) * l;
				apr.gra.Line( vect2(v0.x,v0.y), vect2(v1.x,v1.y), rgb(0.8,0.2,0.2), 2.0 );
			}
			if ( bAxisY  )
			{
				vect3 v1 = v0 + vect3(
					apr.pers.cam.mat.m[0][1] / apr.pers.aspect,
					apr.pers.cam.mat.m[1][1],
					apr.pers.cam.mat.m[2][1]
				) * l;
				apr.gra.Line( vect2(v0.x,v0.y), vect2(v1.x,v1.y), rgb(0.2,0.8,0.2), 2.0 );
			}
			if ( bAxisZ )
			{
				vect3 v1 = v0 + vect3(
					apr.pers.cam.mat.m[0][2] / apr.pers.aspect,
					apr.pers.cam.mat.m[1][2],
					apr.pers.cam.mat.m[2][2]
				) * l;
				apr.gra.Line( vect2(v0.x,v0.y), vect2(v1.x,v1.y), rgb(0.1,0.3,1), 2.0 );

			}

			// 軸名表示
			apr.gra.Print( mpos+apr.gra.Dot(16,-12),string("")+(bAxisX?"X":"")+(bAxisY?"Y":"")+(bAxisZ?"Z":"") ); 

			apr.gra.SetZTest( true );

		}
	} axis;


	Pers pers;

	//------------------------------------------------------------------------------
	void line3d( vect3 p0, vect3 p1, rgb col )
	//------------------------------------------------------------------------------
	{
		float l = 0.2;
		vect3 a = p0* pers.cam.mat.invers();
		vect3 b = p1* pers.cam.mat.invers();
		vect3 v0;
		vect3 v1;
		bool flg = pers.calcScissorLine3d( a, b, v0, v1 );
		if ( flg ) gra.Line( vect2(v0.x,v0.y), vect2(v1.x,v1.y), col );
	}

	//------------------------------------------------------------------------------
	void pset3d( vect3 p0, rgb col, float wide )
	//------------------------------------------------------------------------------
	{
		vect3 v = pers.calcWorldToScreen3( p0 );
		if ( v.z > 0 )
		{
			gra.Pset( vect2(v.x,v.y), col, wide );
		}

	}

	
	//------------------------------------------------------------------------------
	bool IsIntersectPlate( vect3 plate_P, vect3 plate_N, vect3 P , vect3 I, vect3& Q)
	//------------------------------------------------------------------------------
	{
		// 球と変面殿衝突判定
		float	f = dot(plate_N, P - plate_P);
	//	if ( f > 0 )
		{
			float	t = -f/dot( plate_N, I );

			if ( t >= 0 )
			{
				Q = I * t + P;
				return true;
			}
		}
		return false;
	};

	//------------------------------------------------------------------------------
	bool IsIntersectSphereLine( vect3 sphere_P, float sphere_r, vect3 line_P , vect3 line_I )
	//------------------------------------------------------------------------------
	{
		//	球と直線の衝突判定
		vect3	OP = line_P - sphere_P;
		float	b = dot( line_I, OP );
		float	aa = sphere_r*sphere_r - dot(OP,OP)+ b*b;
		return ( aa>=0 ) ;
	};

	struct
	{
		//------------------------------------------------------------------------------
		void DrawGrid( Apr& apr, vect3 pos, int NUM_U, int NUM_V, float dt, rgb col )
		//------------------------------------------------------------------------------
		{	// ミニグリッド
			vect3 vt = vect3(0,0,0);
			float du = NUM_U*dt;
			float dv = NUM_V*dt;
			vect3 a;
			vect3 b;
			{
				a = pos+vect3(-du, 0,-du);
				b = pos+vect3( du, 0,-du);
				vt = vect3(0,0,dt);
				for ( int i = 0 ; i < NUM_V*2+1 ; i++ )
				{
					apr.line3d( a, b, col );
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
					apr.line3d( a, b, col );
					a+=vt;
					b+=vt;
				}
			}			

			{//原点表示
				float r = 0.1;
				vect3 v0;
				for ( int i = 0 ; i <= 360 ; i+=20 )
				{
					vect3 p = vect3( r*cos(rad(i)), 0, r*sin(rad(i)) ) + pos;
					vect3 v1 = apr.pers.calcWorldToScreen3( p );
					if ( i > 0 ) apr.gra.Line( v0,v1, col );
					v0 = v1;
				}
			}
		}

	} grid;
	
	

	struct Select
	{
		struct
		{
			float	w;
			Joint*	pj;
		} one = {0,0};

		vect2 rect_st = vect2(0,0);			//	矩形選択開始位置
		enum class CALC
		{
			NONE,
			ADD,
			SUB,
			COPY,
			REV,
		};
		CALC rect_mode = CALC::NONE;	//	矩形選択中フラグ

		//------------------------------------------------------------------------------
		void Setup( Pers& pers, Skeleton& skeleton, vect2 mpos )
		//------------------------------------------------------------------------------
		{
			one = {0,0};

			for ( Joint& j : skeleton.tblJoint )
			{
				if ( j.bCtrl == false ) continue;

				vect3 v = pers.calcWorldToScreen3( j.pos );

				if ( (vect2(v.x,v.y)-mpos).abs() < 0.04f )
				{
					if ( one.w < v.z ) // 近い場合はより手前が優先
					{
						one.w = v.z;
						one.pj = &j;
					}
				}
			}
			if ( one.pj )
			{
				#if 0
				{
					// 優先度つけ
					for ( Joint& j : skeleton.tblJoint )
					{
						j.priority = 999;
					}
					function<void( Joint&,int)> funcSetPriority = [&funcSetPriority] ( Joint& j, int prio )
					{
						j.priority = prio;
						for ( Joint& r : j.relative )
						{
							if ( r.priority > prio+1 ) funcSetPriority( r, prio+1 );
						}
					};
					
					funcSetPriority( *one.pj, 1 );
				}
				#endif

			}
		}

		// 矩形カーソル開始 新規選択
		//------------------------------------------------------------------------------
		void SelectRectNew( vect2 mpos )
		//------------------------------------------------------------------------------
		{
			rect_mode = CALC::COPY;
			rect_st = mpos;
		}

		// 矩形カーソル開始 追加選択
		//------------------------------------------------------------------------------
		void SelectRectAdd( vect2 mpos )
		//------------------------------------------------------------------------------
		{
			rect_mode = CALC::ADD;
			rect_st = mpos;
		}

		// 矩形カーソル開始 反転選択
		//------------------------------------------------------------------------------
		void SelectRectRev( vect2 mpos )
		//------------------------------------------------------------------------------
		{
			rect_mode = CALC::REV;
			rect_st = mpos;
		}

		// 矩形カーソル開始 削除選択
		//------------------------------------------------------------------------------
		void SelectRectSub( vect2 mpos )
		//------------------------------------------------------------------------------
		{
			rect_mode = CALC::SUB;
			rect_st = mpos;
		}
		
		//------------------------------------------------------------------------------
		void calcRectMode( CALC rect_mode, bool& bPreselect, bool& bSelected )
		//------------------------------------------------------------------------------
		{
			switch( rect_mode )
			{
				case CALC::ADD:		if ( bPreselect ) bSelected = true;
					break;

				case CALC::SUB:		if ( bPreselect ) bSelected = false;
					break;

				case CALC::COPY:	bSelected = bPreselect;
					break;

				case CALC::REV:		if ( bPreselect ) bSelected = !bSelected;
					break;

				case CALC::NONE:
					break;
			}
			bPreselect = false;
		};


		// 矩形カーソル終了（選択決定）
		//------------------------------------------------------------------------------
		void SelectRectEnd( Skeleton& skeleton )
		//------------------------------------------------------------------------------
		{
			for ( Joint& j : skeleton.tblJoint )
			{
				calcRectMode( rect_mode, j.stat.bPreselect, j.stat.bSelected );
			}

			rect_mode = CALC::NONE;

		}

		// 矩形カーソル選択	
		//------------------------------------------------------------------------------
		void SelectRectBegin( Pers& pers, Skeleton& skeleton , vect2 mpos )
		//------------------------------------------------------------------------------
		{
			vect2 v0 = min( rect_st, mpos );
			vect2 v1 = max( rect_st, mpos );

			for ( Joint& j : skeleton.tblJoint )
			{
				if ( j.bCtrl == false ) continue;
			
				j.stat.bPreselect = false;

				vect2 v = pers.calcWorldToScreen2( j.pos );

				if ( v.x > v0.x && v.x < v1.x && v.y > v0.y && v.y < v1.y )
				{
					j.stat.bPreselect = true;
				}
			}
		}

		// 単独 新規選択
		//------------------------------------------------------------------------------
		void SelectOneOnly( Skeleton& skeleton )
		//------------------------------------------------------------------------------
		{
			// 選択クリア
			for ( Joint& j : skeleton.tblJoint )
			{
				j.stat.bSelected = false;
			}
			
			one.pj->stat.bSelected = true;
		}

		// 単独 追加選択
		//------------------------------------------------------------------------------
		void SelectOneAdd()
		//------------------------------------------------------------------------------
		{
			one.pj->stat.bSelected = true;
		}

		// 単独 反転規選択
		//------------------------------------------------------------------------------
		void SelectOneRev()
		//------------------------------------------------------------------------------
		{
			one.pj->stat.bSelected = !one.pj->stat.bSelected;
		}

		// 単独 削除選択
		//------------------------------------------------------------------------------
		void SelectOneSub()
		//------------------------------------------------------------------------------
		{
			one.pj->stat.bSelected = false;
		}

		
		// 選択リストのJoint移動
		//------------------------------------------------------------------------------
		void MoveSelected( Pers& pers, Skeleton& skeleton, vect2 gmov )
		//------------------------------------------------------------------------------
		{

			vect3 v = vect3(gmov.x*pers.aspect, gmov.y, 0)/one.w/pers.rate;
			mat44 mrot = pers.cam.mat;
			mrot.SetTranslate(vect3(0,0,0));
		//	mrot.invers(); 逆行列にしなくても同じ結果
			v = v* mrot;
			for ( Joint& j : skeleton.tblJoint )
			{
				if ( j.stat.bSelected )
				{
					j.pos += v ;
				}
			}
			
			// キーフレームへ反映
			skeleton.RefrectKeyframe();
		}
		
		//--

		// 選択リスト表示
		//------------------------------------------------------------------------------
		void DrawJoint( Pers& pers, SysGra& gra, Skeleton& skeleton , vect2 mpos )
		//------------------------------------------------------------------------------
		{	
			gra.SetZTest( false );

			for ( Joint& j : skeleton.tblJoint )
			{

				bool bPreselect = j.stat.bPreselect;
				bool bSelected = j.stat.bSelected;
				
				calcRectMode( rect_mode, bPreselect, bSelected );

				if ( bSelected )
				{
					gra.Pset( pers.calcWorldToScreen3( j.pos ), rgb(1,0,0), 11 );
				}

			}

			// 矩形カーソル 表示
			if (  rect_mode != CALC::NONE )
			{
				gra.Box( rect_st, mpos, rgb(1,1,1)*0.5f);
			}

			// 矩形カーソル 情報表示
			for ( Joint& j : skeleton.tblJoint )
			{
				if ( j.bCtrl == false ) continue;

				vect2 pos = pers.calcWorldToScreen2( j.pos );
				gra.Print( pos+gra.Dot(14,0), to_string(j.id) );
			}

			gra.SetZTest( true );
		}

	} selector;



	// 軸回りの回転マトリクスを取得
	//------------------------------------------------------------------------------
//	mat33 GetRotateAxis( vect3 axis, float th )
	mat33 GetRotateAxis( vect3 axis )
	//------------------------------------------------------------------------------
	{
		vect3 nx,ny,nz;

/*		ny = axis;
		nx = vect3(1,0,0);
		nz = cross(nx,ny).normalize();
		nx = cross(nx,ny).normalize();
*/
		float ry	= atan2(axis.z,axis.x);
		float lxz	= sqrt( axis.z*axis.z + axis.x*axis.x );
		float rz	= atan2(axis.y,lxz);



		return mrotz( rz )  * mroty( ry );
/*
		float	c = cos(th);
		float	s = sin(th);
		mat33 rot = mat33(
			1,  0,  0,
			0,  c, -s,
			0,  s,  c
		);

		return rot * mrotz( rz )  * mroty( ry );
*/		
	}
	
	//------------------------------------------------------------------------------
	int main()
	//------------------------------------------------------------------------------
	{
		int text_y = 0;

		unique_ptr<Skeleton> pSkeleton(new Skeleton);

		{
			//読み込み
			unique_ptr<Skeleton> pNew(new Skeleton);
//			pNew->LoadSkeleton( "human.mot" );
			pNew->LoadSkeleton( "bone.mot" );
			pSkeleton = move(pNew);
		}
		pSkeleton->stat.bShowSkin = false;
		pSkeleton->stat.bShowLocus = false;

		// ドラム
		struct Drum
		{

			const float s = 0.05f;
			const float l = 0.03f;
			const float m = 0.02f;
			const float n = 0.02f;
			vector<vect3> vert=
			{
				{	-s,		 s,		-s	},
				{	 s,		 s,		-s	},
				{	-s,		-s,		-s	},
				{	 s,		-s,		-s	},
				{	-s,		 s,		 s	},
				{	 s,		 s,		 s	},
				{	-s,		-s,		 s	},
				{	 s,		-s,		 s	},

				{	-l,		-s-l, 	-l	},	//yマーク
				{	 l,		-s-l, 	-l	},
				{	-l,		-s-l, 	 l	},
				{	 l,		-s-l, 	 l	},

				{	s+m,	+m, 	-m	},	//xマーク
				{	s+m,	+m, 	+m	},
				{	s+m,	-m, 	 0	},

				{	-n,	 -n, 	s+n	},	//zマーク
				{	+n,	 -n, 	s+n	},
				{	-n,	 +n, 	s+n	},
				{	+n,	 +n, 	s+n	},
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

			Drum()
			{
				{
					int	ofs = vert.size();
					int cnt = 0;
					float r = 0.05;
					float w = 0.05;
					for ( int i = 0 ; i < 360 ; i+= 30 )
					{
						float z = r*cos(rad(i));
						float y = r*sin(rad(i));
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
			}

			//------------------------------------------------------------------------------
			void DrawDrum( Apr& apr, Pers& pers, SysGra& gra,  vect3 pos, mat33 m  )
			//------------------------------------------------------------------------------
			{
				disp.clear();

				for ( vect3 v : vert )
				{

					//	右手系座標系
					//	右手ねじ周り
					//	roll	:z	奥+
					//	pitch	:x	右+
					//	yaw		:y	下+
					v= v * m + pos;

					v = v * pers.cam.mat.invers();

					disp.emplace_back( v );

				}


				// 軸
				if(0)
				{
					vect3	nx = vect3( m.m[0][0], m.m[0][1], m.m[0][2] );
					vect3	ny = vect3( m.m[1][0], m.m[1][1], m.m[1][2] );
					vect3	nz = vect3( m.m[2][0], m.m[2][1], m.m[2][2] );
					apr.line3d( pos,pos+nx*0.2, rgb(1,0,0) );
					apr.line3d( pos,pos+ny*0.2, rgb(0,1,0) );
					apr.line3d( pos,pos+nz*0.2, rgb(0,0,1) );
				}
				
				// Tri
				for ( ivect3 t : tri )
				{
					vect3 v0 = pers.calcViewScreen3( disp[t.n0] );
					vect3 v1 = pers.calcViewScreen3( disp[t.n1] );
					vect3 v2 = pers.calcViewScreen3( disp[t.n2] );
//					if ( v0.z>0 )
					{
						gra.Tri( v0,v1,v2, rgb(1,0,1));
						gra.Tri( v2,v1,v0, rgb(1,0,1)/2);
					}

				}
				for ( ivect2 e : edge )
				{
					const vect3& a = disp[e.p];
					const vect3& b = disp[e.n];

					vect3 v0;
					vect3 v1;
					bool flg = pers.calcScissorLine3d( a, b, v0, v1 );
					if ( flg )
					{
						gra.Line( vect2(v0.x,v0.y), vect2(v1.x,v1.y), rgb(0,1,1));
					}
				}
			}
			
		} drum;


		// ドラム
		struct Box
		{

			const float s = 0.04f;
			const float l = 0.03f;
			const float m = 0.02f;
			const float n = 0.02f;
			vector<vect3> vert=
			{
				{	-s,		 s,		-s	},//0
				{	 s,		 s,		-s	},//1
				{	-s,		-s,		-s	},//2
				{	 s,		-s,		-s	},//3
				{	-s,		 s,		 s	},//4
				{	 s,		 s,		 s	},//5
				{	-s,		-s,		 s	},//6
				{	 s,		-s,		 s	},//7

				{	-l,		-s-l, 	-l	},//8	//yマーク
				{	 l,		-s-l, 	-l	},//9
				{	-l,		-s-l, 	 l	},//10
				{	 l,		-s-l, 	 l	},//11

				{	s+m,	+m, 	-m	},//	//xマーク
				{	s+m,	+m, 	+m	},//
				{	s+m,	-m, 	 0	},//

				{	-n,		 -n, 	s+n	},//	//zマーク
				{	+n,		 -n, 	s+n	},//
				{	-n,		 +n, 	s+n	},//
				{	+n,		 +n, 	s+n	},//
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

			Box()
			{
			}


			//------------------------------------------------------------------------------
			void DrawBox( Apr& apr, Pers& pers, SysGra& gra,  vect3 pos, mat33 m , bool bAxis = true, bool bTri = true )
			//------------------------------------------------------------------------------
			{
				disp.clear();

				for ( vect3 v : vert )
				{

					//	右手系座標系
					//	右手ねじ周り
					//	roll	:z	奥+
					//	pitch	:x	右+
					//	yaw		:y	下+
					v= v * m + pos;

					v = v * pers.cam.mat.invers();

					disp.emplace_back( v );

				}


				// 軸
				if( bAxis )
				{
					vect3	nx = vect3( m.m[0][0], m.m[0][1], m.m[0][2] );
					vect3	ny = vect3( m.m[1][0], m.m[1][1], m.m[1][2] );
					vect3	nz = vect3( m.m[2][0], m.m[2][1], m.m[2][2] );
					apr.line3d( pos,pos+nx*0.2, rgb(1,0,0) );
					apr.line3d( pos,pos+ny*0.2, rgb(0,1,0) );
					apr.line3d( pos,pos+nz*0.2, rgb(0,0,1) );
				}
				
				// Tri
				if ( bTri )
				{
					for ( ivect3 t : tri )
					{
						vect3 v0 = pers.calcViewScreen3( disp[t.n0] );
						vect3 v1 = pers.calcViewScreen3( disp[t.n1] );
						vect3 v2 = pers.calcViewScreen3( disp[t.n2] );
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

					vect3 v0;
					vect3 v1;
					bool flg = pers.calcScissorLine3d( a, b, v0, v1 );
					if ( flg )
					{
						gra.Line( vect2(v0.x,v0.y), vect2(v1.x,v1.y), rgb(0,1,1));
					}
				}
			}
			
		} box;

	


		cout<<fixed<<setprecision(24);

#if 1 // camera
		pers.cam.pos = vect3(  0.3, 0.7, -1.2 );
		pers.cam.at = vect3( 0,  0.7, 0 );
#endif

if(0)
{
	mat33 m1 = mrotx(rad(30)) * mroty(rad(15)) * mrotz(rad(8));
	m1.dump();	
	m1.invers().dump();
	m1.dump();	

	(m1*m1.invers()).dump();


	mat33 m2 = mrotz(rad(8)) * mroty(rad(15)) * mrotx(rad(30));
	m2.dump();	

	(m1*m2).invers();

	return 0 ;				
}
		//===========================================================================
		while( Update() )
		{
			text_y = 0;

			//=================================
			// カメラ
			//=================================


			// パースペクティブ
			if (keys.Y.rep) {pers.fovy-=2;cout << pers.fovy <<" "<<1/tan(rad(pers.fovy)) << endl; }
			if (keys.H.rep) {pers.fovy+=2;cout << pers.fovy <<" "<<1/tan(rad(pers.fovy)) << endl; }


			// パース更新
			pers.Update( vect2( gra.GetWidth(), gra.GetHeight() ) );

			// カメラ回転
			if ( (!keys.ALT.on && mouse.R.on && !mouse.L.on && !mouse.M.on) || (keys.ALT.on && !mouse.R.on && mouse.L.on && !mouse.M.on) ) pers.cam.Rotation( -vect3(mouse.gmov,0)*18.0f );

			// カメラ平行移動
			if ( mouse.M.on ) pers.cam.Move( -vect3(mouse.gmov,0)/pers.getW((pers.cam.pos-pers.cam.at).abs()));

			// マウスホイールZOOM
			if ( !keys.ALT.on  ) pers.cam.Zoom( -mouse.wheel*2/gra.GetHeight()/pers.getW((pers.cam.pos-pers.cam.at).abs()) );
			
			// カメラ移動
			if ( (keys.ALT.on && mouse.R.on) || ( mouse.R.on && mouse.L.on ) ) pers.cam.Zoom( -mouse.gmov.y/pers.getW((pers.cam.pos-pers.cam.at).abs()) );
			

			// カメラマトリクス計算
			{
				pers.cam.mat.LookAt( pers.cam.pos, pers.cam.at, pers.cam.up );
			}

			//=================================
			//入力
			//=================================
			{

				if ( pSkeleton->cur.bSelecting ==false && keys.SHIFT.on && (keys.UP.hi || keys.DOWN.hi || keys.LEFT.hi ||keys.RIGHT.hi) )
				{
					pSkeleton->cur.bSelecting = true;
					pSkeleton->cur.selecting_act = pSkeleton->cur.act;
					pSkeleton->cur.selecting_pose = pSkeleton->cur.pose;
				}
				if ( !keys.SHIFT.on && (keys.UP.hi || keys.DOWN.hi || keys.LEFT.hi ||keys.RIGHT.hi) )
				{
					pSkeleton->cur.bSelecting = false;
				}
				
				// キーフレームロード
				if ( keys.CTRL.on && keys.L.hi ) 
				{
					//読み込み
					unique_ptr<Skeleton> pNew(new Skeleton);
					pNew->LoadSkeleton( "human.mot" );

					pSkeleton = move(pNew);
				}

				// キーフレームセーブ
				if ( keys.CTRL.on && keys.S.hi ) pSkeleton->SaveSkeleton();

				// キーフレームペースト
				if ( keys.CTRL.on && keys.V.hi ) pSkeleton->PastKeyframe();

				// キーフレームコピー
				if ( keys.CTRL.on && keys.C.hi ) pSkeleton->CopyKeyframe();

				// キーフレームカット
				if ( keys.CTRL.on && keys.X.hi ) pSkeleton->CutKeyframe();

				// キーフレーム追加
				if ( keys.K.hi ) pSkeleton->InsertKeyframe();

				// キーフレーム次
				if ( keys.RIGHT.rep ) pSkeleton->NextKeyframe();

				// キーフレーム最後
				if ( keys.CTRL.on && keys.RIGHT.rep ) pSkeleton->LastKeyframe();

				// キーフレーム前
				if ( keys.LEFT.rep ) pSkeleton->PrevKeyframe();

				// キーフレーム先頭
				if ( keys.CTRL.on && keys.LEFT.rep ) pSkeleton->TopKeyframe();

				// アニメーション記録
				if ( keys.I.hi ) pSkeleton->AddAnimation();

				// アニメーション前
				if ( keys.UP.rep ) pSkeleton->PrevAnimation();

				// アニメーション次
				if ( keys.DOWN.rep ) pSkeleton->NextAnimation();

				// アニメーションリクエスト
				if ( keys.P.hi ) pSkeleton->ReqAnimation();

				// アニメーション再生
				if ( pSkeleton->anim.bPlaying )	pSkeleton->PlayAnimation();

				// X/Y/Z軸選択モード切替
				if ( keys.Z.hi ) {axis.bAxisZ = true;	axis.bAxisX = false;	axis.bAxisY = false;}
				if ( keys.X.hi ) {axis.bAxisZ = false;	axis.bAxisX = true;	axis.bAxisY = false;}
				if ( keys.C.hi ) {axis.bAxisZ = false;	axis.bAxisX = false;	axis.bAxisY = true;}
				if ( keys.V.hi ) {axis.bAxisZ = true;	axis.bAxisX = true;	axis.bAxisY = true;}
			}


			if ( keys._1.hi ) pSkeleton->stat.bShowBone = !pSkeleton->stat.bShowBone;
			if ( keys._2.hi ) pSkeleton->stat.bShowSkin = !pSkeleton->stat.bShowSkin;
			if ( keys._3.hi ) pSkeleton->stat.bShowLocus = !pSkeleton->stat.bShowLocus;




			// マウス座標（投影面座標）を３Ｄ空間座標に逆変換
			if(0)
			{
				vect3 P = pers.calcScreenToWorld( vect3(mouse.gpos,0) );
				vect3 I = pers.calcRayvect( P );


				line3d( vect3(0,0,0), P, vect3(1,1,0));
				line3d( vect3(0,0,0), P+I*10.0f, vect3(1,1,1));
			}
			
			//ジョイント選択
			{

				// 最近点検索
				if ( !keys.ALT.on && mouse.L.hi ) 
					selector.Setup( pers, *pSkeleton, mouse.gpos );

				// 矩形カーソル開始 新規選択
				if ( !keys.ALT.on && mouse.L.on && !keys.CTRL.on && !keys.SHIFT.on && selector.one.pj == 0 && selector.rect_mode == Select::CALC::NONE ) 
					selector.SelectRectNew( mouse.gpos );

				// 矩形カーソル開始 追加選択
				if ( !keys.ALT.on && mouse.L.on && !keys.CTRL.on && keys.SHIFT.on && selector.one.pj == 0 && selector.rect_mode == Select::CALC::NONE ) 
					selector.SelectRectAdd( mouse.gpos );

				// 矩形カーソル開始 反転選択
				if ( !keys.ALT.on && mouse.L.on && keys.CTRL.on && !keys.SHIFT.on && selector.one.pj == 0 && selector.rect_mode == Select::CALC::NONE ) 
					selector.SelectRectRev( mouse.gpos );

				// 矩形カーソル開始 削除選択
				if ( !keys.ALT.on && mouse.L.on && keys.CTRL.on && keys.SHIFT.on && selector.one.pj == 0 && selector.rect_mode == Select::CALC::NONE ) 
					selector.SelectRectSub( mouse.gpos );

				// 矩形カーソル終了（選択決定）
				if ( !keys.ALT.on && !mouse.L.on && selector.rect_mode != Select::CALC::NONE ) 
					selector.SelectRectEnd( (*pSkeleton) );

				// 矩形カーソル選択	
				if ( !keys.ALT.on && mouse.L.on && selector.rect_mode != Select::CALC::NONE ) 
					selector.SelectRectBegin( pers, (*pSkeleton) , mouse.gpos );

				// 単独 新規選択
				if ( !keys.ALT.on && mouse.L.hi && !keys.CTRL.on && !keys.SHIFT.on && selector.one.pj && selector.one.pj->stat.bSelected == false ) 
					selector.SelectOneOnly( (*pSkeleton) );

				// 単独 追加選択
				if ( !keys.ALT.on && mouse.L.hi && !keys.CTRL.on && keys.SHIFT.on && selector.one.pj ) 
					selector.SelectOneAdd();

				// 単独 反転選択
				if ( !keys.ALT.on && mouse.L.hi && keys.CTRL.on && !keys.SHIFT.on && selector.one.pj ) 
					selector.SelectOneRev();

				// 単独 削除選択
				if ( !keys.ALT.on && mouse.L.hi && keys.CTRL.on && keys.SHIFT.on && selector.one.pj ) 
					selector.SelectOneSub();
				
				// 選択リストのJoint移動
				if ( !keys.ALT.on && mouse.L.on && !keys.CTRL.on && !keys.SHIFT.on && selector.one.pj ) 
				{
					Skeleton& skeleton = (*pSkeleton);
					vect2 gmov = mouse.gmov;

					vect3 v = vect3(gmov.x*pers.aspect, gmov.y, 0)/selector.one.w/pers.rate;
					mat44 mrot = pers.cam.mat;
					mrot.SetTranslate(vect3(0,0,0));
				//	mrot.invers(); 逆行列にしなくても同じ結果
					v = v* mrot;
					for ( Joint& j : skeleton.tblJoint )
					{
						if ( j.stat.bSelected )
						{
							j.pos += v ;
						}
					}
					
					// キーフレームへ反映
					skeleton.RefrectKeyframe();
				}
			}





			//=================================
			// 更新
			//=================================

			// 画面クリア
			gra.Clr(rgb(0.3,0.3,0.3));

#if 1 	//剛体実験
			// 優先度つけ
			for ( Joint& j : pSkeleton->tblJoint )
			{
				j.weight = 0.33;
				if ( j.id == 2  ) j.weight = 0.000;
				if ( j.id == 0  ) j.weight = 0.000;
				if ( j.id == 1  ) j.weight = 0.000;
				if ( j.id == 3  ) j.weight = 0.000;
				if ( j.stat.bSelected && mouse.L.on ) j.weight = 0.0;
			}
#endif
			//スケルトン更新
			pSkeleton->UpdateSkeleton();

			// 床グリッド描画
			grid.DrawGrid( *this, vect3(0,0,0), 10, 10, 1, vect3(0.2,0.2,0.2) );

			// スケルトン 描画
			pSkeleton->DrawSkeleton( pers, gra );

			{
				mat33	mmune = midentity();
				static mat33	mkata = midentity();;
				mat33	mhiji = midentity();;
				mat33	mte = midentity() ;
				vect3	p0 = pSkeleton->tblJoint[0].pos;
				vect3	p1 = pSkeleton->tblJoint[1].pos;
				vect3	p2 = pSkeleton->tblJoint[2].pos;
				vect3	p3 = pSkeleton->tblJoint[3].pos;
				vect3	p4 = pSkeleton->tblJoint[4].pos;
				vect3	p5 = pSkeleton->tblJoint[5].pos;
				// 箱 胸
				if(1)
				{
					vect3 nx,ny,nz;
					ny = (p0-p3).normalize();
					nx = (p1-p2).normalize();
					nz = cross(ny,nx).normalize();
					nx = cross(ny,nz).normalize();

					mmune = mat33(
						nx.x,	nx.y,	nx.z,
						ny.x,	ny.y,	ny.z,
						nz.x,	nz.y,	nz.z
					);

//					mmune = pSkeleton->tblJoint[0].mat;
					box.DrawBox( (*this), pers, gra, p0, mmune, false, false );
				}
				// 箱 肩
				if(1)
				{
					vect3 nx,ny,nz;
					ny = (p2-p4).normalize();
					nx = (p2-p0).normalize();
					nz = cross(nx,ny).normalize();
					nx = cross(ny,nz).normalize();
					nz = cross(nx,ny).normalize();

					mat33	m(
						nx.x,	nx.y,	nx.z,
						ny.x,	ny.y,	ny.z,
						nz.x,	nz.y,	nz.z
					);

/*
					mat33 mr = GetRotateAxis( (p4-p2).normalize() );

					mkata *= mr;
					if ( keys.CTRL.on )		mkata *= mrotx(rad(0.5));
					if ( keys.SHIFT.on )	mkata *= mrotx(rad(-0.5));
					mkata *= mr.invers();
*/
					if ( keys.CTRL.on )		mkata.rotateByAxis( (p4-p2).normalize(), rad(0.5));
					if ( keys.SHIFT.on )	mkata.rotateByAxis( (p4-p2).normalize(), rad(-0.5));
					
					box.DrawBox( (*this), pers, gra, p2, mkata );
				}
				// 箱 肘
				{
					vect3 nx,ny,nz;
					ny = (p2-p4).normalize();
					nx = cross((p2-p4).normalize(),(p4-p5).normalize()).normalize();
					nz = cross(nx,ny).normalize();

					mat33	m(
						nx.x,	nx.y,	nx.z,
						ny.x,	ny.y,	ny.z,
						nz.x,	nz.y,	nz.z
					);	
					mhiji = m;
//					mhiji = pSkeleton->tblJoint[4].mat;
					drum.DrawDrum( (*this), pers, gra, p4, mhiji );
				}
				// 箱 手
				if(0){
					vect3 nx,ny
					,nz;
					ny = (p4-p5).normalize();
					nx = (p2-p0).normalize();
					nz = cross(nx,ny).normalize();
					nx = cross(ny,nz).normalize();
					nz = cross(nx,ny).normalize();

					mat33	m(
						nx.x,	nx.y,	nx.z,
						ny.x,	ny.y,	ny.z,
						nz.x,	nz.y,	nz.z
					);	
					mte = m;
					drum.DrawDrum( (*this), pers, gra, p5, mte );
				}
			}

			// 選択リスト表示
			selector.DrawJoint( pers, gra, (*pSkeleton) , mouse.gpos );

			// マニュピレーター描画
			{
				axis.DrawAxis( mouse.gpos, *this );

			}
			

/*
			for ( float a = 0 ; a < 4 ; a += 0.1 )
			{
				gra.SetZTest( false );
					gra.Print(1,text_y++,to_string(a));

				rgb col = rgb(0,1,0); 
				float w = 1.0;
				
				if ( a > 1-0.1/2 &&  a < 1+0.1/2 ) {col = rgb(1,1,1); w=3;}
				if ( a > 2-0.1/2 &&  a < 2+0.1/2 ) {col = rgb(1,1,1); w=3;}
				if ( a > 3-0.1/2 &&  a < 3+0.1/2 ) {col = rgb(1,1,1); w=3;}
				if ( a > 4-0.1/2 &&  a < 4+0.1/2 ) {col = rgb(1,1,1); w=3;}
				if ( a > 5-0.1/2 &&  a < 5+0.1/2 ) {col = rgb(1,1,1); w=3;}
				gra.Pset( vect2(a, pow(a,a))*0.1*0.5+vect2(0,-0.5) , col, w);
				gra.SetZTest( true );
			}
*/

			// animカーソルビュー cursor
			{
				gra.SetZTest( false );

				bool flg = false;
				for ( int y = 0 ; y < (signed)pSkeleton->animations.size() ; y++ )
				{
					for ( int x = 0 ; x < (signed)pSkeleton->animations[y].pose.size() ; x++ )
					{
						if ( pSkeleton->cur.bSelecting && ( y == pSkeleton->cur.selecting_act && x == pSkeleton->cur.selecting_pose ) ) flg=!flg;
						if ( pSkeleton->cur.bSelecting && ( y == pSkeleton->cur.act && x == pSkeleton->cur.pose ) ) flg=!flg;

						vect2 v = vect2( x, y )*vect2( 4, 8 ) + vect2(400,16);
						{
							gra.Fill( gra.Conv(v), gra.Conv(v+vect2(3,7)), rgb(1,1,1) );
						}

						if ( y == pSkeleton->cur.act && x == pSkeleton->cur.pose )
						{
							gra.Fill( gra.Conv(v+vect2(0,4)), gra.Conv(v+vect2(3,7)), rgb(1,0,0) );
						}
						
						if ( flg )
						{
							gra.Fill( gra.Conv(v+vect2(0,4)), gra.Conv(v+vect2(3,7)), rgb(1,0,0) );
						}
					}
				}

				gra.SetZTest( true );
			}


				
			#if 0
				{	//砲台
					v.normalize();
					float rx = acos(dot(vect3(0,1,0),v));
					float ry = atan2(v.x,v.z);
					
					mat44	mx;
					mat44	my;
					my.setRotateY(-ry);
					mx.setRotateX(-rx);
					mat44 m = mx * my;

					pSkeleton->ring.ring_DrawMat( gra, pers, vect3(  0,0,0), m );
				}
			#endif

			{
				static bool flgInfo = true;
				if( keys.F2.hi ) flgInfo = !flgInfo;
				if ( flgInfo )
				{
					gra.Print(1,text_y++,string("[ ")+pSkeleton->filename+" ]");
					gra.Print(1,text_y++,string("fovY:")+to_string(int(pers.fovy)));
					gra.Print(1,text_y++,string("sz:")+to_string(pers.sz) +string(" fy:")+to_string(pers.fy));
					gra.Print(1,text_y++,string("far:")+to_string((pers.cam.pos-pers.cam.at).abs())); 
					gra.Print(1,text_y++,string("at  x=")+to_string(pers.cam.at.x)+string(" y=")+to_string(pers.cam.at.y)+string(" z=")+to_string(pers.cam.at.z) ); 
					gra.Print(1,text_y++,string("pos x=")+to_string(pers.cam.pos.x)+string(" y=")+to_string(pers.cam.pos.y)+string(" z=")+to_string(pers.cam.pos.z) ); 
					gra.Print(1,text_y++,string("anim=")+to_string(pSkeleton->cur.act) + string(" cnt=")+to_string(pSkeleton->animations.size()) ); 
					if ( pSkeleton->animations.size() > 0 ) 
					{
						gra.Print(1,text_y++,string("pose=")+to_string(pSkeleton->cur.pose) + string(" cnt=")+to_string(pSkeleton->animations[pSkeleton->cur.act].pose.size()) ); 
					}
					gra.Print(1,text_y++,string("peak=")+to_string(time_peak/1000)+string("msec") ); 
				}

				if ( keys.F1.on )
				{
					gra.Print(1,text_y++,string("[F1] help"));
					gra.Print(1,text_y++,string("[Y] pers -"));
					gra.Print(1,text_y++,string("[H] pers +"));
					gra.Print(1,text_y++,string("[L] Load"));
					gra.Print(1,text_y++,string("[S] Save"));
					gra.Print(1,text_y++,string("--Keyframe--"));
					gra.Print(1,text_y++,string("[K] Insert"));
					gra.Print(1,text_y++,string("[X] Cut"));
					gra.Print(1,text_y++,string("[C] Copy"));
					gra.Print(1,text_y++,string("[V] Past"));
					gra.Print(1,text_y++,string("[LEFT]  -"));
					gra.Print(1,text_y++,string("[RIGHT] +"));
					gra.Print(1,text_y++,string("--Animation--"));
					gra.Print(1,text_y++,string("[I] Add"));
					gra.Print(1,text_y++,string("[P] Play"));
					gra.Print(1,text_y++,string("[UP] -"));
					gra.Print(1,text_y++,string("[DOWN] +"));
					gra.Print(1,text_y++,string("--Other--"));
					gra.Print(1,text_y++,string("[1] select 3d"));
					gra.Print(1,text_y++,string("[2] select 2main"));
				}
				else
				{
					//gra.Print(vect2(10,16*y++),string("[F1] Help"));
				}
			}

			
			// 処理時間表示
			{
				static chrono::system_clock::duration dime_a;
				static chrono::system_clock::duration dime_b;
				static chrono::system_clock::duration dime_sec;
				static chrono::system_clock::duration dime_now;
				static chrono::system_clock::duration dime_max;

				dime_b = chrono::system_clock::now().time_since_epoch(); 
				if ( dime_max < dime_b-dime_a ) dime_max = dime_b-dime_a;

				// ウェイト(60fps)
				while( chrono::system_clock::now().time_since_epoch()-dime_a < chrono::microseconds(16666) )
				{
	 				this_thread::sleep_for(chrono::microseconds(100));
				}

				// 表示
			    dime_now = chrono::system_clock::now().time_since_epoch();
				if ( dime_now-dime_sec > chrono::seconds(2) ) // n sec毎表示
				{
					dime_sec = chrono::system_clock::now().time_since_epoch();

					long long f2 = chrono::duration_cast<chrono::microseconds>(dime_max).count();
					time_peak = f2;
					dime_max = chrono::seconds(0);
				}
				dime_a = chrono::system_clock::now().time_since_epoch();  
			}

		}
		return 0;
	}


} ;


//------------------------------------------------------------------------------
int main()
//------------------------------------------------------------------------------
{
cout << "start" << endl;
	Apr	apr("Ray4 " __DATE__, 300,300,768, 512 );
	return apr.main();
}

