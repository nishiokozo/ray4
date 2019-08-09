#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <cmath>
#include <functional>
#include <fstream>
#include <sstream>
using namespace std;

#include "geom.h"
#include "SysKeys.h"
#include "SysMouse.h"
#include "SysGra.h"
//#include "Sys.h"

#include "obj.h"
#include "bone.h"

	// カトマル曲線3D
	//------------------------------------------------------------------------------
static	vect3 catmull3d_func( double t, const vect3 P0, const vect3 P1, const vect3 P2, const vect3 P3 )
	//------------------------------------------------------------------------------
	{
		//catmull-Rom 曲線
		// P(t)=P1*(2t^3-3t^2+1)+m0*(t^3-2t^2+t)+P2*(-2t^3+3t^2)+m1*(t^3-t^2)
		// m0=(P2-P0)/2
		// m1=(P3-P1)/2

		vect3 m0 = (P2-P0)/2.0;
		vect3 m1 = (P3-P1)/2.0;
		vect3 P =  P1*(  2*t*t*t - 3*t*t +1) + m0*( t*t*t -2*t*t +t )
				 + P2*( -2*t*t*t + 3*t*t   ) + m1*( t*t*t - t*t );

		return P;
	};


	//------------------------------------------------------------------------------
	void Bone::ReqAnimation()
	//------------------------------------------------------------------------------
	{
		anim.bSelecting = false;
		anim.bForward = true;
		anim.bPlaying = true;
		anim.n = 0;
		anim.t = 0;
		anim.dt = 0.1;
	}	

	//------------------------------------------------------------------------------
	void Bone::PlayAnimation()
	//------------------------------------------------------------------------------
	{
		if ( animations.size() ==0 ) return;
		if ( animations[anim.num].pose.size()==0) return;
		if ( animations[anim.num].pose[ 0 ].pos.size()==0 ) return;

		if(static_cast<signed>(animations[anim.num].pose.size())<2) return;

		int n0 = anim.n-1;
		int n1 = anim.n;
		int n2 = anim.n+1;
		int n3 = anim.n+2;
		if ( n0<0 ) n0 = 0;
		if ( n3>=static_cast<signed>(animations[anim.num].pose.size()) ) n3 =n2;

		for ( int j = 0 ; j < static_cast<signed>(animations[anim.num].pose[0].pos.size()) ; j++ )
		{
/*
			if ( g_flg ) 
			{
				if ( j == 3 || j == 4 ) continue;	//肩
				if ( j == 5 || j == 2 ) continue;	//腹首
				if ( j == 6 || j == 7) continue;	//腰
	//			if ( j == 12 || j == 14 ) continue;	//肘
	//			if ( j == 8 || j == 10 ) continue;	//足
	//			if ( j == 9 || j == 11 ) continue;	//膝
			}
*/
			vect3 P0 = animations[anim.num].pose[ n0 ].pos[j];
			vect3 P1 = animations[anim.num].pose[ n1 ].pos[j];
			vect3 P2 = animations[anim.num].pose[ n2 ].pos[j];
			vect3 P3 = animations[anim.num].pose[ n3 ].pos[j];
			vect3 b = catmull3d_func(anim.t, P0,P1,P2,P3 );

			tblJoint[j].pos = b;

		}

		if ( anim.bForward ) anim.t+=anim.dt; else anim.t-=anim.dt;


		if ( anim.t >= 1.0 ) 
		{
			if ( anim.n+1 < static_cast<signed>(animations[anim.num].pose.size())-1 )
			{
				anim.t = 0;
				anim.n+=1;
			}
			else
			{
				anim.t = 1.0;
				anim.bForward = !anim.bForward;
			}
		}
		else
		if ( anim.t <= 0.0 ) 
		{
			if ( anim.n >= 1 )
			{
				anim.t = 1.0;
				anim.n-=1;
			}
			else
			{
				anim.t = 0.0;
				anim.bForward = !anim.bForward;
			}
		}
		if ( anim.bForward == false ) anim.bPlaying = false;
	}
	
	//------------------------------------------------------------------------------
	void Bone::PrevKeyframe()
	//------------------------------------------------------------------------------
	{
		anim.bPlaying = false;

		if ( animations.size() ==0 ) return;
		if ( animations[anim.num].pose.size()==0) return;
		if ( animations[anim.num].pose[ 0 ].pos.size()==0 ) return;

		anim.pose--;
//		if ( anim.pose < 0 ) anim.pose = static_cast<signed>(animations[anim.num].pose.size())-1;
		if ( anim.pose < 0 ) anim.pose = 0;

		if ( anim.pose >= 0 )
		{
			// キーフレーム切り替え
			int i = 0;
			for ( Joint3& j : tblJoint )
			{
				j.pos = animations[anim.num].pose[ anim.pose ].pos[i];
				i++;
			}
		}
	}

	//------------------------------------------------------------------------------
	void Bone::TopKeyframe()
	//------------------------------------------------------------------------------
	{
		anim.bPlaying = false;

		if ( animations.size() ==0 ) return;
		if ( animations[anim.num].pose.size()==0) return;
		if ( animations[anim.num].pose[ 0 ].pos.size()==0 ) return;

		anim.pose = 0;

		if ( anim.pose >= 0 )
		{
			// キーフレーム切り替え
			int i = 0;
			for ( Joint3& j : tblJoint )
			{
				j.pos = animations[anim.num].pose[ anim.pose ].pos[i];
				i++;
			}
		}
	}

	//------------------------------------------------------------------------------
	void Bone::NextKeyframe()
	//------------------------------------------------------------------------------
	{
		anim.bPlaying = false;

		if ( animations.size() ==0 ) return;
		if ( animations[anim.num].pose.size()==0) return;
		if ( animations[anim.num].pose[ 0 ].pos.size()==0 ) return;

		anim.pose++; 
//		if ( anim.pose > static_cast<signed>(animations[num].pose.size())-1 ) anim.pose = 0;
		if ( anim.pose > static_cast<signed>(animations[anim.num].pose.size())-1 ) anim.pose = static_cast<signed>(animations[anim.num].pose.size())-1;

		if ( anim.pose >= 0 )
		{
			// キーフレーム切り替え
			int i = 0;
			for ( Joint3& j : tblJoint )
			{
				j.pos = animations[anim.num].pose[ anim.pose ].pos[i];
				i++;
			}
		}
	}

	//------------------------------------------------------------------------------
	void Bone::LastKeyframe()
	//------------------------------------------------------------------------------
	{
		anim.bPlaying = false;

		if ( animations.size() ==0 ) return;
		if ( animations[anim.num].pose.size()==0) return;
		if ( animations[anim.num].pose[ 0 ].pos.size()==0 ) return;

		anim.pose = static_cast<signed>(animations[anim.num].pose.size())-1;

		if ( anim.pose >= 0 )
		{
			// キーフレーム切り替え
			int i = 0;
			for ( Joint3& j : tblJoint )
			{
				j.pos = animations[anim.num].pose[ anim.pose ].pos[i];
				i++;
			}
		}
	}

	//------------------------------------------------------------------------------
	void Bone::RefrectKeyframe()
	//------------------------------------------------------------------------------
	{
		anim.bPlaying = false;

		if ( animations.size() ==0 ) return;
		if ( animations[anim.num].pose.size()==0) return;
		if ( animations[anim.num].pose[ 0 ].pos.size()==0 ) return;

		for ( int i = 0 ; i < static_cast<signed>(tblJoint.size()) ; i++ )
		{ 
			animations[anim.num].pose[ anim.pose ].pos[i] = tblJoint[i].pos;
		}
	}
	
	//------------------------------------------------------------------------------
	void Bone::InsertKeyframe()
	//------------------------------------------------------------------------------
	{
		anim.bPlaying = false;


		animations[anim.num].pose.emplace( animations[anim.num].pose.begin() + anim.pose );
		for ( const Joint3& j : tblJoint )
		{
			animations[anim.num].pose[ anim.pose ].pos.emplace_back( j.pos );
		}
	}

	//------------------------------------------------------------------------------
	void Bone::CopyKeyframe()
	//------------------------------------------------------------------------------
	{

/*
	struct Bone
	{
		vector<Joint3>				tblJoint;
		vector<Bone3>				tblBone;

		struct Pos
		{
			vector<vect3>		pos;
		};
		struct Keyframe
		{
			vector<Pos>			pose;
		};
		vector<Keyframe>		animations;
	};
*/
		unique_ptr<Bone> pNew(new Bone);
		{
			for ( Joint3& j : tblJoint )
			{
				pNew->tblJoint.emplace_back( j.pos );
			}

			for ( Bone3& b : tblBone )
			{
				pNew->tblBone.emplace_back( pNew->tblJoint, b.n0, b.n1 );
			}
		}
		{
			int	cntAction = static_cast<signed>(animations.size());
			for ( int num = 0 ; num < cntAction ; num++ )
			{
				pNew->animations.emplace_back();

				int	cntPose = static_cast<signed>(animations[num].pose.size());
				for ( int pose = 0 ; pose < cntPose ; pose++ )
				{
					pNew->animations[num].pose.emplace_back();

					for ( int j = 0 ; j < static_cast<signed>(tblJoint.size()) ; j++ )
					{
						vect3 pos = animations[num].pose[pose].pos[j];
						pNew->animations[num].pose[pose].pos.emplace_back( pos );
					}
				}
			}
		}
		anim.pCopybuf = move(pNew);
		anim.copied_num = anim.num;
		anim.copied_pose = anim.pose;
	}

	//------------------------------------------------------------------------------
	void Bone::PastKeyframe()
	//------------------------------------------------------------------------------
	{
		anim.bPlaying = false;

		InsertKeyframe();
		for ( int i = 0 ; i < static_cast<signed>(tblJoint.size()) ; i++ )
		{ 
//			vect3 v = animations[anim.copied_num].pose[ anim.copied_pose ].pos[i];
			vect3 v = anim.pCopybuf->animations[anim.copied_num].pose[ anim.copied_pose ].pos[i];
		
			animations[anim.num].pose[ anim.pose ].pos[i] = v;
			tblJoint[i].pos = v;
		}

	}

	//------------------------------------------------------------------------------
	void Bone::CutKeyframe()
	//------------------------------------------------------------------------------
	{
		anim.bPlaying = false;

		if ( animations.size() ==0 ) return;
		if ( animations[anim.num].pose.size()==0 ) return;
		if ( animations[anim.num].pose[ 0 ].pos.size()==0 ) return;

		CopyKeyframe();
	
		animations[anim.num].pose.erase(animations[anim.num].pose.begin() +anim.pose );	

		if ( anim.pose > (signed)animations[anim.num].pose.size()-1 ) anim.pose = (signed)animations[anim.num].pose.size()-1;

		if ( animations.size() ==0 ) return;
		if ( animations[anim.num].pose.size() ==0 ) return;

		{
//			anim.pose = 0;
			{
				// キーフレーム切り替え
				int i = 0;
				for ( Joint3& j : tblJoint )
				{
					j.pos = animations[anim.num].pose[ anim.pose ].pos[i];
					i++;
				}
			}
		}

	}

	//------------------------------------------------------------------------------
	void Bone::PrevAnimation()
	//------------------------------------------------------------------------------
	{
		anim.bPlaying = false;

		anim.num--;
		if ( anim.num < 0 ) anim.num = 0;

		if ( animations.size() ==0 ) return;
		if ( animations[anim.num].pose.size()==0) return;
		if ( animations[anim.num].pose[ 0 ].pos.size()==0 ) return;

		{
			anim.pose = 0;
			{
				// キーフレーム切り替え
				int i = 0;
				for ( Joint3& j : tblJoint )
				{
					j.pos = animations[anim.num].pose[ anim.pose ].pos[i];
					i++;
				}
			}
		}
	}

	//------------------------------------------------------------------------------
	void Bone::NextAnimation()
	//------------------------------------------------------------------------------
	{
		anim.bPlaying = false;

		anim.num++; 
		if ( anim.num > static_cast<signed>(animations.size())-1 ) anim.num = static_cast<signed>(animations.size())-1;

		if ( animations.size() ==0 ) return;
		if ( animations[anim.num].pose.size()==0) return;
		if ( animations[anim.num].pose[ 0 ].pos.size()==0 ) return;

		{
			anim.pose = 0;

			{
				// キーフレーム切り替え
				int i = 0;
				for ( Joint3& j : tblJoint )
				{
				
					j.pos = animations[anim.num].pose[ anim.pose ].pos[i];
					i++;
				}
			}
		}
	}

	//------------------------------------------------------------------------------
	void Bone::AddAnimation()
	//------------------------------------------------------------------------------
	{
		anim.bPlaying = false;

		anim.num = static_cast<signed>(animations.size());
		anim.pose = 0;
		animations.emplace_back();

		{

			animations[anim.num].pose.emplace_back();
			for ( const Joint3& j : tblJoint )
			{
				animations[anim.num].pose[ anim.pose ].pos.emplace_back( j.pos );
			}
		}
	}

	//------------------------------------------------------------------------------
	void Bone::update()
	//------------------------------------------------------------------------------
	{
		for ( int i = 0 ; i < 5 ; i++ )
		{
			// 骨コリジョン 張力計算
			for ( Bone3 b : tblBone )
			{
				vect3 v = b.j1.pos - b.j0.pos;
				double l = v.length() - b.length;
				vect3 va  =	v.normalize()*l;

				double w0 = 0;
				double w1 = 0;
#if 0
				if ( b.j0.priority ==1 ) 
				{
					w0 = 0.0;
					w1 = 1.0;
				}
				else
				if ( b.j1.priority ==1 ) 
				{
					w0 = 1.0;
					w1 = 0.0;
				}
				else
#endif
				{
					w0 = 0.33;
					w1 = 0.33;
				}
				b.j0.tension += va*w0;
				b.j1.tension -= va*w1;

			}

			// 張力解消
			for ( Joint3& a : tblJoint )
			{
				a.pos += a.tension;
				a.tension=0;
			}
		}
	}

	//------------------------------------------------------------------------------
	void Bone::draw( Pers& pers, mat44& cam_mat, SysGra& gra )
	//------------------------------------------------------------------------------
	{
		// Human pers
		for ( Joint3& j : tblJoint )
		{
			//	右手系座標系
			//	右手ねじ周り
			//	roll	:z	奥+
			//	pitch	:x	右+
			//	yaw		:y	下+
			vect3 v= j.pos;

			v = v * cam_mat.invers();

			j.world = v;

			j.disp = pers.calcPoint(v);
		}

		// Human 描画
		for ( Bone3 b : tblBone )
		{
			if ( b.j0.disp.z > 0 && b.j0.disp.z > 0 )
			{
				vect2 v0(b.j0.disp.x,b.j0.disp.y);
				vect2 v1(b.j1.disp.x,b.j1.disp.y);
				gra.Line( v0,v1, rgb(1,1,1));
			}
		}
	}

	//------------------------------------------------------------------------------
	void Bone::drawMotion( Pers& pers, mat44& cam_mat, SysGra& gra )
	//------------------------------------------------------------------------------
	{
		// マーカースプライン変換表示
		if ( static_cast<signed>(animations.size()) > 0 )
		{
			double dt = anim.dt;
			double div = 1/dt;

			for ( int n = -1 ; n < static_cast<signed>(animations[anim.num].pose.size())-3+1 ; n++ )
			{
				int n0 = n;
				int n1 = n+1;
				int n2 = n+2;
				int n3 = n+3;
				if ( n0 < 0 ) n0 = 0;
				if ( n3 >= static_cast<signed>(animations[anim.num].pose.size()) ) n3 = n2;
			
				for ( int j = 0 ;  j < static_cast<signed>(tblJoint.size()) ; j++ )
				{
					if ( tblJoint[j].bSelected == false ) continue;
				
					vect3 P0 = animations[anim.num].pose[ n0 ].pos[j];
					vect3 P1 = animations[anim.num].pose[ n1 ].pos[j];
					vect3 P2 = animations[anim.num].pose[ n2 ].pos[j];
					vect3 P3 = animations[anim.num].pose[ n3 ].pos[j];

					double t = dt;
					vect3 a = catmull3d_func(0, P0,P1,P2,P3 );
					for ( int i = 0 ; i <div ; i++ )
					{
						vect3 b = catmull3d_func(t, P0,P1,P2,P3 );

						vect3 v0;
						vect3 v1;
						bool flg = pers.ScissorLine( a* cam_mat.invers(), b* cam_mat.invers(), v0, v1 );

						if ( flg )
						{
							gra.Line( vect2(v0.x,v0.y), vect2(v1.x,v1.y), rgb(0,0,1));
						}
						
						if ( v1.z > 0 ) gra.Fill(vect2(v1.x,v1.y)-2, vect2(v1.x,v1.y)+3,rgb(0,0,1));

						a=b;
						t+=dt;

					}	
				}
			}
		}
	}

	//------------------------------------------------------------------------------
	void Bone::saveMotion( const char* filename )
	//------------------------------------------------------------------------------
	{
		anim.bPlaying = false;

		fstream fo( filename, ios::out );

		{
			fo << "joint" << endl;
			for ( Joint3& j : tblJoint )	// 関節の位置
			{
				fo << "\t"<< j.pos.x << "\t" << j.pos.y << "\t" << j.pos.z << endl;
			}

			fo << "bone" << endl;
			for ( Bone3& b : tblBone )	// 骨
			{
				b.length = (b.j1.pos - b.j0.pos).length();
				fo  << "\t"<< b.n0 << "\t" << b.n1 << endl;
			}
		}
		{
			int	cntAction = static_cast<signed>(animations.size());
			for ( int num = 0 ; num < cntAction ; num++ )
			{
				fo << "motion" << endl;
				int	cntPose = static_cast<signed>(animations[num].pose.size());
				for ( int pose = 0 ; pose < cntPose ; pose++ )
				{
					for ( int j = 0 ; j < static_cast<signed>(tblJoint.size()) ; j++ )
					{
						fo  << "\t"<< animations[num].pose[ pose ].pos[ j ].x << "\t" << animations[num].pose[ pose ].pos[ j ].y << "\t" << animations[num].pose[ pose ].pos[ j ].z << endl;
					}
					if( pose+1 < cntPose ) fo << "," << endl;
				}
			}
		}
		fo << "end" << endl;

		cout << "SAVED" << endl;
	}
	//------------------------------------------------------------------------------
	void Bone::loadMotion( const char* filename )
	//------------------------------------------------------------------------------
	{
		anim.bPlaying = false;


	//	Bone*	pNew = new Bone;

		fstream fi( filename, ios::in );
		string buf;
		enum
		{
			ModeNone,
			ModeJoint,
			ModeBone,
			ModeMotion,
		} mode = ModeNone;
		
		function<vector<string>(const string&, char)> split = [] (const string &s, char delim) 
		{
			vector<string> elems;
			stringstream ss(s);
			string item;
			while (getline(ss, item, delim)) 
			{
				if (!item.empty()) 
				{
					elems.push_back(item);
				}
			}
			return elems;
		};

		int num = 0;

		while ( getline( fi, buf ) )
		{
	 		if( *(buf.end()-1) == 0x0d ) buf.pop_back();//CRLF対応

			cout << buf << endl;
			if ( string(buf) == "joint" )	{mode = ModeJoint;	continue;}
			if ( string(buf) == "bone" )	{mode = ModeBone;	continue;}
			if ( string(buf) == "motion" )	
			{
				mode = ModeMotion;
				num = static_cast<signed>(animations.size());
				animations.emplace_back();
				animations[num].pose.emplace_back();
				continue;
			}
			if ( string(buf) == "," ) 
			{
				animations[num].pose.emplace_back();
				continue;
			}
			if ( string(buf) == "end" )	
			{	
				for ( Bone3& b : tblBone )	// 関節の距離を決定する。
				{
					b.length = (b.j1.pos - b.j0.pos).length();
				}
				{
					int cnt = 0 ;
					for ( Joint3& j : tblJoint )
					{
						j.id = cnt++;				//id登録
					}
				}
				for ( Bone3& b : tblBone )	// ジョイントに関節の距離を決定する。
				{
					b.j1.relative.emplace_back( b.j0 ); 
					b.j0.relative.emplace_back( b.j1 ); 
				}
				anim.pose = 0;
				break;
			}
			switch( mode )
			{
				case ModeJoint:
					{
						vector<string> v = split( buf, '\t');
						double x = stod(v[0]);
						double y = stod(v[1]);
						double z = stod(v[2]);
						tblJoint.emplace_back( vect3(x,y,z) );
						//	cout << x << "," << y << "," << z << endl; 
					}
					break;
				case ModeBone:
					{
						vector<string> v = split( buf, '\t');
						int n0 = stoi(v[0]);
						int n1 = stoi(v[1]);
						tblBone.emplace_back( tblJoint, n0, n1 );
						//	cout << x << "," << y << "," << z << endl; 
					}
					break;
				case ModeMotion:
					{
						vector<string> v = split( buf, '\t');
						double x = stod(v[0]);
						double y = stod(v[1]);
						double z = stod(v[2]);
						animations[num].pose[ animations[num].pose.size()-1 ].pos.emplace_back( x,y,z );
						//	cout << x << "," << y << "," << z << endl; 
					}
					break;
				default:
					break;
			}

		}
		cout << "LOADED" << endl;
	//	return pNew;
	}
