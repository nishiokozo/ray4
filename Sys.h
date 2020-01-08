class Sys
{
public:

	bool	Update();

	void  SetWincursor( bool flg );


	void InitWinapi(
		std::function<void()> funcOnCreate,
		std::function<void( int width, int height )> funcOnSize,
		std::function<void()> funcOnPaint,
		std::function<void()> funcDestroy
	);
	void OpenWindow( const char* name, int pos_x, int pos_y, int width, int height  );
//	void SetOnPaint( std::function<void()> func );
//	void SetOnSize( std::function<void( int width, int height )> func );
//	void SetOnDestroy( std::function<void()> func );
//	void SetOnCreate( std::function<void()> func );


};
