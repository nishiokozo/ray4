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
	int GetHeight();
	int GetWheel();

	bool	Update();

	void InitWinapi();
	void OpenWindow( const char* name, int pos_x, int pos_y, int width, int height  );
	void SetTitle( std::string name );

	void SetOnCreate( std::function<void()> func );
	void SetOnSize( std::function<void( int width, int height )> func );
//	void SetOnMove( function<void( int pos_x, int pos_y )> func );
	void SetOnPaint( std::function<void()> func );
	void SetOnDestroy( std::function<void()> func );


};
