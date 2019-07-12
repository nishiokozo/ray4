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

	SysGdi gdi;

	SysGra		draw;
	SysKeys&	keys = SysKeys::GetInstance();
	SysMouse&	mouse = SysMouse::GetInstance();


};
