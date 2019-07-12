class	SysWin
{
	SysWin();
	virtual ~SysWin();

public:
	struct	//	WIN
	{
		WNDCLASSEX		tWndClass;
		HINSTANCE		hInstance;
		const CHAR*		cpClassName;
		const CHAR*		cpWindowName;
		CHAR*			cpMenu;
		MSG				tMsg;
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
	void SetOnShowwindow( function<void()> func );
	void SetOnPaint( function<void()> func );


	static LRESULT CALLBACK WinProc
	(
		  HWND		hWnd
		, UINT		uMsg
		, WPARAM	wParam	//メッセージの付加情報
		, LPARAM	lParam	//メッセージの付加情報
	);


};

//extern SysWin win;
