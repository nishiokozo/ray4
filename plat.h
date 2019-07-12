class Plat
{
public:
	struct
	{
		int x;
		int	y;
		int width;
		int	height;
	} m;

	Plat( const char* name, int pos_x, int pos_y, int width, int height  );
	bool	Update();

	SysWin	win;
	SysKeys&	keys = SysKeys::GetInstance();
	SysMouse&	mouse = SysMouse::GetInstance();


};
