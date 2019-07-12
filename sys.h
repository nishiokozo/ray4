#include <windows.h>

#include <functional>




class	Sys
{
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


	Sys();
	virtual ~Sys();

	bool	Update();

	void OpenWindow( const char* name, int pos_x, int pos_y, int width, int height  );

	void SetOnSize( function<void(HWND)> func );
	void SetOnDestroy( function<void(HWND)> func );
	void SetOnShowwindow( function<void(HWND)> func );
	void SetOnPaint( function<void(HWND)> func );


	static LRESULT CALLBACK WinProc
	(
		  HWND		hWnd
		, UINT		uMsg
		, WPARAM	wParam	//メッセージの付加情報
		, LPARAM	lParam	//メッセージの付加情報
	);

};
