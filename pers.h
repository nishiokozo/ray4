struct Obj
{
	bool	bSelected	 = false;
	bool 	bPreselect	= false;		//	仮選択

	vect3	pos;

	Obj(){};
	Obj( vect3 _pos ) : pos(_pos){};
	virtual ~Obj(){};
};

struct Point3 : Obj
{
	Point3( vect3 _pos ) :Obj(_pos) { pos = _pos;}
	vect3	a;
	vect3	b;

	Point3( vect3 _pos, vect3 _a, vect3 _b ) :Obj(_pos) { pos=_pos; a=_a; b=_b;}
};

struct Edge
{
	bool	bSelected	 = false;
	bool	bPreselect	= false;		//	仮選択
	int		n0;
	int		n1;
	rgb col=rgb(1,1,1);
	float	wide = 2.0;

	Edge( int _n0, int _n1 ) : n0(_n0),n1(_n1){};
	Edge( int _n0, int _n1, rgb c, float w ) : n0(_n0),n1(_n1),col(c),wide(w){};
	virtual ~Edge(){};
};

struct Cp
{
	std::vector<std::vector<std::shared_ptr<Obj>>> tbltbl_pObj;
	std::vector<std::vector<std::shared_ptr<Edge>>> tbltbl_pEdge;
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

	bool	bOrtho = false;	// 平行投影フラグ

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
	std::tuple<bool,vect3> calcScreenToGround( vect2 q );	// 透視変換後の画面座標から『床』上の座標を求める。
	//--------------------------------------------------------------------------

	//--------------------------------------------------------------------------
	bool calcScissorLine3d( vect3 v0, vect3 v1, vect3& va, vect3& vb );	// 3D 空間座標のままシザリングを行う。
	//--------------------------------------------------------------------------

	//------------------------------------------------------------------------------
	void Focus( std::vector<std::vector<std::shared_ptr<Obj>>>& tbltbl_pObj );
	//------------------------------------------------------------------------------


	struct Pen
	{
		void Tri3d( SysGra& gra, Pers& pers, vect3 p0, vect3 p1, vect3 p2, rgb col=rgb(1,1,1) );
		void Pset2d( SysGra& gra, Pers& pers, vect2 p0, rgb col=rgb(1,1,1), float wide=1.0f );
		void Pset3d( SysGra& gra, Pers& pers, vect3 p0, rgb col=rgb(1,1,1), float wide=1.0f );
		void Line2d( SysGra& gra, Pers& pers, vect2 p0, vect2 p1, rgb col=rgb(1,1,1), float wide=1.0f );
		void Line3d( SysGra& gra, Pers& pers, vect3 p0, vect3 p1, rgb col=rgb(1,1,1), float wide=1.0f );
		void Line3d_scissor( SysGra& gra, Pers& pers, vect3 p0, vect3 p1, rgb col=rgb(1,1,1), float wide=1.0f );
		void Circle3d( SysGra& gra, Pers& pers, vect3 p0, float radius, float step=24, rgb col=rgb(1,1,1), float wide=1.0f );
		void Circle2d( SysGra& gra, Pers& pers, vect2 p0, float radius, float step=24, rgb col=rgb(1,1,1), float wide=1.0f );
		void Print3d( SysGra& gra, Pers& pers, vect3 p0, float x, float y, std::string str, rgb col=rgb(1,1,1) );
		void Print2d( SysGra& gra, Pers& pers, vect2 p0, float x, float y, std::string str, rgb col=rgb(1,1,1) );

		void DrawBezier( SysGra& gra, Pers& pers, vect3 P0, vect3 P1, vect3 P2, vect3 P3 );
		void DrawBezierSurface(
			SysGra& gra, Pers& pers,
			vect3 p0 , vect3 p1 , vect3 p2 , vect3 p3,
			vect3 p0u, vect3 p1u, vect3 p2u, vect3 p3u,
			vect3 p0v, vect3 p1v, vect3 p2v, vect3 p3v
		);

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
		rgb col=rgb(0.2,0.2,0.2);
		mat33	mat =  mat33::midentity();

		void SetCol( const rgb& _col ) { col =_col; }
		void SetMat( const mat33& _mat ) { mat = _mat; }
		void DrawGrid3d( SysGra& gra, Pers& pers, vect3 pos, mat33 m, int NUM_U, int NUM_V, float dt, rgb col=rgb(1,1,1), bool bPlot = false );
		void DrawGrid( SysGra& gra, Pers& pers );
		void DrawEternalGlid( SysGra& gra, Pers& pers, vect3 pos, mat33 m, float NUM_U, float NUM_V, rgb col=rgb(1,1,1) );

		struct Plot
		{
			int MaxPlot = 100;
			int cntPlot = 0;
			bool	bScroll = false;
			rgb col=rgb(1,1,1);
			float step;

			std::vector<float> tblDot;
			int amtPlot = 0;
			
			Plot( 
				int	_max,
				float _step, 			// 送りステップ
				rgb _col = rgb(1,1,1) 	// 色
			);
			void ResetPlot();
			void WritePlot( float val );
			void DrawPlot( SysGra& gra, Pers& pers, mat33& m );

		} plot = Plot( 100, 0.02, rgb(1,0,1) );
		
		void Circle( SysGra& gra, Pers& pers, vect2 p0, float radius, float step=24, rgb col=rgb(1,1,1), float wide=1.0f );
		void Line( SysGra& gra, Pers& pers, vect2 v0, vect2 v1, rgb col=rgb(1,1,1), float wide=1.0f );
		void Print( SysGra& gra, Pers& pers, vect2 p0, float x, float y, std::string str );

	} grid;


	struct Prim
	{
		void DrawPlate( SysGra& gra, Pers& pers, vect3 p, vect3 q, int n, rgb col=rgb(1,1,1) );
		void DrawMat33(  SysGra& gra, Pers& pers, vect3 v0, mat33 m );
		void DrawBox( SysGra& gra, Pers& pers, vect3 pos, mat33 m , bool bAxis = true, bool bTri = true );
		void DrawDrum( SysGra& gra, Pers& pers,  vect3 pos, mat33 m  );
		void DrawTire( SysGra& gra, Pers& pers, vect3 pos, float head, float bank, float radius );
		void DrawSquare( SysGra& gra, Pers& pers, vect3 pos, mat33 m , bool bAxis = true, bool bTri = true );
		void DrawCircle( SysGra& gra, Pers& pers, vect3 pos, mat33 m, float radius, rgb col=rgb(1,1,1) );
		void DrawSphere( SysGra& gra, Pers& pers, vect3 pos, mat33 m, float radius, rgb col=rgb(1,1,1) );
		void DrawVect( SysGra& gra, Pers& pers, int& text_y, vect3 v0, vect3 v, float sc, rgb col=rgb(1,1,1), std::string str="?", bool bShadow = true, bool bDump=true, bool bFlip=true );
	} prim;

};

