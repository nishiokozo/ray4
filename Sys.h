class Sys
{
public:
//	struct
//	{
//		int pos_x;
//		int	pos_y;
//		int width;
//		int	height;
//	} m;

	Sys( const char* name, int pos_x, int pos_y, int width, int height  );
	bool	Update();

	SysGra gra;
	SysKeys&	keys = SysKeys::GetInstance();
	SysMouse&	mouse = SysMouse::GetInstance();


};
