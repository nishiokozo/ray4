//2020/04/21

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
#include "lab35.h"

using namespace std;

struct Lab35::Impl
{
	const int	RINGSIZE =	10;

//	bool	bResetAll = true;
//	bool	bResetParam = true;
//	bool	bPause = false;
//	bool	bStep = false;


	enum State
	{
		Reset,
		Title,
		Play,
		End,
	};


	State	valState = State::Reset;
	float	timeState = 0.0;

		static const	int MaxRound = 3;
		int numRound = 0;
		int numPlayer = 0;
		int tblScore[MaxRound];
	
		int	 hiscore = 0;
		bool	flgClear = false;

//	vector<shared_ptr<Obj>>	tbl_pObj;
	struct Stone
	{
		vect2	prev_pos;
		vect2	pos;
		float	radius;
		vect2	vel;
		
		Stone( vect2 _pos, float _r ) :  pos(_pos), radius(_r) {}
		Stone( vect2 _pos, float _r, vect2 _mov ) :  pos(_pos), radius(_r), vel(_mov) {}
	
		int point = 0;
	};

	vector<Stone>	tblStone;
	
//	mat33	mat = mat33::mrotx(rad(90));
	mat33	mat = mat33::mrotx(rad(0));

	//------------------------------------------------------------------------------
	void StateReset( SysKeys& keys, SysMouse& mouse, SysSound& sound, SysGra& gra, Pers& pers )
	//------------------------------------------------------------------------------
	{

		// カメラ
		pers.cam.pos = vect3( 0.0, 0.0, -25.0 );
		pers.cam.at = vect3( 0,  0.0, 0 );
		pers.cam.Update();

		// ストーン
//		pImpl->tblStone.emplace_back( Impl::Stone(vect2(1,0),0.5) );
//		pImpl->tblStone.emplace_back( Impl::Stone(vect2(0,1),0.5) );
		this->tblStone.clear();
		#if 0
			float step = 5;
			for ( float x = -RINGSIZE+step/2 ; x < RINGSIZE-1 ; x += step )
			{
				for ( float y = -RINGSIZE+step/2 ; y < RINGSIZE-1 ; y += step )
				{
					this->tblStone.emplace_back( Stone(vect2(x,y),0.5) );
				}
			}
		#endif
		this->valState = State::Title;
		this->timeState = 0.0;

		pers.flgUseGui = false;

	}

	//------------------------------------------------------------------------------
	void StateTitle( SysKeys& keys, SysMouse& mouse, SysSound& sound, SysGra& gra, Pers& pers )
	//------------------------------------------------------------------------------
	{
		// 画面クリア
//		gra.Clr(rgb(0.3,0.3,0.3));
//		pers.grid.DrawGrid3d( gra, pers, vect3(0,0,0), pImpl->mat, pImpl->RINGSIZE,pImpl->RINGSIZE, 1, rgb(0.2,0.2,0.2) );
//		gra.Print(1,(float)text_y++,"35 : Curing(Title)" ); 

		gra.Print(22,15," Curing Game" ); 

		
		gra.Print(20,20," Push Mouse.L Button " ); 
		
		if ( mouse.L.hi ) 
		{
			this->valState = State::Play;
			this->numRound=0;
			this->tblScore[0]=0;
			this->tblStone.clear();
		}

	}


	//------------------------------------------------------------------------------
	void StatePlay( SysKeys& keys, SysMouse& mouse, SysSound& sound, SysGra& gra, Pers& pers )
	//------------------------------------------------------------------------------
	{
//		// 画面クリア
//		pers.grid.DrawGrid3d( gra, pers, vect3(0,0,0), this->mat, this->RINGSIZE,this->RINGSIZE, 1, rgb(0.2,0.2,0.2) );



		float	timeDelay = 0;
		
		{
			int x = 10;
			int y = 2;

			if ( flgClear == false )
			{
				gra.Print(40,3,"Round :"+to_string(numRound+1) ); 
			}


			
			{
					int ox = 14;
					int oy = 2;
					int total = 0;

				for ( int i = 0 ; i < MaxRound ; i++ )
				{
					int x = ox;
					int y = oy;

					gra.Print(x+i*2,y++, to_string(i+1));
					gra.Print(x+i*2,y++, "--" );
					
				}

				for ( int i = 0 ; i < numRound ; i++ )
				{
					int x = ox;
					int y = oy+2;

					int score = tblScore[i];
					gra.Print(x+i*2,y++, to_string( score ) );
					total += score;
					
				}
				{
					int x = ox+MaxRound*2;;
					int y = oy;
					gra.Print(x,y++, "total");
					gra.Print(x,y++, "-----" );
					gra.Print(x,y++, to_string( total ) );
				}

				if ( flgClear == false )
				{
					int x = ox+numRound*2;
					int y = oy+3;
					gra.Print(x,y++, "^^" );
				}
			}
		}


		static vect2 posCursor;

		{
			vect3 P1 = pers.calcScreenToWorld3( vect3(mouse.pos,0) );
			vect3 I1 = pers.calcRayvect( P1 );

			auto[b,Q] = pers.grid.IntersectOn( P1, I1 );	// Grid空間座標を求める。

	//		pers.grid.Circle( gra, pers,Q, 0.5, 24, rgb(0,1,0) );
	//		pers.grid.Pset( gra, pers, Q, rgb(0,1,0), 12 );
	//		pers.grid.Print( gra, pers, Q, -26,-52, to_string(Q.x) + " " +to_string(Q.y)  ); 
	//			pers.grid.Line( gra, pers, vect2(1,1), vect2(-1,-1), rgb(0,1,1), 2 );


			if ( b ) posCursor = Q;

			if ( timeState > 1.0 )
			{

//				if ( mouse.L.lo ) this->tblStone.emplace_back( Q, 0.5, vect2(0,0.1) );
//				if ( mouse.F.lo ) this->tblStone.emplace_back( Q, 0.5, vect2(0,0.4) );
//				if ( mouse.B.lo ) this->tblStone.emplace_back( Q, 0.5, vect2(0,0.8) );
			}

		}



		static	bool	flgInMove = false;
		static	bool	flgHippari = false;
		static	vect2	posHippari;
		
		if ( mouse.L.hi && flgInMove == false && flgClear == false ) 
		{
			if ( flgHippari == false )
			{
				flgHippari = true;
			
				posHippari = posCursor;
			}
		}

		if ( mouse.L.on == false ) 
		{
			if ( flgHippari )
			{
				// ショット
				
				vect2 v = posHippari-posCursor ;
				
				if ( v.abs() > 0.1 )
				{

					this->tblStone.emplace_back( posCursor, 0.5, v*0.1 );
					sound.mml_play( "T1800O5V10c#cr");
				}
			
			}
			flgHippari = false;
		}
		
		if ( flgHippari )
		{
			pers.grid.Line( gra, pers, posHippari, posCursor, rgb(1,1,1), 2 );
		}

		// メッセージ
		if ( flgClear == true )
		{
			gra.Print(20,6,"   C L E A R !   " ); 
			gra.Print(20,7,"C O M P L E A T !" ); 

			gra.Print(15,25,"Click L-button to New Game" ); 
			

 			if ( mouse.L.hi )
 			{
	 			flgClear = false;
				this->valState = State::Title;
 			}
 
 
		}
		else
		if ( flgInMove == true )
		{
			gra.Print(15,25,"In Move.." ); 
		}
		else
		if ( flgHippari == false )
		{
			gra.Print(15,25,"Click L-button and drag mouse." ); 
		}
		else
		if ( flgHippari == true )
		{
			gra.Print(15,25,"Release L-button to shot" ); 
		}
		
		// チェック：動いてる途中かどうか
		{
			bool isMove = false;
			for ( auto& o : this->tblStone )
			{
				if ( o.vel.abs() > 0.01 ) isMove = true;
			}

			if ( flgInMove == true )
			{
				if ( isMove == false )
				{
					// 今止まった
						sound.mml_play( "T1800O5V10c#cr");


					// 点数計算
					int total_score = 0;
					for ( auto& o : this->tblStone )
					{
						vect2 posCenter(0,0);
						
						float len = (o.pos - posCenter ).abs(); 
						
						
						int score = 0;
						if ( len < 2.0 ) score = 3;
						else
						if ( len < 4.0 ) score = 2;
						else
						if ( len < 6.0 ) score = 1;
						else
						{
//						score += 1;
						sound.mml_play( "T1800O5V10e#e");
						}
						o.point = score;
						total_score += score;
						

					}
						
					tblScore[ numRound ] = total_score;

		
					// ラウンドアップ
					numRound++;
					if ( numRound >= MaxRound ) 
					{
						// クリア
						flgClear = true;

						if ( hiscore < total_score )
						{
							hiscore = total_score;
							sound.mml_play( "T100v6o4c1-gcfcfgdg+c7r:v9o3g1eg+ca+c+db+d+g7r:v9o3e1ceafabgb+e7r");
						
//							this->limWaitInput = 10;
						}
						

//						numRound=0;
					}
					
				}
			}
			flgInMove = isMove;

		}
	

		// 移動
		for ( auto& o : this->tblStone )
		{
			o.prev_pos = o.pos;
			o.pos += o.vel;
			o.vel *= 0.99;
		}

		// 衝突：壁
		for ( auto& b1 : this->tblStone )
		{
			if ( b1.pos.y + b1.radius > RINGSIZE )
			{
				b1.pos = b1.prev_pos;
				b1.vel.y = -b1.vel.y;
			}
			if ( b1.pos.y - b1.radius < -RINGSIZE )
			{
				b1.pos = b1.prev_pos;
				b1.vel.y = -b1.vel.y;
			}

			if ( b1.pos.x + b1.radius > RINGSIZE )
			{
				b1.pos = b1.prev_pos;
				b1.vel.x = -b1.vel.x;
			}
			if ( b1.pos.x - b1.radius < -RINGSIZE )
			{
				b1.pos = b1.prev_pos;
				b1.vel.x = -b1.vel.x;
			}
		//		pers.grid.Print( gra, pers, b1.pos, -26,-52, to_string(b1.pos.x) + " " +to_string(b1.pos.y)  ); 
		}
		
		// 衝突：ストーン同士
		for ( auto& b1 : this->tblStone )
		{
			for ( auto& b2 : this->tblStone )
			{
				if ( &b1 != &b2 )
				{
					vect2	v = (b2.pos-b1.pos);
					float	len = v.abs();
					float	bor = b1.radius + b2.radius;
					if ( len < bor )
					{
						b1.pos = b1.prev_pos;
						b2.pos = b2.prev_pos;

						v = v * (bor-len);
						vect2	N = v.normalize();
						vect2	vel1 = func_reflect( b1.vel, N, 1.0 );
						vect2	vel2 = func_reflect( b2.vel, N, 1.0 );

						vect2	v1 = dot(-vel2,N)*N;
						vect2	v2 = dot(-vel1,N)*N;

						b1.vel += v1 - v2;
						b2.vel += v2 - v1;

					}
				}
			}
		}

		// 減衰
		for ( auto& o : this->tblStone )
		{
			o.vel *= 0.999;
		}


		// 表示：カーソル
		if ( flgInMove == false )
		{
			pers.grid.Circle( gra, pers, posCursor, 0.5, 24, rgb(0,1,0),2 );
//			gra.Pset2d( mouse.pos, rgb(0,1,1), 4 );
		}
		{
			rgb		col = rgb(1,1,0);
			vect2 v0 = posCursor;
			vect2 v1 = posCursor + vect2(1.0,-0.5);
			vect2 v2 = posCursor + vect2(0.5,-1.0);
			pers.grid.Line( gra, pers, v0,v1, col, 1 );
			pers.grid.Line( gra, pers, v0,v2, col, 1 );
			pers.grid.Line( gra, pers, v2,v1, col, 1 );
		}

		// 表示：ステージ
		{
			pers.grid.Line( gra, pers, vect2(-RINGSIZE,-RINGSIZE), vect2(-RINGSIZE, RINGSIZE), rgb(1,1,1), 2 );
			pers.grid.Line( gra, pers, vect2( RINGSIZE,-RINGSIZE), vect2( RINGSIZE, RINGSIZE), rgb(1,1,1), 2 );
			pers.grid.Line( gra, pers, vect2(-RINGSIZE,-RINGSIZE), vect2( RINGSIZE,-RINGSIZE), rgb(1,1,1), 2 );
			pers.grid.Line( gra, pers, vect2(-RINGSIZE, RINGSIZE), vect2( RINGSIZE, RINGSIZE), rgb(1,1,1), 2 );

			pers.grid.Circle( gra, pers, vect2(0,0), 1, 24, rgb(1,1,0) );
			pers.grid.Circle( gra, pers, vect2(0,0), 3, 24, rgb(1,1,0) );
			pers.grid.Circle( gra, pers, vect2(0,0), 5, 24, rgb(1,1,0) );
		}

		// 表示：ストーン
		for ( auto o : this->tblStone )
		{
			pers.grid.Circle( gra, pers,o.pos, o.radius, 24, rgb(1,1,1) );
			if ( o.point > 0 )
			{
				pers.grid.Print( gra, pers, o.pos,-4,4, to_string(o.point), rgb(1,1,1) );
			}

		}
	}


};
Lab35::Lab35() : pImpl( new Lab35::Impl ){}

//------------------------------------------------------------------------------
void Lab35::Update( SysKeys& keys, SysMouse& mouse, SysSound& sound, SysGra& gra, Pers& pers, float delta, int& text_y, Cp& cp )
//------------------------------------------------------------------------------
{
		gra.Clr(rgb(0.3,0.3,0.3));
		gra.Print(0,(float)text_y++,"35:Curing"  ); 

		gra.Print(30,0,"HI SCORE " + to_string(pImpl->hiscore) ); 


	//初期化
//	if ( pImpl->bResetAll )
//	{
//		pImpl->bResetAll = false;
//	}

	Impl::State prev = pImpl->valState;

	switch( pImpl->valState )
	{
		case Impl::State::Reset:
			pImpl->StateReset( keys, mouse, sound, gra, pers );
		
			break;
		case Impl::State::Title:
			pImpl->StateTitle( keys, mouse, sound, gra, pers );
			break;

		case Impl::State::Play:
			pImpl->StatePlay( keys, mouse, sound, gra, pers );
			break;
		default:
			break;
	}

	if ( prev != pImpl->valState )
	{
		pImpl->timeState = 0;
	}
	else
	{
		pImpl->timeState += delta;
	}

/*
	vect3 vx = vect3(1,0,0);
	vect3 vy = vect3(0,1,0);
	vect3 vz = vect3(0,0,1);
	{
		vect3 p0 = vect3(-1,1,0);
		pers.prim.DrawArrow( gra, pers, text_y, p0, vx ,1	, rgb(1,0,0), "+x" );
		pers.prim.DrawArrow( gra, pers, text_y, p0, vy ,1	, rgb(0,1,0), "+y" );
		pers.prim.DrawArrow( gra, pers, text_y, p0, vz ,1	, rgb(0,0,1), "+z" );
	}

	{
		vect3 p0 = vect3(1,1,0);
		vect3 xy = cross(vx,vy);
		vect3 yz = cross(vy,vz);
		vect3 zx = cross(vz,vx);
		pers.prim.DrawArrow( gra, pers, text_y, p0, yz ,1	, rgb(1,0,0), "cross y*z" );
		pers.prim.DrawArrow( gra, pers, text_y, p0, zx ,1	, rgb(0,1,0), "cross z*x" );
		pers.prim.DrawArrow( gra, pers, text_y, p0, xy ,1	, rgb(0,0,1), "cross x*y" );

	}
*/
	if ( keys.R.hi )	pImpl->valState = Impl::State::Reset;
	if ( keys.Q.hi )	pImpl->mat *= mat33::mrotx(rad(-30));
	if ( keys.A.hi )	pImpl->mat *= mat33::mrotx(rad(30));


//	pers.prim.DrawCircle( gra, pers, vect3( 0,0, 0 ), mat33::mrotz(0.0), 1.0, rgb(1,1,1) );


	gra.Wait(); 
}
