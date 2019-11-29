struct Lab;
struct LabObj
{
	struct M
	{
		bool			bInitParam = false;
		bool			bInitAll = false;
		bool			bPause = false;
		bool			bStep = false;
		vector<Obj*>	tbl_pObj;
		vector<Edge*>	tbl_pEdge;
		void			drawVect( SysGra& gra, Pers& pers, int& text_y, vect3 v0, vect3 v, float sc, rgb col, string str );
	} m;

	LabObj(){};
	virtual ~LabObj(){};
	virtual void Update( SysKeys& keys, SysMouse& mouse, SysGra& gra, Pers& pers, float delta, int& text_y, Cp& cp ) = 0;


	struct	Graphs
	{
		struct Plot
		{
			//	y = 0面に座標(0,0,0)を中心にグラフ（ドット）を打ってゆく

			static const int MaxPlot = 100;
			float tblPlot[MaxPlot];
			int cntPlot = 0;
			bool	bScroll = false;
			rgb col;
			float step;
			
			//------------------------------------------------------------------------------
			Plot( 
			//------------------------------------------------------------------------------
				float _step, 			// 送りステップ
				rgb _col = rgb(1,1,1) 	// 色
			)
			{
				col = _col;
				step = _step;
				for ( int i = 0 ; i < MaxPlot ; i++ )
				{
					tblPlot[i] = 0;
				}
				ResetPlot();
			}

			//------------------------------------------------------------------------------
			void ResetPlot()
			//------------------------------------------------------------------------------
			{
				bScroll = false;
				cntPlot = 0 ;
			}
			//------------------------------------------------------------------------------
			void WritePlot( float val )
			//------------------------------------------------------------------------------
			{
				if ( cntPlot >= MaxPlot ) 
				{
					bScroll = true;
					cntPlot = 0;
				}

				tblPlot[ cntPlot++ ] = val;
			}
			//------------------------------------------------------------------------------
			void DrawPlot( SysGra& gra, Pers& pers )
			//------------------------------------------------------------------------------
			{
				gra.SetZTest( false );
				
				float x = 0;
				
				if ( bScroll )
				for ( int i = cntPlot ; i < MaxPlot ; i++ )
				{
					pers.pen.pset3d( gra, pers, vect3( x, 0, tblPlot[i] ),  col, 2 );
					x += step;
				}
				for ( int i = 0 ; i < cntPlot-1 ; i++ )
				{
					pers.pen.pset3d( gra, pers, vect3( x, 0, tblPlot[i] ),  col, 2 );
					x += step;
				}
				if ( cntPlot > 0 ) pers.pen.pset3d( gra, pers, vect3( x, 0, tblPlot[cntPlot-1] ),  rgb(1,1,1), 2 );

				gra.SetZTest( true );
			}

		};

		vector<Plot>	plots;

		//------------------------------------------------------------------------------
		int Create(
		//------------------------------------------------------------------------------
			float _step, 			// 送りステップ
			rgb _col = rgb(1,1,1) 	// 色
		)
		{
			int idx = (signed)plots.size(); 
			plots.emplace_back( _step, _col );
			return idx;
		}

		//------------------------------------------------------------------------------
		void Clear()
		//------------------------------------------------------------------------------
		{
			plots.clear();
		}
		
		//------------------------------------------------------------------------------
		void Request( int idx, float val )
		//------------------------------------------------------------------------------
		{
			if ( idx >= (signed)plots.size() ) return;
			plots[ idx ].WritePlot( val );
		}

		//------------------------------------------------------------------------------
		void Draw( SysGra& gra, Pers& pers )
		//------------------------------------------------------------------------------
		{
			for ( Plot& p : plots )
			{
				p.DrawPlot( gra, pers );
			}
		}
	};

};

struct Lab
{
	struct M
	{
		int	idxLab = 0;	
		int	idxTbl = 0;
	} m;
	
	Lab();

	LabObj*	pLab = 0;

	void SetNextIdx( int val, Cp& cp );
	void SetIdx( int n, Cp& cp );
	void Update( SysKeys& keys, SysMouse& mouse, SysGra& gra, Pers& pers, float delta, int& text_y, Cp& cp );

};
