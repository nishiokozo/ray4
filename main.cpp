// 2017/07/07 ray3
// 2019/06/25 ray4

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
#include <iomanip> // for setprecision
#include <tuple>

#include "geom.h"

#include "SysSound.h"
#include "SysKeys.h"
#include "SysMouse.h"
#include "SysGra.h"
#include "Sys.h"

#include "func.h"
#include "pers.h"
#include "gui.h"

#include "lab.h"

using namespace std;

struct Apr : public Sys
{
	Gui gui;
	Lab lab;
	Pers pers;
	SysKeys		keys;
	SysMouse	mouse;
	SysSound	sound;
	SysGra gra;

	//------------------------------------------------------------------------------
	int main( int num )
	//------------------------------------------------------------------------------
	{
		lab.m.idxLab = num;
		lab.SetIdx(lab.m.idxLab,gui.cp);
		
		//	ウィンドウ生成関数
		auto funcOnCreate = [&]()
		{
			gra.OnCreate();
		};
		// ウィンドウサイズ変更関数
		auto funcOnSize = [&]( int width, int height )
		{
			gra.OnSize( width, height );
		};
		// ウィンドウペイント関数
		auto funcOnPaint = [&]()
		{
			gra.OnPaint();
		};
		// ウィンドウ破棄関数
		auto funcOnDestroy = [&]()
		{
			gra.OnDestroy();
		};
		SetWincursor( false );
	
		InitWinapi(
			funcOnCreate,
			funcOnSize,
			funcOnPaint,
			funcOnDestroy
		);
		OpenWindow( "Ray4 " __DATE__, 300,300,768, 512 );

		int text_y = 0;

		//===========================================================================
		while( Update() )
		{
			keys.Update();
			mouse.Update();

			if ( keys._9.hi )
			{
				sound.mml_play( "T100v6o4c1-gcfcfgdg+c7r:v11o3g1eg+ca+c+db+d+g7r:v11o3e1ceafabgb+e7r");
			}

			text_y = 0;

			//=================================
			// カメラ
			//=================================
			{
				// パースペクティブ
				if (keys.Y.hi) {pers.fovy-=2;cout << pers.fovy <<" "<<1/tan(deg2rad(pers.fovy)) << endl; }
				if (keys.H.hi) {pers.fovy+=2;cout << pers.fovy <<" "<<1/tan(deg2rad(pers.fovy)) << endl; }

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
				if ( keys.F.hi )	pers.Focus( gui.cp.tbltbl_pObj );
				
				// カメラマトリクス計算
				pers.cam.Update();
			}

			//=================================
			// カーソルモード X/Y/Z軸選択モード切替
			//=================================

			if ( keys.Z.hi + keys.X.hi + keys.C.hi > 0 && keys.Z.on + keys.X.on + keys.C.on == keys.Z.hi + keys.X.hi + keys.C.hi ) 
			{
				// 排他的選択
				pers.axis.bAxisZ = false;
				pers.axis.bAxisX = false;
				pers.axis.bAxisY = false;
				if ( keys.Z.hi ) pers.axis.bAxisZ = true;
				if ( keys.X.hi ) pers.axis.bAxisX = true;
				if ( keys.C.hi ) pers.axis.bAxisY = true;
			}
			else
			{
				// 追加選択
				if ( keys.Z.on ) pers.axis.bAxisZ = true;
				if ( keys.X.on ) pers.axis.bAxisX = true;
				if ( keys.C.on ) pers.axis.bAxisY = true;
			}

			//=================================
			// 実験 on/off
			//=================================
			if ( keys._0.hi ) 
			{
				gui.bShowPoint = !gui.bShowPoint;
			}



			//=================================
			// 描画	Lab
			//=================================
			{
				const float	T	= 1.0/60.0;				// 時間/frame

//				if ( keys.CTRL.on && mouse.B.hi ) {gui.one.bEnable = false;lab.SetNextIdx(-1,gui.cp);};
//				if ( keys.CTRL.on && mouse.F.hi ) {gui.one.bEnable = false;lab.SetNextIdx(+1,gui.cp);};
				if ( keys.B.hi ) {gui.one.bEnable = false;lab.SetNextIdx(-1,gui.cp);};
				if ( keys.N.hi ) {gui.one.bEnable = false;lab.SetNextIdx(+1,gui.cp);};
				lab.Update( keys, mouse, sound, gra, pers, T, text_y, gui.cp );
			}
			

			//=================================
			//	GUI操作
			//=================================
			{

				// 最近点検索
				if ( !keys.ALT.on && mouse.L.hi ) 
					gui.TouchFirst( gra, pers, gui.cp.tbltbl_pObj, mouse.pos );

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
					gui.SelectRectEnd( gui.cp.tbltbl_pObj );

				// 矩形カーソル選択	
				if ( !keys.ALT.on && mouse.L.on && gui.rect_mode != Gui::G_CALC::NONE ) 
					gui.SelectRectBegin( pers, gui.cp.tbltbl_pObj , mouse.pos );

				// 単独 新規選択
				if ( !keys.ALT.on && mouse.L.hi && !keys.CTRL.on && !keys.SHIFT.on && gui.one.bEnable && gui.cp.tbltbl_pObj[ gui.one.idxTbl ][ gui.one.idxObj ]->bSelected == false ) 
					gui.SelectOneOnly( gui.cp.tbltbl_pObj );

				// 単独 追加選択
				if ( !keys.ALT.on && mouse.L.hi && !keys.CTRL.on && keys.SHIFT.on && gui.one.bEnable ) 
					gui.SelectOneAdd( gui.cp.tbltbl_pObj );

				// 単独 反転選択
				if ( !keys.ALT.on && mouse.L.hi && keys.CTRL.on && !keys.SHIFT.on && gui.one.bEnable ) 
					gui.SelectOneRev( gui.cp.tbltbl_pObj );

				// 単独 削除選択
				if ( !keys.ALT.on && mouse.L.hi && keys.CTRL.on && keys.SHIFT.on && gui.one.bEnable ) 
					gui.SelectOneSub( gui.cp.tbltbl_pObj );

				// 移動

				if ( !keys.ALT.on && mouse.L.on && !keys.CTRL.on && !keys.SHIFT.on && gui.one.bEnable ) 
				{
					bool bByCamera = false;
					bool bByFloor = false;
					bool bByXY = false;
					if ( pers.axis.bAxisX && pers.axis.bAxisY && pers.axis.bAxisZ )
					{
						// カメラに並行
						bByCamera = true;
					}
					else
					if ( pers.axis.bAxisX && pers.axis.bAxisY )
					{
						// XYに平行
						bByXY = true;
					}
					else
					if ( pers.axis.bAxisX && pers.axis.bAxisZ )
					{
						// 床に平行
						bByFloor = true;
					}
					else
					{
						bByCamera = true;
					}
					gui.MoveObj( gra, pers, gui.cp.tbltbl_pObj, mouse.pos, mouse.prev, mouse.mov, keys.T.on, bByCamera, bByFloor, bByXY );
/*
					if ( (*pSkeleton).bActive )
					{
						if ( (*pSkeleton).m.idxTbl == gui.one.idxTbl )
						{
							// キーフレームへ反映
							(*pSkeleton).RefrectKeyframe();
						}
					}
*/
				}

			}

			//=================================
			// 表示 矩形カーソル、制御点
			//=================================
			gui.DrawController( gra, pers, gui.cp.tbltbl_pObj, gui.cp.tbltbl_pEdge, mouse.pos );
			

			//=================================
			// 描画	マニュピレーター
			//=================================
			pers.axis.DrawAxis( gra, pers, mouse.pos );

			//=================================
			// 情報表示
			//=================================
		//	gra.Print(1,(float)text_y++,string("fovY:")+to_string(int(pers.fovy)));
		//	gra.Print(1,(float)text_y++,string("peak: ")+to_string(gra.time_peak/1000.0)+string("ms") ); 
			if ( keys.ALT.on )
			{
				vect3	pos = pers.cam.pos;
				vect3	at = pers.cam.at;
				gra.Print(1,(float)text_y++,string("cam.pos: ")+to_string(pos.x)+" , "+to_string(pos.y)+" , "+to_string(pos.z)); 
				gra.Print(1,(float)text_y++,string("cam.at : ")+to_string( at.x)+" , "+to_string( at.y)+" , "+to_string( at.z)); 
				
				
			}
			if ( gui.one.bEnable )
			{
				gra.Print(1,(float)text_y++,string("m.idxTbl=")+to_string(gui.one.idxTbl)+":"+to_string(gui.one.idxObj) ); 
//				Obj* p =gui.cp.tbltbl_pObj[ gui.one.idxTbl ][ gui.one.idxObj ];
//				if ( p ) gra.Print(1,(float)text_y++,string("one: ")+to_string(o.pos.x)+" , "+to_string(o.pos.y)+" , "+to_string(o.pos.z)); 
				Obj& o = *gui.cp.tbltbl_pObj[ gui.one.idxTbl ][ gui.one.idxObj ];
				gra.Print(1,(float)text_y++,string("one: ")+to_string(o.pos.x)+" , "+to_string(o.pos.y)+" , "+to_string(o.pos.z)); 
			}

			// 描画
			gra.Flush();

		}


		return 0;
	}
};


//------------------------------------------------------------------------------
int main( int argc, char* argv[] )
//------------------------------------------------------------------------------
{
	int num = 0; 
	
	if ( argc > 0 )
	{
		num = atoi(argv[1]);
		cout << num << endl;
	}

	cout<<fixed<<setprecision(24);	// 浮動小数出力桁数

	cout << "start" << endl;
	Apr	apr;



	return apr.main( num );
}

