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
#include <tuple>

#include "geom.h"


#include "SysKeys.h"
#include "SysMouse.h"
#include "SysGra.h"
#include "Sys.h"

#include "func.h"
#include "pers.h"

#include "raytrace.h"
#include "skeleton.h"
#include "gui.h"

#include "lab.h"

using namespace std;


Gui gui;
Lab lab;

struct CutmullRing
{
	bool	bActive = false;	//	使用可能

	int idxTbl = 0;

	~CutmullRing()
	{
		for ( Obj* p : tblPoint )
		{
			delete p;
		}	
	}
	
	
	vector<Obj*>	tblPoint =
	{
		new Obj( vect3(	-0.5,	0.12,	-0.5) ),
		new Obj( vect3(	+0.5,	0.12,	-0.5) ),
		new Obj( vect3(	+0.9,	0.12,	 0.5) ),
		new Obj( vect3(	 0.0,	0.12,	 0.5) ),
		new Obj( vect3(	-0.9,	0.12,	 0.5) ),

		new Obj( vect3(	-0.5,	0.12,	-0.5)*1.2 ),
		new Obj( vect3(	+0.5,	0.12,	-0.5)*1.2 ),
		new Obj( vect3(	+0.9,	0.12,	 0.5)*1.2 ),
		new Obj( vect3(	0.0,	0.12,	 0.5)*1.2 ),
		new Obj( vect3(	-0.9,	0.12,	 0.5)*1.2 ),
	};
	
	vector<ivect2>	tblIdx =
	{
		{0,5},
		{1,6},
		{2,7},
		{3,8},
		{4,9},
	};

	//------------------------------------------------------------------------------
	void drawPoint( SysGra& gra, Pers& pers, vector<Obj*>& tblPoint )
	//------------------------------------------------------------------------------
	{
		int cnt = 0 ;
		gra.SetZTest(false);
		for ( Obj* p : tblPoint )
		{
			vect3 v = pers.calcWorldToScreen3( p->pos );
			if ( v.z > 0 )
			{
				if ( p->bSelected ) 
						gra.Pset( v, rgb(1,0,0), 11 ); 
				else	gra.Pset( v, rgb(0,0,1), 11 ); 

				gra.Print( v.xy() + gra.Dot(16,-12), to_string(cnt) ); 
			}
			cnt++;
		}
		gra.SetZTest(true);
	}

	//------------------------------------------------------------------------------
	void drawCutmull( SysGra& gra, Pers& pers, vector<Obj*> tbl, vector<ivect2>idx )
	//------------------------------------------------------------------------------
	{
		// 描画 カーブ
		function<void()> func = [&]()
		{
			rgb	col(1,1,1);
			int size = (signed)idx.size();
			vect3	v0;
			vect3	v2;
			vect3	w0;
			vect3	w2;
			for ( int i = 0 ; i < size ; i++ )
			{
				int n0 = idx[i].n0;
				int n1 = idx[(i+1)%size].n0;
				int n2 = idx[(i+2)%size].n0;
				int n3 = idx[(i+3)%size].n0;

				int m0 = idx[i].n1;
				int m1 = idx[(i+1)%size].n1;
				int m2 = idx[(i+2)%size].n1;
				int m3 = idx[(i+3)%size].n1;
			
				vect3 P0 = tbl[n0]->pos;
				vect3 P1 = tbl[n1]->pos;
				vect3 P2 = tbl[n2]->pos;
				vect3 P3 = tbl[n3]->pos;

				vect3 Q0 = tbl[m0]->pos;
				vect3 Q1 = tbl[m1]->pos;
				vect3 Q2 = tbl[m2]->pos;
				vect3 Q3 = tbl[m3]->pos;
				
				for ( float t = 0.0 ; t < 1.0 ; t+=0.1 )
				{
					vect3 v1 = catmull3_func(t, P0,P1,P2,P3 );
					vect3 w1 = catmull3_func(t, Q0,Q1,Q2,Q3 );
					if ( (i==0 && t==0) ) 
					{
						v2=v1;
						w2=w1;
					}
					else 
					{
						pers.line3d( gra, pers, v0, v1, col);
						pers.line3d( gra, pers, w0, w1, col);
						pers.line3d( gra, pers, w1, v1, col);

						{
							vect3 a = v0;a.y=0;
							vect3 b = v1;b.y=0;
							pers.line3d( gra, pers, a, b, rgb(0.2,0.2,0.2));
						}
						{
							vect3 a = w0;a.y=0;
							vect3 b = w1;b.y=0;
							pers.line3d( gra, pers, a, b, rgb(0.2,0.2,0.2));
						}

					}

						{
							vect3 nx,ny,nz;
							nz = (v1-v0).normalize();
							ny = vect3(0,1,0);
							nx = cross(nz,ny).normalize();
							ny = cross(nx,nz).normalize();

							mat33	m(
								nx.x,	nx.y,	nx.z,
								ny.x,	ny.y,	ny.z,
								nz.x,	nz.y,	nz.z
							);

							//square.DrawSquare( gra, pers, v1, m, false  );
						}


					v0 = v1;
					w0 = w1;
				}
			}
			pers.line3d( gra, pers, v0, v2, col);
			pers.line3d( gra, pers, w0, w2, col);
			pers.line3d( gra, pers, w2, v2, col );
			{
				vect3 a = v0;a.y=0;
				vect3 b = v2;b.y=0;
				pers.line3d( gra, pers, a, b, rgb(0.2,0.2,0.2));
			}
			{
				vect3 a = w0;a.y=0;
				vect3 b = w2;b.y=0;
				pers.line3d( gra, pers, a, b, rgb(0.2,0.2,0.2));
			}
		};
		func();

	}

};

struct BezierRing
{
	bool	bActive = false;	//	使用可能

	int idxTbl = 0;

	~BezierRing()
	{
		for ( Obj* p : tblPoint )
		{
			delete p;
		}	
	}

	vector<Obj*> tblPoint =
	{
		new Point3(vect3(-1.0, 0.0, 0.0 ),vect3( 0.0, 0.0, 1.0 ),vect3( 0.0, 0.0,-1.0 )),
		new Point3(vect3( 1.0, 0.0, 0.0 ),vect3( 0.0, 0.0,-1.0 ),vect3( 0.0, 0.0, 1.0 )),

	};
	vector<int>	idxPoint =
	{
		0,1,0
	};

	//------------------------------------------------------------------------------
	void exec_drawBezier( SysGra& gra, Pers& pers, vector<Obj*>& tblPoint, vector<int>& idxPoint, int idxTbl , vect3& P, vect3& I, bool bSerch, bool bCut )
	//------------------------------------------------------------------------------
	{
		//ベジェ計算＆描画
		int div = 10;
		float dt = 1/(float)div;

		int size = static_cast<signed>(idxPoint.size());

		float	mind = infinit;
		vect3	minQ1;
		vect3	minQ0;
		vect3	minQ;
		int		minn;
		float	mint;
		vect3	mindt;
		bool	minb = false;
		vect3	minv;

		for ( int n = 0 ; n < size-1 ; n++ )
		{
			int n0 = idxPoint[n];
			int n1 = idxPoint[n+1];

			Point3* p0 = dynamic_cast<Point3*>(tblPoint[n0]);
			Point3* p1 = dynamic_cast<Point3*>(tblPoint[n1]);

			vect3 P0 =     p0->pos;
			vect3 P1 = P0 +p0->b;
			vect3 P3 =     p1->pos;
			vect3 P2 = P3 +p1->a;

			float t  = dt;
			vect3 pos0 = p0->pos;
			for ( int i = 0 ; i < div ; i++ )
			{
				vect3 pos1 = bezier3_func( t, P0, P1, P2, P3 );
				pers.line3d( gra, pers, pos0, pos1, rgb(1,1,1) );

				// マウスベクトルとの最近点
				if ( bSerch )
				{
					auto[b,d,Q0,Q1,t0,t1] = distanceLineSegline_func( P, I, pos0, pos1 );
					if ( b ) 
					{
						if ( mind > d && 0.2 > d )
						{
							minb	= true;
							mind	= d;
							minn	= n;

							minQ0 = Q0;
							minQ1 = Q1;

							mint	= t-(1/(float)div) + (t1/(pos1-pos0).abs())/((float)div);
							minQ	= bezier3_func( mint, P0, P1, P2, P3 );
							mindt	= bezier3_delta_func( mint, P0, P1, P2, P3 );

							minv = (minQ-Q0).normalize();
						}
					}

				}

				pos0=pos1;
				t+=dt;
			}

		}

		// マウスベクトルとの最近点表示
		gra.SetZTest( false );
		if ( minb ) 
		{
			pers.pset3d( gra, pers, minQ, rgb(1,0,0), 5 );
		}
		gra.SetZTest( true );

		//--------
		// 切断
		//--------
		if ( minb )
		{

			if ( bCut )
			{
				vect3	q = minQ;
				vect3	v = mindt/3;
				float	t0 = mint;
				float	t1 = 1.0-mint;

				// 頂点生成
				{
					
					tblPoint.emplace_back( new Point3( q, -v*t0, v*t1 ) );
					idxPoint.insert( idxPoint.begin()+minn+1, (signed)tblPoint.size()-1);
				}

				{
					Point3* p0 = dynamic_cast<Point3*>(tblPoint[idxPoint[minn+0]]);
					Point3* p2 = dynamic_cast<Point3*>(tblPoint[idxPoint[minn+2]]);
					p0->b *= t0;
					p2->a *= t1;
				}
				gui.rect_mode = Gui::G_CALC::NONE;


				// 追加したポイントを選択状態にする
				{
					int idx = idxPoint[minn+1];
					vect3 v = pers.calcWorldToScreen3( minQ );
					//
					gui.one.w = v.z;
					gui.one.idxTbl = idxTbl;
					gui.one.idxObj = idx;
					gui.one.bEnable = true;
					tblPoint[ idx ]->bSelected = true;
					tblPoint[ idx ]->bPreselect = false;
					gui.one.bSelected_a = false;
					gui.one.bSelected_b = false;
				}
			}

			{
				gra.SetZTest( false );
				pers.print3d( gra, pers, minQ, -26,-32-20, to_string(mint) ); 
				gra.SetZTest( true );
			}
		}


	};
	
};

struct
{

	//-------------------------------------------------------------------------
	void skeleton_update( SysKeys& keys, SysMouse& mouse, Pers& pers, Skeleton& skeleton )
	//-------------------------------------------------------------------------
	{
		if ( skeleton.bActive )
		{

			if ( skeleton.cur.bSelecting ==false && keys.SHIFT.on && (keys.UP.hi || keys.DOWN.hi || keys.LEFT.hi ||keys.RIGHT.hi) )
			{
				skeleton.cur.bSelecting = true;
				skeleton.cur.selecting_act = skeleton.cur.act;
				skeleton.cur.selecting_pose = skeleton.cur.pose;
			}
			if ( !keys.SHIFT.on && (keys.UP.hi || keys.DOWN.hi || keys.LEFT.hi ||keys.RIGHT.hi) )
			{
				skeleton.cur.bSelecting = false;
			}
			
			// キーフレームセーブ
			if ( keys.CTRL.on && keys.S.hi ) skeleton.SaveSkeleton();

			// キーフレームペースト
			if ( keys.CTRL.on && keys.V.hi ) skeleton.PastKeyframe();

			// キーフレームコピー
			if ( keys.CTRL.on && keys.C.hi ) skeleton.CopyKeyframe();

			// キーフレームカット
			if ( keys.CTRL.on && keys.X.hi ) skeleton.CutKeyframe();

			// キーフレーム追加
			if ( keys.K.hi ) skeleton.InsertKeyframe();

			// キーフレーム次
			if ( keys.RIGHT.rep ) skeleton.NextKeyframe();

			// キーフレーム最後
			if ( keys.CTRL.on && keys.RIGHT.rep ) skeleton.LastKeyframe();

			// キーフレーム前
			if ( keys.LEFT.rep ) skeleton.PrevKeyframe();

			// キーフレーム先頭
			if ( keys.CTRL.on && keys.LEFT.rep ) skeleton.TopKeyframe();

			// アニメーション記録
			if ( keys.I.hi ) skeleton.AddAnimation();

			// アニメーション前
			if ( keys.UP.rep ) skeleton.PrevAnimation();

			// アニメーション次
			if ( keys.DOWN.rep ) skeleton.NextAnimation();

			// アニメーションリクエスト
			if ( keys.P.hi ) skeleton.ReqAnimation();

			// アニメーション再生
			if ( skeleton.anim.bPlaying )	skeleton.PlayAnimation();


			if ( keys._1.hi ) skeleton.stat.bShowBone = !skeleton.stat.bShowBone;
			if ( keys._2.hi ) skeleton.stat.bShowSkin = !skeleton.stat.bShowSkin;
			if ( keys._3.hi ) skeleton.stat.bShowLocus = !skeleton.stat.bShowLocus;
		}
		

	}

	//-------------------------------------------------------------------------
	void skeleton_draw( SysGra& gra, SysKeys& keys, SysMouse& mouse, Pers& pers, Skeleton& skeleton, int& text_y )
	//-------------------------------------------------------------------------
	{
		if ( skeleton.bActive == false ) return;

	#if 0 	//剛体実験
		// 優先度つけ
		{
			int n = 0;
			for ( Obj* po : skeleton.tblPoint )
			{
				Joint* p = dynamic_cast<Joint*>(po);

				p->weight = 0.33;
				if ( n == 2  ) p->weight = 0.000;
				if ( n == 0  ) p->weight = 0.000;
				if ( n == 1  ) p->weight = 0.000;
				if ( n == 3  ) p->weight = 0.000;
				if ( p->bSelected && mouse.L.on ) p->weight = 0.0;
				n++;
			}
		}
	#else
		// 優先度つけ
		{
			int n = 0;
			for ( Obj* po : skeleton.tblPoint )
			{
				Joint* p = dynamic_cast<Joint*>(po);

				p->weight = 0.33;
				if ( p->bSelected && mouse.L.on ) p->weight = 0.0;
				n++;
			}
		}
	#endif

		//スケルトン更新
		skeleton.UpdateSkeleton();


		// スケルトン 描画
		skeleton.DrawSkeleton( pers, gra );

		if(1)
		{
			mat33	mmune = midentity();
			static mat33	mkata = midentity();
			mat33	mhiji = midentity();;
			mat33	mte = midentity() ;
			vect3	pos0 = skeleton.tblPoint[0]->pos;
			vect3	pos1 = skeleton.tblPoint[1]->pos;
			vect3	p2 = skeleton.tblPoint[2]->pos;
			vect3	p3 = skeleton.tblPoint[3]->pos;
			vect3	p4 = skeleton.tblPoint[4]->pos;
			vect3	p5 = skeleton.tblPoint[5]->pos;
			// 箱 胸
			if(1)
			{
				vect3 nx,ny,nz;
				ny = (pos0-p3).normalize();
				nx = (pos1-p2).normalize();
				nz = cross(ny,nx).normalize();
				nx = cross(ny,nz).normalize();

				mmune = mat33(
					nx.x,	nx.y,	nx.z,
					ny.x,	ny.y,	ny.z,
					nz.x,	nz.y,	nz.z
				);

//				box.DrawBox( gra, pers, pos0, mmune, false, false );
				pers.DrawBox( gra, pers, pos0, mmune, false, false );
			}
			// 箱 肩
			if(1)
			{
				vect3 nx,ny,nz;
				ny = (p2-p4).normalize();
				nx = (p2-pos0).normalize();
				nz = cross(nx,ny).normalize();
				nx = cross(ny,nz).normalize();
				nz = cross(nx,ny).normalize();

				mat33	m(
					nx.x,	nx.y,	nx.z,
					ny.x,	ny.y,	ny.z,
					nz.x,	nz.y,	nz.z
				);

				if ( keys.CTRL.on )		mkata.rotateByAxis( (p4-p2).normalize(), deg2rad(0.5));
				if ( keys.SHIFT.on )	mkata.rotateByAxis( (p4-p2).normalize(), deg2rad(-0.5));
				
//				box.DrawBox( gra, pers, p2, mkata );
				pers.DrawBox( gra, pers, p2, mkata, false, false );
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
//					mhiji = skeleton.tblPoint[4].mat;
				pers.DrawDrum( gra, pers, p4, mhiji );
			}
			// 箱 手
			if(0)
			{
				vect3 nx,ny,nz;
				ny = (p4-p5).normalize();
				nx = (p2-pos0).normalize();
				nz = cross(nx,ny).normalize();
				nx = cross(ny,nz).normalize();
				nz = cross(nx,ny).normalize();

				mat33	m(
					nx.x,	nx.y,	nx.z,
					ny.x,	ny.y,	ny.z,
					nz.x,	nz.y,	nz.z
				);	
				mte = m;
				pers.DrawDrum( gra, pers, p5, mte );
			}
		}

		// animカーソルビュー cursor
		{
			gra.SetZTest( false );

			bool flg = false;
			for ( int y = 0 ; y < (signed)skeleton.animations.size() ; y++ )
			{
				for ( int x = 0 ; x < (signed)skeleton.animations[y].pose.size() ; x++ )
				{
					if ( skeleton.cur.bSelecting && ( y == skeleton.cur.selecting_act && x == skeleton.cur.selecting_pose ) ) flg=!flg;
					if ( skeleton.cur.bSelecting && ( y == skeleton.cur.act && x == skeleton.cur.pose ) ) flg=!flg;

					vect2 v = vect2( (float)x, (float)y )*vect2( 4, 8 ) + vect2(400,16);
					{
						gra.Fill( gra.Conv(v), gra.Conv(v+vect2(3,7)), rgb(1,1,1) );
					}

					if ( y == skeleton.cur.act && x == skeleton.cur.pose )
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


		{
			static bool flgInfo = true;
			if( keys.F2.hi ) flgInfo = !flgInfo;
			if ( flgInfo )
			{

				gra.Print(1,(float)text_y++,string("[ ")+skeleton.filename+" ]");
				gra.Print(1,(float)text_y++,string("anim=")+to_string(skeleton.cur.act) + string(" cnt=")+to_string(skeleton.animations.size()) ); 
				if ( skeleton.animations.size() > 0 ) 
				{
					gra.Print(1,(float)text_y++,string("pose=")+to_string(skeleton.cur.pose) + string(" cnt=")+to_string(skeleton.animations[skeleton.cur.act].pose.size()) ); 
				}
			}

			if ( keys.F1.on )
			{
				gra.Print(1,(float)text_y++,string("[F1] help"));
				gra.Print(1,(float)text_y++,string("[Y] pers -"));
				gra.Print(1,(float)text_y++,string("[H] pers +"));
				gra.Print(1,(float)text_y++,string("[L] Load"));
				gra.Print(1,(float)text_y++,string("[S] Save"));
				gra.Print(1,(float)text_y++,string("--Keyframe--"));
				gra.Print(1,(float)text_y++,string("[K] Insert"));
				gra.Print(1,(float)text_y++,string("[X] Cut"));
				gra.Print(1,(float)text_y++,string("[C] Copy"));
				gra.Print(1,(float)text_y++,string("[V] Past"));
				gra.Print(1,(float)text_y++,string("[LEFT]  -"));
				gra.Print(1,(float)text_y++,string("[RIGHT] +"));
				gra.Print(1,(float)text_y++,string("--Animation--"));
				gra.Print(1,(float)text_y++,string("[I] Add"));
				gra.Print(1,(float)text_y++,string("[P] Play"));
				gra.Print(1,(float)text_y++,string("[UP] -"));
				gra.Print(1,(float)text_y++,string("[DOWN] +"));
				gra.Print(1,(float)text_y++,string("--Other--"));
				gra.Print(1,(float)text_y++,string("[1] select 3d"));
				gra.Print(1,(float)text_y++,string("[2] select 2main"));
			}
			else
			{
				//gra.Print(vect2(10,16*y++),string("[F1] Help"));
			}
		}
	}
} util;



struct
{

	bool bAxisX = true;;
	bool bAxisY = true;;
	bool bAxisZ = true;;

	//------------------------------------------------------------------------------
	void DrawAxis( SysGra& gra, Pers& pers, vect2 mpos )
	//------------------------------------------------------------------------------
	{
		gra.SetZTest( false );

		vect3 pos = pers.calcScreenToWorld3( vect3(mpos,0) );

		vect3 v0 = pers.calcWorldToScreen3( pos );

		// 軸表示
		float l = 0.1;
		if ( bAxisX  )
		{
			vect3 v1 = v0 + vect3(
				pers.cam.mat.m[0][0] / pers.aspect,
				pers.cam.mat.m[1][0],
				pers.cam.mat.m[2][0]
			) * l;
			gra.Line( v0, v1, rgb(0.8,0.2,0.2), 2.0 );
		}
		if ( bAxisY  )
		{
			vect3 v1 = v0 + vect3(
				pers.cam.mat.m[0][1] / pers.aspect,
				pers.cam.mat.m[1][1],
				pers.cam.mat.m[2][1]
			) * l;
			gra.Line( v0, v1, rgb(0.2,0.8,0.2), 2.0 );
		}
		if ( bAxisZ )
		{
			vect3 v1 = v0 + vect3(
				pers.cam.mat.m[0][2] / pers.aspect,
				pers.cam.mat.m[1][2],
				pers.cam.mat.m[2][2]
			) * l;
			gra.Line( v0, v1, rgb(0.1,0.3,1), 2.0 );

		}

		// 軸名表示
		gra.Print( mpos+gra.Dot(16,-12),string("")+(bAxisX?"X":"")+(bAxisY?"Y":"")+(bAxisZ?"Z":"") ); 

		gra.SetZTest( true );

	}
} axis;

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




	Pers pers;

	//------------------------------------------------------------------------------
	int main()
	//------------------------------------------------------------------------------
	{
		int text_y = 0;

		//=================================
		// 各種Obj 定義
		//=================================
		unique_ptr<Skeleton> pSkeleton(new Skeleton);
		unique_ptr<BezierRing> pBezier(new BezierRing);
		unique_ptr<CutmullRing> pCutmull(new CutmullRing);

		//===========================================================================
		while( Update() )
		{
			text_y = 0;

			//=================================
			// カメラ
			//=================================
			{
				// パースペクティブ
				if (keys.Y.rep) {pers.fovy-=2;cout << pers.fovy <<" "<<1/tan(deg2rad(pers.fovy)) << endl; }
				if (keys.H.rep) {pers.fovy+=2;cout << pers.fovy <<" "<<1/tan(deg2rad(pers.fovy)) << endl; }

				// パース更新
				pers.Update( vect2( gra.GetWidth(), gra.GetHeight() ) );

				
				// カメラ回転
				if ( (!keys.ALT.on && mouse.R.on && !mouse.L.on && !mouse.M.on) || (keys.ALT.on && !mouse.R.on && mouse.L.on && !mouse.M.on) ) pers.cam.Rotation( -vect3(mouse.mov,0)*18.0f );

				// カメラ平行移動
				if ( mouse.M.on ) pers.cam.Move( -vect3(mouse.mov,0)/pers.getW((pers.cam.pos-pers.cam.at).abs()));

				// マウスホイールZOOM
				if ( !keys.ALT.on  ) pers.cam.Zoom( -mouse.wheel*2/gra.GetHeight()/pers.getW((pers.cam.pos-pers.cam.at).abs()) );
				
				// カメラ移動
				if ( (keys.ALT.on && mouse.R.on) || ( mouse.R.on && mouse.L.on ) ) pers.cam.Zoom( -mouse.mov.y/pers.getW((pers.cam.pos-pers.cam.at).abs()) );

				// フォーカス
				if ( keys.F.hi )	pers.Focus( gui.tbls );
				
				// カメラマトリクス計算
				pers.cam.Update();
			}

			//=================================
			// カーソルモード X/Y/Z軸選択モード切替
			//=================================

			if ( keys.Z.hi + keys.X.hi + keys.C.hi > 0 && keys.Z.on + keys.X.on + keys.C.on == keys.Z.hi + keys.X.hi + keys.C.hi ) 
			{
				// 排他的選択
				axis.bAxisZ = false;
				axis.bAxisX = false;
				axis.bAxisY = false;
				if ( keys.Z.hi ) axis.bAxisZ = true;
				if ( keys.X.hi ) axis.bAxisX = true;
				if ( keys.C.hi ) axis.bAxisY = true;
			}
			else
			{
				// 追加選択
				if ( keys.Z.on ) axis.bAxisZ = true;
				if ( keys.X.on ) axis.bAxisX = true;
				if ( keys.C.on ) axis.bAxisY = true;
			}

			//=================================
			// skeleton 入力
			//=================================
			util.skeleton_update( keys, mouse, pers, (*pSkeleton) );


			//=================================
			// キーフレームロード
			//=================================
			if ( keys.CTRL.on && keys.L.hi ) 
			{
				//読み込み
				unique_ptr<Skeleton> pNew(new Skeleton);
				//pNew->LoadSkeleton( "human.mot" );
				pNew->LoadSkeleton( "bone.mot" );
				pNew->stat.bShowSkin = false;
				pNew->stat.bShowLocus = false;
				pSkeleton = move(pNew);
			}
			if ( keys.CTRL.on && keys.SEMICOLON.hi ) 
			{
				//読み込み
				unique_ptr<Skeleton> pNew(new Skeleton);
				pNew->LoadSkeleton( "human.mot" );
				//pNew->LoadSkeleton( "bone.mot" );
				pNew->stat.bShowSkin = false;
				pNew->stat.bShowLocus = false;
				pSkeleton = move(pNew);
			}

			//=================================
			// 実験 on/off
			//=================================
			if ( keys._1.hi ) 
			{
				(*pBezier).bActive = !(*pBezier).bActive;	//	使用可能
			}
			if ( keys._2.hi ) 
			{
				(*pCutmull).bActive = !(*pCutmull).bActive;	//	使用可能
			}



			//=================================
			// 画面クリア
			//=================================
			gra.Clr(rgb(0.3,0.3,0.3));

			//=================================
			// 床グリッド描画
			//=================================
			pers.grid.DrawGrid3d( gra, pers, vect3(0,0,0), midentity(), 10, 10, 1, rgb(0.2,0.2,0.2) );



			//=================================
			// マウス座標（投影面座標）を３Ｄ空間座標に逆変換＆描画
			//=================================

			if(0)
			{
			
				vect3 P = pers.calcScreenToWorld3( vect3(mouse.pos,0) );
				vect3 I = pers.calcRayvect( P );

				vect3 P2 = vect3( 0, 0.4, 0 );
				vect3 P3 = vect3( 0.5, 0.6, 0.4 );

				auto[b,d,Q0,Q1,t0,t1] = distanceLineSegline_func( P, I, P2, P3 );

				if ( b ) pers.line3d( gra, pers, Q0, Q1,  vect3(0,1,0));
				else pers.line3d( gra, pers, Q0, Q1,  vect3(1,0,0));

				pers.line3d( gra, pers, P, P+I,  vect3(1,0,0));
				pers.line3d( gra, pers, P2, P3, vect3(1,1,1));

				{
					vect3 a0 = lab.tblObj[0]->pos;
					vect3 a1 = lab.tblObj[1]->pos;
				
					static vect3 p(0,1,0);
					pers.pset3d( gra, pers, p, rgb(1,1,1), 11 );
					pers.pset3d( gra, pers, a0, rgb(0,1,1), 7 );
					pers.pset3d( gra, pers, a1, rgb(1,1,0), 7 );
				
					p.rotateByAxis( a0, a1-a0, deg2rad(1) );
				}


			}


			//=================================
			//	登録
			//=================================
			gui.tbls.clear();
			if ( (*pBezier).bActive ) (*pBezier).idxTbl = gui.EntryTbl( (*pBezier).tblPoint );
			if ( (*pCutmull).bActive ) (*pCutmull).idxTbl = gui.EntryTbl( (*pCutmull).tblPoint );
			if ( (*pSkeleton).bActive ) (*pSkeleton).idxTbl = gui.EntryTbl( (*pSkeleton).tblPoint );
			gui.EntryTbl( lab.tblObj );


			//=================================
			//	GUI操作
			//=================================
			{

				// 最近点検索
				if ( !keys.ALT.on && mouse.L.hi ) 
					gui.TouchFirst( pers, gui.tbls, mouse.pos );

				// 矩形カーソル開始 新規選択
				if ( !keys.ALT.on && mouse.L.on && !keys.CTRL.on && !keys.SHIFT.on && gui.one.bEnable == false && gui.rect_mode == Gui::G_CALC::NONE ) 
					gui.SelectRectOnly( mouse.pos );

				// 矩形カーソル開始 追加選択
				if ( !keys.ALT.on && mouse.L.on && !keys.CTRL.on && keys.SHIFT.on && gui.one.bEnable == false && gui.rect_mode == Gui::G_CALC::NONE ) 
					gui.SelectRectAdd( mouse.pos );

				// 矩形カーソル開始 反転選択
				if ( !keys.ALT.on && mouse.L.on && keys.CTRL.on && !keys.SHIFT.on && gui.one.bEnable == false && gui.rect_mode == Gui::G_CALC::NONE ) 
					gui.SelectRectRev( mouse.pos );

				// 矩形カーソル開始 削除選択
				if ( !keys.ALT.on && mouse.L.on && keys.CTRL.on && keys.SHIFT.on && gui.one.bEnable == false && gui.rect_mode == Gui::G_CALC::NONE ) 
					gui.SelectRectSub( mouse.pos );

				// 矩形カーソル終了（選択決定）
				if ( !keys.ALT.on && !mouse.L.on && gui.rect_mode != Gui::G_CALC::NONE ) 
					gui.SelectRectEnd( gui.tbls );

				// 矩形カーソル選択	
				if ( !keys.ALT.on && mouse.L.on && gui.rect_mode != Gui::G_CALC::NONE ) 
					gui.SelectRectBegin( pers, gui.tbls , mouse.pos );

				// 単独 新規選択
				if ( !keys.ALT.on && mouse.L.hi && !keys.CTRL.on && !keys.SHIFT.on && gui.one.bEnable && gui.tbls[ gui.one.idxTbl ][ gui.one.idxObj ]->bSelected == false ) 
					gui.SelectOneOnly( gui.tbls );

				// 単独 追加選択
				if ( !keys.ALT.on && mouse.L.hi && !keys.CTRL.on && keys.SHIFT.on && gui.one.bEnable ) 
					gui.SelectOneAdd( gui.tbls );

				// 単独 反転選択
				if ( !keys.ALT.on && mouse.L.hi && keys.CTRL.on && !keys.SHIFT.on && gui.one.bEnable ) 
					gui.SelectOneRev( gui.tbls );

				// 単独 削除選択
				if ( !keys.ALT.on && mouse.L.hi && keys.CTRL.on && keys.SHIFT.on && gui.one.bEnable ) 
					gui.SelectOneSub( gui.tbls );

				// 移動

				if ( !keys.ALT.on && mouse.L.on && !keys.CTRL.on && !keys.SHIFT.on && gui.one.bEnable ) 
				{
					bool bByCamera = false;
					if ( axis.bAxisX && axis.bAxisY && axis.bAxisZ )
					{
						bByCamera = true;
					}
					else
					{
						axis.bAxisX = true;
						axis.bAxisY = false;
						axis.bAxisZ = true;
					}
					gui.MoveObj( gra, pers, gui.tbls, mouse.pos, mouse.prev, mouse.mov, keys.T.on, bByCamera );

					if ( (*pSkeleton).bActive )
					{
						if ( (*pSkeleton).idxTbl == gui.one.idxTbl )
						{
							// キーフレームへ反映
							(*pSkeleton).RefrectKeyframe();
						}
					}
				}

			}

			//=================================
			// skeleton 描画
			//=================================
			util.skeleton_draw( gra, keys, mouse, pers, (*pSkeleton), text_y );

			//=================================
			//	BezierRing 表示
			//=================================
			if ( (*pBezier).bActive ) 
			{
				// マウスベクトル
				vect3 P = pers.calcScreenToWorld3( vect3(mouse.pos,0) );
				vect3 I = pers.calcRayvect( P );

				// 表示 加工 ベジェ 三次曲線
	//				bezier.exec_drawBezier( gra, pers, (*pBezier).tblPoint, (*pBezier).idxPoint, (*pBezier).idxTbl, P, I, keys.E.on, mouse.L.hi );
				(*pBezier).exec_drawBezier( gra, pers, (*pBezier).tblPoint, (*pBezier).idxPoint, (*pBezier).idxTbl, P, I, keys.E.on, mouse.L.hi );

			}

			//=================================
			// コース描画 CutmullRing
			//=================================
			if ( (*pCutmull).bActive )
			{
				// 描画
				(*pCutmull).drawCutmull( gra, pers, (*pCutmull).tblPoint, (*pCutmull).tblIdx );
			}




			//=================================
			// 表示 矩形カーソル、制御点
			//=================================
			gui.DrawController( pers, gra, gui.tbls, mouse.pos );
			
			
			//=================================
			// 情報表示
			//=================================
			gra.Print(1,(float)text_y++,string("fovY:")+to_string(int(pers.fovy)));
			gra.Print(1,(float)text_y++,string("sz:")+to_string(pers.sz) +string(" fy:")+to_string(pers.fy));
			gra.Print(1,(float)text_y++,string("far:")+to_string((pers.cam.pos-pers.cam.at).abs())); 
			gra.Print(1,(float)text_y++,string("at  x=")+to_string(pers.cam.at.x)+string(" y=")+to_string(pers.cam.at.y)+string(" z=")+to_string(pers.cam.at.z) ); 
			gra.Print(1,(float)text_y++,string("pos x=")+to_string(pers.cam.pos.x)+string(" y=")+to_string(pers.cam.pos.y)+string(" z=")+to_string(pers.cam.pos.z) ); 
			gra.Print(1,(float)text_y++,string("peak=")+to_string(time_peak/1000)+string("msec") ); 

			gra.Print(1,(float)text_y++,string("idxTbl=")+to_string(gui.one.idxTbl) ); 
			gra.Print(1,(float)text_y++,string("idxObj=")+to_string(gui.one.idxObj) ); 

			//=================================
			// 描画	マニュピレーター
			//=================================
			axis.DrawAxis( gra, pers, mouse.pos );


			//=================================
			// 描画	Lab
			//=================================
			switch( 3 )
			{
				case 1:	// 描画	グラフ実験
					lab.graph( keys, mouse, gra, pers );
					break;

				case 2:// 描画	角速度 実験
					lab.kakusokudo( keys, mouse, gra, pers );
					break;

				case 3:	// 描画	引力実験
					lab.gravity( keys, mouse, gra, pers );
					break;

				case 4:	// 描画	振り子3D実験
					lab.furiko3d( keys, mouse, gra, pers );
					break;

				case 5:	// 描画	振り子2D実験
					lab.furiko2d( keys, mouse, gra, pers );
					break;

				case 6:	// 描画	タイヤ実験実験
					lab.tire3d( keys, mouse, gra, pers );
					break;
				default:break;
			}

			//=================================
			// 処理時間表示
			//=================================
			{
				static chrono::system_clock::duration time_a;
				static chrono::system_clock::duration time_b;
				static chrono::system_clock::duration time_sec;
				static chrono::system_clock::duration time_now;
				static chrono::system_clock::duration time_max;

				time_b = chrono::system_clock::now().time_since_epoch(); 
				if ( time_max < time_b-time_a ) time_max = time_b-time_a;

				// ウェイト(60fps)
				while( chrono::system_clock::now().time_since_epoch()-time_a < chrono::microseconds(16666) )
				{
	 				this_thread::sleep_for(chrono::microseconds(100));
				}

				// 表示
			    time_now = chrono::system_clock::now().time_since_epoch();
				if ( time_now-time_sec > chrono::seconds(2) ) // n sec毎表示
				{
					time_sec = chrono::system_clock::now().time_since_epoch();

					long long f2 = chrono::duration_cast<chrono::microseconds>(time_max).count();
					time_peak = f2;
					time_max = chrono::seconds(0);
				}
				time_a = chrono::system_clock::now().time_since_epoch();  
			}

		}


		return 0;
	}
};


//------------------------------------------------------------------------------
int main()
//------------------------------------------------------------------------------
{
	cout<<fixed<<setprecision(24);	// 浮動小数出力桁数

	cout << "start" << endl;
	Apr	apr("Ray4 " __DATE__, 300,300,768, 512 );
//	Apr	apr("Ray4 " __DATE__, 300,300,320, 200 );

	apr.SetWincursor( false );

	return apr.main();
}

