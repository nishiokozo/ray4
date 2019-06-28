	struct	Line
	{
		double	x0;
		double	y0;
		double	x1;
		double	y1;
		int		col;
	};

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

	bool exec();
	unsigned char* GetAddrPixels();
	int GetBytePixels();



	void	line( double x0, double y0, double x1, double y1, int col);
	int		rgb(double r, double g , double b );
};
