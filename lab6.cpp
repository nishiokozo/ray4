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

#include "geom.h"

#include "SysSound.h"
#include "SysKeys.h"
#include "SysMouse.h"
#include "SysGra.h"

#include "func.h"
#include "pers.h"

#include "lab.h"
#include "lab6.h"

using namespace std;

struct Lab6::Impl
{
	vect3 G_pos = vect3(0,2.0,0);
	vect3 G_acc= vect3(0,0,0.0);
	float G_radius = 0.5;
	float G_head = rad(35);	//	タイヤの方向
	float G_bank = rad(40);	//	回転角
	float G_rspd = rad(0);	//	角速度
	float G = 9.80665;				// 重力加速度
	float grate = 9.80665 / 60/60;	// 重力加速度

	vect3 pos = G_pos;
	vect3 acc1 = G_acc;
	vect3 acc2 = G_acc;
	float radius = G_radius;
	float head = G_head;
	float bank = G_bank;
	float rspd = G_rspd;

};
Lab6::Lab6() : pImpl( new Lab6::Impl ){}

//------------------------------------------------------------------------------
void Lab6::Update( SysKeys& keys, SysMouse& mouse, SysSound& sound, SysGra& gra, Pers& pers, float delta, int& text_y, Cp& cp )
//------------------------------------------------------------------------------
{
	//画面クリア
	gra.Clr(rgb(0.3,0.3,0.3));
	pers.grid.DrawGrid3d( gra, pers, vect3(0,0,0), mat33::mrotx(rad(90)), 10, 10, 1, rgb(0.2,0.2,0.2) );

	gra.Print(1,(float)text_y++,string("<<lab6_tire3d>>")); 



	
	// 動き
	{
		// 強制
		{
			// リセット
			if ( keys.R.hi ) 
			{
				pImpl->pos = pImpl->G_pos;
				pImpl->acc1 = pImpl->G_acc;
				pImpl->acc2 = pImpl->G_acc;
				pImpl->radius = pImpl->G_radius;
				pImpl->head = pImpl->G_head;
				pImpl->bank = pImpl->G_bank;
				pImpl->rspd = pImpl->G_rspd;
			}
			if ( mouse.F.hi ) 
			{
				pImpl->acc1 += vect3( sin(pImpl->head), 0, cos(pImpl->head) )*0.02;
				pImpl->acc2 += vect3( sin(pImpl->head), 0, cos(pImpl->head) )*0.02;
			}
			if ( mouse.B.hi ) pImpl->rspd += rad(5);
			if ( mouse.R.hi ) pImpl->rspd += rad(-5);
		}

		// 重力
		{
			pImpl->acc1.y -= pImpl->grate;
			pImpl->acc2.y -= pImpl->grate;
//						pImpl->pos += (pImpl->acc1+pImpl->acc2)/2 ;
		}

		// 回転
		{
			pImpl->bank += pImpl->rspd;
		}
	}

	float high = abs(pImpl->radius*cos(pImpl->bank));
	
	// 衝突	地面
	if ( high > pImpl->pos.y ) 
	{

		pImpl->pos.y = high;

		// バウンド
		if ( pImpl->bank > 0 )
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
			float a = atan( vel.y * sin(pImpl->bank) / pImpl->radius );
			pImpl->rspd = a;
		}
		// 衝突 pImpl->bank
		if ( pImpl->bank > rad(90) ) pImpl->bank -= rad(180);
		if ( pImpl->bank < rad(-90) ) pImpl->bank += rad(180);
#endif
	}
	
	// 設置地面
	if ( high == pImpl->pos.y ) 
	{
		// 衝突 pImpl->bank
		if ( pImpl->bank > rad(90) ) pImpl->bank -= rad(180);
		if ( pImpl->bank < rad(-90) ) pImpl->bank += rad(180);

		// 回転
		{
			
			float a = atan( pImpl->grate * sin(pImpl->bank) / pImpl->radius );
			pImpl->rspd += a;
		}
	}

	// 減衰	空気抵抗
	{
//						pImpl->rspd *= 0.99;
	}

	// 描画
	pers.prim.DrawTire( gra, pers, pImpl->pos, pImpl->head, pImpl->bank, pImpl->radius );


	// 1/60sec 待
	gra.Wait();
}
