#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <cmath>
#include <functional>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iomanip>


#include "geom.h"

#include "SysGra.h"

#include "pers.h"
#include "skeleton.h"
#include "func.h"

using namespace std;

//------------------------------------------------------------------------------
void Skeleton::SaveSkeleton()
//------------------------------------------------------------------------------
{

	anim.bPlaying = false;

	fstream fo( filename.c_str(), ios::out );


	{
		fo << "joint" << endl;
		fo << fixed;
		fo << showpoint;
		fo << showpos;
		for ( Joint& j : tblJointForm )	// 関節の位置
		{
			fo << "\t"<< j.pos.x << "\t" << j.pos.y << "\t" << j.pos.z << endl;
		}
		fo << noshowpoint;
		fo << noshowpos;


		fo << "bone" << endl;
		for ( Bone& b : tblBone )	// 骨
		{
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
				for ( int j = 0 ; j < static_cast<signed>(tblJointForm.size()) ; j++ )
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
void Skeleton::LoadSkeleton( const string fn )
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
				if ( item.substr(0,2) == "//" ) break;
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
				Joint&	j0 = tblJointForm[b.n0];
				Joint&	j1 = tblJointForm[b.n1];
				b.length = (j1.pos - j0.pos).abs();
			}
			{
				int cnt = 0 ;
				for ( Joint& j : tblJointForm )
				{
					j.id = cnt++;				//id登録
				}
			}
			for ( Bone& b : tblBone )	// ジョイントに関節の距離を決定する。
			{
				Joint&	j0 = tblJointForm[b.n0];
				Joint&	j1 = tblJointForm[b.n1];
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
					float weight	= (v.size() >3 && v[3]!="-"  )?stod(v[3]):0.33f;
					bool bCtrl 		= (v.size() >4 && v[4]=="-" )?false:true;
					tblJointForm.emplace_back( vect3(x,y,z) , weight, bCtrl );
					//	cout << x << "," << y << "," << z << endl; 
				}
				break;
			case ModeBone:
				{
					vector<string> v = split( buf, '\t');
					int n0 = stoi(v[0]);
					int n1 = stoi(v[1]);
					bool bBold = (v.size() >2 && v[2]=="-" )?false:true;
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
	this->bActive = true;


	// カーソル更新
	tblJoint = tblJointForm;
	if ( (signed)animations.size() > cur.act )
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
		vect3 b = catmull3_func(anim.t, P0,P1,P2,P3 );

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
void Skeleton::UpdateSkeleton()
//------------------------------------------------------------------------------
{
	// 保管
	for ( Joint& a : tblJoint )
	{
//		a.prev = (a.prev+a.pos)/2.0;

		if ( a.prev != a.pos )
		{
			a.prev8 = (a.prev7+a.prev)/2;
			a.prev7 = (a.prev6+a.prev)/2;
			a.prev6 = (a.prev5+a.prev)/2;
			a.prev5 = (a.prev4+a.prev)/2;
			a.prev4 = (a.prev3+a.prev)/2;
			a.prev3 = (a.prev2+a.prev)/2;
			a.prev2 = (a.prev+a.prev)/2;
		}
		a.prev = a.pos;
	}


	for ( int i = 0 ; i < 111 ; i++ )	// 収束回数多いほど収束数る
	{
		// 骨コリジョン 張力計算
		for ( Bone b : tblBone )
		{
			Joint&	j0 = tblJoint[b.n0];
			Joint&	j1 = tblJoint[b.n1];
			vect3 v = j1.pos - j0.pos;
			float l = v.abs() - b.length;
			vect3 va  =	v.normalize()*l;


			float w0 = 0.33f;
			float w1 = 0.33f;
			w0 = j0.weight;
			w1 = j1.weight;

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
void Skeleton::DrawSkeleton( Pers& pers, SysGra& gra )
//------------------------------------------------------------------------------
{

	// 影 描画
	if ( stat.bShowBone )
	{
		gra.SetZTest( false );

		rgb col = rgb(0.2,0.2,0.2);
		for ( Bone b : tblBone )
		{
			Joint&	j0 = tblJoint[b.n0];
			Joint&	j1 = tblJoint[b.n1];
	
	
			vect3 v0= j0.pos;
			v0.y = 0;
			v0 = pers.calcWorldToScreen3(v0);

			vect3 v1= j1.pos;
			v1.y = 0;
			v1 = pers.calcWorldToScreen3(v1);

	
			if ( v1.z > 0 && v1.z > 0 )
			{
				gra.Line( v0,v1, col,2);

			}
		}
		gra.SetZTest( true );
	}	
	
//	rgb col = rgb(0,1,0);


	// 透視投影変換
	for ( Joint& j : tblJoint )
	{
		//	右手系座標系
		//	右手ねじ周り
		//	roll	:z	奥+
		//	pitch	:x	右+
		//	yaw		:y	下+
		j.disp = pers.calcWorldToScreen3( j.pos );
	}

	// 肉 描画
	if ( stat.bShowSkin )
	{
		for ( Bone b : tblBone )
		{
			Joint&	j0 = tblJoint[b.n0];
			Joint&	j1 = tblJoint[b.n1];
			if ( j0.disp.z > 0 && j0.disp.z > 0 )
			{
				// 
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



	// 骨 描画
	if ( stat.bShowBone )
	{
		rgb col = rgb(0,0,1);
		
		gra.SetZTest( false );
		for ( Bone b : tblBone )
		{
			Joint&	j0 = tblJoint[b.n0];
			Joint&	j1 = tblJoint[b.n1];
			if ( j0.disp.z > 0 && j0.disp.z > 0 )
			{
				if ( b.bBold )
				gra.Line( j0.disp,j1.disp, col, b.bBold?3:1);

			}
		}

		// ジョイント表示
		for ( Joint& j : tblJoint )
		{
			vect3 v0 = pers.calcWorldToScreen3( j.pos );
			if ( j.bCtrl ) gra.Pset( v0, col, 11 );
		}
		gra.SetZTest( true );
	}
	
	// 剛体実験

	if (0)
	{
		Joint&	j = tblJoint[2];

		static vect3 v = vect3(-0.2, 0, 0);
	
		vect3 p0 = (tblJoint[1].prev8-tblJoint[2].prev8).normalize();
		vect3 p1 = (tblJoint[1].pos-tblJoint[2].pos).normalize();
		float d = dot(p0,p1); 
//cout << fixed<< d << endl;
//if ( dot(p0,p1) == 1.0f ) cout << "a"<<endl;
		vect3 p2 = cross(p1,p0).normalize();
		float a = dot( p2, tblJoint[2].binormal);
		float b = dot( -p2, tblJoint[2].binormal);
		if ( a < b  ) p2 = -p2;

//		if ( d > 0.9999999 ) p2 = tblJoint[2].binormal;
//		if ( d > 0.999 ) p2 = tblJoint[2].binormal;
		tblJoint[2].binormal = p2;

		vect3 p3 = cross(p2,p1);
		mat44	m( 
			p1.x, p1.y, p1.z,	0,
			p2.x, p2.y, p2.z,	0,
			p3.x, p3.y, p3.z,	0,
			0,	0,	0,	1
		);


//		vect3 va = -v * m;
//		vect3 vb =  v * m;
//		vect3 v0 = pers.calcWorldToScreen3( j.pos-p3 );
//		vect3 v1 = pers.calcWorldToScreen3( j.pos+p3 );
//		gra.Line(v0,v1,rgb(1,0,0));



		vect3 vj0 = pers.calcWorldToScreen3( j.pos );
		vect3 vj1 = pers.calcWorldToScreen3( j.pos+p1 );
		vect3 vj2 = pers.calcWorldToScreen3( j.pos+p2 );
		vect3 vj3 = pers.calcWorldToScreen3( j.pos+p3 );
		gra.Line(vj0,vj1,rgb(1,0,1));
		gra.Line(vj0,vj2,rgb(1,1,0));
		gra.Line(vj0,vj3,rgb(0,1,1));

	
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
					if ( tblJoint[ j ].bSelected == false ) continue;
				
					vect3 P0 = animations[cur.act].pose[ n0 ].joint[ j ].pos;
					vect3 P1 = animations[cur.act].pose[ n1 ].joint[ j ].pos;
					vect3 P2 = animations[cur.act].pose[ n2 ].joint[ j ].pos;
					vect3 P3 = animations[cur.act].pose[ n3 ].joint[ j ].pos;

					float t = dt;
					vect3 a = catmull3_func(0, P0,P1,P2,P3 );
					for ( int i = 0 ; i <div ; i++ )
					{
						vect3 b = catmull3_func(t, P0,P1,P2,P3 );

						g_line3d( gra, pers, a, b, vect3(1,1,1));

						a=b;
						t+=dt;

					}	

					// キーフレーム表示
					{
						vect3 v0 = pers.calcWorldToScreen3(P0);
						vect3 v1 = pers.calcWorldToScreen3(P1);
						vect3 v2 = pers.calcWorldToScreen3(P2);
						vect3 v3 = pers.calcWorldToScreen3(P3);
						gra.Pset(v0, rgb(0,0,1), 11);
						gra.Pset(v1, rgb(0,0,1), 11);
						gra.Pset(v2, rgb(0,0,1), 11);
						gra.Pset(v3, rgb(0,0,1), 11);
					}
				}
			}
		}

		gra.SetZTest( true );
	}

}

