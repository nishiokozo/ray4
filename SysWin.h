class	SysWin
{
	SysWin();
	virtual ~SysWin();

public:
	struct	//	WIN
	{
		HWND			hWnd;
	} win;


	struct
	{
		int x;
		int	y;
		int width;
		int	height;
	} m;

	static SysWin&	GetInstance();
	

	bool	Update();

	void OpenWindow( const char* name, int pos_x, int pos_y, int width, int height  );

	void SetOnSize( function<void()> func );
	void SetOnDestroy( function<void()> func );
	void SetOnCreate( function<void()> func );
	void SetOnPaint( function<void()> func );


};
