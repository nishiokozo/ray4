struct Lab;
struct LabObj
{
	LabObj(){};
	virtual ~LabObj(){};
	virtual void Update( Lab& lab, SysKeys& keys, SysMouse& mouse, SysGra& gra, Pers& pers, float delta, int& text_y, Cp& cp ) = 0;
};

struct Lab
{
	vector<Obj*> tbl_pObj;
	vector<Edge*> tbl_pEdge;
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
		for ( Obj* p : (*this).tbl_pObj ) delete p;
		for ( Edge* p : (*this).tbl_pEdge ) delete p;
	}


	void SetIdx( int n, Cp& cp );
	void Update( SysKeys& keys, SysMouse& mouse, SysGra& gra, Pers& pers, float delta, int& text_y, Cp& cp );
	void drawVect( SysGra& gra, Pers& pers, int& text_y, vect3 v0, vect3 v, float sc, rgb col, string str );

};
