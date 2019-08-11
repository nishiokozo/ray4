	struct	Obj
	{
		int id = 0;

		bool 	bRectSelected	= false;		//	矩形選択中、選択＆非選択
		bool 	bRectIn			= false;		//	矩形選択中、矩形選択対象
		bool 	bSelected		= false;		//	選択
		bool 	bAffectable		= false;		//	削除対象

		virtual void Move2( vect2 v ) =0;
		virtual vect2 Pos2() =0;
	};

	struct Pers
	{
		double	fovy;		// 画角
		double	sz;			// 投影面までの距離
		double	sc;			// 投影面の高さ/2（描画スケール）
		double	cx;			// 描画画面の中心W
		double	cy;			// 描画画面の中心H
		double	width;		// 描画画面の解像度W/2
		double	height;		// 描画画面の解像度H/2
		double	aspect;		// 描画画面のアスペクト比
		mat44	m;
	
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
			sc = 1;									// 投影面の高さ/2
			sz = sc/tan(rad(fovy)/2);				// 投影面までの距離
		#else
			sz = 1.0;								// 投影面までの距離
			sc = sz*tan(rad(fovy)/2);				// 投影面の高さ/2
		#endif
			cx		= screensize.x/2;				// 描画画面の中心W
			cy		= screensize.y/2;				// 描画画面の中心H
			width	= screensize.x/2;				// 描画画面の解像度W/2
			height	= screensize.y/2;				// 描画画面の解像度H/2
			aspect	= screensize.y/screensize.x;	// 描画画面のアスペクト比


		} 

		//--------------------------------------------------------------------------
		vect3 calcPoint( vect3 v ) const
		//--------------------------------------------------------------------------
		{
		#if 1
			vect3 ret;
		//	double w = 1/(v.z+sz);
		//	ret.x = v.x*w	*sz /sc *width  *aspect	+cx;
		//	ret.y = v.y*w	*sz /sc *height			+cy;
		//	ret.z = w;
			ret.x = v.x/(v.z+sz)	*sz /sc *width  *aspect								+cx;
			ret.y = 								v.y/(v.z+sz)	*sz /sc *height		+cy;
			ret.z = 															1/(v.z+sz);
			return ret;
		#else
			double w = 1/(v.z+sz);
			return v * mat44( 
				w *sz /sc *width  *aspect	,	0					,	0	,	0,
				0							,	w *sz /sc *height	,	0	,	0,
				0							,	0					,	0	,	0,
				cx							,	cy					,	w	,	0
			);
		#endif
		}
		
		//--------------------------------------------------------------------------
		bool ScissorLine( vect3 v0, vect3 v1, vect3& va, vect3& vb ) const
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
					if ( c.z > 1.0-sz )
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
