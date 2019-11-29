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
#include <random>

#include "geom.h"

#include "SysKeys.h"
#include "SysMouse.h"
#include "SysGra.h"

#include "func.h"
#include "pers.h"

#include "lab.h"
#include "lab6.h"
//------------------------------------------------------------------------------
void Lab6::Update( SysKeys& keys, SysMouse& mouse, SysGra& gra, Pers& pers, float delta, int& text_y, Cp& cp )
//------------------------------------------------------------------------------
{
	//画面クリア
	gra.Clr(rgb(0.3,0.3,0.3));
	pers.grid.DrawGrid3d( gra, pers, vect3(0,0,0), midentity(), 10, 10, 1, rgb(0.2,0.2,0.2) );

	gra.Print(1,(float)text_y++,string("<<lab6_tire3d>>")); 

	const vect3 G_pos = vect3(0,2.0,0);
	const vect3 G_acc= vect3(0,0,0.0);
	const float G_radius = 0.5;
	const float G_head = deg2rad(35);	//	タイヤの方向
	const float G_bank = deg2rad(40);	//	回転角
	const float G_rspd = deg2rad(0);	//	角速度
	const float G = 9.80665;				// 重力加速度
	const float grate = 9.80665 / 60/60;	// 重力加速度

	static vect3 pos = G_pos;
	static vect3 acc1 = G_acc;
	static vect3 acc2 = G_acc;
	static float radius = G_radius;
	static float head = G_head;
	static float bank = G_bank;
	static float rspd = G_rspd;

	
	// 動き
	{
		// 強制
		{
			// リセット
			if ( keys.R.hi ) 
			{
				pos = G_pos;
				acc1 = G_acc;
				acc2 = G_acc;
				radius = G_radius;
				head = G_head;
				bank = G_bank;
				rspd = G_rspd;
			}
			if ( mouse.F.hi ) 
			{
				acc1 += vect3( sin(head), 0, cos(head) )*0.02;
				acc2 += vect3( sin(head), 0, cos(head) )*0.02;
			}
			if ( mouse.B.hi ) rspd += deg2rad(5);
			if ( mouse.R.hi ) rspd += deg2rad(-5);
		}

		// 重力
		{
			acc1.y -= grate;
			acc2.y -= grate;
//						pos += (acc1+acc2)/2 ;
		}

		// 回転
		{
			bank += rspd;
		}
	}

	float high = abs(radius*cos(bank));
	
	// 衝突	地面
	if ( high > pos.y ) 
	{

		pos.y = high;

		// バウンド
		if ( bank > 0 )
		{
		}
		else
		{
		}
//						vel.y = -vel.y * 0.5; 
//					vel.y = 0;
#if 0
		// 回転
		{
			float a = atan( vel.y * sin(bank) / radius );
			rspd = a;
		}
		// 衝突 bank
		if ( bank > deg2rad(90) ) bank -= deg2rad(180);
		if ( bank < deg2rad(-90) ) bank += deg2rad(180);
#endif
	}
	
	// 設置地面
	if ( high == pos.y ) 
	{
		// 衝突 bank
		if ( bank > deg2rad(90) ) bank -= deg2rad(180);
		if ( bank < deg2rad(-90) ) bank += deg2rad(180);

		// 回転
		{
			float a = atan( grate * sin(bank) / radius );
			rspd += a;
		}
	}

	// 減衰	空気抵抗
	{
//						rspd *= 0.99;
	}

	// 描画
	pers.prim.DrawTire( gra, pers, pos, head, bank, radius );
}
