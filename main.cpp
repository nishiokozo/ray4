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

	long long	time_peak = 0;

	struct	Obj
	{
		bool 	bRectSelected	= false;		//	矩形選択中、選択＆非選択
		bool 	bRectIn			= false;		//	矩形選択中、矩形選択対象
		bool 	bSelected		= false;		//	選択
		bool 	bAffectable		= false;		//	削除対象

		vect2	tag;
	
		virtual void Move2( vect2 v ) =0;// {};
		virtual vect2 Pos2() =0;// {};
	};

	struct Joint3 : Obj
	{
		int id;
		vect3 pos;
		vect3 tension;
		vect3 world;
		vect3 disp;
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
		void Move2( vect2 v )
		{
			//none
		}
		vect2 Pos2()
		{
			return vect2( disp.x, disp.y );
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

	struct Marker
	{
		const SysGra&	gra;
		const Figure&	fig;
		Obj&	obj;
		int		colNormal = rgb(1,1,0);
		int		colSelected = rgb(1,0,0);

		Marker( SysGra& _gra, Figure& _fig, Obj& _obj ) : gra(_gra), fig(_fig), obj(_obj)//, pos2(v)
		{
			obj.bSelected		= false;
			obj.bRectIn			= false;
			obj.bRectSelected	= false;
			obj.bAffectable		= false;
		}
		Marker(const Marker& a) : gra(a.gra), fig(a.fig), obj(a.obj)//, pos2(a.pos2)
		{
			obj.bSelected		= a.obj.bSelected;
			obj.bRectIn			= a.obj.bRectIn;
			obj.bRectSelected	= a.obj.bRectSelected;
			obj.bAffectable 	= a.obj.bAffectable;
		}	
		const Marker&	operator=(Marker&& a){return a;}	
		void draw()
		{
			bool flg =  obj.bSelected;
			
			if ( obj.bRectIn )
			{
				flg = obj.bRectSelected;
			}
			
			if ( flg )			
			{
				fig.draw( obj.Pos2(),rad(0), colSelected );
			}
			else
			{
				fig.draw( obj.Pos2(),rad(0), colNormal );
			}
		
		}

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

		vector<Marker>	tblMarker2;
		//------------------------------------------------------------------------------
		void funcMarkerDraw2()
		//------------------------------------------------------------------------------
		{
			for ( Marker& m : tblMarker2 )
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
//				tblMarker2.emplace_back( gra, fig, mouse.pos, rad(0) );
			}


			// マーカー削除
			if  ( keys.CTRL.on && keys.X.hi )
			{
				for ( Marker& m : tblMarker2 )
				{
					if ( m.obj.bSelected )
					{
						m.obj.bAffectable = true;
					}
				}

				for ( int i = static_cast<signed>(tblMarker2.size())-1 ; i >= 0 ; i-- )
				{
					if ( tblMarker2[i].obj.bAffectable )
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
					Marker*	pm;
					int		cnt;
				} a = {99999,0,0};

				// 最近マーカーを検索
				for ( Marker& m : tblMarker2 )
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
						for ( Marker& m : tblMarker2 )
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

						for ( Marker& m : tblMarker2 )
						{
							m.obj.bRectIn = false;
						}

						// 矩形内マーカーを検索
						for ( Marker& m : tblMarker2 )
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
					for ( Marker& m : tblMarker2 )
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
					for ( Marker& m : tblMarker2 )
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

		vector<Marker>	tblMarker3;

	} mc;

	struct Data
	{
		vector<Joint3>				tblJoint;
		vector<Bone3>				tblBone;

		struct Pos
		{
			vector<vect3>		pos;
		};
		struct Keyframe
		{
			vector<Pos>			pose;
		};
		vector<Keyframe>		animations;
	};

	struct
	{
		int	num = 0;	//	アニメーションカーソル位置
		int	pose = 0;	//	キーフレームカーソル位置
		int copied_num = 0;
		int copied_key = 0;
//		unique_ptr<Data> pCopybuf(new Data);
		unique_ptr<Data> pCopybuf;

		//
		double	t = 0;
		double	dt = 0.1;
		int n = 0;
		bool	bForward = true;
		bool	bPlaying = false;
	} anim;

	Figure figCircle=Figure(gra);

	// カメラ
	struct
	{
		vect3	pos = vect3( -2,-2, -2+0.1 );
		vect3 	at = vect3( 0, -1, 0 );
		vect3	up = vect3( 0, 1, 0);
	  	mat44	mat;		
	} cam ;

	struct Pers
	{
		double	fovy;		// 画角
		double	sz;			// 投影面までの距離
		double	sc;			// 投影面の高さ/2（描画スケール）
		double	cx;			// 描画画面の中心W
		double	cy;			// 描画画面の中心H
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
		#if 0
			sc = 1;									// 投影面の高さ/2
			sz = sc/tan(rad(fovy)/2);				// 投影面までの距離
		#else
			sz = 1.0;								// 投影面までの距離
			sc = sz*tan(rad(fovy)/2);				// 投影面の高さ/2
		#endif
			cx		= screensize.x/2;				// 描画画面の中心W
			cy		= screensize.y/2;				// 描画画面の中心H
			width	= screensize.x/2;				// 描画画面の解像度W/2
			height	= screensize.y/2;				// 描画画面の解像度H/2
			aspect	= screensize.y/screensize.x;	// 描画画面のアスペクト比
		} 

		//--------------------------------------------------------------------------
		vect3 calcPoint( vect3 v ) const
		//--------------------------------------------------------------------------
		{
			vect3 ret;
			double w = 1/(v.z+sz);
			ret.x = v.x/(v.z+sz)	*sz /sc *width  *aspect	+cx;
			ret.y = v.y/(v.z+sz)	*sz /sc *height			+cy;
			ret.z = w;
			return ret;
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



	};
	Pers pers;

	// カトマル曲線3D
	//------------------------------------------------------------------------------
	vect3 catmull3d_func( double t, const vect3 P0, const vect3 P1, const vect3 P2, const vect3 P3 )
	//------------------------------------------------------------------------------
	{
		//catmull-Rom 曲線
		// P(t)=P1*(2t^3-3t^2+1)+m0*(t^3-2t^2+t)+P2*(-2t^3+3t^2)+m1*(t^3-t^2)
		// m0=(P2-P0)/2
		// m1=(P3-P1)/2

		vect3 m0 = (P2-P0)/2.0;
		vect3 m1 = (P3-P1)/2.0;
		vect3 P =  P1*(  2*t*t*t - 3*t*t +1) + m0*( t*t*t -2*t*t +t )
				 + P2*( -2*t*t*t + 3*t*t   ) + m1*( t*t*t - t*t );

		return P;
	};

	//------------------------------------------------------------------------------
	void bone_ReqAnimation()
	//------------------------------------------------------------------------------
	{
		anim.bForward = true;
		anim.bPlaying = true;
		anim.n = 0;
		anim.t = 0;
		anim.dt = 0.1;
	}	

	//------------------------------------------------------------------------------
	void bone_Play( Data& data )
	//------------------------------------------------------------------------------
	{
		int num = anim.num;
		if(static_cast<signed>(data.animations[num].pose.size())<4) return;

		int n0 = anim.n-1;
		int n1 = anim.n;
		int n2 = anim.n+1;
		int n3 = anim.n+2;
		if ( n0<0 ) n0 = 0;
		if ( n3>=static_cast<signed>(data.animations[num].pose.size()) ) n3 =n2;

		for ( int j = 0 ; j < static_cast<signed>(data.animations[num].pose[0].pos.size()) ; j++ )
		{
/*
			if ( g_flg ) 
			{
				if ( j == 3 || j == 4 ) continue;	//肩
				if ( j == 5 || j == 2 ) continue;	//腹首
				if ( j == 6 || j == 7) continue;	//腰
	//			if ( j == 12 || j == 14 ) continue;	//肘
	//			if ( j == 8 || j == 10 ) continue;	//足
	//			if ( j == 9 || j == 11 ) continue;	//膝
			}
*/
			vect3 P0 = data.animations[num].pose[ n0 ].pos[j];
			vect3 P1 = data.animations[num].pose[ n1 ].pos[j];
			vect3 P2 = data.animations[num].pose[ n2 ].pos[j];
			vect3 P3 = data.animations[num].pose[ n3 ].pos[j];
			vect3 b = catmull3d_func(anim.t, P0,P1,P2,P3 );

			data.tblJoint[j].pos = b;

		}

		if ( anim.bForward ) anim.t+=anim.dt; else anim.t-=anim.dt;


		if ( anim.t >= 1.0 ) 
		{
			if ( anim.n+1 < static_cast<signed>(data.animations[num].pose.size())-1 )
			{
				anim.t = 0;
				anim.n+=1;
			}
			else
			{
				anim.t = 1.0;
				anim.bForward = !anim.bForward;
			}
		}
		else
		if ( anim.t <= 0.0 ) 
		{
			if ( anim.n >= 1 )
			{
				anim.t = 1.0;
				anim.n-=1;
			}
			else
			{
				anim.t = 0.0;
				anim.bForward = !anim.bForward;
			}
		}
		if ( anim.bForward == false ) anim.bPlaying = false;
	}
	
	//------------------------------------------------------------------------------
	void bone_PrevKeyframe( Data& data )
	//------------------------------------------------------------------------------
	{
		anim.bPlaying = false;

		int num = anim.num;
		if ( data.animations.size() ==0 ) return;
		if ( data.animations[num].pose.size()==0) return;
		if ( data.animations[num].pose[ 0 ].pos.size()==0 ) return;

		anim.pose--;
//		if ( anim.pose < 0 ) anim.pose = static_cast<signed>(data.animations[num].pose.size())-1;
		if ( anim.pose < 0 ) anim.pose = 0;

		if ( anim.pose >= 0 )
		{
			// キーフレーム切り替え
			int i = 0;
			for ( Joint3& j : data.tblJoint )
			{
				j.pos = data.animations[num].pose[ anim.pose ].pos[i];
				i++;
			}
		}
	}

	//------------------------------------------------------------------------------
	void bone_TopKeyframe( Data& data )
	//------------------------------------------------------------------------------
	{
		anim.bPlaying = false;

		int num = anim.num;
		if ( data.animations.size() ==0 ) return;
		if ( data.animations[num].pose.size()==0) return;
		if ( data.animations[num].pose[ 0 ].pos.size()==0 ) return;

		anim.pose = 0;

		if ( anim.pose >= 0 )
		{
			// キーフレーム切り替え
			int i = 0;
			for ( Joint3& j : data.tblJoint )
			{
				j.pos = data.animations[num].pose[ anim.pose ].pos[i];
				i++;
			}
		}
	}

	//------------------------------------------------------------------------------
	void bone_NextKeyframe( Data& data )
	//------------------------------------------------------------------------------
	{
		anim.bPlaying = false;

		int num = anim.num;
		if ( data.animations.size() ==0 ) return;
		if ( data.animations[num].pose.size()==0) return;
		if ( data.animations[num].pose[ 0 ].pos.size()==0 ) return;

		anim.pose++; 
//		if ( anim.pose > static_cast<signed>(data.animations[num].pose.size())-1 ) anim.pose = 0;
		if ( anim.pose > static_cast<signed>(data.animations[num].pose.size())-1 ) anim.pose = static_cast<signed>(data.animations[num].pose.size())-1;

		if ( anim.pose >= 0 )
		{
			// キーフレーム切り替え
			int i = 0;
			for ( Joint3& j : data.tblJoint )
			{
				j.pos = data.animations[num].pose[ anim.pose ].pos[i];
				i++;
			}
		}
	}

	//------------------------------------------------------------------------------
	void bone_LastKeyframe( Data& data )
	//------------------------------------------------------------------------------
	{
		anim.bPlaying = false;

		int num = anim.num;
		if ( data.animations.size() ==0 ) return;
		if ( data.animations[num].pose.size()==0) return;
		if ( data.animations[num].pose[ 0 ].pos.size()==0 ) return;

		anim.pose = static_cast<signed>(data.animations[num].pose.size())-1;

		if ( anim.pose >= 0 )
		{
			// キーフレーム切り替え
			int i = 0;
			for ( Joint3& j : data.tblJoint )
			{
				j.pos = data.animations[num].pose[ anim.pose ].pos[i];
				i++;
			}
		}
	}

	//------------------------------------------------------------------------------
	void bone_RefrectKeyframe( Data& data )
	//------------------------------------------------------------------------------
	{
		anim.bPlaying = false;

		int num = anim.num;
		if ( data.animations.size() ==0 ) return;
		if ( data.animations[num].pose.size()==0) return;
		if ( data.animations[num].pose[ 0 ].pos.size()==0 ) return;

		for ( int i = 0 ; i < static_cast<signed>(data.tblJoint.size()) ; i++ )
		{ 
			data.animations[num].pose[ anim.pose ].pos[i] = data.tblJoint[i].pos;
		}
	}
	
	//------------------------------------------------------------------------------
	void bone_InsertKeyframe( Data& data )
	//------------------------------------------------------------------------------
	{
		anim.bPlaying = false;

		int num = anim.num;

		data.animations[num].pose.emplace( data.animations[num].pose.begin() + anim.pose );
		for ( const Joint3& j : data.tblJoint )
		{
			data.animations[num].pose[ anim.pose ].pos.emplace_back( j.pos );
		}
	}

	//------------------------------------------------------------------------------
	void bone_CopyKeyframe( Data& data )
	//------------------------------------------------------------------------------
	{

/*
	struct Data
	{
		vector<Joint3>				tblJoint;
		vector<Bone3>				tblBone;

		struct Pos
		{
			vector<vect3>		pos;
		};
		struct Keyframe
		{
			vector<Pos>			pose;
		};
		vector<Keyframe>		animations;
	};
*/
		unique_ptr<Data> pNew(new Data);
		{
			for ( Joint3& j : data.tblJoint )
			{
				pNew->tblJoint.emplace_back( j.pos );
			}

			for ( Bone3& b : data.tblBone )
			{
				pNew->tblBone.emplace_back( pNew->tblJoint, b.n0, b.n1 );
			}
		}
		{
			int	cntAction = static_cast<signed>(data.animations.size());
			for ( int num = 0 ; num < cntAction ; num++ )
			{
				pNew->animations.emplace_back();

				int	cntPose = static_cast<signed>(data.animations[num].pose.size());
				for ( int pose = 0 ; pose < cntPose ; pose++ )
				{
					pNew->animations[num].pose.emplace_back();

					for ( int j = 0 ; j < static_cast<signed>(data.tblJoint.size()) ; j++ )
					{
						vect3 pos = data.animations[num].pose[pose].pos[j];
						pNew->animations[num].pose[pose].pos.emplace_back( pos );
					}
				}
			}
		}
		anim.pCopybuf = move(pNew);
		anim.copied_num = anim.num;
		anim.copied_key = anim.pose;
	}

	//------------------------------------------------------------------------------
	void bone_PastKeyframe( Data& data )
	//------------------------------------------------------------------------------
	{
		bone_InsertKeyframe( data );
		for ( int i = 0 ; i < static_cast<signed>(data.tblJoint.size()) ; i++ )
		{ 
//			vect3 v = data.animations[anim.copied_num].pose[ anim.copied_key ].pos[i];
			vect3 v = anim.pCopybuf->animations[anim.copied_num].pose[ anim.copied_key ].pos[i];
		
			data.animations[anim.num].pose[ anim.pose ].pos[i] = v;
			data.tblJoint[i].pos = v;
		}

		
	}

	//------------------------------------------------------------------------------
	void bone_DeleteKeyframe( Data& data )
	//------------------------------------------------------------------------------
	{
		anim.bPlaying = false;

		int num = anim.num;
		if ( data.animations.size() ==0 ) return;
		if ( data.animations[num].pose.size()==0) return;
		if ( data.animations[num].pose[ 0 ].pos.size()==0 ) return;

		data.animations[num].pose.erase(data.animations[num].pose.begin() +anim.pose );	

		if ( anim.pose >= (signed)data.animations[num].pose.size() ) anim.pose = (signed)data.animations[num].pose.size()-1;

	}

	//------------------------------------------------------------------------------
	void bone_PrevAnimation( Data& data )
	//------------------------------------------------------------------------------
	{
		anim.bPlaying = false;

		anim.num--;
		if ( anim.num < 0 ) anim.num = 0;

		int num = anim.num;
		if ( data.animations.size() ==0 ) return;
		if ( data.animations[num].pose.size()==0) return;
		if ( data.animations[num].pose[ 0 ].pos.size()==0 ) return;

		{
			int num = anim.num;
			anim.pose = 0;
			{
				// キーフレーム切り替え
				int i = 0;
				for ( Joint3& j : data.tblJoint )
				{
					j.pos = data.animations[num].pose[ anim.pose ].pos[i];
					i++;
				}
			}
		}
	}

	//------------------------------------------------------------------------------
	void bone_NextAnimation( Data& data )
	//------------------------------------------------------------------------------
	{
		anim.bPlaying = false;

		anim.num++; 
		if ( anim.num > static_cast<signed>(data.animations.size())-1 ) anim.num = static_cast<signed>(data.animations.size())-1;

		int num = anim.num;
		if ( data.animations.size() ==0 ) return;
		if ( data.animations[num].pose.size()==0) return;
		if ( data.animations[num].pose[ 0 ].pos.size()==0 ) return;

		{
			anim.pose = 0;

			{
				// キーフレーム切り替え
				int i = 0;
				for ( Joint3& j : data.tblJoint )
				{
				
					j.pos = data.animations[num].pose[ anim.pose ].pos[i];
					i++;
				}
			}
		}
	}

	//------------------------------------------------------------------------------
	void bone_AddAnimation( Data& data )
	//------------------------------------------------------------------------------
	{
		anim.bPlaying = false;

		anim.num = static_cast<signed>(data.animations.size());
		anim.pose = 0;
		data.animations.emplace_back();

		{
			int num = anim.num;

			data.animations[num].pose.emplace_back();
			for ( const Joint3& j : data.tblJoint )
			{
				data.animations[num].pose[ anim.pose ].pos.emplace_back( j.pos );
			}
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
		anim.bPlaying = false;

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
			int	cntAction = static_cast<signed>(data.animations.size());
			for ( int num = 0 ; num < cntAction ; num++ )
			{
				fo << "motion" << endl;
				int	cntPose = static_cast<signed>(data.animations[num].pose.size());
				for ( int pose = 0 ; pose < cntPose ; pose++ )
				{
					for ( int j = 0 ; j < static_cast<signed>(data.tblJoint.size()) ; j++ )
					{
						fo  << "\t"<< data.animations[num].pose[ pose ].pos[ j ].x << "\t" << data.animations[num].pose[ pose ].pos[ j ].y << "\t" << data.animations[num].pose[ pose ].pos[ j ].z << endl;
					}
					if( pose+1 < cntPose ) fo << "," << endl;
				}
			}
		}
		fo << "end" << endl;

		cout << "SAVED" << endl;
	}
	//------------------------------------------------------------------------------
	void bone_loadX( Data& data, const char* filename )
	//------------------------------------------------------------------------------
	{
		anim.bPlaying = false;


	//	Data*	pNew = new Data;

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

		int num = 0;

		while ( getline( fi, buf ) )
		{
	 		if( *(buf.end()-1) == 0x0d ) buf.pop_back();//CRLF対応

			cout << buf << endl;
			if ( string(buf) == "joint" )	{mode = ModeJoint;	continue;}
			if ( string(buf) == "bone" )	{mode = ModeBone;	continue;}
			if ( string(buf) == "motion" )	
			{
				mode = ModeMotion;
				num = static_cast<signed>(data.animations.size());
				data.animations.emplace_back();
				data.animations[num].pose.emplace_back();
				continue;
			}
			if ( string(buf) == "," ) 
			{
				data.animations[num].pose.emplace_back();
				continue;
			}
			if ( string(buf) == "end" )	
			{	
				for ( Bone3& b : data.tblBone )	// 関節の距離を決定する。
				{
					b.length = (b.j1.pos - b.j0.pos).length();
				}
				{
					int cnt = 0 ;
					for ( Joint3& j : data.tblJoint )
					{
						j.id = cnt++;				//id登録
					}
				}
				for ( Bone3& b : data.tblBone )	// ジョイントに関節の距離を決定する。
				{
					b.j1.relative.emplace_back( b.j0 ); 
					b.j0.relative.emplace_back( b.j1 ); 
				}
				//マーカー削除＆登録
				mc.tblMarker3.clear();
				for ( Joint3& j : data.tblJoint )	//マーカー登録
				{
					mc.tblMarker3.emplace_back( gra, figCircle, j );
				}
				anim.pose = 0;
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
						data.tblJoint.emplace_back( vect3(x,y,z) );
						//	cout << x << "," << y << "," << z << endl; 
					}
					break;
				case ModeBone:
					{
						vector<string> v = split( buf, '\t');
						int n0 = stoi(v[0]);
						int n1 = stoi(v[1]);
						data.tblBone.emplace_back( data.tblJoint, n0, n1 );
						//	cout << x << "," << y << "," << z << endl; 
					}
					break;
				case ModeMotion:
					{
						vector<string> v = split( buf, '\t');
						double x = stod(v[0]);
						double y = stod(v[1]);
						double z = stod(v[2]);
						data.animations[num].pose[ data.animations[num].pose.size()-1 ].pos.emplace_back( x,y,z );
						//	cout << x << "," << y << "," << z << endl; 
					}
					break;
				default:
					break;
			}

		}
		cout << "LOADED" << endl;
	//	return pNew;
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
			mc.tblMarker2.emplace_back( gra, figCircle, c );
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
			mc.tblMarker2.emplace_back( gra, figCircle, b );
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
			mc.tblMarker2.emplace_back( gra, figCircle, j );
		}

		//人

#if 0 
		Data* pPreset = new Data;
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
			mc.tblMarker3.emplace_back( gra, figCircle, j );
		}

		pPreset->animations.emplace_back();

		Data* pData = pPreset;

#else
		unique_ptr<Data> pData(new Data);
		bone_loadX( *pData, "primary.mot");
#endif

		{
			unique_ptr<Data> p(new Data);
			anim.pCopybuf = move(p);
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

				
				// キーフレームロード
//				if ( keys.L.hi ) pData = bone_load( "human.mot");
				if ( keys.L.hi ) 
				{
					unique_ptr<Data> pNew(new Data);
					bone_loadX( *pNew, "human.mot");
					pData = move(pNew);
				}


				// キーフレームセーブ
				if ( keys.S.hi ) bone_save( *pData, "human.mot" );

				// キーフレームへ反映
				if ( keys.J.hi ) bone_RefrectKeyframe( *pData );

				// キーフレームペースト
				if ( keys.V.hi ) bone_PastKeyframe( *pData );

				// キーフレームコピー
				if ( keys.C.hi ) bone_CopyKeyframe( *pData );

				// キーフレーム追加
				if ( keys.K.hi ) bone_InsertKeyframe( *pData );

				// キーフレーム削除
				if ( keys.D.hi ) bone_DeleteKeyframe( *pData );

				// キーフレーム次
				if ( keys.RIGHT.rep ) bone_NextKeyframe( *pData );

				// キーフレーム最後
				if ( keys.CTRL.on && keys.RIGHT.rep ) bone_LastKeyframe( *pData );

				// キーフレーム前
				if ( keys.LEFT.rep ) bone_PrevKeyframe( *pData );

				// キーフレーム先頭
				if ( keys.CTRL.on && keys.LEFT.rep ) bone_TopKeyframe( *pData );

				// アニメーション記録
				if ( keys.I.hi ) bone_AddAnimation( *pData );

				// アニメーション前
				if ( keys.UP.rep ) bone_PrevAnimation( *pData );

				// アニメーション次
				if ( keys.DOWN.rep ) bone_NextAnimation( *pData );

				// アニメーションリクエスト
				if ( keys.P.hi ) bone_ReqAnimation();

//if ( keys.Q.hi ) g_flg =!g_flg;
//gra.Print(vect2(16*20,16*1),string("flg ")+ to_string(g_flg) );

				// アニメーション再生
				if ( anim.bPlaying )	bone_Play( *pData );
				
				
			}

			{
				int y = 1;
				gra.Print(vect2(10,16*y++),string("fovY:")+to_string(int(pers.fovy)));
				gra.Print(vect2(10,16*y++),string("sz:")+to_string(pers.sz) +string(" sc:")+to_string(pers.sc));
				gra.Print( vect2(10,16*y++),string("far:")+to_string((cam.pos-cam.at).length())); 
				gra.Print( vect2(10,16*y++),string("at  x=")+to_string(cam.at.x)+string(" y=")+to_string(cam.at.y)+string(" z=")+to_string(cam.at.z) ); 
				gra.Print( vect2(10,16*y++),string("pos x=")+to_string(cam.pos.x)+string(" y=")+to_string(cam.pos.y)+string(" z=")+to_string(cam.pos.z) ); 
				{
					int num = anim.num;
					gra.Print( vect2(10,16*y++),string("anim=")+to_string(num) + string(" cnt=")+to_string(pData->animations.size()) ); 
					if ( pData->animations.size() > 0 ) 
					{
						gra.Print( vect2(10,16*y++),string("pose=")+to_string(anim.pose) + string(" cnt=")+to_string(pData->animations[num].pose.size()) ); 
					}
				}
				gra.Print( vect2(10,16*31),string("peak=")+to_string(time_peak/1000)+string("msec") ); 
			}

			// animカーソルビュー cursor
			{
				for ( int y = 0 ; y < (signed)pData->animations.size() ; y++ )
				{
					for ( int x = 0 ; x < (signed)pData->animations[y].pose.size() ; x++ )
					{
						vect2 v = vect2( x, y )*vect2( 4, 8 ) + vect2(400,16);

						{
							gra.Fill( v, v+vect2(3,7), rgb(1,1,1) );
						}

						if ( y == anim.num && x == anim.pose )
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


			// 3Dマーカー入力
			if  ( !keys.ALT.on ) //marker3_input();
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
					for ( Marker& m : mc.tblMarker3 )
					{
						if ( m.obj.bSelected )
						{
							m.obj.bAffectable = true;
						}
					}

					for ( int i = static_cast<signed>(mc.tblMarker3.size())-1 ; i >= 0 ; i-- )
					{
						if ( mc.tblMarker3[i].obj.bAffectable )
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
						Marker*	pm;
						int		cnt;
					} a = {99999,0,0};

					// 最近マーカーを検索
					for ( Marker& m : mc.tblMarker3 )
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
							bDrag = true;
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
							for ( Marker& m : mc.tblMarker3 )
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
		//pTar3 = &a.pm->joint;
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
								
								funcSetPriority( a.pm->joint, 1 );
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

							for ( Marker& m : mc.tblMarker3 )
							{
								m.obj.bRectIn = false;
							}

							// 矩形内マーカーを検索
							for ( Marker& m : mc.tblMarker3 )
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
						// human マーカー移動
						{
							{
							
								// マウスの先のＺ位置を仮り決めする
								double aveZ = 0;
								int cnt = 0;
								for ( Marker& m : mc.tblMarker3 )
								{
									Joint3* pj = dynamic_cast<Joint3*>(&m.obj);
									if ( pj->bSelected )
									{
										aveZ += 1/pj->disp.z;
										cnt++;
									}
								}
								aveZ /= cnt;
								if ( aveZ < 1 ) aveZ = 1;
								aveZ = 1/aveZ;

								// 移動
								{
									for ( Marker& m : mc.tblMarker3 )
									{
										Joint3* pj = dynamic_cast<Joint3*>(&m.obj);
										if ( pj->bSelected )
										{
											// 平行移動
			//								double sz = 1/tan(rad(pers.fovy)/2);				// 投影面までの距離
											vect3 v = vect3(mouse.mov.x, mouse.mov.y, 0)/2.45/pers.height/aveZ;
											mat44 mrot = cam.mat;
											mrot.SetTranslate(vect3(0,0,0));
											mrot.invers();
											v = v* mrot;
											pj->pos += v ;
										}
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
						for ( Marker& m : mc.tblMarker3 )
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
			// human 更新
			bone_update( *pData );

			// 3Dマーカー表示
			for ( Marker m : mc.tblMarker3 )
			{
				m.draw();
			}
/*
if(0)
			{
				int		cntAve=0;
				vect2	posAve=0;
				for ( Marker m : mc.tblMarker3 )
				{
						//m.draw();
					bool flg =  m.obj.bSelected;
					
					if ( m.obj.bRectIn )
					{
						flg = m.obj.bRectSelected;
					}
					int col=m.colNormal;

					vect2 disp2 = m.obj.Pos2();//vect2(m.obj.disp.x, m.obj.disp.y);
					if ( flg )			
					{
						col = m.colSelected;
						cntAve++;
						posAve +=  disp2;
					}
					gra.Circle( disp2, 7, col );
					gra.Print( disp2+vect2(10,0), "id" );//to_string(m.obj.id) );

				}

				if ( cntAve >= 2 )
				{
					posAve /= cntAve;
				}
			}
*/

			// カトマル3D モーション軌跡表示
			if(1)
			{

				int num = anim.num;
				Data& data = (*pData);
				// マーカースプライン変換表示
				if ( static_cast<signed>(data.animations.size()) > 0 )
				{
					double dt = anim.dt;
					double div = 1/dt;

					for ( int n = -1 ; n < static_cast<signed>(data.animations[num].pose.size())-3+1 ; n++ )
					{
						int n0 = n;
						int n1 = n+1;
						int n2 = n+2;
						int n3 = n+3;
						if ( n0 < 0 ) n0 = 0;
						if ( n3 >= static_cast<signed>(data.animations[num].pose.size()) ) n3 = n2;
					
						for ( int j = 0 ;  j < static_cast<signed>(data.tblJoint.size()) ; j++ )
						{
							if ( data.tblJoint[j].bSelected == false ) continue;
						
							vect3 P0 = data.animations[num].pose[ n0 ].pos[j];
							vect3 P1 = data.animations[num].pose[ n1 ].pos[j];
							vect3 P2 = data.animations[num].pose[ n2 ].pos[j];
							vect3 P3 = data.animations[num].pose[ n3 ].pos[j];

							double t = dt;
							vect3 a = catmull3d_func(0, P0,P1,P2,P3 );
							for ( int i = 0 ; i <div ; i++ )
							{
								vect3 b = catmull3d_func(t, P0,P1,P2,P3 );

								vect3 v0;
								vect3 v1;
								bool flg = pers.ScissorLine( a* cam.mat.invers(), b* cam.mat.invers(), v0, v1 );

								if ( flg )
								{
									gra.Line( vect2(v0.x,v0.y), vect2(v1.x,v1.y), rgb(0,0,1));
								}
								
								if ( v1.z > 0 ) gra.Fill(vect2(v1.x,v1.y)-2, vect2(v1.x,v1.y)+3,rgb(0,0,1));

								a=b;
								t+=dt;

							}	
						}
					}
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




	

