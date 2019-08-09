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

		struct
		{
			int	num = 0;	//	アニメーションカーソル位置
			int	pose = 0;	//	キーフレームカーソル位置
			int copied_num = 0;
			int copied_pose = 0;
			unique_ptr<Bone> pCopybuf;

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

		void loadMotion( const char* filename );
		void saveMotion( const char* filename );
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
		void update( Pers& pers, mat44& cam_mat, SysGra& gra );
		void drawMotion( Pers& pers, mat44& cam_mat, SysGra& gra );

	};

