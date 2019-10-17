struct Obj
{
	bool	bSelected	 = false;
	bool 	bPreselect	= false;		//	仮選択

	vect3	pos;

	Obj(){};
	Obj( vect3 _pos ) : pos(_pos){};
	virtual ~Obj(){};
};



struct Pers
{

	// カメラ
	struct
	{
		vect3	pos = vect3(  0.5, 1.5, -3+0.1 );
		vect3 	at = vect3( 0,  1, 0 );
		vect3	up = vect3( 0, 1, 0);
	  	mat44	mat;		

		//------------------------------------------------------------------------------
		void Rotation( vect3 mov )
		//------------------------------------------------------------------------------
		{
			float len = (pos-at).abs();
			float l = (pos-at).abs()/10;
			l=max(l,0.00001f/8);
			l=min(l,8);

			float eyeup = dot( (pos-at).normalize(), up ); // 視線ベクトルとupベクトルの織り成すcos th
			if ( eyeup > sin(deg2rad(80)) )
			{
				if ( mov.y > 0 ) mov.y = 0;
			}

			{
				// 回転
				vect3	v = mov * l;
				mat33 mrot = mat.GetRotate();
				v = v* mrot;

				{
					pos += v;
					
					// 正規化
					{
						vect3 dir = (pos-at).normalize();
						pos = at+dir*len;
					}
				}
			}
		}

		//------------------------------------------------------------------------------
		void Move( vect3 v )
		//------------------------------------------------------------------------------
		{
			mat33 mrot = mat.GetRotate();
			v = v* mrot;

			at += v;
			pos += v;
		}

		//------------------------------------------------------------------------------
		void Zoom( float step )
		//------------------------------------------------------------------------------
		{
			vect3	v= vect3(0,0,step);
			mat33 mrot = mat.GetRotate();
			v = v* mrot;

			vect3 r = pos;
			pos += v;
			if( (pos-at).abs() <= v.abs() ) pos = (r-at).normalize()*0.00001+at;
		}

		//------------------------------------------------------------------------------
		void Update()
		//------------------------------------------------------------------------------
		{
			mat.LookAt( pos, at, up );
		}
		
	} cam ;


	float	fovy;		// 画角
	float	sz;			// 投影面までの距離
	float	fy;			// 投影面の高さ/2（描画スケール）
	float	aspect;		// 描画画面のアスペクト比
	float	rate;		// 投影面未満の接近率

	//--------------------------------------------------------------------------
	Pers()
	//--------------------------------------------------------------------------
	{
		fovy=90/2;
//		sz = 1/tan(deg2rad(fovy)/2);				// 投影面までの距離
		rate = 8.0f*8;

	}

	//--------------------------------------------------------------------------
	void Update( vect2 screensize )
	//--------------------------------------------------------------------------
	{
	#if 0
		//投影面サイズ基準
		fy = 1;									// 投影面の高さ/2
		sz = fy/tan(deg2rad(fovy)/2);				// 投影面までの距離
	#else
		//投影面までの距離基準	こちらの方がニアクリップがしやすく扱いやすい。
		sz = 1.0;								// 投影面までの距離
		fy = sz*tan(deg2rad(fovy)/2);				// 投影面の高さ/2
	#endif
		aspect	= screensize.x/screensize.y;	// 描画画面のアスペクト比

	} 

	//--------------------------------------------------------------------------
	float getW( float z ) const
	//--------------------------------------------------------------------------
	{
		return 1/((fy/sz)*(z+sz));	// 投影面のz位置を0とした場合のzに対するw値を求める。
	}


	//--------------------------------------------------------------------------
	vect3 calcViewScreen3( vect3 v )	// 透視変換
	//--------------------------------------------------------------------------
	{
		vect3 ret;
		{
			float w = getW(v.z);
			ret.x = v.x* w / aspect;
			ret.y = v.y* w 	;
//			ret.z = w;	// 三次元ベクトルで返す都合上、ZにW値を入れている。
			ret.z = w/rate;	// 三次元ベクトルで返す都合上、ZにW値を入れている。
		}

		return ret;
	}
	//--------------------------------------------------------------------------
	vect2 calcDisp2( vect3 v )	// 透視変換
	//--------------------------------------------------------------------------
	{
		v = calcViewScreen3(v);
		return vect2( v.x, v.y );
	}
	
	//--------------------------------------------------------------------------
	vect3 calcWorldToScreen3( vect3 v )	// 透視変換
	//--------------------------------------------------------------------------
	{
		v = v * cam.mat.invers();
		return calcViewScreen3(v);
	}

	//--------------------------------------------------------------------------
	vect2 calcWorldToScreen2( vect3 v )	// 透視変換
	//--------------------------------------------------------------------------
	{
		v = calcWorldToScreen3(v);
		return vect2(v.x,v.y);
	}

	//--------------------------------------------------------------------------
	vect3 calcScreenToWorld3( vect3 q )	// 透視変換後の画面座標から空間座標を求める。
	//--------------------------------------------------------------------------
	{
		vect3 v;
		float w = getW(0);
		v.x = q.x/(w /aspect);
		v.y = q.y/(w);
		v.z = q.z/(w);
		return v * cam.mat;
	}

	//--------------------------------------------------------------------------
	vect3 calcScreenToWorld2( vect2 q )	// 透視変換後の画面座標から空間座標を求める。
	//--------------------------------------------------------------------------
	{
		return calcScreenToWorld3( vect3(q,0) );
	}
	
	//--------------------------------------------------------------------------
	vect3 calcRayvect( vect3 v1 ) 
	//--------------------------------------------------------------------------
	{
		vect3 v0 = vect3(0,0,-sz) * cam.mat; // 投影原点

		vect3 v = (v1-v0).normalize(); // 正規化

		return v;
	}

	//--------------------------------------------------------------------------
	tuple<bool,vect3> calcScreenToGround( vect2 q )	// 透視変換後の画面座標から『床』上の座標を求める。
	//--------------------------------------------------------------------------
	{
		vect3 P = calcScreenToWorld2( q );
		vect3 I = calcRayvect( P );
		auto[b,t,Q] = distanceIntersectPlate( vect3(0,0,0), vect3(0,1,0), P, I );
		return {b,Q};
	}

	//--------------------------------------------------------------------------
	bool calcScissorLine3d( vect3 v0, vect3 v1, vect3& va, vect3& vb )	// 3D 空間座標のままシザリングを行う。
	//--------------------------------------------------------------------------
	{
		va = calcViewScreen3(v0);
		vb = calcViewScreen3(v1);

		{//シザリング ニアクリップ
			function<vect3(vect3,vect3,int)>nearclip = [ this,&nearclip ]( vect3 a, vect3 b, int n )
			{
				if (n <=0 ) return b;

				vect3 c =  (a+b)/2;
		
				if ( c.z <= -sz )
				{
					c = nearclip( a, c, n-1 );
				}
				if ( c.z > 0.0 )
				{
					c = nearclip( c, b, n-1 );
				}
				return c;
			};
			if ( va.z > 0|| vb.z > 0 )
			{
				if ( va.z < 0 )
				{
					vect3 c = nearclip(v1,v0,8);
					va = calcViewScreen3(c);
				}
				if ( vb.z < 0 )
				{
					vect3 c = nearclip(v0,v1,8);
					vb = calcViewScreen3(c);
				}
			}
		}
		return ( va.z > 0 && vb.z > 0 );
	}
	
	//------------------------------------------------------------------------------
	void Focus( vector<vector<Obj*>>& tbls )
	//------------------------------------------------------------------------------
	{
		vect3 b(-infinit);
		vect3 s(infinit);
		vect3 b2;
		vect3 s2;

		for ( vector<Obj*>& tblObj : tbls )
		{
			for ( Obj* p : tblObj )
			{
				if ( p->bSelected )
				{
					vect3 v = calcWorldToScreen3( p->pos );
					if ( b.x < v.x ) b2.x = p->pos.x;
					if ( b.y < v.y ) b2.y = p->pos.y;
					if ( b.z < v.z ) b2.z = p->pos.z;
					if ( s.x > v.x ) s2.x = p->pos.x;
					if ( s.y > v.y ) s2.y = p->pos.y;
					if ( s.z > v.z ) s2.z = p->pos.z;
				}
			}
		}
		vect3 v2 = ave( b2, s2 );
		cam.pos = cam.pos - cam.at + v2;
		cam.at = v2;
//					cam.Update();
	}

		

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
		void DrawGrid3d( SysGra& gra, Pers& pers,  vect3 pos, mat33 m, int NUM_U, int NUM_V, float dt, rgb col  )
		//------------------------------------------------------------------------------
		{	// ミニグリッド
			vect3 vt = vect3(0,0,0);
			float du = (float)NUM_U*dt;
			float dv = (float)NUM_V*dt;
			vect3 a;
			vect3 b;
			{
				a = pos+vect3(-du, 0,-du);
				b = pos+vect3( du, 0,-du);
				vt = vect3(0,0,dt);
				for ( int i = 0 ; i < NUM_V*2+1 ; i++ )
				{
					vect3 v0 = a * m;
					vect3 v1 = b * m;
					pers.line3d_scissor( gra, pers, v0, v1, col );
					a+=vt;
					b+=vt;
				}
			}			
			{

				a = pos+vect3(-dv, 0, dv);
				b = pos+vect3(-dv, 0,-dv);
				vt = vect3(dt,0,0);
				for ( int i = 0 ; i < NUM_U*2+1 ; i++ )
				{
					vect3 v0 = a * m;
					vect3 v1 = b * m;
					pers.line3d_scissor( gra, pers, v0, v1, col );
					a+=vt;
					b+=vt;
				}
			}			

			{//原点表示
				float r = 0.1;
				vect3 a;
				for ( int i = 0 ; i <= 360 ; i+=20 )
				{
					vect3 b = vect3( r*cos(deg2rad((float)i)), 0, r*sin(deg2rad((float)i)) ) + pos;
					if ( i > 0 ) 
					{
						vect3 v0 = a * m;
						vect3 v1 = b * m;
						pers.line3d( gra, pers, v0,v1, col );
					}
					a = b;
				}
			}
		}
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

};

