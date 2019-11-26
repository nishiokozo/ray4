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

};

struct Lab
{
	int		idx = 0;	

	int idxTbl;
	
	Lab();

	LabObj*	pLab = 0;

	void SetIdx( int n, Cp& cp );
	void Update( SysKeys& keys, SysMouse& mouse, SysGra& gra, Pers& pers, float delta, int& text_y, Cp& cp );

};
