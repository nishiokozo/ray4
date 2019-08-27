
// 輪
struct Ring
{
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

		{0,1,4},{4,1,5},
		{6,3,2},{6,7,3},
	};


	//------------------------------------------------------------------------------
	void ring_DrawMat( SysGra& gra, Pers& pers, vect3 pos, mat44 m )
	//------------------------------------------------------------------------------
	{
		vect3 l = vect3(0,0,1).normalize();	// 正面ライト
		for ( ivect3 v : tbl_faces )
		{

			vect3 v0 = tbl_vert[v.n0];
			vect3 v1 = tbl_vert[v.n1];
			vect3 v2 = tbl_vert[v.n2];

			v0= v0 * m + pos ;
			v1= v1 * m + pos ;
			v2= v2 * m + pos ;

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

			v0 = pers.calcDisp3( v0 );
			v1 = pers.calcDisp3( v1 );
			v2 = pers.calcDisp3( v2 );


					gra.Tri( v0, v1, v2, rgb(d,d,d) );
		}

	}

};

struct Joint3// : Obj
{
	struct
	{
		bool 	bSelected		= false;		//	選択
		bool 	bPreselect		= false;		//	仮選択
	} stat;

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

	struct
	{
		bool	bShowBone = true;	//	骨
		bool	bShowSkin = true;	//	皮
		bool	bShowLocus = true;	//	軌跡
	} stat;

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

