// 2017/07/07 ray3
// 2019/06/25 ray4

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

#include "raytrace.h"
#include "skeleton.h"

using namespace std;


enum class G_CALC
{
	NONE,
	ADD,
	SUB,
	COPY,
	REV,
};

struct Point3 : Obj
{
	Point3( vect3 _pos ) { pos = _pos;}
	vect3	a;
	vect3	b;

	Point3( vect3 _pos, vect3 _a, vect3 _b ) { pos=_pos; a=_a; b=_b;}
};

struct One
{
	bool	bEnable;
	float	w;		// wバッファ値
	int		idxTbl;	// オブジェクト番号
	int		idxObj;	// オブジェクト番号
	//--
	bool	bSelected_a = false;
	bool	bSelected_b = false;

	One(){clear();}
	void clear()
	{
		bEnable = false;
		w = 0.0;
		idxTbl = 0;
		idxObj = 0;
		bSelected_a = false;
		bSelected_b = false;
	}
	
};

//unique
static vector<Obj*> g_tblPoint;

struct
{
	vect2 rect_st = vect2(0,0);			//	矩形選択開始位置
	G_CALC rect_mode = G_CALC::NONE;	//	矩形選択中フラグ
	One one;

	//------------------------------------------------------------------------------
	void TouchFirst( Pers& pers, vector<vector<Obj*>>& tbls, vect2 mpos )
	//------------------------------------------------------------------------------
	{
		one.clear();
		int n2 = 0;
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
						if ( (v.xy()-mpos).abs() < 0.04f )
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
						if ( (v.xy()-mpos).abs() < 0.04f )
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
						if ( (v.xy()-mpos).abs() < 0.04f )
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
	void SelectRectOnly( vect2 mpos )
	//------------------------------------------------------------------------------
	{
		rect_mode = G_CALC::COPY;
		rect_st = mpos;
	}

	// 矩形カーソル開始 追加選択
	//------------------------------------------------------------------------------
	void SelectRectAdd( vect2 mpos )
	//------------------------------------------------------------------------------
	{
		rect_mode = G_CALC::ADD;
		rect_st = mpos;
	}

	// 矩形カーソル開始 反転選択
	//------------------------------------------------------------------------------
	void SelectRectRev( vect2 mpos )
	//------------------------------------------------------------------------------
	{
		rect_mode = G_CALC::REV;
		rect_st = mpos;
	}

	// 矩形カーソル開始 削除選択
	//------------------------------------------------------------------------------
	void SelectRectSub( vect2 mpos )
	//------------------------------------------------------------------------------
	{
		rect_mode = G_CALC::SUB;
		rect_st = mpos;
	}
	
	//------------------------------------------------------------------------------
	void calcRectMode( G_CALC rect_mode, bool& bPreselect, bool& bSelected )
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
	void SelectRectEnd( vector<vector<Obj*>>& tbls )
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
	void SelectRectBegin( Pers& pers, vector<vector<Obj*>>& tbls , vect2 mpos )
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
	void SelectOneOnly( vector<vector<Obj*>>& tbls )
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
	void SelectOneAdd( vector<vector<Obj*>>& tbls )
	//------------------------------------------------------------------------------
	{
		tbls[ one.idxTbl ][ one.idxObj ]->bSelected = true;
	}

	// 単独 反転選択
	//------------------------------------------------------------------------------
	void SelectOneRev( vector<vector<Obj*>>& tbls )
	//------------------------------------------------------------------------------
	{
		tbls[ one.idxTbl ][ one.idxObj ]->bSelected = !tbls[ one.idxTbl ][ one.idxObj ]->bSelected;
	}

	// 単独 削除選択
	//------------------------------------------------------------------------------
	void SelectOneSub( vector<vector<Obj*>>& tbls )
	//------------------------------------------------------------------------------
	{
		tbls[ one.idxTbl ][ one.idxObj ]->bSelected = false;
	}

	// 選択リスト表示
	//------------------------------------------------------------------------------
	void DrawController( Pers& pers, SysGra& gra, vector<vector<Obj*>>& tbls, vect2 mpos )
	//------------------------------------------------------------------------------
	{
		gra.SetZTest( false );

		{
			int n = 0;
			int n2 = 0;
			for ( vector<Obj*>& tblPoint : tbls )
			{
//				if ( n2 > 0 ) 
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
								g_line3d( gra, pers, p->pos, p->pos +p->a, col2 );
								g_line3d( gra, pers, p->pos, p->pos +p->b, col2 );
								g_pset3d( gra, pers, p->pos+p->a, col2a, wide2 ); 
								g_pset3d( gra, pers, p->pos+p->b, col2b, wide2 ); 
							}
						}
					}

					// 汎用制御点
					for ( Obj* p : tblPoint )
					{

						bool bPreselect = p->bPreselect;
						bool bSelected = p->bSelected;
						
						calcRectMode( rect_mode, bPreselect, bSelected );

						if ( bSelected )
						{
							gra.Pset( pers.calcWorldToScreen3( p->pos ), rgb(1,0,0), 11 );
						}
						else
						{
							gra.Pset( pers.calcWorldToScreen3( p->pos ), rgb(0,0,1), 11 );
						}

						{
							vect2 pos = pers.calcWorldToScreen2( p->pos );
							gra.Print( pos+gra.Dot(14,0), to_string(n++) );
						}
					}
				}
				n2++;
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
	void MoveObj( SysGra& gra, Pers& pers, vector<vector<Obj*>>& tbls, vect2& mpos, vect2& mprev, vect2& mmov, bool bSame, bool bByCamera )
	//------------------------------------------------------------------------------
	{
		bool bsel = false;

		
		if ( one.bEnable )
		{
			// タンジェントベクトル操作
			Point3* p = dynamic_cast<Point3*>(tbls[ one.idxTbl ][ one.idxObj ]);
			if ( p )
			{

				vect2	scale = vect2(pers.aspect, 1)/one.w/pers.rate;

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
				v = vect3(mmov.x*pers.aspect, mmov.y, 0)/one.w/pers.rate;
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
				auto[b0,t0,Q0] = distanceIntersectPlate( vect3(0,0,0), vect3(0,1,0), P0, I0);
				auto[b1,t1,Q1] = distanceIntersectPlate( vect3(0,0,0), vect3(0,1,0), P1, I1);
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

	vector<vector<Obj*>> tbls;	// 0 はダミー

	//------------------------------------------------------------------------------
	int EntryTbl( vector<Obj*>tbl )
	//------------------------------------------------------------------------------
	{
		int idx =  (signed)tbls.size();
		tbls.emplace_back( tbl );
		return idx;
	}

} gui;

//------------------------------------------------------------------------------
auto cource_selectCutmull = []( SysGra& gra, Pers& pers, vector<Point3>& tbl, vect2& mpos )
//------------------------------------------------------------------------------
{
	for ( Point3& c : tbl )
	{
		c.bSelected = false;
	}

	for ( Point3& c : tbl )
	{
		vect3 v = pers.calcWorldToScreen3( c.pos );

		if ( gra.Pixel(mpos-v.xy()).abs() < 16 )
		{
			c.bSelected = true;
		}
	}
};

//------------------------------------------------------------------------------
void cource_moveCutmull( Pers& pers, vector<Point3>& tbl, vect2& mmov )
//------------------------------------------------------------------------------
{
	vect2 scale;
	{
		// 最終選択を求める
		for ( Point3& c : tbl )
		{
			if ( c.bSelected )
			{
				// 適当な一つから移動スケールを求める
				vect3 v = pers.calcWorldToScreen3( c.pos );
				scale = vect2(pers.aspect, 1)/v.z/pers.rate;
				break;
			}
		}
	}

	// 移動
	for ( Point3& c : tbl )
	{
		if ( c.bSelected )
		{
			vect3 v = vect3( mmov * scale, 0 );
			mat33 mrot = pers.cam.mat.GetRotate();
			v = v* mrot;
			c.pos += v;
			
		}
	}
}
struct Cource
{
	bool	bActive = false;	//	使用可能

	int idxTbl = 0;
	vector<Obj*> tblPoint =
	{
		new Point3(vect3(-1.0, 0.0, 0.0 ),vect3( 0.0, 0.0, 1.0 ),vect3( 0.0, 0.0,-1.0 )),
		new Point3(vect3( 1.0, 0.0, 0.0 ),vect3( 0.0, 0.0,-1.0 ),vect3( 0.0, 0.0, 1.0 )),

	};
	vector<int>	idxPoint =
	{
		0,1,0
	};

	~Cource()
	{
		for ( Obj* p : tblPoint )
		{
			delete p;
		}	
	}
	
};
;

struct Bezier
{
	//------------------------------------------------------------------------------
	void cource_exec_drawBezier( SysGra& gra, Pers& pers, vector<Obj*>& tblPoint, vector<int>& idxPoint, int idxTbl , vect3& P, vect3& I, bool bSerch, bool bCut )
	//------------------------------------------------------------------------------
	{
		//ベジェ計算＆描画
		int div = 10;
		float dt = 1/(float)div;

		int size = static_cast<signed>(idxPoint.size());

		float	mind = infinit;
		vect3	minQ1;
		vect3	minQ0;
		vect3	minQ;
		int		minn;
		float	mint;
		vect3	mindt;
		bool	minb = false;
		vect3	minv;

		for ( int n = 0 ; n < size-1 ; n++ )
		{
			int n0 = idxPoint[n];
			int n1 = idxPoint[n+1];

			Point3* p0 = dynamic_cast<Point3*>(tblPoint[n0]);
			Point3* p1 = dynamic_cast<Point3*>(tblPoint[n1]);

			vect3 P0 =     p0->pos;
			vect3 P1 = P0 +p0->b;
			vect3 P3 =     p1->pos;
			vect3 P2 = P3 +p1->a;

			float t  = dt;
			vect3 pos0 = p0->pos;
			for ( int i = 0 ; i < div ; i++ )
			{
				vect3 pos1 = bezier3_func( t, P0, P1, P2, P3 );
				g_line3d( gra, pers, pos0, pos1, rgb(1,1,1) );

				// マウスベクトルとの最近点
				if ( bSerch )
				{
					auto[b,d,Q0,Q1,t0,t1] = distanceLineSegline_func( P, I, pos0, pos1 );
					if ( b ) 
					{
						if ( mind > d && 0.2 > d )
						{
							minb	= true;
							mind	= d;
							minn	= n;

							minQ0 = Q0;
							minQ1 = Q1;

							mint	= t-(1/(float)div) + (t1/(pos1-pos0).abs())/((float)div);
							minQ	= bezier3_func( mint, P0, P1, P2, P3 );
							mindt	= bezier3_delta_func( mint, P0, P1, P2, P3 );

							minv = (minQ-Q0).normalize();
						}
					}

				}

				pos0=pos1;
				t+=dt;
			}

		}

		// マウスベクトルとの最近点表示
		gra.SetZTest( false );
		if ( minb ) 
		{
			g_pset3d( gra, pers, minQ, rgb(1,0,0), 5 );
		}
		gra.SetZTest( true );

		//--------
		// 切断
		//--------
		if ( minb )
		{

			if ( bCut )
			{
				vect3	q = minQ;
				vect3	v = mindt/3;
				float	t0 = mint;
				float	t1 = 1.0-mint;

				// 頂点生成
				{
					
					tblPoint.emplace_back( new Point3( q, -v*t0, v*t1 ) );
					idxPoint.insert( idxPoint.begin()+minn+1, (signed)tblPoint.size()-1);
				}

				{
					Point3* p0 = dynamic_cast<Point3*>(tblPoint[idxPoint[minn+0]]);
					Point3* p2 = dynamic_cast<Point3*>(tblPoint[idxPoint[minn+2]]);
					p0->b *= t0;
					p2->a *= t1;
				}
				gui.rect_mode = G_CALC::NONE;


				// 追加したポイントを選択状態にする
				{
					int idx = idxPoint[minn+1];
					vect3 v = pers.calcWorldToScreen3( minQ );
					//
					gui.one.w = v.z;
					gui.one.idxTbl = idxTbl;
					gui.one.idxObj = idx;
					gui.one.bEnable = true;
					tblPoint[ idx ]->bSelected = true;
					tblPoint[ idx ]->bPreselect = false;
					gui.one.bSelected_a = false;
					gui.one.bSelected_b = false;
				}
			}

			{
				gra.SetZTest( false );
				g_print3d( gra, pers, minQ, -26,-32-20, to_string(mint) ); 
				gra.SetZTest( true );
			}
		}


	};
} bezier;

//------------------------------------------------------------------------------
void cource_drawPoint( SysGra& gra, Pers& pers, vector<Obj*>& tblPoint )
//------------------------------------------------------------------------------
{
	int cnt = 0 ;
	gra.SetZTest(false);
	for ( Obj* p : tblPoint )
	{
		vect3 v = pers.calcWorldToScreen3( p->pos );
		if ( v.z > 0 )
		{
			if ( p->bSelected ) 
					gra.Pset( v, rgb(1,0,0), 11 ); 
			else	gra.Pset( v, rgb(0,0,1), 11 ); 

			gra.Print( v.xy() + gra.Dot(16,-12), to_string(cnt) ); 
		}
		cnt++;
	}
	gra.SetZTest(true);
}

//------------------------------------------------------------------------------
void cource_drawCutmull( SysGra& gra, Pers& pers, vector<Point3>& tbl, vector<ivect2>idx )
//------------------------------------------------------------------------------
{
	// 描画 カーブ
	function<void()> func = [&]()
	{
		rgb	col(1,1,1);
		int size = (signed)idx.size();
		vect3	v0;
		vect3	v2;
		vect3	w0;
		vect3	w2;
		for ( int i = 0 ; i < size ; i++ )
		{
			int n0 = idx[i].n0;
			int n1 = idx[(i+1)%size].n0;
			int n2 = idx[(i+2)%size].n0;
			int n3 = idx[(i+3)%size].n0;

			int m0 = idx[i].n1;
			int m1 = idx[(i+1)%size].n1;
			int m2 = idx[(i+2)%size].n1;
			int m3 = idx[(i+3)%size].n1;
		
			vect3 P0 = tbl[n0].pos;
			vect3 P1 = tbl[n1].pos;
			vect3 P2 = tbl[n2].pos;
			vect3 P3 = tbl[n3].pos;

			vect3 Q0 = tbl[m0].pos;
			vect3 Q1 = tbl[m1].pos;
			vect3 Q2 = tbl[m2].pos;
			vect3 Q3 = tbl[m3].pos;
			
			for ( float t = 0.0 ; t < 1.0 ; t+=0.1 )
			{
				vect3 v1 = catmull3_func(t, P0,P1,P2,P3 );
				vect3 w1 = catmull3_func(t, Q0,Q1,Q2,Q3 );
				if ( (i==0 && t==0) ) 
				{
					v2=v1;
					w2=w1;
				}
				else 
				{
					g_line3d( gra, pers, v0, v1, col);
					g_line3d( gra, pers, w0, w1, col);
					g_line3d( gra, pers, w1, v1, col);

					{
						vect3 a = v0;a.y=0;
						vect3 b = v1;b.y=0;
						g_line3d( gra, pers, a, b, rgb(0.2,0.2,0.2));
					}
					{
						vect3 a = w0;a.y=0;
						vect3 b = w1;b.y=0;
						g_line3d( gra, pers, a, b, rgb(0.2,0.2,0.2));
					}

				}

					{
						vect3 nx,ny,nz;
						nz = (v1-v0).normalize();
						ny = vect3(0,1,0);
						nx = cross(nz,ny).normalize();
						ny = cross(nx,nz).normalize();

						mat33	m(
							nx.x,	nx.y,	nx.z,
							ny.x,	ny.y,	ny.z,
							nz.x,	nz.y,	nz.z
						);

						//square.DrawSquare( gra, pers, v1, m, false  );
					}


				v0 = v1;
				w0 = w1;
			}
		}
		g_line3d( gra, pers, v0, v2, col);
		g_line3d( gra, pers, w0, w2, col);
		g_line3d( gra, pers, w2, v2, col );
		{
			vect3 a = v0;a.y=0;
			vect3 b = v2;b.y=0;
			g_line3d( gra, pers, a, b, rgb(0.2,0.2,0.2));
		}
		{
			vect3 a = w0;a.y=0;
			vect3 b = w2;b.y=0;
			g_line3d( gra, pers, a, b, rgb(0.2,0.2,0.2));
		}
	};
	func();

}

struct Square
{

	const float s = 0.04f;
	vector<vect3> vert=
	{
		{	-s,		 s,		0	},//0
		{	 s,		 s,		0	},//1
		{	-s,		-s,		0	},//2
		{	 s,		-s,		0	},//3

	};
	vector<vect3> disp;

	vector<ivect2>	edge
	{
		{	0,	1	},
		{	1,	3	},
		{	3,	2	},
		{	2,	0	},
	};

	//------------------------------------------------------------------------------
	void DrawSquare( SysGra& gra, Pers& pers, vect3 pos, mat33 m , bool bAxis = true, bool bTri = true )
	//------------------------------------------------------------------------------
	{
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
			g_line3d( gra, pers, pos, pos+nx*0.2, rgb(1,0,0) );
			g_line3d( gra, pers, pos, pos+ny*0.2, rgb(0,1,0) );
			g_line3d( gra, pers, pos, pos+nz*0.2, rgb(0,0,1) );
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
			g_line3d( gra, pers, a, b, col, false );

		}
	}
	
} square;


struct Box
{

	const float s = 0.04f;
	const float l = 0.03f;
	const float m = 0.02f;
	const float n = 0.02f;
	vector<vect3> vert=
	{
		{	-s,		 s,		-s	},//0
		{	 s,		 s,		-s	},//1
		{	-s,		-s,		-s	},//2
		{	 s,		-s,		-s	},//3
		{	-s,		 s,		 s	},//4
		{	 s,		 s,		 s	},//5
		{	-s,		-s,		 s	},//6
		{	 s,		-s,		 s	},//7

		{	-l,		-s-l, 	-l	},//8	//yマーク
		{	 l,		-s-l, 	-l	},//9
		{	-l,		-s-l, 	 l	},//10
		{	 l,		-s-l, 	 l	},//11

		{	s+m,	+m, 	-m	},//	//xマーク
		{	s+m,	+m, 	+m	},//
		{	s+m,	-m, 	 0	},//

		{	-n,		 -n, 	s+n	},//	//zマーク
		{	+n,		 -n, 	s+n	},//
		{	-n,		 +n, 	s+n	},//
		{	+n,		 +n, 	s+n	},//
	};
	vector<vect3> disp;

	vector<ivect2>	edge
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
	vector<ivect3>	tri
	{
		{	8,10,9	},{	9,10,11	},	// yマーク
		{	14,	13, 12	},	// xマーク
	};

	Box()
	{
	}


	//------------------------------------------------------------------------------
	void DrawBox( SysGra& gra, Pers& pers, vect3 pos, mat33 m , bool bAxis = true, bool bTri = true )
	//------------------------------------------------------------------------------
	{
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
			g_line3d( gra, pers, pos, pos+nx*0.2, rgb(1,0,0) );
			g_line3d( gra, pers, pos, pos+ny*0.2, rgb(0,1,0) );
			g_line3d( gra, pers, pos, pos+nz*0.2, rgb(0,0,1) );
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

			g_line3d( gra, pers, a, b, col, false );

		}
	}
	
};
Box box;


struct Apr : public Sys
{
	//------------------------------------------------------------------------------
	Apr( const char* name, int pos_x, int pos_y, int width, int height ) : Sys( name, pos_x, pos_y, width, height )
	//------------------------------------------------------------------------------
	{}

	//------------------------------------------------------------------------------
	~Apr()
	//------------------------------------------------------------------------------
	{}

	long long	time_peak = 0;


	struct Manupirator
	{

		bool bAxisX = true;;
		bool bAxisY = true;;
		bool bAxisZ = true;;

		//------------------------------------------------------------------------------
		void DrawAxis( vect2 mpos, Apr& apr )
		//------------------------------------------------------------------------------
		{
			apr.gra.SetZTest( false );

			vect3 pos = apr.pers.calcScreenToWorld3( vect3(mpos,0) );

			vect3 v0 = apr.pers.calcWorldToScreen3( pos );

			// 軸表示
			float l = 0.1;
			if ( bAxisX  )
			{
				vect3 v1 = v0 + vect3(
					apr.pers.cam.mat.m[0][0] / apr.pers.aspect,
					apr.pers.cam.mat.m[1][0],
					apr.pers.cam.mat.m[2][0]
				) * l;
				apr.gra.Line( v0, v1, rgb(0.8,0.2,0.2), 2.0 );
			}
			if ( bAxisY  )
			{
				vect3 v1 = v0 + vect3(
					apr.pers.cam.mat.m[0][1] / apr.pers.aspect,
					apr.pers.cam.mat.m[1][1],
					apr.pers.cam.mat.m[2][1]
				) * l;
				apr.gra.Line( v0, v1, rgb(0.2,0.8,0.2), 2.0 );
			}
			if ( bAxisZ )
			{
				vect3 v1 = v0 + vect3(
					apr.pers.cam.mat.m[0][2] / apr.pers.aspect,
					apr.pers.cam.mat.m[1][2],
					apr.pers.cam.mat.m[2][2]
				) * l;
				apr.gra.Line( v0, v1, rgb(0.1,0.3,1), 2.0 );

			}

			// 軸名表示
			apr.gra.Print( mpos+apr.gra.Dot(16,-12),string("")+(bAxisX?"X":"")+(bAxisY?"Y":"")+(bAxisZ?"Z":"") ); 

			apr.gra.SetZTest( true );

		}
	} axis;


	Pers pers;

	//------------------------------------------------------------------------------
	void pset3d( vect3 pos0, rgb col, float wide )
	//------------------------------------------------------------------------------
	{
		vect3 v = pers.calcWorldToScreen3( pos0 );
		if ( v.z > 0 )
		{
			gra.Pset( vect2(v.x,v.y), col, wide );
		}

	}

	

	struct Grid
	{
/*
		//------------------------------------------------------------------------------
		void DrawGrid( SysGra& gra, Pers& pers, vect3 pos, int NUM_U, int NUM_V, float dt, rgb col )
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
					g_line3d_scissor( gra, pers, a, b, col );
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
					g_line3d_scissor( gra, pers, a, b, col );
					a+=vt;
					b+=vt;
				}
			}			

			{//原点表示
				float r = 0.1;
				vect3 v0;
				for ( int i = 0 ; i <= 360 ; i+=20 )
				{
					vect3 p = vect3( r*cos(deg2rad((float)i)), 0, r*sin(deg2rad((float)i)) ) + pos;
					vect3 v1 = pers.calcWorldToScreen3( p );
					if ( i > 0 ) gra.Line( v0,v1, col );
					v0 = v1;
				}
			}
		}
*/
		//------------------------------------------------------------------------------
		void DrawGrid3d( SysGra& gra, Pers& pers,  vect3 pos, mat33 m, int NUM_U, int NUM_V, float dt, rgb col  )
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
					g_line3d_scissor( gra, pers, v0, v1, col );
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
					g_line3d_scissor( gra, pers, v0, v1, col );
					a+=vt;
					b+=vt;
				}
			}			

			{//原点表示
				float r = 0.1;
				vect3 a;
				for ( int i = 0 ; i <= 360 ; i+=20 )
				{
					vect3 b = vect3( r*cos(deg2rad((float)i)), 0, r*sin(deg2rad((float)i)) ) + pos;
					if ( i > 0 ) 
					{
						vect3 v0 = a * m;
						vect3 v1 = b * m;
						g_line3d( gra, pers, v0,v1, col );
					}
					a = b;
				}
			}
		}
		//------------------------------------------------------------------------------
		void Draw2D( SysGra& gra, Pers& pers, vect2 pos, rgb col )
		//------------------------------------------------------------------------------
		{
			gra.Line( vect2(-1,pos.y), vect2(1,pos.y), col*0);
			gra.Line( vect2(pos.x,-1), vect2(pos.x,1), col*0);
			for ( float x = -10 ; x < 10 ; x += 1 )
			{
				gra.Line( vect2(pos.x+x/pers.aspect,-1), vect2(pos.x+x/pers.aspect,1), col );
			}
			for ( float y = -10 ; y < 10 ; y += 1 )
			{
				gra.Line( vect2(-1,pos.y+y), vect2(1,pos.y+y), col );
			}

			gra.Circle( pos, 0.05, col );
		
		}

	} grid;
	
	
	// タイヤ
	struct Tire
	{

		vector<vect3> vert;
		vector<vect3> disp;

		//------------------------------------------------------------------------------
		Tire()
		//------------------------------------------------------------------------------
		{
			float r = 1.0;
			for ( float t = 0 ; t < 2*pi ; t+=deg2rad(5) )
			{
				float x = 0;
				float y = r * sin(t);
				float z = r * cos(t);

				vert.emplace_back( x, y, z );
			}
		}

		//------------------------------------------------------------------------------
		void DrawRing( SysGra& gra, Pers& pers, vect3 pos, float head, float bank, float radius )
		//------------------------------------------------------------------------------
		{
			mat33 m = mrotz(bank) * mroty(head);

			rgb col(0,1,1);
			int i = 0;
			vect3 v0;
			for ( vect3 v : vert )
			{
				vect3 v1 = v*radius * m + pos;

				if ( i > 0 ) 
				{
					g_line3d( gra, pers, v0, v1, col, 2 );

					// 影
					vect3 a0= v0;
					vect3 a1= v1;
					a0.y=0;
					a1.y=0;
					g_line3d( gra, pers, a0, a1, col/4, 2 );
				}

				v0 = v1;
				i++;
			}

		}
		
	} tire;


	struct
	{

		// ドラム
		struct Drum
		{

			const float s = 0.05f;
			const float l = 0.03f;
			const float m = 0.02f;
			const float n = 0.02f;
			vector<vect3> vert=
			{
				{	-s,		 s,		-s	},
				{	 s,		 s,		-s	},
				{	-s,		-s,		-s	},
				{	 s,		-s,		-s	},
				{	-s,		 s,		 s	},
				{	 s,		 s,		 s	},
				{	-s,		-s,		 s	},
				{	 s,		-s,		 s	},

				{	-l,		-s-l, 	-l	},	//yマーク
				{	 l,		-s-l, 	-l	},
				{	-l,		-s-l, 	 l	},
				{	 l,		-s-l, 	 l	},

				{	s+m,	+m, 	-m	},	//xマーク
				{	s+m,	+m, 	+m	},
				{	s+m,	-m, 	 0	},

				{	-n,	 -n, 	s+n	},	//zマーク
				{	+n,	 -n, 	s+n	},
				{	-n,	 +n, 	s+n	},
				{	+n,	 +n, 	s+n	},
			};
			vector<vect3> disp;

			vector<ivect2>	edge
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
			vector<ivect3>	tri
			{
	//				{	14,	13, 12	},	// xマーク
			};

			//------------------------------------------------------------------------------
			Drum()
			//------------------------------------------------------------------------------
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
			}

			//------------------------------------------------------------------------------
			void DrawDrum( SysGra& gra, Pers& pers,  vect3 pos, mat33 m  )
			//------------------------------------------------------------------------------
			{
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
					g_line3d( gra, pers, pos, pos+nx*0.2, rgb(1,0,0) );
					g_line3d( gra, pers, pos, pos+ny*0.2, rgb(0,1,0) );
					g_line3d( gra, pers, pos, pos+nz*0.2, rgb(0,0,1) );
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

					g_line3d( gra, pers, a, b, col, false );
				}
			}
			
		} drum;
	
		//-------------------------------------------------------------------------
		void skeleton_update( SysKeys& keys, SysMouse& mouse, Pers& pers, Skeleton& skeleton )
		//-------------------------------------------------------------------------
		{
			if ( skeleton.bActive )
			{

				if ( skeleton.cur.bSelecting ==false && keys.SHIFT.on && (keys.UP.hi || keys.DOWN.hi || keys.LEFT.hi ||keys.RIGHT.hi) )
				{
					skeleton.cur.bSelecting = true;
					skeleton.cur.selecting_act = skeleton.cur.act;
					skeleton.cur.selecting_pose = skeleton.cur.pose;
				}
				if ( !keys.SHIFT.on && (keys.UP.hi || keys.DOWN.hi || keys.LEFT.hi ||keys.RIGHT.hi) )
				{
					skeleton.cur.bSelecting = false;
				}
				
				// キーフレームロード
/*
				if ( keys.CTRL.on && keys.L.hi ) 
				{
					//読み込み
					unique_ptr<Skeleton> pNew(new Skeleton);
					//pNew->LoadSkeleton( "human.mot" );
					pNew->LoadSkeleton( "bone.mot" );
				}
*/
				// キーフレームセーブ
				if ( keys.CTRL.on && keys.S.hi ) skeleton.SaveSkeleton();

				// キーフレームペースト
				if ( keys.CTRL.on && keys.V.hi ) skeleton.PastKeyframe();

				// キーフレームコピー
				if ( keys.CTRL.on && keys.C.hi ) skeleton.CopyKeyframe();

				// キーフレームカット
				if ( keys.CTRL.on && keys.X.hi ) skeleton.CutKeyframe();

				// キーフレーム追加
				if ( keys.K.hi ) skeleton.InsertKeyframe();

				// キーフレーム次
				if ( keys.RIGHT.rep ) skeleton.NextKeyframe();

				// キーフレーム最後
				if ( keys.CTRL.on && keys.RIGHT.rep ) skeleton.LastKeyframe();

				// キーフレーム前
				if ( keys.LEFT.rep ) skeleton.PrevKeyframe();

				// キーフレーム先頭
				if ( keys.CTRL.on && keys.LEFT.rep ) skeleton.TopKeyframe();

				// アニメーション記録
				if ( keys.I.hi ) skeleton.AddAnimation();

				// アニメーション前
				if ( keys.UP.rep ) skeleton.PrevAnimation();

				// アニメーション次
				if ( keys.DOWN.rep ) skeleton.NextAnimation();

				// アニメーションリクエスト
				if ( keys.P.hi ) skeleton.ReqAnimation();

				// アニメーション再生
				if ( skeleton.anim.bPlaying )	skeleton.PlayAnimation();


				if ( keys._1.hi ) skeleton.stat.bShowBone = !skeleton.stat.bShowBone;
				if ( keys._2.hi ) skeleton.stat.bShowSkin = !skeleton.stat.bShowSkin;
				if ( keys._3.hi ) skeleton.stat.bShowLocus = !skeleton.stat.bShowLocus;
			}
			

		}

		//-------------------------------------------------------------------------
		void skeleton_draw( SysGra& gra, SysKeys& keys, SysMouse& mouse, Pers& pers, Skeleton& skeleton, int& text_y )
		//-------------------------------------------------------------------------
		{
			if ( skeleton.bActive == false ) return;

		#if 0 	//剛体実験
			// 優先度つけ
			{
				int n = 0;
				for ( Obj* po : skeleton.tblPoint )
				{
					Joint* p = dynamic_cast<Joint*>(po);

					p->weight = 0.33;
					if ( n == 2  ) p->weight = 0.000;
					if ( n == 0  ) p->weight = 0.000;
					if ( n == 1  ) p->weight = 0.000;
					if ( n == 3  ) p->weight = 0.000;
					if ( p->bSelected && mouse.L.on ) p->weight = 0.0;
					n++;
				}
			}
		#else
			// 優先度つけ
			{
				int n = 0;
				for ( Obj* po : skeleton.tblPoint )
				{
					Joint* p = dynamic_cast<Joint*>(po);

					p->weight = 0.33;
					if ( p->bSelected && mouse.L.on ) p->weight = 0.0;
					n++;
				}
			}
		#endif

			//スケルトン更新
			skeleton.UpdateSkeleton();


			// スケルトン 描画
			skeleton.DrawSkeleton( pers, gra );

			if(0)
			{
				mat33	mmune = midentity();
				static mat33	mkata = midentity();
				mat33	mhiji = midentity();;
				mat33	mte = midentity() ;
				vect3	pos0 = skeleton.tblPoint[0]->pos;
				vect3	pos1 = skeleton.tblPoint[1]->pos;
				vect3	p2 = skeleton.tblPoint[2]->pos;
				vect3	p3 = skeleton.tblPoint[3]->pos;
				vect3	p4 = skeleton.tblPoint[4]->pos;
				vect3	p5 = skeleton.tblPoint[5]->pos;
				// 箱 胸
				if(1)
				{
					vect3 nx,ny,nz;
					ny = (pos0-p3).normalize();
					nx = (pos1-p2).normalize();
					nz = cross(ny,nx).normalize();
					nx = cross(ny,nz).normalize();

					mmune = mat33(
						nx.x,	nx.y,	nx.z,
						ny.x,	ny.y,	ny.z,
						nz.x,	nz.y,	nz.z
					);

					box.DrawBox( gra, pers, pos0, mmune, false, false );
				}
				// 箱 肩
				if(1)
				{
					vect3 nx,ny,nz;
					ny = (p2-p4).normalize();
					nx = (p2-pos0).normalize();
					nz = cross(nx,ny).normalize();
					nx = cross(ny,nz).normalize();
					nz = cross(nx,ny).normalize();

					mat33	m(
						nx.x,	nx.y,	nx.z,
						ny.x,	ny.y,	ny.z,
						nz.x,	nz.y,	nz.z
					);

					if ( keys.CTRL.on )		mkata.rotateByAxis( (p4-p2).normalize(), deg2rad(0.5));
					if ( keys.SHIFT.on )	mkata.rotateByAxis( (p4-p2).normalize(), deg2rad(-0.5));
					
					box.DrawBox( gra, pers, p2, mkata );
				}
				// 箱 肘
				{
					vect3 nx,ny,nz;
					ny = (p2-p4).normalize();
					nx = cross((p2-p4).normalize(),(p4-p5).normalize()).normalize();
					nz = cross(nx,ny).normalize();

					mat33	m(
						nx.x,	nx.y,	nx.z,
						ny.x,	ny.y,	ny.z,
						nz.x,	nz.y,	nz.z
					);	
					mhiji = m;
	//					mhiji = skeleton.tblPoint[4].mat;
					drum.DrawDrum( gra, pers, p4, mhiji );
				}
				// 箱 手
				if(0)
				{
					vect3 nx,ny,nz;
					ny = (p4-p5).normalize();
					nx = (p2-pos0).normalize();
					nz = cross(nx,ny).normalize();
					nx = cross(ny,nz).normalize();
					nz = cross(nx,ny).normalize();

					mat33	m(
						nx.x,	nx.y,	nx.z,
						ny.x,	ny.y,	ny.z,
						nz.x,	nz.y,	nz.z
					);	
					mte = m;
					drum.DrawDrum( gra, pers, p5, mte );
				}
			}

			// animカーソルビュー cursor
			{
				gra.SetZTest( false );

				bool flg = false;
				for ( int y = 0 ; y < (signed)skeleton.animations.size() ; y++ )
				{
					for ( int x = 0 ; x < (signed)skeleton.animations[y].pose.size() ; x++ )
					{
						if ( skeleton.cur.bSelecting && ( y == skeleton.cur.selecting_act && x == skeleton.cur.selecting_pose ) ) flg=!flg;
						if ( skeleton.cur.bSelecting && ( y == skeleton.cur.act && x == skeleton.cur.pose ) ) flg=!flg;

						vect2 v = vect2( (float)x, (float)y )*vect2( 4, 8 ) + vect2(400,16);
						{
							gra.Fill( gra.Conv(v), gra.Conv(v+vect2(3,7)), rgb(1,1,1) );
						}

						if ( y == skeleton.cur.act && x == skeleton.cur.pose )
						{
							gra.Fill( gra.Conv(v+vect2(0,4)), gra.Conv(v+vect2(3,7)), rgb(1,0,0) );
						}
						
						if ( flg )
						{
							gra.Fill( gra.Conv(v+vect2(0,4)), gra.Conv(v+vect2(3,7)), rgb(1,0,0) );
						}
					}
				}

				gra.SetZTest( true );
			}


				
			#if 0
				{	//砲台
					v.normalize();
					float rx = acos(dot(vect3(0,1,0),v));
					float ry = atan2(v.x,v.z);
					
					mat44	mx;
					mat44	my;
					my.setRotateY(-ry);
					mx.setRotateX(-rx);
					mat44 m = mx * my;

					skeleton.skin.skin( gra, pers, vect3(  0,0,0), m );
				}
			#endif

			{
				static bool flgInfo = true;
				if( keys.F2.hi ) flgInfo = !flgInfo;
				if ( flgInfo )
				{

					gra.Print(1,(float)text_y++,string("[ ")+skeleton.filename+" ]");
					gra.Print(1,(float)text_y++,string("anim=")+to_string(skeleton.cur.act) + string(" cnt=")+to_string(skeleton.animations.size()) ); 
					if ( skeleton.animations.size() > 0 ) 
					{
						gra.Print(1,(float)text_y++,string("pose=")+to_string(skeleton.cur.pose) + string(" cnt=")+to_string(skeleton.animations[skeleton.cur.act].pose.size()) ); 
					}
				}

				if ( keys.F1.on )
				{
					gra.Print(1,(float)text_y++,string("[F1] help"));
					gra.Print(1,(float)text_y++,string("[Y] pers -"));
					gra.Print(1,(float)text_y++,string("[H] pers +"));
					gra.Print(1,(float)text_y++,string("[L] Load"));
					gra.Print(1,(float)text_y++,string("[S] Save"));
					gra.Print(1,(float)text_y++,string("--Keyframe--"));
					gra.Print(1,(float)text_y++,string("[K] Insert"));
					gra.Print(1,(float)text_y++,string("[X] Cut"));
					gra.Print(1,(float)text_y++,string("[C] Copy"));
					gra.Print(1,(float)text_y++,string("[V] Past"));
					gra.Print(1,(float)text_y++,string("[LEFT]  -"));
					gra.Print(1,(float)text_y++,string("[RIGHT] +"));
					gra.Print(1,(float)text_y++,string("--Animation--"));
					gra.Print(1,(float)text_y++,string("[I] Add"));
					gra.Print(1,(float)text_y++,string("[P] Play"));
					gra.Print(1,(float)text_y++,string("[UP] -"));
					gra.Print(1,(float)text_y++,string("[DOWN] +"));
					gra.Print(1,(float)text_y++,string("--Other--"));
					gra.Print(1,(float)text_y++,string("[1] select 3d"));
					gra.Print(1,(float)text_y++,string("[2] select 2main"));
				}
				else
				{
					//gra.Print(vect2(10,16*y++),string("[F1] Help"));
				}
			}
		}
	} util;


	struct
	{
		//=================================
		// 描画	角速度 実験
		//=================================
		void kakusokudo( SysKeys& keys, SysMouse& mouse, SysGra& gra, Pers& pers, Grid& grid )
		{
			static bool bInit = false;
			if ( !bInit )
			{
				bInit = true;
				g_tblPoint.clear();
				g_tblPoint.emplace_back( new Obj(vect3(0,0.1,0)) );
				g_tblPoint.emplace_back( new Obj(vect3(1,0.1,0)) );
				g_tblPoint.emplace_back( new Obj(vect3(1,0.1,0.4)) );
			}

			const float	G = 9.8;			// 重力加速度
			const float	T = 1.0/60.0;		// 時間/frame
			const float	g = 9.8 *T*T;		// 重力加速度/frame

			vect3&	v0 = g_tblPoint[0]->pos;	//	barの根本
			vect3&	v1 = g_tblPoint[1]->pos;	//	barの先端
			vect3&	v2 = g_tblPoint[2]->pos;	// 速度指定

			// 角度リセット
			if ( keys.R.hi )	bInit = false ;

			{
				vect3	bar = (v1-v0);									//	棒
				float	radius = bar.abs();
				vect3	moment = cross(-bar,v2-v1);						//	回転モーメント
				vect3	velocity = cross(bar/radius, moment/radius );	//	ベロシティ

			 	float	th = velocity.abs()/radius;						//	角速度
				vect3	to = mrotateByAxis( moment, th ) * bar;			//	移動計算

				{// 影 描画
					vect3	va = v0;;va.y = 0;
					vect3	vb = v0+to;vb.y = 0;
					g_line3d( gra, pers, va, vb, rgb(1,1,1)/4, 2 );
					va = v0;va.y = 0;
					vb = v0+moment;vb.y = 0;
					g_line3d( gra, pers, va, vb, rgb(1,1,1)/4, 2 );
					va = v1;va.y = 0;
					vb = v1+velocity;vb.y = 0;
					g_line3d( gra, pers, va, vb, rgb(1,1,1)/4, 2 );
					va = v0;va.y = 0;
					vb = v1;vb.y = 0;
					g_line3d( gra, pers, va, vb, rgb(1,1,1)/4, 2 );
				}
				g_line3d( gra, pers, v0, v1, rgb(1,1,1), 2 );	//	棒
				g_line3d( gra, pers, v1, v2, rgb(1,1,0), 1 );	// 外的な力
				g_line3d( gra, pers, v0, v0+to, rgb(0,1,1), 1 );	// 回転先ベクトル
				g_line3d( gra, pers, v0, v0+moment, rgb(1,0,1), 1 );	// モーメント/回転軸
				g_line3d( gra, pers, v1, v1+velocity, rgb(0,1,0), 1 );	// 移動ベクトル

				g_pset3d( gra, pers, v1+velocity, rgb(0,1,0),5 );g_print3d( gra, pers, v1+velocity, 0,0, "velocity" ); 
				g_pset3d( gra, pers, v0+moment, rgb(1,0,1),5 );g_print3d( gra, pers, v0+moment, 0,0, "moment" ); 
				g_pset3d( gra, pers, v0+to  , rgb(0,1,1),5 );g_print3d( gra, pers, v0+to  , 0,0, "to" ); 
//				gra.Print(1,(float)text_y++,string("th=")+to_string(rad2deg(th)) ); 
					
			}

		}
	
		void graph( SysGra& gra, Pers& pers, Grid& grid )
		{
	//		pers.cam.at = pers.cam.pos;
	//		pers.cam.at.z = 0;

			rgb	col = vect3(0.2,0.2,0.2);

			gra.Clr(rgb(0.3,0.3,0.3));
			grid.DrawGrid3d( gra, pers, vect3(0,0,0), mrotx(deg2rad(90)), 100, 100, 1, vect3(0.2,0.2,0.2) );
//				grid.Draw2D( gra, pers, vect2(0,0), rgb(0.2,0.2,0.2) );


			float n = 10;
			{
				float dt = 0.2;
				float a = 1*dt*dt;
				float v = 0;
				float s = 0;

				for ( float t = 0 ; t < n ; t+=dt )
				{
		//			g_pset2d( gra, pers, vect2(t,s), rgb(1,0,0), 5 );
		//			g_print2d( gra, pers, vect2(t,s), to_string(s) );

					v += a;
					s += v;
				}
			}

			{
				float a = 0.5;
				float v = 0;
				float s = 0;

				float t0 = 0;
				float v0 = v;
				float s0 = s;

				for ( float t = 0 ; t < n ; t+=0.1 )
				{
					v = a * t;
					s = v * t * 0.5f;

					g_line2d( gra, pers, vect2(t0,s0), vect2(t,s), rgb(0,0.5,1), 1 );
					g_line2d( gra, pers, vect2(t0,v0), vect2(t,v), rgb(0,1,0), 1 );

					g_pset2d( gra, pers, vect2(t,s), rgb(0,0.5,1), 3 );
					g_pset2d( gra, pers, vect2(t,v), rgb(0,1,0), 3 );
					g_print2d( gra, pers, vect2(t,s), to_string(s) );
				
					t0 = t;
					v0 = v;
					s0 = s;


				}
			}

		}
	} test;

	//------------------------------------------------------------------------------
	int main()
	//------------------------------------------------------------------------------
	{

		int text_y = 0;

		//=================================
		// skeleton 定義
		//=================================
		unique_ptr<Skeleton> pSkeleton(new Skeleton);

		//=================================
		// skeleton ロード
		//=================================
		if(0)
		{
			//読み込み
			unique_ptr<Skeleton> pNew(new Skeleton);
		//	pNew->LoadSkeleton( "human.mot" );
			pNew->LoadSkeleton( "bone.mot" );
			pNew->stat.bShowSkin = false;
			pNew->stat.bShowLocus = true;
			pSkeleton = move(pNew);
		}

		//=================================
		// cource 定義
		//=================================
		unique_ptr<Cource> pCource(new Cource);
//		(*pCource).bActive = true;	//	使用可能



		cout<<fixed<<setprecision(24);

	#if 1 // camera
		pers.cam.pos = vect3(  0.3, 0.7, -1.2 );
		pers.cam.at = vect3( 0,  0.7, 0 );



		//右から
		pers.cam.pos = vect3(  2.0, 0.5, -0.5 );
		pers.cam.at = vect3( 0,  0.0, 0 );


		//2D グラフ用
		pers.cam.pos = vect3(  0.0, 0.0, -7.0 );
		pers.cam.at = vect3( 0,  0.0, 0 );

		//上から
		pers.cam.pos = vect3(  0.0, 3.0, -0.5 );
		pers.cam.at = vect3( 0,  0.0, 0 );
	#endif


		//===========================================================================
		while( Update() )
		{
			text_y = 0;

			//=================================
			// カメラ
			//=================================
			{
				// パースペクティブ
				if (keys.Y.rep) {pers.fovy-=2;cout << pers.fovy <<" "<<1/tan(deg2rad(pers.fovy)) << endl; }
				if (keys.H.rep) {pers.fovy+=2;cout << pers.fovy <<" "<<1/tan(deg2rad(pers.fovy)) << endl; }

				// パース更新
				pers.Update( vect2( gra.GetWidth(), gra.GetHeight() ) );

				
				// カメラ回転
				if ( (!keys.ALT.on && mouse.R.on && !mouse.L.on && !mouse.M.on) || (keys.ALT.on && !mouse.R.on && mouse.L.on && !mouse.M.on) ) pers.cam.Rotation( -vect3(mouse.mov,0)*18.0f );

				// カメラ平行移動
				if ( mouse.M.on ) pers.cam.Move( -vect3(mouse.mov,0)/pers.getW((pers.cam.pos-pers.cam.at).abs()));

				// マウスホイールZOOM
				if ( !keys.ALT.on  ) pers.cam.Zoom( -mouse.wheel*2/gra.GetHeight()/pers.getW((pers.cam.pos-pers.cam.at).abs()) );
				
				// カメラ移動
				if ( (keys.ALT.on && mouse.R.on) || ( mouse.R.on && mouse.L.on ) ) pers.cam.Zoom( -mouse.mov.y/pers.getW((pers.cam.pos-pers.cam.at).abs()) );
				
				// カメラマトリクス計算
//				pers.cam.mat.LookAt( pers.cam.pos, pers.cam.at, pers.cam.up );
				pers.cam.Update();
			}

			//=================================
			// カーソルモード X/Y/Z軸選択モード切替
			//=================================

			if ( keys.Z.hi + keys.X.hi + keys.C.hi > 0 && keys.Z.on + keys.X.on + keys.C.on == keys.Z.hi + keys.X.hi + keys.C.hi ) 
			{
				// 排他的選択
				axis.bAxisZ = false;
				axis.bAxisX = false;
				axis.bAxisY = false;
				if ( keys.Z.hi ) axis.bAxisZ = true;
				if ( keys.X.hi ) axis.bAxisX = true;
				if ( keys.C.hi ) axis.bAxisY = true;
			}
			else
			{
				// 追加選択
				if ( keys.Z.on ) axis.bAxisZ = true;
				if ( keys.X.on ) axis.bAxisX = true;
				if ( keys.C.on ) axis.bAxisY = true;
			}

			//=================================
			// skeleton 入力
			//=================================
			util.skeleton_update( keys, mouse, pers, (*pSkeleton) );


			//=================================
			// キーフレームロード
			//=================================
			if ( keys.CTRL.on && keys.L.hi ) 
			{
				//読み込み
				unique_ptr<Skeleton> pNew(new Skeleton);
				//pNew->LoadSkeleton( "human.mot" );
				pNew->LoadSkeleton( "bone.mot" );
				pNew->stat.bShowSkin = false;
				pNew->stat.bShowLocus = false;
				pSkeleton = move(pNew);
			}
			if ( keys.CTRL.on && keys.SEMICOLON.hi ) 
			{
				//読み込み
				unique_ptr<Skeleton> pNew(new Skeleton);
				pNew->LoadSkeleton( "human.mot" );
				//pNew->LoadSkeleton( "bone.mot" );
				pNew->stat.bShowSkin = false;
				pNew->stat.bShowLocus = false;
				pSkeleton = move(pNew);
			}



			//=================================
			// 画面クリア
			//=================================
			gra.Clr(rgb(0.3,0.3,0.3));

			//=================================
			// 床グリッド描画
			//=================================
			grid.DrawGrid3d( gra, pers, vect3(0,0,0), midentity(), 10, 10, 1, rgb(0.2,0.2,0.2) );



			//=================================
			// マウス座標（投影面座標）を３Ｄ空間座標に逆変換＆描画
			//=================================

			if(0)
			{
			
				vect3 P = pers.calcScreenToWorld3( vect3(mouse.pos,0) );
				vect3 I = pers.calcRayvect( P );

				vect3 P2 = vect3( 0, 0.4, 0 );
				vect3 P3 = vect3( 0.5, 0.6, 0.4 );

				auto[b,d,Q0,Q1,t0,t1] = distanceLineSegline_func( P, I, P2, P3 );

				if ( b ) g_line3d( gra, pers, Q0, Q1,  vect3(0,1,0));
				else g_line3d( gra, pers, Q0, Q1,  vect3(1,0,0));

				g_line3d( gra, pers, P, P+I,  vect3(1,0,0));
				g_line3d( gra, pers, P2, P3, vect3(1,1,1));

				{
					vect3 a0 = g_tblPoint[0]->pos;
					vect3 a1 = g_tblPoint[1]->pos;
				
					static vect3 p(0,1,0);
					g_pset3d( gra, pers, p, rgb(1,1,1), 11 );
					g_pset3d( gra, pers, a0, rgb(0,1,1), 7 );
					g_pset3d( gra, pers, a1, rgb(1,1,0), 7 );
				
					p.rotateByAxis( a0, a1-a0, deg2rad(1) );
				}


			}


			//=================================
			//	登録
			//=================================
			gui.tbls.clear();
			if ( (*pCource).bActive ) (*pCource).idxTbl = gui.EntryTbl( (*pCource).tblPoint );
			if ( (*pSkeleton).bActive ) (*pSkeleton).idxTbl = gui.EntryTbl( (*pSkeleton).tblPoint );
			gui.EntryTbl( g_tblPoint );


			//=================================
			//	GUI操作
			//=================================
			{

				// 最近点検索
				if ( !keys.ALT.on && mouse.L.hi ) 
					gui.TouchFirst( pers, gui.tbls, mouse.pos );

				// 矩形カーソル開始 新規選択
				if ( !keys.ALT.on && mouse.L.on && !keys.CTRL.on && !keys.SHIFT.on && gui.one.bEnable == false && gui.rect_mode == G_CALC::NONE ) 
					gui.SelectRectOnly( mouse.pos );

				// 矩形カーソル開始 追加選択
				if ( !keys.ALT.on && mouse.L.on && !keys.CTRL.on && keys.SHIFT.on && gui.one.bEnable == false && gui.rect_mode == G_CALC::NONE ) 
					gui.SelectRectAdd( mouse.pos );

				// 矩形カーソル開始 反転選択
				if ( !keys.ALT.on && mouse.L.on && keys.CTRL.on && !keys.SHIFT.on && gui.one.bEnable == false && gui.rect_mode == G_CALC::NONE ) 
					gui.SelectRectRev( mouse.pos );

				// 矩形カーソル開始 削除選択
				if ( !keys.ALT.on && mouse.L.on && keys.CTRL.on && keys.SHIFT.on && gui.one.bEnable == false && gui.rect_mode == G_CALC::NONE ) 
					gui.SelectRectSub( mouse.pos );

				// 矩形カーソル終了（選択決定）
				if ( !keys.ALT.on && !mouse.L.on && gui.rect_mode != G_CALC::NONE ) 
					gui.SelectRectEnd( gui.tbls );

				// 矩形カーソル選択	
				if ( !keys.ALT.on && mouse.L.on && gui.rect_mode != G_CALC::NONE ) 
					gui.SelectRectBegin( pers, gui.tbls , mouse.pos );

				// 単独 新規選択
				if ( !keys.ALT.on && mouse.L.hi && !keys.CTRL.on && !keys.SHIFT.on && gui.one.bEnable && gui.tbls[ gui.one.idxTbl ][ gui.one.idxObj ]->bSelected == false ) 
					gui.SelectOneOnly( gui.tbls );

				// 単独 追加選択
				if ( !keys.ALT.on && mouse.L.hi && !keys.CTRL.on && keys.SHIFT.on && gui.one.bEnable ) 
					gui.SelectOneAdd( gui.tbls );

				// 単独 反転選択
				if ( !keys.ALT.on && mouse.L.hi && keys.CTRL.on && !keys.SHIFT.on && gui.one.bEnable ) 
					gui.SelectOneRev( gui.tbls );

				// 単独 削除選択
				if ( !keys.ALT.on && mouse.L.hi && keys.CTRL.on && keys.SHIFT.on && gui.one.bEnable ) 
					gui.SelectOneSub( gui.tbls );

				// 移動

				if ( !keys.ALT.on && mouse.L.on && !keys.CTRL.on && !keys.SHIFT.on && gui.one.bEnable ) 
				{
					bool bByCamera = false;
					if ( axis.bAxisX && axis.bAxisY && axis.bAxisZ )
					{
						bByCamera = true;
					}
					else
					{
						axis.bAxisX = true;
						axis.bAxisY = false;
						axis.bAxisZ = true;
					}
					gui.MoveObj( gra, pers, gui.tbls, mouse.pos, mouse.prev, mouse.mov, keys.T.on, bByCamera );

					if ( (*pSkeleton).bActive )
					{
						if ( (*pSkeleton).idxTbl == gui.one.idxTbl )
						{
							// キーフレームへ反映
							(*pSkeleton).RefrectKeyframe();
						}
					}
				}

			}

			//=================================
			// skeleton 描画
			//=================================
			util.skeleton_draw( gra, keys, mouse, pers, (*pSkeleton), text_y );

			//=================================
			//	infCource 表示
			//=================================
			if ( (*pCource).bActive ) 
			{
				// マウスベクトル
				vect3 P = pers.calcScreenToWorld3( vect3(mouse.pos,0) );
				vect3 I = pers.calcRayvect( P );

				// 表示 加工 ベジェ 三次曲線
				bezier.cource_exec_drawBezier( gra, pers, (*pCource).tblPoint, (*pCource).idxPoint, (*pCource).idxTbl, P, I, keys.E.on, mouse.L.hi );

			}



			//=================================
			// 表示 矩形カーソル、制御点
			//=================================
			gui.DrawController( pers, gra, gui.tbls, mouse.pos );
			
			//=================================
			// コース描画
			//=================================
			{
				static vector<Point3>	cource =
				{
					vect3(	-0.5,	0.12,	-0.5),
					vect3(	+0.5,	0.12,	-0.5),
					vect3(	+0.9,	0.12,	 0.5),
					vect3(	 0.0,	0.12,	 0.5),
					vect3(	-0.9,	0.12,	 0.5),

					vect3(	-0.5,	0.12,	-0.5)*1.2,
					vect3(	+0.5,	0.12,	-0.5)*1.2,
					vect3(	+0.9,	0.12,	 0.5)*1.2,
					vect3(	0.0,	0.12,	 0.5)*1.2,
					vect3(	-0.9,	0.12,	 0.5)*1.2,
				};
				
				static vector<ivect2>	idx =
				{
					{0,5},
					{1,6},
					{2,7},
					{3,8},
					{4,9},
				};
				
				// 選択 制御点
				if ( mouse.L.hi ) cource_selectCutmull( gra, pers, cource, mouse.pos );

				// 移動 制御点（スクリーン並行）
				if ( mouse.L.on ) cource_moveCutmull( pers, cource, mouse.mov );

				// 描画
			#if 0
				cource_drawCutmull( gra, pers, cource, idx );

				// 表示 制御点
				cource_drawPoint( gra, pers, cource );
			#endif
				
			}


			
			//=================================
			// 情報表示
			//=================================
			gra.Print(1,(float)text_y++,string("fovY:")+to_string(int(pers.fovy)));
			gra.Print(1,(float)text_y++,string("sz:")+to_string(pers.sz) +string(" fy:")+to_string(pers.fy));
			gra.Print(1,(float)text_y++,string("far:")+to_string((pers.cam.pos-pers.cam.at).abs())); 
			gra.Print(1,(float)text_y++,string("at  x=")+to_string(pers.cam.at.x)+string(" y=")+to_string(pers.cam.at.y)+string(" z=")+to_string(pers.cam.at.z) ); 
			gra.Print(1,(float)text_y++,string("pos x=")+to_string(pers.cam.pos.x)+string(" y=")+to_string(pers.cam.pos.y)+string(" z=")+to_string(pers.cam.pos.z) ); 
			gra.Print(1,(float)text_y++,string("peak=")+to_string(time_peak/1000)+string("msec") ); 

			gra.Print(1,(float)text_y++,string("idxTbl=")+to_string(gui.one.idxTbl) ); 
			gra.Print(1,(float)text_y++,string("idxObj=")+to_string(gui.one.idxObj) ); 

			//=================================
			// 描画	マニュピレーター
			//=================================
			axis.DrawAxis( mouse.pos, *this );

			//=================================
			// 描画	グラフ実験
			//=================================

		//	test.graph( gra, pers, grid );

			//=================================
			// 描画	角速度 実験
			//=================================
	//		test.kakusokudo( keys, mouse, gra, pers, grid );

#if 1
			//=================================
			// 描画	引力実験
			//=================================
			{
				static vect3	spd;
				static const 	int MaxPlanet = 300;
				static vect3	tblPlanet[MaxPlanet];
				static int		cntPlanet=0;;
				static bool bInit = false;
				if ( !bInit )
				{
					bInit = true;
					g_tblPoint.clear();
					g_tblPoint.emplace_back( new Obj(vect3(0,0.1,0)) );
					//g_tblPoint.emplace_back( new Obj(vect3(1,0.1,0)) );
					g_tblPoint.emplace_back( new Obj(vect3(1,0.1,-0.4)) );

					//上から
					pers.cam.pos = vect3(  0.0, 0.5, -2.0 );
					pers.cam.at = vect3( 0,  0.0, 0 );

					spd = vect3(0, 0.01, 0.02);
				}

				const float	G = 9.8;			// 重力加速度
				const float	T = 1.0/60.0;		// 時間/frame
				const float	g = 9.8 *T*T;		// 重力加速度/frame

				vect3&	v0 = g_tblPoint[0]->pos;	//	太陽
				vect3&	v1 = g_tblPoint[1]->pos;	//	地球



				float 	dis = (v0-v1).abs();	// 距離
				vect3 	dir = (v0-v1).normalize();		// 方向

				if ( dis < 0.01 ) dis = 0.01;
				
				spd = (spd + dir/dis/1000);

				v1 += spd; 
			
			
				tblPlanet[ cntPlanet++ ] = v1;	
				if ( cntPlanet >= MaxPlanet ) cntPlanet = 0;

				for ( int i = 0 ; i < MaxPlanet ; i++ )
				{
					g_pset3d( gra, pers, tblPlanet[i], rgb(0,1,1),3 );
				}

				// 角度リセット
				if ( keys.R.hi )	bInit = false ;
				
				

				

/*
				static bool bShot = false; 

				// 角度リセット
				if ( keys.R.hi )	{bInit = false ;bShot = false;}

				if ( mouse.B.hi )	bShot= !bShot ;

				{
					vect3	bar = (v1-v0);									//	棒
					float	radius = bar.abs();
					vect3	moment = cross(-bar,v2-v1);						//	回転モーメント
					vect3	velocity = cross(bar/radius, moment/radius );	//	ベロシティ

				 	static float	th = velocity.abs()/radius;						//	角速度


					if ( bShot )
					{
						  th += velocity.abs()/radius;						//	角速度
					}

					vect3	to = mrotateByAxis( moment, th ) * bar;			//	移動計算

					{// 影 描画
						vect3	va = v0;;va.y = 0;
						vect3	vb = v0+to;vb.y = 0;
						g_line3d( gra, pers, va, vb, rgb(1,1,1)/4, 2 );
						va = v0;va.y = 0;
						vb = v0+moment;vb.y = 0;
						g_line3d( gra, pers, va, vb, rgb(1,1,1)/4, 2 );
						va = v1;va.y = 0;
						vb = v1+velocity;vb.y = 0;
						g_line3d( gra, pers, va, vb, rgb(1,1,1)/4, 2 );
						va = v0;va.y = 0;
						vb = v1;vb.y = 0;
						g_line3d( gra, pers, va, vb, rgb(1,1,1)/4, 2 );
					}
					g_line3d( gra, pers, v0, v1, rgb(1,1,1), 2 );	//	棒
					g_line3d( gra, pers, v1, v2, rgb(1,1,0), 1 );	// 外的な力
					g_line3d( gra, pers, v0, v0+to, rgb(0,1,1), 1 );	// 回転先ベクトル
					g_line3d( gra, pers, v0, v0+moment, rgb(1,0,1), 1 );	// モーメント/回転軸
					g_line3d( gra, pers, v1, v1+velocity, rgb(0,1,0), 1 );	// 移動ベクトル

					g_pset3d( gra, pers, v1+velocity, rgb(0,1,0),5 );g_print3d( gra, pers, v1+velocity, 0,0, "velocity" ); 
					g_pset3d( gra, pers, v0+moment, rgb(1,0,1),5 );g_print3d( gra, pers, v0+moment, 0,0, "moment" ); 
					g_pset3d( gra, pers, v0+to  , rgb(0,1,1),5 );g_print3d( gra, pers, v0+to  , 0,0, "to" ); 
					gra.Print(1,(float)text_y++,string("th=")+to_string(rad2deg(th)) ); 
						
				}
*/

			}
#endif

#if 0
			//=================================
			// 描画	振り子実験
			//=================================
			{

				static bool bInit = false;
				if ( !bInit )
				{
					pers.cam.pos = vect3(  0.0, 0.0, -7.0 );
					pers.cam.at = vect3( 0,  0.0, 0 );

					bInit = true;
					g_tblPoint.clear();
					g_tblPoint.emplace_back( new Obj(vect3(0, 2.0, 0)) );
					g_tblPoint.emplace_back( new Obj(vect3(1, 2.0, 0)) );
				}

				gra.Clr(rgb(0.3,0.3,0.3));
				grid.DrawGrid3d( gra, pers, vect3(0,0,0), mrotx(deg2rad(90)), 100, 100, 1, vect3(0.2,0.2,0.2) );

				const float G = 9.8;			// 重力加速度
				const float T = 1.0/60.0;		// 時間/frame
				const float g = 9.8 *T*T;		// 重力加速度/frame

				vect3&	v0 = g_tblPoint[0]->pos;	//	barの根本
				vect3&	v1 = g_tblPoint[1]->pos;	//	barの先端


				// 角度リセット
				if ( keys.R.hi )	bInit = false ;

				// 縮む
				if ( mouse.F.hi )	v1 = (v1+v0)/2;

				// 伸びる
				if ( mouse.B.hi )	v1 = (v1-v0)*2+v0;


			#if 1
					vect3	bar = (v1-v0);									//	棒
					float	radius = bar.abs();								//	棒長さ
					vect3	moment = cross(-bar,-g);						//	回転モーメント
					vect3	velocity = cross(bar/radius, moment/radius );	//	ベロシティ

				 	float	th = velocity.abs()/radius;						//	角速度
					static vect3	to = bar;
					to += mrotateByAxis( moment, th ) * bar-bar;			//	移動計算



					g_line3d( gra, pers, v0, v0+to, rgb(1,1,1), 2 );

			#endif


			#if 0
				{
					static float	rsp = 0;
					static vect3	vv = 0;

					vect3 v = (v1-v0);

					// 角速度に変換
					{
						float b = atan2(v.x,v.y);
						float t = -g * sin(b);			//	接線速度
						float r = t/2/pi/v.abs();		//	角加速度
						rsp +=r;						//	角速度
					}

					// 回転
					{
						float x = v.x *cos(rsp) - v.y*sin(rsp); 
						float y = v.x *sin(rsp) + v.y*cos(rsp); 
						vect3 a = vect3(x,y,0);
						v1 = v0 + a;

						g_line3d( gra, pers, v1, v1+(a-v)*10, rgb(1,0,0),2 );
						
					}

					g_line3d( gra, pers, v0, v1, rgb(1,1,1), 2 );

					// 加速ベクトルの実験
					{
						vect3 vg = vect3(0,-g,0);	//	重力加速度ベクトル
						vect3 n0 = cross( v, vg );
						vect3 vt = cross( n0, v );

//						g_line3d( gra, pers, v1, v1+vt*100, rgb(0,1,0) );
						vect3 v2 = v*1.05+v0;
//						g_line3d( gra, pers, v2, v2+vv, rgb(0,1,0) );

//						g_line3d( gra, pers, v2, v2+vg*100, rgb(1,0,0) );
						g_line3d( gra, pers, v2, v2+vt*100, rgb(0,1,0) );
						g_line3d( gra, pers, v2, v2+n0*100, rgb(0,0,1) );					}

				}
			#endif
			#if 0
				{
					vect3 vg = vect3(0,-g,0);	//	重力加速度ベクトル
					vect3 v = (v2-v0).normalize();
					vect3 n0 = cross( v, vg );
					vect3 vt = cross( n0, v );

					vv += vt;
					v2 += vt*10;

//					g_line3d( gra, pers,  0,  vv*10, rgb(0,1,1), 2 );
					g_line3d( gra, pers,  vect3(1,0,0),  vect3(1,0,0)+vt*100, rgb(0,1,0), 1 );

					g_line3d( gra, pers, v2, v2+vt*100, rgb(0,1,0) );
					g_line3d( gra, pers, v0, v2, rgb(0,1,1), 2 );

				}
			#endif

			//gra.Print(1,(float)text_y++,string("len=")+to_string((v1-v0).abs()) ); 
				
			}
#endif
#if 0
			{
				gra.Clr(rgb(0.3,0.3,0.3));
				grid.DrawGrid3d( gra, pers, vect3(0,0,0), mrotx(deg2rad(90)), 100, 100, 1, vect3(0.2,0.2,0.2) );

				const float G = 9.8;				// 重力加速度
				const float T = 1.0/60.0;			// 時間/frame
				const float grate = 9.8 *T*T;		// 重力加速度/frame

				static float	radius = 1.0;
				static vect3	v0 = vect3(0, 2,0);
				static vect3	v1 = v0+vect3(radius,0,0);
				static float	rsp = 0;

				g_line3d( gra, pers, v0, v1, rgb(1,1,1), 2 );

				// 角度リセット
				if ( keys.R.hi )	{v1 = v0+vect3((v1-v0).abs(),0,0);rsp=0;}

				// 縮む
				if ( mouse.F.hi )	v1 = (v1+v0)/2;

				// 伸びる
				if ( mouse.B.hi )	v1 = (v1-v0)*2+v0;

				{
					vect3 v = v1-v0;
					float b = atan2(v.x,v.y);

					// 角速度に変換
					float tsp = -grate * sin(b);	//	接線速度
					float r = tsp/2/pi/v.abs();		//	角加速度
					rsp +=r;						//	角速度

					// 回転
					float x = v.x *cos(rsp) - v.y*sin(rsp); 
					float y = v.x *sin(rsp) + v.y*cos(rsp); 
					v1 = v0+vect3(x,y,0);

				}

			//gra.Print(1,(float)text_y++,string("len=")+to_string((v1-v0).abs()) ); 
				
			}
#endif

			//=================================
			// 描画	タイヤ実験
			//=================================
if(0)
			{
				const vect3 G_pos = vect3(0,2.0,0);
				const vect3 G_acc= vect3(0,0,0.0);
				const float G_radius = 0.5;
				const float G_head = deg2rad(35);	//	タイヤの方向
				const float G_bank = deg2rad(40);	//	回転角
				const float G_rspd = deg2rad(0);	//	角速度
				const float G = 9.8;				// 重力加速度
				const float grate = 9.8 / 60/60;	// 重力加速度

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
//						acc.y = -acc.y * 0.5; 
//					acc.y = 0;
#if 0
					// 回転
					{
						float a = atan( acc.y * sin(bank) / radius );
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
				tire.DrawRing( gra, pers, pos, head, bank, radius );
			}



			//=================================
			// 処理時間表示
			//=================================
			{
				static chrono::system_clock::duration time_a;
				static chrono::system_clock::duration time_b;
				static chrono::system_clock::duration time_sec;
				static chrono::system_clock::duration time_now;
				static chrono::system_clock::duration time_max;

				time_b = chrono::system_clock::now().time_since_epoch(); 
				if ( time_max < time_b-time_a ) time_max = time_b-time_a;

				// ウェイト(60fps)
				while( chrono::system_clock::now().time_since_epoch()-time_a < chrono::microseconds(16666) )
				{
	 				this_thread::sleep_for(chrono::microseconds(100));
				}

				// 表示
			    time_now = chrono::system_clock::now().time_since_epoch();
				if ( time_now-time_sec > chrono::seconds(2) ) // n sec毎表示
				{
					time_sec = chrono::system_clock::now().time_since_epoch();

					long long f2 = chrono::duration_cast<chrono::microseconds>(time_max).count();
					time_peak = f2;
					time_max = chrono::seconds(0);
				}
				time_a = chrono::system_clock::now().time_since_epoch();  
			}

		}

		for ( Obj* p : g_tblPoint )
		{
			delete p;
		}

		return 0;
	}


} ;


//------------------------------------------------------------------------------
int main()
//------------------------------------------------------------------------------
{
	cout << "start" << endl;
	Apr	apr("Ray4 " __DATE__, 300,300,768, 512 );
//	Apr	apr("Ray4 " __DATE__, 300,300,320, 200 );

	apr.SetWincursor( false );

	return apr.main();
}

