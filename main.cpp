// 2017/07/07 ray3
// 2019/06/25 ray4

#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <cmath>
#include <functional>
#include <fstream>
#include <sstream>
using namespace std;

#include "geom.h"


#include "SysKeys.h"
#include "SysMouse.h"
#include "SysGra.h"
#include "Sys.h"

#include "raytrace.h"
#include "obj.h"
#include "bone.h"



struct Apr : public Sys
{

	long long	time_peak = 0;




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

	struct Marker
	{
		SysGra&	gra;
		Obj&	obj;
//		int		colNormal = rgb(1,1,0);
//		int		colSelected = rgb(1,0,0);

		Marker( SysGra& _gra, Obj& _obj ) : gra(_gra), obj(_obj)
		{
			obj.bSelected		= false;
			obj.bRectIn			= false;
			obj.bRectSelected	= false;
			obj.bAffectable		= false;
		}
		Marker(const Marker& a) : gra(a.gra), obj(a.obj)
		{
			obj.bSelected		= a.obj.bSelected;
			obj.bRectIn			= a.obj.bRectIn;
			obj.bRectSelected	= a.obj.bRectSelected;
			obj.bAffectable 	= a.obj.bAffectable;
		}	
		const Marker&	operator=(Marker&& a){return a;}	
/*
		void draw()
		{
			bool flg =  obj.bSelected;
			
			if ( obj.bRectIn )
			{
				flg = obj.bRectSelected;
			}
			
			if ( flg )			
			{
				gra.Circle( obj.Pos2(), 7, colSelected );
			}
			else
			{
				gra.Circle( obj.Pos2(), 7, colNormal );
			}
		
		}
*/

	};


	~Apr()
	{
	}

	struct Joint2 : Obj
	{
		vect2 pos;
		vect2 tension;
		double len;
		Joint2( vect2 v )
		{
			pos = v;
			tension = 0;
			len = 0;
		}
		void Move2( vect2 v )
		{
			pos += v;
		}
		vect2 Pos2()
		{
			return pos;
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
	struct Catmull : Obj
	{
		vect2	pos;
		Catmull( const vect2& _pos ) : pos(_pos){}

		void Move2( vect2 v )
		{
			pos += v;
		}
		vect2 Pos2()
		{
			return pos;
		}

	};
	struct Bezier : Obj
	{
		vect2	pos;
		Bezier( const vect2& _pos ) : pos(_pos){}
		void Move2( vect2 v )
		{
			pos += v;
		}
		vect2 Pos2()
		{
			return pos;
		}
	};

	struct
	{

/*
		vector<Marker>	marker_tblMarker2;
		//------------------------------------------------------------------------------
		void funcMarkerDraw2()
		//------------------------------------------------------------------------------
		{
			for ( Marker& m : marker_tblMarker2 )
			{
///				m.draw();
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
//				marker_tblMarker2.emplace_back( gra, fig, mouse.pos, rad(0) );
			}


			// マーカー削除
			if  ( keys.CTRL.on && keys.X.hi )
			{
				for ( Marker& m : marker_tblMarker2 )
				{
					if ( m.obj.bSelected )
					{
						m.obj.bAffectable = true;
					}
				}

				for ( int i = static_cast<signed>(marker_tblMarker2.size())-1 ; i >= 0 ; i-- )
				{
					if ( marker_tblMarker2[i].obj.bAffectable )
					{
							   marker_tblMarker2.erase(marker_tblMarker2.begin() +i);	
					}
				}
			}
			
			// マーカー選択
			if ( mouse.L.on )
			{
				struct
				{
					double	len;
					Marker*	pm;
					int		cnt;
				} a = {99999,0,0};

				// 最近マーカーを検索
				for ( Marker& m : marker_tblMarker2 )
				{
					double len = (m.obj.Pos2()-mouse.pos).length();
					if ( len < 20.0 && a.len > len )
					{
						a.len = len;
						a.pm = &m;
						a.cnt++;
					}
				}

				// マーカー選択＆解除
				if ( mouse.L.hi )
				{
					// 矩形選択
					if ( a.pm == 0 ) 
					{
						//bDrag = true;
						drag_start = mouse.pos;
					}

					// マーカー全解除
					if ( keys.CTRL.on ){}
					else
					if ( keys.SHIFT.on ){}
					else
					if ( a.pm && a.pm->obj.bSelected == true ){}
					else
					{
						for ( Marker& m : marker_tblMarker2 )
						{
							m.obj.bSelected = false;
						}
					}
					
					//	マーカー選択
					if ( a.pm )
					{
						if ( keys.CTRL.on )
						{
							a.pm->obj.bSelected = !a.pm->obj.bSelected;
						}
						else
						{
							a.pm->obj.bSelected = true;
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

						for ( Marker& m : marker_tblMarker2 )
						{
							m.obj.bRectIn = false;
						}

						// 矩形内マーカーを検索
						for ( Marker& m : marker_tblMarker2 )
						{
							double len = (m.obj.Pos2()-mouse.pos).length();
							if ( m.obj.Pos2().x > v0.x && m.obj.Pos2().x < v1.x && m.obj.Pos2().y > v0.y && m.obj.Pos2().y < v1.y )
							{
								m.obj.bRectIn = true;
								if ( keys.CTRL.on )
								{
									m.obj.bRectSelected = !m.obj.bSelected;
								}
								else
								{
									m.obj.bRectSelected = true;
								}
							}
						}

					}
					else
					// マーカー移動
					for ( Marker& m : marker_tblMarker2 )
					{
						if ( m.obj.bSelected )
						{
							m.obj.Move2( mouse.mov );
						}
					}
				}
			}
			else
			{
				if ( bDrag )
				{
					bDrag = false;
					for ( Marker& m : marker_tblMarker2 )
					{
						if ( m.obj.bRectIn )
						{
							m.obj.bSelected = m.obj.bRectSelected;
						}
						m.obj.bRectIn = false;
						m.obj.bRectSelected = false;
					}
				}
			}

		}
*/

		struct Selector
		{
			vector<Marker>	marker_tblMarker3;
			vect2 marker_rect_pos = vect2(0,0);			//	矩形選択開始位置
			bool marker_rect_bSelect = false;	//	矩形選択中フラグ
			struct
			{
				double	len;
				Marker*	pm;
				int		cnt;
			} marker_a = {99999,0,0};
			int		colNormal = rgb(1,1,0);
			int		colSelected = rgb(1,0,0);

			//---------------------------------------------------------------------
			void marker_clear( vect2 mouse_pos )
			//---------------------------------------------------------------------
			{
				marker_a.len = 9999;
				marker_a.pm = 0;
				marker_a.cnt = 0;

				// 最近マーカーを検索
				for ( Marker& m : marker_tblMarker3 )
				{
					double len = (m.obj.Pos2()-mouse_pos).length();
					if ( len < 20.0 && marker_a.len > len )
					{
						marker_a.len = len;
						marker_a.pm = &m;
						marker_a.cnt++;
					}
				}

			}

			//---------------------------------------------------------------------
			void marker_beginRectcursor( vect2 mouse_pos )
			//---------------------------------------------------------------------
			{
				marker_rect_bSelect = true;
				marker_rect_pos = mouse_pos;
			}

			//---------------------------------------------------------------------
			void marker_allclear()
			//---------------------------------------------------------------------
			{
				for ( Marker& m : marker_tblMarker3 )
				{
					m.obj.bSelected = false;
				}
			}

			//---------------------------------------------------------------------
			void marker_selectReverse()
			//---------------------------------------------------------------------
			{
				marker_a.pm->obj.bSelected = !marker_a.pm->obj.bSelected;
			}

			//---------------------------------------------------------------------
			void marker_selectAdd()
			//---------------------------------------------------------------------
			{
				marker_a.pm->obj.bSelected = true;
			}

			//---------------------------------------------------------------------
			void marker_selectOne()
			//---------------------------------------------------------------------
			{
				for ( Marker& m : marker_tblMarker3 )
				{
					m.obj.bSelected = false;
				}
				marker_a.pm->obj.bSelected = true;
			}

			//---------------------------------------------------------------------
			void marker_rect_selectReverse( vect2 mouse_pos )
			//---------------------------------------------------------------------
			{
				vect2 v0 = min( marker_rect_pos, mouse_pos );
				vect2 v1 = max( marker_rect_pos, mouse_pos );

				// 矩形カーソル選択解除
				for ( Marker& m : marker_tblMarker3 )
				{
					m.obj.bRectIn = false;
				}

				// 矩形カーソル内マーカーを検索
				for ( Marker& m : marker_tblMarker3 )
				{
					double len = (m.obj.Pos2()-mouse_pos).length();
					if ( m.obj.Pos2().x > v0.x && m.obj.Pos2().x < v1.x && m.obj.Pos2().y > v0.y && m.obj.Pos2().y < v1.y )
					{
						m.obj.bRectIn = true;
						m.obj.bRectSelected = !m.obj.bSelected;
					}
				}
			}

			//---------------------------------------------------------------------
			void marker_rect_selectAdd( vect2 mouse_pos )
			//---------------------------------------------------------------------
			{
				vect2 v0 = min( marker_rect_pos, mouse_pos );
				vect2 v1 = max( marker_rect_pos, mouse_pos );

				// 矩形カーソル選択解除
				for ( Marker& m : marker_tblMarker3 )
				{
					m.obj.bRectIn = false;
				}

				// 矩形カーソル内マーカーを検索
				for ( Marker& m : marker_tblMarker3 )
				{
					double len = (m.obj.Pos2()-mouse_pos).length();
					if ( m.obj.Pos2().x > v0.x && m.obj.Pos2().x < v1.x && m.obj.Pos2().y > v0.y && m.obj.Pos2().y < v1.y )
					{
						m.obj.bRectIn = true;
						m.obj.bRectSelected = true;
					}
				}

			}

			//---------------------------------------------------------------------
			void marker_move( vect2 mouse_mov, Pers& pers, mat44& cam_mat, Bone& bone )
			//---------------------------------------------------------------------
			{
				for ( Marker& m : marker_tblMarker3 )
				{
					Joint3* pj = dynamic_cast<Joint3*>(&m.obj);
					if ( pj->bSelected )
					{
						// 平行移動
						vect3 v = vect3(mouse_mov.x, mouse_mov.y, 0)/2.45/pers.height/(pj->disp.z);
						mat44 mrot = cam_mat;
						mrot.SetTranslate(vect3(0,0,0));
						mrot.invers();
						v = v* mrot;
						pj->pos += v ;
					}
				}
				bone.RefrectKeyframe();
			}

			//---------------------------------------------------------------------
			void marker_endRect()
			//---------------------------------------------------------------------
			{
				marker_rect_bSelect = false;
				for ( Marker& m : marker_tblMarker3 )
				{
					if ( m.obj.bRectIn )
					{
						m.obj.bSelected = m.obj.bRectSelected;
					}
					m.obj.bRectIn = false;
					m.obj.bRectSelected = false;
				}
			}
			
			//---------------------------------------------------------------------
			void marker_draw( vect2 mouse_pos, SysGra& gra )
			//---------------------------------------------------------------------
			{

				if ( marker_rect_bSelect )
				{
					vect2 v0 = min( marker_rect_pos, mouse_pos );
					vect2 v1 = max( marker_rect_pos, mouse_pos );
					gra.Box( v0,v1, rgb(0,0.5,1));
				}

				for ( Marker m : marker_tblMarker3 )
				{
					bool flg =  m.obj.bSelected;
					
					if ( m.obj.bRectIn )
					{
						flg = m.obj.bRectSelected;
					}
					
					if ( flg )			
					{
						gra.Circle( m.obj.Pos2(), 7, colSelected );
					}
					else
					{
						gra.Circle( m.obj.Pos2(), 7, colNormal );
					}
				}
			}
		};
		Selector selector3;
		Selector selector2;

	} mc;


	Figure figCircle=Figure(gra);

	// カメラ
	struct
	{
		vect3	pos = vect3( -2,-2, -2+0.1 );
		vect3 	at = vect3( 0, -1, 0 );
		vect3	up = vect3( 0, 1, 0);
	  	mat44	mat;		
	} cam ;


	Pers pers;



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

		// カトマル曲線2D
		auto catmull_func = []( double t, const vect2 P0, const vect2 P1, const vect2 P2, const vect2 P3 )
		{
			//catmull-Rom 曲線
			// P(t)=P1*(2t^3-3t^2+1)+m0*(t^3-2t^2+t)+P2*(-2t^3+3t^2)+m1*(t^3-t^2)
			// m0=(P2-P0)/2
			// m1=(P3-P1)/2

			vect2 m0 = (P2-P0)/2.0;
			vect2 m1 = (P3-P1)/2.0;
			vect2 P =  P1*(  2*t*t*t - 3*t*t +1) + m0*( t*t*t -2*t*t +t )
					 + P2*( -2*t*t*t + 3*t*t   ) + m1*( t*t*t - t*t );

			return P;
		};

		vector<Catmull> catmull_tbl =
		{
			#define X 700
			#define Y 50
			Catmull( vect2(X-+ 0,Y +0) ),
			Catmull( vect2(X+50,Y+ 60) ),
			Catmull( vect2(X+ 0,Y+120) ),
			Catmull( vect2(X+50,Y+180) ),
			#undef X
			#undef Y
		};

		for ( Catmull& c : catmull_tbl )	// マーカー対象に位置を登録
		{
			mc.selector2.marker_tblMarker3.emplace_back( gra, c );

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

		vector<Bezier> bezier_tbl =
		{
			#define X 550
			#define Y 400
				Bezier( vect2(X+ 00,Y+90) ),
				Bezier( vect2(X+ 00,Y+20) ),
				Bezier( vect2(X+100,Y+90) ),
				Bezier( vect2(X+100,Y+60) ),
				Bezier( vect2(X+100,Y+20) ),
				Bezier( vect2(X+200,Y+60) ),
				Bezier( vect2(X+200,Y+90) ),
			#undef X
			#undef Y
		};
		for ( Bezier& b : bezier_tbl )	// マーカー対象に位置を登録
		{
			mc.selector2.marker_tblMarker3.emplace_back( gra, b );
		}

		//骨---------------------------------------
		vector<Joint2> tblJoint_2d;
		tblJoint_2d.reserve(1000);
		vector<Bone2> tblBone_2d;
		if(1)
		{	//	対△
			int cx=200,cy=300;

			tblJoint_2d.emplace_back( vect2( cx,cy) );
			tblJoint_2d.emplace_back( vect2( cx-20,cy+60) );
			tblJoint_2d.emplace_back( vect2( cx+20,cy+60) );
			tblJoint_2d.emplace_back( vect2( cx+0 ,cy+120) );

			tblBone_2d.emplace_back( tblJoint_2d[0], tblJoint_2d[1] );
			tblBone_2d.emplace_back( tblJoint_2d[0], tblJoint_2d[2] );
			tblBone_2d.emplace_back( tblJoint_2d[1], tblJoint_2d[2] );
			tblBone_2d.emplace_back( tblJoint_2d[1], tblJoint_2d[3] );
			tblBone_2d.emplace_back( tblJoint_2d[2], tblJoint_2d[3] );
		}
		if(0)
		{	//	四角格子メッシュ
			int cx=200,cy=300;
			const int  W=40;
			const int  H=80;
			const int  X=2;
			const int  Y=3;
			for ( int x = 0 ; x < X ; x++ )
			{
				for ( int y = 0 ; y < Y ; y++ )
				{
					tblJoint_2d.emplace_back( vect2( x*W+cx,y*H+cy) );
				}
			}
			for ( int y=0 ; y < Y-1 ; y++ )
			{
				for ( int x=0 ; x < X ; x++ )
				{
					tblBone_2d.emplace_back( tblJoint_2d[x*Y+y], tblJoint_2d[x*Y+y+1] );
				}
			}
			for ( int y=0 ; y < Y ; y++ )
			{
				for ( int x=0 ; x < X-1 ; x++ )
				{
					tblBone_2d.emplace_back( tblJoint_2d[x*Y+y], tblJoint_2d[(x+1)*Y+y] );
				}
			}
		}
		if(0)
		{	//	三角形メッシュ
			function<void(int,int,int,int)> func = [&]( int v0, int v1, int v2, int n )
			{ 
				if ( n > 0 )
				{
					int ix = static_cast<signed>(tblJoint_2d.size());
					tblJoint_2d.emplace_back( vect2( (tblJoint_2d[v0].pos + tblJoint_2d[v1].pos )/2 ) );
					tblJoint_2d.emplace_back( vect2( (tblJoint_2d[v1].pos + tblJoint_2d[v2].pos )/2 ) );
					tblJoint_2d.emplace_back( vect2( (tblJoint_2d[v2].pos + tblJoint_2d[v0].pos )/2 ) );
					
					func( v0, ix+0, ix+2, n-1 );
					func( v1, ix+1, ix+0, n-1 );
					func( v2, ix+2, ix+1, n-1 );
				}
				else
				{
					tblBone_2d.emplace_back( tblJoint_2d[v0], tblJoint_2d[v1] );
					tblBone_2d.emplace_back( tblJoint_2d[v1], tblJoint_2d[v2] );
					tblBone_2d.emplace_back( tblJoint_2d[v2], tblJoint_2d[v0] );
				}
			};

			double R=80;
			tblJoint_2d.emplace_back( vect2(200+0, 500-R*tan(rad(60))	-R*tan(rad(30)) ));
			tblJoint_2d.emplace_back( vect2(200-R, 500+  	    	   	-R*tan(rad(30))) );
			tblJoint_2d.emplace_back( vect2(200+R, 500+  				-R*tan(rad(30))) );

			func( 0, 1, 2, 3 );
			cout << tblJoint_2d.size() << endl;
		}
		for ( Bone2& b : tblBone_2d )
		{
			b.length = (b.j1.pos - b.j0.pos).length();
		}
		for ( Joint2& j : tblJoint_2d )	//マーカー対象に位置を登録
		{
			mc.selector2.marker_tblMarker3.emplace_back( gra, j );
		}

		//人

#if 0 
		Bone* pPreset = new Bone;
		{	//	人
			double cx=0,cy=-160,os=0.01;

			pPreset->tblJoint.emplace_back( os*vect3( cx-10,	cy- 20,	0 )	);//0
			pPreset->tblJoint.emplace_back( os*vect3( cx+10,	cy- 20,	0 )	);//1
			pPreset->tblJoint.emplace_back( os*vect3( cx+ 0,	cy+  0,	0 )	);//2
			pPreset->tblJoint.emplace_back( os*vect3( cx-20,	cy+  0,	0 )	);//3
			pPreset->tblJoint.emplace_back( os*vect3( cx+20,	cy+  0,	0 )	);//4
			pPreset->tblJoint.emplace_back( os*vect3( cx+ 0,	cy+ 40,	0 )	);//5
			pPreset->tblJoint.emplace_back( os*vect3( cx-15,	cy+ 70,	0 )	);//6
			pPreset->tblJoint.emplace_back( os*vect3( cx+15,	cy+ 70,	0 )	);//7
			pPreset->tblJoint.emplace_back( os*vect3( cx-15,	cy+115,	0 )	);//8
			pPreset->tblJoint.emplace_back( os*vect3( cx-15,	cy+160,	0 )	);//9
			pPreset->tblJoint.emplace_back( os*vect3( cx+15,	cy+115,	0 )	);//10
			pPreset->tblJoint.emplace_back( os*vect3( cx+15,	cy+160,	0 )	);//11
			pPreset->tblJoint.emplace_back( os*vect3( cx-20,	cy+ 40,	0 )	);//12
			pPreset->tblJoint.emplace_back( os*vect3( cx-20,	cy+ 80,	0 )	);//13
			pPreset->tblJoint.emplace_back( os*vect3( cx+20,	cy+ 40,	0 )	);//14
			pPreset->tblJoint.emplace_back( os*vect3( cx+20,	cy+ 80,	0 )	);//15

		 	pPreset->tblBone.emplace_back( pPreset->tblJoint, 0, 1 );	//head
			pPreset->tblBone.emplace_back( pPreset->tblJoint, 1, 2 );
			pPreset->tblBone.emplace_back( pPreset->tblJoint, 2, 0 );
			pPreset->tblBone.emplace_back( pPreset->tblJoint, 2, 5 ); //chest
			pPreset->tblBone.emplace_back( pPreset->tblJoint, 5, 3 );
			pPreset->tblBone.emplace_back( pPreset->tblJoint, 3, 2 );
			pPreset->tblBone.emplace_back( pPreset->tblJoint, 2, 4 );
			pPreset->tblBone.emplace_back( pPreset->tblJoint, 4, 5 );
			pPreset->tblBone.emplace_back( pPreset->tblJoint, 5, 7 ); //hip
			pPreset->tblBone.emplace_back( pPreset->tblJoint, 7, 6 );
			pPreset->tblBone.emplace_back( pPreset->tblJoint, 6, 5 );
			pPreset->tblBone.emplace_back( pPreset->tblJoint, 6, 8 ); //leg
			pPreset->tblBone.emplace_back( pPreset->tblJoint, 8, 9 );
			pPreset->tblBone.emplace_back( pPreset->tblJoint, 7,10 ); //leg
			pPreset->tblBone.emplace_back( pPreset->tblJoint,10,11 );
			pPreset->tblBone.emplace_back( pPreset->tblJoint, 3,12 ); //arm
			pPreset->tblBone.emplace_back( pPreset->tblJoint,12,13 );
			pPreset->tblBone.emplace_back( pPreset->tblJoint, 4,14 ); //arm
			pPreset->tblBone.emplace_back( pPreset->tblJoint,14,15 );
		}
		for ( Bone3& b : pPreset->tblBone )	// 関節の距離を決定する。
		{
			b.length = (b.j1.pos - b.j0.pos).length();
		}
		{
			int cnt = 0 ;
			for ( Joint3& j : pPreset->tblJoint )
			{
				j.id = cnt++;				//id登録
			}
		}
		for ( Bone3& b : pPreset->tblBone )	// ジョイントに関節の距離を決定する。
		{
			b.j1.relative.emplace_back( b.j0 ); 
			b.j0.relative.emplace_back( b.j1 ); 
		}

		for ( Joint3& j : pPreset->tblJoint )	//マーカー登録
		{
			mc.marker_tblMarker3.emplace_back( gra, j );
		}

		pPreset->animations.emplace_back();

		Bone* pBone = pPreset;

#else
		unique_ptr<Bone> pBone(new Bone);
//		pBone->load( "primary.mot");
#endif

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



	#if 0
			// レイトレ
		while( Update() )
	 	{
			raytrace( gra );
		}
		return 0;
	#endif	


		//===========================================================================
		while( Update() )
		{
			// パースペクティブ
			if (keys.Y.rep) {pers.fovy-=2;cout << pers.fovy <<" "<<1/tan(rad(pers.fovy)) << endl; }
			if (keys.H.rep) {pers.fovy+=2;cout << pers.fovy <<" "<<1/tan(rad(pers.fovy)) << endl; }

			// パース更新
			pers.Update( vect2( m.width, m.height ) );

			// 画面クリア
			gra.Clr(rgb(0.3,0.3,0.3));

			{
				int y = 10;

				if ( keys.F1.on )
				{
					gra.Print(vect2(10,16*y++),string("[Y] pers -"));
					gra.Print(vect2(10,16*y++),string("[H] pers +"));
					gra.Print(vect2(10,16*y++),string("[L] Load"));
					gra.Print(vect2(10,16*y++),string("[S] Save"));
					gra.Print(vect2(10,16*y++),string("--Keyframe--"));
					gra.Print(vect2(10,16*y++),string("[J] Refrect"));
					gra.Print(vect2(10,16*y++),string("[K] Insert"));
					gra.Print(vect2(10,16*y++),string("[D] Delete"));
					gra.Print(vect2(10,16*y++),string("[C] Copy"));
					gra.Print(vect2(10,16*y++),string("[V] Past"));
					gra.Print(vect2(10,16*y++),string("[LEFT]  -"));
					gra.Print(vect2(10,16*y++),string("[RIGHT] +"));
					gra.Print(vect2(10,16*y++),string("--Animation--"));
					gra.Print(vect2(10,16*y++),string("[I] Add"));
					gra.Print(vect2(10,16*y++),string("[UP] -"));
					gra.Print(vect2(10,16*y++),string("[DOWN] +"));
					gra.Print(vect2(10,16*y++),string("[P] Play"));
				}
				else
				{
					gra.Print(vect2(10,16*y++),string("[F1] Help"));
				}


				if ( pBone->cur.bSelecting ==false && keys.SHIFT.on && (keys.UP.hi || keys.DOWN.hi || keys.LEFT.hi ||keys.RIGHT.hi) )
				{
					pBone->cur.bSelecting = true;
					pBone->cur.selecting_act = pBone->cur.act;
					pBone->cur.selecting_pose = pBone->cur.pose;
				}
				if ( !keys.SHIFT.on && (keys.UP.hi || keys.DOWN.hi || keys.LEFT.hi ||keys.RIGHT.hi) )
				{
					pBone->cur.bSelecting = false;
				}
				
				// キーフレームロード
				if ( keys.L.hi ) 
				{
					unique_ptr<Bone> pNew(new Bone);
					pNew->loadMotion( "human.mot");
					//マーカー削除＆登録
					mc.selector3.marker_tblMarker3.clear();
					for ( Joint3& j : pNew->tblJoint )	//マーカー登録
					{
						mc.selector3.marker_tblMarker3.emplace_back( gra, j );
					}
					pBone = move(pNew);

				}


				// キーフレームセーブ
				if ( keys.S.hi ) pBone->saveMotion( "human.mot" );

				// キーフレームへ反映
//				if ( keys.J.hi ) pBone->RefrectKeyframe();

				// キーフレームペースト
				if ( keys.V.hi ) pBone->PastKeyframe();

				// キーフレームコピー
				if ( keys.C.hi ) pBone->CopyKeyframe();

				// キーフレーム追加
				if ( keys.K.hi ) pBone->InsertKeyframe();

				// キーフレームカット
				if ( keys.X.hi ) pBone->CutKeyframe();

				// キーフレーム次
				if ( keys.RIGHT.rep ) pBone->NextKeyframe();

				// キーフレーム最後
				if ( keys.CTRL.on && keys.RIGHT.rep ) pBone->LastKeyframe();

				// キーフレーム前
				if ( keys.LEFT.rep ) pBone->PrevKeyframe();

				// キーフレーム先頭
				if ( keys.CTRL.on && keys.LEFT.rep ) pBone->TopKeyframe();

				// アニメーション記録
				if ( keys.I.hi ) pBone->AddAnimation();

				// アニメーション前
				if ( keys.UP.rep ) pBone->PrevAnimation();

				// アニメーション次
				if ( keys.DOWN.rep ) pBone->NextAnimation();

				// アニメーションリクエスト
				if ( keys.P.hi ) pBone->ReqAnimation();

				// アニメーション再生
				if ( pBone->anim.bPlaying )	pBone->PlayAnimation();
				
				
			}

			{
				int y = 1;
				gra.Print(vect2(10,16*y++),string("fovY:")+to_string(int(pers.fovy)));
				gra.Print(vect2(10,16*y++),string("sz:")+to_string(pers.sz) +string(" sc:")+to_string(pers.sc));
				gra.Print( vect2(10,16*y++),string("far:")+to_string((cam.pos-cam.at).length())); 
				gra.Print( vect2(10,16*y++),string("at  x=")+to_string(cam.at.x)+string(" y=")+to_string(cam.at.y)+string(" z=")+to_string(cam.at.z) ); 
				gra.Print( vect2(10,16*y++),string("pos x=")+to_string(cam.pos.x)+string(" y=")+to_string(cam.pos.y)+string(" z=")+to_string(cam.pos.z) ); 
				{
					gra.Print( vect2(10,16*y++),string("anim=")+to_string(pBone->cur.act) + string(" cnt=")+to_string(pBone->animations.size()) ); 
					if ( pBone->animations.size() > 0 ) 
					{
						gra.Print( vect2(10,16*y++),string("pose=")+to_string(pBone->cur.pose) + string(" cnt=")+to_string(pBone->animations[pBone->cur.act].pose.size()) ); 
					}
				}
				gra.Print( vect2(10,16*31),string("peak=")+to_string(time_peak/1000)+string("msec") ); 
			}

			// animカーソルビュー cursor
			{
				bool flg = false;
				for ( int y = 0 ; y < (signed)pBone->animations.size() ; y++ )
				{
					for ( int x = 0 ; x < (signed)pBone->animations[y].pose.size() ; x++ )
					{
						if ( pBone->cur.bSelecting && ( y == pBone->cur.selecting_act && x == pBone->cur.selecting_pose ) ) flg=!flg;
						if ( pBone->cur.bSelecting && ( y == pBone->cur.act && x == pBone->cur.pose ) ) flg=!flg;

						vect2 v = vect2( x, y )*vect2( 4, 8 ) + vect2(400,16);
						{
							gra.Fill( v, v+vect2(3,7), rgb(1,1,1) );
						}

						if ( y == pBone->cur.act && x == pBone->cur.pose )
						{
							gra.Fill( v+vect2(0,4), v+vect2(3,7), rgb(1,0,0) );
						}
						
						if ( flg )
						{
							gra.Fill( v+vect2(0,4), v+vect2(3,7), rgb(1,0,0) );
						}
					}
				}
			}

			// カメラ回転
			if ( (!keys.ALT.on && mouse.R.on) || (keys.ALT.on && mouse.L.on) )
			{
				double len = (cam.pos-cam.at).length();
				double l = (cam.pos-cam.at).length()/10;
				l=max(l,0.00001);
				l=min(l,8);

				// 回転
				vect3	v= vect3(-mouse.mov.x/28,-mouse.mov.y/28,0) * l;
				mat44 mrot = cam.mat;
				mrot.SetTranslate(vect3(0,0,0));
				v = v* mrot;
				cam.pos += v;
				
				{
					vect3 dir = (cam.pos-cam.at).normalize();
					cam.pos = cam.at+dir*len;
				}

			}

			// カメラ平行移動
			if ( !keys.ALT.on  &&  mouse.M.on )
			{
				double l = (cam.pos-cam.at).length()/10;
				if ( l < 0.4 ) l = 0.4;
				if ( l > 4 ) l = 4.0;

				vect3	v= vect3(-mouse.mov.x/80,-mouse.mov.y/80,0)*l;
				mat44 mrot = cam.mat;
				mrot.SetTranslate(vect3(0,0,0));
				v = v* mrot;

				cam.at += v;
				cam.pos += v;
			}

			// マウスホイールZOOM
			if ( !keys.ALT.on  )
			{
				double l = (cam.pos-cam.at).length()/10;
					l=max(l,0.01);
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
			}

			// カメラ移動
			if ( keys.ALT.on && mouse.R.on )
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
				const int NUM = 10;
				if(0)
				{	// ドットグリッド
					int col = rgb(1,1,1);
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
					int col = rgb(0.5,0.5,0.5);
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
					double dt = 1/div;

					for ( int n = -1 ; n < static_cast<signed>(catmull_tbl.size())-3+1 ; n++ )
					{
						int n0 = n;
						int n1 = n+1;
						int n2 = n+2;
						int n3 = n+3;
						if ( n0 < 0 ) n0 = 0;
						if ( n3 >= static_cast<signed>(catmull_tbl.size()) ) n3 = n2;
					
						double t = dt;
						vect2 v0 = catmull_func(0, catmull_tbl[n0].pos, catmull_tbl[n1].pos, catmull_tbl[n2].pos, catmull_tbl[n3].pos );
						for ( int i = 0 ; i <div ; i++ )
						{
							vect2 v1 = catmull_func(t, catmull_tbl[n0].pos, catmull_tbl[n1].pos, catmull_tbl[n2].pos, catmull_tbl[n3].pos );
							gra.Line( v1, v0, rgb(1,1,1));
							gra.Fill( v1-1,v1+2, rgb(1,1,1));
							v0=v1;
							t+=dt;

						}	
							
					}
				}
			}


			

			// ベジェ 三次曲線
			{
				{//ベジェ計算＆描画
					double div = 20;
					double dt = 1/div;

					for ( int n = 0 ; n < static_cast<signed>(bezier_tbl.size())-3 ; n+=3 )
					{
						double t  = dt;
						vect2 v0 = bezier_tbl[n+0].pos;
						for ( int i = 0 ; i < div ; i++ )
						{
							vect2 v1 = bezier_func( t, bezier_tbl[n+0].pos, bezier_tbl[n+1].pos, bezier_tbl[n+2].pos, bezier_tbl[n+3].pos );
							gra.Line( v0,v1, rgb(1,1,1));
							gra.Fill( v1-1,v1+2, rgb(1,1,1));
							v0=v1;
							t+=dt;
						}
					}

				}

				{// 補助ライン描画
					int cnt = 0;
					vect2 v0 = bezier_tbl[0].pos;
					for ( int i = 1 ; i < static_cast<signed>(bezier_tbl.size()) ; i++ )
					{ 
						vect2 v1 = bezier_tbl[i].pos;
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
				static	bool	bForward = true;

				static int n = 0;
				gv1 = bezier_func( t, bezier_tbl[n+0].pos, bezier_tbl[n+1].pos, bezier_tbl[n+2].pos, bezier_tbl[n+3].pos );

//				gra.Circle( gv1, 5,rgb(0,0,1));
				gra.Fill( gv1-4, gv1+4, rgb(1,1,1));

				if ( bForward ) t+=0.01; else t-=0.01;


				if ( t >= 1.0 ) 
				{
					if ( n+3 < static_cast<signed>(bezier_tbl.size())-3 )
					{
						t = 0;
						n+=3;
					}
					else
					{
						t = 1.0;
						bForward = !bForward;
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
						bForward = !bForward;
					}
				}
			}

			static vect2 gv2;
			if(1)
			{//カトマルアニメーション
		
				static	double t = 0;
				static	bool	bForward = true;

				static int n = 0;
				int n0 = n-1;
				int n1 = n;
				int n2 = n+1;
				int n3 = n+2;
				if ( n0<0 ) n0 = 0;
				if ( n3>=static_cast<signed>(catmull_tbl.size()) ) n3 =n2;

				gv2 = catmull_func( t, catmull_tbl[n0].pos, catmull_tbl[n1].pos, catmull_tbl[n2].pos, catmull_tbl[n3].pos );

//				gra.Circle( gv2, 5,rgb(0,0,1));
				gra.Fill( gv2-4, gv2+4, rgb(1,1,1));

				if ( bForward ) t+=0.01; else t-=0.01;


				if ( t >= 1.0 ) 
				{
					if ( n+1 < static_cast<signed>(catmull_tbl.size())-1 )
					{
						t = 0;
						n+=1;
					}
					else
					{
						t = 1.0;
						bForward = !bForward;
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
						bForward = !bForward;
					}
				}
			}


			

			//=================================
			// 入力
			//=================================

			// マーカー操作	
//			mc.funcMarkerController2( figCircle, mouse, keys, gra );
			// 3Dマーカー入力
			{
				// 最近マーカー初期化
				if ( !keys.ALT.on && mouse.L.hi ) mc.selector2.marker_clear( mouse.pos );

				// 矩形カーソル開始
				if ( !keys.ALT.on && mouse.L.hi && mc.selector2.marker_a.pm == 0 ) mc.selector2.marker_beginRectcursor( mouse.pos );

				// マーカー全解除
				if ( !keys.ALT.on && mouse.L.hi && !keys.CTRL.on && !keys.SHIFT.on && !mc.selector2.marker_a.pm ) mc.selector2.marker_allclear();

				// マーカー 反転選択
				if ( !keys.ALT.on && mouse.L.hi &&  keys.CTRL.on && !keys.SHIFT.on &&  mc.selector2.marker_a.pm ) mc.selector2.marker_selectReverse();

				//	マーカー 追加選択 
				if ( !keys.ALT.on && mouse.L.hi && !keys.CTRL.on &&  keys.SHIFT.on &&  mc.selector2.marker_a.pm ) mc.selector2.marker_selectAdd();

				//	マーカー 単独選択 
				if ( !keys.ALT.on && mouse.L.hi && !keys.CTRL.on && !keys.SHIFT.on &&  mc.selector2.marker_a.pm && mc.selector2.marker_a.pm->obj.bSelected == false ) mc.selector2.marker_selectOne();

				// マーカー移動
				if ( !keys.ALT.on && mouse.L.on && !keys.CTRL.on && !keys.SHIFT.on && !mc.selector2.marker_rect_bSelect ) 
				{
//					mc.selector2.marker_move( mouse.mov, pers, cam.mat, (*pBone) );
					// マーカー移動
					for ( Marker& m : mc.selector2.marker_tblMarker3 )
					{
						if ( m.obj.bSelected )
						{
							m.obj.Move2( mouse.mov );
						}
					}

				}

				// 矩形カーソル 反転 選択	
				if ( !keys.ALT.on && mouse.L.on &&  keys.CTRL.on && !keys.SHIFT.on &&  mc.selector2.marker_rect_bSelect ) mc.selector2.marker_rect_selectReverse( mouse.pos );

				// 矩形カーソル 追加選択	
				if ( !keys.ALT.on && mouse.L.on && !keys.CTRL.on &&  mc.selector2.marker_rect_bSelect ) mc.selector2.marker_rect_selectAdd( mouse.pos );

				// 矩形カーソル解除	
				if ( !keys.ALT.on && !mouse.L.on &&  mc.selector2.marker_rect_bSelect ) mc.selector2.marker_endRect();
				
			}

			

Joint2& tar = tblJoint_2d[0];
tar.pos = gv1;

			//=================================
			// 2D骨力
			//=================================
			// 慣性移動
			for ( Joint2& a : tblJoint_2d )
			{
//				a.pos += a.accell;
//				a.accell*=0.90;
			}

			for ( Joint2& j : tblJoint_2d )
			{
//				j.mov = ( j.pos - j.prev );
			}

			for ( int i = 0 ; i < 1 ; i++ )
			{
				// 骨コリジョン 張力計算
				for ( Bone2 b : tblBone_2d )
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
				for ( Joint2& a : tblJoint_2d )
				{
					a.pos += a.tension;
					//a.accell += a.tension;
					a.tension=0;
				}
			}

			// 保管	
			for ( Joint2& a : tblJoint_2d )
			{
//				a.prev = a.pos;
			}

			// 骨描画
			for ( Bone2 b : tblBone_2d )
			{
				vect2 v0 = b.j0.pos;
				vect2 v1 = b.j1.pos;

				gra.Line( v0, v1, rgb( 1,1,1 ) );
			}

			// マーカー表示
//			mc.funcMarkerDraw2();
			mc.selector2.marker_draw( mouse.pos, gra );



			// 3Dマーカー入力
			{
				// 最近マーカー初期化
				if ( !keys.ALT.on && mouse.L.hi ) mc.selector3.marker_clear( mouse.pos );

				// 矩形カーソル開始
				if ( !keys.ALT.on && mouse.L.hi && mc.selector3.marker_a.pm == 0 ) mc.selector3.marker_beginRectcursor( mouse.pos );

				// マーカー全解除
				if ( !keys.ALT.on && mouse.L.hi && !keys.CTRL.on && !keys.SHIFT.on && !mc.selector3.marker_a.pm ) mc.selector3.marker_allclear();

				// マーカー 反転選択
				if ( !keys.ALT.on && mouse.L.hi &&  keys.CTRL.on && !keys.SHIFT.on &&  mc.selector3.marker_a.pm ) mc.selector3.marker_selectReverse();

				//	マーカー 追加選択 
				if ( !keys.ALT.on && mouse.L.hi && !keys.CTRL.on &&  keys.SHIFT.on &&  mc.selector3.marker_a.pm ) mc.selector3.marker_selectAdd();

				//	マーカー 単独選択 
				if ( !keys.ALT.on && mouse.L.hi && !keys.CTRL.on && !keys.SHIFT.on &&  mc.selector3.marker_a.pm && mc.selector3.marker_a.pm->obj.bSelected == false ) mc.selector3.marker_selectOne();

				// マーカー移動
				if ( !keys.ALT.on && mouse.L.on && !keys.CTRL.on && !keys.SHIFT.on && !mc.selector3.marker_rect_bSelect ) mc.selector3.marker_move( mouse.mov, pers, cam.mat, (*pBone) );

				// 矩形カーソル 反転 選択	
				if ( !keys.ALT.on && mouse.L.on &&  keys.CTRL.on && !keys.SHIFT.on &&  mc.selector3.marker_rect_bSelect ) mc.selector3.marker_rect_selectReverse( mouse.pos );

				// 矩形カーソル 追加選択	
				if ( !keys.ALT.on && mouse.L.on && !keys.CTRL.on &&  mc.selector3.marker_rect_bSelect ) mc.selector3.marker_rect_selectAdd( mouse.pos );

				// 矩形カーソル解除	
				if ( !keys.ALT.on && !mouse.L.on &&  mc.selector3.marker_rect_bSelect ) mc.selector3.marker_endRect();
				
			}

			// human 更新
			pBone->update();

			// human 描画
			pBone->draw( pers, cam.mat, gra );

			// カトマル3D モーション軌跡表示
			pBone->drawMotion( pers, cam.mat, gra );

			// 3Dマーカー表示
			mc.selector3.marker_draw( mouse.pos, gra );




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
				int cx=56,cy=464;
				int sx =cx-128;
				int sy =cy-128;
				int ex =cx+128;
				int ey =cy+128;

				static int cnt = 0;
	#if 0
				auto func = [&]( double x0, double y0, double x1, double y1, int col)
				{
					gra.Line(x0,y0,x1,y1,col);
				};
				figTriangle.draw( func, 256,256,rad(cnt), rgb(0,1,1) );
	#else
				figTriangle.draw( vect2(cx,cy),rad(cnt), rgb(0,1,1) );
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

					long long f2 = chrono::duration_cast<chrono::microseconds>(dime_max).count();
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
