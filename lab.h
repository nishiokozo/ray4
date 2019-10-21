struct Lab
{
	vector<Obj*> tblObj;
	bool bInit = false;
	bool bInitCam = false;
	int idx = 0;	
	
	Lab();


	//------------------------------------------------------------------------------
	~Lab()
	//------------------------------------------------------------------------------
	{
		for ( Obj* p : (*this).tblObj ) delete p;
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
	}

	void tire3d( SysKeys& keys, SysMouse& mouse, SysGra& gra, Pers& pers, int& text_y );
	void furiko2d( SysKeys& keys, SysMouse& mouse, SysGra& gra, Pers& pers, int& text_y );
	void furiko3d( SysKeys& keys, SysMouse& mouse, SysGra& gra, Pers& pers, int& text_y );
	void gravityPlanet( SysKeys& keys, SysMouse& mouse, SysGra& gra, Pers& pers, int& text_y );
	void kakusokudo( SysKeys& keys, SysMouse& mouse, SysGra& gra, Pers& pers, int& text_y );
	void kakusokudo7( SysKeys& keys, SysMouse& mouse, SysGra& gra, Pers& pers, int& text_y );
	void graph( SysKeys& keys, SysMouse& mouse, SysGra& gra, Pers& pers, int& text_y );
	void Update( SysKeys& keys, SysMouse& mouse, SysGra& gra, Pers& pers, int& text_y );

};
