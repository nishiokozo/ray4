struct Lab10 : LabObj
{
	void Update( SysKeys& keys, SysMouse& mouse, SysSound& sound, SysGra& gra, Pers& pers, float delta, int& text_y, Cp& cp );

	Lab10();
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};
