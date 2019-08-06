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




struct Apr : public Sys
{
	int numKeyframe = 0;

	long long	time_peak = 0;

	struct Joint3
	{
		int id;
		vect3 pos;
		vect3 tension;
		vect3 world;
		vect3 disp;
		vect2 readonly_disp2;
		double len;
		int priority;
		
		vector<reference_wrapper<Joint3>>	relative;
		Joint3( vect3 v )
		{
			id = 0;
			pos = v;
			tension = 0;
			len = 0;
		}
	};

	struct Bone3
	{
		int n0;
		int n1;
		Joint3& j0;
		Joint3& j1;
		double length;
		Bone3( vector<Joint3>& tbl, int _n0, int _n1 ) :n0(_n0), n1(_n1), j0(tbl[_n0]), j1(tbl[_n1]){}
//			Bone3( vector<Joint3>& tbl, int _n0, int _n1 ) :n0(_n0), n1(_n1), j0(tbl[_n0]), j1(tbl[_n1]){}
	};



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
		Joint3&	joint;
//		vect3&	posx;
//		vect2&	dispx;
		bool 	bRectSelected;		//	矩形選択中、選択＆非選択
		bool 	bRectIn;			//	矩形選択中、矩形選択対象
		bool 	bSelected;			//	選択
		bool 	bAffectable;		//	削除対象
		double	th;
		int		colNormal = rgb(1,1,0);
		int		colSelected = rgb(1,0,0);

//		Marker3( SysGra& _gra, Figure& _fig, Joint3& _joint, vect3& _pos, vect2& _disp, double _th ) : gra(_gra), fig(_fig), joint(_joint), posx(_pos), dispx(_disp)
		Marker3( SysGra& _gra, Figure& _fig, Joint3& _joint, double _th ) : gra(_gra), fig(_fig), joint(_joint)//, posx(_pos), dispx(_disp)
		{
			bSelected		= false;
			bRectIn			= false;
			bRectSelected	= false;
			bAffectable		= false;
			th				= _th;
		}
		Marker3(const Marker3& a) : gra(a.gra), fig(a.fig), joint(a.joint)//, posx(a.posx), dispx(a.dispx)
		{
			bSelected		= a.bSelected;
			bRectIn			= a.bRectIn;
			bRectSelected	= a.bRectSelected;
			bAffectable 	= a.bAffectable;
			th 				= a.th;
		}	
		const Marker3&	operator=(Marker3&& a){return a;}	

/*
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
*/
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

				for ( int i = static_cast<signed>(tblMarker2.size())-1 ; i >= 0 ; i-- )
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

	} mc;

	struct Data
	{
		vector<Joint3>			tblJoint;
		vector<Bone3>			tblBone;
		vector<vector<vect3>>	tblKeyframe;
	};

	Figure figCircle=Figure(gra);

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

	//------------------------------------------------------------------------------
	void bone_ChangeKeyframeDown( Data& data )
	//------------------------------------------------------------------------------
	{
		numKeyframe--;
		numKeyframe = max( numKeyframe, 0 );
		numKeyframe = min( numKeyframe, static_cast<signed>(data.tblKeyframe.size())-1 );

		if ( numKeyframe >= 0 )
		{
			// キーフレーム切り替え
			int i = 0;
			for ( Joint3& j : data.tblJoint )
			{
				j.pos = data.tblKeyframe[ numKeyframe ][i];
				i++;
			}
		}
	}

	//------------------------------------------------------------------------------
	void bone_ChangeKeyframeUp( Data& data )
	//------------------------------------------------------------------------------
	{
		numKeyframe++; 
		numKeyframe = max( numKeyframe, 0 );
		numKeyframe = min( numKeyframe, static_cast<signed>(data.tblKeyframe.size())-1 );

		if ( numKeyframe >= 0 )
		{
			// キーフレーム切り替え
			int i = 0;
			for ( Joint3& j : data.tblJoint )
			{
				j.pos = data.tblKeyframe[ numKeyframe ][i];
				i++;
			}
		}
	}

	//------------------------------------------------------------------------------
	void bone_AddKeyframe( Data& data )
	//------------------------------------------------------------------------------
	{
		numKeyframe = static_cast<signed>(data.tblKeyframe.size());
		data.tblKeyframe.emplace_back();
		for ( const Joint3& j : data.tblJoint )
		{
			data.tblKeyframe[ numKeyframe ].emplace_back( j.pos );
		}
	}

	//------------------------------------------------------------------------------
	void bone_update( Data& data )
	//------------------------------------------------------------------------------
	{
		for ( int i = 0 ; i < 5 ; i++ )
		{
			// 骨コリジョン 張力計算
			for ( Bone3 b : data.tblBone )
			{
				vect3 v = b.j1.pos - b.j0.pos;
				double l = v.length() - b.length;
				vect3 va  =	v.normalize()*l;

				double w0 = 0;
				double w1 = 0;
#if 0
				if ( b.j0.priority ==1 ) 
				{
					w0 = 0.0;
					w1 = 1.0;
				}
				else
				if ( b.j1.priority ==1 ) 
				{
					w0 = 1.0;
					w1 = 0.0;
				}
				else
#endif
				{
					w0 = 0.33;
					w1 = 0.33;
				}
				b.j0.tension += va*w0;
				b.j1.tension -= va*w1;

			}

			// 張力解消
			for ( Joint3& a : data.tblJoint )
			{
				a.pos += a.tension;
				a.tension=0;
			}
		}

		// Human pers
		for ( Joint3& j : data.tblJoint )
		{
			//	右手系座標系
			//	右手ねじ周り
			//	roll	:z	奥+
			//	pitch	:x	右+
			//	yaw		:y	下+
			vect3 v= j.pos;

			v = v * cam.mat.invers();

			j.world = v;

			j.disp = pers.calcPoint(v);
			j.readonly_disp2 = vect2(j.disp.x,j.disp.y);
		}

		// Human 描画
		for ( Bone3 b : data.tblBone )
		{
			if ( b.j0.disp.z > 0 && b.j0.disp.z > 0 )
			{
				vect2 v0(b.j0.disp.x,b.j0.disp.y);
				vect2 v1(b.j1.disp.x,b.j1.disp.y);
				gra.Line( v0,v1, rgb(1,1,1));
			}
		}
	}
	//------------------------------------------------------------------------------
	void bone_save( Data& data, const char* filename )
	//------------------------------------------------------------------------------
	{
		fstream fo( filename, ios::out );

		{
			fo << "joint" << endl;
			for ( Joint3& j : data.tblJoint )	// 関節の位置
			{
				fo << "\t"<< j.pos.x << "\t" << j.pos.y << "\t" << j.pos.z << endl;
			}

			fo << "bone" << endl;
			for ( Bone3& b : data.tblBone )	// 骨
			{
				b.length = (b.j1.pos - b.j0.pos).length();
				fo  << "\t"<< b.n0 << "\t" << b.n1 << endl;
			}
		}
		{
			fo << "motion" << endl;
			int	cnt = static_cast<signed>(data.tblKeyframe.size());
			for ( int i = 0 ; i < cnt ; i++ )
			{
//							fo << to_string(i)  << endl;
				for ( int j = 0 ; j < static_cast<signed>(data.tblJoint.size()) ; j++ )
				{
					fo  << "\t"<< data.tblKeyframe[ i ][ j ].x << "\t" << data.tblKeyframe[ i ][ j ].y << "\t" << data.tblKeyframe[ i ][ j ].z << endl;
				}
				if( i+1 < cnt ) fo << "," << endl;
			}
		}
		fo << "end" << endl;

		cout << "SAVED" << endl;
	}
	//------------------------------------------------------------------------------
	Data*	bone_load( const char* filename )
	//------------------------------------------------------------------------------
	{
		Data*	pNew = new Data;

		fstream fi( filename, ios::in );
		string buf;
		enum
		{
			ModeNone,
			ModeJoint,
			ModeBone,
			ModeMotion,
		} mode = ModeNone;
		
		function<vector<string>(const string&, char)> split = [] (const string &s, char delim) 
		{
			vector<string> elems;
			stringstream ss(s);
			string item;
			while (getline(ss, item, delim)) 
			{
				if (!item.empty()) 
				{
					elems.push_back(item);
				}
			}
			return elems;
		};

		while ( getline( fi, buf ) )
		{
			cout << buf << endl;
			if ( string(buf) == "joint" )	{mode = ModeJoint;	continue;}
			if ( string(buf) == "bone" )	{mode = ModeBone;	continue;}
			if ( string(buf) == "motion" )	
			{
				mode = ModeMotion;	
				pNew->tblKeyframe.emplace_back();
				continue;
			}
			if ( string(buf) == "," ) 
			{
				pNew->tblKeyframe.emplace_back();
				continue;
			}
			if ( string(buf) == "end" )	
			{	
				for ( Bone3& b : pNew->tblBone )	// 関節の距離を決定する。
				{
					b.length = (b.j1.pos - b.j0.pos).length();
				}
				{
					int cnt = 0 ;
					for ( Joint3& j : pNew->tblJoint )
					{
						j.id = cnt++;				//id登録
					}
				}
				for ( Bone3& b : pNew->tblBone )	// ジョイントに関節の距離を決定する。
				{
					b.j1.relative.emplace_back( b.j0 ); 
					b.j0.relative.emplace_back( b.j1 ); 
				}
				//マーカー削除＆登録
				mc.tblMarker3.clear();
				for ( Joint3& j : pNew->tblJoint )	//マーカー登録
				{
					mc.tblMarker3.emplace_back( gra, figCircle, j, rad(-90) );
				}
				numKeyframe = 0;
				break;
			}
			switch( mode )
			{
				case ModeJoint:
					{
						vector<string> v = split( buf, '\t');
						double x = stod(v[0]);
						double y = stod(v[1]);
						double z = stod(v[2]);
						pNew->tblJoint.emplace_back( vect3(x,y,z) );
						//	cout << x << "," << y << "," << z << endl; 
					}
					break;
				case ModeBone:
					{
						vector<string> v = split( buf, '\t');
						int n0 = stoi(v[0]);
						int n1 = stoi(v[1]);
						pNew->tblBone.emplace_back( pNew->tblJoint, n0, n1 );
						//	cout << x << "," << y << "," << z << endl; 
					}
					break;
				case ModeMotion:
					{
						vector<string> v = split( buf, '\t');
						double x = stod(v[0]);
						double y = stod(v[1]);
						double z = stod(v[2]);
						pNew->tblKeyframe[ pNew->tblKeyframe.size()-1 ].emplace_back( x,y,z );
						//	cout << x << "," << y << "," << z << endl; 
					}
					break;
				default:
					break;
			}

		}
		cout << "LOADED" << endl;
		return pNew;
	}

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

//		Figure figCircle=Figure(gra);
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
		vector<Joint2> tblJoint_2d;
		tblJoint_2d.reserve(1000);
		vector<Bone2> tblBone_2d;
		if(1)
		{	//	対△
			int ox=200,oy=300;

			tblJoint_2d.emplace_back( vect2( ox,oy) );
			tblJoint_2d.emplace_back( vect2( ox-20,oy+60) );
			tblJoint_2d.emplace_back( vect2( ox+20,oy+60) );
			tblJoint_2d.emplace_back( vect2( ox+0 ,oy+120) );

			tblBone_2d.emplace_back( tblJoint_2d[0], tblJoint_2d[1] );
			tblBone_2d.emplace_back( tblJoint_2d[0], tblJoint_2d[2] );
			tblBone_2d.emplace_back( tblJoint_2d[1], tblJoint_2d[2] );
			tblBone_2d.emplace_back( tblJoint_2d[1], tblJoint_2d[3] );
			tblBone_2d.emplace_back( tblJoint_2d[2], tblJoint_2d[3] );
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
					tblJoint_2d.emplace_back( vect2( x*W+ox,y*H+oy) );
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
			mc.tblMarker2.emplace_back( gra, figCircle, j.pos, rad(-90) );
		}

		//人


		Data* pHuman = new Data;

		{	//	人
			double ox=0,oy=-160,os=0.01;

			pHuman->tblJoint.emplace_back( os*vect3( ox-10,	oy- 20,	0 )	);//0
			pHuman->tblJoint.emplace_back( os*vect3( ox+10,	oy- 20,	0 )	);//1
			pHuman->tblJoint.emplace_back( os*vect3( ox+ 0,	oy+  0,	0 )	);//2
			pHuman->tblJoint.emplace_back( os*vect3( ox-20,	oy+  0,	0 )	);//3
			pHuman->tblJoint.emplace_back( os*vect3( ox+20,	oy+  0,	0 )	);//4
			pHuman->tblJoint.emplace_back( os*vect3( ox+ 0,	oy+ 40,	0 )	);//5
			pHuman->tblJoint.emplace_back( os*vect3( ox-15,	oy+ 70,	0 )	);//6
			pHuman->tblJoint.emplace_back( os*vect3( ox+15,	oy+ 70,	0 )	);//7
			pHuman->tblJoint.emplace_back( os*vect3( ox-15,	oy+115,	0 )	);//8
			pHuman->tblJoint.emplace_back( os*vect3( ox-15,	oy+160,	0 )	);//9
			pHuman->tblJoint.emplace_back( os*vect3( ox+15,	oy+115,	0 )	);//10
			pHuman->tblJoint.emplace_back( os*vect3( ox+15,	oy+160,	0 )	);//11
			pHuman->tblJoint.emplace_back( os*vect3( ox-20,	oy+ 40,	0 )	);//12
			pHuman->tblJoint.emplace_back( os*vect3( ox-20,	oy+ 80,	0 )	);//13
			pHuman->tblJoint.emplace_back( os*vect3( ox+20,	oy+ 40,	0 )	);//14
			pHuman->tblJoint.emplace_back( os*vect3( ox+20,	oy+ 80,	0 )	);//15

		 	pHuman->tblBone.emplace_back( pHuman->tblJoint, 0, 1 );	//head
			pHuman->tblBone.emplace_back( pHuman->tblJoint, 1, 2 );
			pHuman->tblBone.emplace_back( pHuman->tblJoint, 2, 0 );
			pHuman->tblBone.emplace_back( pHuman->tblJoint, 2, 5 ); //chest
			pHuman->tblBone.emplace_back( pHuman->tblJoint, 5, 3 );
			pHuman->tblBone.emplace_back( pHuman->tblJoint, 3, 2 );
			pHuman->tblBone.emplace_back( pHuman->tblJoint, 2, 4 );
			pHuman->tblBone.emplace_back( pHuman->tblJoint, 4, 5 );
			pHuman->tblBone.emplace_back( pHuman->tblJoint, 5, 7 ); //hip
			pHuman->tblBone.emplace_back( pHuman->tblJoint, 7, 6 );
			pHuman->tblBone.emplace_back( pHuman->tblJoint, 6, 5 );
			pHuman->tblBone.emplace_back( pHuman->tblJoint, 6, 8 ); //leg
			pHuman->tblBone.emplace_back( pHuman->tblJoint, 8, 9 );
			pHuman->tblBone.emplace_back( pHuman->tblJoint, 7,10 ); //leg
			pHuman->tblBone.emplace_back( pHuman->tblJoint,10,11 );
			pHuman->tblBone.emplace_back( pHuman->tblJoint, 3,12 ); //arm
			pHuman->tblBone.emplace_back( pHuman->tblJoint,12,13 );
			pHuman->tblBone.emplace_back( pHuman->tblJoint, 4,14 ); //arm
			pHuman->tblBone.emplace_back( pHuman->tblJoint,14,15 );
		}
		for ( Bone3& b : pHuman->tblBone )	// 関節の距離を決定する。
		{
			b.length = (b.j1.pos - b.j0.pos).length();
		}
		{
			int cnt = 0 ;
			for ( Joint3& j : pHuman->tblJoint )
			{
				j.id = cnt++;				//id登録
			}
		}
		for ( Bone3& b : pHuman->tblBone )	// ジョイントに関節の距離を決定する。
		{
			b.j1.relative.emplace_back( b.j0 ); 
			b.j0.relative.emplace_back( b.j1 ); 
		}

		for ( Joint3& j : pHuman->tblJoint )	//マーカー登録
		{
			mc.tblMarker3.emplace_back( gra, figCircle, j, rad(-90) );
		}


		
		Data* pData = pHuman;



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

			{
				gra.Print(vect2(100,16*1),string("[S]ave/[L]oad mot"));
				// キーフレームロード
				if ( keys.L.hi )
				{
					pData = bone_load( "human.mot");
				}

				// キーフレームセーブ
				if ( keys.S.hi )
				{
					bone_save( *pData, "human.mot" );
				}

				// キーフレーム記録
				if ( keys.K.hi )
				{
					bone_AddKeyframe( *pData );
				}

				// キーフレーム移動
				if ( keys.UP.rep )
				{
					bone_ChangeKeyframeUp( *pData );
				}

				if ( keys.DOWN.rep )
				{
					bone_ChangeKeyframeDown( *pData );
				}
			}

			gra.Print(vect2(10,16*1),string("[Y]/[H] fovY:")+to_string(int(pers.fovy)));
			gra.Print( vect2(10,16*2),string("far:")+to_string((cam.pos-cam.at).length())); 
			gra.Print( vect2(10,16*3),string("at x=")+to_string(cam.at.x)+string("y=")+to_string(cam.at.y)+string(" z=")+to_string(cam.at.z) ); 
			gra.Print( vect2(10,16*4),string("key=")+to_string(numKeyframe) + string(" cnt=")+to_string(pData->tblKeyframe.size()) ); 
			gra.Print( vect2(10,16*31),string("peak=")+to_string(time_peak/1000)+string("msec") ); 

			if ( !keys.ALT.on )
			{

				// マウスホイールZOOM
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

				// カメラ回転
				if ( mouse.R.on )
				{
					double l = (cam.pos-cam.at).length()/10;
					l=max(l,0.00001);
					l=min(l,8);

					// 回転
					vect3	v= vect3(-mouse.mov.x/28,-mouse.mov.y/28,0) * l;
					mat44 mrot = cam.mat;
					mrot.SetTranslate(vect3(0,0,0));
					v = v* mrot;

					cam.pos += v;
				}

				// カメラ平行移動
				if ( mouse.M.on )
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
	
			}
			// カメラ移動
			if ( keys.ALT.on )
			{

				if ( mouse.L.on )
				{
					double l = (cam.pos-cam.at).length()/10;
					l=max(l,0.01);
					l=min(l,8);

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

					for ( int n = -1 ; n < static_cast<signed>(catmull_tbl.size())-3+1 ; n++ )
					{
						int n0 = n;
						int n1 = n+1;
						int n2 = n+2;
						int n3 = n+3;
						if ( n0 < 0 ) n0 = 0;
						if ( n3 >= static_cast<signed>(catmull_tbl.size()) ) n3 = n2;
					
						double t = st;
						vect2 v0 = catmull_func(0, catmull_tbl[n0], catmull_tbl[n1], catmull_tbl[n2], catmull_tbl[n3] );
						for ( int i = 0 ; i <div ; i++ )
						{
							vect2 v1 = catmull_func(t, catmull_tbl[n0], catmull_tbl[n1], catmull_tbl[n2], catmull_tbl[n3] );
							gra.Line( v1, v0, rgb(1,1,1));
							gra.Fill( v1-1,v1+2, rgb(1,1,1));
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

					for ( int n = 0 ; n < static_cast<signed>(bezier_tbl.size())-3 ; n+=3 )
					{
						double t  = st;
						vect2 v0 = bezier_tbl[n+0];
						for ( int i = 0 ; i < div ; i++ )
						{
							vect2 v1 = bezier_func( t, bezier_tbl[n+0], bezier_tbl[n+1], bezier_tbl[n+2], bezier_tbl[n+3] );
							gra.Line( v0,v1, rgb(1,1,1));
							gra.Fill( v1-1,v1+2, rgb(1,1,1));
							v0=v1;
							t+=st;
						}
					}

				}

				{// 補助ライン描画
					int cnt = 0;
					vect2 v0 = bezier_tbl[0];
					for ( int i = 1 ; i < static_cast<signed>(bezier_tbl.size()) ; i++ )
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

//				gra.Circle( gv1, 5,rgb(0,0,1));
				gra.Fill( gv1-4, gv1+4, rgb(1,1,1));

				if ( dir ) t+=0.01; else t-=0.01;


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
				if ( n3>=static_cast<signed>(catmull_tbl.size()) ) n3 =n2;

				gv2 = catmull_func( t, catmull_tbl[n0], catmull_tbl[n1], catmull_tbl[n2], catmull_tbl[n3] );

//				gra.Circle( gv2, 5,rgb(0,0,1));
				gra.Fill( gv2-4, gv2+4, rgb(1,1,1));

				if ( dir ) t+=0.01; else t-=0.01;


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
			mc.funcMarkerDraw2();


			//-----------------------------------------------------------------
			// 3Dマーカー入力
			//-----------------------------------------------------------------
//static Joint3* pTar3 = 0;

//			mc.funcMarkerController3( figCircle, mouse, keys, gra );
			if  ( !keys.ALT.on ) 
			{
				static vect2 drag_start(0,0);
				static bool bDrag = false;

				// マーカー追加
				if ( keys.M.hi )
				{
				//	mc.tblMarker3.emplace_back( gra, fig, mouse.pos, rad(0) );
				}

				// マーカー削除
				if  ( keys.CTRL.on && keys.X.hi )
				{
					for ( Marker3& m : mc.tblMarker3 )
					{
						if ( m.bSelected )
						{
							m.bAffectable = true;
						}
					}

					for ( int i = static_cast<signed>(mc.tblMarker3.size())-1 ; i >= 0 ; i-- )
					{
						if ( mc.tblMarker3[i].bAffectable )
						{
								   mc.tblMarker3.erase(mc.tblMarker3.begin() +i);	
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
					for ( Marker3& m : mc.tblMarker3 )
					{
						double len = (m.joint.readonly_disp2-mouse.pos).length();
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
							for ( Marker3& m : mc.tblMarker3 )
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
//pTar3 = &a.pmark->joint;
							#if 0
								// 優先度つけ
								for ( Joint3& j : data.tblJoint )
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
								
								funcSetPriority( a.pmark->joint, 1 );
							#endif

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

							for ( Marker3& m : mc.tblMarker3 )
							{
								m.bRectIn = false;
							}

							// 矩形内マーカーを検索
							for ( Marker3& m : mc.tblMarker3 )
							{
								double len = (m.joint.readonly_disp2-mouse.pos).length();
								if ( m.joint.disp.x > v0.x && m.joint.disp.x < v1.x && m.joint.disp.y > v0.y && m.joint.disp.y < v1.y )
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
						// human マーカー移動
						{
							// マウスの先のＺ位置を仮り決めする
							double aveZ = 0;
							int cnt = 0;
							for ( Marker3& m : mc.tblMarker3 )
							{
								if ( m.bSelected )
								{
									aveZ += 1/m.joint.disp.z;
									cnt++;
								}
							}
							aveZ /= cnt;
							if ( aveZ < 1 ) aveZ = 1;
							aveZ = 1/aveZ;

							// 移動
							{
								for ( Marker3& m : mc.tblMarker3 )
								{
									if ( m.bSelected )
									{
										// 平行移動
		//								double sz = 1/tan(rad(pers.fovy)/2);				// 投影面までの距離
										vect3 v = vect3(mouse.mov.x, mouse.mov.y, 0)/2.45/pers.height/aveZ;
										mat44 mrot = cam.mat;
										mrot.SetTranslate(vect3(0,0,0));
										mrot.invers();
										v = v* mrot;
										m.joint.pos += v ;
									}
								}
							}
						}
					}
				}
				else
				{
					if ( bDrag )
					{
						bDrag = false;
						for ( Marker3& m : mc.tblMarker3 )
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

			
//if ( pTar3 ) gra.Circle( pTar3->readonly_disp2, 10, rgb(1,1,1 ));
			//=================================
			// Human
			//=================================
			bone_update( *pData );


			// human sマーカー表示
			//mc.funcMarkerDraw3();
			{
				int		cntAve=0;
				vect2	posAve=0;
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
						posAve +=  m.joint.readonly_disp2;
					}
					gra.Circle( m.joint.readonly_disp2, 7, col );
//					gra.Print( m.joint.readonly_disp2+vect2(10,0), to_string(m.joint.priority) );
					gra.Print( m.joint.readonly_disp2+vect2(10,0), to_string(m.joint.id) );
		
				}

				if ( cntAve >= 2 )
				{
					posAve /= cntAve;
					//gra.Circle( posAve, 10, rgb(0,1,0));

				}
				
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




	

