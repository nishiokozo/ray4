struct Lab;
struct LabPart
{
	LabPart();
	virtual ~LabPart();
	void Update( Lab& lab, SysKeys& keys, SysMouse& mouse, SysGra& gra, Pers& pers, float delta, int& text_y );
};

struct Lab
{
	vector<Obj*> tblObj;
	vector<Edge*> tblEdge;
	bool	bInitParam = false;
	bool	bInitAll = false;
	int		idx = 0;	

	int idxTbl;
	
	Lab();

	LabPart*	pLab14 = 0;

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


		if ( idx == 14 )
		{	
			if ( !pLab14 )
			{
				pLab14 = new LabPart;
			}
		}
		else
		{
			if ( pLab14 ) 
			{
				delete pLab14;
				pLab14=0;
			};
		}
	}

	//------------------------------------------------------------------------------
	void Update( SysKeys& keys, SysMouse& mouse, SysGra& gra, Pers& pers, float delta, int& text_y );
	//------------------------------------------------------------------------------

	void drawVect( SysGra& gra, Pers& pers, int& text_y, vect3 v0, vect3 v, float sc, rgb col, string str );

	void lab0_gridonly( Lab& lab, SysKeys& keys, SysMouse& mouse, SysGra& gra, Pers& pers, float delta, int& text_y );
	void lab12_RidgePlateDot( Lab& lab, SysKeys& keys, SysMouse& mouse, SysGra& gra, Pers& pers, float delta, int& text_y );
	void lab13_intersect_plate_curve( Lab& lab, SysKeys& keys, SysMouse& mouse, SysGra& gra, Pers& pers, float delta, int& text_y );

//	void lab14( Lab& lab, SysKeys& keys, SysMouse& mouse, SysGra& gra, Pers& pers, float delta, int& text_y );

};
