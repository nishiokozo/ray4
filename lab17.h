struct Lab17 : LabObj
{
	Lab17();
	void Update( SysKeys& keys, SysMouse& mouse, SysGra& gra, Pers& pers, float delta, int& text_y, Cp& cp );

    struct Impl;
    std::unique_ptr<Impl> pImpl;
};