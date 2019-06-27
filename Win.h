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
	unsigned char* getAddrPixels();
	int getBytePixels();

};
