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
	vect3 catmull3d_func( double t, const vect3 P0, const vect3 P1, const vect3 P2, const vect3 P3 )
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
	void bone_ReqAnimation( Data& data )
	//------------------------------------------------------------------------------
	{
		data.anim.bSelecting = false;
		data.anim.bForward = true;
		data.anim.bPlaying = true;
		data.anim.n = 0;
		data.anim.t = 0;
		data.anim.dt = 0.1;
	}	

	//------------------------------------------------------------------------------
	void bone_Play( Data& data )
	//------------------------------------------------------------------------------
	{
//		int num = data.anim.num;
		if(static_cast<signed>(data.animations[data.anim.num].pose.size())<4) return;

		int n0 = data.anim.n-1;
		int n1 = data.anim.n;
		int n2 = data.anim.n+1;
		int n3 = data.anim.n+2;
		if ( n0<0 ) n0 = 0;
		if ( n3>=static_cast<signed>(data.animations[data.anim.num].pose.size()) ) n3 =n2;

		for ( int j = 0 ; j < static_cast<signed>(data.animations[data.anim.num].pose[0].pos.size()) ; j++ )
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
			vect3 P0 = data.animations[data.anim.num].pose[ n0 ].pos[j];
			vect3 P1 = data.animations[data.anim.num].pose[ n1 ].pos[j];
			vect3 P2 = data.animations[data.anim.num].pose[ n2 ].pos[j];
			vect3 P3 = data.animations[data.anim.num].pose[ n3 ].pos[j];
			vect3 b = catmull3d_func(data.anim.t, P0,P1,P2,P3 );

			data.tblJoint[j].pos = b;

		}

		if ( data.anim.bForward ) data.anim.t+=data.anim.dt; else data.anim.t-=data.anim.dt;


		if ( data.anim.t >= 1.0 ) 
		{
			if ( data.anim.n+1 < static_cast<signed>(data.animations[data.anim.num].pose.size())-1 )
			{
				data.anim.t = 0;
				data.anim.n+=1;
			}
			else
			{
				data.anim.t = 1.0;
				data.anim.bForward = !data.anim.bForward;
			}
		}
		else
		if ( data.anim.t <= 0.0 ) 
		{
			if ( data.anim.n >= 1 )
			{
				data.anim.t = 1.0;
				data.anim.n-=1;
			}
			else
			{
				data.anim.t = 0.0;
				data.anim.bForward = !data.anim.bForward;
			}
		}
		if ( data.anim.bForward == false ) data.anim.bPlaying = false;
	}
	
	//------------------------------------------------------------------------------
	void bone_PrevKeyframe( Data& data )
	//------------------------------------------------------------------------------
	{
		data.anim.bPlaying = false;

//		int num = data.anim.num;
		if ( data.animations.size() ==0 ) return;
		if ( data.animations[data.anim.num].pose.size()==0) return;
		if ( data.animations[data.anim.num].pose[ 0 ].pos.size()==0 ) return;

		data.anim.pose--;
//		if ( data.anim.pose < 0 ) data.anim.pose = static_cast<signed>(data.animations[data.anim.num].pose.size())-1;
		if ( data.anim.pose < 0 ) data.anim.pose = 0;

		if ( data.anim.pose >= 0 )
		{
			// キーフレーム切り替え
			int i = 0;
			for ( Joint3& j : data.tblJoint )
			{
				j.pos = data.animations[data.anim.num].pose[ data.anim.pose ].pos[i];
				i++;
			}
		}
	}

	//------------------------------------------------------------------------------
	void bone_TopKeyframe( Data& data )
	//------------------------------------------------------------------------------
	{
		data.anim.bPlaying = false;

//		int num = data.anim.num;
		if ( data.animations.size() ==0 ) return;
		if ( data.animations[data.anim.num].pose.size()==0) return;
		if ( data.animations[data.anim.num].pose[ 0 ].pos.size()==0 ) return;

		data.anim.pose = 0;

		if ( data.anim.pose >= 0 )
		{
			// キーフレーム切り替え
			int i = 0;
			for ( Joint3& j : data.tblJoint )
			{
				j.pos = data.animations[data.anim.num].pose[ data.anim.pose ].pos[i];
				i++;
			}
		}
	}

	//------------------------------------------------------------------------------
	void bone_NextKeyframe( Data& data )
	//------------------------------------------------------------------------------
	{
		data.anim.bPlaying = false;

//		int num = data.anim.num;
		if ( data.animations.size() ==0 ) return;
		if ( data.animations[data.anim.num].pose.size()==0) return;
		if ( data.animations[data.anim.num].pose[ 0 ].pos.size()==0 ) return;

		data.anim.pose++; 
//		if ( data.anim.pose > static_cast<signed>(data.animations[num].pose.size())-1 ) data.anim.pose = 0;
		if ( data.anim.pose > static_cast<signed>(data.animations[data.anim.num].pose.size())-1 ) data.anim.pose = static_cast<signed>(data.animations[data.anim.num].pose.size())-1;

		if ( data.anim.pose >= 0 )
		{
			// キーフレーム切り替え
			int i = 0;
			for ( Joint3& j : data.tblJoint )
			{
				j.pos = data.animations[data.anim.num].pose[ data.anim.pose ].pos[i];
				i++;
			}
		}
	}

	//------------------------------------------------------------------------------
	void bone_LastKeyframe( Data& data )
	//------------------------------------------------------------------------------
	{
		data.anim.bPlaying = false;

//		int num = data.anim.num;
		if ( data.animations.size() ==0 ) return;
		if ( data.animations[data.anim.num].pose.size()==0) return;
		if ( data.animations[data.anim.num].pose[ 0 ].pos.size()==0 ) return;

		data.anim.pose = static_cast<signed>(data.animations[data.anim.num].pose.size())-1;

		if ( data.anim.pose >= 0 )
		{
			// キーフレーム切り替え
			int i = 0;
			for ( Joint3& j : data.tblJoint )
			{
				j.pos = data.animations[data.anim.num].pose[ data.anim.pose ].pos[i];
				i++;
			}
		}
	}

	//------------------------------------------------------------------------------
	void bone_RefrectKeyframe( Data& data )
	//------------------------------------------------------------------------------
	{
		data.anim.bPlaying = false;

//		int num = data.anim.num;
		if ( data.animations.size() ==0 ) return;
		if ( data.animations[data.anim.num].pose.size()==0) return;
		if ( data.animations[data.anim.num].pose[ 0 ].pos.size()==0 ) return;

		for ( int i = 0 ; i < static_cast<signed>(data.tblJoint.size()) ; i++ )
		{ 
			data.animations[data.anim.num].pose[ data.anim.pose ].pos[i] = data.tblJoint[i].pos;
		}
	}
	
	//------------------------------------------------------------------------------
	void bone_InsertKeyframe( Data& data )
	//------------------------------------------------------------------------------
	{
		data.anim.bPlaying = false;

//		int num = data.anim.num;

		data.animations[data.anim.num].pose.emplace( data.animations[data.anim.num].pose.begin() + data.anim.pose );
		for ( const Joint3& j : data.tblJoint )
		{
			data.animations[data.anim.num].pose[ data.anim.pose ].pos.emplace_back( j.pos );
		}
	}

	//------------------------------------------------------------------------------
	void bone_CopyKeyframe( Data& data )
	//------------------------------------------------------------------------------
	{

/*
	struct Data
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
		unique_ptr<Data> pNew(new Data);
		{
			for ( Joint3& j : data.tblJoint )
			{
				pNew->tblJoint.emplace_back( j.pos );
			}

			for ( Bone3& b : data.tblBone )
			{
				pNew->tblBone.emplace_back( pNew->tblJoint, b.n0, b.n1 );
			}
		}
		{
			int	cntAction = static_cast<signed>(data.animations.size());
			for ( int num = 0 ; num < cntAction ; num++ )
			{
				pNew->animations.emplace_back();

				int	cntPose = static_cast<signed>(data.animations[num].pose.size());
				for ( int pose = 0 ; pose < cntPose ; pose++ )
				{
					pNew->animations[num].pose.emplace_back();

					for ( int j = 0 ; j < static_cast<signed>(data.tblJoint.size()) ; j++ )
					{
						vect3 pos = data.animations[num].pose[pose].pos[j];
						pNew->animations[num].pose[pose].pos.emplace_back( pos );
					}
				}
			}
		}
		data.anim.pCopybuf = move(pNew);
		data.anim.copied_num = data.anim.num;
		data.anim.copied_pose = data.anim.pose;
	}

	//------------------------------------------------------------------------------
	void bone_PastKeyframe( Data& data )
	//------------------------------------------------------------------------------
	{
		data.anim.bPlaying = false;

		bone_InsertKeyframe( data );
		for ( int i = 0 ; i < static_cast<signed>(data.tblJoint.size()) ; i++ )
		{ 
//			vect3 v = data.animations[data.anim.copied_num].pose[ data.anim.copied_pose ].pos[i];
			vect3 v = data.anim.pCopybuf->animations[data.anim.copied_num].pose[ data.anim.copied_pose ].pos[i];
		
			data.animations[data.anim.num].pose[ data.anim.pose ].pos[i] = v;
			data.tblJoint[i].pos = v;
		}

	}

	//------------------------------------------------------------------------------
	void bone_CutKeyframe( Data& data )
	//------------------------------------------------------------------------------
	{
		data.anim.bPlaying = false;

		if ( data.animations.size() ==0 ) return;
		if ( data.animations[data.anim.num].pose.size()==0 ) return;
		if ( data.animations[data.anim.num].pose[ 0 ].pos.size()==0 ) return;

		bone_CopyKeyframe( data );
	
		data.animations[data.anim.num].pose.erase(data.animations[data.anim.num].pose.begin() +data.anim.pose );	

		if ( data.anim.pose > (signed)data.animations[data.anim.num].pose.size()-1 ) data.anim.pose = (signed)data.animations[data.anim.num].pose.size()-1;

		if ( data.animations.size() ==0 ) return;
		if ( data.animations[data.anim.num].pose.size() ==0 ) return;

		{
//			int num = data.anim.num;
//			data.anim.pose = 0;
			{
				// キーフレーム切り替え
				int i = 0;
				for ( Joint3& j : data.tblJoint )
				{
					j.pos = data.animations[data.anim.num].pose[ data.anim.pose ].pos[i];
					i++;
				}
			}
		}

	}

	//------------------------------------------------------------------------------
	void bone_PrevAnimation( Data& data )
	//------------------------------------------------------------------------------
	{
		data.anim.bPlaying = false;

		data.anim.num--;
		if ( data.anim.num < 0 ) data.anim.num = 0;

//		int num = data.anim.num;
		if ( data.animations.size() ==0 ) return;
		if ( data.animations[data.anim.num].pose.size()==0) return;
		if ( data.animations[data.anim.num].pose[ 0 ].pos.size()==0 ) return;

		{
//			int num = data.anim.num;
			data.anim.pose = 0;
			{
				// キーフレーム切り替え
				int i = 0;
				for ( Joint3& j : data.tblJoint )
				{
					j.pos = data.animations[data.anim.num].pose[ data.anim.pose ].pos[i];
					i++;
				}
			}
		}
	}

	//------------------------------------------------------------------------------
	void bone_NextAnimation( Data& data )
	//------------------------------------------------------------------------------
	{
		data.anim.bPlaying = false;

		data.anim.num++; 
		if ( data.anim.num > static_cast<signed>(data.animations.size())-1 ) data.anim.num = static_cast<signed>(data.animations.size())-1;

//		int num = data.anim.num;
		if ( data.animations.size() ==0 ) return;
		if ( data.animations[data.anim.num].pose.size()==0) return;
		if ( data.animations[data.anim.num].pose[ 0 ].pos.size()==0 ) return;

		{
			data.anim.pose = 0;

			{
				// キーフレーム切り替え
				int i = 0;
				for ( Joint3& j : data.tblJoint )
				{
				
					j.pos = data.animations[data.anim.num].pose[ data.anim.pose ].pos[i];
					i++;
				}
			}
		}
	}

	//------------------------------------------------------------------------------
	void bone_AddAnimation( Data& data )
	//------------------------------------------------------------------------------
	{
		data.anim.bPlaying = false;

		data.anim.num = static_cast<signed>(data.animations.size());
		data.anim.pose = 0;
		data.animations.emplace_back();

		{
//			int num = data.anim.num;

			data.animations[data.anim.num].pose.emplace_back();
			for ( const Joint3& j : data.tblJoint )
			{
				data.animations[data.anim.num].pose[ data.anim.pose ].pos.emplace_back( j.pos );
			}
		}
	}

	//------------------------------------------------------------------------------
	void bone_update( Data& data, Pers& pers, mat44& cam_mat, SysGra& gra )
	//------------------------------------------------------------------------------
	{
		for ( int i = 0 ; i < 5 ; i++ )
		{
			// 骨コリジョン 張力計算
			for ( Bone3 b : data.tblBone )
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
			for ( Joint3& a : data.tblJoint )
			{
				a.pos += a.tension;
				a.tension=0;
			}
		}

		// Human pers
		for ( Joint3& j : data.tblJoint )
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
		for ( Bone3 b : data.tblBone )
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
	void bone_drawMotion(  Data& data, Pers& pers, mat44& cam_mat, SysGra& gra )
	//------------------------------------------------------------------------------
	{
//				int num = anim.num;
//		Data& data = (*pData);
		// マーカースプライン変換表示
		if ( static_cast<signed>(data.animations.size()) > 0 )
		{
			double dt = data.anim.dt;
			double div = 1/dt;

			for ( int n = -1 ; n < static_cast<signed>(data.animations[data.anim.num].pose.size())-3+1 ; n++ )
			{
				int n0 = n;
				int n1 = n+1;
				int n2 = n+2;
				int n3 = n+3;
				if ( n0 < 0 ) n0 = 0;
				if ( n3 >= static_cast<signed>(data.animations[data.anim.num].pose.size()) ) n3 = n2;
			
				for ( int j = 0 ;  j < static_cast<signed>(data.tblJoint.size()) ; j++ )
				{
					if ( data.tblJoint[j].bSelected == false ) continue;
				
					vect3 P0 = data.animations[data.anim.num].pose[ n0 ].pos[j];
					vect3 P1 = data.animations[data.anim.num].pose[ n1 ].pos[j];
					vect3 P2 = data.animations[data.anim.num].pose[ n2 ].pos[j];
					vect3 P3 = data.animations[data.anim.num].pose[ n3 ].pos[j];

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
	void bone_save( Data& data, const char* filename )
	//------------------------------------------------------------------------------
	{
		data.anim.bPlaying = false;

		fstream fo( filename, ios::out );

		{
			fo << "joint" << endl;
			for ( Joint3& j : data.tblJoint )	// 関節の位置
			{
				fo << "\t"<< j.pos.x << "\t" << j.pos.y << "\t" << j.pos.z << endl;
			}

			fo << "bone" << endl;
			for ( Bone3& b : data.tblBone )	// 骨
			{
				b.length = (b.j1.pos - b.j0.pos).length();
				fo  << "\t"<< b.n0 << "\t" << b.n1 << endl;
			}
		}
		{
			int	cntAction = static_cast<signed>(data.animations.size());
			for ( int num = 0 ; num < cntAction ; num++ )
			{
				fo << "motion" << endl;
				int	cntPose = static_cast<signed>(data.animations[num].pose.size());
				for ( int pose = 0 ; pose < cntPose ; pose++ )
				{
					for ( int j = 0 ; j < static_cast<signed>(data.tblJoint.size()) ; j++ )
					{
						fo  << "\t"<< data.animations[num].pose[ pose ].pos[ j ].x << "\t" << data.animations[num].pose[ pose ].pos[ j ].y << "\t" << data.animations[num].pose[ pose ].pos[ j ].z << endl;
					}
					if( pose+1 < cntPose ) fo << "," << endl;
				}
			}
		}
		fo << "end" << endl;

		cout << "SAVED" << endl;
	}
	//------------------------------------------------------------------------------
	void bone_load( Data& data, const char* filename )
	//------------------------------------------------------------------------------
	{
		data.anim.bPlaying = false;


	//	Data*	pNew = new Data;

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
				num = static_cast<signed>(data.animations.size());
				data.animations.emplace_back();
				data.animations[num].pose.emplace_back();
				continue;
			}
			if ( string(buf) == "," ) 
			{
				data.animations[num].pose.emplace_back();
				continue;
			}
			if ( string(buf) == "end" )	
			{	
				for ( Bone3& b : data.tblBone )	// 関節の距離を決定する。
				{
					b.length = (b.j1.pos - b.j0.pos).length();
				}
				{
					int cnt = 0 ;
					for ( Joint3& j : data.tblJoint )
					{
						j.id = cnt++;				//id登録
					}
				}
				for ( Bone3& b : data.tblBone )	// ジョイントに関節の距離を決定する。
				{
					b.j1.relative.emplace_back( b.j0 ); 
					b.j0.relative.emplace_back( b.j1 ); 
				}
				data.anim.pose = 0;
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
						data.tblJoint.emplace_back( vect3(x,y,z) );
						//	cout << x << "," << y << "," << z << endl; 
					}
					break;
				case ModeBone:
					{
						vector<string> v = split( buf, '\t');
						int n0 = stoi(v[0]);
						int n1 = stoi(v[1]);
						data.tblBone.emplace_back( data.tblJoint, n0, n1 );
						//	cout << x << "," << y << "," << z << endl; 
					}
					break;
				case ModeMotion:
					{
						vector<string> v = split( buf, '\t');
						double x = stod(v[0]);
						double y = stod(v[1]);
						double z = stod(v[2]);
						data.animations[num].pose[ data.animations[num].pose.size()-1 ].pos.emplace_back( x,y,z );
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
