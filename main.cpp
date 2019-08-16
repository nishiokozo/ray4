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
#include "pers.h"
#include "bone.h"

		static vect3	g_dummy = vect3(0,0,0);


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
		Obj&	obj;

		Marker( Obj& _obj, int id ) : obj(_obj)
		{
			obj.id				= id;
			obj.bSelected		= false;
			obj.bRectIn			= false;
			obj.bRectSelected	= false;
			obj.bAffectable		= false;
		}
		Marker(const Marker& a) : obj(a.obj)
		{
			obj.id				= a.obj.id;
			obj.bSelected		= a.obj.bSelected;
			obj.bRectIn			= a.obj.bRectIn;
			obj.bRectSelected	= a.obj.bRectSelected;
			obj.bAffectable 	= a.obj.bAffectable;
		}	
		const Marker&	operator=(Marker&& a){return a;}	

	};



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
		bool IsVisuable()
		{
			return true;
		}
		virtual ~Joint2(){}
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
		bool IsVisuable()
		{
			return true;
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
		bool IsVisuable()
		{
			return true;
		}
	};

	struct PinAxis : Obj
	{
		
		vect3 disp;
		void Move2( vect2 v )
		{
		}
		vect2 Pos2()
		{
			return vect2( disp.x, disp.y );
		}
		bool IsVisuable()
		{
			return true;
		}
	};
	


	struct Selector
	{
		enum MODE
		{
			MODE_NONE,
			MODE_2D,
			MODE_3D,
		};

		int mode = MODE_NONE;

		vector<Marker>	tblMarker;
		vect2 rect_pos = vect2(0,0);			//	矩形選択開始位置
		bool rect_bSelect = false;	//	矩形選択中フラグ
		struct
		{
			double	len;
			Marker*	pm;
			int		cnt;
		} a = {99999,0,0};
		int		colNormal = rgb(1,1,0);
		int		colSelected = rgb(1,0,0);

		//---------------------------------------------------------------------
		void clear( vect2 mouse_pos )
		//---------------------------------------------------------------------
		{
			a.len = 9999;
			a.pm = 0;
			a.cnt = 0;

			// 最近マーカーを検索
			for ( Marker& m : tblMarker )
			{
				double len = (m.obj.Pos2()-mouse_pos).length();
				if ( len < 20.0 && a.len > len )
				{
					a.len = len;
					a.pm = &m;
					a.cnt++;
				}
			}

		}

		//---------------------------------------------------------------------
		void beginRectcursor( vect2 mouse_pos )
		//---------------------------------------------------------------------
		{
			rect_bSelect = true;
			rect_pos = mouse_pos;
		}

		//---------------------------------------------------------------------
		void allclear()
		//---------------------------------------------------------------------
		{
			for ( Marker& m : tblMarker )
			{
				m.obj.bSelected = false;
			}
		}

		//---------------------------------------------------------------------
		void selectReverse()
		//---------------------------------------------------------------------
		{
			a.pm->obj.bSelected = !a.pm->obj.bSelected;
		}

		//---------------------------------------------------------------------
		void selectAdd()
		//---------------------------------------------------------------------
		{
			a.pm->obj.bSelected = true;
		}

		//---------------------------------------------------------------------
		void selectOne()
		//---------------------------------------------------------------------
		{
			for ( Marker& m : tblMarker )
			{
				m.obj.bSelected = false;
			}
			a.pm->obj.bSelected = true;
		}

		//---------------------------------------------------------------------
		void rect_selectReverse( vect2 mouse_pos )
		//---------------------------------------------------------------------
		{
			vect2 v0 = min( rect_pos, mouse_pos );
			vect2 v1 = max( rect_pos, mouse_pos );

			// 矩形カーソル選択解除
			for ( Marker& m : tblMarker )
			{
				m.obj.bRectIn = false;
			}

			// 矩形カーソル内マーカーを検索
			for ( Marker& m : tblMarker )
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
		void rect_selectAdd( vect2 mouse_pos )
		//---------------------------------------------------------------------
		{
			vect2 v0 = min( rect_pos, mouse_pos );
			vect2 v1 = max( rect_pos, mouse_pos );

			// 矩形カーソル選択解除
			for ( Marker& m : tblMarker )
			{
				m.obj.bRectIn = false;
			}

			// 矩形カーソル内マーカーを検索
			for ( Marker& m : tblMarker )
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
		void endRect()
		//---------------------------------------------------------------------
		{
			rect_bSelect = false;
			for ( Marker& m : tblMarker )
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
		void drawController( vect2 mouse_pos, SysGra& gra )
		//---------------------------------------------------------------------
		{

			// 矩形エリア表示
			if ( rect_bSelect )
			{
				vect2 v0 = min( rect_pos, mouse_pos );
				vect2 v1 = max( rect_pos, mouse_pos );
				gra.Box( v0,v1, rgb(0,0.5,1));
			}

			// コントローラー表示
			for ( Marker m : tblMarker )
			{
				if ( m.obj.IsVisuable() )
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
		}
	

		
	};

	Selector selector;

	struct Manupirator : Obj
	{
		vect3 disp;
		void Move2( vect2 v )
		{
		}
		vect2 Pos2()
		{
			return vect2( disp.x, disp.y );
		}
		bool IsVisuable()
		{
			return true;
		}

		bool bAxisX = true;;
		bool bAxisY = true;;
		bool bAxisZ = true;;

		vect3	pos = vect3(0,-1,0);
		bool bActive = true;
		
		PinAxis	pinAxisX;
		PinAxis	pinAxisY;
		PinAxis	pinAxisZ;

		//------------------------------------------------------------------------------
		void manupirator_setPos( vect3 _pos )
		//------------------------------------------------------------------------------
		{
			pos = _pos;
			bActive = true;
		}
		//------------------------------------------------------------------------------
		void manupirator_drawAxis( Apr& apr )
		//------------------------------------------------------------------------------
		{
			{
					vect3 v0 = apr.pers.calcDisp( pos * apr.pers.cam.mat.invers() );
					disp = v0;
				double l = 30;
				if ( bAxisX  )
				{
					vect3 v1 = v0 + vect3( l,0,0) * apr.pers.cam.mat.invers();
		//			apr.gra.Circle( vect2(v1.x,v1.y), 7, rgb(1,1,0) );
					apr.gra.Line( vect2(v0.x,v0.y), vect2(v1.x,v1.y), rgb(1,0,0) );
	
					pinAxisX.disp = v1;
	
				}
				if ( bAxisY  )
				{
//					vect3 v0 = apr.pers.calcDisp( pos * apr.pers.cam.mat.invers() );
					vect3 v1 = v0 + vect3( 0,l,0) * apr.pers.cam.mat.invers();
					//apr.gra.Circle( vect2(v1.x,v1.y), 7, rgb(1,1,0) );
					apr.gra.Line( vect2(v0.x,v0.y), vect2(v1.x,v1.y), rgb(0,1,0) );
	
					pinAxisY.disp = v1;
				}
				if ( bAxisZ )
				{
//					vect3 v0 = apr.pers.calcDisp( pos * apr.pers.cam.mat.invers() );
					vect3 v1 = v0 + vect3( 0,0,l) * apr.pers.cam.mat.invers();
				//	apr.gra.Circle( vect2(v1.x,v1.y), 7, rgb(1,1,0) );
					apr.gra.Line( vect2(v0.x,v0.y), vect2(v1.x,v1.y), rgb(0,0,1) );
	
					pinAxisZ.disp = v1;
				}
			}

		
			if(0)
			{
				double l = 0.1;

				if ( bAxisX ) apr.line3d( (pos + vect3(-l,0,0)), (pos + vect3(l,0,0)), rgb(1,0,0) );
				if ( bAxisY ) apr.line3d( (pos + vect3(0,-l,0)), (pos + vect3(0,l,0)), rgb(0,1,0) );
				if ( bAxisZ ) apr.line3d( (pos + vect3(0,0,-l)), (pos + vect3(0,0,l)), rgb(0,0,1) );
			}

			if(0)
			{
				double l = 0.1;
				if ( bAxisX && bAxisY )
				{
					apr.line3d( (pos + vect3(-l,-l,0)), (pos + vect3( l,-l,0)), rgb(0,0,1) );
					apr.line3d( (pos + vect3(-l, l,0)), (pos + vect3( l, l,0)), rgb(0,0,1) );
					apr.line3d( (pos + vect3(-l,-l,0)), (pos + vect3(-l, l,0)), rgb(0,0,1) );
					apr.line3d( (pos + vect3( l,-l,0)), (pos + vect3( l, l,0)), rgb(0,0,1) );
				}

				if ( bAxisZ && bAxisY )
				{
					apr.line3d( (pos + vect3(0,-l,-l)), (pos + vect3(0, l,-l)), rgb(1,0,0) );
					apr.line3d( (pos + vect3(0,-l, l)), (pos + vect3(0, l, l)), rgb(1,0,0) );
					apr.line3d( (pos + vect3(0,-l,-l)), (pos + vect3(0,-l, l)), rgb(1,0,0) );
					apr.line3d( (pos + vect3(0, l,-l)), (pos + vect3(0, l, l)), rgb(1,0,0) );
				}

				if ( bAxisZ && bAxisX )
				{
					apr.line3d( (pos + vect3(-l,0,-l)), (pos + vect3( l,0,-l)), rgb(0,1,0) );
					apr.line3d( (pos + vect3(-l,0, l)), (pos + vect3( l,0, l)), rgb(0,1,0) );
					apr.line3d( (pos + vect3(-l,0,-l)), (pos + vect3(-l,0, l)), rgb(0,1,0) );
					apr.line3d( (pos + vect3( l,0,-l)), (pos + vect3( l,0, l)), rgb(0,1,0) );
				}
			}	

			if(0)
			{
				double l = 0.15;
				if ( bAxisZ && bAxisY ) apr.circle3d_x( pos, l, rgb(1,0,0) );
				if ( bAxisX && bAxisZ ) apr.circle3d_y( pos, l, rgb(0,1,0) );
				if ( bAxisX && bAxisY ) apr.circle3d_z( pos, l, rgb(0,0,1) );
			}
		}
	} manupirator;

	
	Figure figCircle=Figure(gra);






	Pers pers;


	//------------------------------------------------------------------------------
	void line3d( vect3 p0, vect3 p1, int col )
	//------------------------------------------------------------------------------
	{
		double l = 0.2;
		vect3 a = p0* pers.cam.mat.invers();
		vect3 b = p1* pers.cam.mat.invers();
		vect3 v0;
		vect3 v1;
		bool flg = pers.calcScissorLine3d( a, b, v0, v1 );
		if ( flg ) gra.Line( vect2(v0.x,v0.y), vect2(v1.x,v1.y), col );
	}

	//------------------------------------------------------------------------------
	void othro_line3d( vect3 p0, vect3 p1, int col )
	//------------------------------------------------------------------------------
	{
		double l = 0.2;
		vect3 a = p0* pers.cam.mat.invers();
		vect3 b = p1* pers.cam.mat.invers();
		vect3 v0;
		vect3 v1;
		bool flg = pers.calcScissorLine3d( a, b, v0, v1 );
		if ( flg ) gra.Line( vect2(v0.x,v0.y), vect2(v1.x,v1.y), col );
	}

	//------------------------------------------------------------------------------
	void circle3d_x( vect3 pos,  double r, int col )
	//------------------------------------------------------------------------------
	{
		vect2 v0;
		for ( int i = 0 ; i <= 360 ; i+=10 )
		{
			vect3 p = vect3( 0, r*cos(rad(i)), r*sin(rad(i)) ) + pos;
			vect3 q = pers.calcDisp( p * pers.cam.mat.invers() );
			vect2 v1 = vect2( q.x, q.y );
			if ( i > 0 ) gra.Line( v0,v1, col );
			v0 = v1;
		}
	}

	//------------------------------------------------------------------------------
	void circle3d_y( vect3 pos,  double r, int col )
	//------------------------------------------------------------------------------
	{
		vect2 v0;
		for ( int i = 0 ; i <= 360 ; i+=10 )
		{
			vect3 p = vect3( r*cos(rad(i)), 0, r*sin(rad(i)) ) + pos;
			vect3 q = pers.calcDisp( p * pers.cam.mat.invers() );
			vect2 v1 = vect2( q.x, q.y );
			if ( i > 0 ) gra.Line( v0,v1, col );
			v0 = v1;
		}
	}

	//------------------------------------------------------------------------------
	void circle3d_z( vect3 pos,  double r, int col )
	//------------------------------------------------------------------------------
	{
		vect2 v0;
		for ( int i = 0 ; i <= 360 ; i+=10 )
		{
			vect3 p = vect3( r*cos(rad(i)), r*sin(rad(i)), 0 ) + pos;
			vect3 q = pers.calcDisp( p * pers.cam.mat.invers() );
			vect2 v1 = vect2( q.x, q.y );
			if ( i > 0 ) gra.Line( v0,v1, col );
			v0 = v1;
		}

	}

	//------------------------------------------------------------------------------
	bool funcIntersectPlate( vect3 plate_P, vect3 plate_N, vect3 P , vect3 I, vect3& Q)
	//------------------------------------------------------------------------------
	{
		double	f = dot(plate_N, P - plate_P);
	//	if ( f > 0 )
		{
			double	t = -f/dot( plate_N, I );

			if ( t >= 0 )
			{
				Q = I * t + P;
				return true;
			}
		}
		return false;
	};

	struct Grid
	{
		vect3	pos;
		int		mode;
		int		NUM_U;
		int		NUM_V;
		double	dt;
		int		col;
	
		//------------------------------------------------------------------------------
		void SetMesh( vect3 _pos, int _mode, int _NUM_U, int _NUM_V, double _dt, int _col )
		//------------------------------------------------------------------------------
		{	// ミニグリッド
			pos 	= _pos;
			mode	= _mode;
			NUM_U	= _NUM_U;
			NUM_V	= _NUM_V;
			dt		= _dt;
			col	 = _col;
		}
		//------------------------------------------------------------------------------
		void DrawMesh( Apr& apr )
		//------------------------------------------------------------------------------
		{	// ミニグリッド
	//			const int NUM_U = 5;
	//		double dt = 0.05;
			vect3 vt = vect3(0,0,0);
			double du = NUM_U*dt;
			double dv = NUM_V*dt;
			vect3 a;
			vect3 b;
//			int col = rgb(0.2,0.2,0.2)*2;
			{
				if ( mode == 0 )
				{
					a = pos+vect3(-du, 0,-du);
					b = pos+vect3( du, 0,-du);
					vt = vect3(0,0,dt);
				}
				if ( mode == 1 )
				{
					a = pos+vect3(-du,-du,0);
					b = pos+vect3( du,-du,0);
					vt = vect3(0,dt,0);
				}
				if ( mode == 2 )
				{
					a = pos+vect3( 0,-du,-du);
					b = pos+vect3( 0, du,-du);
					vt = vect3(0,0,dt);
				}
				for ( int i = 0 ; i < NUM_V*2+1 ; i++ )
				{
					apr.line3d( a, b, col );
					a+=vt;
					b+=vt;
				}
			}			
			{

				if ( mode == 0 )
				{
					a = pos+vect3(-dv, 0, dv);
					b = pos+vect3(-dv, 0,-dv);
					vt = vect3(dt,0,0);
				}
				if ( mode == 1 )
				{
					a = pos+vect3(-dv, dv, 0);
					b = pos+vect3(-dv,-dv, 0);
					vt = vect3(dt,0,0);
				}
				if ( mode == 2 )
				{
					a = pos+vect3( 0,-dv, dv);
					b = pos+vect3( 0,-dv,-dv);
					vt = vect3(0,dt,0);
				}
				for ( int i = 0 ; i < NUM_U*2+1 ; i++ )
				{
					apr.line3d( a, b, col );
					a+=vt;
					b+=vt;
				}
			}			
		}
	};
	Grid gridGround;
	Grid gridMini;
	
	
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


		pPreset->animations.emplace_back();

		Bone* pBone = pPreset;

#else
		unique_ptr<Bone> pBone(new Bone);
#endif
				{
					//読み込み
					unique_ptr<Bone> pNew(new Bone);
					pNew->loadMotion( "human.mot" );

					{
						int id = 0;
						selector.tblMarker.clear();
						for ( Joint3& j : pNew->tblJoint )	//マーカー登録
						{
							selector.tblMarker.emplace_back( j, id++ );
						}
						selector.mode = Selector::MODE_3D;
					}
					pBone = move(pNew);
				}


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

		// 輪
		struct
		{
			double	rx = rad(0);
			double	ry = rad(0);
			double	rz = rad(0);

			vect3 pos = {2.5,-0.5,2.5};

			const double h = 0.1;
			const double w = 0.25;
			vector<vect3> vert=
			{
				{	-w,	 h,	-w	},
				{	 w,	 h,	-w	},
				{	-w,	-h,	-w	},
				{	 w,	-h,	-w	},
				{	-w,	 h,	 w	},
				{	 w,	 h,	 w	},
				{	-w,	-h,	 w	},
				{	 w,	-h,	 w	},
			};

			vector<ivect3>	faces =
			{
				{2,3,0},{3,1,0},
				{3,7,1},{1,7,5},
				{7,6,5},{5,6,4},
				{6,2,4},{4,2,0},
			};
			

		} ring;

		struct Trigon
		{
			double	z;
			vect2	v0;
			vect2	v1;
			vect2	v2;
			int		col;
			Trigon( 
				double	_z,
				vect2	_v0,
				vect2	_v1,
				vect2	_v2,
				int		_col
			)
			:
			z(_z),
			v0(_v0),
			v1(_v1),
			v2(_v2),
			col(_col)
			{}		
		};
		
		vector<Trigon>	trigons;


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
					gra.Print(vect2(10,16*y++),string("[F1] help"));
					gra.Print(vect2(10,16*y++),string("[Y] pers -"));
					gra.Print(vect2(10,16*y++),string("[H] pers +"));
					gra.Print(vect2(10,16*y++),string("[L] Load"));
					gra.Print(vect2(10,16*y++),string("[S] Save"));
					gra.Print(vect2(10,16*y++),string("--Keyframe--"));
					gra.Print(vect2(10,16*y++),string("[K] Insert"));
					gra.Print(vect2(10,16*y++),string("[X] Cut"));
					gra.Print(vect2(10,16*y++),string("[C] Copy"));
					gra.Print(vect2(10,16*y++),string("[V] Past"));
					gra.Print(vect2(10,16*y++),string("[LEFT]  -"));
					gra.Print(vect2(10,16*y++),string("[RIGHT] +"));
					gra.Print(vect2(10,16*y++),string("--Animation--"));
					gra.Print(vect2(10,16*y++),string("[I] Add"));
					gra.Print(vect2(10,16*y++),string("[P] Play"));
					gra.Print(vect2(10,16*y++),string("[UP] -"));
					gra.Print(vect2(10,16*y++),string("[DOWN] +"));
					gra.Print(vect2(10,16*y++),string("--Other--"));
					gra.Print(vect2(10,16*y++),string("[1] select 3d"));
					gra.Print(vect2(10,16*y++),string("[2] select 2main"));
				}
				else
				{
					//gra.Print(vect2(10,16*y++),string("[F1] Help"));
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
				if ( keys.CTRL.on && keys.L.hi ) 
				{
					//読み込み
					unique_ptr<Bone> pNew(new Bone);
					pNew->loadMotion( "human.mot" );

					{
						int id = 0;
						selector.tblMarker.clear();
						for ( Joint3& j : pNew->tblJoint )	//マーカー登録
						{
							selector.tblMarker.emplace_back( j, id++ );
						}
						selector.mode = Selector::MODE_3D;
					}
					pBone = move(pNew);
				}

				// キーフレームセーブ
				if ( keys.CTRL.on && keys.S.hi ) pBone->saveMotion( "human.mot" );

				// キーフレームペースト
				if ( keys.CTRL.on && keys.V.hi ) pBone->PastKeyframe();

				// キーフレームコピー
				if ( keys.CTRL.on && keys.C.hi ) pBone->CopyKeyframe();

				// キーフレームカット
				if ( keys.CTRL.on && keys.X.hi ) pBone->CutKeyframe();

				// キーフレーム追加
				if ( keys.K.hi ) pBone->InsertKeyframe();

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

				// X/Y/Z軸選択モード切替
			#if 0
				if ( keys.X.hi ) manupirator.bAxisX = !manupirator.bAxisX;
				if ( keys.C.hi ) manupirator.bAxisY = !manupirator.bAxisY;
				if ( keys.Z.hi ) manupirator.bAxisZ = !manupirator.bAxisZ;
			#else
				if ( keys.Z.hi ) {manupirator.bAxisZ = true;	manupirator.bAxisX = false;	manupirator.bAxisY = false;}
				if ( keys.X.hi ) {manupirator.bAxisZ = false;	manupirator.bAxisX = true;	manupirator.bAxisY = false;}
				if ( keys.C.hi ) {manupirator.bAxisZ = false;	manupirator.bAxisX = false;	manupirator.bAxisY = true;}
				if ( keys.V.hi ) {manupirator.bAxisZ = true;	manupirator.bAxisX = true;	manupirator.bAxisY = true;}
			#endif
			}


			{
				int y = 1;
				gra.Print(vect2(10,16*y++),string("fovY:")+to_string(int(pers.fovy)));
				if( keys.F2.on )
				{
					gra.Print(vect2(10,16*y++),string("sz:")+to_string(pers.sz) +string(" fy:")+to_string(pers.fy));
					gra.Print( vect2(10,16*y++),string("far:")+to_string((pers.cam.pos-pers.cam.at).length())); 
					gra.Print( vect2(10,16*y++),string("at  x=")+to_string(pers.cam.at.x)+string(" y=")+to_string(pers.cam.at.y)+string(" z=")+to_string(pers.cam.at.z) ); 
					gra.Print( vect2(10,16*y++),string("pos x=")+to_string(pers.cam.pos.x)+string(" y=")+to_string(pers.cam.pos.y)+string(" z=")+to_string(pers.cam.pos.z) ); 
					gra.Print( vect2(10,16*y++),string("anim=")+to_string(pBone->cur.act) + string(" cnt=")+to_string(pBone->animations.size()) ); 
					if ( pBone->animations.size() > 0 ) 
					{
						gra.Print( vect2(10,16*y++),string("pose=")+to_string(pBone->cur.pose) + string(" cnt=")+to_string(pBone->animations[pBone->cur.act].pose.size()) ); 
					}
					gra.Print( vect2(10,16*y++),string("peak=")+to_string(time_peak/1000)+string("msec") ); 
				}
//					gra.Print( vect2(10,16*y++),string("axis ")+(manupirator.bAxisZ?"Z":"-")+(manupirator.bAxisX?"X":"-")+(manupirator.bAxisY?"Y":"-") ); 

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
			if ( (!keys.ALT.on && mouse.R.on) || (keys.ALT.on && mouse.L.on) ) pers.cam.Rotation( vect3(-mouse.mov.x/28,-mouse.mov.y/28,0) );

			// カメラ平行移動
			if ( mouse.M.on ) pers.cam.Move( vect3(-mouse.mov.x,-mouse.mov.y,0)/pers.height/pers.getW((pers.cam.pos-pers.cam.at).length()));

			// マウスホイールZOOM
			if ( !keys.ALT.on  ) pers.cam.Zoom( -mouse.wheel*1.5 /pers.height/pers.getW((pers.cam.pos-pers.cam.at).length()) );
			
			// カメラ移動
			if ( keys.ALT.on && mouse.R.on ) pers.cam.Zoom( mouse.mov.y*3/pers.height/pers.getW((pers.cam.pos-pers.cam.at).length()) );
			
			// カメラマトリクス計算
			{
				pers.cam.mat.LookAt( pers.cam.pos, pers.cam.at, pers.cam.up );
			}

			// カメラ注視点表示
			{
				vect3 v = pers.calcDisp(pers.cam.at*pers.cam.mat.invers());
				if ( v.z > 0 ) 
				{
					//gra.Circle( vect2(v.x,v.y), 8, rgb(0,1,0));
				}
			}

			// グリッドgridMini
			{
				const int NUM = 20;

				gridGround.SetMesh( vect3(0,0,0), 0, NUM, NUM, 1, rgb(0.2,0.2,0.2) );
				gridGround.DrawMesh( *this );

				if(0)
				{	// 格子グリッド
//					int col = rgb(0.5,0.5,0.5);
					int col = rgb(0.2,0.2,0.2);
					{
						vect3 a(-NUM, 0,-NUM);
						vect3 b( NUM, 0,-NUM);
						for ( int i = 0 ; i < NUM*2+1 ; i++ )
						{
							line3d( a, b, col );
/*							vect3 v0;
							vect3 v1;
							bool flg = pers.calcScissorLine3d( a* pers.cam.mat.invers(), b* pers.cam.mat.invers(), v0, v1 );

							if ( flg )
							{
								gra.Line( vect2(v0.x,v0.y), vect2(v1.x,v1.y), col );
							}
							
*/
							a.z+=1.0;
							b.z+=1.0;
						}
					}			
					{
						vect3 a(-NUM, 0, NUM);
						vect3 b(-NUM, 0,-NUM);
						for ( int i = 0 ; i < NUM*2+1 ; i++ )
						{
							line3d( a, b, col );
/*
							vect3 v0;
							vect3 v1;
							bool flg = pers.calcScissorLine3d( a* pers.cam.mat.invers(), b* pers.cam.mat.invers(), v0, v1 );
							if ( flg )
							{
								gra.Line( vect2(v0.x,v0.y), vect2(v1.x,v1.y), col);
							}
*/
							a.x+=1.0;
							b.x+=1.0;
						}
					}			
				}

				if(0)
				{	// ドットグリッド
					int col = rgb(1,1,1);
					{
						for ( int i = -NUM ; i <= NUM ; i++ )
						{
							for ( int j = -NUM ; j <= NUM ; j++ )
							{
								vect3 v0 = pers.calcDisp( vect3(i,0,j)  * pers.cam.mat.invers() );
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
			}
			
			// 箱
			//calcDisp rotate
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

				v = v * pers.cam.mat.invers();

				box.disp.emplace_back( v );

			}
			
			
			// 箱
			for ( ivect2 e : box.edge )
			{
				const vect3& a = box.disp[e.p];
				const vect3& b = box.disp[e.n];


				vect3 v0;
				vect3 v1;
				bool flg = pers.calcScissorLine3d( a, b, v0, v1 );
				if ( flg )
				{
					gra.Line( vect2(v0.x,v0.y), vect2(v1.x,v1.y), rgb(0,1,1));
				}

			}


			// 輪 ring
			//calcDisp rotate
			{
				vect3 l = vect3(0,0,1).normalize();	// 正面ライト
				for ( ivect3 v : ring.faces )
				{
					mat44	rotx;
					mat44	roty;
					mat44	rotz;
					rotx.setRotateX(ring.rx);
					roty.setRotateY(ring.ry);
					rotz.setRotateZ(ring.rz);

					vect3 v0 = ring.vert[v.n0];
					vect3 v1 = ring.vert[v.n1];
					vect3 v2 = ring.vert[v.n2];

					v0= rotx * roty * rotz *v0 + ring.pos ;
					v1= rotx * roty * rotz *v1 + ring.pos ;
					v2= rotx * roty * rotz *v2 + ring.pos ;

					v0 = v0 * pers.cam.mat.invers();
					v1 = v1 * pers.cam.mat.invers();
					v2 = v2 * pers.cam.mat.invers();
	
					double d = 0;
					{
						vect3 a = (v1-v0); 
						vect3 b = (v2-v0); 
						vect3 n = cross(a,b).normalize();
						d = dot(n,l) + 0.2;
						if ( d < 0.0 ) d=0;
						if ( d > 1.0 ) d=1.0;
					}

					v0 = pers.calcDisp( v0 );
					v1 = pers.calcDisp( v1 );
					v2 = pers.calcDisp( v2 );
					vect2 d0 = vect2(v0.x,v0.y);
					vect2 d1 = vect2(v1.x,v1.y);
					vect2 d2 = vect2(v2.x,v2.y);

					{
						vect2 a = vect2(d1-d0);
						vect2 b = vect2(d2-d0);
						double z = a.x*b.y-a.y*b.x;
						if ( z > 0 ) 
						{
							trigons.emplace_back( z, d0, d1, d2, rgb(d,d,d) );
//							gra.Tri( d0, d1, d2, rgb(d,d,d));
						}
					}
				}
			}	
			
			// トリゴン描画 trigons	
			for ( Trigon& t : trigons )
			{
					gra.Tri( t.v0, t.v1, t.v2, t.col);
			}
			trigons.clear();

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


			

			// マーカー登録
			{
				if ( keys._3.hi )	//マニュピレーター
				{
					selector.mode = Selector::MODE_3D;
					int id = 0;
					selector.tblMarker.clear();
											// マニュピレーターのマーカー登録
							selector.tblMarker.emplace_back( manupirator, id++ );
							selector.tblMarker.emplace_back( manupirator.pinAxisX, id++ );
							selector.tblMarker.emplace_back( manupirator.pinAxisY, id++ );
							selector.tblMarker.emplace_back( manupirator.pinAxisZ, id++ );


				}
				if ( keys._2.hi )	//2D
				{
					int id = 0;
					selector.tblMarker.clear();
					for ( Catmull& c : catmull_tbl )	// マーカー対象に位置を登録
					{
						selector.tblMarker.emplace_back( c, id++ );
					}
					for ( Bezier& b : bezier_tbl )	// マーカー対象に位置を登録
					{
						selector.tblMarker.emplace_back( b, id++ );
					}
					for ( Joint2& j : tblJoint_2d )	//マーカー対象に位置を登録
					{
						selector.tblMarker.emplace_back( j, id++ );
					}
					selector.mode = Selector::MODE_2D;
				}
				if ( keys._1.hi )	//3D human
				{
					int id = 0;
					selector.tblMarker.clear();
					for ( Joint3& j : pBone->tblJoint )	//マーカー登録
					{
						selector.tblMarker.emplace_back( j, id++ );
					}
				}

			}



			// マーカー操作	
			{
				// 最近マーカー初期化
				if ( !keys.ALT.on && mouse.L.hi ) selector.clear( mouse.pos );

				// 矩形カーソル開始
				if ( !keys.ALT.on && mouse.L.hi && selector.a.pm == 0 ) selector.beginRectcursor( mouse.pos );

				// マーカー全解除
				if ( !keys.ALT.on && mouse.L.hi && !keys.CTRL.on && !keys.SHIFT.on && !selector.a.pm ) 
				{
					selector.allclear();
					manupirator.bAxisZ = true;	manupirator.bAxisX = true;	manupirator.bAxisY = true;
				}

				// マーカー 反転選択
				if ( !keys.ALT.on && mouse.L.hi &&  keys.CTRL.on && !keys.SHIFT.on &&  selector.a.pm ) selector.selectReverse();

				//	マーカー 追加選択 
				if ( !keys.ALT.on && mouse.L.hi && !keys.CTRL.on &&  keys.SHIFT.on &&  selector.a.pm ) selector.selectAdd();

				//	マーカー 単独選択 
				if ( !keys.ALT.on && mouse.L.hi && !keys.CTRL.on && !keys.SHIFT.on &&  selector.a.pm && selector.a.pm->obj.bSelected == false ) selector.selectOne();

				// 矩形カーソル 反転 選択	
				if ( !keys.ALT.on && mouse.L.on &&  keys.CTRL.on && !keys.SHIFT.on &&  selector.rect_bSelect ) selector.rect_selectReverse( mouse.pos );

				// 矩形カーソル 追加選択	
				if ( !keys.ALT.on && mouse.L.on && !keys.CTRL.on &&  selector.rect_bSelect ) selector.rect_selectAdd( mouse.pos );

				// 矩形カーソル解除	
				if ( !keys.ALT.on && !mouse.L.on &&  selector.rect_bSelect ) selector.endRect();

				// マーカー移動準備
				if ( !keys.ALT.on && mouse.L.hi && !keys.CTRL.on && !keys.SHIFT.on && !selector.rect_bSelect ) 
				{
					if ( selector.mode == Selector::MODE_3D )
					{

						#if 1
						if ( selector.a.pm )
						{
							// 優先度つけ
							for ( Joint3& j : pBone->tblJoint )
							{
								j.priority = 999;
							}
							function<void( Joint3&,int)> funcSetPriority = [&funcSetPriority] ( Joint3& j, int prio )
							{
								j.priority = prio;
								for ( Joint3& r : j.relative )
								{
									if ( r.priority > prio+1 ) funcSetPriority( r, prio+1 );
								}
							};
							
							Joint3* pj = dynamic_cast<Joint3*>(&selector.a.pm->obj);
							if ( pj ) funcSetPriority( *pj, 1 );
						}
						#endif


						// 3Dマーカー移動
						for ( Marker& m : selector.tblMarker )
						{
							Joint3* pj = dynamic_cast<Joint3*>(&m.obj);
							if ( pj && pj->bSelected )
							{
								// 平行移動 カメラ面
								if ( manupirator.bAxisX && manupirator.bAxisY && manupirator.bAxisZ )
								{
								}
								else
								{// 3D 平面上を触る
									
									if ( manupirator.bAxisX && manupirator.bAxisZ )
									{
										gridMini.SetMesh( pj->pos, 0, 5, 5, 0.05, rgb(0.2,0.2,0.2) );
									}
									else
									if ( manupirator.bAxisX && manupirator.bAxisY )
									{
										gridMini.SetMesh( pj->pos, 1, 5, 5, 0.05, rgb(0.2,0.2,0.2) );
									}
									else
									if ( manupirator.bAxisZ && manupirator.bAxisY )
									{
										gridMini.SetMesh( pj->pos, 2, 5, 5, 0.05, rgb(0.2,0.2,0.2) );
									}
									else
									if ( manupirator.bAxisX )
									{
										gridMini.SetMesh( pj->pos, 0, 1, 5, 0.05, rgb(0.2,0.2,0.2) );
									}
									else
									if ( manupirator.bAxisY )
									{
										gridMini.SetMesh( pj->pos, 1, 1, 5, 0.05, rgb(0.2,0.2,0.2) );
									}
									else
									if ( manupirator.bAxisZ )
									{
										gridMini.SetMesh( pj->pos, 0, 1, 5, 0.05, rgb(0.2,0.2,0.2) );
									}
									
									
								}


							}
						}
					}
				
				}
						gra.Print( vect2(mouse.pos.x+10,mouse.pos.y-0),string("")+(manupirator.bAxisX?"X":"")+(manupirator.bAxisY?"Y":"")+(manupirator.bAxisZ?"Z":"") ); 
				// マーカー移動
				if ( !keys.ALT.on && mouse.L.on && !keys.CTRL.on && !keys.SHIFT.on && !selector.rect_bSelect ) 
				{
					if ( selector.mode == Selector::MODE_3D )
					{

						// 3Dマーカー移動
						for ( Marker& m : selector.tblMarker )
						{
							Joint3* pj = dynamic_cast<Joint3*>(&m.obj);
							if ( pj && pj->bSelected )
							{
								// 平行移動 カメラ面
								if ( manupirator.bAxisX && manupirator.bAxisY && manupirator.bAxisZ )
								{
									vect3 v = vect3(mouse.mov.x, mouse.mov.y, 0)/pers.height/(pj->disp.z);
									mat44 mrot = pers.cam.mat;
									mrot.SetTranslate(vect3(0,0,0));
									mrot.invers();
									v = v* mrot;
									pj->pos += v ;
								}
								else
								{// 3D 平面上を触る
									vect3	plate_P = pj->pos;
									vect3	plate_N;
									if ( manupirator.bAxisX && manupirator.bAxisZ )
									{
										// X-Z 平面 を仮定する。
										 plate_N = vect3(0,-1,0);
									}
									else
									if ( manupirator.bAxisX && manupirator.bAxisY )
									{
										// X-Y 平面 を仮定する。
										 plate_N = vect3(0,0,-1);
									}
									else
									if ( manupirator.bAxisZ && manupirator.bAxisY )
									{
										// Z-Y 平面 を仮定する。
										 plate_N = vect3(-1,0,0);
									}
									else
									if ( manupirator.bAxisX )
									{
										 plate_N = vect3(0,-1,0);
									}
									else
									if ( manupirator.bAxisY )
									{
										 plate_N = vect3(0,0,-1);
									}
									else
									if ( manupirator.bAxisZ )
									{
										 plate_N = vect3(-1,0,0);
									}

									vect3 P = pers.calcInvers( vect2( mouse.pos.x, mouse.pos.y ) );
									vect3 I = pers.calcRayvect( P );
									vect3 Q;
									bool b = funcIntersectPlate( plate_P, plate_N,  P, I, Q );
									if ( b )
									{
										vect3 P = pers.calcInvers( vect2( mouse.prev.x, mouse.prev.y ) );
										vect3 I = pers.calcRayvect( P );
										vect3 Q0;
										bool b = funcIntersectPlate( plate_P, plate_N, P, I, Q0 );
										if ( b )
										{
											if ( manupirator.bAxisY )
											{
												double y  = dot( (Q-Q0), vect3(0,1,0) );
												pj->pos.y += y;
											}
											if ( manupirator.bAxisZ )
											{
												double z  = dot( (Q-Q0), vect3(0,0,1) );
												pj->pos.z += z;
											}
											if ( manupirator.bAxisX )
											{
												double x  = dot( (Q-Q0), vect3(1,0,0) );
												pj->pos.x += x;
											}
											//circle3d_y( pj->pos, 0.1, rgb(0.8,0.2,0.2) );
										}
									}

										//gridMini.DrawMesh( *this );
									
									
								}


							}
						}
						pBone->RefrectKeyframe();
					}
					if ( selector.mode == Selector::MODE_2D )
					{
						// 2Dマーカー移動
						for ( Marker& m : selector.tblMarker )
						{
							if ( m.obj.bSelected )
							{
								m.obj.Move2( mouse.mov );
							}
						}
					}
				}				
			}			

			


			//=================================
			// 2D joint
			//=================================
			{
				Joint2& tar = tblJoint_2d[0];
				tar.pos = gv1;


				for ( int i = 0 ; i < 1 ; i++ )
				{
					// 骨コリジョン 張力計算
					for ( Bone2 b : tblBone_2d )
					{
						vect2 v = b.j1.pos - b.j0.pos;
						double l = v.length() - b.length;
						double w = 0;

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

				// 骨描画
				for ( Bone2 b : tblBone_2d )
				{
					vect2 v0 = b.j0.pos;
					vect2 v1 = b.j1.pos;

					gra.Line( v0, v1, rgb( 1,1,1 ) );
				}
			}



			//=================================
			// 3D joint
			//=================================

			// human 更新
			pBone->update();

			// human 描画
			pBone->draw( pers, gra );


			if ( !(pBone->anim.bPlaying && selector.mode == Selector::MODE_3D) )
			{
				// カトマル3D モーション軌跡表示
				pBone->drawMotion( pers, gra );

				// マーカー表示
				selector.drawController( mouse.pos, gra );

			}
			
			
			
			// 原点
			circle3d_y( vect3(0,0,0), 0.1, rgb(0.2,0.2,0.2) );


			// マニュピレーター描画
			if ( manupirator.bActive  ) 
			{
				vect3 p = pers.calcInvers( vect2( mouse.pos.x, mouse.pos.y ) );
				manupirator.manupirator_setPos( p );
				manupirator.manupirator_drawAxis( *this );

			}


			// マウス座標（投影面座標）を３Ｄ空間座標に逆変換
			if(0)
			{
				vect3 v = pers.calcInvers( vect2( mouse.pos.x, mouse.pos.y ) );
				vect3 p = pers.calcRay( v, 10 );


				line3d( v, p, rgb(1,0,0));
				line3d( vect3(0,0,0), v, rgb(1,1,0));
				line3d( vect3(0,-1,0), p, rgb(1,0,1));



				gra.Print( vect2(10,16*20),string("v x=")+to_string(v.x) + string(" y=")+to_string(v.y) +string(" z=")+to_string(v.z) );
				gra.Print( vect2(10,16*21),string("p x=")+to_string(p.x) + string(" y=")+to_string(p.y) +string(" z=")+to_string(p.z) );
//				gra.Print( vect2(10,16*21),string(" x=")+to_string(pers.cam.pos.x) + string(" y=")+to_string(pers.cam.pos.y) +string(" z=")+to_string(pers.cam.pos.z) );
			}

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
				figTriangle.draw( vect2(cx,cy),rad(cnt), rgb(0,1,1) );
				cnt++;
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
	Apr	apr("Ray4 " __DATE__, 300,300,768, 512 );
	return apr.main();
}
