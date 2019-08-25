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
#include "bone.h"


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
		void manupirator_drawAxis( vect3 pos, Apr& apr )
		//------------------------------------------------------------------------------
		{
			vect3 v0 = apr.pers.calcDisp3( pos * apr.pers.cam.mat.invers() );



			float l = 0.1;
			if ( bAxisX  )
			{
				vect3 v1 = v0 + vect3(
					apr.pers.cam.mat.m[0][0] * apr.pers.aspect,
					apr.pers.cam.mat.m[1][0],
					apr.pers.cam.mat.m[2][0]
				) * l;
				apr.gra.Line( vect2(v0.x,v0.y), vect2(v1.x,v1.y), rgb(0.8,0.2,0.2) );
			}
			if ( bAxisY  )
			{
				vect3 v1 = v0 + vect3(
					apr.pers.cam.mat.m[0][1] * apr.pers.aspect,
					apr.pers.cam.mat.m[1][1],
					apr.pers.cam.mat.m[2][1]
				) * l;
				apr.gra.Line( vect2(v0.x,v0.y), vect2(v1.x,v1.y), rgb(0.2,0.8,0.2) );
			}
			if ( bAxisZ )
			{
				vect3 v1 = v0 + vect3(
					apr.pers.cam.mat.m[0][2] * apr.pers.aspect,
					apr.pers.cam.mat.m[1][2],
					apr.pers.cam.mat.m[2][2]
				) * l;
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
	void pset3d( vect3 p0, rgb col, float wide )
	//------------------------------------------------------------------------------
	{
		vect3 v = pers.calcDisp3( p0 * pers.cam.mat.invers() );
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

			{//原点表示
				float r = 0.1;
				vect2 v0;
				for ( int i = 0 ; i <= 360 ; i+=20 )
				{
					vect3 p = vect3( r*cos(rad(i)), 0, r*sin(rad(i)) ) + pos;
					vect3 q = apr.pers.calcDisp3( p * apr.pers.cam.mat.invers() );
					vect2 v1 = vect2( q.x, q.y );
					if ( i > 0 ) apr.gra.Line( v0,v1, col );
					v0 = v1;
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
		int text_y = 0;
//		vector<Joint3> listSelected;
		vector<reference_wrapper<Joint3>>	list;


		unique_ptr<Bone> pBone(new Bone);

				{
					//読み込み
					unique_ptr<Bone> pNew(new Bone);
					pNew->loadMotion( "human.mot" );
					pBone = move(pNew);
				}
pBone->stat.bShowSkin = false;

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



		//===========================================================================
		while( Update() )
		{
			text_y = 0;

			// パースペクティブ
			if (keys.Y.rep) {pers.fovy-=2;cout << pers.fovy <<" "<<1/tan(rad(pers.fovy)) << endl; }
			if (keys.H.rep) {pers.fovy+=2;cout << pers.fovy <<" "<<1/tan(rad(pers.fovy)) << endl; }

			// パース更新
			pers.Update( vect2( gra.GetWidth(), gra.GetHeight() ) );

			// 画面クリア
			gra.Clr(rgb(0.3,0.3,0.3));

			{


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
							gra.Fill( gra.Conv(v), gra.Conv(v+vect2(3,7)), rgb(1,1,1) );
						}

						if ( y == pBone->cur.act && x == pBone->cur.pose )
						{
							gra.Fill( gra.Conv(v+vect2(0,4)), gra.Conv(v+vect2(3,7)), rgb(1,0,0) );
						}
						
						if ( flg )
						{
							gra.Fill( gra.Conv(v+vect2(0,4)), gra.Conv(v+vect2(3,7)), rgb(1,0,0) );
						}
					}
				}
			}


			// カメラ回転
			if ( (!keys.ALT.on && mouse.R.on && !mouse.L.on && !mouse.M.on) || (keys.ALT.on && !mouse.R.on && mouse.L.on && !mouse.M.on) ) pers.cam.Rotation( vect3(-mouse.mov.x/28,mouse.mov.y/28,0) );

			// カメラ平行移動
			if ( mouse.M.on ) pers.cam.Move( vect3(-mouse.mov.x,mouse.mov.y,0)*2/gra.GetHeight()/pers.getW((pers.cam.pos-pers.cam.at).length()));

			// マウスホイールZOOM
			if ( !keys.ALT.on  ) pers.cam.Zoom( -mouse.wheel*2/gra.GetHeight()/pers.getW((pers.cam.pos-pers.cam.at).length()) );
			
			// カメラ移動
			if ( (keys.ALT.on && mouse.R.on) || ( mouse.R.on && mouse.L.on ) ) pers.cam.Zoom( mouse.mov.y*2/gra.GetHeight()/pers.getW((pers.cam.pos-pers.cam.at).length()) );
			
			// カメラマトリクス計算
			{
				pers.cam.mat.LookAt( pers.cam.pos, pers.cam.at, pers.cam.up );
			}

			// カメラ注視点表示
			{
				vect3 v = pers.calcDisp3(pers.cam.at*pers.cam.mat.invers());
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


			//=================================
			// 3D joint
			//=================================

			// human 更新
			pBone->update();

			// human 描画
			pBone->draw( pers, gra );

			if ( keys._1.hi ) pBone->stat.bShowBone = !pBone->stat.bShowBone;
			if ( keys._2.hi ) pBone->stat.bShowSkin = !pBone->stat.bShowSkin;


			// マニュピレーター描画
			{
//				vect3 p = pers.calcInvers( gra.Conv(mouse.pos) );
				vect3 p = pers.calcScreenToWorld( vect3(gra.Conv(mouse.pos),0) );
				manupirator.manupirator_drawAxis( p, *this );

			}




			// マウス座標（投影面座標）を３Ｄ空間座標に逆変換
			if(1)
			{
				vect3 P = pers.calcScreenToWorld( vect3(gra.Conv(mouse.pos),0) );
				vect3 I = pers.calcRayvect( P );


				line3d( vect3(0,0,0), P, vect3(1,1,0));
				line3d( vect3(0,0,0), P+I*10.0f, vect3(1,1,1));



				vect3 mpos = vect3(gra.Conv(mouse.pos),0);

				// 選択リストクリア
				list.clear();

				// 最近点を一つだけのリストを作成
//				if ( mouse.L.on ) 
				{
					struct
					{
						float	z;
						Joint3*	p;
					} a = {0,0};

					for ( Joint3& j : pBone->tblJoint )
					{
						vect3 v = pers.calcWorldToScreen3( j.pos );

	//					if ( IsIntersectSphereLine( j.pos, 0.05f, P, I ) )
						if ( (vect2(v.x,v.y)-gra.Conv(mouse.pos)).length() < 0.05f )
						{
							if ( a.z < v.z )
							{
								a.z = v.z;
								a.p = &j;
							}
						}
					}
					if ( a.p ) list.push_back( *a.p );
				}


				// 選択リスト表示
				for ( Joint3& j : list )
				{
						gra.Pset( pers.calcDisp2( j.pos * pers.cam.mat.invers() ), rgb(1,0,0), 5 );
				}

			#if 0
				{	//砲台
					v.normalize();
					float rx = acos(dot(vect3(0,1,0),v));
					float ry = atan2(v.x,v.z);

					gra.Print( gra.Conv(vect2(10,16*(text_y++))),string("ry=")+to_string(deg(ry)) );
					gra.Print( gra.Conv(vect2(10,16*(text_y++))),string("v.x=")+to_string(v.x) );
					gra.Print( gra.Conv(vect2(10,16*(text_y++))),string("v.z=")+to_string(v.z) );
					gra.Print( gra.Conv(vect2(10,16*(text_y++))),string("ry=")+to_string(deg(ry)) );
						
					mat44	mx;
					mat44	my;
					my.setRotateY(-ry);
					mx.setRotateX(-rx);
					mat44 m = mx * my;

					pBone->ring.ring_DrawMat( gra, pers, vect3(  0,0,0), m );
				}
			#endif
			}

			{
				if( keys.F2.hi ) flgInfo = !flgInfo;
				if ( flgInfo )
				{
					gra.Print( gra.Conv(vect2(10,16*text_y++)),string("fovY:")+to_string(int(pers.fovy)));
					gra.Print( gra.Conv(vect2(10,16*text_y++)),string("sz:")+to_string(pers.sz) +string(" fy:")+to_string(pers.fy));
					gra.Print( gra.Conv(vect2(10,16*text_y++)),string("far:")+to_string((pers.cam.pos-pers.cam.at).length())); 
					gra.Print( gra.Conv(vect2(10,16*text_y++)),string("at  x=")+to_string(pers.cam.at.x)+string(" y=")+to_string(pers.cam.at.y)+string(" z=")+to_string(pers.cam.at.z) ); 
					gra.Print( gra.Conv(vect2(10,16*text_y++)),string("pos x=")+to_string(pers.cam.pos.x)+string(" y=")+to_string(pers.cam.pos.y)+string(" z=")+to_string(pers.cam.pos.z) ); 
					gra.Print( gra.Conv(vect2(10,16*text_y++)),string("anim=")+to_string(pBone->cur.act) + string(" cnt=")+to_string(pBone->animations.size()) ); 
					if ( pBone->animations.size() > 0 ) 
					{
						gra.Print( gra.Conv(vect2(10,16*text_y++)),string("pose=")+to_string(pBone->cur.pose) + string(" cnt=")+to_string(pBone->animations[pBone->cur.act].pose.size()) ); 
					}
					gra.Print( gra.Conv(vect2(10,16*text_y++)),string("peak=")+to_string(time_peak/1000)+string("msec") ); 
				}


				if ( keys.F1.on )
				{
					gra.Print(gra.Conv(vect2(10,16*text_y++)),string("[F1] help"));
					gra.Print(gra.Conv(vect2(10,16*text_y++)),string("[Y] pers -"));
					gra.Print(gra.Conv(vect2(10,16*text_y++)),string("[H] pers +"));
					gra.Print(gra.Conv(vect2(10,16*text_y++)),string("[L] Load"));
					gra.Print(gra.Conv(vect2(10,16*text_y++)),string("[S] Save"));
					gra.Print(gra.Conv(vect2(10,16*text_y++)),string("--Keyframe--"));
					gra.Print(gra.Conv(vect2(10,16*text_y++)),string("[K] Insert"));
					gra.Print(gra.Conv(vect2(10,16*text_y++)),string("[X] Cut"));
					gra.Print(gra.Conv(vect2(10,16*text_y++)),string("[C] Copy"));
					gra.Print(gra.Conv(vect2(10,16*text_y++)),string("[V] Past"));
					gra.Print(gra.Conv(vect2(10,16*text_y++)),string("[LEFT]  -"));
					gra.Print(gra.Conv(vect2(10,16*text_y++)),string("[RIGHT] +"));
					gra.Print(gra.Conv(vect2(10,16*text_y++)),string("--Animation--"));
					gra.Print(gra.Conv(vect2(10,16*text_y++)),string("[I] Add"));
					gra.Print(gra.Conv(vect2(10,16*text_y++)),string("[P] Play"));
					gra.Print(gra.Conv(vect2(10,16*text_y++)),string("[UP] -"));
					gra.Print(gra.Conv(vect2(10,16*text_y++)),string("[DOWN] +"));
					gra.Print(gra.Conv(vect2(10,16*text_y++)),string("--Other--"));
					gra.Print(gra.Conv(vect2(10,16*text_y++)),string("[1] select 3d"));
					gra.Print(gra.Conv(vect2(10,16*text_y++)),string("[2] select 2main"));
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
	Apr	apr("Ray4 " __DATE__, 300,300,768, 512 );
	return apr.main();
}
