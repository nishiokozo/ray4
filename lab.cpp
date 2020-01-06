//2019/10/17

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
#include "lab0.h"
#include "lab1.h"
#include "lab2.h"
#include "lab3.h"
#include "lab4.h"
#include "lab5.h"
#include "lab6.h"
#include "lab7.h"
#include "lab8.h"
#include "lab9.h"
#include "lab10.h"
#include "lab11.h"
#include "lab12.h"
#include "lab13.h"
#include "lab14.h"
#include "lab15.h"
#include "lab16.h"
#include "lab17.h"
#include "lab18.h"
#include "lab19.h"
#include "lab20.h"
#include "lab21.h"
#include "lab22.h"
#include "lab23.h"
#include "lab24.h"
#include "lab25.h"
#include "lab26.h"
#include "lab27.h"
#include "lab28.h"
#include "lab29.h"

//------------------------------------------------------------------------------
void Lab::SetIdx( int n, Cp& cp )
//------------------------------------------------------------------------------
{
//cout << m.idxLab << ">" << n << endl;
	m.idxLab = max( n, 0 ); 

	cp.tbltbl_pObj.clear();
	cp.tbltbl_pEdge.clear();

	if ( pLab ) 
	{
		pLab = 0;	// 開放
	};

	switch( m.idxLab )
	{
		case 0:		pLab = unique_ptr<LabObj>( new Lab0 );	break;
		case 1:		pLab = unique_ptr<LabObj>( new Lab1 );	break;
		case 2:		pLab = unique_ptr<LabObj>( new Lab2 );	break;
		case 3:		pLab = unique_ptr<LabObj>( new Lab3 );	break;
		case 4:		pLab = unique_ptr<LabObj>( new Lab4 );	break;
		case 5:		pLab = unique_ptr<LabObj>( new Lab5 );	break;
		case 6:		pLab = unique_ptr<LabObj>( new Lab6 );	break;
		case 7:		pLab = unique_ptr<LabObj>( new Lab7 );	break;
		case 8:		pLab = unique_ptr<LabObj>( new Lab8 );	break;
		case 9:		pLab = unique_ptr<LabObj>( new Lab9 );	break;
		case 10:	pLab = unique_ptr<LabObj>( new Lab10 );	break;
		case 11:	pLab = unique_ptr<LabObj>( new Lab11 );	break;
		case 12:	pLab = unique_ptr<LabObj>( new Lab12 );	break;
		case 13:	pLab = unique_ptr<LabObj>( new Lab13 );	break;
		case 14:	pLab = unique_ptr<LabObj>( new Lab14 );	break;
		case 15:	pLab = unique_ptr<LabObj>( new Lab15 );	break;
		case 16:	pLab = unique_ptr<LabObj>( new Lab16 );	break;
		case 17:	pLab = unique_ptr<LabObj>( new Lab17 );	break;
		case 18:	pLab = unique_ptr<LabObj>( new Lab18 );	break;
		case 19:	pLab = unique_ptr<LabObj>( new Lab19 );	break;
		case 20:	pLab = unique_ptr<LabObj>( new Lab20 );	break;
		case 21:	pLab = unique_ptr<LabObj>( new Lab21 );	break;
		case 22:	pLab = unique_ptr<LabObj>( new Lab22 );	break;
		case 23:	pLab = unique_ptr<LabObj>( new Lab23 );	break;
		case 24:	pLab = unique_ptr<LabObj>( new Lab24 );	break;
		case 25:	pLab = unique_ptr<LabObj>( new Lab25 );	break;
		case 26:	pLab = unique_ptr<LabObj>( new Lab26 );	break;
		case 27:	pLab = unique_ptr<LabObj>( new Lab27 );	break;
		case 28:	pLab = unique_ptr<LabObj>( new Lab28 );	break;
		case 29:	pLab = unique_ptr<LabObj>( new Lab29 );	break;
		default: break;
	}
}

//------------------------------------------------------------------------------
void Lab::SetNextIdx( int val, Cp& cp )
//------------------------------------------------------------------------------
{
	int	tbl[] = 
	{
		0,
		10,		//	カラータワー
		29,		//	レイトレーシング
		14,		//	Catmull rom spline
		15,		//	Bezier
		27,		//	B-Spline 2次 3次(uniform)
		28,		//	B-Spline 3次
		16,		//	スケルトンアニメーション
		1,		//	加速度グラフ
		2,		//	軸回り回転
		3,		//	衛生シミュレーション
		4,		//	角速度実験1
		5,		//	角速度実験2
		6,		//	タイヤ
		7,		//	角速度実験3
		8,		//	角速度実験4
		13,		//	曲線と平面の衝突
		11,		//	ボールの落下と床
		12,		//	ボール同士の衝突
		9,		//	ボール同士の衝突と落下
		17,		//	ボールと板とのスピン
		18,		//	ボールのスピン
		19,		//	モーター・トルク実験
		20,		//	2D モーター・トルク実験
		21,		//	2D ポイント・トルク実験
		22,		//	2D 箱・質量衝突実験
		23,		//	1D 箱・質量衝突実験
		24,		//	Bezier Surface surface(one)
		25,		//	Bezier Surface Cource
		26,		//	Bezier Surface Edit
	};	
	const int size = static_cast<signed>(sizeof(tbl)/sizeof(int));

	int num = -1;
	for ( int i = 0 ; i < size ; i++ )
	{
		if ( tbl[i] == m.idxLab )
		{
			num = i;
			break;
		}
	}

	if ( num >= 0 )
	{
		num = num +val;
		if ( num >= 0 && num < size )
		{
			SetIdx( tbl[num], cp );
		}
	}
}

//------------------------------------------------------------------------------
void Lab::Update( SysKeys& keys, SysMouse& mouse, SysSound& sound, SysGra& gra, Pers& pers, float delta, int& text_y, Cp& cp )
//------------------------------------------------------------------------------
{
	if ( pLab ) pLab->Update( keys, mouse, sound, gra, pers, delta, text_y, cp );
}
