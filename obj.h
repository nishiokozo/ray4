struct	Obj
{
	int id = 0;

	bool 	bRectSelected	= false;		//	矩形選択中、選択＆非選択
	bool 	bRectIn			= false;		//	矩形選択中、矩形選択対象
	bool 	bSelected		= false;		//	選択
	bool 	bAffectable		= false;		//	削除対象

	virtual void Move2( vect2 v ) =0;
	virtual vect2 Pos2() =0;
	virtual	bool IsVisuable() = 0;
};

struct Pers
{

	// カメラ
	struct
	{
		vect3	pos = vect3( -2,-2, -2+0.1 );
		vect3 	at = vect3( 0, -1, 0 );
		vect3	up = vect3( 0, 1, 0);
	  	mat44	mat;		

		//------------------------------------------------------------------------------
		void Rotation( vect3 mov )
		//------------------------------------------------------------------------------
		{
			double len = (pos-at).length();
			double l = (pos-at).length()/10;
			l=max(l,0.00001);
			l=min(l,8);

			// 回転
			vect3	v = mov * l;
			mat44 mrot = mat;
			mrot.SetTranslate(vect3(0,0,0));
			v = v* mrot;
			pos += v;
			
			{
				vect3 dir = (pos-at).normalize();
				pos = at+dir*len;
			}

		}

		//------------------------------------------------------------------------------
		void Move( vect3 v )
		//------------------------------------------------------------------------------
		{
			mat44 mrot = mat;
			mrot.SetTranslate(vect3(0,0,0));
			v = v* mrot;

			at += v;
			pos += v;
		}

		//------------------------------------------------------------------------------
		void Zoom( double step )
		//------------------------------------------------------------------------------
		{
			vect3	v= vect3(0,0,step);
			mat44 mrot = mat;
			mrot.SetTranslate(vect3(0,0,0));
			v = v* mrot;

			vect3 r = pos;
			pos += v;
			if( (pos-at).length() <= v.length() ) pos = (r-at).normalize()*0.00001+at;
		}

	} cam ;


	double	fovy;		// 画角
	double	sz;			// 投影面までの距離
	double	fy;			// 投影面の高さ/2（描画スケール）
	double	cx;			// 描画画面の中心W
	double	cy;			// 描画画面の中心H
	double	width;		// 描画画面の解像度W/2
	double	height;		// 描画画面の解像度H/2
	double	aspect;		// 描画画面のアスペクト比

	//--------------------------------------------------------------------------
	Pers()
	//--------------------------------------------------------------------------
	{
		fovy=90/2;
		sz = 1/tan(rad(fovy)/2);				// 投影面までの距離
	}

	//--------------------------------------------------------------------------
	void Update( vect2 screensize )
	//--------------------------------------------------------------------------
	{
	#if 0
		//投影面サイズ基準
		fy = 1;									// 投影面の高さ/2
		sz = fy/tan(rad(fovy)/2);				// 投影面までの距離
	#else
		//投影面までの距離基準	こちらの方がニアクリップがしやすく扱いやすい。
		sz = 1.0;								// 投影面までの距離
		fy = sz*tan(rad(fovy)/2);				// 投影面の高さ/2
	#endif
		cx		= screensize.x/2;				// 描画画面の中心W
		cy		= screensize.y/2;				// 描画画面の中心H
		width	= screensize.x/2;				// 描画画面の解像度W/2
		height	= screensize.y/2;				// 描画画面の解像度H/2
		aspect	= screensize.y/screensize.x;	// 描画画面のアスペクト比


	} 

	//--------------------------------------------------------------------------
	double getW( double z ) const
	//--------------------------------------------------------------------------
	{
		return 1/((fy/sz)*(z+sz));	// 投影面のz位置を0とした場合のzに対するw値を求める。
	//	return 1/(fy*(z+1));	// 投影面のz位置を0とした場合のzに対するw値を求める。
	}

	//--------------------------------------------------------------------------
	vect3 calcDisp( vect3 v )	// 透視変換
	//--------------------------------------------------------------------------
	{
		vect3 ret;
		{
			double w = getW(v.z);
			ret.x = v.x* w *width  *aspect	+cx;
			ret.y = v.y* w *height			+cy;
			ret.z = w;	// 三次元ベクトルで返す都合上、ZにW値を入れている。
		}

		return ret;
	}
	
	//--------------------------------------------------------------------------
	vect3 calcInvers( vect2 q )	// 透視変換後の画面座標から空間座標を求める。
	//--------------------------------------------------------------------------
	{
		vect3 v;
		double w = getW(0);
		v.x = (q.x -cx)/(w *width  *aspect);
		v.y = (q.y -cy)/(w *height);
		v.z = 0;
		return v * cam.mat;
	}
	
	//--------------------------------------------------------------------------
	vect3 calcRay( vect3 v1, double len ) 
	//--------------------------------------------------------------------------
	{
		vect3 v0 = vect3(0,0,-sz) * cam.mat; // 投影原点

		vect3 v = (v1-v0).normalize()*len + v0;

		return v;
	}

	//--------------------------------------------------------------------------
	vect3 calcRayvect( vect3 v1 )	// 投影原点絡みたベクトル
	//--------------------------------------------------------------------------
	{
		vect3 v0 = vect3(0,0,-sz) * cam.mat; // 投影原点
		return (v1-v0).normalize();
	}
	
	//--------------------------------------------------------------------------
	bool calcScissorLine3d( vect3 v0, vect3 v1, vect3& va, vect3& vb )	// 3D 空間座標のままシザリングを行う。
	//--------------------------------------------------------------------------
	{
		va = calcDisp(v0);
		vb = calcDisp(v1);

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
					va = calcDisp(c);
				}
				if ( vb.z < 0 )
				{
					vect3 c = nearclip(v0,v1,8);
					vb = calcDisp(c);
				}
			}
		}
		return ( va.z > 0 && vb.z > 0 );
	}			
/*
	//------------------------------------------------------------------------------
	void line3d( SysGra& gra, vect3 p0, vect3 p1, int col )
	//------------------------------------------------------------------------------
	{
		double l = 0.2;
		vect3 a = p0* cam.mat.invers();
		vect3 b = p1* cam.mat.invers();
		vect3 v0;
		vect3 v1;
		bool flg = calcScissorLine3d( a, b, v0, v1 );
		if ( flg ) gra.Line( vect2(v0.x,v0.y), vect2(v1.x,v1.y), col );
	}
*/
};

// 輪
struct Ring
{
	struct Trigon
	{
		double	z;
		vect2	v0;
		vect2	v1;
		vect2	v2;
		int		col;
		Trigon( 
			double	_z,
			vect2	_v0,
			vect2	_v1,
			vect2	_v2,
			int		_col
		)
		:
		z(_z),
		v0(_v0),
		v1(_v1),
		v2(_v2),
		col(_col)
		{}		

	    bool operator<( const Trigon& a ) const 
	    {
	        return z < a.z;
	    }

	};
	
	vector<Trigon>	trigons;
	const double h = 0.05;
	const double w = 0.08;
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

	//------------------------------------------------------------------------------
	void CalcPers( Pers& pers, vect3 pos, vect3 rot )
	//------------------------------------------------------------------------------
	{
		vect3 l = vect3(0,0,1).normalize();	// 正面ライト
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

			double d = 0;
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
				double z = a.x*b.y-a.y*b.x;
				if ( z > 0 ) 
				{
					trigons.emplace_back( z, d0, d1, d2, rgb(d,d,d) );
//							gra.Tri( d0, d1, d2, rgb(d,d,d));
				}
			}
		}
		sort(trigons.begin(), trigons.end());

	}

	//------------------------------------------------------------------------------
	void DrawTrigons( SysGra& gra )
	//------------------------------------------------------------------------------
	{
		// トリゴン描画 trigons	
		for ( Trigon& t : trigons )
		{
				gra.Tri( t.v0, t.v1, t.v2, t.col);
		}
		trigons.clear();
	}

};

