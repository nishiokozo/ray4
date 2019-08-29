#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <cmath>
#include <functional>
#include <fstream>
#include <sstream>
#include <algorithm>
using namespace std;

#include "geom.h"
//#include "SysKeys.h"
//#include "SysMouse.h"
#include "SysGra.h"
//#include "Sys.h"

#include "obj.h"
#include "skeleton.h"

// カトマル曲線3D
//------------------------------------------------------------------------------
static	vect3 catmull3d_func( float t, const vect3 P0, const vect3 P1, const vect3 P2, const vect3 P3 )
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
void Skeleton::ReqAnimation()
//------------------------------------------------------------------------------
{
	cur.bSelecting = false;
	anim.bForward = true;
	anim.bPlaying = true;
	anim.n = 0;
	anim.t = 0;
	anim.dt = 0.1;
}	

//------------------------------------------------------------------------------
void Skeleton::PlayAnimation()
//------------------------------------------------------------------------------
{
	if ( animations.size() ==0 ) return;
	if ( animations[cur.act].pose.size()==0) return;
	if ( animations[cur.act].pose[ 0 ].joint.size()==0 ) return;

	if(static_cast<signed>(animations[cur.act].pose.size())<2) return;

	int n0 = anim.n-1;
	int n1 = anim.n;
	int n2 = anim.n+1;
	int n3 = anim.n+2;
	if ( n0<0 ) n0 = 0;
	if ( n3>=static_cast<signed>(animations[cur.act].pose.size()) ) n3 =n2;

	for ( int j = 0 ; j < static_cast<signed>(animations[cur.act].pose[0].joint.size()) ; j++ )
	{

		vect3 P0 = animations[cur.act].pose[ n0 ].joint[ j ].pos;
		vect3 P1 = animations[cur.act].pose[ n1 ].joint[ j ].pos;
		vect3 P2 = animations[cur.act].pose[ n2 ].joint[ j ].pos;
		vect3 P3 = animations[cur.act].pose[ n3 ].joint[ j ].pos;
		vect3 b = catmull3d_func(anim.t, P0,P1,P2,P3 );

		tblJoint[ j ].pos = b;

	}

	if ( anim.bForward ) anim.t+=anim.dt; else anim.t-=anim.dt;


	if ( anim.t >= 1.0 ) 
	{
		if ( anim.n+1 < static_cast<signed>(animations[cur.act].pose.size())-1 )
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
void Skeleton::PrevKeyframe()
//------------------------------------------------------------------------------
{
	anim.bPlaying = false;

	if ( animations.size() ==0 ) return;
	if ( animations[cur.act].pose.size()==0) return;
	if ( animations[cur.act].pose[ 0 ].joint.size()==0 ) return;

	cur.pose--;
//		if ( cur.pose < 0 ) cur.pose = static_cast<signed>(animations[cur.act].pose.size())-1;
	if ( cur.pose < 0 ) cur.pose = 0;

	if ( cur.pose >= 0 )
	{
		// キーフレーム切り替え
		int i = 0;
		for ( Joint& j : tblJoint )
		{
			j.pos = animations[cur.act].pose[ cur.pose ].joint[ i ].pos;
			i++;
		}
	}
}

//------------------------------------------------------------------------------
void Skeleton::TopKeyframe()
//------------------------------------------------------------------------------
{
	anim.bPlaying = false;

	if ( animations.size() ==0 ) return;
	if ( animations[cur.act].pose.size()==0) return;
	if ( animations[cur.act].pose[ 0 ].joint.size()==0 ) return;

	cur.pose = 0;

	if ( cur.pose >= 0 )
	{
		// キーフレーム切り替え
		int i = 0;
		for ( Joint& j : tblJoint )
		{
			j.pos = animations[cur.act].pose[ cur.pose ].joint[ i ].pos;
			i++;
		}
	}
}

//------------------------------------------------------------------------------
void Skeleton::NextKeyframe()
//------------------------------------------------------------------------------
{
	anim.bPlaying = false;

	if ( animations.size() ==0 ) return;
	if ( animations[cur.act].pose.size()==0) return;
	if ( animations[cur.act].pose[ 0 ].joint.size()==0 ) return;

	cur.pose++; 
//		if ( cur.pose > static_cast<signed>(animations[ act ].pose.size())-1 ) cur.pose = 0;
	if ( cur.pose > static_cast<signed>(animations[cur.act].pose.size())-1 ) cur.pose = static_cast<signed>(animations[cur.act].pose.size())-1;

	if ( cur.pose >= 0 )
	{
		// キーフレーム切り替え
		int i = 0;
		for ( Joint& j : tblJoint )
		{
			j.pos = animations[cur.act].pose[ cur.pose ].joint[ i ].pos;
			i++;
		}
	}
}

//------------------------------------------------------------------------------
void Skeleton::LastKeyframe()
//------------------------------------------------------------------------------
{
	anim.bPlaying = false;

	if ( animations.size() ==0 ) return;
	if ( animations[cur.act].pose.size()==0) return;
	if ( animations[cur.act].pose[ 0 ].joint.size()==0 ) return;

	cur.pose = static_cast<signed>(animations[cur.act].pose.size())-1;

	if ( cur.pose >= 0 )
	{
		// キーフレーム切り替え
		int i = 0;
		for ( Joint& j : tblJoint )
		{
			j.pos = animations[cur.act].pose[ cur.pose ].joint[ i ].pos;
			i++;
		}
	}
}

//------------------------------------------------------------------------------
void Skeleton::RefrectKeyframe()
//------------------------------------------------------------------------------
{
	anim.bPlaying = false;

	if ( animations.size() ==0 ) return;
	if ( animations[cur.act].pose.size()==0) return;
	if ( animations[cur.act].pose[ 0 ].joint.size()==0 ) return;

	for ( int i = 0 ; i < static_cast<signed>(tblJoint.size()) ; i++ )
	{ 
		animations[cur.act].pose[ cur.pose ].joint[ i ].pos = tblJoint[ i ].pos;
	}
}

//------------------------------------------------------------------------------
void Skeleton::InsertKeyframe()
//------------------------------------------------------------------------------
{
	anim.bPlaying = false;

	if ( animations.size() ==0 ) 
	{
		animations.emplace_back();
	}
	
	animations[cur.act].pose.emplace( animations[cur.act].pose.begin() + cur.pose );
	for ( const Joint& j : tblJoint )
	{
		animations[cur.act].pose[ cur.pose ].joint.emplace_back( j.pos );
	}
}

//------------------------------------------------------------------------------
void Skeleton::CopyKeyframe()
//------------------------------------------------------------------------------
{

/*
struct Skeleton
{
	vector<Joint>				tblJoint;
	vector<Bone>				tblBone;

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
	unique_ptr<Skeleton> pNew(new Skeleton);
	{
		for ( Joint& j : tblJoint )
		{
			pNew->tblJoint.emplace_back( j.pos, j.weight, j.bCtrl );
		}

		for ( Bone& b : tblBone )
		{
			pNew->tblBone.emplace_back( b.n0, b.n1, b.bBold );
		}
	}
	{
		int	cntAction = static_cast<signed>(animations.size());
		for ( int act = 0 ; act < cntAction ; act++ )
		{
			pNew->animations.emplace_back();

			int	cntPose = static_cast<signed>(animations[ act ].pose.size());
			for ( int pose = 0 ; pose < cntPose ; pose++ )
			{
				pNew->animations[ act ].pose.emplace_back();

				for ( int j = 0 ; j < static_cast<signed>(tblJoint.size()) ; j++ )
				{
					vect3 pos = animations[ act ].pose[pose].joint[ j ].pos;
					pNew->animations[ act ].pose[pose].joint.emplace_back( pos );
				}
			}
		}
	}
	cur.pCopybuf = move(pNew);
	cur.copied_act = cur.act;
	cur.copied_pose = cur.pose;
}

//------------------------------------------------------------------------------
void Skeleton::PastKeyframe()
//------------------------------------------------------------------------------
{
	anim.bPlaying = false;

	InsertKeyframe();
	for ( int i = 0 ; i < static_cast<signed>(tblJoint.size()) ; i++ )
	{ 
//			vect3 v = animations[cur.copied_act].pose[ cur.copied_pose ].joint[ i ].pos;
		vect3 v = cur.pCopybuf->animations[cur.copied_act].pose[ cur.copied_pose ].joint[ i ].pos;
	
		animations[cur.act].pose[ cur.pose ].joint[ i ].pos = v;
		tblJoint[ i ].pos = v;
	}

}

//------------------------------------------------------------------------------
void Skeleton::CutKeyframe()
//------------------------------------------------------------------------------
{
	anim.bPlaying = false;

	if ( animations.size() ==0 ) return;
	if ( animations[cur.act].pose.size()==0 ) return;
	if ( animations[cur.act].pose[ 0 ].joint.size()==0 ) return;

	CopyKeyframe();

	animations[cur.act].pose.erase(animations[cur.act].pose.begin() +cur.pose );	

	if ( cur.pose > (signed)animations[cur.act].pose.size()-1 ) cur.pose = (signed)animations[cur.act].pose.size()-1;

	if ( animations.size() ==0 ) return;
	if ( animations[cur.act].pose.size() ==0 ) return;

	{
//			cur.pose = 0;
		{
			// キーフレーム切り替え
			int i = 0;
			for ( Joint& j : tblJoint )
			{
				j.pos = animations[cur.act].pose[ cur.pose ].joint[ i ].pos;
				i++;
			}
		}
	}

}

//------------------------------------------------------------------------------
void Skeleton::PrevAnimation()
//------------------------------------------------------------------------------
{
	anim.bPlaying = false;

	cur.act--;
	if ( cur.act < 0 ) cur.act = 0;

	if ( animations.size() ==0 ) return;
	if ( animations[cur.act].pose.size()==0) return;
	if ( animations[cur.act].pose[ 0 ].joint.size()==0 ) return;

	{
		cur.pose = 0;
		{
			// キーフレーム切り替え
			int i = 0;
			for ( Joint& j : tblJoint )
			{
				j.pos = animations[cur.act].pose[ cur.pose ].joint[ i ].pos;
				i++;
			}
		}
	}
}

//------------------------------------------------------------------------------
void Skeleton::NextAnimation()
//------------------------------------------------------------------------------
{
	anim.bPlaying = false;

	cur.act++; 
	if ( cur.act > static_cast<signed>(animations.size())-1 ) cur.act = static_cast<signed>(animations.size())-1;

	if ( animations.size() ==0 ) return;
	if ( animations[cur.act].pose.size()==0) return;
	if ( animations[cur.act].pose[ 0 ].joint.size()==0 ) return;

	{
		cur.pose = 0;

		{
			// キーフレーム切り替え
			int i = 0;
			for ( Joint& j : tblJoint )
			{
			
				j.pos = animations[cur.act].pose[ cur.pose ].joint[ i ].pos;
				i++;
			}
		}
	}
}

//------------------------------------------------------------------------------
void Skeleton::AddAnimation()
//------------------------------------------------------------------------------
{
	anim.bPlaying = false;

	cur.act = static_cast<signed>(animations.size());
	cur.pose = 0;
	animations.emplace_back();

	{

		animations[cur.act].pose.emplace_back();
		for ( const Joint& j : tblJoint )
		{
			animations[cur.act].pose[ cur.pose ].joint.emplace_back( j.pos );
		}
	}
}

//------------------------------------------------------------------------------
void Skeleton::update()
//------------------------------------------------------------------------------
{
	for ( int i = 0 ; i < 11 ; i++ )
	{
		// 骨コリジョン 張力計算
		for ( Bone b : tblBone )
		{
			Joint&	j0 = tblJoint[b.n0];
			Joint&	j1 = tblJoint[b.n1];
			vect3 v = j1.pos - j0.pos;
			float l = v.length() - b.length;
			vect3 va  =	v.normalize()*l;


#if 1
			float w0 = 0.33f;
			float w1 = 0.33f;
#else
			float w0 = j0.weight/(j1.weight + j0.weight) * 0.33f;
			float w1 = j1.weight/(j1.weight + j0.weight) * 0.33f;
#endif

#if 1
			if ( j0.priority ==1 ) 
			{
				w0 = 0.0f;
				w1 = 0.33f;
			}
			else
			if ( j1.priority ==1 ) 
			{
				w0 = 0.33f;
				w1 = 0.0f;
			}
#endif
			j0.tension += va*w0;
			j1.tension -= va*w1;

		}

		// 張力解消
		for ( Joint& a : tblJoint )
		{
			a.pos += a.tension;
			a.tension=0;
		}
	}
}

//------------------------------------------------------------------------------
void Skeleton::DrawBone( Pers& pers, SysGra& gra )
//------------------------------------------------------------------------------
{

	rgb col = rgb(0,1,0);
	// Human pers
	for ( Joint& j : tblJoint )
	{
		//	右手系座標系
		//	右手ねじ周り
		//	roll	:z	奥+
		//	pitch	:x	右+
		//	yaw		:y	下+
		vect3 v= j.pos;

		v = v * pers.cam.mat.invers();

		j.disp = pers.calcDisp3(v);
	}

	if ( stat.bShowSkin )
	{
		// Human 描画
		for ( Bone b : tblBone )
		{
			Joint&	j0 = tblJoint[b.n0];
			Joint&	j1 = tblJoint[b.n1];
			if ( j0.disp.z > 0 && j0.disp.z > 0 )
			{
				// 肉
				const int cnt = 3;

				for ( int i = 0 ; i <= cnt ; i++ )
				{
					float t = (float)i / (float)cnt;

					vect3 pos = ( j1.pos - j0.pos )*t + j0.pos;

					vect3 v = ( j1.pos - j0.pos ).normalize();
					float rx = acos(dot(vect3(0,1,0),v));
					float ry = atan2(v.x,v.z);
					{
						mat44	mx;
						mat44	my;
						my.setRotateY(-ry);
						mx.setRotateX(-rx);
						mat44 m = mx * my;
						ring.ring_DrawMat( gra, pers, pos, m );
					}

				}

			}
		}
	}

	if ( stat.bShowBone )
	{
		gra.SetZTest( false );
		// Human 描画
		for ( Bone b : tblBone )
		{
			Joint&	j0 = tblJoint[b.n0];
			Joint&	j1 = tblJoint[b.n1];
			if ( j0.disp.z > 0 && j0.disp.z > 0 )
			{
				// 骨
				vect2 v0(j0.disp.x,j0.disp.y);
				vect2 v1(j1.disp.x,j1.disp.y);
				if ( b.bBold ) 
					gra.Line( v0,v1, col,3);
				else
					gra.Line( v0,v1, col,1);

			}
		}

		// ジョイント表示
		for ( Joint& j : tblJoint )
		{
			vect2 v0 = pers.calcDisp2( j.pos * pers.cam.mat.invers() );
			if ( j.bCtrl ) gra.Pset( v0, rgb(0,1,0), 11 );

//			vect2 v1 = pers.calcDisp2( (j.pos+j.bnormal) * pers.cam.mat.invers() );
//			gra.Line( v0,v1, rgb(0,1,0) );

//			vect2 v2 = pers.calcDisp2( (j.pos-j.bnormal) * pers.cam.mat.invers() );
//			gra.Line( v0,v2, rgb(0,1,0) );

		}
		gra.SetZTest( true );
	}

	if ( stat.bShowLocus )
	{
		gra.SetZTest( false );

		// 軌跡表示
		if ( static_cast<signed>(animations.size()) > 0 )
		{
			const float dt = anim.dt;
			const float div = 1/dt;

			for ( int n = -1 ; n < static_cast<signed>(animations[cur.act].pose.size())-3+1 ; n++ )
			{
				int n0 = n;
				int n1 = n+1;
				int n2 = n+2;
				int n3 = n+3;
				if ( n0 < 0 ) n0 = 0;
				if ( n3 >= static_cast<signed>(animations[cur.act].pose.size()) ) n3 = n2;
				for ( int j = 0 ;  j < static_cast<signed>(tblJoint.size()) ; j++ )
				{
					if ( tblJoint[ j ].stat.bSelected == false ) continue;
				
					vect3 P0 = animations[cur.act].pose[ n0 ].joint[ j ].pos;
					vect3 P1 = animations[cur.act].pose[ n1 ].joint[ j ].pos;
					vect3 P2 = animations[cur.act].pose[ n2 ].joint[ j ].pos;
					vect3 P3 = animations[cur.act].pose[ n3 ].joint[ j ].pos;

					float t = dt;
					vect3 a = catmull3d_func(0, P0,P1,P2,P3 );
					for ( int i = 0 ; i <div ; i++ )
					{
						vect3 b = catmull3d_func(t, P0,P1,P2,P3 );

						vect3 v0;
						vect3 v1;
						bool flg = pers.calcScissorLine3d( a* pers.cam.mat.invers(), b* pers.cam.mat.invers(), v0, v1 );

						rgb	col = rgb(1,1,1);
						if ( flg )
						{
							gra.Line( vect2(v0.x,v0.y), vect2(v1.x,v1.y), col,1);
						}
						
//						if ( v1.z > 0 ) gra.Pset(vect2(v1.x,v1.y), col, 4);

						a=b;
						t+=dt;

					}	
				}
			}
		}

		gra.SetZTest( true );
	}

}

//------------------------------------------------------------------------------
void Skeleton::saveMotion()
//------------------------------------------------------------------------------
{
	anim.bPlaying = false;

	fstream fo( filename.c_str(), ios::out );

	{
		fo << "joint" << endl;
		for ( Joint& j : tblJoint )	// 関節の位置
		{
			fo << "\t"<< j.pos.x << "\t" << j.pos.y << "\t" << j.pos.z << endl;
		}

		fo << "bone" << endl;
		for ( Bone& b : tblBone )	// 骨
		{
			Joint&	j0 = tblJoint[b.n0];
			Joint&	j1 = tblJoint[b.n1];
			b.length = (j1.pos - j0.pos).length();
			fo  << "\t"<< b.n0 << "\t" << b.n1 << endl;
		}
	}
	{
		int	cntAction = static_cast<signed>(animations.size());
		for ( int act = 0 ; act < cntAction ; act++ )
		{
			fo << "motion" << endl;
			int	cntPose = static_cast<signed>(animations[ act ].pose.size());
			for ( int pose = 0 ; pose < cntPose ; pose++ )
			{
				for ( int j = 0 ; j < static_cast<signed>(tblJoint.size()) ; j++ )
				{
					fo  << "\t"<< animations[ act ].pose[ pose ].joint[ j ].pos.x << "\t" << animations[ act ].pose[ pose ].joint[ j ].pos.y << "\t" << animations[ act ].pose[ pose ].joint[ j ].pos.z << endl;
				}
				if( pose+1 < cntPose ) fo << "," << endl;
			}
		}
	}
	fo << "end" << endl;

	cout << "SAVED" << endl;
}
//------------------------------------------------------------------------------
void Skeleton::loadMotion( const string fn )
//------------------------------------------------------------------------------
{
	filename = fn;

	anim.bPlaying = false;


	fstream fi( filename.c_str(), ios::in );
	string buf;
	enum
	{
		ModeNone,
		ModeJoint,
		ModeBone,
		ModeMotion,
	} mode = ModeNone;
	
	function<vector<string>(const string&, char)> split = [] (const string &s, char delimiter ) 
	{
		vector<string> elems;
		stringstream ss(s);
		string item;
		while (getline(ss, item, delimiter )) 
		{
			if (!item.empty()) 
			{
				elems.push_back(item);
			}
		}
		return elems;
	};

	int act = 0;

	while ( getline( fi, buf ) )
	{
 		if ( *(buf.end()-1) == 0x0d ) buf.pop_back();//CRLF対応
		if ( buf.size() == 0 ) continue;
		if ( buf.substr(0,2) == "//" ) continue;
		cout << buf << endl;
		if ( string(buf) == "joint" )	{mode = ModeJoint;	continue;}
		if ( string(buf) == "bone" )	{mode = ModeBone;	continue;}
		if ( string(buf) == "motion" )	
		{
			mode = ModeMotion;
			act = static_cast<signed>(animations.size());
			animations.emplace_back();
			animations[ act ].pose.emplace_back();
			continue;
		}
		if ( string(buf) == "," ) 
		{
			animations[ act ].pose.emplace_back();
			continue;
		}
		if ( string(buf) == "end" )	
		{	
			for ( Bone& b : tblBone )	// 関節の距離を決定する。
			{
				Joint&	j0 = tblJoint[b.n0];
				Joint&	j1 = tblJoint[b.n1];
				b.length = (j1.pos - j0.pos).length();
			}
			{
				int cnt = 0 ;
				for ( Joint& j : tblJoint )
				{
					j.id = cnt++;				//id登録
				}
			}
			for ( Bone& b : tblBone )	// ジョイントに関節の距離を決定する。
			{
				Joint&	j0 = tblJoint[b.n0];
				Joint&	j1 = tblJoint[b.n1];
				j1.relative.emplace_back( j0 ); 
				j0.relative.emplace_back( j1 ); 
			}
			cur.pose = 0;
			break;
		}
		switch( mode )
		{
			case ModeJoint:
				{
					vector<string> v = split( buf, '\t');
					float x = stod(v[0]);
					float y = stod(v[1]);
					float z = stod(v[2]);
					float weight = v.size() >3?stod(v[2]):1.0f;
					bool bCtrl = (v.size() >4 && v[4]=="-" )?false:true;
					tblJoint.emplace_back( vect3(x,y,z) , weight, bCtrl );
					//	cout << x << "," << y << "," << z << endl; 
				}
				break;
			case ModeBone:
				{
					vector<string> v = split( buf, '\t');
					int n0 = stoi(v[0]);
					int n1 = stoi(v[1]);
					bool bBold = (v.size() >2 && v[2]=="B" )?false:true;
					tblBone.emplace_back( n0, n1, bBold );
					//	cout << x << "," << y << "," << z << endl; 
				}
				break;
			case ModeMotion:
				{
					vector<string> v = split( buf, '\t');
					float x = stod(v[0]);
					float y = stod(v[1]);
					float z = stod(v[2]);
					animations[ act ].pose[ animations[ act ].pose.size()-1 ].joint.emplace_back( vect3(x,y,z) );
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
