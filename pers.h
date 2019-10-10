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
	#if 1
		vect3	pos = vect3(  0.5, 1.5, -3+0.1 );
		vect3 	at = vect3( 0,  1, 0 );
	#else
		vect3	pos = vect3(  0.5, 1.5, -1+0.1 );
		vect3 	at = vect3( 0,  1.5, 0 );
	#endif
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
		void Zoom( float step )
		//------------------------------------------------------------------------------
		{
			vect3	v= vect3(0,0,step);
			mat44 mrot = mat;
			mrot.SetTranslate(vect3(0,0,0));
			v = v* mrot;

			vect3 r = pos;
			pos += v;
			if( (pos-at).abs() <= v.abs() ) pos = (r-at).normalize()*0.00001+at;
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


};



extern void g_pset3d( SysGra& gra, Pers& pers, vect3 p0, rgb col, float wide = 1.0f );
extern void g_line3d( SysGra& gra, Pers& pers, vect3 p0, vect3 p1, rgb col, float wide = 1.0f );
extern void g_line3d_scissor( SysGra& gra, Pers& pers, vect3 p0, vect3 p1, rgb col, float wide = 1.0f );
extern void g_print3d( SysGra& gra, Pers& pers, vect3 p0, float x, float y, string str );
extern void g_pset2d( SysGra& gra, Pers& pers, vect2 p0, rgb col, float wide = 1.0f );
extern void g_print2d( SysGra& gra, Pers& pers, vect2 p0, string str );
extern void g_line2d( SysGra& gra, Pers& pers, vect2 p0, vect2 p1, rgb col, float wide=1.0f );
