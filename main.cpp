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

#include "raytrace.h"




struct Apr : public Sys
{
	double	time_peak = 0;
	vector<vector<vect3>> human_keyframe;
	int human_numKeyframe = 0;

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

	struct Marker3
	{
		const SysGra&	gra;
		const Figure&	fig;
		vect3&	pos;
		vect2&	disp;
		bool 	bRectSelected;		//	矩形選択中、選択＆非選択
		bool 	bRectIn;			//	矩形選択中、矩形選択対象
		bool 	bSelected;			//	選択
		bool 	bAffectable;		//	削除対象
		double	th;
		int		colNormal = rgb(1,1,0);
		int		colSelected = rgb(1,0,0);

		Marker3( SysGra& _gra, Figure& _fig, vect3& _pos, vect2& _disp, double _th ) : gra(_gra), fig(_fig), pos(_pos), disp(_disp)
		{
			bSelected		= false;
			bRectIn			= false;
			bRectSelected	= false;
			bAffectable		= false;
			th				= _th;
		}
		Marker3(const Marker3& a) : gra(a.gra), fig(a.fig), pos(a.pos), disp(a.disp)
		{
			bSelected		= a.bSelected;
			bRectIn			= a.bRectIn;
			bRectSelected	= a.bRectSelected;
			bAffectable 	= a.bAffectable;
			th 				= a.th;
		}	
		const Marker3&	operator=(Marker3&& a){return a;}	

		//------------------------------------------------------------------------------
		void draw()
		//------------------------------------------------------------------------------
		{
			bool flg =  bSelected;
			
			if ( bRectIn )
			{
				flg = bRectSelected;
			}
			
			if ( flg )			
			{
				fig.draw( disp,th, colSelected );
			}
			else
			{
				fig.draw( disp,th, colNormal );
			}
		
		}
	};


	
	~Apr()
	{
	}

	struct
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
						//bDrag = true;
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

//	} mc;
//	struct
//	{
		vector<Marker3>	tblMarker3;
		//------------------------------------------------------------------------------
		void funcMarkerDraw3()
		//------------------------------------------------------------------------------
		{
			for ( Marker3 m : tblMarker3 )
			{
				m.draw();
			}
		}
		//------------------------------------------------------------------------------
		void funcMarkerController3( Figure& fig, SysMouse& mouse, SysKeys& keys, SysGra& gra )
		//------------------------------------------------------------------------------
		{
			static vect2 drag_start(0,0);
			static bool bDrag = false;

			if  ( keys.ALT.on ) return;

			// マーカー追加
			if ( keys.M.hi )
			{
			//	tblMarker3.emplace_back( gra, fig, mouse.pos, rad(0) );
			}


			// マーカー削除
			if  ( keys.CTRL.on && keys.X.hi )
			{
				for ( Marker3& m : tblMarker3 )
				{
					if ( m.bSelected )
					{
						m.bAffectable = true;
					}
				}

				for ( int i = tblMarker3.size()-1 ; i >= 0 ; i-- )
				{
					if ( tblMarker3[i].bAffectable )
					{
							   tblMarker3.erase(tblMarker3.begin() +i);	
					}
				}
			}
			
			// マーカー選択
			if ( mouse.L.on )
			{
				struct
				{
					double	len;
					Marker3*	pmark;
					int		cnt;
				} a = {99999,0,0};

				// 最近マーカーを検索
				for ( Marker3& m : tblMarker3 )
				{
					double len = (m.disp-mouse.pos).length();
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
						for ( Marker3& m : tblMarker3 )
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

						for ( Marker3& m : tblMarker3 )
						{
							m.bRectIn = false;
						}

						// 矩形内マーカーを検索
						for ( Marker3& m : tblMarker3 )
						{
							double len = (m.disp-mouse.pos).length();
							if ( m.disp.x > v0.x && m.disp.x < v1.x && m.disp.y > v0.y && m.disp.y < v1.y )
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
					for ( Marker3& m : tblMarker3 )
					{
						if ( m.bSelected )
						{
//							if ( keys.Z.on ) m.pos.x += mouse.mov.x/100;
///							if ( keys.X.on ) m.pos.x += mouse.mov.x/100;
//							if ( keys.C.on ) m.pos.x += mouse.mov.y/100;
						}
					}
				}
			}
			else
			{
				if ( bDrag )
				{
					bDrag = false;
					for ( Marker3& m : tblMarker3 )
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
	int main()
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
			const double R=30;
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
			#define X 700
			#define Y 50
			vect2(X-+ 0,Y +0),
			vect2(X+50,Y+ 60),
			vect2(X+ 0,Y+120),
			vect2(X+50,Y+180),
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
			#define X 550
			#define Y 400
				vect2(X+ 00,Y+90),
				vect2(X+ 00,Y+20),
				vect2(X+100,Y+90),
				vect2(X+100,Y+60),
				vect2(X+100,Y+20),
				vect2(X+200,Y+60),
				vect2(X+200,Y+90),
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
			tblJoint.emplace_back( vect2( ox-20,oy+60) );
			tblJoint.emplace_back( vect2( ox+20,oy+60) );
			tblJoint.emplace_back( vect2( ox+0 ,oy+120) );

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
			vect2 mark_disp;
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
			human_tblJoint.emplace_back( os*vect3( ox+ 0,	oy+ 40,	0 )	);//5
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
			mc.tblMarker3.emplace_back( gra, figCircle, j.pos, j.mark_disp, rad(-90) );

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

			vect3 pos = {4.5,-0.51,8.5};

			vector<vect3> vert=
			{
				{	-0.5,	 0.5,	-0.5	},
				{	 0.5,	 0.5,	-0.5	},
				{	-0.5,	-0.5,	-0.5	},
				{	 0.5,	-0.5,	-0.5	},
				{	-0.5,	 0.5,	 0.5	},
				{	 0.5,	 0.5,	 0.5	},
				{	-0.5,	-0.5,	 0.5	},
				{	 0.5,	-0.5,	 0.5	},
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
			vect3	pos = vect3( 0,-1, -2);
			vect3 	at = vect3( 0, -1, 0 );
			vect3	up = vect3( 0, 1, 0);
		  	mat44	mat;		
		} cam ;
		
		struct Pers
		{
			double	fovy;		// 画角
			double	sz;			// 投影面までの距離
			double	ox;			// 描画画面の中心W
			double	oy;			// 描画画面の中心H
			double	width;		// 描画画面の解像度W/2
			double	height;		// 描画画面の解像度H/2
			double	aspect;		// 描画画面のアスペクト比
		
			//--------------------------------------------------------------------------
			Pers()
			//--------------------------------------------------------------------------
			{
				fovy=90/2;
				sz = 1/tan(rad(fovy)/2);				// 投影面までの距離
			}
		
			//--------------------------------------------------------------------------
			void Update( vect2 screensize )
			//--------------------------------------------------------------------------
			{
				sz = 1/tan(rad(fovy)/2);				// 投影面までの距離
				ox		= screensize.x/2;				// 描画画面の中心W
				oy		= screensize.y/2;				// 描画画面の中心H
				width	= screensize.x/2;				// 描画画面の解像度W/2
				height	= screensize.y/2;				// 描画画面の解像度H/2
				aspect	= screensize.y/screensize.x;	// 描画画面のアスペクト比
			} 

			//--------------------------------------------------------------------------
			bool ScissorLine( vect3 v0, vect3 v1, vect3& va, vect3& vb ) const
			//--------------------------------------------------------------------------
			{
				va = calcPoint(v0);
				vb = calcPoint(v1);

				{//シザリング ニアクリップ
					function<vect3(vect3,vect3,int)>nearclip = [ this,&nearclip ]( vect3 a, vect3 b, int n )
					{
						if (n <=0 ) return b;

						vect3 c =  (a+b)/2;
				
						if ( c.z <= -sz )
						{
							c = nearclip( a, c, n-1 );
						}
						if ( c.z > 1.0-sz )
						{
							c = nearclip( c, b, n-1 );
						}
						return c;
					};
					if ( va.z > 0|| vb.z > 0 )
					{
						if ( va.z < 0 )
						{
							vect3 c = nearclip(v1,v0,8);
							va = calcPoint(c);
						}
						if ( vb.z < 0 )
						{
							vect3 c = nearclip(v0,v1,8);
							vb = calcPoint(c);
						}
					}
				}
				return ( va.z > 0 && vb.z > 0 );
			}			

			//--------------------------------------------------------------------------
			vect3 calcPoint( vect3 v ) const
			//--------------------------------------------------------------------------
			{
				vect3 ret;
				double w = 1/(v.z+sz);
				ret.x = v.x*w	*sz* width  *aspect	+ox;
				ret.y = v.y*w	*sz* height			+oy;
				ret.z = w;
				return ret;
			}
	
		};
		Pers pers;


		//===========================================================================
		while( Update() )
		{
			// パースペクティブ
			if (keys.Y.rep) {pers.fovy-=2;cout << pers.fovy <<" "<<1/tan(rad(pers.fovy)) << endl; }
			if (keys.H.rep) {pers.fovy+=2;cout << pers.fovy <<" "<<1/tan(rad(pers.fovy)) << endl; }

			// パース更新
			pers.Update( vect2( m.width, m.height ) );




	 	#if 0
			// レイトレ
	 		static int py=0;
			raytrace( gra, py++ );
			if ( py >= m.height ) py=0;
		#else

			// 画面クリア
			gra.Clr(rgb(0.3,0.3,0.3));
		#endif

			gra.Print(vect2(10,16*1),string("Y/H fovY:")+to_string(int(pers.fovy)));
			// マウスホイールZOOM
			{
				double l = (cam.pos-cam.at).length()/10;
					l=max(l,0.00001);
					l=min(l,8);

				double step = -mouse.wheel/25;

				vect3	v= vect3(0,0,step*l);
				mat44 mrot = cam.mat;
				mrot.SetTranslate(vect3(0,0,0));
				v = v* mrot;

				vect3 r = cam.pos;
				cam.pos += v;
//				if( (cam.pos-cam.at).length() < v.length() ) cam.pos = r;
				if( (cam.pos-cam.at).length() <= v.length() ) cam.pos = (r-cam.at).normalize()*0.00001+cam.at;

				gra.Print( vect2(10,16*2),string("far:")+to_string((cam.pos-cam.at).length())); 
				gra.Print( vect2(10,16*3),string("at x=")+to_string(cam.at.x)+string("y=")+to_string(cam.at.y)+string(" z=")+to_string(cam.at.z) ); 
			}

			gra.Print( vect2(10,16*4),string("key=")+to_string(human_numKeyframe) + string(" cnt=")+to_string(human_keyframe.size()) ); 
			gra.Print( vect2(10,16*29),string("peak=")+to_string(time_peak/1000)+string("msec") ); 


			if ( !keys.ALT.on )
			{

				if ( mouse.R.on )
				{
					double l = (cam.pos-cam.at).length()/10;
					l=max(l,0.00001);
					l=min(l,8);
//					if ( l < 0.05 ) l = 0.05;
//					if ( l > 4 ) l = 4.0;

					// 回転
					vect3	v= vect3(-mouse.mov.x/28,-mouse.mov.y/28,0) * l;
					mat44 mrot = cam.mat;
					mrot.SetTranslate(vect3(0,0,0));
					v = v* mrot;

					cam.pos += v;
				}
			}


			// カメラ移動

			if ( keys.ALT.on )
			{

				if ( mouse.L.on )
				{
					double l = (cam.pos-cam.at).length()/10;
					l=max(l,0.00001);
					l=min(l,8);
//					if ( l < 0.05 ) l = 0.05;
//					if ( l > 4 ) l = 4.0;

					// 回転
					vect3	v= vect3(-mouse.mov.x/28,-mouse.mov.y/28,0) * l;
					mat44 mrot = cam.mat;
					mrot.SetTranslate(vect3(0,0,0));
					v = v* mrot;

					cam.pos += v;
				}
				if ( mouse.R.on )
				{
					// ズーム
					vect3	v= vect3(0,0,mouse.mov.y/100);
					mat44 mrot = cam.mat;
					mrot.SetTranslate(vect3(0,0,0));
					v = v* mrot;

					vect3 r = cam.pos;
					cam.pos += v;
					if( (cam.pos-cam.at).length() <= v.length() ) cam.pos = (r-cam.at).normalize()*0.00001+cam.at;
				}
				if ( mouse.M.on )
				{
					double l = (cam.pos-cam.at).length()/10;
					if ( l < 0.4 ) l = 0.4;
					if ( l > 4 ) l = 4.0;

					// 平行移動
					vect3	v= vect3(-mouse.mov.x/80,-mouse.mov.y/80,0)*l;
					mat44 mrot = cam.mat;
					mrot.SetTranslate(vect3(0,0,0));
					v = v* mrot;

					cam.at += v;
					cam.pos += v;
				}
			}
			// カメラマトリクス計算
			{
				cam.mat.LookAt( cam.pos, cam.at, cam.up );
			}

			// カメラ注視点表示
			{
				vect3 v = pers.calcPoint(cam.at*cam.mat.invers());
				if ( v.z > 0 ) 
				{
					//gra.Circle( vect2(v.x,v.y), 8, rgb(0,1,0));
				}
			}

			// グリッドgrid
			{
				int col = rgb(0.5,0.5,0.5);
				if(0)
				{	// ドットグリッド
					const int NUM = 4;
					{
						for ( int i = -NUM ; i <= NUM ; i++ )
						{
							for ( int j = -NUM ; j <= NUM ; j++ )
							{
								vect3 v0 = pers.calcPoint( vect3(i,0,j)  * cam.mat.invers() );
								if ( v0.z > 0 ) 
								{
									double r = 5* v0.z;
									if ( r < 1.0 )
									{
										gra.Pset( vect2(v0.x,v0.y), col);
									}
									else
									{
										gra.Circle( vect2(v0.x,v0.y), r,col);
									}
								}
							}
						}
					}
				}

				if(1)
				{	// 格子グリッド
					const int NUM = 20;
					{
						vect3 a(-NUM, 0,-NUM);
						vect3 b( NUM, 0,-NUM);
						for ( int i = 0 ; i < NUM*2+1 ; i++ )
						{
							vect3 v0;
							vect3 v1;
							bool flg = pers.ScissorLine( a* cam.mat.invers(), b* cam.mat.invers(), v0, v1 );

							if ( flg )
							{
								gra.Line( vect2(v0.x,v0.y), vect2(v1.x,v1.y), col );
							}
							

							a.z+=1.0;
							b.z+=1.0;
						}
					}			
					{
#if 1
						vect3 a(-NUM, 0, NUM);
						vect3 b(-NUM, 0,-NUM);
						for ( int i = 0 ; i < NUM*2+1 ; i++ )
#else
						vect3 a(0, 0, NUM);
						vect3 b(0, 0,-NUM);
						for ( int i = 0 ; i < 1 ; i++ )
#endif
						{
							vect3 v0;
							vect3 v1;
							bool flg = pers.ScissorLine( a* cam.mat.invers(), b* cam.mat.invers(), v0, v1 );
							if ( flg )
							{
								gra.Line( vect2(v0.x,v0.y), vect2(v1.x,v1.y), col);
							}
							a.x+=1.0;
							b.x+=1.0;
						}
					}			
				}
			}
			
			// 箱
			//calcPoint rotate
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

				v = v * cam.mat.invers();

				box.disp.emplace_back( v );

			}
			
			
			// 箱
			for ( ivect2 e : box.edge )
			{
				const vect3& a = box.disp[e.p];
				const vect3& b = box.disp[e.n];


				vect3 v0;
				vect3 v1;
				bool flg = pers.ScissorLine( a, b, v0, v1 );
				if ( flg )
				{
					gra.Line( vect2(v0.x,v0.y), vect2(v1.x,v1.y), rgb(0,1,1));
				}

			}


			
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

			static vect2 gv1;
			if(1)
			{//ベジェアニメーション
		
				static	double t = 0;
				static	bool	dir = true;

				static int n = 0;
				gv1 = bezier_func( t, bezier_tbl[n+0], bezier_tbl[n+1], bezier_tbl[n+2], bezier_tbl[n+3] );

				gra.Circle( gv1, 6,rgb(1,0,0));

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

			static vect2 gv2;
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

				gv2 = catmull_func( t, catmull_tbl[n0], catmull_tbl[n1], catmull_tbl[n2], catmull_tbl[n3] );

				gra.Circle( gv2, 6,rgb(1,0,0));

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
tar.pos = gv1;

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

			// マーカー表示
			mc.funcMarkerDraw2();


			// 3Dマーカー入力
			mc.funcMarkerController3( figCircle, mouse, keys, gra );

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

			// Human pers
			for ( Joint3& j : human_tblJoint )
			{
				//	右手系座標系
				//	右手ねじ周り
				//	roll	:z	奥+
				//	pitch	:x	右+
				//	yaw		:y	下+
#if 0
				mat44	rotx;
				mat44	roty;
				mat44	rotz;
				mat44	trans;
				rotx.setRotateX(human.rot.x);
				roty.setRotateY(human.rot.y);
				rotz.setRotateZ(human.rot.z);
				trans.SetTranslate(human.pos);
				vect3 v= rotx * roty * rotz *  j.pos + human.pos;
#else
				vect3 v= j.pos;
#endif

//				v += -cam.pos;

				v = v * cam.mat.invers();

				j.world = v;

				j.disp = pers.calcPoint(v);
				j.mark_disp = vect2(j.disp.x,j.disp.y);
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
//					gra.Fill( v-3,v+3, rgb(1,1,1));
				}

			}



			// マーカー表示
//			mc.funcMarkerDraw3();
			{
				int		cntAve=0;
				vect3	posAve=0;
				for ( Marker3 m : mc.tblMarker3 )
				{
						//m.draw();
					bool flg =  m.bSelected;
					
					if ( m.bRectIn )
					{
						flg = m.bRectSelected;
					}
					int col=m.colNormal;
					if ( flg )			
					{
						col = m.colSelected;
						cntAve++;
						posAve += m.pos;
					}
//					gra.Fill( m.disp-3,m.disp+3, col );
					gra.Circle( m.disp, 7, col );
				}

						posAve /= cntAve;;

				if ( cntAve )
				{
					// マーカー移動
					if ( !keys.ALT.on && mouse.L.on )
					{
						for ( Marker3& m : mc.tblMarker3 )
						{
							if ( m.bSelected )
							{
								// 平行移動
								vect3 v = vect3(mouse.mov.x, mouse.mov.y, 0)/80;
								mat44 mrot = cam.mat;
								mrot.SetTranslate(vect3(0,0,0));
								mrot.invers();
								v = v* mrot;
							//	double	l= (mouse.mov.x + mouse.mov.y)/80;
							
	//							if ( keys.Z.on ) m.pos.x += l;
	//							if ( keys.X.on ) m.pos.y += l;
	//							if ( keys.C.on ) m.pos.z -= l;
								m.pos += v ;
							}
						}
					}

/*

					{//x
						vect3 a = posAve - vect3(0,0, 0.0);
						vect3 b = posAve - vect3(0.1,0, 0);
						vect3 v0;
						vect3 v1;
						bool flg = pers.ScissorLine( a* cam.mat.invers(), b* cam.mat.invers(), v0, v1 );
						if ( flg )
						{
							gra.Line( vect2(v0.x,v0.y), vect2(v1.x,v1.y), rgb(1,1,0) );
						}
					}
					{//y
						vect3 a = posAve - vect3(0,0, 0.0);
						vect3 b = posAve - vect3(0,0.1,0);
						vect3 v0;
						vect3 v1;
						bool flg = pers.ScissorLine( a* cam.mat.invers(), b* cam.mat.invers(), v0, v1 );
						if ( flg )
						{
							gra.Line( vect2(v0.x,v0.y), vect2(v1.x,v1.y), rgb(1,1,0) );
						}
					}
					{//z
						vect3 a = posAve - vect3(0,0, 0.0);
						vect3 b = posAve - vect3(0,0, 0.1);
						vect3 v0;
						vect3 v1;
						bool flg = pers.ScissorLine( a* cam.mat.invers(), b* cam.mat.invers(), v0, v1 );
						if ( flg )
						{
							gra.Line( vect2(v0.x,v0.y), vect2(v1.x,v1.y), rgb(1,1,0) );
						}
					}
*/
				}
				
			}


#if 1
			{
				// キーフレーム記録
				if ( keys.K.hi )
				{
					human_numKeyframe = human_keyframe.size();
					human_keyframe.emplace_back();
					for ( const Joint3& j : human_tblJoint )
					{
						human_keyframe[ human_numKeyframe ].emplace_back( j.pos );
					}
				}
				// キーフレーム移動
				bool bChanged = false;
				if ( keys.UP.rep ) 		{ human_numKeyframe++; bChanged=true; }
				if ( keys.DOWN.rep ) 	{ human_numKeyframe--; bChanged=true; }
				human_numKeyframe = max( human_numKeyframe, 0 );
//				human_numKeyframe = min( human_numKeyframe, ((signed)human_keyframe.size())-1 );
				human_numKeyframe = min( human_numKeyframe, (signed)human_keyframe.size()-1 );


				if ( bChanged )
				{
					// キーフレーム切り替え
					int i = 0;
					for ( Joint3& j : human_tblJoint )
					{
						j.pos = human_keyframe[ human_numKeyframe ][i];
						i++;
					}
				}
			}
#endif


			// 点 
			{
				gra.Pset(vect2(1,1),rgb(1,1,1));
				gra.Pset(vect2(766,1),rgb(1,1,1));
				gra.Pset(vect2(1,510),rgb(1,1,1));
				gra.Pset(vect2(766,510),rgb(1,1,1));
			}
			
			// 塗りつぶし三角
			{
				vect2	ofs = vect2(10,432);
				double	scale=0.5;
				vect2	v0 = scale * vect2( 55,0) + ofs;
				vect2	v1 = scale * vect2( 10,90) + ofs;
				vect2	v2 = scale * vect2(100,90) + ofs;
				gra.Tri( v0,v1,v2,rgb(0.5,0.3,0.2));
			}
			// figTriangle
			{
				int ox=56,oy=464;
				int sx =ox-128;
				int sy =oy-128;
				int ex =ox+128;
				int ey =oy+128;

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

					double f2 = chrono::duration_cast<chrono::microseconds>(dime_max).count();
//					cout << "time-max " << f2/1000 << " msec" << endl;
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
	Apr	apr("Ray4 " __DATE__, 300,300,768, 512 );
	return apr.main();
}




	

