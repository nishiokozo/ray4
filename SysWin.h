class	SysWin
{
	SysWin();
	virtual ~SysWin();

public:
	struct	//	WIN
	{
		HWND			hWnd;
	} win;



	static SysWin&	GetInstance();
	
	int GetPosX();
	int GetPosY();
	int GetWidth();
	int GetHeigit();
	int GetWheel();

	bool	Update();

	void OpenWindow( const char* name, int pos_x, int pos_y, int width, int height  );

	void SetOnCreate( function<void()> func );
	void SetOnSize( function<void( int width, int height )> func );
	void SetOnMove( function<void( int pos_x, int pos_y )> func );
	void SetOnPaint( function<void()> func );
	void SetOnDestroy( function<void()> func );


};
