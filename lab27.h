struct Lab27 : LabObj
{
	void Update( SysKeys& keys, SysMouse& mouse, SysSound& sound, SysGra& gra, Pers& pers, float delta, int& text_y, Cp& cp );

	Lab27();
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};
