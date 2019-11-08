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

	void tri3d( SysGra& gra, Pers& pers, vect3 p0, vect3 p1, vect3 p2, rgb col );

	void pset2d( SysGra& gra, Pers& pers, vect2 p0, rgb col = rgb(1,1,1), float wide = 1.0f );
	void pset3d( SysGra& gra, Pers& pers, vect3 p0, rgb col = rgb(1,1,1), float wide = 1.0f );

	void line2d( SysGra& gra, Pers& pers, vect2 p0, vect2 p1, rgb col = rgb(1,1,1), float wide=1.0f );
	void line3d( SysGra& gra, Pers& pers, vect3 p0, vect3 p1, rgb col = rgb(1,1,1), float wide = 1.0f );
	void line3d_scissor( SysGra& gra, Pers& pers, vect3 p0, vect3 p1, rgb col = rgb(1,1,1), float wide = 1.0f );

	void print3d( SysGra& gra, Pers& pers, vect3 p0, float x, float y, string str );
	void print2d( SysGra& gra, Pers& pers, vect2 p0, float x, float y, string str );

	void showMat33(  SysGra& gra, Pers& pers, vect3 v0, mat33 m );
	void DrawBox( SysGra& gra, Pers& pers, vect3 pos, mat33 m , bool bAxis = true, bool bTri = true );
	void DrawDrum( SysGra& gra, Pers& pers,  vect3 pos, mat33 m  );



	// タイヤ
	struct Tire
	{

		vector<vect3> vert;
		vector<vect3> disp;

		//------------------------------------------------------------------------------
		Tire()
		//------------------------------------------------------------------------------
		{
			float r = 1.0;
			for ( float t = 0 ; t < 2*pi ; t+=deg2rad(5) )
			{
				float x = 0;
				float y = r * sin(t);
				float z = r * cos(t);

				vert.emplace_back( x, y, z );
			}
		}

		//------------------------------------------------------------------------------
		void DrawTire( SysGra& gra, Pers& pers, vect3 pos, float head, float bank, float radius )
		//------------------------------------------------------------------------------
		{
			mat33 m = mrotz(bank) * mroty(head);

			rgb col(0,1,1);
			int i = 0;
			vect3 v0;
			for ( vect3 v : vert )
			{
				vect3 v1 = v*radius * m + pos;

				if ( i > 0 ) 
				{
					pers.line3d( gra, pers, v0, v1, col, 2 );

					// 影
					vect3 a0= v0;
					vect3 a1= v1;
					a0.y=0;
					a1.y=0;
					pers.line3d( gra, pers, a0, a1, col/4, 2 );
				}

				v0 = v1;
				i++;
			}

		}
		
	} tire;

	struct Square
	{

		const float s = 0.04f;
		vector<vect3> vert=
		{
			{	-s,		 s,		0	},//0
			{	 s,		 s,		0	},//1
			{	-s,		-s,		0	},//2
			{	 s,		-s,		0	},//3

		};
		vector<vect3> disp;

		vector<ivect2>	edge
		{
			{	0,	1	},
			{	1,	3	},
			{	3,	2	},
			{	2,	0	},
		};

		//------------------------------------------------------------------------------
		void DrawSquare( SysGra& gra, Pers& pers, vect3 pos, mat33 m , bool bAxis = true, bool bTri = true )
		//------------------------------------------------------------------------------
		{
			disp.clear();

			for ( vect3 v : vert )
			{

				//	右手系座標系
				//	右手ねじ周り
				//	roll	:z	奥+
				//	pitch	:x	右+
				//	yaw		:y	下+
				v= v * m + pos;

				disp.emplace_back( v );

			}


			// 軸
			if( bAxis )
			{
				vect3	nx = vect3( m.m[0][0], m.m[0][1], m.m[0][2] );
				vect3	ny = vect3( m.m[1][0], m.m[1][1], m.m[1][2] );
				vect3	nz = vect3( m.m[2][0], m.m[2][1], m.m[2][2] );
				pers.line3d( gra, pers, pos, pos+nx*0.2, rgb(1,0,0) );
				pers.line3d( gra, pers, pos, pos+ny*0.2, rgb(0,1,0) );
				pers.line3d( gra, pers, pos, pos+nz*0.2, rgb(0,0,1) );
			}
			
			int cnt = 0;
			for ( ivect2 e : edge )
			{
				const vect3& a = disp[e.p];
				const vect3& b = disp[e.n];
				rgb col = rgb(0,1,1);
				if ( cnt == 0 ) col = rgb(1,0,0);
				//if ( cnt == 1 ) col = rgb(0,1,0);
				cnt++;
				pers.line3d( gra, pers, a, b, col, false );

			}
		}
		
	} square;


	struct Grid
	{
		//------------------------------------------------------------------------------
		void DrawGrid3d( SysGra& gra, Pers& pers,  vect3 pos, mat33 m, int NUM_U, int NUM_V, float dt, rgb col  );
		//------------------------------------------------------------------------------

		//------------------------------------------------------------------------------
		void Draw2D( SysGra& gra, Pers& pers, vect2 pos, rgb col )
		//------------------------------------------------------------------------------
		{
			gra.Line( vect2(-1,pos.y), vect2(1,pos.y), col*0);
			gra.Line( vect2(pos.x,-1), vect2(pos.x,1), col*0);
			for ( float x = -10 ; x < 10 ; x += 1 )
			{
				gra.Line( vect2(pos.x+x/pers.aspect,-1), vect2(pos.x+x/pers.aspect,1), col );
			}
			for ( float y = -10 ; y < 10 ; y += 1 )
			{
				gra.Line( vect2(-1,pos.y+y), vect2(1,pos.y+y), col );
			}

			gra.Circle( pos, 0.05, col );
		
		}

	} grid;

	struct Axis
	{

		bool bAxisX = true;;
		bool bAxisY = true;;
		bool bAxisZ = true;;

		//------------------------------------------------------------------------------
		void DrawAxis( SysGra& gra, Pers& pers, vect2 mpos )
		//------------------------------------------------------------------------------
		{
			gra.SetZTest( false );

			vect3 pos = pers.calcScreenToWorld3( vect3(mpos,0) );

			vect3 v0 = pers.calcWorldToScreen3( pos );

			// 軸表示
			float l = 0.1;
			if ( bAxisX  )
			{
				vect3 v1 = v0 + vect3(
					pers.cam.mat.m[0][0] / pers.aspect,
					pers.cam.mat.m[1][0],
					pers.cam.mat.m[2][0]
				) * l;
				gra.Line( v0, v1, rgb(0.8,0.2,0.2), 2.0 );
			}
			if ( bAxisY  )
			{
				vect3 v1 = v0 + vect3(
					pers.cam.mat.m[0][1] / pers.aspect,
					pers.cam.mat.m[1][1],
					pers.cam.mat.m[2][1]
				) * l;
				gra.Line( v0, v1, rgb(0.2,0.8,0.2), 2.0 );
			}
			if ( bAxisZ )
			{
				vect3 v1 = v0 + vect3(
					pers.cam.mat.m[0][2] / pers.aspect,
					pers.cam.mat.m[1][2],
					pers.cam.mat.m[2][2]
				) * l;
				gra.Line( v0, v1, rgb(0.1,0.3,1), 2.0 );

			}

			// 軸名表示
			gra.Print( mpos+gra.Dot(16,-12),string("")+(bAxisX?"X":"")+(bAxisY?"Y":"")+(bAxisZ?"Z":"") ); 


			gra.Pset( v0, rgb(1,1,1),3 );

			gra.SetZTest( true );

		}
	} axis;

};

