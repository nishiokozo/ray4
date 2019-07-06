
class	Win
{
public:
	struct
	{
		int width;
		int	height;
	} m;
	
	Win( const char* name, int pos_x, int pos_y, int width, int height  );
	virtual ~Win();

	bool Exec();

	void	Bezier( double x0, double y0, double x1, double y1, double x2, double y2, double x3, double y3, int col);
	void	Tri( double x0, double y0, double x1, double y1, double x2, double y2, int col);
	void	Line( double x0, double y0, double x1, double y1, int col);
	void	Pset( double x0, double y0, int col);
	void	Circle( double x, double y, double r, int col );
	void	Clr( int col );
	int 	Rgb(double r, double g , double b );
};
