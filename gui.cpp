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
void Gui::DrawController( Pers& pers, SysGra& gra, vector<vector<Obj*>>& tbls, vector<vector<Edge*>>& tbltblEdge, vect2 mpos )
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
					pers.line3d( gra, pers, a, b, c, p->wide );
				}


				if ( bSelected )
				{
					// 選択点
					pers.line3d( gra, pers, p0->pos, p1->pos, rgb(1,0,0), p->wide );
				}
				else
				{
					// 非選択点
					pers.line3d( gra, pers, p0->pos, p1->pos, p->col, p->wide );
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
						pers.line3d( gra, pers, p->pos, p->pos +p->a, col2 );
						pers.line3d( gra, pers, p->pos, p->pos +p->b, col2 );
						pers.pset3d( gra, pers, p->pos+p->a, col2a, wide2 ); 
						pers.pset3d( gra, pers, p->pos+p->b, col2b, wide2 ); 
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
						pers.pset3d( gra, pers, p->pos, rgb(1,0,0), 11 );
					}
					else
					{
						// 非選択点
						pers.pset3d( gra, pers, p->pos, rgb(0,0,1), 11 );
					}

					{
						pers.print3d( gra, pers, p->pos, -14, 0, to_string(n++) ); 
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
void Gui::MoveObj( SysGra& gra, Pers& pers, vector<vector<Obj*>>& tbls, vect2& mpos, vect2& mprev, vect2& mmov, bool bSame, bool bByCamera )
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
		// 床に平行
		{
			vect3 P0 = pers.calcScreenToWorld3( vect3(mprev,0) );
			vect3 P1 = pers.calcScreenToWorld3( vect3(mpos,0) );
			vect3 I0 = pers.calcRayvect( P0 );
			vect3 I1 = pers.calcRayvect( P1 );
			auto[b0,t0,Q0] = func_distance_Plate_Line( vect3(0,0,0), vect3(0,1,0), P0, I0);
			auto[b1,t1,Q1] = func_distance_Plate_Line( vect3(0,0,0), vect3(0,1,0), P1, I1);
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

