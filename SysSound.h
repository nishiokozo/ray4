
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

	SysSound();
	~SysSound();

	void	SetVolume( float val );

	void	psg_Set( int num, int val );
	int 	psg_Get	( int num );

	void	mml_wait();
	void	mml_play( const char*str ); // 最後がRが必要 Oは0~7 音長は0~9
	void info();

};
