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

#include "geom.h"


#include "SysSound.h"
#include "SysKeys.h"
#include "SysMouse.h"
#include "SysGra.h"
#include "Sys.h"

#include "func.h"
#include "pers.h"

#include "gui.h"

using namespace std;

//------------------------------------------------------------------------------
void Gui::TouchFirst( SysGra& gra, Pers& pers, vector<vector<shared_ptr<Obj>>>& tbltbl_pObj, vect2 mpos )
//------------------------------------------------------------------------------
{
	one.clear();
	int n2 = 0;
//	float len = infinit_max;
	
	for ( vector<shared_ptr<Obj>>& tbl_pObj : tbltbl_pObj )
	{
		int n = 0;
		for ( shared_ptr<Obj> p : tbl_pObj )
		{
			// ポインターのエレメント選択
			if ( p->bSelected )
			{
				Point3* p2 = dynamic_cast<Point3*>(p.get());
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
			for ( shared_ptr<Obj> p : tbl_pObj )
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
void Gui::SelectRectEnd( vector<vector<shared_ptr<Obj>>>& tbltbl_pObj )
//------------------------------------------------------------------------------
{
	for ( vector<shared_ptr<Obj>>& tbl_pObj : tbltbl_pObj )
	for ( shared_ptr<Obj> p : tbl_pObj )
	{
		calcRectMode( rect_mode, p->bPreselect, p->bSelected );
	}

	rect_mode = G_CALC::NONE;

}

// 矩形カーソル選択	
//------------------------------------------------------------------------------
void Gui::SelectRectBegin( Pers& pers, vector<vector<shared_ptr<Obj>>>& tbltbl_pObj , vect2 mpos )
//------------------------------------------------------------------------------
{
	vect2 v0 = min( rect_st, mpos );
	vect2 v1 = max( rect_st, mpos );

	for ( vector<shared_ptr<Obj>>& tbl_pObj : tbltbl_pObj )
	for ( shared_ptr<Obj> p : tbl_pObj )
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
void Gui::SelectOneOnly( vector<vector<shared_ptr<Obj>>>& tbltbl_pObj )
//------------------------------------------------------------------------------
{
	// 選択クリア
	for ( vector<shared_ptr<Obj>>& tbl_pObj : tbltbl_pObj )
	{
		for ( shared_ptr<Obj> p : tbl_pObj )
		{
			p->bSelected = false;
		}
	}
	
	tbltbl_pObj[ one.idxTbl ][ one.idxObj ]->bSelected = true;
}

// 単独 追加選択
//------------------------------------------------------------------------------
void Gui::SelectOneAdd( vector<vector<shared_ptr<Obj>>>& tbltbl_pObj )
//------------------------------------------------------------------------------
{
	tbltbl_pObj[ one.idxTbl ][ one.idxObj ]->bSelected = true;
}

// 単独 反転選択
//------------------------------------------------------------------------------
void Gui::SelectOneRev( vector<vector<shared_ptr<Obj>>>& tbltbl_pObj )
//------------------------------------------------------------------------------
{
	tbltbl_pObj[ one.idxTbl ][ one.idxObj ]->bSelected = !tbltbl_pObj[ one.idxTbl ][ one.idxObj ]->bSelected;
}

// 単独 削除選択
//------------------------------------------------------------------------------
void Gui::SelectOneSub( vector<vector<shared_ptr<Obj>>>& tbltbl_pObj )
//------------------------------------------------------------------------------
{
	tbltbl_pObj[ one.idxTbl ][ one.idxObj ]->bSelected = false;
}

// 選択リスト表示
//------------------------------------------------------------------------------
void Gui::DrawController( SysGra& gra, Pers& pers, vector<vector<shared_ptr<Obj>>>& tbltbl_pObj, vector<vector<shared_ptr<Edge>>>& tbltbl_pEdge, vect2 mpos )
//------------------------------------------------------------------------------
{
	gra.SetZTest( false );

	{
		for ( int i = 0 ; i < (signed)tbltbl_pEdge.size() ; i++ )
		{
			// 汎用制御点
			for ( shared_ptr<Edge> p : tbltbl_pEdge[i] )
			{

				bool bPreselect = p->bPreselect;
				bool bSelected = p->bSelected;

				calcRectMode( rect_mode, bPreselect, bSelected );

				Obj&	o0 = *tbltbl_pObj[ i ][ p->n0 ];
				Obj&	o1 = *tbltbl_pObj[ i ][ p->n1 ];


				{	// 影
					vect3	a = o0.pos;	a.y=0;
					vect3	b = o1.pos;	b.y=0;
					rgb		c = (p->col+rgb(0.75,0.75,0.75))/4;
					pers.pen.Line3d( gra, pers, a, b, c, p->wide );
				}


				if ( bSelected )
				{
					// 選択点
					pers.pen.Line3d( gra, pers, o0.pos, o1.pos, rgb(1,0,0), p->wide );
				}
				else
				{
					// 非選択点
					pers.pen.Line3d( gra, pers, o0.pos, o1.pos, p->col, p->wide );
				}

			}
		}
	}

	{
		int n = 0;
		for ( vector<shared_ptr<Obj>>& tbl_pObj : tbltbl_pObj )
		{

			// ベジェ制御点
			for ( shared_ptr<Obj> po : tbl_pObj )
			{
				Point3* p = dynamic_cast<Point3*>(po.get());
				if ( p ) 
				{

//					float wide = 11;
					float wide2 = 7;
//					rgb	col = rgb(0,0,1);
					rgb	col2 = rgb(0,1,0);
					rgb	col2a = rgb(0,1,0);
					rgb	col2b = rgb(0,1,0);

					bool bPreselect = p->bPreselect;
					bool bSelected = p->bSelected;
					
					calcRectMode( rect_mode, bPreselect, bSelected );

					if ( bSelected )
					{
						pers.pen.Line3d( gra, pers, p->pos, p->pos +p->a, col2 );
						pers.pen.Line3d( gra, pers, p->pos, p->pos +p->b, col2 );
						pers.pen.Pset3d( gra, pers, p->pos+p->a, col2a, wide2 ); 
						pers.pen.Pset3d( gra, pers, p->pos+p->b, col2b, wide2 ); 
					}
				}
			}

			// 汎用制御点
			for ( shared_ptr<Obj> p : tbl_pObj )
			{

				bool bPreselect = p->bPreselect;
				bool bSelected = p->bSelected;
				
				calcRectMode( rect_mode, bPreselect, bSelected );

				if ( bShowPoint )
				{
					if ( bSelected )
					{
						// 選択点
						pers.pen.Pset3d( gra, pers, p->pos, rgb(1,0,0), 11 );
					}
					else
					{
						// 非選択点
						pers.pen.Pset3d( gra, pers, p->pos, rgb(0,0,1), 11 );
					}

					{
						pers.pen.Print3d( gra, pers, p->pos, -14, 0, to_string(n++) ); 
					}
				}
			}
		}
	}

	// 矩形カーソル 表示
	if (  rect_mode != G_CALC::NONE )
	{
		gra.Box2d( rect_st, mpos, rgb(1,1,1)*0.5f);
	}

	gra.SetZTest( true );
}

//------------------------------------------------------------------------------
void Gui::MoveObj( SysGra& gra, Pers& pers, vector<vector<shared_ptr<Obj>>>& tbltbl_pObj, vect2& mpos, vect2& mprev, vect2& mmov, bool bSame, bool bByCamera, bool bByFloor, bool bByXY )
//------------------------------------------------------------------------------
{
	bool bsel = false;

	
	if ( one.bEnable )
	{
		// タンジェントベクトル操作
		Point3* p = dynamic_cast<Point3*>(tbltbl_pObj[ one.idxTbl ][ one.idxObj ].get());
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
			auto[b0,Q0,s0] = func_intersect_Plate_HarfLine( vect3(0,0,0), vect3(0,0,-1), P0, I0);
			auto[b1,Q1,s1] = func_intersect_Plate_HarfLine( vect3(0,0,0), vect3(0,0,-1), P1, I1);
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
			auto[b0,Q0,s0] = func_intersect_Plate_HarfLine( vect3(0,0,0), vect3(0,1,0), P0, I0);
			auto[b1,Q1,s1] = func_intersect_Plate_HarfLine( vect3(0,0,0), vect3(0,1,0), P1, I1);
			v = Q1-Q0;
		}
		for ( vector<shared_ptr<Obj>>& tbl_pObj : tbltbl_pObj )
		{
			for ( shared_ptr<Obj> p : tbl_pObj )
			{
				if ( p->bSelected )
				{
					p->pos += v ;
				}
			}
		}

	}


}




