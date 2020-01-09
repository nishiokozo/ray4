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

#include "geom.h"

#include "SysSound.h"
#include "SysKeys.h"
#include "SysMouse.h"
#include "SysGra.h"

#include "func.h"
#include "pers.h"

#include "lab.h"
#include "lab31.h"

using namespace std;

struct Lab31::Impl
{
	bool	bResetAll = true;
	bool	bResetParam = true;
	bool	bPause = false;
	bool	bStep = false;

	vector<shared_ptr<Obj>>	tbl_pObj;

	struct PN
	{
		vect3	pos;
		vect3	norm;
	};

	vector<PN>tblVert;
};
Lab31::Lab31() : pImpl( new Lab31::Impl ){}

//------------------------------------------------------------------------------
void Lab31::Update( SysKeys& keys, SysMouse& mouse, SysSound& sound, SysGra& gra, Pers& pers, float delta, int& text_y, Cp& cp )
//------------------------------------------------------------------------------
{
	// 画面クリア
	gra.Clr(rgb(0.3,0.3,0.3));
//	pers.grid.DrawGrid3d( gra, pers, vect3(0,0,0), mat33::midentity(), 10, 10, 1, rgb(0.2,0.2,0.2) );
	gra.Print(1,(float)text_y++,"31 : .obj point rendering" ); 

	//初期化
	if ( pImpl->bResetAll )
	{
		pImpl->bResetAll = false;

		// カメラ
		pers.cam.pos = vect3( -4.0, 4.0, -5.0 );
		pers.cam.at = vect3( 0,  1.0, 0 );
		pers.cam.Update();

		pImpl->tbl_pObj.clear();
		pImpl->tbl_pObj.emplace_back( new Obj(vect3(  0, 3,  0.0 )) );
		cp.tbltbl_pObj.emplace_back( pImpl->tbl_pObj );

		{
			fstream fi( "bunny.obj", ios::in );
			string buf;
			int cntV = 0;
			int cntVN = 0;
			while ( getline( fi, buf ) )
			{
				vector<string> list = func_split( buf, ' ' );
				
				if ( list.size() )
				{
					if ( list[0] == "v" )
					{
						float v0 = stof(list[1]);
						float v1 = stof(list[2]);
						float v2 = stof(list[3]);
						pImpl->tblVert.emplace_back();
						pImpl->tblVert[cntV++].pos = vect3(v0,v1,v2);
					}
					if ( list[0] == "vn" )
					{				
						float v0 = stof(list[1]);
						float v1 = stof(list[2]);
						float v2 = stof(list[3]);
//						pImpl->tblNorm.emplace_back( v0,v1,v2);
						pImpl->tblVert[cntVN++].norm = vect3(v0,v1,v2)+vect3(1,1,1);
					}
				}	
			}
			
			cout << "vert:" << pImpl->tblVert.size() << endl;
				
		}
	}
	
	vect3	light = pImpl->tbl_pObj[0]->pos.normalize();
	
	for ( const Impl::PN& v : pImpl->tblVert )
	{
		vect3 n = v.norm-vect3(1,1,1);
		float a = dot(light,n);
		float d = max(0,a);
		rgb	col=rgb(d,d,d);
//	col = n;
		if ( a >0 )
		pers.pen.pset3d( gra, pers, v.pos, col,3 );

	}

}
