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
	vect3 calcPoint2( vect3 v )	// 透視投影変換
	//--------------------------------------------------------------------------
	{
		v= v * cam.mat.invers();
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
	vect3 calcPoint( vect3 v )	// 透視変換
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
	bool calcScissorLine3d( vect3 v0, vect3 v1, vect3& va, vect3& vb )
	//--------------------------------------------------------------------------
	{
		va = calcPoint(v0);
		vb = calcPoint(v1);

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
					va = calcPoint(c);
				}
				if ( vb.z < 0 )
				{
					vect3 c = nearclip(v0,v1,8);
					vb = calcPoint(c);
				}
			}
		}
		return ( va.z > 0 && vb.z > 0 );
	}			
};
