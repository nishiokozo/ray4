	struct Joint3 : Obj
	{
		int id;
		vect3 pos;
		vect3 tension;
		vect3 world;
		vect3 disp;
		double len;
		int priority;
		
		vector<reference_wrapper<Joint3>>	relative;
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
	};

	struct Bone3
	{
		int n0;
		int n1;
		Joint3& j0;
		Joint3& j1;
		double length;
		Bone3( vector<Joint3>& tbl, int _n0, int _n1 ) :n0(_n0), n1(_n1), j0(tbl[_n0]), j1(tbl[_n1]){}
	};


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

		struct
		{
			int	num = 0;	//	アニメーションカーソル位置
			int	pose = 0;	//	キーフレームカーソル位置
			int copied_num = 0;
			int copied_pose = 0;
			unique_ptr<Data> pCopybuf;

			bool	bSelecting = false;
			int 	selecting_num = 0;
			int 	selecting_pose = 0;

			//
			double	t = 0;
			double	dt = 0.1;
			int n = 0;
			bool	bForward = true;
			bool	bPlaying = false;
		} anim;

	};

	void bone_load( Data& data, const char* filename );
	void bone_save( Data& data, const char* filename );
	void bone_AddAnimation( Data& data );
	void bone_NextAnimation( Data& data );
	void bone_PrevAnimation( Data& data );
	void bone_CutKeyframe( Data& data );
	void bone_PastKeyframe( Data& data );
	void bone_CopyKeyframe( Data& data );
	void bone_InsertKeyframe( Data& data );
	void bone_RefrectKeyframe( Data& data );
	void bone_LastKeyframe( Data& data );
	void bone_NextKeyframe( Data& data );
	void bone_TopKeyframe( Data& data );
	void bone_PrevKeyframe( Data& data );
	void bone_Play( Data& data );
	void bone_ReqAnimation( Data& data );
	vect3 catmull3d_func( double t, const vect3 P0, const vect3 P1, const vect3 P2, const vect3 P3 );
	void bone_update( Data& data, Pers& pers, mat44& cam_mat, SysGra& gra );
	void bone_drawMotion(  Data& data, Pers& pers, mat44& cam_mat, SysGra& gra );
