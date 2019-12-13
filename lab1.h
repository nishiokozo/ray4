struct Lab1 : LabObj
{
    struct Impl;
    std::unique_ptr<Impl> pImpl;
	Lab1();
	void Update( SysKeys& keys, SysMouse& mouse, SysSound& sound, SysGra& gra, Pers& pers, float delta, int& text_y, Cp& cp );
};
