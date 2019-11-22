struct Lab;
struct LabObj
{
	LabObj(){};
	virtual ~LabObj(){};
	virtual void Update( Lab& lab, SysKeys& keys, SysMouse& mouse, SysGra& gra, Pers& pers, float delta, int& text_y, Cp& cp ) = 0;
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

	LabObj*	pLab = 0;

	//------------------------------------------------------------------------------
	~Lab()
	//------------------------------------------------------------------------------
	{
		for ( Obj* p : (*this).tblObj ) delete p;
		for ( Edge* p : (*this).tblEdge ) delete p;
	}


	void SetIdx( int n );

	//------------------------------------------------------------------------------
	void Update( SysKeys& keys, SysMouse& mouse, SysGra& gra, Pers& pers, float delta, int& text_y, Cp& cp );
	//------------------------------------------------------------------------------

	void drawVect( SysGra& gra, Pers& pers, int& text_y, vect3 v0, vect3 v, float sc, rgb col, string str );

	void lab0_gridonly( Lab& lab, SysKeys& keys, SysMouse& mouse, SysGra& gra, Pers& pers, float delta, int& text_y, Cp& cp );
	void lab12_RidgePlateDot( Lab& lab, SysKeys& keys, SysMouse& mouse, SysGra& gra, Pers& pers, float delta, int& text_y, Cp& cp );
	void lab13_intersect_plate_curve( Lab& lab, SysKeys& keys, SysMouse& mouse, SysGra& gra, Pers& pers, float delta, int& text_y, Cp& cp );

//	void lab14( Lab& lab, SysKeys& keys, SysMouse& mouse, SysGra& gra, Pers& pers, float delta, int& text_y, Cp& cp );

};
