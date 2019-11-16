

struct Point3 : Obj
{
	Point3( vect3 _pos ) :Obj(_pos) { pos = _pos;}
	vect3	a;
	vect3	b;

	Point3( vect3 _pos, vect3 _a, vect3 _b ) :Obj(_pos) { pos=_pos; a=_a; b=_b;}
};


struct Gui
{
	vector<vector<Obj*>> tbls;
	vector<vector<Edge*>> tbltblEdge;

	bool bShowPoint = true;

	struct One
	{
		bool	bEnable;
		float	w;		// wバッファ値
		int		idxTbl;	// オブジェクト番号
		int		idxObj;	// オブジェクト番号
		//--
		bool	bSelected_a = false;
		bool	bSelected_b = false;

		One()
		{
			clear();
		}
		
		void clear()
		{
			bEnable = false;
			w = 0.0;
			idxTbl = 0;
			idxObj = 0;
			bSelected_a = false;
			bSelected_b = false;
		}
		
	};

	enum class G_CALC
	{
		NONE,
		ADD,
		SUB,
		COPY,
		REV,
	};

	vect2 rect_st = vect2(0,0);			//	矩形選択開始位置
	G_CALC rect_mode = G_CALC::NONE;	//	矩形選択中フラグ
	One one;


	//------------------------------------------------------------------------------
	int EntryTbl( vector<Obj*>tbl, vector<Edge*>tblEdge )
	//------------------------------------------------------------------------------
	{
		int idx =  (signed)tbls.size();
		tbls.emplace_back( tbl );
		tbltblEdge.emplace_back( tblEdge );
		return idx;
	}

	//------------------------------------------------------------------------------
	void TouchFirst( SysGra& gra, Pers& pers, vector<vector<Obj*>>& tbls, vect2 mpos );
	//------------------------------------------------------------------------------

	// 矩形カーソル開始 新規選択
	//------------------------------------------------------------------------------
	void SelectRectOnly( vect2 mpos );
	//------------------------------------------------------------------------------

	// 矩形カーソル開始 追加選択
	//------------------------------------------------------------------------------
	void SelectRectAdd( vect2 mpos );
	//------------------------------------------------------------------------------

	// 矩形カーソル開始 反転選択
	//------------------------------------------------------------------------------
	void SelectRectRev( vect2 mpos );
	//------------------------------------------------------------------------------

	// 矩形カーソル開始 削除選択
	//------------------------------------------------------------------------------
	void SelectRectSub( vect2 mpos );
	//------------------------------------------------------------------------------
	
	//------------------------------------------------------------------------------
	void calcRectMode( G_CALC rect_mode, bool& bPreselect, bool& bSelected );
	//------------------------------------------------------------------------------

	// 矩形カーソル終了（選択決定）
	//------------------------------------------------------------------------------
	void SelectRectEnd( vector<vector<Obj*>>& tbls );
	//------------------------------------------------------------------------------

	// 矩形カーソル選択	
	//------------------------------------------------------------------------------
	void SelectRectBegin( Pers& pers, vector<vector<Obj*>>& tbls , vect2 mpos );
	//------------------------------------------------------------------------------

	// 単独 新規選択
	//------------------------------------------------------------------------------
	void SelectOneOnly( vector<vector<Obj*>>& tbls );
	//------------------------------------------------------------------------------

	// 単独 追加選択
	//------------------------------------------------------------------------------
	void SelectOneAdd( vector<vector<Obj*>>& tbls );
	//------------------------------------------------------------------------------

	// 単独 反転選択
	//------------------------------------------------------------------------------
	void SelectOneRev( vector<vector<Obj*>>& tbls );
	//------------------------------------------------------------------------------

	// 単独 削除選択
	//------------------------------------------------------------------------------
	void SelectOneSub( vector<vector<Obj*>>& tbls );

	struct Pen
	{
		void tri3d( SysGra& gra, Pers& pers, vect3 p0, vect3 p1, vect3 p2, rgb col );
		void pset2d( SysGra& gra, Pers& pers, vect2 p0, rgb col = rgb(1,1,1), float wide = 1.0f );
		void pset3d( SysGra& gra, Pers& pers, vect3 p0, rgb col = rgb(1,1,1), float wide = 1.0f );
		void line2d( SysGra& gra, Pers& pers, vect2 p0, vect2 p1, rgb col = rgb(1,1,1), float wide=1.0f );
		void line3d( SysGra& gra, Pers& pers, vect3 p0, vect3 p1, rgb col = rgb(1,1,1), float wide = 1.0f );
		void line3d_scissor( SysGra& gra, Pers& pers, vect3 p0, vect3 p1, rgb col = rgb(1,1,1), float wide = 1.0f );
		void circle3d( SysGra& gra, Pers& pers, vect3 p0, float radius, rgb col, float wide=1.0f );
		void print3d( SysGra& gra, Pers& pers, vect3 p0, float x, float y, string str );
		void print2d( SysGra& gra, Pers& pers, vect2 p0, float x, float y, string str );
	} pen;

	struct Prim
	{
		void DrawPlate( SysGra& gra, Pers& pers, Gui& gui, vect3 p, vect3 n, rgb col=rgb(0,1,0) );
		void DrawMat33(  SysGra& gra, Pers& pers, Gui& gui, vect3 v0, mat33 m );
		void DrawBox( SysGra& gra, Pers& pers, Gui& gui, vect3 pos, mat33 m , bool bAxis = true, bool bTri = true );
		void DrawDrum( SysGra& gra, Pers& pers, Gui& gui,  vect3 pos, mat33 m  );
		void DrawShpere( SysGra& gra, Pers& pers, Gui& gui, float radius, vect3 pos, mat33 m  );
		void DrawTire( SysGra& gra, Pers& pers, Gui& gui, vect3 pos, float head, float bank, float radius );
		void DrawSquare( SysGra& gra, Pers& pers, Gui& gui, vect3 pos, mat33 m , bool bAxis = true, bool bTri = true );
	} prim;

	//------------------------------------------------------------------------------
	// 選択リスト表示
	//------------------------------------------------------------------------------
	void DrawController( SysGra& gra, Pers& pers, Gui& gui, vector<vector<Obj*>>& tbls, vector<vector<Edge*>>& tbltblEdge, vect2 mpos );
	//------------------------------------------------------------------------------

	//------------------------------------------------------------------------------
	void MoveObj( SysGra& gra, Pers& pers, Gui& gui, vector<vector<Obj*>>& tbls, vect2& mpos, vect2& mprev, vect2& mmov, bool bSame, bool bByCamera, bool bByFloor, bool bByXY );
	//------------------------------------------------------------------------------

	struct Axis
	{

		bool bAxisX = true;;
		bool bAxisY = true;;
		bool bAxisZ = true;;

		//------------------------------------------------------------------------------
		void DrawAxis( SysGra& gra, Pers& pers, Gui& gui, vect2 mpos );
		//------------------------------------------------------------------------------
	} axis;

	struct Grid
	{
		//------------------------------------------------------------------------------
		void DrawGrid3d( SysGra& gra, Pers& pers, Gui& gui, vect3 pos, mat33 m, int NUM_U, int NUM_V, float dt, rgb col  );
		//------------------------------------------------------------------------------
	} grid;


};
