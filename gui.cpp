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

#include "geom.h"


#include "SysKeys.h"
#include "SysMouse.h"
#include "SysGra.h"
#include "Sys.h"

#include "func.h"
#include "pers.h"

//#include "raytrace.h"
//#include "skeleton.h"
#include "gui.h"
//------------------------------------------------------------------------------
void Gui::TouchFirst( SysGra& gra, Pers& pers, vector<vector<Obj*>>& tbls, vect2 mpos )
//------------------------------------------------------------------------------
{
	one.clear();
	int n2 = 0;
	float len = infinit_max;
	
	for ( vector<Obj*>& tblPoint : tbls )
	{
		int n = 0;
		for ( Obj* p : tblPoint )
		{
			// ポインターのエレメント選択
			if ( p->bSelected )
			{
				Point3* p2 = dynamic_cast<Point3*>(p);
				if (p2)
				{
					vect3 v = pers.calcWorldToScreen3( p2->pos +p2->a );
//					if ( (v.xy()-mpos).abs() < 0.04f )
					if ( (v.xy()-mpos).abs() < gra.Dot(11,11).abs() )
					{
						if ( one.w < v.z ) // 近い場合はより手前が優先
						{
							one.w = v.z;
							one.bEnable = true;
							one.idxTbl = n2;
							one.idxObj = n;
	
							one.bSelected_a = true;
							one.bSelected_b = false;
						}
					}
				}
				if (p2)
				{
					vect3 v = pers.calcWorldToScreen3( p2->pos +p2->b );
//					if ( (v.xy()-mpos).abs() < 0.04f )
					if ( (v.xy()-mpos).abs() < gra.Dot(11,11).abs() )
					{
						if ( one.w < v.z ) // 近い場合はより手前が優先
						{
							one.w = v.z;
							one.bEnable = true;
							one.idxTbl = n2;
							one.idxObj = n;
	
							one.bSelected_a = false;
							one.bSelected_b = true;
						}
					}
				}
			}
			
			// ポインター選択
			if ( one.bEnable == false )
			{
					vect3 v = pers.calcWorldToScreen3( p->pos );
					if ( (v.xy()-mpos).abs() < gra.Dot(11,11).abs() )
					{
						if ( one.w < v.z ) // 近い場合はより手前が優先
						{
							one.w = v.z;
							one.bEnable = true;
							one.idxTbl = n2;
							one.idxObj = n;
						}
					}
			}
			n++;
		}
		n2++;
	}
	if ( one.bEnable )
	{
		#if 0
		{
			// 優先度つけ
			for ( Obj* p : tblPoint )
			{
				p->priority = 999;
			}
			function<void( Joint&,int)> funcSetPriority = [&funcSetPriority] ( Joint& j, int prio )
			{
				p->priority = prio;
				for ( Joint& r : p->relative )
				{
					if ( r.priority > prio+1 ) funcSetPriority( r, prio+1 );
				}
			};
			
			funcSetPriority( one.bEnable, 1 );
		}
		#endif

	}
}

// 矩形カーソル開始 新規選択
//------------------------------------------------------------------------------
void Gui::SelectRectOnly( vect2 mpos )
//------------------------------------------------------------------------------
{
	rect_mode = G_CALC::COPY;
	rect_st = mpos;
}

// 矩形カーソル開始 追加選択
//------------------------------------------------------------------------------
void Gui::SelectRectAdd( vect2 mpos )
//------------------------------------------------------------------------------
{
	rect_mode = G_CALC::ADD;
	rect_st = mpos;
}

// 矩形カーソル開始 反転選択
//------------------------------------------------------------------------------
void Gui::SelectRectRev( vect2 mpos )
//------------------------------------------------------------------------------
{
	rect_mode = G_CALC::REV;
	rect_st = mpos;
}

// 矩形カーソル開始 削除選択
//------------------------------------------------------------------------------
void Gui::SelectRectSub( vect2 mpos )
//------------------------------------------------------------------------------
{
	rect_mode = G_CALC::SUB;
	rect_st = mpos;
}

//------------------------------------------------------------------------------
void Gui::calcRectMode( G_CALC rect_mode, bool& bPreselect, bool& bSelected )
//------------------------------------------------------------------------------
{
	switch( rect_mode )
	{
		case G_CALC::ADD:		if ( bPreselect ) bSelected = true;
			break;

		case G_CALC::SUB:		if ( bPreselect ) bSelected = false;
			break;

		case G_CALC::COPY:	bSelected = bPreselect;
			break;

		case G_CALC::REV:		if ( bPreselect ) bSelected = !bSelected;
			break;

		case G_CALC::NONE:
			break;
	}
	bPreselect = false;
};


// 矩形カーソル終了（選択決定）
//------------------------------------------------------------------------------
void Gui::SelectRectEnd( vector<vector<Obj*>>& tbls )
//------------------------------------------------------------------------------
{
	for ( vector<Obj*>& tblPoint : tbls )
	for ( Obj* p : tblPoint )
	{
		calcRectMode( rect_mode, p->bPreselect, p->bSelected );
	}

	rect_mode = G_CALC::NONE;

}

// 矩形カーソル選択	
//------------------------------------------------------------------------------
void Gui::SelectRectBegin( Pers& pers, vector<vector<Obj*>>& tbls , vect2 mpos )
//------------------------------------------------------------------------------
{
	vect2 v0 = min( rect_st, mpos );
	vect2 v1 = max( rect_st, mpos );

	for ( vector<Obj*>& tblPoint : tbls )
	for ( Obj* p : tblPoint )
	{
		p->bPreselect = false;

		vect2 v = pers.calcWorldToScreen2( p->pos );

		if ( v.x > v0.x && v.x < v1.x && v.y > v0.y && v.y < v1.y )
		{
			p->bPreselect = true;
		}
	}
}

// 単独 新規選択
//------------------------------------------------------------------------------
void Gui::SelectOneOnly( vector<vector<Obj*>>& tbls )
//------------------------------------------------------------------------------
{
	// 選択クリア
	for ( vector<Obj*>& tblPoint : tbls )
	{
		for ( Obj* p : tblPoint )
		{
			p->bSelected = false;
		}
	}
	
	tbls[ one.idxTbl ][ one.idxObj ]->bSelected = true;
}

// 単独 追加選択
//------------------------------------------------------------------------------
void Gui::SelectOneAdd( vector<vector<Obj*>>& tbls )
//------------------------------------------------------------------------------
{
	tbls[ one.idxTbl ][ one.idxObj ]->bSelected = true;
}

// 単独 反転選択
//------------------------------------------------------------------------------
void Gui::SelectOneRev( vector<vector<Obj*>>& tbls )
//------------------------------------------------------------------------------
{
	tbls[ one.idxTbl ][ one.idxObj ]->bSelected = !tbls[ one.idxTbl ][ one.idxObj ]->bSelected;
}

// 単独 削除選択
//------------------------------------------------------------------------------
void Gui::SelectOneSub( vector<vector<Obj*>>& tbls )
//------------------------------------------------------------------------------
{
	tbls[ one.idxTbl ][ one.idxObj ]->bSelected = false;
}

// 選択リスト表示
//------------------------------------------------------------------------------
void Gui::DrawController( SysGra& gra, Pers& pers, Gui& gui, vector<vector<Obj*>>& tbls, vector<vector<Edge*>>& tbltblEdge, vect2 mpos )
//------------------------------------------------------------------------------
{
	gra.SetZTest( false );

	{
		for ( int i = 0 ; i < (signed)tbltblEdge.size() ; i++ )
		{
			// 汎用制御点
			for ( Edge* p : tbltblEdge[i] )
			{

				bool bPreselect = p->bPreselect;
				bool bSelected = p->bSelected;

				calcRectMode( rect_mode, bPreselect, bSelected );

				Obj*	p0 = tbls[ i ][ p->n0 ];
				Obj*	p1 = tbls[ i ][ p->n1 ];


				{	// 影
					vect3	a = p0->pos;	a.y=0;
					vect3	b = p1->pos;	b.y=0;
					rgb		c = (p->col+rgb(0.75,0.75,0.75))/4;
					gui.pen.line3d( gra, pers, a, b, c, p->wide );
				}


				if ( bSelected )
				{
					// 選択点
					gui.pen.line3d( gra, pers, p0->pos, p1->pos, rgb(1,0,0), p->wide );
				}
				else
				{
					// 非選択点
					gui.pen.line3d( gra, pers, p0->pos, p1->pos, p->col, p->wide );
				}

			}
		}
	}

	{
		int n = 0;
		for ( vector<Obj*>& tblPoint : tbls )
		{

			// ベジェ制御点
			for ( Obj* po : tblPoint )
			{
				Point3* p = dynamic_cast<Point3*>(po);
				if ( p ) 
				{

					float wide = 11;
					float wide2 = 7;
					rgb	col = rgb(0,0,1);
					rgb	col2 = rgb(0,1,0);
					rgb	col2a = rgb(0,1,0);
					rgb	col2b = rgb(0,1,0);

					bool bPreselect = p->bPreselect;
					bool bSelected = p->bSelected;
					
					calcRectMode( rect_mode, bPreselect, bSelected );

					if ( bSelected )
					{
						gui.pen.line3d( gra, pers, p->pos, p->pos +p->a, col2 );
						gui.pen.line3d( gra, pers, p->pos, p->pos +p->b, col2 );
						gui.pen.pset3d( gra, pers, p->pos+p->a, col2a, wide2 ); 
						gui.pen.pset3d( gra, pers, p->pos+p->b, col2b, wide2 ); 
					}
				}
			}

			// 汎用制御点
			for ( Obj* p : tblPoint )
			{

				bool bPreselect = p->bPreselect;
				bool bSelected = p->bSelected;
				
				calcRectMode( rect_mode, bPreselect, bSelected );

				if ( bShowPoint )
				{
					if ( bSelected )
					{
						// 選択点
						gui.pen.pset3d( gra, pers, p->pos, rgb(1,0,0), 11 );
					}
					else
					{
						// 非選択点
						gui.pen.pset3d( gra, pers, p->pos, rgb(0,0,1), 11 );
					}

					{
						gui.pen.print3d( gra, pers, p->pos, -14, 0, to_string(n++) ); 
					}
				}
			}
		}
	}

	// 矩形カーソル 表示
	if (  rect_mode != G_CALC::NONE )
	{
		gra.Box( rect_st, mpos, rgb(1,1,1)*0.5f);
	}

	gra.SetZTest( true );
}

//------------------------------------------------------------------------------
void Gui::MoveObj( SysGra& gra, Pers& pers, Gui& gui, vector<vector<Obj*>>& tbls, vect2& mpos, vect2& mprev, vect2& mmov, bool bSame, bool bByCamera, bool bByFloor, bool bByXY )
//------------------------------------------------------------------------------
{
	bool bsel = false;

	
	if ( one.bEnable )
	{
		// タンジェントベクトル操作
		Point3* p = dynamic_cast<Point3*>(tbls[ one.idxTbl ][ one.idxObj ]);
		if ( p )
		{

			vect2	scale = vect2(pers.aspect, 1)/one.w*pers.rate_w;

			if ( one.bSelected_a )
			{
				vect3 v = vect3( mmov * scale, 0 );
				mat33 mrot = pers.cam.mat.GetRotate();
				v = v* mrot;
				p->a += v;
				if(bSame)	p->b = -p->a.normalize()*p->b.abs();
				
				bsel = true;
			}
			if ( one.bSelected_b )
			{
				vect3 v = vect3( mmov * scale, 0 );
				mat33 mrot = pers.cam.mat.GetRotate();
				v = v* mrot;
				p->b += v;
				if(bSame) p->a = -p->b.normalize()*p->a.abs();

				bsel = true;
			}
		}
	}

	if ( !bsel )
	{
		vect3 v;


		// 移動 カメラに並行
		if ( bByCamera )
		{
			v = vect3(mmov.x*pers.aspect, mmov.y, 0)/one.w*pers.rate_w;
			mat33 mrot = pers.cam.mat.GetRotate();
			v = v* mrot;
		}
		else
		if ( bByXY )
		// XYに平行
		{
			vect3 P0 = pers.calcScreenToWorld3( vect3(mprev,0) );
			vect3 P1 = pers.calcScreenToWorld3( vect3(mpos,0) );
			vect3 I0 = pers.calcRayvect( P0 );
			vect3 I1 = pers.calcRayvect( P1 );
			auto[b0,Q0,s0] = func_distance_Plate_Harfline( vect3(0,0,0), vect3(0,0,-1), P0, I0);
			auto[b1,Q1,s1] = func_distance_Plate_Harfline( vect3(0,0,0), vect3(0,0,-1), P1, I1);
			v = Q1-Q0;
		}
		else
		if ( bByFloor )
		// 床に平行
		{
			vect3 P0 = pers.calcScreenToWorld3( vect3(mprev,0) );
			vect3 P1 = pers.calcScreenToWorld3( vect3(mpos,0) );
			vect3 I0 = pers.calcRayvect( P0 );
			vect3 I1 = pers.calcRayvect( P1 );
			auto[b0,Q0,s0] = func_distance_Plate_Harfline( vect3(0,0,0), vect3(0,1,0), P0, I0);
			auto[b1,Q1,s1] = func_distance_Plate_Harfline( vect3(0,0,0), vect3(0,1,0), P1, I1);
			v = Q1-Q0;
		}
		for ( vector<Obj*>& tblPoint : tbls )
		{
			for ( Obj* p : tblPoint )
			{
				if ( p->bSelected )
				{
					p->pos += v ;
				}
			}
		}

	}


}

////////////////
// Axis
////////////////
//------------------------------------------------------------------------------
void Gui::Axis::DrawAxis( SysGra& gra, Pers& pers, Gui& gui, vect2 mpos )
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


	gra.Pset( v0, rgb(1,1,1),3 );

	gra.SetZTest( true );

}
		
////////////////
// Grid
////////////////
//------------------------------------------------------------------------------
void Gui::Grid::DrawGrid3d( SysGra& gra, Pers& pers, Gui& gui, vect3 pos, mat33 m, int NUM_U, int NUM_V, float dt, rgb col  )
//------------------------------------------------------------------------------
{	// ミニグリッド
	vect3 vt = vect3(0,0,0);
	float du = (float)NUM_U*dt;
	float dv = (float)NUM_V*dt;
	vect3 a;
	vect3 b;
	{
		a = pos+vect3(-du, 0,-du);
		b = pos+vect3( du, 0,-du);
		vt = vect3(0,0,dt);
		for ( int i = 0 ; i < NUM_V*2+1 ; i++ )
		{
			vect3 v0 = a * m;
			vect3 v1 = b * m;
			gui.pen.line3d_scissor( gra, pers, v0, v1, col );
			a+=vt;
			b+=vt;
		}
	}			
	{

		a = pos+vect3(-dv, 0, dv);
		b = pos+vect3(-dv, 0,-dv);
		vt = vect3(dt,0,0);
		for ( int i = 0 ; i < NUM_U*2+1 ; i++ )
		{
			vect3 v0 = a * m;
			vect3 v1 = b * m;
			gui.pen.line3d_scissor( gra, pers, v0, v1, col );
			a+=vt;
			b+=vt;
		}
	}			

	{//原点円表示
		float r = 0.1;
		vect3 a;
		for ( float th = 0 ; th <= deg2rad(360) ; th+=deg2rad(20) )
		{
			vect3 b = vect3( r*cos(th), 0, r*sin(th) ) + pos;
			if ( th > 0 ) 
			{
				vect3 v0 = a * m;
				vect3 v1 = b * m;
				gui.pen.line3d( gra, pers, v0,v1, col );
			}
			a = b;
		}
	}
	
	{// 10cmメモリ
		vect3 a;
		for ( float f = -1.0 ; f <= 1.0 ; f+=0.1 )
		{
			if ( abs(f)<=0.11 ) continue;
			{
				vect3 v0 = vect3( f, 0, -0.025 ) * m;
				vect3 v1 = vect3( f, 0,  0.025 ) * m;
				gui.pen.line3d( gra, pers, v0,v1, col );
			}
			{
				vect3 v0 = vect3( -0.025, 0, f ) * m;
				vect3 v1 = vect3(  0.025, 0, f ) * m;
				gui.pen.line3d( gra, pers, v0,v1, col );
			}
		}
	}
}



/////////////////////
// 支援ツール
/////////////////////
//------------------------------------------------------------------------------
void Gui::Prim::DrawPlate( SysGra& gra, Pers& pers, Gui& gui, vect3 pos, vect3 n1, rgb col )
//------------------------------------------------------------------------------
{
	mat33 m = mslerp( n1, 1.0 );

	//	仮想平面表示
	float r = 0.25;
	for ( int i = 0 ; i < 4 ; i++  )
	{
		r = (float)(1+i)*0.25;
	
		vect3 a;
		for ( float th = 0 ; th <= deg2rad(360) ; th+=deg2rad(10) )
		{
			vect3 b = vect3( r*cos(th), r*sin(th), 0 ) ;
			if ( th > 0 ) 
			{
				vect3 v0 = a * m;
				vect3 v1 = b * m;
				gui.pen.line3d( gra, pers, v0+ pos, v1+ pos, col );
			}
			a = b;
		}
	}

		gui.pen.line3d( gra, pers, pos, pos+ n1*0.2, col, 3 );
		gui.pen.pset3d( gra, pers, pos+ n1*0.2, col, 5 );

}

//------------------------------------------------------------------------------
void Gui::Prim::DrawMat33(  SysGra& gra, Pers& pers, Gui& gui, vect3 v0, mat33 m )
//------------------------------------------------------------------------------
{
	vect3 a = {m.m[0][0],m.m[0][1],m.m[0][2]};
	vect3 b = {m.m[1][0],m.m[1][1],m.m[1][2]};
	vect3 c = {m.m[2][0],m.m[2][1],m.m[2][2]};

	gui.pen.line3d( gra, pers, v0		, v0+a/2, rgb(1,0,0), 1 );
	gui.pen.line3d( gra, pers, v0		, v0+b/2, rgb(0,1,0), 1 );
	gui.pen.line3d( gra, pers, v0		, v0+c/2, rgb(0,0,1), 1 );
	gui.pen.line3d( gra, pers, v0+a/2	, v0+c/2, rgb(0.5,0.5,0.5), 1 );
	gui.pen.line3d( gra, pers, v0+b/2	, v0+c/2, rgb(0.5,0.5,0.5), 1 );
	gui.pen.line3d( gra, pers, v0+b/2	, v0+a/2, rgb(0.5,0.5,0.5), 1 );

}


//------------------------------------------------------------------------------
void Gui::Prim::DrawBox( SysGra& gra, Pers& pers, Gui& gui, vect3 pos, mat33 m , bool bAxis, bool bTri )
//------------------------------------------------------------------------------
{
	const float S = 0.04f;
	const float L = 0.03f;
	const float M = 0.02f;
	const float N = 0.02f;
	static vector<vect3> vert=
	{
		{	-S,		 S,		-S	},//0
		{	 S,		 S,		-S	},//1
		{	-S,		-S,		-S	},//2
		{	 S,		-S,		-S	},//3
		{	-S,		 S,		 S	},//4
		{	 S,		 S,		 S	},//5
		{	-S,		-S,		 S	},//6
		{	 S,		-S,		 S	},//7

		{	-L,		-S-L, 	-L	},//8	//yマーク
		{	 L,		-S-L, 	-L	},//9
		{	-L,		-S-L, 	 L	},//10
		{	 L,		-S-L, 	 L	},//11

		{	S+M,	+M, 	-M	},//	//xマーク
		{	S+M,	+M, 	+M	},//
		{	S+M,	-M, 	 0	},//

		{	-N,		 -N, 	S+N	},//	//zマーク
		{	+N,		 -N, 	S+N	},//
		{	-N,		 +N, 	S+N	},//
		{	+N,		 +N, 	S+N	},//
	};
	static vector<vect3> disp;

	static vector<ivect2>	edge
	{
		{	0,	1	},
		{	1,	3	},
		{	3,	2	},
		{	2,	0	},
		{	4,	5	},
		{	5,	7	},
		{	7,	6	},
		{	6,	4	},
		{	0,	4	},
		{	1,	5	},
		{	2,	6	},
		{	3,	7	},
		
		{	8,	9	},	//yマーク
		{	9,	11	},
		{	11,	10	},
		{	10,	8	},
#if 0

		{	12,	13	},	//xマーク
		{	13,	14	},
		{	14,	12	},


		{	15,	18	},	//zマーク
		{	17,	16	},
#endif
		
	};
	static vector<ivect3>	tri
	{
		{	8,10,9	},{	9,10,11	},	// yマーク
		{	14,	13, 12	},	// xマーク
	};


	disp.clear();

	for ( vect3 v : vert )
	{

		//	右手系座標系
		//	右手ねじ周り
		//	roll	:z	奥+
		//	pitch	:x	右+
		//	yaw		:y	下+
		v= v * m + pos;

		disp.emplace_back( v );

	}


	// 軸
	if( bAxis )
	{
		vect3	nx = vect3( m.m[0][0], m.m[0][1], m.m[0][2] );
		vect3	ny = vect3( m.m[1][0], m.m[1][1], m.m[1][2] );
		vect3	nz = vect3( m.m[2][0], m.m[2][1], m.m[2][2] );
		gui.pen.line3d( gra, pers, pos, pos+nx*0.2, rgb(1,0,0) );
		gui.pen.line3d( gra, pers, pos, pos+ny*0.2, rgb(0,1,0) );
		gui.pen.line3d( gra, pers, pos, pos+nz*0.2, rgb(0,0,1) );
	}
	
	// Tri
	if ( bTri )
	{
		for ( ivect3 t : tri )
		{
			vect3 v0 = pers.calcWorldToScreen3( disp[t.n0] );
			vect3 v1 = pers.calcWorldToScreen3( disp[t.n1] );
			vect3 v2 = pers.calcWorldToScreen3( disp[t.n2] );
//					if ( v0.z>0 )
			{
				gra.Tri( v0,v1,v2, rgb(1,0,1));
				gra.Tri( v2,v1,v0, rgb(1,0,1)/2);
			}

		}
	}
	for ( ivect2 e : edge )
	{
		const vect3& a = disp[e.p];
		const vect3& b = disp[e.n];
		const rgb col = rgb(0,1,1);

		gui.pen.line3d( gra, pers, a, b, col, false );

	}
}
	// ドラム
//------------------------------------------------------------------------------
void Gui::Prim::DrawDrum( SysGra& gra, Pers& pers, Gui& gui,  vect3 pos, mat33 m  )
//------------------------------------------------------------------------------
{
	static const float S = 0.05f;
	static const float L = 0.03f;
	static const float M = 0.02f;
	static const float N = 0.02f;
	static vector<vect3> vert=
	{
		{	-S,		 S,		-S	},
		{	 S,		 S,		-S	},
		{	-S,		-S,		-S	},
		{	 S,		-S,		-S	},
		{	-S,		 S,		 S	},
		{	 S,		 S,		 S	},
		{	-S,		-S,		 S	},
		{	 S,		-S,		 S	},

		{	-L,		-S-L, 	-L	},	//yマーク
		{	 L,		-S-L, 	-L	},
		{	-L,		-S-L, 	 L	},
		{	 L,		-S-L, 	 L	},

		{	S+M,	+M, 	-M	},	//xマーク
		{	S+M,	+M, 	+M	},
		{	S+M,	-M, 	 0	},

		{	-N,	 -N, 	S+N	},	//zマーク
		{	+N,	 -N, 	S+N	},
		{	-N,	 +N, 	S+N	},
		{	+N,	 +N, 	S+N	},
	};
	static vector<vect3> disp;

	static vector<ivect2>	edge
	{
/*				{	0,	1	},
		{	1,	3	},
		{	3,	2	},
		{	2,	0	},
		{	4,	5	},
		{	5,	7	},
		{	7,	6	},
		{	6,	4	},
		{	0,	4	},
		{	1,	5	},
		{	2,	6	},
		{	3,	7	},
*/				
#if 0
		{	8,	9	},	//yマーク
		{	9,	11	},
		{	11,	10	},
		{	10,	8	},

//				{	12,	13	},	//xマーク
//				{	13,	14	},
//				{	14,	12	},

		{	15,	18	},	//zマーク
		{	17,	16	},
#endif
		
	};
	static vector<ivect3>	tri
	{
//				{	14,	13, 12	},	// xマーク
	};

	static bool bInit = false;
	if ( !bInit )
	{
		{
			int	ofs = (signed)vert.size();
			int cnt = 0;
			float r = 0.05;
			float w = 0.05;
			for ( int i = 0 ; i < 360 ; i+= 30 )
			{
				float z = r*cos(deg2rad((float)i));
				float y = r*sin(deg2rad((float)i));
				vert.emplace_back(-w,y,z);
				vert.emplace_back( w,y,z);
				vert.emplace_back( w,y*0.5,z*0.5);
				cnt++;
			}
			
			for ( int i = 0 ; i < cnt ; i++ )
			{
				
				int st = i;
				int en = i+1;
				if ( i+1 == cnt ) en = 0;
				
				const int n = 3;

				edge.emplace_back(ofs+st*n  ,ofs+en*n);
				edge.emplace_back(ofs+st*n+1,ofs+en*n+1);
				edge.emplace_back(ofs+st*n+2,ofs+en*n+2);
				if ( i%3 == 0 ) edge.emplace_back(ofs+st*n  ,ofs+st*n+1);
			}
		}
		bInit = true;
	}

	disp.clear();

	for ( vect3 v : vert )
	{

		//	右手系座標系
		//	右手ねじ周り
		//	roll	:z	奥+
		//	pitch	:x	右+
		//	yaw		:y	下+
		v= v * m + pos;

		disp.emplace_back( v );

	}


	// 軸
	if(0)
	{
		vect3	nx = vect3( m.m[0][0], m.m[0][1], m.m[0][2] );
		vect3	ny = vect3( m.m[1][0], m.m[1][1], m.m[1][2] );
		vect3	nz = vect3( m.m[2][0], m.m[2][1], m.m[2][2] );
		gui.pen.line3d( gra, pers, pos, pos+nx*0.2, rgb(1,0,0) );
		gui.pen.line3d( gra, pers, pos, pos+ny*0.2, rgb(0,1,0) );
		gui.pen.line3d( gra, pers, pos, pos+nz*0.2, rgb(0,0,1) );
	}
	
	// Tri
	for ( ivect3 t : tri )
	{
		vect3 v0 = pers.calcWorldToScreen3( disp[t.n0] );
		vect3 v1 = pers.calcWorldToScreen3( disp[t.n1] );
		vect3 v2 = pers.calcWorldToScreen3( disp[t.n2] );
		if ( v0.z>0 )
		{
			gra.Tri( v0,v1,v2, rgb(1,0,1));
			gra.Tri( v2,v1,v0, rgb(1,0,1)/2);
		}

	}
	for ( ivect2 e : edge )
	{
		const vect3& a = disp[e.p];
		const vect3& b = disp[e.n];
		const rgb col = rgb(0,1,1);

		gui.pen.line3d( gra, pers, a, b, col, false );
	}
}

// 球
//------------------------------------------------------------------------------
void Gui::Prim::DrawShpere( SysGra& gra, Pers& pers, Gui& gui, float r, vect3 pos, mat33 m  )
//------------------------------------------------------------------------------
{
	float step = 2*pi/24.0;
	rgb col = rgb(0,1,1);
	vect3	vx0;
	vect3	vy0;
	vect3	vz0;
	for ( float th = 0 ; th < 2*pi ; th += step )
	{
		float u = r*cos(th);
		float v = r*sin(th);
	
		vect3	vx1= m*vect3(0,u,v)+pos;
		vect3	vy1= m*vect3(u,0,v)+pos;
		vect3	vz1= m*vect3(u,v,0)+pos;
		
		if ( th > 0 )
		{
			gui.pen.line3d( gra, pers, vx0, vx1, col );
			gui.pen.line3d( gra, pers, vy0, vy1, col );
			gui.pen.line3d( gra, pers, vz0, vz1, col );
		}
		vx0 = vx1;
		vy0 = vy1;
		vz0 = vz1;
	}
	
}

//------------------------------------------------------------------------------
void Gui::Prim::DrawTire( SysGra& gra, Pers& pers, Gui& gui, vect3 pos, float head, float bank, float radius )
//------------------------------------------------------------------------------
{
	static vector<vect3> vert;
	static vector<vect3> disp;
	static bool bInit = false;
	if ( !bInit )
	{
		bInit = true;
		float r = 1.0;
		for ( float t = 0 ; t < 2*pi ; t+=deg2rad(5) )
		{
			float x = 0;
			float y = r * sin(t);
			float z = r * cos(t);

			vert.emplace_back( x, y, z );
		}
	}
	
	mat33 m = mrotz(bank) * mroty(head);

	rgb col(0,1,1);
	int i = 0;
	vect3 v0;
	for ( vect3 v : vert )
	{
		vect3 v1 = v*radius * m + pos;

		if ( i > 0 ) 
		{
			gui.pen.line3d( gra, pers, v0, v1, col, 2 );

			// 影
			vect3 a0= v0;
			vect3 a1= v1;
			a0.y=0;
			a1.y=0;
			gui.pen.line3d( gra, pers, a0, a1, col/4, 2 );
		}

		v0 = v1;
		i++;
	}

}

//------------------------------------------------------------------------------
void Gui::Prim::DrawSquare( SysGra& gra, Pers& pers, Gui& gui, vect3 pos, mat33 m , bool bAxis, bool bTri )
//------------------------------------------------------------------------------
{
	static const float s = 0.04f;
	static vector<vect3> vert=
	{
		{	-s,		 s,		0	},//0
		{	 s,		 s,		0	},//1
		{	-s,		-s,		0	},//2
		{	 s,		-s,		0	},//3

	};
	static vector<vect3> disp;

	static vector<ivect2>	edge
	{
		{	0,	1	},
		{	1,	3	},
		{	3,	2	},
		{	2,	0	},
	};

	disp.clear();

	for ( vect3 v : vert )
	{

		//	右手系座標系
		//	右手ねじ周り
		//	roll	:z	奥+
		//	pitch	:x	右+
		//	yaw		:y	下+
		v= v * m + pos;

		disp.emplace_back( v );

	}


	// 軸
	if( bAxis )
	{
		vect3	nx = vect3( m.m[0][0], m.m[0][1], m.m[0][2] );
		vect3	ny = vect3( m.m[1][0], m.m[1][1], m.m[1][2] );
		vect3	nz = vect3( m.m[2][0], m.m[2][1], m.m[2][2] );
		gui.pen.line3d( gra, pers, pos, pos+nx*0.2, rgb(1,0,0) );
		gui.pen.line3d( gra, pers, pos, pos+ny*0.2, rgb(0,1,0) );
		gui.pen.line3d( gra, pers, pos, pos+nz*0.2, rgb(0,0,1) );
	}
	
	int cnt = 0;
	for ( ivect2 e : edge )
	{
		const vect3& a = disp[e.p];
		const vect3& b = disp[e.n];
		rgb col = rgb(0,1,1);
		if ( cnt == 0 ) col = rgb(1,0,0);
		//if ( cnt == 1 ) col = rgb(0,1,0);
		cnt++;
		gui.pen.line3d( gra, pers, a, b, col, false );

	}
}


//------------------------------------------------------------------------------
void Gui::Pen::line3d_scissor( SysGra& gra, Pers& pers, vect3 p0, vect3 p1, rgb col, float wide )
//------------------------------------------------------------------------------
{
	vect3 a = p0* pers.cam.mat.invers();
	vect3 b = p1* pers.cam.mat.invers();
	vect3 v0;
	vect3 v1;
	bool flg = pers.calcScissorLine3d( a, b, v0, v1 );
	if ( flg ) gra.Line( v0, v1, col, wide );
}

//------------------------------------------------------------------------------
void Gui::Pen::line3d( SysGra& gra, Pers& pers, vect3 p0, vect3 p1, rgb col, float wide )
//------------------------------------------------------------------------------
{
	vect3 v0 = pers.calcWorldToScreen3( p0 );
	vect3 v1 = pers.calcWorldToScreen3( p1 );
	if ( v0.z > 0 && v1.z > 0 ) gra.Line( v0, v1, col, wide );
}

//------------------------------------------------------------------------------
void Gui::Pen::tri3d( SysGra& gra, Pers& pers, vect3 p0, vect3 p1, vect3 p2, rgb col )
//------------------------------------------------------------------------------
{
	vect3 v0 = pers.calcWorldToScreen3( p0 );
	vect3 v1 = pers.calcWorldToScreen3( p1 );
	vect3 v2 = pers.calcWorldToScreen3( p2 );
	if ( v0.z > 0 && v1.z > 0 && v2.z > 0 ) gra.Tri( v0, v1, v2, col );
}

//------------------------------------------------------------------------------
void Gui::Pen::pset3d( SysGra& gra, Pers& pers, vect3 p0, rgb col, float wide )
//------------------------------------------------------------------------------
{
	vect3 v0 = pers.calcWorldToScreen3( p0 );
	if ( v0.z > 0 ) gra.Pset( v0, col, wide);
}

//------------------------------------------------------------------------------
void Gui::Pen::print3d( SysGra& gra, Pers& pers, vect3 p0, float x, float y, string str )
//------------------------------------------------------------------------------
{
	vect3 v0 = pers.calcWorldToScreen3( p0 );
	if ( v0.z > 0 ) gra.Print( v0.xy() + gra.Dot(x,y), str ); 
}

//------------------------------------------------------------------------------
void Gui::Pen::circle3d( SysGra& gra, Pers& pers, vect3 p0, float radius, rgb col, float wide )
//------------------------------------------------------------------------------
{
	vect3 v0 = pers.calcWorldToScreen3( p0 );
	float r = radius*v0.z;

	if ( v0.z > 0 ) gra.Circle( v0, r, col, wide );

}

// ~2d関数はXY平面、z=0に描画する。
//------------------------------------------------------------------------------
void Gui::Pen::pset2d( SysGra& gra, Pers& pers, vect2 p0, rgb col, float wide )
//------------------------------------------------------------------------------
{
	(*this).pset3d( gra, pers, vect3(p0,0), col, wide );
}

//------------------------------------------------------------------------------
void Gui::Pen::print2d( SysGra& gra, Pers& pers, vect2 p0, float x, float y, string str )
//------------------------------------------------------------------------------
{
	(*this).print3d( gra, pers, vect3(p0,0), 0, 0, str );

}
//------------------------------------------------------------------------------
void Gui::Pen::line2d( SysGra& gra, Pers& pers, vect2 p0, vect2 p1, rgb col, float wide )
//------------------------------------------------------------------------------
{
	(*this).line3d( gra, pers, vect3(p0,0), vect3(p1,0), col, wide );

}