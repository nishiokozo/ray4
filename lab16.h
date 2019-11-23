struct Lab16 : LabObj
{
	Lab16();
	~Lab16();

	void Update( Lab& lab, SysKeys& keys, SysMouse& mouse, SysGra& gra, Pers& pers, float delta, int& text_y, Cp& cp );

    class Lab16Impl* pImpl; 

};
