struct Lab9 : LabObj
{
	void Update( SysKeys& keys, SysMouse& mouse, SysGra& gra, Pers& pers, float delta, int& text_y, Cp& cp );

	Lab9();
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};
