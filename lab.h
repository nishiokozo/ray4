struct Lab;
struct LabObj
{
	struct M
	{
		bool			bInitParam = false;
		bool			bInitAll = false;
		bool			bPause = false;
		bool			bStep = false;
		vector<shared_ptr<Obj>>	tbl_pObj;
		vector<shared_ptr<Edge>>	tbl_pEdge;
		void			drawVect( SysGra& gra, Pers& pers, int& text_y, vect3 v0, vect3 v, float sc, rgb col, string str );
	} m;

	LabObj(){};
	virtual ~LabObj(){};
	virtual void Update( SysKeys& keys, SysMouse& mouse, SysGra& gra, Pers& pers, float delta, int& text_y, Cp& cp ) = 0;
};

struct Lab
{
	struct M
	{
		int	idxLab = 0;	
		int	idxTbl = 0;
	} m;
	
	unique_ptr<LabObj>	pLab = 0;

	void SetNextIdx( int val, Cp& cp );
	void SetIdx( int n, Cp& cp );
	void Update( SysKeys& keys, SysMouse& mouse, SysGra& gra, Pers& pers, float delta, int& text_y, Cp& cp );

};
