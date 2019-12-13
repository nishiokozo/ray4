struct Lab19 : LabObj
{
	Lab19();
	void Update( SysKeys& keys, SysMouse& mouse, SysSound& sound, SysGra& gra, Pers& pers, float delta, int& text_y, Cp& cp );

    struct Impl;
    std::unique_ptr<Impl> pImpl;
};
