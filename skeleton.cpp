#include <iostream>
#include <memory>
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

#include "func.h"
#include "pers.h"
#include "skeleton.h"

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
//		for ( Bone& b : tblBone )	// 骨
		{
//			fo  << "\t"<< b.n0 << "\t" << b.n1 << endl;
		}
		for ( shared_ptr<Edge> p : tbl_pEdge )	// 骨
		{
			fo  << "\t"<< p->n0 << "\t" << p->n1 << endl;
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
//			for ( Bone& b : tblBone )	// 関節の距離を決定する。
			{
//				Joint&	j0 = tblJointForm[b.n0];
//				Joint&	j1 = tblJointForm[b.n1];
//				b.length = (j1.pos - j0.pos).abs();
			}
			for ( shared_ptr<Edge> p  : tbl_pEdge )	// 関節の距離を決定する。
			{
				Bone* pb = dynamic_cast<Bone*>(p.get());

				Joint&	j0 = tblJointForm[p->n0];
				Joint&	j1 = tblJointForm[p->n1];
				pb->length = (j1.pos - j0.pos).abs();
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
//					tblBone.emplace_back( n0, n1, bBold );
					tbl_pEdge.emplace_back( new Bone( n0, n1, bBold ) );
			
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


	for ( Joint& j : tblJointForm )	// 関節の位置
	{
		tbl_pObj.emplace_back( new Joint( j.pos , j.weight, j.bCtrl ) );
	}

	// 最初のアニメーションキーフレームを初期設定

	if ( (signed)animations.size() > cur.act )
	{
		// キーフレーム切り替え
		int i = 0;
	//	for ( shared_ptr<Obj> p : tbl_pObj )
		for ( shared_ptr<Obj> p : tbl_pObj )
		{
			p->pos = animations[cur.act].pose[ cur.pose ].joint[ i ].pos;
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
		vect3 b = func_catmull_rom_spline(anim.t, P0,P1,P2,P3 );

		tbl_pObj[ j ]->pos = b;

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
	if ( cur.pose < 0 ) cur.pose = 0;

	if ( cur.pose >= 0 )
	{
		// キーフレーム切り替え
		int i = 0;
		for ( shared_ptr<Obj> p : tbl_pObj )
		{
			p->pos = animations[cur.act].pose[ cur.pose ].joint[ i ].pos;
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
		for ( shared_ptr<Obj> p : tbl_pObj )
		{
			p->pos = animations[cur.act].pose[ cur.pose ].joint[ i ].pos;
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
		for ( shared_ptr<Obj> p : tbl_pObj )
		{
			p->pos = animations[cur.act].pose[ cur.pose ].joint[ i ].pos;
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
		for ( shared_ptr<Obj> p : tbl_pObj )
		{
			p->pos = animations[cur.act].pose[ cur.pose ].joint[ i ].pos;
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

	for ( int i = 0 ; i < static_cast<signed>(tbl_pObj.size()) ; i++ )
	{ 
		animations[cur.act].pose[ cur.pose ].joint[ i ].pos = tbl_pObj[ i ]->pos;
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
	for ( shared_ptr<Obj> p : tbl_pObj )
	{
		animations[cur.act].pose[ cur.pose ].joint.emplace_back( p->pos );
	}
}

//------------------------------------------------------------------------------
void Skeleton::CopyKeyframe()
//------------------------------------------------------------------------------
{

	unique_ptr<Skeleton> pNew(new Skeleton);
	{
		for ( shared_ptr<Obj> po : tbl_pObj )
		{
			Joint* p = dynamic_cast<Joint*>(po.get());
			
			pNew->tbl_pObj.emplace_back( new Joint( p->pos, p->weight, p->bCtrl ) );
		}

//		for ( Bone& b : tblBone )
		{
//			pNew->tblBone.emplace_back( b.n0, b.n1, b.bBold );
		}
		for ( shared_ptr<Edge> p : tbl_pEdge )
		{
			Bone* pb = dynamic_cast<Bone*>(p.get());
			pNew->tbl_pEdge.emplace_back( new Bone( pb->n0, pb->n1, pb->bBold ) );
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

				for ( int j = 0 ; j < static_cast<signed>(tbl_pObj.size()) ; j++ )
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
	for ( int i = 0 ; i < static_cast<signed>(tbl_pObj.size()) ; i++ )
	{ 
		vect3 v = cur.pCopybuf->animations[cur.copied_act].pose[ cur.copied_pose ].joint[ i ].pos;
	
		animations[cur.act].pose[ cur.pose ].joint[ i ].pos = v;
		tbl_pObj[ i ]->pos = v;
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
		{
			// キーフレーム切り替え
			int i = 0;
			for ( shared_ptr<Obj> p : tbl_pObj )
			{
				p->pos = animations[cur.act].pose[ cur.pose ].joint[ i ].pos;
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
			for ( shared_ptr<Obj> p : tbl_pObj )
			{
				p->pos = animations[cur.act].pose[ cur.pose ].joint[ i ].pos;
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
			for ( shared_ptr<Obj> p : tbl_pObj )
			{
			
				p->pos = animations[cur.act].pose[ cur.pose ].joint[ i ].pos;
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
		for ( shared_ptr<Obj> p : tbl_pObj )
		{
			animations[cur.act].pose[ cur.pose ].joint.emplace_back( p->pos );
		}
	}
}

//------------------------------------------------------------------------------
void Skeleton::UpdateSkeleton()
//------------------------------------------------------------------------------
{
	// 保管
	for ( shared_ptr<Obj> po : tbl_pObj )
	{
		Joint* p = dynamic_cast<Joint*>(po.get());

		if ( p->prev != p->pos )
		{
			p->prev8 = (p->prev7+p->prev)/2;
			p->prev7 = (p->prev6+p->prev)/2;
			p->prev6 = (p->prev5+p->prev)/2;
			p->prev5 = (p->prev4+p->prev)/2;
			p->prev4 = (p->prev3+p->prev)/2;
			p->prev3 = (p->prev2+p->prev)/2;
			p->prev2 = (p->prev+p->prev)/2;
		}
		p->prev = p->pos;
	}


	for ( int i = 0 ; i < 111 ; i++ )	// 収束回数多いほど収束数る
	{
		// 骨コリジョン 張力計算
/*
		for ( Bone b : tblBone )
		{
			Joint*	p0 = dynamic_cast<Joint*>(tbl_pObj[b.n0].get());
			Joint*	p1 = dynamic_cast<Joint*>(tbl_pObj[b.n1].get());
			vect3 v = p1->pos - p0->pos;
			float l = v.abs() - b.length;
			vect3 va  =	v.normalize()*l;


			float w0 = 0.33f;
			float w1 = 0.33f;
			w0 = p0->weight;
			w1 = p1->weight;

			p0->tension += va*w0;
			p1->tension -= va*w1;
		}
*/
		for ( shared_ptr<Edge> p : tbl_pEdge )
		{
			Bone* pb = dynamic_cast<Bone*>(p.get());
			
			Joint*	p0 = dynamic_cast<Joint*>(tbl_pObj[pb->n0].get());
			Joint*	p1 = dynamic_cast<Joint*>(tbl_pObj[pb->n1].get());
			vect3 v = p1->pos - p0->pos;
			float l = v.abs() - pb->length;
			vect3 va  =	v.normalize()*l;


			float w0 = 0.33f;
			float w1 = 0.33f;
			w0 = p0->weight;
			w1 = p1->weight;

			p0->tension += va*w0;
			p1->tension -= va*w1;
		}

		// 張力解消
		for ( shared_ptr<Obj> po : tbl_pObj )
		{
			Joint* p = dynamic_cast<Joint*>(po.get());

			p->pos += p->tension;
			p->tension=vect3(0,0,0);
		}
	}
	
}

//------------------------------------------------------------------------------
void Skeleton::DrawSkeleton( SysGra& gra, Pers& pers )
//------------------------------------------------------------------------------
{


	// 透視投影変換
	for ( shared_ptr<Obj> po : tbl_pObj )
	{
		Joint* p = dynamic_cast<Joint*>(po.get());
		//	右手系座標系
		//	右手ねじ周り
		//	roll	:z	奥+
		//	pitch	:x	右+
		//	yaw		:y	下+
		p->disp = pers.calcWorldToScreen3( p->pos );
	}

	// 肉 描画

	if ( stat.bShowSkin )
	{
//		for ( Bone b : tblBone )
		for ( shared_ptr<Edge> p : tbl_pEdge )
		{
			Bone* pb = dynamic_cast<Bone*>(p.get());

			Joint*	p0 = dynamic_cast<Joint*>(tbl_pObj[pb->n0].get());
			Joint*	p1 = dynamic_cast<Joint*>(tbl_pObj[pb->n1].get());

			if ( p0->disp.z > 0 && p0->disp.z > 0 )
			{
				// 
				const int cnt = 3;

				for ( int i = 0 ; i <= cnt ; i++ )
				{
					float t = (float)i / (float)cnt;

					vect3 pos = ( p1->pos - p0->pos )*t + p0->pos;

					vect3 v = ( p1->pos - p0->pos ).normalize();
					float rx = acos(dot(vect3(0,1,0),v));
					float ry = atan2(v.x,v.z);
					{
						mat44	mx;
						mat44	my;
						my.setRotateY(-ry);
						mx.setRotateX(-rx);
						mat44 m = mx * my;
						skin.ring_DrawMat( gra, pers, pos, m );
					}

				}

			}
		}
	}



	
	// 剛体実験

/*
	if (0)
	{
		Joint*	p = dynamic_cast<Joint*>(tbl_pObj[2].get());

		constexpr vect3 v = vect3(-0.2, 0, 0);
	
		vect3 pos0 = (tbl_pObj[1]->prev8-tbl_pObj[2]->prev8).normalize();
		vect3 pos1 = (tbl_pObj[1]->pos-tbl_pObj[2]->pos).normalize();
		float d = dot(pos0,pos1); 
//cout << fixed<< d << endl;
//if ( dot(pos0,pos1) == 1.0f ) cout << "a"<<endl;
		vect3 pos2 = cross(pos1,pos0).normalize();
		float a = dot( pos2, tbl_pObj[2]->binormal);
		float b = dot( -pos2, tbl_pObj[2]->binormal);
		if ( a < b  ) pos2 = -pos2;

//		if ( d > 0.9999999 ) pos2 = tbl_pObj[2]->binormal;
//		if ( d > 0.999 ) pos2 = tbl_pObj[2]->binormal;
		tbl_pObj[2]->binormal = pos2;

		vect3 pos3 = cross(pos2,pos1);
		mat44	m( 
			pos1.x, pos1.y, pos1.z,	0,
			pos2.x, pos2.y, pos2.z,	0,
			pos3.x, pos3.y, pos3.z,	0,
			0,	0,	0,	1
		);


//		vect3 va = -v * m;
//		vect3 vb =  v * m;
//		vect3 v0 = pers.calcWorldToScreen3( p->pos-p3 );
//		vect3 v1 = pers.calcWorldToScreen3( p->pos+p3 );
//		gra.Line(v0,v1,rgb(1,0,0));



		vect3 vj0 = pers.calcWorldToScreen3( p->pos );
		vect3 vj1 = pers.calcWorldToScreen3( p->pos+p1 );
		vect3 vj2 = pers.calcWorldToScreen3( p->pos+p2 );
		vect3 vj3 = pers.calcWorldToScreen3( p->pos+p3 );
		gra.Line(vj0,vj1,rgb(1,0,1));
		gra.Line(vj0,vj2,rgb(1,1,0));
		gra.Line(vj0,vj3,rgb(0,1,1));

	
	}
*/


	if ( stat.bShowLocus )
	{
		gra.SetZTest( false );

		// 軌跡表示
		if ( static_cast<signed>(animations.size()) > 0 )
		{
			const float dt = anim.dt;
			const int div = (int)(1/dt);

			for ( int n = -1 ; n < static_cast<signed>(animations[cur.act].pose.size())-3+1 ; n++ )
			{
				int n0 = n;
				int n1 = n+1;
				int n2 = n+2;
				int n3 = n+3;
				if ( n0 < 0 ) n0 = 0;
				if ( n3 >= static_cast<signed>(animations[cur.act].pose.size()) ) n3 = n2;
				for ( int j = 0 ;  j < static_cast<signed>(tbl_pObj.size()) ; j++ )
				{
					if ( tbl_pObj[ j ]->bSelected == false ) continue;
				
					vect3 P0 = animations[cur.act].pose[ n0 ].joint[ j ].pos;
					vect3 P1 = animations[cur.act].pose[ n1 ].joint[ j ].pos;
					vect3 P2 = animations[cur.act].pose[ n2 ].joint[ j ].pos;
					vect3 P3 = animations[cur.act].pose[ n3 ].joint[ j ].pos;

					float t = dt;
					vect3 a = func_catmull_rom_spline(0, P0,P1,P2,P3 );
					for ( int i = 0 ; i <div ; i++ )
					{
						vect3 b = func_catmull_rom_spline(t, P0,P1,P2,P3 );

						rgb col = rgb(0,1,0);
						pers.pen.line3d( gra, pers, a, b, col);

						a=b;
						t+=dt;

					}	

					// キーフレーム表示
					{
						vect3 v0 = pers.calcWorldToScreen3(P0);
						vect3 v1 = pers.calcWorldToScreen3(P1);
						vect3 v2 = pers.calcWorldToScreen3(P2);
						vect3 v3 = pers.calcWorldToScreen3(P3);
						float wide = 7;
						rgb col = rgb(0,1,0);
						gra.Pset(v0, col, wide);
						gra.Pset(v1, col, wide);
						gra.Pset(v2, col, wide);
						gra.Pset(v3, col, wide);
					}
				}
			}
		}

		gra.SetZTest( true );
	}

}

