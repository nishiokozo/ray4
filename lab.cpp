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

//------------------------------------------------------------------------------
void LabObj::M::drawVect( SysGra& gra, Pers& pers, int& text_y, vect3 v0, vect3 v, float sc, rgb col, string str, bool bShadow, bool bDump, bool bFlip )
//------------------------------------------------------------------------------
{
	gra.SetZTest(false);

	vect3 v1 = v0+v*sc;

	// 影
	if ( bShadow )
	{
		vect3	a = v0;	a.y=0;
		vect3	b = v1;	b.y=0;
		rgb		c = (col+rgb(0.75,0.75,0.75))/4;
		pers.pen.line3d( gra, pers, a, b, c, 1 );
		pers.pen.pset3d( gra, pers,    b, c, 5 );
	}

	// 線
	pers.pen.line3d( gra, pers, v0, v1, col, 1 );
//	pers.pen.pset3d( gra, pers,     v1, col, 5 );

	if ( bFlip )
	{
		pers.pen.print3d( gra, pers, 	v1,12,0, str ); 
	}
	else
	{
		pers.pen.print3d( gra, pers, 	v0,0,36, str ); 
	}

	// 矢印
	#if 1
	{
		gra.SetCulling( false );
		// 2D矢印 実験
		{
			vect2 a0 = pers.calcWorldToScreen3( v0 ).xy();
			vect2 a1 = pers.calcWorldToScreen3( v1 ).xy();

			vect2 v = (a1-a0).normalize();
			vect2 n = cross( vect3(v,0), vect3(0,0,1) ).xy();

			vect2 a = a1;
			vect2 b = a1-n*0.1/7 - v*0.2/7;
			vect2 c = a1+n*0.1/7 - v*0.2/7;
			gra.Tri( a,b,c, col );
//			gra.Line( a,b, rgb(1,1,0), 1 );
//			gra.Line( a,c, rgb(1,1,0), 1 );
//			gra.Line( c,b, rgb(1,1,0), 1 );

		}
		gra.SetCulling( true );
	}
	#else
	{
		gra.SetCulling( false );
		// 3D矢印 実験
		{
			vect3 v = v1-v0;
			vect3 n = cross( v, vect3(0,0,1) ).normalize();

			vect3 a = v1;
			vect3 b = v1-n*0.1 - v.normalize()*0.2;
			vect3 c = v1+n*0.1 - v.normalize()*0.2;
			pers.pen.tri3d( gra, pers, a,b,c, col );
		}
		gra.SetCulling( true );
	}
	#endif
	
	if ( bDump )
	{
		gra.Print(1,(float)text_y++, str+": "+to_string(v.x)+" , "+to_string(v.y)+" , "+to_string(v.z));
	}

	gra.SetZTest(true);
};



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
		14,		//	コース
		15,		//	ベジェ
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
		22,		//	2D 棒・トルク実験
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
