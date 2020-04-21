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
#include "lab34.h"

using namespace std;

struct Lab34::Impl
{
	bool	bResetAll = true;
	bool	bResetParam = true;
	bool	bPause = false;
	bool	bStep = false;

	vector<shared_ptr<Obj>>	tbl_pObj;

	struct PN
	{
		vect3	pos;
		vect3	col;
	};

	vector<PN>tblVert;
};
Lab34::Lab34() : pImpl( new Lab34::Impl ){}

//------------------------------------------------------------------------------
void Lab34::Update( SysKeys& keys, SysMouse& mouse, SysSound& sound, SysGra& gra, Pers& pers, float delta, int& text_y, Cp& cp )
//------------------------------------------------------------------------------
{
	// 画面クリア
	gra.Clr(rgb(0.3,0.3,0.3));
//	pers.grid.DrawGrid3d( gra, pers, vect3(0,0,0), mat33::midentity(), 10, 10, 1, rgb(0.2,0.2,0.2) );
	gra.Print(1,(float)text_y++,"34 : .xyz point cloud rendering" ); 

	//初期化
	if ( pImpl->bResetAll )
	{
		pImpl->bResetAll = false;

		// カメラ
		pers.cam.pos = vect3( 0.0, 600.0, -1000.0 );
		pers.cam.at = vect3( 0,  8.0, 0 );
		pers.cam.Update();

		pImpl->tbl_pObj.clear();
		pImpl->tbl_pObj.emplace_back( new Obj(vect3(  0, 3,  0.0 )) );
		cp.tbltbl_pObj.emplace_back( pImpl->tbl_pObj );

		{
			fstream fi( "PC_Demo-light.XYZ", ios::in );
			string buf;
			getline( fi, buf ); // 頂点数
			while ( getline( fi, buf ) )
			{
				//頂点間引き
				vector<string> list = func_split( buf, ',' );
			
				float v0 = stof(list[0]);
				float v1 = stof(list[1]);
				float v2 = stof(list[2]);
				float tm = stof(list[3]);
				float r = stof(list[4])/255.0;
				float g = stof(list[5])/255.0;
				float b = stof(list[6])/255.0;
				pImpl->tblVert.emplace_back();
				int n= (signed)pImpl->tblVert.size()-1;
				pImpl->tblVert[n].pos = vect3(v0,v1,v2);
				pImpl->tblVert[n].col = vect3(r,g,b);
			}
			
			cout << "vert:" << pImpl->tblVert.size() << endl;
		}
		#if 0
			cout<<fixed<<setprecision(6);	// 浮動小数出力桁数
			for ( const Impl::PN& v : pImpl->tblVert )
			{
				cout << v.pos.x <<","<< v.pos.y <<","<< v.pos.z <<",255,"<< int(v.col.r*255) <<"," << int(v.col.g*255) <<"," << int(v.col.b*255) << endl;
			}
		#endif

	}
	
	//--
	
	for ( const Impl::PN& v : pImpl->tblVert )
	{
	
		pers.pen.Pset3d( gra, pers, v.pos, v.col,2 );

	}

}
