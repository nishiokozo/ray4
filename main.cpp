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
#include <algorithm>
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


//	struct PinAxis : Obj
//	{
//		
//		vect3 disp;
//		void Move2( vect2 v )
//		{
//		}
//		vect2 Pos2()
//		{
//			return vect2( disp.x, disp.y );
//		}
//		bool IsVisuable()
//		{
//			return true;
//		}
//	};
	


	struct Selector
	{


		vector<Marker>	tblMarker;
		vect2 rect_pos = vect2(0,0);			//	矩形選択開始位置
		bool rect_bSelect = false;	//	矩形選択中フラグ
		struct
		{
			float	len;
			Marker*	pm;
			int		cnt;
		} a = {99999,0,0};
		rgb		colNormal = vect3(0.8,0.8,0);
		rgb		colSelected = vect3(0.8,0,0);

		//---------------------------------------------------------------------
		void searchNear( vect2 mouse_pos )
		//---------------------------------------------------------------------
		{
			a.len = 9999;
			a.pm = 0;
			a.cnt = 0;

			// 最近マーカーを検索
			for ( Marker& m : tblMarker )
			{
				float len = (m.obj.Pos2()-mouse_pos).length();
				if ( len < 20.0f/512.0f && a.len > len )
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
				float len = (m.obj.Pos2()-mouse_pos).length();
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
				float len = (m.obj.Pos2()-mouse_pos).length();
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
						gra.Pset( m.obj.Pos2(), colSelected,9 );
					}
					else
					{
						gra.Pset( m.obj.Pos2(), colNormal,9 );
					}
				}
			}
		}
	

		
	};

	Selector selector;

	struct Manupirator
	{

		bool bAxisX = true;;
		bool bAxisY = true;;
		bool bAxisZ = true;;

		//------------------------------------------------------------------------------
		void manupirator_drawAxis( vect3 pos, Apr& apr )
		//------------------------------------------------------------------------------
		{
			vect3 v0 = apr.pers.calcDisp( pos * apr.pers.cam.mat.invers() );



			float l = 0.1;
			if ( bAxisX  )
			{
				vect3 v1 = vect3( l,0,0);
				mat44 mrot = apr.pers.cam.mat;
				mrot.SetTranslate(vect3(0,0,0));
				mrot.invers();
				v1 = v0 + v1* mrot;
				apr.gra.Line( vect2(v0.x,v0.y), vect2(v1.x,v1.y), rgb(0.8,0.2,0.2) );
			}
			if ( bAxisY  )
			{
				vect3 v1 = vect3( 0,l,0);
				mat44 mrot = apr.pers.cam.mat;
				mrot.SetTranslate(vect3(0,0,0));
				mrot.invers();
				v1 = v0 + v1* mrot;
				apr.gra.Line( vect2(v0.x,v0.y), vect2(v1.x,v1.y), rgb(0.2,0.8,0.2) );
			}
			if ( bAxisZ )
			{
				vect3 v1 = vect3( 0,0,l);
				mat44 mrot = apr.pers.cam.mat;
				mrot.SetTranslate(vect3(0,0,0));
				mrot.invers();
				v1 = v0 + v1* mrot;
				apr.gra.Line( vect2(v0.x,v0.y), vect2(v1.x,v1.y), rgb(0.2,0.2,1) );

			}


		}
	} manupirator;


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
	void circle3d_x( vect3 pos,  float r, rgb col )
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
	void circle3d_y( vect3 pos,  float r, rgb col )
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
	void circle3d_z( vect3 pos,  float r, rgb col )
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

	struct Grid
	{
		vect3	pos;
//		int		mode;
		int		NUM_U;
		int		NUM_V;
		float	dt;
		rgb		col;
	
		//------------------------------------------------------------------------------
		void SetMesh( vect3 _pos, int _NUM_U, int _NUM_V, float _dt, rgb _col )
		//------------------------------------------------------------------------------
		{	// ミニグリッド
			pos 	= _pos;
			NUM_U	= _NUM_U;
			NUM_V	= _NUM_V;
			dt		= _dt;
			col	 = _col;
		}
		//------------------------------------------------------------------------------
		void DrawMesh( Apr& apr )
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
		}
	};
	Grid gridGround;
	
	bool flgInfo = true;
	
	//------------------------------------------------------------------------------
	int main()
	//------------------------------------------------------------------------------
	{


		unique_ptr<Bone> pBone(new Bone);

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
//						selector.mode = Selector::MODE_3D;
					}
					pBone = move(pNew);
				}


		// 箱
		struct
		{
			float	rx = rad(0);
			float	ry = rad(0);
			float	rz = rad(0);

			vect3 pos = {3.5, 0.51,8.5};

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
					gra.Print(vect2(10,16*y++)/vect2(768/2,-512/2)+vect2(-1,1),string("[F1] help"));
					gra.Print(vect2(10,16*y++)/vect2(768/2,-512/2)+vect2(-1,1),string("[Y] pers -"));
					gra.Print(vect2(10,16*y++)/vect2(768/2,-512/2)+vect2(-1,1),string("[H] pers +"));
					gra.Print(vect2(10,16*y++)/vect2(768/2,-512/2)+vect2(-1,1),string("[L] Load"));
					gra.Print(vect2(10,16*y++)/vect2(768/2,-512/2)+vect2(-1,1),string("[S] Save"));
					gra.Print(vect2(10,16*y++)/vect2(768/2,-512/2)+vect2(-1,1),string("--Keyframe--"));
					gra.Print(vect2(10,16*y++)/vect2(768/2,-512/2)+vect2(-1,1),string("[K] Insert"));
					gra.Print(vect2(10,16*y++)/vect2(768/2,-512/2)+vect2(-1,1),string("[X] Cut"));
					gra.Print(vect2(10,16*y++)/vect2(768/2,-512/2)+vect2(-1,1),string("[C] Copy"));
					gra.Print(vect2(10,16*y++)/vect2(768/2,-512/2)+vect2(-1,1),string("[V] Past"));
					gra.Print(vect2(10,16*y++)/vect2(768/2,-512/2)+vect2(-1,1),string("[LEFT]  -"));
					gra.Print(vect2(10,16*y++)/vect2(768/2,-512/2)+vect2(-1,1),string("[RIGHT] +"));
					gra.Print(vect2(10,16*y++)/vect2(768/2,-512/2)+vect2(-1,1),string("--Animation--"));
					gra.Print(vect2(10,16*y++)/vect2(768/2,-512/2)+vect2(-1,1),string("[I] Add"));
					gra.Print(vect2(10,16*y++)/vect2(768/2,-512/2)+vect2(-1,1),string("[P] Play"));
					gra.Print(vect2(10,16*y++)/vect2(768/2,-512/2)+vect2(-1,1),string("[UP] -"));
					gra.Print(vect2(10,16*y++)/vect2(768/2,-512/2)+vect2(-1,1),string("[DOWN] +"));
					gra.Print(vect2(10,16*y++)/vect2(768/2,-512/2)+vect2(-1,1),string("--Other--"));
					gra.Print(vect2(10,16*y++)/vect2(768/2,-512/2)+vect2(-1,1),string("[1] select 3d"));
					gra.Print(vect2(10,16*y++)/vect2(768/2,-512/2)+vect2(-1,1),string("[2] select 2main"));
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
				if ( keys.Z.hi ) {manupirator.bAxisZ = true;	manupirator.bAxisX = false;	manupirator.bAxisY = false;}
				if ( keys.X.hi ) {manupirator.bAxisZ = false;	manupirator.bAxisX = true;	manupirator.bAxisY = false;}
				if ( keys.C.hi ) {manupirator.bAxisZ = false;	manupirator.bAxisX = false;	manupirator.bAxisY = true;}
				if ( keys.V.hi ) {manupirator.bAxisZ = true;	manupirator.bAxisX = true;	manupirator.bAxisY = true;}
			}


			{
				if( keys.F2.hi ) flgInfo = !flgInfo;
				if ( flgInfo )
				{
				int y = 1;
					gra.Print( vect2(10,16*y++)/vect2(768/2,-512/2)+vect2(-1,1),string("fovY:")+to_string(int(pers.fovy)));
					gra.Print( vect2(10,16*y++)/vect2(768/2,-512/2)+vect2(-1,1),string("sz:")+to_string(pers.sz) +string(" fy:")+to_string(pers.fy));
					gra.Print( vect2(10,16*y++)/vect2(768/2,-512/2)+vect2(-1,1),string("far:")+to_string((pers.cam.pos-pers.cam.at).length())); 
					gra.Print( vect2(10,16*y++)/vect2(768/2,-512/2)+vect2(-1,1),string("at  x=")+to_string(pers.cam.at.x)+string(" y=")+to_string(pers.cam.at.y)+string(" z=")+to_string(pers.cam.at.z) ); 
					gra.Print( vect2(10,16*y++)/vect2(768/2,-512/2)+vect2(-1,1),string("pos x=")+to_string(pers.cam.pos.x)+string(" y=")+to_string(pers.cam.pos.y)+string(" z=")+to_string(pers.cam.pos.z) ); 
					gra.Print( vect2(10,16*y++)/vect2(768/2,-512/2)+vect2(-1,1),string("anim=")+to_string(pBone->cur.act) + string(" cnt=")+to_string(pBone->animations.size()) ); 
					if ( pBone->animations.size() > 0 ) 
					{
						gra.Print( vect2(10,16*y++)/vect2(768/2,-512/2)+vect2(-1,1),string("pose=")+to_string(pBone->cur.pose) + string(" cnt=")+to_string(pBone->animations[pBone->cur.act].pose.size()) ); 
					}
					gra.Print( vect2(10,16*y++)/vect2(768/2,-512/2)+vect2(-1,1),string("peak=")+to_string(time_peak/1000)+string("msec") ); 
				}

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
			if ( (!keys.ALT.on && mouse.R.on) || (keys.ALT.on && mouse.L.on) ) pers.cam.Rotation( vect3(-mouse.mov.x/28,mouse.mov.y/28,0) );

			// カメラ平行移動
			if ( mouse.M.on ) pers.cam.Move( vect3(-mouse.mov.x,mouse.mov.y,0)/pers.height/pers.getW((pers.cam.pos-pers.cam.at).length()));

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
				}
			}

			// グリッドgridMini
			{
				const int NUM = 10;

				gridGround.SetMesh( vect3(0,0,0), NUM, NUM, 1, vect3(0.2,0.2,0.2) );
				gridGround.DrawMesh( *this );

			}
			
			// 箱
			//calcDisp rotate
			box.disp.clear();
			for ( vect3 v : box.vert )
			{

	#if 0
				float	x,y,z;
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

	

			// マーカー操作	
			{
				// 最近マーカー初期化
				if ( !keys.ALT.on && mouse.L.hi ) selector.searchNear( mouse.pos/vect2(768/2,-512/2)+vect2(-1,1) );

				// 矩形カーソル開始
				if ( !keys.ALT.on && mouse.L.hi && selector.a.pm == 0 ) selector.beginRectcursor( mouse.pos/vect2(768/2,-512/2)+vect2(-1,1) );

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
				if ( !keys.ALT.on && mouse.L.on &&  keys.CTRL.on && !keys.SHIFT.on &&  selector.rect_bSelect ) selector.rect_selectReverse( mouse.pos/vect2(768/2,-512/2)+vect2(-1,1) );

				// 矩形カーソル 追加選択	
				if ( !keys.ALT.on && mouse.L.on && !keys.CTRL.on &&  selector.rect_bSelect ) selector.rect_selectAdd( mouse.pos/vect2(768/2,-512/2)+vect2(-1,1) );

				// 矩形カーソル解除	
				if ( !keys.ALT.on && !mouse.L.on &&  selector.rect_bSelect ) selector.endRect();

				// マーカー移動準備
				if ( !keys.ALT.on && mouse.L.hi && !keys.CTRL.on && !keys.SHIFT.on && !selector.rect_bSelect ) 
				{
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

					}
				
				}
						gra.Print( vect2(mouse.pos.x+10,mouse.pos.y-0)/vect2(768/2,-512/2)+vect2(-1,1), string("")+(manupirator.bAxisX?"X":"")+(manupirator.bAxisY?"Y":"")+(manupirator.bAxisZ?"Z":"") ); 
				// マーカー移動
				if ( !keys.ALT.on && mouse.L.on && !keys.CTRL.on && !keys.SHIFT.on && !selector.rect_bSelect ) 
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
								vect2 mmov = mouse.mov /vect2(768/2,-512/2);//+vect2(-1,1);
								vect3 v = vect3(mmov.x, mmov.y, 0)/(pj->disp.z);
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

								vect3 P = pers.calcInvers( mouse.pos/vect2(768/2,-512/2)+vect2(-1,1) );
								vect3 I = pers.calcRayvect( P );
								vect3 Q;
								bool b = funcIntersectPlate( plate_P, plate_N,  P, I, Q );
								if ( b )
								{
									vect3 P = pers.calcInvers( mouse.prev/vect2(768/2,-512/2)+vect2(-1,1) );
									vect3 I = pers.calcRayvect( P );
									vect3 Q0;
									bool b = funcIntersectPlate( plate_P, plate_N, P, I, Q0 );
									if ( b )
									{
										if ( manupirator.bAxisY )
										{
											float y  = dot( (Q-Q0), vect3(0,1,0) );
											pj->pos.y += y;
										}
										if ( manupirator.bAxisZ )
										{
											float z  = dot( (Q-Q0), vect3(0,0,1) );
											pj->pos.z += z;
										}
										if ( manupirator.bAxisX )
										{
											float x  = dot( (Q-Q0), vect3(1,0,0) );
											pj->pos.x += x;
										}
										//circle3d_y( pj->pos, 0.1, vect3(0.8,0.2,0.2) );
									}
								}

								
								
							}


						}
					}
					pBone->RefrectKeyframe();
				}				
			}			

			

			//=================================
			// 3D joint
			//=================================

			// human 更新
			pBone->update();

			// human 描画
			pBone->draw( pers, gra );


			if ( !pBone->anim.bPlaying  )
			{
				// カトマル3D モーション軌跡表示
				pBone->drawMotion( pers, gra );

				// マーカー表示
				selector.drawController( mouse.pos/vect2(768/2,-512/2)+vect2(-1,1), gra );

			}
			
			
			
			// 原点
			circle3d_y( vect3(0,0,0), 0.1, vect3(0.2,0.2,0.2) );


			// マニュピレーター描画
//			if ( manupirator.bActive  ) 
			{
				vect3 p = pers.calcInvers( mouse.pos/vect2(768/2,-512/2)+vect2(-1,1) );
//				manupirator.manupirator_setPos( p );
				manupirator.manupirator_drawAxis( p, *this );

			}


			// マウス座標（投影面座標）を３Ｄ空間座標に逆変換
			if(0)
			{
				vect3 v = pers.calcInvers( mouse.pos/vect2(768/2,-512/2)+vect2(-1,1) );
				vect3 p = pers.calcRay( v, 10 );


				line3d( v, p, vect3(1,0,0));
				line3d( vect3(0,0,0), v, vect3(1,1,0));
				line3d( vect3(0,-1,0), p, vect3(1,0,1));



				gra.Print( vect2(10,16*20),string("v x=")+to_string(v.x) + string(" y=")+to_string(v.y) +string(" z=")+to_string(v.z) );
				gra.Print( vect2(10,16*21),string("p x=")+to_string(p.x) + string(" y=")+to_string(p.y) +string(" z=")+to_string(p.z) );
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
