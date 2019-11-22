struct Lab
{
	vector<Obj*> tblObj;
	vector<Edge*> tblEdge;
	bool	bInitParam = false;
	bool	bInitAll = false;
	int		idx = 0;	

	int idxTbl;
	
	Lab();


	//------------------------------------------------------------------------------
	~Lab()
	//------------------------------------------------------------------------------
	{
		for ( Obj* p : (*this).tblObj ) delete p;
		for ( Edge* p : (*this).tblEdge ) delete p;
	}


	//------------------------------------------------------------------------------
	void SetIdx( int n )
	//------------------------------------------------------------------------------
	{
		idx = max( n, 0 ); 
		bInitParam=false;
		bInitAll=false;

		for ( Obj* p : (*this).tblObj ) delete p;
		tblObj.clear();

		for ( Edge* p : (*this).tblEdge ) delete p;
		tblEdge.clear();
	}

	//------------------------------------------------------------------------------
	void Update( SysKeys& keys, SysMouse& mouse, SysGra& gra, Pers& pers, float delta, int& text_y );
	//------------------------------------------------------------------------------

	void drawVect( SysGra& gra, Pers& pers, int& text_y, vect3 v0, vect3 v, float sc, rgb col, string str );

	void lab0_gridonly( Lab& lab, SysKeys& keys, SysMouse& mouse, SysGra& gra, Pers& pers, float delta, int& text_y );
	void lab12_RidgePlateDot( Lab& lab, SysKeys& keys, SysMouse& mouse, SysGra& gra, Pers& pers, float delta, int& text_y );
	void lab13( Lab& lab, SysKeys& keys, SysMouse& mouse, SysGra& gra, Pers& pers, float delta, int& text_y );

};
