struct Obj
{
	bool	bSelected	 = false;
	bool 	bPreselect	= false;		//	仮選択

	vect3	pos;

//	Obj(){};							// 定義したくないが継承すると強制的にデフォルトコンストラクタが要求される
	Obj( vect3 _pos ) : pos(_pos){};
	virtual ~Obj(){};
};

struct Edge
{
	bool	bSelected	 = false;
	bool	bPreselect	= false;		//	仮選択
	int		n0;
	int		n1;
	rgb		col	= rgb(1,1,1);
	float	wide = 2.0;

	Edge( int _n0, int _n1 ) : n0(_n0),n1(_n1){};
	Edge( int _n0, int _n1, rgb c, float w ) : n0(_n0),n1(_n1),col(c),wide(w){};
	virtual ~Edge(){};
};

struct Pers
{

	// カメラ
	struct Cam
	{
		vect3	pos = vect3(  0.5, 1.5, -3+0.1 );
		vect3 	at = vect3( 0,  1, 0 );
		vect3	up = vect3( 0, 1, 0);
	  	mat44	mat;		

		//------------------------------------------------------------------------------
		void Rotation( vect3 mov );
		//------------------------------------------------------------------------------

		//------------------------------------------------------------------------------
		void Move( vect3 v );
		//------------------------------------------------------------------------------

		//------------------------------------------------------------------------------
		void Zoom( float step );
		//------------------------------------------------------------------------------

		//------------------------------------------------------------------------------
		void Update();
		//------------------------------------------------------------------------------
		
	} cam ;


	float	fovy;		// 画角
	float	sz;			// 投影面までの距離
	float	fy;			// 投影面の高さ/2（描画スケール）
	float	aspect;		// 描画画面のアスペクト比
	float	rate_w;		// 投影面未満の接近率 wバッファに書き込むレート

	//--------------------------------------------------------------------------
	Pers();
	//--------------------------------------------------------------------------

	//--------------------------------------------------------------------------
	void Update( vect2 screensize );
	//--------------------------------------------------------------------------

	//--------------------------------------------------------------------------
	float getW( float z ) const;
	//--------------------------------------------------------------------------

	//--------------------------------------------------------------------------
	vect3 calcViewScreen3( vect3 v );	// 透視変換
	//--------------------------------------------------------------------------

	//--------------------------------------------------------------------------
	vect2 calcDisp2( vect3 v );	// 透視変換
	//--------------------------------------------------------------------------
	
	//--------------------------------------------------------------------------
	vect3 calcWorldToScreen3( vect3 v );	// 透視変換
	//--------------------------------------------------------------------------

	//--------------------------------------------------------------------------
	vect2 calcWorldToScreen2( vect3 v );	// 透視変換
	//--------------------------------------------------------------------------

	//--------------------------------------------------------------------------
	vect3 calcScreenToWorld3( vect3 q );	// 透視変換後の画面座標から空間座標を求める。
	//--------------------------------------------------------------------------

	//--------------------------------------------------------------------------
	vect3 calcScreenToWorld2( vect2 q );	// 透視変換後の画面座標から空間座標を求める。
	//--------------------------------------------------------------------------
	
	//--------------------------------------------------------------------------
	vect3 calcRayvect( vect3 v1 ); 
	//--------------------------------------------------------------------------

	//--------------------------------------------------------------------------
	tuple<bool,vect3> calcScreenToGround( vect2 q );	// 透視変換後の画面座標から『床』上の座標を求める。
	//--------------------------------------------------------------------------

	//--------------------------------------------------------------------------
	bool calcScissorLine3d( vect3 v0, vect3 v1, vect3& va, vect3& vb );	// 3D 空間座標のままシザリングを行う。
	//--------------------------------------------------------------------------

	//------------------------------------------------------------------------------
	void Focus( vector<vector<Obj*>>& tbls );
	//------------------------------------------------------------------------------


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

	struct Axis
	{

		bool bAxisX = true;;
		bool bAxisY = true;;
		bool bAxisZ = true;;

		//------------------------------------------------------------------------------
		void DrawAxis( SysGra& gra, Pers& pers, vect2 mpos );
		//------------------------------------------------------------------------------
	} axis;

	struct Grid
	{
		//------------------------------------------------------------------------------
		void DrawGrid3d( SysGra& gra, Pers& pers, vect3 pos, mat33 m, int NUM_U, int NUM_V, float dt, rgb col  );
		//------------------------------------------------------------------------------
	} grid;


	struct Prim
	{
		void DrawPlate( SysGra& gra, Pers& pers, vect3 p, vect3 q, int n, rgb col=rgb(0,1,0) );
		void DrawMat33(  SysGra& gra, Pers& pers, vect3 v0, mat33 m );
		void DrawBox( SysGra& gra, Pers& pers, vect3 pos, mat33 m , bool bAxis = true, bool bTri = true );
		void DrawDrum( SysGra& gra, Pers& pers,  vect3 pos, mat33 m  );
		void DrawShpere( SysGra& gra, Pers& pers, float radius, vect3 pos, mat33 m  );
		void DrawTire( SysGra& gra, Pers& pers, vect3 pos, float head, float bank, float radius );
		void DrawSquare( SysGra& gra, Pers& pers, vect3 pos, mat33 m , bool bAxis = true, bool bTri = true );
	} prim;

};

