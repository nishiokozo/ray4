class Sys
{
public:

	bool	Update();

	void  SetWincursor( bool flg );


	void InitWinapi(
		function<void()> funcOnCreate,
		function<void( int width, int height )> funcOnSize,
		function<void()> funcOnPaint,
		function<void()> funcDestroy
	);
	void OpenWindow( const char* name, int pos_x, int pos_y, int width, int height  );
//	void SetOnPaint( function<void()> func );
//	void SetOnSize( function<void( int width, int height )> func );
//	void SetOnDestroy( function<void()> func );
//	void SetOnCreate( function<void()> func );


};
