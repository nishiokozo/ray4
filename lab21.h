struct Lab21 : LabObj
{
	void Update( SysKeys& keys, SysMouse& mouse, SysSound& sound, SysGra& gra, Pers& pers, float delta, int& text_y, Cp& cp );

	Lab21();
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};
