//2019/10/17

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

//------------------------------------------------------------------------------
void LabObj::M::drawVect( SysGra& gra, Pers& pers, int& text_y, vect3 v0, vect3 v, float sc, rgb col, string str )
//------------------------------------------------------------------------------
{
	static const rgb col0 = rgb( 0, 0, 0 );
	static const rgb col1 = rgb( 0, 0, 1 );
	static const rgb col2 = rgb( 1, 0, 0 );
	static const rgb col3 = rgb( 1, 0, 1 );
	static const rgb col4 = rgb( 0, 1, 0 );
	static const rgb col5 = rgb( 0, 1, 1 );
	static const rgb col6 = rgb( 1, 1, 0 );
	static const rgb col7 = rgb( 1, 1, 1 );

	gra.SetZTest(false);

	vect3 v1 = v0+v*sc;

	// 影
	{
		vect3	a = v0;	a.y=0;
		vect3	b = v1;	b.y=0;
		rgb		c = (col+rgb(0.75,0.75,0.75))/4;
		pers.pen.line3d( gra, pers, a, b, c, 1 );
		pers.pen.pset3d( gra, pers,    b, c, 5 );
	}

	// 線
	pers.pen.line3d( gra, pers, v0, v1, col, 1 );
	pers.pen.pset3d( gra, pers,     v1, col, 5 );
	pers.pen.print3d( gra, pers, 	v1,12,0, str ); 

	// 矢印
	if (0)
	{

		vect2 a = pers.calcWorldToScreen3( v0 ).xy();
		vect2 b = pers.calcWorldToScreen3( v1 ).xy();
//		if ( b.z > 0  ) 
		{
			vect3 c = vect3((b-a).normalize(),0);
			vect2 d = cross(c,vect3(0,0,1)).xy();
			vect2 e = (d+(b-a))*100;
//			gra.Line( b, b + gra.Dot(-e.x,e.y), col6, 2 );
			gra.Line( b, b + d*1, col6, 2 );
		}
	}

	gra.Print(1,(float)text_y++, str+": "+to_string(v.x)+" , "+to_string(v.y)+" , "+to_string(v.z));

	gra.SetZTest(true);
};



//------------------------------------------------------------------------------
void Lab::SetIdx( int n, Cp& cp )
//------------------------------------------------------------------------------
{
cout << m.idxLab << ">" << n << endl;
	m.idxLab = max( n, 0 ); 

	cp.tbltbl_pObj.clear();
	cp.tbltbl_pEdge.clear();

	if ( pLab ) 
	{
		delete pLab;
		pLab=0;
	};

	switch( m.idxLab )
	{
		case 0:		pLab = new Lab0;	break;
		case 1:		pLab = new Lab1;	break;
		case 2:		pLab = new Lab2;	break;
		case 3:		pLab = new Lab3;	break;
		case 4:		pLab = new Lab4;	break;
		case 5:		pLab = new Lab5;	break;
		case 6:		pLab = new Lab6;	break;
		case 7:		pLab = new Lab7;	break;
		case 8:		pLab = new Lab8;	break;
		case 9:		pLab = new Lab9;	break;
		case 10:	pLab = new Lab10;	break;
		case 11:	pLab = new Lab11;	break;
		case 12:	pLab = new Lab12;	break;
		case 13:	pLab = new Lab13;	break;
		case 14:	pLab = new Lab14;	break;
		case 15:	pLab = new Lab15;	break;
		case 16:	pLab = new Lab16;	break;
		default: break;
	}
}

//------------------------------------------------------------------------------
void Lab::SetNextIdx( int val, Cp& cp )
//------------------------------------------------------------------------------
{
	static	int	tbl[] = 
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
	};	
	static const int size = static_cast<signed>(sizeof(tbl)/sizeof(int));

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
void Lab::Update( SysKeys& keys, SysMouse& mouse, SysGra& gra, Pers& pers, float delta, int& text_y, Cp& cp )
//------------------------------------------------------------------------------
{
	if ( pLab ) pLab->Update( keys, mouse, gra, pers, delta, text_y, cp );
}

//------------------------------------------------------------------------------
Lab::Lab()
//------------------------------------------------------------------------------
{
	m.idxLab = 13;
}

