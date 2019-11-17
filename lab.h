struct Lab
{
	vector<Obj*> tblObj;
	vector<Edge*> tblEdge;
	bool bInit = false;
	bool bInitCam = false;
	int idx = 0;	

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
		bInit=false;
		bInitCam=false;

		for ( Obj* p : (*this).tblObj ) delete p;
		tblObj.clear();

		for ( Edge* p : (*this).tblEdge ) delete p;
		tblEdge.clear();
	}

	//------------------------------------------------------------------------------
	void Update( SysKeys& keys, SysMouse& mouse, SysGra& gra, Pers& pers, float delta, int& text_y );
	//------------------------------------------------------------------------------

	void lab12_RidgePlateDot( Lab& lab, SysKeys& keys, SysMouse& mouse, SysGra& gra, Pers& pers, float delta, int& text_y );

};
