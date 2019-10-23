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

	void vector_six_lab8( SysKeys& keys, SysMouse& mouse, SysGra& gra, Pers& pers, int& text_y );

	void tire3d( SysKeys& keys, SysMouse& mouse, SysGra& gra, Pers& pers, int& text_y );
	void furiko2d( SysKeys& keys, SysMouse& mouse, SysGra& gra, Pers& pers, int& text_y );
	void furiko3d( SysKeys& keys, SysMouse& mouse, SysGra& gra, Pers& pers, int& text_y );
	void gravityPlanet( SysKeys& keys, SysMouse& mouse, SysGra& gra, Pers& pers, int& text_y );
	void kakusokudo( SysKeys& keys, SysMouse& mouse, SysGra& gra, Pers& pers, int& text_y );
	void kakusokudo7( SysKeys& keys, SysMouse& mouse, SysGra& gra, Pers& pers, int& text_y );
	void graph( SysKeys& keys, SysMouse& mouse, SysGra& gra, Pers& pers, int& text_y );
	void Update( SysKeys& keys, SysMouse& mouse, SysGra& gra, Pers& pers, int& text_y );

};
