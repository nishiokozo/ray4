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

#include "pers.h"
#include "func.h"

#include "raytrace.h"
#include "skeleton.h"

using namespace std;


struct Point3
{
	bool	bSelected = false;
	bool 	bPreselect		= false;		//	仮選択

	vect3	pos;
	chrono::system_clock::duration time_pos;
	Point3( vect3 _pos ) : pos(_pos) {}
	vect3	a;
	vect3	b;
	bool	bSelected_a = false;
	bool	bSelected_b = false;
	chrono::system_clock::duration time_a;
	chrono::system_clock::duration time_b;
	Point3( vect3 _pos, vect3 _a, vect3 _b ) : pos(_pos) , a(_a), b(_b){}
};

//------------------------------------------------------------------------------
auto curce_select = []( SysGra& gra, Pers& pers, vector<Point3>& tbl, vect2& mpos )
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
void courcr_move( Pers& pers, vector<Point3>& tbl, vect2& mmov )
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
			mat44 mrot = pers.cam.mat;
			mrot.SetTranslate(vect3(0,0,0));
			v = v* mrot;
			c.pos += v;
			
		}
	}
}
struct Cource
{
//	vector<Point3> tbl;
//	vector<int>	idx;
	vector<Point3> tbl =
	{
		{vect3(-1.0, 0.0, 0.0 ),vect3( 0.0, 0.0, 1.0 ),vect3( 0.0, 0.0,-1.0 )},
		{vect3( 1.0, 0.0, 0.0 ),vect3( 0.0, 0.0,-1.0 ),vect3( 0.0, 0.0, 1.0 )},

	};
	vector<int>	idx =
	{
		0,1,0

	};
} infCource;

struct Bezier
{
/*	vector<Point3> tbl =
	{
		{vect3(-1.0, 0.0, 0.0 ),vect3( 0.0, 0.0, 1.0 ),vect3( 0.0, 0.0,-1.0 )},
		{vect3( 1.0, 0.0, 0.0 ),vect3( 0.0, 0.0,-1.0 ),vect3( 0.0, 0.0, 1.0 )},

	};
	vector<int>	idx =
	{
		0,1,0

	};
*/
	struct
	{

		bool	bRect = false;
		vect2	rect_st;

		bool	bSelectedOnlyOne = false;	// 一つだけ選択フラグ
	} stat;

		struct Selector
		{
			struct
			{
				float	w;
				Point3*	pj;
			} one = {0,0};

			vect2 rect_st = vect2(0,0);			//	矩形選択開始位置
			enum class CALC
			{
				NONE,
				ADD,
				SUB,
				COPY,
				REV,
			};
			CALC rect_mode = CALC::NONE;	//	矩形選択中フラグ

			//------------------------------------------------------------------------------
			void Setup( Pers& pers, Cource& skeleton, vect2 mpos )
			//------------------------------------------------------------------------------
			{
				one = {0,0};

				for ( Point3& j : skeleton.tbl )
				{
//					if ( j.bCtrl == false ) continue;

					vect3 v = pers.calcWorldToScreen3( j.pos );

					if ( (vect2(v.x,v.y)-mpos).abs() < 0.04f )
					{
						if ( one.w < v.z ) // 近い場合はより手前が優先
						{
							one.w = v.z;
							one.pj = &j;
						}
					}
				}
			}

			// 矩形カーソル開始 新規選択
			//------------------------------------------------------------------------------
			void SelectRectNew( vect2 mpos )
			//------------------------------------------------------------------------------
			{
				rect_mode = CALC::COPY;
				rect_st = mpos;
			}

			// 矩形カーソル開始 追加選択
			//------------------------------------------------------------------------------
			void SelectRectAdd( vect2 mpos )
			//------------------------------------------------------------------------------
			{
				rect_mode = CALC::ADD;
				rect_st = mpos;
			}

			// 矩形カーソル開始 反転選択
			//------------------------------------------------------------------------------
			void SelectRectRev( vect2 mpos )
			//------------------------------------------------------------------------------
			{
				rect_mode = CALC::REV;
				rect_st = mpos;
			}

			// 矩形カーソル開始 削除選択
			//------------------------------------------------------------------------------
			void SelectRectSub( vect2 mpos )
			//------------------------------------------------------------------------------
			{
				rect_mode = CALC::SUB;
				rect_st = mpos;
			}
			
			//------------------------------------------------------------------------------
			void calcRectMode( CALC rect_mode, bool& bPreselect, bool& bSelected )
			//------------------------------------------------------------------------------
			{
				switch( rect_mode )
				{
					case CALC::ADD:		if ( bPreselect ) bSelected = true;
						break;

					case CALC::SUB:		if ( bPreselect ) bSelected = false;
						break;

					case CALC::COPY:	bSelected = bPreselect;
						break;

					case CALC::REV:		if ( bPreselect ) bSelected = !bSelected;
						break;

					case CALC::NONE:
						break;
				}
				bPreselect = false;
			};


			// 矩形カーソル終了（選択決定）
			//------------------------------------------------------------------------------
			void SelectRectEnd( Cource& skeleton )
			//------------------------------------------------------------------------------
			{
				for ( Point3& j : skeleton.tbl )
				{
					calcRectMode( rect_mode, j.bPreselect, j.bSelected );
				}

				rect_mode = CALC::NONE;

			}

			// 矩形カーソル選択	
			//------------------------------------------------------------------------------
			void SelectRectBegin( Pers& pers, Cource& skeleton , vect2 mpos )
			//------------------------------------------------------------------------------
			{
				vect2 v0 = min( rect_st, mpos );
				vect2 v1 = max( rect_st, mpos );

				for ( Point3& j : skeleton.tbl )
				{
//					if ( j.bCtrl == false ) continue;
				
					j.bPreselect = false;

					vect2 v = pers.calcWorldToScreen2( j.pos );

					if ( v.x > v0.x && v.x < v1.x && v.y > v0.y && v.y < v1.y )
					{
						j.bPreselect = true;
					}
				}
			}

			// 単独 新規選択
			//------------------------------------------------------------------------------
			void SelectOneOnly( Cource& skeleton )
			//------------------------------------------------------------------------------
			{
				// 選択クリア
				for ( Point3& j : skeleton.tbl )
				{
					j.bSelected = false;
				}
				
				one.pj->bSelected = true;
			}

			// 単独 追加選択
			//------------------------------------------------------------------------------
			void SelectOneAdd()
			//------------------------------------------------------------------------------
			{
				one.pj->bSelected = true;
			}

			// 単独 反転選択
			//------------------------------------------------------------------------------
			void SelectOneRev()
			//------------------------------------------------------------------------------
			{
				one.pj->bSelected = !one.pj->bSelected;
			}

			// 単独 削除選択
			//------------------------------------------------------------------------------
			void SelectOneSub()
			//------------------------------------------------------------------------------
			{
				one.pj->bSelected = false;
			}

			
/*
			// 選択リストのPoint3移動
			//------------------------------------------------------------------------------
			void MoveSelected( Pers& pers, Cource& skeleton, vect2 gmov )
			//------------------------------------------------------------------------------
			{

				vect3 v = vect3(gmov.x*pers.aspect, gmov.y, 0)/one.w/pers.rate;
				mat44 mrot = pers.cam.mat;
				mrot.SetTranslate(vect3(0,0,0));
			//	mrot.invers(); 逆行列にしなくても同じ結果
				v = v* mrot;
				for ( Point3& j : skeleton.tbl )
				{
					if ( j.bSelected )
					{
						j.pos += v ;
					}
				}
				
				// キーフレームへ反映
//				skeleton.RefrectKeyframe();
			}

			//--
*/			

			// 選択リスト表示
			//------------------------------------------------------------------------------
			void DrawPoint( Pers& pers, SysGra& gra, Cource& infCource , vect2 mpos )
			//------------------------------------------------------------------------------
			{	
				gra.SetZTest( false );
/*
				for ( Point3& j : infCource.tbl )
				{

					bool bPreselect = j.bPreselect;
					bool bSelected = j.bSelected;
					
					calcRectMode( rect_mode, bPreselect, bSelected );

					if ( bSelected )
					{
						gra.Pset( pers.calcWorldToScreen3( j.pos ), rgb(1,0,0), 11 );
					}

				}
*/
				// 矩形カーソル 表示
				if (  rect_mode != CALC::NONE )
				{
					gra.Box( rect_st, mpos, rgb(1,1,1)*0.5f);
				}

				// 矩形カーソル 情報表示
				for ( Point3& j : infCource.tbl )
				{
//					if ( j.bCtrl == false ) continue;

					vect2 pos = pers.calcWorldToScreen2( j.pos );
//					gra.Print( pos+gra.Dot(14,0), to_string(j.id) );
				}

				gra.SetZTest( true );
			}

		} selector;
/*	
	//------------------------------------------------------------------------------
	void selectBeginRect( vect2& mpos )
	//------------------------------------------------------------------------------
	{
		stat.rect_st = mpos;
		stat.bRect = true; 
	}
	//------------------------------------------------------------------------------
	void selectEndRect()
	//------------------------------------------------------------------------------
	{
		stat.bRect = false;
	}
	//------------------------------------------------------------------------------
	void selectDragRect( SysGra& gra, Pers& pers, vect2& mpos )
	//------------------------------------------------------------------------------
	{
		gra.Box( stat.rect_st, mpos, rgb(0,1,1));

		float x0 = min( stat.rect_st.x, mpos.x ); 
		float x1 = max( stat.rect_st.x, mpos.x ); 
		float y0 = min( stat.rect_st.y, mpos.y ); 
		float y1 = max( stat.rect_st.y, mpos.y ); 

		for ( Point3& c : tbl )
		{
			vect3 v = pers.calcWorldToScreen3( c.pos );
			
			if ( v.z > 0 )
			{
				if ( x0 < v.x && x1 > v.x && y0 < v.y && y1 > v.y )
				{
					c.bSelected = true;
			//		c.time_pos = 0;
				}
			}
		}

	}
	
	//------------------------------------------------------------------------------
	void curce_selectClear()
	//------------------------------------------------------------------------------
	{
		// 全解除
		{
			for ( Point3& c : tbl )
			{
				c.bSelected = false;
				c.bSelected_a = false;
				c.bSelected_b = false;
			}
		}
	}
	//------------------------------------------------------------------------------
	bool IsTouchNode( SysGra& gra, Pers& pers, vect2& mpos )
	//------------------------------------------------------------------------------
	{
		//カーソルが特定ノードに触れてイルカどうかを判断

		bool bTouch = false;

		for ( Point3& c : tbl )
		{
			if ( c.bSelected )
			{
				{
					vect3 v = pers.calcWorldToScreen3( c.pos+c.a );
					if ( gra.Pixel(mpos-v.xy()).abs() < 16 )
					{
						bTouch = true;
						break;
					}
				}

				{
					vect3 v = pers.calcWorldToScreen3( c.pos+c.b );
					if ( gra.Pixel(mpos-v.xy()).abs() < 16 )
					{
						bTouch = true;
						break;
					}
				}

			}
				{
					vect3 v = pers.calcWorldToScreen3( c.pos );
					if ( gra.Pixel(mpos-v.xy()).abs() < 16 )
					{
						bTouch = true;
						break;
					}
				}
		}

		return bTouch = bTouch;
	}


*/	

	//------------------------------------------------------------------------------
	bool curce_selectOnlyOne( SysGra& gra, Pers& pers, Cource& infCource, vect2& mpos )
	//------------------------------------------------------------------------------
	{
		chrono::system_clock::duration time = chrono::system_clock::now().time_since_epoch();

		for ( Point3& c : infCource.tbl )
		{
			c.bSelected_a = false;
			c.bSelected_b = false;
		}

		bool bTouch = false;

		for ( Point3& c : infCource.tbl )
		{
			if ( c.bSelected )
			{
				{
					vect3 v = pers.calcWorldToScreen3( c.pos+c.a );
					if ( gra.Pixel(mpos-v.xy()).abs() < 16 )
					{
						c.bSelected_a = true;
						c.time_a = time;
						bTouch = true;
						break;
					}
				}

				{
					vect3 v = pers.calcWorldToScreen3( c.pos+c.b );
					if ( gra.Pixel(mpos-v.xy()).abs() < 16 )
					{
						c.bSelected_b = true;
						c.time_b = time;
						bTouch = true;
						break;
					}
				}
			}
		}
		
		if ( bTouch == false )
		{
			Point3* pHit = 0;
			for ( Point3& c : infCource.tbl )
			{
				vect3 v = pers.calcWorldToScreen3( c.pos );
				if ( gra.Pixel(mpos-v.xy()).abs() < 16 )
				{
					pHit = &c;
				}
			}

			if ( pHit )
			{
				Point3& c = *pHit;
				if ( c.bSelected == false )
				{
					// 新規選択
					for ( Point3& c : infCource.tbl )
					{
						c.bSelected = false;
					}
					c.bSelected = true;
					c.time_pos = time;
				}
					bTouch = true;
				
			}
			
		}
		
		return bTouch;	
	};

	//------------------------------------------------------------------------------
	void courcr_moveBezier( SysGra& gra, Pers& pers, Cource& infCource, vect2& mmov, bool bCtrl, bool bShfit, bool bAlt )
	//------------------------------------------------------------------------------
	{
					vect3 v = vect3(mmov.x*pers.aspect, mmov.y, 0)/selector.one.w/pers.rate;
					mat44 mrot = pers.cam.mat;
					mrot.SetTranslate(vect3(0,0,0));
					v = v* mrot;
					for ( Point3& j : infCource.tbl )
					{
						if ( j.bSelected )
						{
							j.pos += v ;
						}
					}
return;

//	cout << "a" << endl;
		enum Element
		{
			Element_none,
			Element_a,
			Element_b,
			Element_pos,
		};
		
		Element	elem = Element_none;

		Point3*	pLast = 0;

		vect2	scale;
		{
			chrono::system_clock::duration time_max;
			// 最終選択を求める
			for ( Point3& c : infCource.tbl )
			{
				if ( c.bSelected )
				{
					if ( time_max < c.time_pos ) 
					{
						time_max = c.time_pos;	

						vect3 v = pers.calcWorldToScreen3( c.pos );
						scale = vect2(pers.aspect, 1)/v.z/pers.rate;
						elem = Element_pos;

						pLast = &c;
					}
					if ( c.bSelected_a )
					{
						if ( time_max < c.time_a ) 
						{
							time_max = c.time_a;	

							vect3 v = pers.calcWorldToScreen3( c.pos + c.a );
							scale = vect2(pers.aspect, 1)/v.z/pers.rate;
							elem = Element_a;

							pLast = &c;
						}
					}
					if ( c.bSelected_b )
					{
						if ( time_max < c.time_b ) 
						{
							time_max = c.time_b;	

							vect3 v = pers.calcWorldToScreen3( c.pos + c.b );
							scale = vect2(pers.aspect, 1)/v.z/pers.rate;
							elem = Element_b;

							pLast = &c;
						}
					}
				}
			}

		}

		if ( pLast && ( elem == Element_a || elem == Element_b ) )
		{
			Point3&c = *pLast;
					if ( elem == Element_a )
					{
						if ( c.bSelected_a )
						{
							vect3 v = vect3( mmov * scale, 0 );
							mat33 mrot = pers.cam.mat.GetRotate();
							v = v* mrot;
							c.a += v;
		if(bAlt)
							c.b = -c.a.normalize()*c.b.abs();
						}
					}
					if ( elem == Element_b )
					{
						if ( c.bSelected_b )
						{
							vect3 v = vect3( mmov * scale, 0 );
							mat33 mrot = pers.cam.mat.GetRotate();
							v = v* mrot;
							c.b += v;
		if(bAlt)
							c.a = -c.b.normalize()*c.a.abs();
						}
					}
		}
		else
		{
		// 移動
			int i=0;
			for ( Point3& c : infCource.tbl )
			{
				if ( c.bSelected )
				{

					if ( elem == Element_pos )
					{
						vect3 v = vect3( mmov * scale, 0 );
						mat44 mrot = pers.cam.mat;
						mrot.SetTranslate(vect3(0,0,0));
						v = v* mrot;
						c.pos += v;
					}
				}
				i++;
			}
		}
	}


	//------------------------------------------------------------------------------
	void cource_drawBezier( SysGra& gra, Pers& pers, Cource& infCource, vect3& P, vect3& I, bool bSerch, bool bCut )
	//------------------------------------------------------------------------------
	{
		//ベジェ計算＆描画
		float div = 10;
		float dt = 1/div;

		int size = static_cast<signed>(infCource.idx.size());

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
			int n0 = infCource.idx[n];
			int n1 = infCource.idx[n+1];
			vect3 P0 =    infCource.tbl[n0].pos;
			vect3 P1 = P0+infCource.tbl[n0].b;
			vect3 P3 =    infCource.tbl[n1].pos;
			vect3 P2 = P3+infCource.tbl[n1].a;

			float t  = dt;
			vect3 p0 = infCource.tbl[n0].pos;
			for ( int i = 0 ; i < div ; i++ )
			{
				vect3 p1 = bezier3_func( t, P0, P1, P2, P3 );
				g_line3d( gra, pers, p0, p1, rgb(1,1,1) );

				// マウスベクトルとの最近点
				if ( bSerch )
				{
					auto[b,d,Q0,Q1,t0,t1] = distanceLineSegline_func( P, I, p0, p1 );
					if ( b ) 
					{
						if ( mind > d && 0.2 > d )
						{
							minb	= true;
							mind	= d;
							minn	= n;

							minQ0 = Q0;
							minQ1 = Q1;

							mint	= t-(1/div) + (t1/(p1-p0).abs())/(div);
							minQ	= bezier3_func( mint, P0, P1, P2, P3 );
							mindt	= bezier3_delta_func( mint, P0, P1, P2, P3 );

							minv = (minQ-Q0).normalize();
						}
					}

				}

				p0=p1;
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
				vect3	p = minQ;
				vect3	v = mindt/3;
				float	t0 = mint;
				float	t1 = 1.0-mint;

				infCource.tbl.emplace_back( p, -v*t0, v*t1  );
				infCource.idx.insert( infCource.idx.begin()+minn+1, infCource.tbl.size()-1);

				infCource.tbl[infCource.idx[minn+0]].b *= t0;
				infCource.tbl[infCource.idx[minn+2]].a *= t1;
				infCource.tbl[infCource.idx[minn+1]].bSelected = true;
			}

			{
				gra.SetZTest( false );
	//			vect3 v = pers.calcWorldToScreen3( minQ );
				g_print3d( gra, pers, minQ, -26,-32-20, to_string(mint) ); 
	//			gra.Print( v.xy() + gra.Dot(-26,-32-20), to_string(mint) ); 
				gra.SetZTest( true );
			}
		}

		// 表示 制御点 	制御線
#if 0
		{
			gra.SetZTest(false);
			
			int n = 0;
			for ( Point3 c : infCource.tbl )
			{	
				float wide = 11;
				float wide2 = 7;
				rgb	col = rgb(0,0,1);
				rgb	col2 = rgb(0,1,0);
				rgb	col2a = rgb(0,1,0);
				rgb	col2b = rgb(0,1,0);
				if ( c.bSelected || c.bSelected_a || c.bSelected_b ) 
				{
					if ( c.bSelected ) 
					{
						col = rgb(1,0,0);
					}
					if ( c.bSelected_a ) 
					{
						col2a = rgb(1,0,0);
					}
					if ( c.bSelected_b ) 
					{
						col2b = rgb(1,0,0);
					}
					g_line3d( gra, pers, c.pos, c.pos+c.a, col2 );
					g_line3d( gra, pers, c.pos, c.pos+c.b, col2 );
					g_pset3d( gra, pers, c.pos+c.a, col2a, wide2 ); 
					g_pset3d( gra, pers, c.pos+c.b, col2b, wide2 ); 
				}

				g_pset3d( gra, pers, c.pos, col, wide ); 

				g_print3d( gra, pers, c.pos, 16,12, to_string(n) ); 
				n++;
			}
			gra.SetZTest(true);
		}
#else
				for ( Point3& j : infCource.tbl )
				{

					bool bPreselect = j.bPreselect;
					bool bSelected = j.bSelected;
					
					selector.calcRectMode( selector.rect_mode, bPreselect, bSelected );

					if ( bSelected )
					{
						gra.Pset( pers.calcWorldToScreen3( j.pos ), rgb(1,0,0), 11 );
					}
					else
					{
						gra.Pset( pers.calcWorldToScreen3( j.pos ), rgb(0,0,1), 11 );
					}

				}
#endif

	};
} bezier;

//------------------------------------------------------------------------------
void cource_drawPoint( SysGra& gra, Pers& pers, vector<Point3>& tbl )
//------------------------------------------------------------------------------
{
	int cnt = 0 ;
	gra.SetZTest(false);
	for ( Point3 c : tbl )
	{
		vect3 v = pers.calcWorldToScreen3( c.pos );
		if ( v.z > 0 )
		{
			if ( c.bSelected ) 
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
		g_line3d( gra, pers, w2, v2, col);
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
	void pset3d( vect3 p0, rgb col, float wide )
	//------------------------------------------------------------------------------
	{
		vect3 v = pers.calcWorldToScreen3( p0 );
		if ( v.z > 0 )
		{
			gra.Pset( vect2(v.x,v.y), col, wide );
		}

	}

	

	struct
	{
		//------------------------------------------------------------------------------
		void DrawGrid( Apr& apr, vect3 pos, int NUM_U, int NUM_V, float dt, rgb col )
		//------------------------------------------------------------------------------
		{	// ミニグリッド
			vect3 vt = vect3(0,0,0);
			float du = NUM_U*dt;
			float dv = NUM_V*dt;
			vect3 a;
			vect3 b;
			{
				a = pos+vect3(-du, 0,-du);
				b = pos+vect3( du, 0,-du);
				vt = vect3(0,0,dt);
				for ( int i = 0 ; i < NUM_V*2+1 ; i++ )
				{
					g_line3d_scissor( apr.gra, apr.pers, a, b, col );
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
					g_line3d_scissor( apr.gra, apr.pers, a, b, col );
					a+=vt;
					b+=vt;
				}
			}			

			{//原点表示
				float r = 0.1;
				vect3 v0;
				for ( int i = 0 ; i <= 360 ; i+=20 )
				{
					vect3 p = vect3( r*cos(rad(i)), 0, r*sin(rad(i)) ) + pos;
					vect3 v1 = apr.pers.calcWorldToScreen3( p );
					if ( i > 0 ) apr.gra.Line( v0,v1, col );
					v0 = v1;
				}
			}
		}

	} grid;
	
	






	struct
	{
		struct Selector
		{
			struct
			{
				float	w;
				Joint*	pj;
			} one = {0,0};

			vect2 rect_st = vect2(0,0);			//	矩形選択開始位置
			enum class CALC
			{
				NONE,
				ADD,
				SUB,
				COPY,
				REV,
			};
			CALC rect_mode = CALC::NONE;	//	矩形選択中フラグ

			//------------------------------------------------------------------------------
			void Setup( Pers& pers, Skeleton& skeleton, vect2 mpos )
			//------------------------------------------------------------------------------
			{
				one = {0,0};

				for ( Joint& j : skeleton.tblJoint )
				{
					if ( j.bCtrl == false ) continue;

					vect3 v = pers.calcWorldToScreen3( j.pos );

					if ( (vect2(v.x,v.y)-mpos).abs() < 0.04f )
					{
						if ( one.w < v.z ) // 近い場合はより手前が優先
						{
							one.w = v.z;
							one.pj = &j;
						}
					}
				}
				if ( one.pj )
				{
					#if 0
					{
						// 優先度つけ
						for ( Joint& j : skeleton.tblJoint )
						{
							j.priority = 999;
						}
						function<void( Joint&,int)> funcSetPriority = [&funcSetPriority] ( Joint& j, int prio )
						{
							j.priority = prio;
							for ( Joint& r : j.relative )
							{
								if ( r.priority > prio+1 ) funcSetPriority( r, prio+1 );
							}
						};
						
						funcSetPriority( *one.pj, 1 );
					}
					#endif

				}
			}

			// 矩形カーソル開始 新規選択
			//------------------------------------------------------------------------------
			void SelectRectNew( vect2 mpos )
			//------------------------------------------------------------------------------
			{
				rect_mode = CALC::COPY;
				rect_st = mpos;
			}

			// 矩形カーソル開始 追加選択
			//------------------------------------------------------------------------------
			void SelectRectAdd( vect2 mpos )
			//------------------------------------------------------------------------------
			{
				rect_mode = CALC::ADD;
				rect_st = mpos;
			}

			// 矩形カーソル開始 反転選択
			//------------------------------------------------------------------------------
			void SelectRectRev( vect2 mpos )
			//------------------------------------------------------------------------------
			{
				rect_mode = CALC::REV;
				rect_st = mpos;
			}

			// 矩形カーソル開始 削除選択
			//------------------------------------------------------------------------------
			void SelectRectSub( vect2 mpos )
			//------------------------------------------------------------------------------
			{
				rect_mode = CALC::SUB;
				rect_st = mpos;
			}
			
			//------------------------------------------------------------------------------
			void calcRectMode( CALC rect_mode, bool& bPreselect, bool& bSelected )
			//------------------------------------------------------------------------------
			{
				switch( rect_mode )
				{
					case CALC::ADD:		if ( bPreselect ) bSelected = true;
						break;

					case CALC::SUB:		if ( bPreselect ) bSelected = false;
						break;

					case CALC::COPY:	bSelected = bPreselect;
						break;

					case CALC::REV:		if ( bPreselect ) bSelected = !bSelected;
						break;

					case CALC::NONE:
						break;
				}
				bPreselect = false;
			};


			// 矩形カーソル終了（選択決定）
			//------------------------------------------------------------------------------
			void SelectRectEnd( Skeleton& skeleton )
			//------------------------------------------------------------------------------
			{
				for ( Joint& j : skeleton.tblJoint )
				{
					calcRectMode( rect_mode, j.stat.bPreselect, j.stat.bSelected );
				}

				rect_mode = CALC::NONE;

			}

			// 矩形カーソル選択	
			//------------------------------------------------------------------------------
			void SelectRectBegin( Pers& pers, Skeleton& skeleton , vect2 mpos )
			//------------------------------------------------------------------------------
			{
				vect2 v0 = min( rect_st, mpos );
				vect2 v1 = max( rect_st, mpos );

				for ( Joint& j : skeleton.tblJoint )
				{
					if ( j.bCtrl == false ) continue;
				
					j.stat.bPreselect = false;

					vect2 v = pers.calcWorldToScreen2( j.pos );

					if ( v.x > v0.x && v.x < v1.x && v.y > v0.y && v.y < v1.y )
					{
						j.stat.bPreselect = true;
					}
				}
			}

			// 単独 新規選択
			//------------------------------------------------------------------------------
			void SelectOneOnly( Skeleton& skeleton )
			//------------------------------------------------------------------------------
			{
				// 選択クリア
				for ( Joint& j : skeleton.tblJoint )
				{
					j.stat.bSelected = false;
				}
				
				one.pj->stat.bSelected = true;
			}

			// 単独 追加選択
			//------------------------------------------------------------------------------
			void SelectOneAdd()
			//------------------------------------------------------------------------------
			{
				one.pj->stat.bSelected = true;
			}

			// 単独 反転選択
			//------------------------------------------------------------------------------
			void SelectOneRev()
			//------------------------------------------------------------------------------
			{
				one.pj->stat.bSelected = !one.pj->stat.bSelected;
			}

			// 単独 削除選択
			//------------------------------------------------------------------------------
			void SelectOneSub()
			//------------------------------------------------------------------------------
			{
				one.pj->stat.bSelected = false;
			}

/*			
			// 選択リストのJoint移動
			//------------------------------------------------------------------------------
			void MoveSelected( Pers& pers, Skeleton& skeleton, vect2 gmov )
			//------------------------------------------------------------------------------
			{

				vect3 v = vect3(gmov.x*pers.aspect, gmov.y, 0)/one.w/pers.rate;
				mat44 mrot = pers.cam.mat;
				mrot.SetTranslate(vect3(0,0,0));
			//	mrot.invers(); 逆行列にしなくても同じ結果
				v = v* mrot;
				for ( Joint& j : skeleton.tblJoint )
				{
					if ( j.stat.bSelected )
					{
						j.pos += v ;
					}
				}
				
				// キーフレームへ反映
				skeleton.RefrectKeyframe();
			}
			
			//--
*/
			// 選択リスト表示
			//------------------------------------------------------------------------------
			void DrawPoint( Pers& pers, SysGra& gra, Skeleton& skeleton , vect2 mpos )
			//------------------------------------------------------------------------------
			{	
				gra.SetZTest( false );

				for ( Joint& j : skeleton.tblJoint )
				{

					bool bPreselect = j.stat.bPreselect;
					bool bSelected = j.stat.bSelected;
					
					calcRectMode( rect_mode, bPreselect, bSelected );

					if ( bSelected )
					{
						gra.Pset( pers.calcWorldToScreen3( j.pos ), rgb(1,0,0), 11 );
					}

				}

				// 矩形カーソル 表示
				if (  rect_mode != CALC::NONE )
				{
					gra.Box( rect_st, mpos, rgb(1,1,1)*0.5f);
				}

				// 矩形カーソル 情報表示
				for ( Joint& j : skeleton.tblJoint )
				{
					if ( j.bCtrl == false ) continue;

					vect2 pos = pers.calcWorldToScreen2( j.pos );
					gra.Print( pos+gra.Dot(14,0), to_string(j.id) );
				}

				gra.SetZTest( true );
			}

		} selector;

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

			Drum()
			{
				{
					int	ofs = vert.size();
					int cnt = 0;
					float r = 0.05;
					float w = 0.05;
					for ( int i = 0 ; i < 360 ; i+= 30 )
					{
						float z = r*cos(rad(i));
						float y = r*sin(rad(i));
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
	//			if ( skeleton.bActive )
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
			
			//=================================
			// skeleton ジョイント選択
			//=================================
			if ( skeleton.bActive )
			{

				// 最近点検索
				if ( !keys.ALT.on && mouse.L.hi ) 
					selector.Setup( pers, skeleton, mouse.pos );

				// 矩形カーソル開始 新規選択
				if ( !keys.ALT.on && mouse.L.on && !keys.CTRL.on && !keys.SHIFT.on && selector.one.pj == 0 && selector.rect_mode == Selector::CALC::NONE ) 
					selector.SelectRectNew( mouse.pos );

				// 矩形カーソル開始 追加選択
				if ( !keys.ALT.on && mouse.L.on && !keys.CTRL.on && keys.SHIFT.on && selector.one.pj == 0 && selector.rect_mode == Selector::CALC::NONE ) 
					selector.SelectRectAdd( mouse.pos );

				// 矩形カーソル開始 反転選択
				if ( !keys.ALT.on && mouse.L.on && keys.CTRL.on && !keys.SHIFT.on && selector.one.pj == 0 && selector.rect_mode == Selector::CALC::NONE ) 
					selector.SelectRectRev( mouse.pos );

				// 矩形カーソル開始 削除選択
				if ( !keys.ALT.on && mouse.L.on && keys.CTRL.on && keys.SHIFT.on && selector.one.pj == 0 && selector.rect_mode == Selector::CALC::NONE ) 
					selector.SelectRectSub( mouse.pos );

				// 矩形カーソル終了（選択決定）
				if ( !keys.ALT.on && !mouse.L.on && selector.rect_mode != Selector::CALC::NONE ) 
					selector.SelectRectEnd( skeleton );

				// 矩形カーソル選択	
				if ( !keys.ALT.on && mouse.L.on && selector.rect_mode != Selector::CALC::NONE ) 
					selector.SelectRectBegin( pers, skeleton , mouse.pos );

				// 単独 新規選択
				if ( !keys.ALT.on && mouse.L.hi && !keys.CTRL.on && !keys.SHIFT.on && selector.one.pj && selector.one.pj->stat.bSelected == false ) 
					selector.SelectOneOnly( skeleton );

				// 単独 追加選択
				if ( !keys.ALT.on && mouse.L.hi && !keys.CTRL.on && keys.SHIFT.on && selector.one.pj ) 
					selector.SelectOneAdd();

				// 単独 反転選択
				if ( !keys.ALT.on && mouse.L.hi && keys.CTRL.on && !keys.SHIFT.on && selector.one.pj ) 
					selector.SelectOneRev();

				// 単独 削除選択
				if ( !keys.ALT.on && mouse.L.hi && keys.CTRL.on && keys.SHIFT.on && selector.one.pj ) 
					selector.SelectOneSub();
				
				// 選択リストのJoint移動
				if ( !keys.ALT.on && mouse.L.on && !keys.CTRL.on && !keys.SHIFT.on && selector.one.pj ) 
				{
	//					Skeleton& skeleton = skeleton;
					vect2 gmov = mouse.mov;

					vect3 v = vect3(gmov.x*pers.aspect, gmov.y, 0)/selector.one.w/pers.rate;
					mat44 mrot = pers.cam.mat;
					mrot.SetTranslate(vect3(0,0,0));
				//	mrot.invers(); 逆行列にしなくても同じ結果
					v = v* mrot;
					for ( Joint& j : skeleton.tblJoint )
					{
						if ( j.stat.bSelected )
						{
							j.pos += v ;
						}
					}
					
					// キーフレームへ反映
					skeleton.RefrectKeyframe();
				}
			}

		}

		//-------------------------------------------------------------------------
		void skeleton_draw( SysGra& gra, SysKeys& keys, SysMouse& mouse, Pers& pers, Skeleton& skeleton, int& text_y )
		//-------------------------------------------------------------------------
		{
			if ( skeleton.bActive == false ) return;

		#if 1 	//剛体実験
			// 優先度つけ
			for ( Joint& j : skeleton.tblJoint )
			{
				j.weight = 0.33;
				if ( j.id == 2  ) j.weight = 0.000;
				if ( j.id == 0  ) j.weight = 0.000;
				if ( j.id == 1  ) j.weight = 0.000;
				if ( j.id == 3  ) j.weight = 0.000;
				if ( j.stat.bSelected && mouse.L.on ) j.weight = 0.0;
			}
		#endif

			//スケルトン更新
			skeleton.UpdateSkeleton();


			// スケルトン 描画
			skeleton.DrawSkeleton( pers, gra );

			{
				mat33	mmune = midentity();
				static mat33	mkata = midentity();
				mat33	mhiji = midentity();;
				mat33	mte = midentity() ;
				vect3	p0 = skeleton.tblJoint[0].pos;
				vect3	p1 = skeleton.tblJoint[1].pos;
				vect3	p2 = skeleton.tblJoint[2].pos;
				vect3	p3 = skeleton.tblJoint[3].pos;
				vect3	p4 = skeleton.tblJoint[4].pos;
				vect3	p5 = skeleton.tblJoint[5].pos;
				// 箱 胸
				if(1)
				{
					vect3 nx,ny,nz;
					ny = (p0-p3).normalize();
					nx = (p1-p2).normalize();
					nz = cross(ny,nx).normalize();
					nx = cross(ny,nz).normalize();

					mmune = mat33(
						nx.x,	nx.y,	nx.z,
						ny.x,	ny.y,	ny.z,
						nz.x,	nz.y,	nz.z
					);

					box.DrawBox( gra, pers, p0, mmune, false, false );
				}
				// 箱 肩
				if(1)
				{
					vect3 nx,ny,nz;
					ny = (p2-p4).normalize();
					nx = (p2-p0).normalize();
					nz = cross(nx,ny).normalize();
					nx = cross(ny,nz).normalize();
					nz = cross(nx,ny).normalize();

					mat33	m(
						nx.x,	nx.y,	nx.z,
						ny.x,	ny.y,	ny.z,
						nz.x,	nz.y,	nz.z
					);

					if ( keys.CTRL.on )		mkata.rotateByAxis( (p4-p2).normalize(), rad(0.5));
					if ( keys.SHIFT.on )	mkata.rotateByAxis( (p4-p2).normalize(), rad(-0.5));
					
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
	//					mhiji = skeleton.tblJoint[4].mat;
					drum.DrawDrum( gra, pers, p4, mhiji );
				}
				// 箱 手
				if(0){
					vect3 nx,ny
					,nz;
					ny = (p4-p5).normalize();
					nx = (p2-p0).normalize();
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

			// 表示
			selector.DrawPoint( pers, gra, skeleton , mouse.pos );

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

						vect2 v = vect2( x, y )*vect2( 4, 8 ) + vect2(400,16);
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

					skeleton.ring.ring_DrawMat( gra, pers, vect3(  0,0,0), m );
				}
			#endif

			{
				static bool flgInfo = true;
				if( keys.F2.hi ) flgInfo = !flgInfo;
				if ( flgInfo )
				{

					gra.Print(1,text_y++,string("[ ")+skeleton.filename+" ]");
					gra.Print(1,text_y++,string("anim=")+to_string(skeleton.cur.act) + string(" cnt=")+to_string(skeleton.animations.size()) ); 
					if ( skeleton.animations.size() > 0 ) 
					{
						gra.Print(1,text_y++,string("pose=")+to_string(skeleton.cur.pose) + string(" cnt=")+to_string(skeleton.animations[skeleton.cur.act].pose.size()) ); 
					}
				}

				if ( keys.F1.on )
				{
					gra.Print(1,text_y++,string("[F1] help"));
					gra.Print(1,text_y++,string("[Y] pers -"));
					gra.Print(1,text_y++,string("[H] pers +"));
					gra.Print(1,text_y++,string("[L] Load"));
					gra.Print(1,text_y++,string("[S] Save"));
					gra.Print(1,text_y++,string("--Keyframe--"));
					gra.Print(1,text_y++,string("[K] Insert"));
					gra.Print(1,text_y++,string("[X] Cut"));
					gra.Print(1,text_y++,string("[C] Copy"));
					gra.Print(1,text_y++,string("[V] Past"));
					gra.Print(1,text_y++,string("[LEFT]  -"));
					gra.Print(1,text_y++,string("[RIGHT] +"));
					gra.Print(1,text_y++,string("--Animation--"));
					gra.Print(1,text_y++,string("[I] Add"));
					gra.Print(1,text_y++,string("[P] Play"));
					gra.Print(1,text_y++,string("[UP] -"));
					gra.Print(1,text_y++,string("[DOWN] +"));
					gra.Print(1,text_y++,string("--Other--"));
					gra.Print(1,text_y++,string("[1] select 3d"));
					gra.Print(1,text_y++,string("[2] select 2main"));
				}
				else
				{
					//gra.Print(vect2(10,16*y++),string("[F1] Help"));
				}
			}
		}
	} util;


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
			pNew->stat.bShowLocus = false;
			pSkeleton = move(pNew);
		}

		cout<<fixed<<setprecision(24);

	#if 1 // camera
		pers.cam.pos = vect3(  0.3, 0.7, -1.2 );
		pers.cam.at = vect3( 0,  0.7, 0 );

		pers.cam.pos = vect3(  0.3, 1.7, -1.7 );
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
				if (keys.Y.rep) {pers.fovy-=2;cout << pers.fovy <<" "<<1/tan(rad(pers.fovy)) << endl; }
				if (keys.H.rep) {pers.fovy+=2;cout << pers.fovy <<" "<<1/tan(rad(pers.fovy)) << endl; }

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
				pers.cam.mat.LookAt( pers.cam.pos, pers.cam.at, pers.cam.up );
			}

			//=================================
			// カーソルモード X/Y/Z軸選択モード切替
			//=================================
			if ( keys.Z.hi ) {axis.bAxisZ = true;	axis.bAxisX = false;	axis.bAxisY = false;}
			if ( keys.X.hi ) {axis.bAxisZ = false;	axis.bAxisX = true;		axis.bAxisY = false;}
			if ( keys.C.hi ) {axis.bAxisZ = false;	axis.bAxisX = false;	axis.bAxisY = true;}
			if ( keys.V.hi ) {axis.bAxisZ = true;	axis.bAxisX = true;		axis.bAxisY = true;}
			if ( keys.V.hi && keys.SHIFT.on ) {axis.bAxisZ = false;	axis.bAxisX = false;	axis.bAxisY = false;}

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



			//=================================
			// 画面クリア
			//=================================
			gra.Clr(rgb(0.3,0.3,0.3));

			//=================================
			// 床グリッド描画
			//=================================
			grid.DrawGrid( *this, vect3(0,0,0), 10, 10, 1, vect3(0.2,0.2,0.2) );


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
			}

			//=================================
			//3字曲線
			//=================================
			{
/*				// 最初のチェック
				if ( mouse.L.hi ) bezier.IsTouchNode( gra, pers, mouse.pos );

				gra.Print(1,text_y++,string("bTouch:")+to_string(int(bTouch)));

				// 選択 制御点
				if ( mouse.L.hi && !keys.SHIFT.on ) bezier.curce_selectOnlyOne( gra, pers, mouse.pos );

				// 矩形選択
				if ( mouse.L.hi && !bTouch ) bezier.selectBeginRect( mouse.pos );
				if ( !mouse.L.on && bezier.stat.bRect ) bezier.selectEndRect();
				if ( mouse.L.on && bezier.stat.bRect ) bezier.selectDragRect( gra, pers, mouse.pos );

				// 全解除
//				if ( bTouch == false .bRect == false ) bezier.curce_selectClear();

				// 移動 制御点（スクリーン並行）
				if ( mouse.L.on && bTouch )  bezier.courcr_moveBezier( gra, pers, mouse.mov, keys.CTRL.on, keys.SHIFT.on, keys.ALT.on );
*/

#if 0
				if ( mouse.L.hi ) bezier.curce_selectOnlyOne( gra, pers, infCource, mouse.pos );
				if ( mouse.L.on )  bezier.courcr_moveBezier( gra, pers, infCource, mouse.mov, keys.CTRL.on, keys.SHIFT.on, keys.ALT.on );
#else
				// 最近点検索
				if ( !keys.ALT.on && mouse.L.hi ) 
					bezier.selector.Setup( pers, infCource, mouse.pos );

				// 矩形カーソル開始 新規選択
				if ( !keys.ALT.on && mouse.L.on && !keys.CTRL.on && !keys.SHIFT.on && bezier.selector.one.pj == 0 && bezier.selector.rect_mode == Bezier::Selector::CALC::NONE ) 
					bezier.selector.SelectRectNew( mouse.pos );

				// 矩形カーソル開始 追加選択
				if ( !keys.ALT.on && mouse.L.on && !keys.CTRL.on && keys.SHIFT.on && bezier.selector.one.pj == 0 && bezier.selector.rect_mode == Bezier::Selector::CALC::NONE ) 
					bezier.selector.SelectRectAdd( mouse.pos );

				// 矩形カーソル開始 反転選択
				if ( !keys.ALT.on && mouse.L.on && keys.CTRL.on && !keys.SHIFT.on && bezier.selector.one.pj == 0 && bezier.selector.rect_mode == Bezier::Selector::CALC::NONE ) 
					bezier.selector.SelectRectRev( mouse.pos );

				// 矩形カーソル開始 削除選択
				if ( !keys.ALT.on && mouse.L.on && keys.CTRL.on && keys.SHIFT.on && bezier.selector.one.pj == 0 && bezier.selector.rect_mode == Bezier::Selector::CALC::NONE ) 
					bezier.selector.SelectRectSub( mouse.pos );

				// 矩形カーソル終了（選択決定）
				if ( !keys.ALT.on && !mouse.L.on && bezier.selector.rect_mode != Bezier::Selector::CALC::NONE ) 
					bezier.selector.SelectRectEnd( infCource );

				// 矩形カーソル選択	
				if ( !keys.ALT.on && mouse.L.on && bezier.selector.rect_mode != Bezier::Selector::CALC::NONE ) 
					bezier.selector.SelectRectBegin( pers, infCource , mouse.pos );

				// 単独 新規選択
				if ( !keys.ALT.on && mouse.L.hi && !keys.CTRL.on && !keys.SHIFT.on && bezier.selector.one.pj && bezier.selector.one.pj->bSelected == false ) 
					bezier.selector.SelectOneOnly( infCource );

				// 単独 追加選択
				if ( !keys.ALT.on && mouse.L.hi && !keys.CTRL.on && keys.SHIFT.on && bezier.selector.one.pj ) 
					bezier.selector.SelectOneAdd();

				// 単独 反転選択
				if ( !keys.ALT.on && mouse.L.hi && keys.CTRL.on && !keys.SHIFT.on && bezier.selector.one.pj ) 
					bezier.selector.SelectOneRev();

				// 単独 削除選択
				if ( !keys.ALT.on && mouse.L.hi && keys.CTRL.on && keys.SHIFT.on && bezier.selector.one.pj ) 
					bezier.selector.SelectOneSub();

				// 移動
				if ( !keys.ALT.on && mouse.L.on && !keys.CTRL.on && !keys.SHIFT.on && bezier.selector.one.pj ) 
					bezier.courcr_moveBezier( gra, pers, infCource, mouse.mov, keys.CTRL.on, keys.SHIFT.on, keys.ALT.on );

				// 表示 制御
				bezier.selector.DrawPoint( pers, gra, infCource , mouse.pos );
#endif

				// マウスベクトル
				vect3 P = pers.calcScreenToWorld3( vect3(mouse.pos,0) );
				vect3 I = pers.calcRayvect( P );

				// 表示 加工 ベジェ 三次曲線
				bezier.cource_drawBezier( gra, pers, infCource, P, I, keys.E.on, mouse.L.hi );


			}
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
				if ( mouse.L.hi ) curce_select( gra, pers, cource, mouse.pos );

				// 移動 制御点（スクリーン並行）
				if ( mouse.L.on ) courcr_move( pers, cource, mouse.mov );

				// 描画
			//	cource_drawCutmull( gra, pers, cource, idx );

				// 表示 制御点
			//	cource_drawPoint( gra, pers, cource );

				
			}
			
			//=================================
			// 情報表示
			//=================================
			gra.Print(1,text_y++,string("fovY:")+to_string(int(pers.fovy)));
			gra.Print(1,text_y++,string("sz:")+to_string(pers.sz) +string(" fy:")+to_string(pers.fy));
			gra.Print(1,text_y++,string("far:")+to_string((pers.cam.pos-pers.cam.at).abs())); 
			gra.Print(1,text_y++,string("at  x=")+to_string(pers.cam.at.x)+string(" y=")+to_string(pers.cam.at.y)+string(" z=")+to_string(pers.cam.at.z) ); 
			gra.Print(1,text_y++,string("pos x=")+to_string(pers.cam.pos.x)+string(" y=")+to_string(pers.cam.pos.y)+string(" z=")+to_string(pers.cam.pos.z) ); 
			gra.Print(1,text_y++,string("peak=")+to_string(time_peak/1000)+string("msec") ); 

			//=================================
			// skeleton 更新
			//=================================
			util.skeleton_draw( gra, keys, mouse, pers, (*pSkeleton), text_y );
	
			//=================================
			// マニュピレーター描画
			//=================================
//			axis.DrawAxis( mouse.pos, *this );

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
		return 0;
	}


} ;


//------------------------------------------------------------------------------
int main()
//------------------------------------------------------------------------------
{
	cout << "start" << endl;
	Apr	apr("Ray4 " __DATE__, 300,300,768, 512 );
	return apr.main();
}

