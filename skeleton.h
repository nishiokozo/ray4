
// 輪
struct Skin
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
	std::vector<PrimTrigon>	trigons;


	const float h = 0.05;
	const float w = 0.08;
	std::vector<vect3> tbl_vert=
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

	std::vector<ivect3>	tbl_faces =
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

			v0 = pers.calcViewScreen3( v0 );
			v1 = pers.calcViewScreen3( v1 );
			v2 = pers.calcViewScreen3( v2 );


					gra.Tri3d( v0, v1, v2, rgb(d,d,d) );
		}

	}

};



struct Joint : Obj
{
	vect3 tension;
	vect3 disp;
	float len;
	int priority;
	bool bCtrl;
	float	weight;

	mat33	mat=mat33(1,0,0, 0,1,0, 0,0,1);

	vect3 prev;
	vect3 prev2;
	vect3 prev3;
	vect3 prev4;
	vect3 prev5;
	vect3 prev6;
	vect3 prev7;
	vect3 prev8;
	vect3 binormal;
	
//	std::vector<reference_wrapper<Joint>>	relative;
	virtual ~Joint(){}
	Joint( vect3 v, float _weight, bool _bCtrl ) :Obj(v)
	{
		pos = v;
		weight = _weight;
		bCtrl = _bCtrl;
		tension = vect3(0,0,0);
		len = 0;
		prev = pos;
		prev2 = pos;
		prev3 = pos;
		prev4 = pos;
		binormal = vect3(1,0,0);
	}

};

struct Bone : Edge
{
	float	length;
	bool	bBold;
	Bone( int _n0, int _n1, bool _bBold ) : Edge(_n0, _n1), bBold(_bBold){}
};


struct Skeleton
{
	bool	bActive = false;	//	使用可能
	int		idxTbl = 0;


	std::string	filename;

	std::vector<Joint>			tblJointForm;	// 基本フォーム
	std::vector<std::shared_ptr<Obj>>			tbl_pObj;		//joint	継承クラスとして使うためポインタ型
	std::vector<std::shared_ptr<Edge>>			tbl_pEdge;		// 継承クラスとして使うためポインタ型

	struct A
	{
		struct P
		{
			struct J
			{
				vect3		pos;
				J( const vect3& _pos ) : pos(_pos) {};
			};
			std::vector<J>		joint;
		};
		std::vector<P>		pose;
	};
	std::vector<A>		animations;	// アニメーションキーフレーム

	struct
	{
		int	act = 0;	//	アニメーションカーソル位置
		int	pose = 0;	//	キーフレームカーソル位置
		int copied_act = 0;
		int copied_pose = 0;
		std::unique_ptr<Skeleton> pCopybuf;

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

	void LoadSkeleton( const std::string filename );
	void SaveSkeleton();
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
	void UpdateSkeleton();
	void DrawSkeleton( SysGra& gra, Pers& pers );


	//
	Skin skin;

};

