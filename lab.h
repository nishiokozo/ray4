struct Lab;
struct LabObj
{
	LabObj(){};
	virtual ~LabObj(){};
	virtual void Update( SysKeys& keys, SysMouse& mouse, SysSound& sound, SysGra& gra, Pers& pers, float delta, int& text_y, Cp& cp ) = 0;
};

struct Lab
{
	struct M
	{
		int	idxLab = 0;	
		int	idxTbl = 0;
	} m;
	
	std::unique_ptr<LabObj>	pLab = 0;

	void SetNextIdx( int val, Cp& cp );
	void SetIdx( int n, Cp& cp );
	void Update( SysKeys& keys, SysMouse& mouse, SysSound& sound, SysGra& gra, Pers& pers, float delta, int& text_y, Cp& cp );

};
