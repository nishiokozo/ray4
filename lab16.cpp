//2019/11/22

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
#include <memory>

#include "geom.h"

#include "SysSound.h"
#include "SysKeys.h"
#include "SysMouse.h"
#include "SysGra.h"

#include "func.h"
#include "pers.h"

#include "lab.h"
#include "lab16.h"
#include "skeleton.h"

using namespace std;


struct Lab16::Impl
{
	bool	bResetAll = true;
	bool	bResetParam = true;
	bool	bPause = false;
	bool	bStep = false;

	vector<shared_ptr<Obj>>	tbl_pObj;
	vector<shared_ptr<Edge>>	tbl_pEdge;

	unique_ptr<Skeleton> pSkeleton;
	mat33	mkata = mat33::midentity();
	bool	flgInfo = true;

};

//------------------------------------------------------------------------------
Lab16::Lab16() : pImpl( new Lab16::Impl )
//------------------------------------------------------------------------------
{
}

//------------------------------------------------------------------------------
void Lab16::Update( SysKeys& keys, SysMouse& mouse, SysSound& sound, SysGra& gra, Pers& pers, float delta, int& text_y, Cp& cp )
//------------------------------------------------------------------------------
{
	// 画面クリア
	gra.Clr(rgb(0.3,0.3,0.3));
	pers.grid.DrawGrid3d( gra, pers, vect3(0,0,0), mat33::mrotx(rad(90)), 10, 10, 1, rgb(0.2,0.2,0.2) );
	gra.Print(1,(float)text_y++,"16 : Skeleton" ); 

	//初期化
	if ( pImpl->bResetAll )
	{
		pImpl->bResetAll = false;

		// カメラ
		pers.cam.pos = vect3( 0.0, 2.0, -5.0 );
		pers.cam.at = vect3( 0,  1.0, 0 );
		pers.cam.Update();

		{
			//gui.one.bEnable = false;
			//読み込み
			unique_ptr<Skeleton> pNew(new Skeleton);
			pNew->LoadSkeleton( "bone.mot" );
			pNew->stat.bShowSkin = false;
			pNew->stat.bShowLocus = false;
			pImpl->pSkeleton = move(pNew);

			cp.tbltbl_pObj.clear();
			cp.tbltbl_pEdge.clear();
			cp.tbltbl_pObj.emplace_back( pImpl->pSkeleton->tbl_pObj );
			cp.tbltbl_pEdge.emplace_back( pImpl->pSkeleton->tbl_pEdge );
		}


	}

	//=================================
	// update
	//=================================
	{
		Skeleton& skeleton = (*pImpl->pSkeleton);
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

	//=================================
	// キーフレームロード
	//=================================
	if ( keys.CTRL.on && keys.L.hi ) 
	{
		//読み込み
		unique_ptr<Skeleton> pNew(new Skeleton);
		pNew->LoadSkeleton( "bone.mot" );
		pNew->stat.bShowSkin = false;
		pNew->stat.bShowLocus = false;
		pImpl->pSkeleton = move(pNew);

		cp.tbltbl_pObj.clear();
		cp.tbltbl_pEdge.clear();
		cp.tbltbl_pObj.emplace_back( pImpl->pSkeleton->tbl_pObj );
		cp.tbltbl_pEdge.emplace_back( pImpl->pSkeleton->tbl_pEdge );
	}
	if ( keys.CTRL.on && keys.SEMICOLON.hi ) 
	{
		//読み込み
		unique_ptr<Skeleton> pNew(new Skeleton);
		pNew->LoadSkeleton( "human.mot" );
		pNew->stat.bShowSkin = false;
		pNew->stat.bShowLocus = false;
		pImpl->pSkeleton = move(pNew);

		cp.tbltbl_pObj.clear();
		cp.tbltbl_pEdge.clear();
		cp.tbltbl_pObj.emplace_back( pImpl->pSkeleton->tbl_pObj );
		cp.tbltbl_pEdge.emplace_back( pImpl->pSkeleton->tbl_pEdge );
	}

	//=================================
	// draw
	//=================================
	if ( (*pImpl->pSkeleton).bActive == false ) return;
	{
		Skeleton& skeleton = (*pImpl->pSkeleton);
		vector<shared_ptr<Obj>> tbl_pObj = (*pImpl->pSkeleton).tbl_pObj;


	#if 0 	//剛体実験
		// 優先度つけ
		{
			int n = 0;
			for ( shared_ptr<Obj> po : tbl_pObj )
			{
				Joint* p = dynamic_cast<Joint*>(po.get());

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
			for ( shared_ptr<Obj> po : tbl_pObj )
			{
				Joint* p = dynamic_cast<Joint*>(po.get());

				p->weight = 0.33;
				if ( p->bSelected && mouse.L.on ) p->weight = 0.0;
				n++;
			}
		}
	#endif

		//スケルトン更新
		skeleton.UpdateSkeleton();


		// スケルトン 描画
		skeleton.DrawSkeleton( gra, pers );

		if(1)
		{
			mat33	mmune = mat33::midentity();
			mat33	mhiji = mat33::midentity();;
			mat33	mte = mat33::midentity();
			vect3	pos0 = tbl_pObj[0]->pos;
			vect3	pos1 = tbl_pObj[1]->pos;
			vect3	p2 = tbl_pObj[2]->pos;
			vect3	p3 = tbl_pObj[3]->pos;
			vect3	p4 = tbl_pObj[4]->pos;
			vect3	p5 = tbl_pObj[5]->pos;
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

				pers.prim.DrawBox( gra, pers, pos0, mmune, false, false );
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

				if ( keys.CTRL.on )		pImpl->mkata.rotateByAxis( (p4-p2).normalize(), rad(0.5));
				if ( keys.SHIFT.on )	pImpl->mkata.rotateByAxis( (p4-p2).normalize(), rad(-0.5));
				
				pers.prim.DrawBox( gra, pers, p2, pImpl->mkata, false, false );
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
				pers.prim.DrawDrum( gra, pers, p4, mhiji );
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
				pers.prim.DrawDrum( gra, pers, p5, mte );
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
						gra.Fill2d( gra.Conv(v), gra.Conv(v+vect2(3,7)), rgb(1,1,1) );
					}

					if ( y == skeleton.cur.act && x == skeleton.cur.pose )
					{
						gra.Fill2d( gra.Conv(v+vect2(0,4)), gra.Conv(v+vect2(3,7)), rgb(1,0,0) );
					}
					
					if ( flg )
					{
						gra.Fill2d( gra.Conv(v+vect2(0,4)), gra.Conv(v+vect2(3,7)), rgb(1,0,0) );
					}
				}
			}

			gra.SetZTest( true );
		}


		{
			if( keys.F2.hi ) pImpl->flgInfo = !pImpl->flgInfo;
			if ( pImpl->flgInfo )
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

	// 1/60sec 待
	gra.Wait();
}
