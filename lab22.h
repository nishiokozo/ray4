struct Lab22 : LabObj
{
	void Update( SysKeys& keys, SysMouse& mouse, SysGra& gra, Pers& pers, float delta, int& text_y, Cp& cp );

	Lab22();
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};
