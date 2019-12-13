
#define	MAX_CH	3
struct MML_INF
{
	int		hz;
	int		vol;
	int		lim;
};
struct MML
{
	int	ch;
	int	flgRequest;
	int	flgPlaying;
	int	cnt;
	int	len;
	MML_INF	 inf[1024];

	void	Get( int i, int* lim, int* vol, int* hz )
	{
		*lim = inf[i].lim;
		*vol = inf[i].vol;
		*hz = inf[i].hz;
	}

	void	Add( int lim, int vol, int hz ) 
	{
		inf[len].lim = lim;
		inf[len].vol = vol;
		inf[len].hz = hz;
		len++;
	}
};

class	SysSound
{
public:

//	double	m_volume;

	SysSound();
	~SysSound();


	void	SetVolume( float val );

	int entry( const char* name );
	void play( int num );
	void stop( int num );
	void playRialtime( int num );
	void psg_Set( int num, int val );
	int 	psg_Get	( int num );

	void	Ring( int n );
	void	Beep();

	void	mml_play( const char*str );
	void	mml_wait();

	void info();



};
