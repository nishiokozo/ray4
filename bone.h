
// 輪
struct Ring
{
	#define	USE_LINE	0
	#define	USE_TRIGON	1
	struct PrimTrigon
	{
		float	z;
		vect2	v0;
		vect2	v1;
		vect2	v2;
		rgb	col;
		PrimTrigon( 
			float	_z,
			vect2	_v0,
			vect2	_v1,
			vect2	_v2,
			vect3	_col
		)
		:
		z(_z),
		v0(_v0),
		v1(_v1),
		v2(_v2),
		col(_col)
		{}		

	    bool operator<( const PrimTrigon& a ) const 
	    {
	        return z < a.z;
	    }

	};
	vector<PrimTrigon>	trigons;

	struct PrimLine
	{
		vect2	v0;
		vect2	v1;
		rgb		col;
		PrimLine( 
			vect2	_v0,
			vect2	_v1,
			int		_col
		)
		:
		v0(_v0),
		v1(_v1),
		col(_col)
		{}		
	};
	vector<PrimLine>	lines;


	const float h = 0.05;
	const float w = 0.08;
	vector<vect3> tbl_vert=
	{
		{	-w,	 h,	-w	},
		{	 w,	 h,	-w	},
		{	-w,	-h,	-w	},
		{	 w,	-h,	-w	},
		{	-w,	 h,	 w	},
		{	 w,	 h,	 w	},
		{	-w,	-h,	 w	},
		{	 w,	-h,	 w	},
	};

	vector<ivect3>	tbl_faces =
	{
		{2,3,0},{3,1,0},
		{3,7,1},{1,7,5},
		{7,6,5},{5,6,4},
		{6,2,4},{4,2,0},
	};

	vector<ivect2>	tbl_edge =
	{
		{0,1}, {2,3}, {0,2}, {1,3},
		{4,5}, {6,7}, {4,6}, {5,7},
		{2,6}, {3,7}, {0,4}, {1,5},
	};

	//------------------------------------------------------------------------------
	void CalcVert( Pers& pers, vect3 pos, vect3 rot )
	//------------------------------------------------------------------------------
	{
		vect3 l = vect3(0,0,1).normalize();	// 正面ライト
	#if USE_TRIGON
		for ( ivect3 v : tbl_faces )
		{
			mat44	rotx;
			mat44	roty;
			mat44	rotz;
			rotx.setRotateX(rot.x);
			roty.setRotateY(rot.y);
			rotz.setRotateZ(rot.z);

			vect3 v0 = tbl_vert[v.n0];
			vect3 v1 = tbl_vert[v.n1];
			vect3 v2 = tbl_vert[v.n2];

			v0= rotx * roty * rotz *v0 + pos ;
			v1= rotx * roty * rotz *v1 + pos ;
			v2= rotx * roty * rotz *v2 + pos ;

			v0 = v0 * pers.cam.mat.invers();
			v1 = v1 * pers.cam.mat.invers();
			v2 = v2 * pers.cam.mat.invers();

			float d = 0;
			{
				vect3 a = (v1-v0); 
				vect3 b = (v2-v0); 
				vect3 n = cross(a,b).normalize();
				d = dot(n,l) + 0.2;
				if ( d < 0.0 ) d=0;
				if ( d > 1.0 ) d=1.0;
			}

			v0 = pers.calcDisp( v0 );
			v1 = pers.calcDisp( v1 );
			v2 = pers.calcDisp( v2 );
			vect2 d0 = vect2(v0.x,v0.y);
			vect2 d1 = vect2(v1.x,v1.y);
			vect2 d2 = vect2(v2.x,v2.y);

			{
				vect2 a = vect2(d1-d0);
				vect2 b = vect2(d2-d0);
				float z = a.x*b.y-a.y*b.x;
				if ( z > 0 ) 
				{
					trigons.emplace_back( z, d0, d1, d2, rgb(d,d,d) );
				}
			}
		}
		sort(trigons.begin(), trigons.end());
	#endif
	#if USE_LINE
		for ( ivect2 v : tbl_edge )
		{
			mat44	rotx;
			mat44	roty;
			mat44	rotz;
			rotx.setRotateX(rot.x);
			roty.setRotateY(rot.y);
			rotz.setRotateZ(rot.z);

			vect3 v0 = tbl_vert[v.n0];
			vect3 v1 = tbl_vert[v.n1];

			v0= rotx * roty * rotz *v0 + pos ;
			v1= rotx * roty * rotz *v1 + pos ;

			v0 = v0 * pers.cam.mat.invers();
			v1 = v1 * pers.cam.mat.invers();

			v0 = pers.calcDisp( v0 );
			v1 = pers.calcDisp( v1 );

			vect2 d0 = vect2(v0.x,v0.y);
			vect2 d1 = vect2(v1.x,v1.y);

			lines.emplace_back( d0, d1, vect3(0,1,1) );
		}
	#endif

	}

	//------------------------------------------------------------------------------
	void DrawTrigons( SysGra& gra )
	//------------------------------------------------------------------------------
	{
	#if USE_TRIGON
		// トリゴン描画 trigons	
		for ( PrimTrigon& t : trigons )
		{
				gra.Tri( t.v0, t.v1, t.v2, t.col);
		}
		trigons.clear();
	#endif
	#if USE_LINE
		for ( PrimLine& e : lines )
		{
			gra.Line( e.v0, e.v1 , e.col );
		}
		lines.clear();
	#endif
	}

};

struct Joint3 : Obj
{
	int id;
	vect3 pos;
	vect3 tension;
	vect3 disp;
	float len;
	int priority;
	
	vector<reference_wrapper<Joint3>>	relative;
	virtual ~Joint3(){}
	Joint3()
	{
		id = 0;
		pos = vect3(0,0,0);
		tension = 0;
		len = 0;
	}
	Joint3( vect3 v )
	{
		id = 0;
		pos = v;
		tension = 0;
		len = 0;
	}
	void Move2( vect2 v )
	{
		//none
	}
	vect2 Pos2()
	{
		return vect2( disp.x, disp.y );
	}
	bool IsVisuable()
	{
		return disp.z > 0;
	}

};

struct Bone3
{
	int n0;
	int n1;
	Joint3& j0;
	Joint3& j1;
	float length;
	Bone3( vector<Joint3>& tbl, int _n0, int _n1 ) :n0(_n0), n1(_n1), j0(tbl[_n0]), j1(tbl[_n1]){}
};


struct Bone
{
	vector<Joint3>			tblJoint;
	vector<Bone3>			tblBone;

	struct Keyframe
	{
		struct Joint
		{
			struct Pos
			{
				vect3		pos;
				Pos( const vect3& _pos ) : pos(_pos) {};
			};
			vector<Pos>		joint;
		};
		vector<Joint>		pose;
	};
	vector<Keyframe>		animations;

	struct
	{
		int	act = 0;	//	アニメーションカーソル位置
		int	pose = 0;	//	キーフレームカーソル位置
		int copied_act = 0;
		int copied_pose = 0;
		unique_ptr<Bone> pCopybuf;

		bool	bSelecting = false;
		int 	selecting_act = 0;
		int 	selecting_pose = 0;
	} cur;
	struct
	{
		float	t = 0;
		float	dt = 0.1;
		int n = 0;
		bool	bForward = true;
		bool	bPlaying = false;
	} anim;

	void loadMotion( const string filename );
	void saveMotion( const string filename );
	void AddAnimation();
	void NextAnimation();
	void PrevAnimation();
	void CutKeyframe();
	void PastKeyframe();
	void CopyKeyframe();
	void InsertKeyframe();
	void RefrectKeyframe();
	void LastKeyframe();
	void NextKeyframe();
	void TopKeyframe();
	void PrevKeyframe();
	void PlayAnimation();
	void ReqAnimation();
	void update();
//		void draw( Pers& pers, mat44& cam_mat, SysGra& gra );
//		void drawMotion( Pers& pers, mat44& cam_mat, SysGra& gra );
	void draw( Pers& pers, SysGra& gra );
	void drawMotion( Pers& pers, SysGra& gra );


	//
	Ring ring;

};

