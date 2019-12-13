//#include <sys.h>
#include <string>
#include <stdio.h>
#include <windows.h>
	#include <memory.h>
	#include <stdio.h>
#include	<cmath>
#include	<iostream>

using namespace std;


#include "SysSound.h"

static	int	g_tblFeq[8][12];
static	double	g_volume = 0;


#define	USE_DIRECTOUT	1

#if USE_DIRECTOUT == false
	int	psg_flg_WOM_DONE;
#endif

extern	void psg_Set2	( int num, int val );

#define SAMPLING_RATE 	44100		// 44.1Khz  byte / sec
//#define SAMPLING_RATE 	48000		// 44.1Khz  byte / sec
#define	SAMPLING_BITS	16
#if	SAMPLING_BITS == 16
	#define	SAMPLING_MAX 	32767
	#define	SAMPLING_ZERO 	0
	#define	SAMPLING_MIN	-32767
#else
	#define	SAMPLING_MAX	255
	#define	SAMPLING_ZERO	128
	#define	SAMPLING_MIN	0
#endif

#define	PSG_CLOCK	(2.0*1000*1000)	//	2Mhz
//#define	PSG_CLOCK	(1.7897725*1000*1000)	//	1.7897725Mhz	


	#define	MML_DIV	64
	//									   全
	//						0	1	2	3	4	5	6	7	8	9
	int	mml_tblLim[10] = 
	{
		MML_DIV*1,
		MML_DIV*2,
		MML_DIV*3,
		MML_DIV*4,
		MML_DIV*6,
		MML_DIV*8,
		MML_DIV*12,
		MML_DIV*16,
		MML_DIV*24, 
		MML_DIV*32
	};

	HANDLE g_mml_hdlThread = 0;
	int	mml_tempo = 120;

//	static	int g_thread_cnt = 0;
	int	g_mml_flg = true;
	int	g_sound_flg = true;
	int	g_fin = false;
	HANDLE g_sound_hdlThread = 0;
//	static	double	thread_st = 0;
//	static	double	thread_tm = 0;

class Wave
{
public:
//c	const static	int	LEN_BUFFER = 	(735+735);
	const static	int	LEN_BUFFER = 	(SAMPLING_RATE/30);
	int numBack;
#if	SAMPLING_BITS == 16
	short bufWaveB[2][ LEN_BUFFER ];
#else
	char bufWaveB[2][ LEN_BUFFER ];
#endif
	WAVEHDR whdrB[2];

	HWAVEOUT hWaveB;

	Wave()
	{
		numBack = 0;
		hWaveB = 0;
	}

};

Wave	g_wave;

#if	USE_DIRECTOUT == false
class Ringbuf
{
 	const	static int LEN_RINGBUF = 10000;
	int		ringbuf_len;
	int		ringbuf_st;
	int		ringbuf_en;
   	double	ringbuf[LEN_RINGBUF];
	int	i;

public:
	//---------------------------------
	Ringbuf()
	//---------------------------------
	{
			ringbuf_len = 0;
			ringbuf_st = 0;
			ringbuf_en = 0;
			i = 0;
	}

	//---------------------------------
	void	PrepareLoop()
	//---------------------------------
	{	
		i = 0;	
	}

	//---------------------------------
	int		IsLoop()
	//---------------------------------
	{
		return (ringbuf_len > 0);
	}

	//---------------------------------
	int		Length()
	//---------------------------------
	{	
		return ringbuf_len;	
	}

	//---------------------------------
	void	NextLoop()
	//---------------------------------
	{
		i++;
		ringbuf_st++;	
		if ( ringbuf_st >= Ringbuf::LEN_RINGBUF ) ringbuf_st -= Ringbuf::LEN_RINGBUF;
		ringbuf_len--;
	}

	//---------------------------------
	int		GetIndex()
	//---------------------------------
	{ 
		return i; 
	}

	//---------------------------------
	double	GetValue()
	//---------------------------------
	{
		return ringbuf[ringbuf_st];
	}

	//---------------------------------
	double	GetValue( int idx )
	//---------------------------------
	{

		idx += ringbuf_st;
		if ( idx >= Ringbuf::LEN_RINGBUF ) idx -= Ringbuf::LEN_RINGBUF;

		return ringbuf[idx];
	}

	//---------------------------------
	void	Add( double val )
	//---------------------------------
	{
		ringbuf[ringbuf_en] = val;
		ringbuf_en++;
	    ringbuf_len++;
		if ( ringbuf_en >= LEN_RINGBUF )
		{
			ringbuf_en = 0;
		}
	}



};
#endif
struct psg_Tone
{
	int	ValPeriod;
	int	Vol4bit;	//(0~15)
	int	FlgEnv;
	int	FlgTone;
	int	FlgNoise;

	int	Limit;
	int	FlgHigh;

};
struct psg_Envelope
{
	int	Limit;
	int	Vol4bit;	//(0~15)

	int	ValPeriod;
	int	psg_wave;

	int	FlgHigh;

	int		FlgHold;
	int		FlgAlternate;
	int		FlgAttack;
	int		FlgContinue;

};
struct psg_Noise
{
	int	Limit;
	int	FlgHigh;
	int	valGenerated;

	int	ValPeriod;
};

static 	psg_Tone		psg_A;
static 	psg_Tone		psg_B;
static 	psg_Tone		psg_C;
static 	psg_Noise		psg_N;
static 	psg_Envelope	psg_E;
#if	USE_DIRECTOUT == false
static 	Ringbuf			psg_out;
//static 	Ringbuf			psg_da_A;
//static 	Ringbuf			psg_da_B;
//static 	Ringbuf			psg_da_C;
#endif//static	int	psg_mixer;

//extern  Sys	sys;

#if 0
double	g_tblDA[16] = 
{
	0,			//0
	0.125 /16,	//1
	0.1515/16,	//2
	0.125 /8,	//3
	0.1515/8,	//4
	0.125 /4,	//5
	0.1515/4,	//6
	0.125 /2,	//7
	0.1515/2,	//8
	0.125,	//9
	0.1515,	//10
	0.25,	//11
	0.303,	//12
	0.5,	//13
	0.707,	//14
	1.0,	//15
};
#endif

#if 0
double	g_tblDA[16] = 
{//**0.8
	0,
	0.043980 * 0.2,
	0.054976 * 0.3,
	0.068719 * 0.4,
	0.085899 * 0.5,
	0.107374 * 0.6,
	0.134218 * 0.7,
	0.167772 * 0.8,
	0.209715,
	0.262144,
	0.327680,
	0.409600,
	0.512000,
	0.640000,
	0.800000,
	1.000000,

};
#endif
#if 1
double	g_tblDA[16] = 
{//**0.8
	0,
	0.200000 * 0.1,
	0.200000 * 0.2,
	0.200000 * 0.3,
	0.200009 * 0.4,
	0.200000 * 0.5,
	0.200000 * 0.6,
	0.200000,
	0.300000,
	0.400000,
	0.500000,
	0.600000,
	0.700000,
	0.800000,
	0.900000,
	1.000000,

};
#endif

//-----------------------------------------------------------------------------
SysSound::~SysSound()
//-----------------------------------------------------------------------------
{
//	printf("sound end %d\n", __LINE__ );
	g_sound_flg = false;
	

	if ( g_mml_hdlThread )	 CloseHandle(g_mml_hdlThread)   ;
//	printf("sound end %d\n", __LINE__ );
	if ( g_sound_hdlThread )	 CloseHandle(g_sound_hdlThread)   ;
//	printf("sound end %d\n", __LINE__ );

	if ( g_wave.hWaveB )
	{
//	printf("sound end %d\n", __LINE__ );
//		waveOutReset(g_wave.hWaveB);
		waveOutUnprepareHeader(g_wave.hWaveB , &g_wave.whdrB[0] , sizeof(WAVEHDR));
		waveOutUnprepareHeader(g_wave.hWaveB , &g_wave.whdrB[1] , sizeof(WAVEHDR));
		waveOutClose(g_wave.hWaveB);
		g_wave.hWaveB = 0;
	}
//	printf("sound end %d\n", __LINE__ );

}

//----------------------------------------------------------------------
void	psg_NOISE_GEN( psg_Noise& noize )
//----------------------------------------------------------------------
{
		if ( noize.Limit <= noize.ValPeriod/2 )
		{
			noize.FlgHigh = rand()&1;
		}
		if ( noize.Limit <= 0 )
		{
			noize.Limit = noize.ValPeriod;
//			noize.FlgHigh = rand()&1;
		}
		noize.Limit--;


}
//----------------------------------------------------------------------
void	psg_ENVELOPE_GEN( psg_Envelope& env )
//----------------------------------------------------------------------
{
		env.Limit--;
		if ( env.Limit <= 0 )
		{
			switch(env.psg_wave)
			{
			case 0:
			case 1:
			case 2:
			case 3:				env.Limit = 0;				break;	//	下り坂単発
			case 8:				env.Limit = env.ValPeriod;	break;	//	下り坂型繰り返し
			default:
				env.Limit = 0;
			}
		}
		if ( env.Limit > env.ValPeriod ) env.Limit = env.ValPeriod;

		env.Vol4bit = (int)( 15.0f * (double(env.Limit) / double(env.ValPeriod)) );


}
//----------------------------------------------------------------------
void	psg_TONE_GEN( psg_Tone& tone )
//----------------------------------------------------------------------
{
	if ( tone.Limit <= tone.ValPeriod/2 )
	{
		tone.FlgHigh = !tone.FlgHigh;
	}
	if ( tone.Limit <= 0 )
	{
		tone.Limit = tone.ValPeriod;
//		tone.FlgHigh = !tone.FlgHigh;
	}
	tone.Limit--;


}
#if	USE_DIRECTOUT == false
//----------------------------------------------------------------------
void	psg_Extend( double * bufout, Ringbuf& ringbuf, double rate )
//----------------------------------------------------------------------
{
	double amt = 0;
	int cnt = 0;
	int	to1 = 0;

	for ( ringbuf.PrepareLoop() ; ringbuf.IsLoop() ; ringbuf.NextLoop() )
	{
		int	to2 = ringbuf.GetIndex() * rate;

		if ( to2 >= Wave::LEN_BUFFER ) 
		{
			break;
		}

		if ( to2 != to1 && cnt > 0 ) 
		{

			bufout[to1] = amt/cnt;

			cnt = 0;
			amt = 0;
		}

		amt += ringbuf.GetValue();
		cnt++;

		to1 = to2; 
	}


}
/*
//----------------------------------------------------------------------
void psg_DA_CONVERTER( Ringbuf& ringbuf, bool a, int Vol4bit )
//----------------------------------------------------------------------
{
	double vol = g_tblDA[ Vol4bit&0xf ];
	ringbuf.Add( a ? vol:0 );
}
*/
#endif

bool flgUseTbl = true;

MML	tblMML[MAX_CH]=
{
	{0,false,false,0,0,{}},
	{1,false,false,0,0,{}},
	{2,false,false,0,0,{}}
};

//----------------------------------------------------------------------
void	mml_se( MML& mml )
//----------------------------------------------------------------------
{
	int	hz,lim,vol;
	mml.Get( mml.cnt, &lim, &vol, &hz );
			

	if ( hz != 0 ) 
	{
		int f = 125000/2/hz;

		int	h = (f>>8)&0xf;
		int	l = f&0xff;

//		printf(" %d: %5dHz vol %2d lim %3d\n" , mml.ch, hz, vol, lim );

		psg_Set2(7,0b111000);
		psg_Set2( 0+mml.ch*2,l);
		psg_Set2( 1+mml.ch*2,h);
		psg_Set2( 8+mml.ch, vol);
	}

}

static	int mml_cnt = 1000;
//----------------------------------------------------------------------
void	mml_playch( MML& mml )
//----------------------------------------------------------------------
{
//		printf(" mml.cnt %d\n" , mml.cnt );
					if ( mml.cnt < mml.len )
					{
						MML_INF&	inf = mml.inf[mml.cnt];

						if ( mml.flgRequest == true )
						{
							mml.flgRequest = false;
	//	printf("--- st %d en\n",mml.ch );
mml_cnt = 0;

							mml_se( mml );
						}

						inf.lim--;
						if ( inf.lim <= 0 ) 
						{
							mml.cnt++;
							if ( mml.cnt < mml.len )
							{
								mml_se( mml );
							}
							else
							{
	//	printf("--- ch %d en\n",mml.ch );
mml_cnt = 1000;
								psg_Set2( 8+mml.ch,0);
								mml.flgPlaying = false;
							}
						}
					}
}

/*

//----------------------------------------------------------------------
void	mml_thread()
//----------------------------------------------------------------------
{
	static	double	st = 0;
	static	double	tm = 0;

	while(1)
	{
		tm = sys.getPerformanceCounter();

		if ( st + (60.0/mml_tempo)/mml_tblLim[5] < tm )
		{
printf("cnt %d %f %d\n",++mml_cnt, tm-st, mml_tempo);
			st = tm;

			for ( int ch = 0 ; ch < MAX_CH ; ch++ )
			{
				MML&		mml = tblMML[ch];

				mml_playch( mml );
				
			}

		}
	}
}
*/
//----------------------------------------------------------------------
void	SysSound::Beep()
//----------------------------------------------------------------------
{
/*
	int	a3 = psg_Get(7);

		psg_Set(7,0b111000);
		mml_play( "T60V15o5E0R" );
		PutSleep( 0.1 );


	psg_Set(7,a3);
*/

}
//----------------------------------------------------------------------
void	SysSound::SetVolume( float val )
//----------------------------------------------------------------------
{
	g_volume = val;
}
//----------------------------------------------------------------------
void	SysSound::mml_wait()
//----------------------------------------------------------------------
{
				for ( int ch = 0 ; ch < MAX_CH ; ch++ )
				{
					MML&		mml = tblMML[ch];

					while( mml.flgPlaying ) {}
				}
}
//----------------------------------------------------------------------
void	SysSound::mml_play( const char*str ) // 最後がRが必要 Oは0~7 音長は0~9
//----------------------------------------------------------------------
{

//	const char*	str = "T100v12o4c3-g#dcfc#dcc7R:V12O3#D5F#GGG7R:V12O2G7GGR";
//	const char*	str = "T100v12o4c3-g#dcfc#dcc7R:V12O3#D5F#GGG7R:";
//	const char*	str = "T100v12o4c3-g#dcfc#dcc7R";
//	const char*	str = "T100v12o3c3dcdcdr:v12o4e3fefefr";
//	const char*	str = "T100v12o3c3dcdcdcdcdcdcdr:";
//	const char*	str = "V12O1#D5F#GGG7R";
	char	strToken[100];
	char	strNum[100];
//	char	s3[100];
	int		lenToken = 0;
	int		lenNum = 0;
//	int		l3 = 0;
	bool	flgFade = false;
	bool	flgOto = false;


	{
		for ( int i = 0 ; i < MAX_CH ; i++ )
		{
//			tblMML[i].ch = 0;
			tblMML[i].flgRequest = 0;
			tblMML[i].flgPlaying = 0;
			tblMML[i].cnt = 0;
			tblMML[i].len = 0;
			for ( int j = 0 ; j < 1024 ; j++ )
			{
				tblMML[i].inf[j].hz = 0;
				tblMML[i].inf[j].vol = 0;
				tblMML[i].inf[j].lim = 0;
			}
		}
	}	


//	printf("str     %s\n" , str );

	int	vol = 12;
	int	lim = 3;
	int	oct = 2;
	int ch = 0;
//	int	max = 0;

//cout << "--" << endl;
	for ( int i = 0 ; i < (signed)strlen(str) ; i++ )
	{
		char	c = (char)toupper(str[i]);
		{
			if ( c==':' )
			{
//				printf("--\n" );
				ch++;

				vol = 12;
				lim = 3;
				oct = 2;
				lenToken = 0;
				lenNum = 0;
				flgFade = false;
				flgOto = false;

				continue;
			}
			if ( flgOto || (flgFade && (isalpha(c)||ispunct(c))) )
			{
				strToken[lenToken] = 0;
				strNum[lenNum] = 0;
				int num = atoi(strNum);
//				printf("strToken   %s %d %d\n" , strToken, num, lenNum );

//				int hz = 0;

				MML&		mml = tblMML[ch];
//cout << strToken << num << endl;

				if ( strcmp( strToken, "T"  ) ==0 ) {mml_tempo=num;}
				else
				if ( strcmp( strToken, "V"  ) ==0 ) {vol=num;}
				else
				if ( strcmp( strToken, "O"  ) ==0 ) {oct=num;}
				else
				{
					if ( strcmp( strToken, "R"  ) == 0 ) {if(lenNum==0){num=lim;} lim=num; mml.flgPlaying=true; mml.flgRequest=true; mml.Add(mml_tblLim[num], vol, 0);}
					if ( strcmp( strToken, "C"  ) == 0 ) {if(lenNum==0){num=lim;} lim=num; mml.flgPlaying=true; mml.flgRequest=true; mml.Add(mml_tblLim[num], vol, g_tblFeq[oct  ][0]);}
					if ( strcmp( strToken, "#C" ) == 0 ) {if(lenNum==0){num=lim;} lim=num; mml.flgPlaying=true; mml.flgRequest=true; mml.Add(mml_tblLim[num], vol, g_tblFeq[oct  ][1]);}
					if ( strcmp( strToken, "D"  ) == 0 ) {if(lenNum==0){num=lim;} lim=num; mml.flgPlaying=true; mml.flgRequest=true; mml.Add(mml_tblLim[num], vol, g_tblFeq[oct  ][2]);}
					if ( strcmp( strToken, "#D" ) == 0 ) {if(lenNum==0){num=lim;} lim=num; mml.flgPlaying=true; mml.flgRequest=true; mml.Add(mml_tblLim[num], vol, g_tblFeq[oct  ][3]);}
					if ( strcmp( strToken, "E"  ) == 0 ) {if(lenNum==0){num=lim;} lim=num; mml.flgPlaying=true; mml.flgRequest=true; mml.Add(mml_tblLim[num], vol, g_tblFeq[oct  ][4]);}
					if ( strcmp( strToken, "F"  ) == 0 ) {if(lenNum==0){num=lim;} lim=num; mml.flgPlaying=true; mml.flgRequest=true; mml.Add(mml_tblLim[num], vol, g_tblFeq[oct  ][5]);}
					if ( strcmp( strToken, "#"  ) == 0 ) {if(lenNum==0){num=lim;} lim=num; mml.flgPlaying=true; mml.flgRequest=true; mml.Add(mml_tblLim[num], vol, g_tblFeq[oct  ][6]);}
					if ( strcmp( strToken, "G"  ) == 0 ) {if(lenNum==0){num=lim;} lim=num; mml.flgPlaying=true; mml.flgRequest=true; mml.Add(mml_tblLim[num], vol, g_tblFeq[oct  ][7]);}
					if ( strcmp( strToken, "#G" ) == 0 ) {if(lenNum==0){num=lim;} lim=num; mml.flgPlaying=true; mml.flgRequest=true; mml.Add(mml_tblLim[num], vol, g_tblFeq[oct  ][8]);}
					if ( strcmp( strToken, "A"  ) == 0 ) {if(lenNum==0){num=lim;} lim=num; mml.flgPlaying=true; mml.flgRequest=true; mml.Add(mml_tblLim[num], vol, g_tblFeq[oct  ][9]);}
					if ( strcmp( strToken, "#A" ) == 0 ) {if(lenNum==0){num=lim;} lim=num; mml.flgPlaying=true; mml.flgRequest=true; mml.Add(mml_tblLim[num], vol, g_tblFeq[oct  ][10]);}
					if ( strcmp( strToken, "B"  ) == 0 ) {if(lenNum==0){num=lim;} lim=num; mml.flgPlaying=true; mml.flgRequest=true; mml.Add(mml_tblLim[num], vol, g_tblFeq[oct  ][11]);}
					if ( strcmp( strToken, "-C" ) == 0 ) {if(lenNum==0){num=lim;} lim=num; mml.flgPlaying=true; mml.flgRequest=true; mml.Add(mml_tblLim[num], vol, g_tblFeq[oct-1][0]);}
					if ( strcmp( strToken, "-D" ) == 0 ) {if(lenNum==0){num=lim;} lim=num; mml.flgPlaying=true; mml.flgRequest=true; mml.Add(mml_tblLim[num], vol, g_tblFeq[oct-1][2]);}
					if ( strcmp( strToken, "-E" ) == 0 ) {if(lenNum==0){num=lim;} lim=num; mml.flgPlaying=true; mml.flgRequest=true; mml.Add(mml_tblLim[num], vol, g_tblFeq[oct-1][4]);}
					if ( strcmp( strToken, "-F" ) == 0 ) {if(lenNum==0){num=lim;} lim=num; mml.flgPlaying=true; mml.flgRequest=true; mml.Add(mml_tblLim[num], vol, g_tblFeq[oct-1][5]);}
					if ( strcmp( strToken, "-G" ) == 0 ) {if(lenNum==0){num=lim;} lim=num; mml.flgPlaying=true; mml.flgRequest=true; mml.Add(mml_tblLim[num], vol, g_tblFeq[oct-1][7]);}
					if ( strcmp( strToken, "-A" ) == 0 ) {if(lenNum==0){num=lim;} lim=num; mml.flgPlaying=true; mml.flgRequest=true; mml.Add(mml_tblLim[num], vol, g_tblFeq[oct-1][9]);}
					if ( strcmp( strToken, "-B" ) == 0 ) {if(lenNum==0){num=lim;} lim=num; mml.flgPlaying=true; mml.flgRequest=true; mml.Add(mml_tblLim[num], vol, g_tblFeq[oct-1][11]);}
					if ( strcmp( strToken, "+C" ) == 0 ) {if(lenNum==0){num=lim;} lim=num; mml.flgPlaying=true; mml.flgRequest=true; mml.Add(mml_tblLim[num], vol, g_tblFeq[oct+1][0]);}
					if ( strcmp( strToken, "+D" ) == 0 ) {if(lenNum==0){num=lim;} lim=num; mml.flgPlaying=true; mml.flgRequest=true; mml.Add(mml_tblLim[num], vol, g_tblFeq[oct+1][2]);}
					if ( strcmp( strToken, "+E" ) == 0 ) {if(lenNum==0){num=lim;} lim=num; mml.flgPlaying=true; mml.flgRequest=true; mml.Add(mml_tblLim[num], vol, g_tblFeq[oct+1][4]);}
					if ( strcmp( strToken, "+F" ) == 0 ) {if(lenNum==0){num=lim;} lim=num; mml.flgPlaying=true; mml.flgRequest=true; mml.Add(mml_tblLim[num], vol, g_tblFeq[oct+1][5]);}
					if ( strcmp( strToken, "+G" ) == 0 ) {if(lenNum==0){num=lim;} lim=num; mml.flgPlaying=true; mml.flgRequest=true; mml.Add(mml_tblLim[num], vol, g_tblFeq[oct+1][7]);}
					if ( strcmp( strToken, "+A" ) == 0 ) {if(lenNum==0){num=lim;} lim=num; mml.flgPlaying=true; mml.flgRequest=true; mml.Add(mml_tblLim[num], vol, g_tblFeq[oct+1][9]);}
					if ( strcmp( strToken, "+B" ) == 0 ) {if(lenNum==0){num=lim;} lim=num; mml.flgPlaying=true; mml.flgRequest=true; mml.Add(mml_tblLim[num], vol, g_tblFeq[oct+1][11]);}
//printf("hx %s %f %d\n",strToken, (float)g_tblFeq[oct+1][11],oct);
				}

				lenToken = 0;
				lenNum = 0;
				flgFade = false;
//				flgOto = false;

			}

			if ( isalpha(c) )
			{
				flgFade = true;
				strToken[lenToken++] = c;
				strToken[lenToken] = 0;
			}
			else
			if ( isdigit(c) )
			{
				strNum[lenNum++] = c;
				strNum[lenNum] = 0;
				flgFade = true;
			}
			else
			{
				strToken[lenToken++] = c;
				strToken[lenToken] = 0;
			}
		}
		
	}


//				printf("--%d\n", max );
#if 0
			for ( int ch = 0 ; ch < MAX_CH ; ch++ )
			{
				MML&		mml = tblMML[ch];
				for  ( int i = 0 ; i < mml.len ; i++ )
				{
					MML_INF&	inf = mml.inf[i];

					printf("ring --%d %3d %d %d\n", ch, inf.hz, mml.cnt, mml.len );
				}
				
			}
#endif

}



//----------------------------------------------------------------------
void	mml_Play1Step( double sec )
//----------------------------------------------------------------------
{
	static	double	time = 0;
	time += sec;
	double	t1 = (60.0/mml_tempo)/mml_tblLim[5];


		if ( t1 <= time )
		{
//printf("cnt %d %f %f %d\n",++mml_cnt, t1,time, mml_tempo);
			time -= t1;

			for ( int ch = 0 ; ch < MAX_CH ; ch++ )
			{
				MML&		mml = tblMML[ch];

				mml_playch( mml );
				
			}

		}
}
//----------------------------------------------------------------------
double	psg_generate1Step( double rate )
//----------------------------------------------------------------------
{
	double	ret_vol = 0;

					{
						static	double		ca = 0;
								double		aa = psg_A.ValPeriod * rate;
						static	double		cb = 0;
								double		ab = psg_B.ValPeriod * rate ;
						static	double		cc = 0;
								double		ac = psg_C.ValPeriod * rate ;
						static	double		cn = 0;
								double		an = psg_N.ValPeriod * rate ;
						static	double		ce = 0;
								double		ae = psg_E.ValPeriod * rate * 16 ;

						if ( ce > ae ) ce-= ae;
						double	ve = 1.0 - ce / ae;

						// AMP. CONTROL
						double	va = 0.0;
						double	vb = 0.0;
						double	vc = 0.0;
						//	TONE GEN.(3)
 						if ( psg_A.FlgTone )	va = ( ca < aa/2 ) ? 1.0 : 0.0; else va = 1.0;
						if ( psg_B.FlgTone )	vb = ( cb < ab/2 ) ? 1.0 : 0.0; else vb = 1.0;
						if ( psg_C.FlgTone )	vc = ( cc < ac/2 ) ? 1.0 : 0.0; else vc = 1.0;

						//	MIXERS(3)
 						if ( psg_A.FlgNoise )	if ( psg_N.valGenerated ) va = (1.0-va);
						if ( psg_B.FlgNoise )	if ( psg_N.valGenerated ) vb = (1.0-vb);
						if ( psg_C.FlgNoise )	if ( psg_N.valGenerated ) vc = (1.0-vc);

						// AMP. CONTROL
  						if ( psg_A.FlgEnv )		va *= ve; else va *= psg_A.Vol4bit / 15.0;
						if ( psg_B.FlgEnv )		vb *= ve; else vb *= psg_B.Vol4bit / 15.0;
						if ( psg_C.FlgEnv )		vc *= ve; else vc *= psg_C.Vol4bit / 15.0;

						// D/A CONVERTERS(3)
						va = g_tblDA[ int(va*15)&0xf ];
						vb = g_tblDA[ int(vb*15)&0xf ];
						vc = g_tblDA[ int(vc*15)&0xf ];

						double vol = (va+vb+vc)/3;

						if ( vol > 1.0 ) vol = 1.0 ;
						ret_vol = vol;

						ca += 1;
						cb += 1;
						cc += 1;
						cn += 1;
						ce += 1;
						if ( ca > aa )							ca -= aa;
						if ( cb > ab )							cb -= ab;
						if ( cc > ac )							cc -= ac;
						if ( cn > an )						
						{
							cn -= an;	
							//	NOISE GEN.
							psg_N.valGenerated = rand()&1;	
						}
						if ( ce > ae )						{
							if ( psg_E.psg_wave == 3 )	ce = ae;
							else
							if ( psg_E.psg_wave == 8 )	ce -= ae;
							else
							ce = ae;
						}
					}

	return ret_vol;
}

#if USE_DIRECTOUT == false
//----------------------------------------------------------------------
void sound_thread(int *pNum )
//----------------------------------------------------------------------
{

	static	double	st = 0;
	static	double	tm = 0;

	static	double	st2 = 0;
	static	double	tm2 = 0;


	while(g_sound_flg)
	{
		tm = sys.getPerformanceCounter();

		if ( st + 16.0/PSG_CLOCK < tm )
		{
			st = tm;

			double	vol = psg_generate1Step( 1.0 );

//printf("gen vol  %f\n", vol );
			psg_out.Add( vol );


		}

#if 1
		if ( psg_flg_WOM_DONE )
		{
		//	if ( psg_out.Length() >= (PSG_CLOCK/16/30) )
			{
				psg_flg_WOM_DONE = false;

				g_wave.numBack = (g_wave.numBack==0);

				double	buf[ Wave::LEN_BUFFER ];

				psg_Extend( buf, psg_out, SAMPLING_RATE/PSG_CLOCK*16 );

				for ( int i = 0 ; i < Wave::LEN_BUFFER ; i++ )
				{
					double vol = buf[i];
					if ( vol >  1.0 ) vol = 1.0;
					vol *= (SAMPLING_MAX-SAMPLING_MIN)/2;

					g_wave.bufWaveB[g_wave.numBack][i] = vol;
				}
				waveOutWrite(g_wave.hWaveB , &g_wave.whdrB[g_wave.numBack] , sizeof(g_wave.whdrB[g_wave.numBack]));
			}
		}
#else

		if ( psg_out.Length() >= (PSG_CLOCK/16/30) )
		{
			g_wave.numBack = (g_wave.numBack==0);

			double	buf[ Wave::LEN_BUFFER ];

			psg_Extend( buf, psg_out, SAMPLING_RATE/PSG_CLOCK*16 );

			for ( int i = 0 ; i < Wave::LEN_BUFFER ; i++ )
			{
				double vol = buf[i];
				if ( vol >  1.0 ) vol = 1.0;
				vol *= (SAMPLING_MAX-SAMPLING_MIN)/2;

				g_wave.bufWaveB[g_wave.numBack][i] = vol;
			}
			waveOutWrite(g_wave.hWaveB , &g_wave.whdrB[g_wave.numBack] , sizeof(g_wave.whdrB[g_wave.numBack]));
		}
#endif

	}
	g_fin = true;
	return;
}
#endif

#if USE_DIRECTOUT == true
//-----------------------------------------------------------------------------
void	psg_convertDA( double& vol, psg_Tone& tone, double ep )
//-----------------------------------------------------------------------------
{
	if ( tone.FlgEnv )
	{
		vol =  ep;
	}
	else
	{
		vol = double(tone.Vol4bit)/16.0;
	}
	// DA
	bool	f = (tone.FlgTone && tone.FlgHigh) || (tone.FlgNoise && psg_N.FlgHigh);

	if ( flgUseTbl )
	{
		unsigned int v = int(vol*16)&0xf;
		vol = g_tblDA[v];

		vol = v/16.0;


	}
//		vol = ( f ) ? vol : -vol;
//		vol = ( f ) ? 0 : vol;
//		vol = ( f ) ? -vol/1 : vol;
// printf("minus %f\n",vol);

		vol = (vol + (f*1.0));
}
#endif

//-----------------------------------------------------------------------------
void CALLBACK waveOutProc(
//-----------------------------------------------------------------------------
	HWAVEOUT phwo , 
	UINT uMsg,
	DWORD dwInstance,  
	DWORD dwParam1, 
	DWORD dwParam2     
)
{
//printf("dparam %d %d\n", (int)dwParam1, (int)dwParam2 );

	{
		if ( uMsg == WOM_DONE )
		{
#if USE_DIRECTOUT == false
				psg_flg_WOM_DONE = true;
#endif

#if USE_DIRECTOUT == true
				g_wave.numBack = (g_wave.numBack==0);
#if 0
				static	double		amt_a = 0;
				static	double		amt_b = 0;
				static	double		amt_c = 0;
				static	double		amt_n = 0;
				static	double		amt_e = 0;
				double tm_a = psg_A.ValPeriod;
				double tm_b = psg_B.ValPeriod;
				double tm_c = psg_C.ValPeriod;
				double tm_n = psg_N.ValPeriod;
				double tm_e = psg_E.ValPeriod;


				for ( int i = 0 ; i < Wave::LEN_BUFFER ; i++ )
				{

					//	TONE / NOISE / ENVELOPE GEN
					if ( amt_a >= tm_a ) { psg_A.FlgHigh = !psg_A.FlgHigh; amt_a -= tm_a;}
					if ( amt_b >= tm_b ) { psg_B.FlgHigh = !psg_B.FlgHigh; amt_b -= tm_b;}
					if ( amt_c >= tm_c ) { psg_C.FlgHigh = !psg_C.FlgHigh; amt_c -= tm_c;}
					if ( amt_n >= tm_n ) { psg_N.FlgHigh = rand()&1      ; amt_n -= tm_n;}
					if ( amt_e >= tm_e ) 
					{
						psg_E.FlgHigh = !psg_E.FlgHigh;
						amt_e = 0;

						switch ( psg_E.psg_wave )
						{
							case  0:
							case  1:
							case  2:
							case  3:	amt_e  = tm_e;	break;
							case  4:
							case  5:
							case  6:
							case  7:	amt_e  = 0;		break;
							case  8:	amt_e -= tm_e;	break;
							case  9:
							case 10:	
							case 11:
							case 12:
							case 13:
							case 14:
							case 15:
							default:	amt_e = tm_e;	break;
						}
					}

					//	GEN
					double wave = 0;
					switch ( psg_E.psg_wave )
					{
						case  8:	wave = 1.0- amt_e / tm_e;	break;
						default :	wave = 1.0- amt_e / tm_e;
					}

					double	va = 0.0;
					double	vb = 0.0;
					double	vc = 0.0;
					psg_convertDA( va, psg_A , wave );
					psg_convertDA( vb, psg_B , wave );
					psg_convertDA( vc, psg_C , wave );

					//	MIXERS
					double vol = (va + vb+ vc);
					if ( vol > 1.0 ) vol = 1.0;
					if ( vol <-1.0 ) vol =-1.0;
					vol *= (SAMPLING_MAX-SAMPLING_MIN)/2;

					//	write
					g_wave.bufWaveB[g_wave.numBack][i] = vol;

					//	count
					amt_a += 2.0*PSG_CLOCK/SAMPLING_RATE/16.0;
					amt_b += 2.0*PSG_CLOCK/SAMPLING_RATE/16.0;
					amt_c += 2.0*PSG_CLOCK/SAMPLING_RATE/16.0;
					amt_n += 1.0*PSG_CLOCK/SAMPLING_RATE/16.0;
					amt_e += 1.0*PSG_CLOCK/SAMPLING_RATE/256.0;
				}
#endif
#if 0
	//	詳細版
				{
					static	double rate = PSG_CLOCK/SAMPLING_RATE;

					static	double		a_amt = 0;
					static	double		b_amt = 0;
					static	double		c_amt = 0;
					static	double		n_amt = 0;
					static	double		e_amt = 0;
							double		a_max = double(psg_A.ValPeriod);
							double		b_max = double(psg_B.ValPeriod);
							double		c_max = double(psg_C.ValPeriod);
							double		n_max = double(psg_N.ValPeriod);
							double		e_max = double(psg_E.ValPeriod);

					for ( int i = 0 ; i < Wave::LEN_BUFFER ; i++ )
					{

						//	tone generate
						int	ta = 0;
						int	tb = 0;
						int	tc = 0;
						if ( psg_A.FlgTone )	ta = ( a_amt < a_max/2 );
						if ( psg_B.FlgTone )	tb = ( b_amt < b_max/2 );
						if ( psg_C.FlgTone )	tc = ( c_amt < c_max/2 );

						//	mixer tone & noise
						if ( psg_A.FlgNoise )	ta = psg_N.valGenerated ? 1 : ta;
						if ( psg_B.FlgNoise )	tb = psg_N.valGenerated ? 1 : tb;
						if ( psg_C.FlgNoise )	tc = psg_N.valGenerated ? 1 : tc;


						int	aa = psg_A.Vol4bit;
						int	ab = psg_B.Vol4bit;
						int	ac = psg_C.Vol4bit;
						{
							//	envelope generate
							int	ve = 0;
							{
								double	vol = 1.0 - e_amt / e_max;
								if ( vol < 0 ) vol = 0;
								ve = int(vol*16)&0xf;
							}

							//	amp control
							if ( psg_A.FlgEnv )	aa = ve;
							if ( psg_B.FlgEnv )	ab = ve;
							if ( psg_C.FlgEnv )	ac = ve;
						}
	
						//	D/A converters
						double	va = double(ta) * double(aa)/15;
						double	vb = double(tb) * double(ab)/15;
						double	vc = double(tc) * double(ac)/15;

						va = g_tblDA[ int(va*15)&0xf ];
						vb = g_tblDA[ int(vb*15)&0xf ];
						vc = g_tblDA[ int(vc*15)&0xf ];

						//	normalize
						double vol = (va+vb+vc)/3;
						if ( vol > 1.0 ) vol = 1.0;
						vol *= (SAMPLING_MAX-SAMPLING_MIN)/2;

						g_wave.bufWaveB[g_wave.numBack][i] = vol;

						//	counting
						a_amt += (rate/16.0);
						b_amt += (rate/16.0);
						c_amt += (rate/16.0);
						n_amt += (rate/16.0);
						e_amt += (rate/256.0);
						if ( a_amt > a_max )		a_amt -= a_max;
						if ( b_amt > b_max )		b_amt -= b_max;
						if ( c_amt > c_max )		c_amt -= c_max;
						if ( n_amt > n_max )	{	n_amt -= n_max; psg_N.valGenerated = rand()&1;}
						if ( e_amt > e_max )		
						{
							if ( psg_E.psg_wave == 3 )	e_amt = e_max;
							else
							if ( psg_E.psg_wave == 8 )	e_amt -= e_max;
							else
							e_amt = e_max;
						}
					}
				}

#endif

#if 1

	//	最終版
				{
					for ( int i = 0 ; i < Wave::LEN_BUFFER ; i++ )
					{
						mml_Play1Step( 0.5/Wave::LEN_BUFFER/15 );
						double	vol = psg_generate1Step( SAMPLING_RATE/PSG_CLOCK*16 );
						vol *= (SAMPLING_MAX-SAMPLING_MIN)/2;
//printf("gen2 vol  %f\n", vol );
						g_wave.bufWaveB[g_wave.numBack][i] = (short)(vol * g_volume);

					}
/*
					static	double		ca = 0;
							double		aa = psg_A.ValPeriod ;
					static	double		cb = 0;
							double		ab = psg_B.ValPeriod ;
					static	double		cc = 0;
							double		ac = psg_C.ValPeriod ;
					static	double		cn = 0;
							double		an = psg_N.ValPeriod ;
					static	double		ce = 0;
							double		ae = psg_E.ValPeriod ;


					for ( int i = 0 ; i < Wave::LEN_BUFFER ; i++ )
					{
						if ( ce > ae ) ce-= ae;

						double	ve = 1.0 - ce / ae;

						double	va = 0.0;
						double	vb = 0.0;
						double	vc = 0.0;

 						if ( psg_A.FlgTone )	va = ( ca < aa/2 ) ? 1.0 : 0.0; else va = 1.0;
 						if ( psg_B.FlgTone )	vb = ( cb < ab/2 ) ? 1.0 : 0.0; else vb = 1.0;
 						if ( psg_C.FlgTone )	vc = ( cc < ac/2 ) ? 1.0 : 0.0; else vc = 1.0;

 						if ( psg_A.FlgNoise )	if ( psg_N.valGenerated ) va = (1.0-va);
 						if ( psg_B.FlgNoise )	if ( psg_N.valGenerated ) vb = (1.0-vb);
 						if ( psg_C.FlgNoise )	if ( psg_N.valGenerated ) vc = (1.0-vc);

  						if ( psg_A.FlgEnv )		va *= ve; else va *= psg_A.Vol4bit / 15.0;
  						if ( psg_B.FlgEnv )		vb *= ve; else vb *= psg_B.Vol4bit / 15.0;
  						if ( psg_B.FlgEnv )		vc *= ve; else vc *= psg_C.Vol4bit / 15.0;

						va = g_tblDA[ int(va*15)&0xf ];
						vb = g_tblDA[ int(vb*15)&0xf ];
						vc = g_tblDA[ int(vc*15)&0xf ];

						double vol = (va+vb+vc)/3;
						if ( vol > 1.0 ) vol = 1.0 ;
						vol *= (SAMPLING_MAX-SAMPLING_MIN)/2;
						g_wave.bufWaveB[g_wave.numBack][i] = vol;

						ca += rate/ 16.0;
						cb += rate/ 16.0;
						cc += rate/ 16.0;
						cn += rate/ 16.0;
						ce += rate/256.0;
						if ( ca > aa )							ca -= aa;
						if ( cb > ab )							cb -= ab;
						if ( cc > ac )							cc -= ac;
						if ( cn > an )						{	cn -= an;	psg_N.valGenerated = rand()&1;	}
						if ( ce > ae )						{
							if ( psg_E.psg_wave == 3 )	ce = ae;
							else
							if ( psg_E.psg_wave == 8 )	ce -= ae;
							else
							ce = ae;
						}
					}
*/
				}
#endif
#if 0

	//	位相有り
				{
					static	double rate = PSG_CLOCK/SAMPLING_RATE;

					static	double		ca = 0;
							double		aa = psg_A.ValPeriod ;

					static	double		ce = 0;
							double		ae = psg_E.ValPeriod ;


					for ( int i = 0 ; i < Wave::LEN_BUFFER ; i++ )
					{

						double	ve = 1.0 - ce / ae;

						double	va = ( ca < aa/2 ) ? 1.0 : 0.0;

						double vol = va*ve;

						vol = g_tblDA[ int(vol*15)&0xf ];


						vol *= (SAMPLING_MAX-SAMPLING_MIN)/2;
						g_wave.bufWaveB[g_wave.numBack][i] = vol;

						ca += rate/ 16.0;
						if ( ca > aa )
						{
							ca -= aa;
						}

						ce += rate/256.0;
						if ( ce > ae )
						{
							ce -= ae;
						}

					}
				}
#endif
#if 0
	//	同期する
				{
					static	double rate = PSG_CLOCK/SAMPLING_RATE;

					static	int		ca = 0;
							int		aa = psg_A.ValPeriod / (rate/ 16.0) ;

					static	int		ce = 0;
							int		ae = psg_E.ValPeriod / (rate/256.0)  ;

//printf("hz  %f %f\n", (PSG_CLOCK/16.0)/psg_A.ValPeriod, (PSG_CLOCK/256.0)/psg_E.ValPeriod );

					for ( int i = 0 ; i < Wave::LEN_BUFFER ; i++ )
					{

						double	vol = 1.0 - double(ce)/double(ae);
//vol = 1.0;
double					va = ( ca < aa/2 ) ? 1.0 : 0;
vol = vol+va;

						unsigned int v = int(vol*15)&0xf;
						vol = g_tblDA[v];


						vol *= (SAMPLING_MAX-SAMPLING_MIN)/2;
						g_wave.bufWaveB[g_wave.numBack][i] = vol;

						ca++;
						if ( ca > aa )
						{
							ca = 0;
						}

						ce++;
						if ( ce > ae )
						{
							ce = 0;
						}

					}
				}
#endif
#if 0
	//	それっぽくなる　音程低いらしい
				{
					static	double rate = PSG_CLOCK/SAMPLING_RATE;

					static	int		ca = 0;
					static	bool	fa = false;
							int		aa = psg_A.ValPeriod / (rate/ 16.0) ;

					static	int		ce = 0;
							int		ae = psg_E.ValPeriod / (rate/256.0)  ;

					
					for ( int i = 0 ; i < Wave::LEN_BUFFER ; i++ )
					{

						double	vol = 1.0 - double(ce)/double(ae);
						unsigned int v = int(vol*15)&0xf;
						vol = g_tblDA[v];
//printf("vol %f %d %d\n", vol, ce, ae );
						vol = ( fa ) ? vol : -vol;
						vol *= (SAMPLING_MAX-SAMPLING_MIN)/2;
						g_wave.bufWaveB[g_wave.numBack][i] = vol;

						ca++;
						if ( ca > aa )
						{
							ca = 0;
							fa = !fa;
						}

						ce++;
						if ( ce > ae )
						{
							ce = 0;
						}

					}
				}
#endif
#if 0
				{
					double	hz = 440;
					static	double		amt_a = 0;
					static	double tm_a = PSG_CLOCK/hz/16/2;
					static	bool flg = 0;
					for ( int i = 0 ; i < Wave::LEN_BUFFER ; i++ )
					{

						if ( amt_a >= tm_a ) { flg = !flg; amt_a -= tm_a;}

						double	vol = 0.0;
						vol = ( flg ) ? 1.0 : -1.0;

						vol *= (SAMPLING_MAX-SAMPLING_MIN)/2;

						g_wave.bufWaveB[g_wave.numBack][i] = vol;

						amt_a += PSG_CLOCK/SAMPLING_RATE/16.0;
					}
				}
#endif
				waveOutWrite(g_wave.hWaveB , &g_wave.whdrB[g_wave.numBack] , sizeof(g_wave.whdrB[g_wave.numBack]));
#endif
		}
	}
}
//-----------------------------------------------------------------------------
SysSound::SysSound()
//-----------------------------------------------------------------------------
{
//	m_volume = 1.0;
	g_volume = 1.0;

	for ( int ch = 0 ; ch < MAX_CH ; ch++ )
	{
		tblMML[ch].flgRequest = false;
		tblMML[ch].flgPlaying = false;
		tblMML[ch].len = 0;
		tblMML[ch].cnt = 0;
	}
	

	long double	f = 55.0/8;
	long double	fq = pow(2,1.0/12.0);
			f *= fq;
			f *= fq;
			f *= fq;


	for ( int i = 0 ; i < 8 ; i++ )
	{
		for ( int j = 0 ; j < 12 ; j++ )
		{
			g_tblFeq[i][j] = (int)(f);
//printf("%d %d %f\n",i,j,(float)f);
			f *= fq;
		}
	}

//printf("rt %f\n",pow(2,1/12));
	{
		WAVEFORMATEX wfx;
		wfx.wFormatTag		= WAVE_FORMAT_PCM;
		wfx.nChannels		= 1;
		wfx.nSamplesPerSec	= SAMPLING_RATE;
		wfx.wBitsPerSample	= SAMPLING_BITS;
		wfx.cbSize			= 0;
		wfx.nBlockAlign		= (WORD)(wfx.nChannels * (wfx.wBitsPerSample/8));
		wfx.nAvgBytesPerSec	= wfx.nSamplesPerSec * wfx.nBlockAlign;
		waveOutOpen(&g_wave.hWaveB , WAVE_MAPPER , &wfx , (DWORD_PTR)waveOutProc , 0 , CALLBACK_FUNCTION);
	}

//	g_mml_hdlThread   = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) mml_thread, &g_mml_flg, 0, NULL);
#if USE_DIRECTOUT == false
	g_sound_hdlThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) sound_thread, &g_sound_flg, 0, NULL);
#endif
	memset( (void*)&psg_A, 0, sizeof(psg_A) );
	memset( (void*)&psg_B, 0, sizeof(psg_B) );
	memset( (void*)&psg_C, 0, sizeof(psg_C) );
	memset( (void*)&psg_N, 0, sizeof(psg_N) );
	memset( (void*)&psg_E, 0, sizeof(psg_E) );
#if USE_DIRECTOUT == false
//	memset( (void*)&psg_da_A, 0, sizeof(psg_da_A) );
//	memset( (void*)&psg_da_B, 0, sizeof(psg_da_B) );
//	memset( (void*)&psg_da_C, 0, sizeof(psg_da_C) );
	memset( (void*)&psg_out, 0, sizeof(psg_out) );
#endif
//	psg_mixer = 0;



	for ( int n = 0 ; n < 2 ; n++ )
	{
		{// sin wave
			for (int i = 0 ; i < Wave::LEN_BUFFER ; i++ )
			{
				g_wave.bufWaveB[n][i] = SAMPLING_ZERO;
			}
		}

		g_wave.whdrB[n].lpData = (LPSTR)g_wave.bufWaveB[n];
		g_wave.whdrB[n].dwBufferLength = Wave::LEN_BUFFER*(SAMPLING_BITS/8);
		g_wave.whdrB[n].dwFlags = WHDR_BEGINLOOP | WHDR_ENDLOOP;
		g_wave.whdrB[n].dwLoops = 1;
		waveOutPrepareHeader(g_wave.hWaveB , &g_wave.whdrB[n] , sizeof(g_wave.whdrB[n]));
		waveOutWrite(g_wave.hWaveB , &g_wave.whdrB[n] , sizeof(g_wave.whdrB[n]));

	}
	g_wave.numBack = 1;

	mml_play( "T600O6V0cr:V0cr:V0cr");// 最初の一音が出ないことがあるのでなんとなく
}


//-----------------------------------------------------------------------------
void psg_Set2( int num, int val )
//-----------------------------------------------------------------------------
{
	switch( num )
	{
		case  0:	psg_A.ValPeriod	= (psg_A.ValPeriod & 0x0f00) |  (0xff&val);		break; //psg_A.Limit = psg_A.ValPeriod;	break;
		case  1:	psg_A.ValPeriod	= (psg_A.ValPeriod & 0x00ff) | ((0x0f&val)<<8);	break; //psg_A.Limit = psg_A.ValPeriod;	break;
		case  2:	psg_B.ValPeriod	= (psg_B.ValPeriod & 0x0f00) |  (0xff&val);		break; //psg_B.Limit = psg_B.ValPeriod;	break;
		case  3:	psg_B.ValPeriod	= (psg_B.ValPeriod & 0x00ff) | ((0x0f&val)<<8);	break; //psg_B.Limit = psg_B.ValPeriod;	break;
		case  4:	psg_C.ValPeriod	= (psg_C.ValPeriod & 0x0f00) |  (0xff&val);		break; //psg_C.Limit = psg_C.ValPeriod;	break;
		case  5:	psg_C.ValPeriod	= (psg_C.ValPeriod & 0x00ff) | ((0x0f&val)<<8);	break; //psg_C.Limit = psg_C.ValPeriod;	break;
		case  6:	psg_N.ValPeriod	= val;												break; //psg_N.Limit = psg_N.ValPeriod; break;
		case  7:	
//		psg_mixer	= val;
					psg_A.FlgTone  = !(val&(1<<0));
					psg_B.FlgTone  = !(val&(1<<1));
					psg_C.FlgTone  = !(val&(1<<2));
					psg_A.FlgNoise = !(val&(1<<3));
					psg_B.FlgNoise = !(val&(1<<4));
					psg_C.FlgNoise = !(val&(1<<5));
					break;
		case  8:	psg_A.Vol4bit	= (val&0x0f);	psg_A.FlgEnv = ((val&0x10)!=0); break;
		case  9:	psg_B.Vol4bit	= (val&0x0f);	psg_B.FlgEnv = ((val&0x10)!=0); break;
		case 10:	psg_C.Vol4bit	= (val&0x0f);	psg_C.FlgEnv = ((val&0x10)!=0); break;
		case 11:	psg_E.ValPeriod	= (psg_E.ValPeriod & 0xff00) |  (0xff&val);			break; //psg_E.Limit = psg_E.ValPeriod; break;
		case 12:	psg_E.ValPeriod	= (psg_E.ValPeriod & 0x00ff) | ((0xff&val)<<8);		break; //psg_E.Limit = psg_E.ValPeriod; break;
		case 13:	psg_E.psg_wave	= val;
					psg_E.FlgHold		= (val>>0)&1;
					psg_E.FlgAlternate	= (val>>1)&1;
					psg_E.FlgAttack		= (val>>2)&1;
					psg_E.FlgContinue	= (val>>3)&1;
					psg_E.Limit = psg_E.ValPeriod; 
					break;
	}

}
//-----------------------------------------------------------------------------
int SysSound::psg_Get( int num )
//-----------------------------------------------------------------------------
{
	int	val = 0;
	switch( num )
	{
		case  0:	val = (psg_A.ValPeriod   ) & 0xff; break;
		case  1:	val = (psg_A.ValPeriod>>8) & 0x0f; break;
		case  2:	val = (psg_B.ValPeriod   ) & 0xff; break;
		case  3:	val = (psg_B.ValPeriod>>8) & 0x0f; break;
		case  4:	val = (psg_C.ValPeriod   ) & 0xff; break;
		case  5:	val = (psg_C.ValPeriod>>8) & 0x0f; break;

		case  6:	val = psg_N.ValPeriod & 0x1f;	break;
		case  7:	val = 0
					| (!psg_A.FlgTone << 0)
					| (!psg_B.FlgTone << 1)
					| (!psg_C.FlgTone << 2)
					| (!psg_A.FlgNoise << 3)
					| (!psg_B.FlgNoise << 4)
					| (!psg_C.FlgNoise << 5)
					;
					break;
		case  8:	val = (psg_A.Vol4bit&0x0f) | (psg_A.FlgEnv<<4); break;
		case  9:	val = (psg_B.Vol4bit&0x0f) | (psg_B.FlgEnv<<4); break;
		case 10:	val = (psg_C.Vol4bit&0x0f) | (psg_C.FlgEnv<<4); break;
		case 11:	val = (psg_E.ValPeriod   ) &0xff; break;
		case 12:	val = (psg_E.ValPeriod>>8) &0xff; break;
		case 13:	val = 0
					| psg_E.FlgHold
					| psg_E.FlgAlternate
					| psg_E.FlgAttack
					| psg_E.FlgContinue
					;
					break;
	}
	return	val;
}
//-----------------------------------------------------------------------------
void SysSound::psg_Set( int num, int val )
//-----------------------------------------------------------------------------
{
	psg_Set2( num, val );
return;
		
if(0)
//	if ( sys.key.hi(VK_SPACE) ) 
	{
		printf(	"psg_A.ValPeriod   %d\n", psg_A.ValPeriod );
		printf( "psg_B.ValPeriod   %d\n", psg_B.ValPeriod );
		printf( "psg_C.ValPeriod   %d\n", psg_C.ValPeriod );
		printf( "psg_noise   %d\n", psg_N.ValPeriod );
		printf( "psg_A.Vol4bit    %d\n", psg_A.Vol4bit );
		printf( "psg_B.Vol4bit    %d\n", psg_B.Vol4bit );
		printf( "psg_C.Vol4bit    %d\n", psg_C.Vol4bit );
		printf( "psg_A.FlgEnv %d\n", psg_A.FlgEnv );
		printf( "psg_B.FlgEnv %d\n", psg_B.FlgEnv );
		printf( "psg_C.FlgEnv %d\n", psg_C.FlgEnv );
		printf( "ValPeriod %d\n", psg_E.ValPeriod );
		printf( "psg_wave    %d\n", psg_E.psg_wave );
		printf( "----------\n" );

	}

}

//-----------------------------------------------------------------------------
void SysSound::info()
//-----------------------------------------------------------------------------
{
	{
		printf("A  %fhz\n", PSG_CLOCK / ( 16.0 * psg_A.ValPeriod) );
		printf("B  %fhz\n", PSG_CLOCK / ( 16.0 * psg_B.ValPeriod) );
		printf("C  %fhz\n", PSG_CLOCK / ( 16.0 * psg_C.ValPeriod) );
		printf("N  %fhz\n", PSG_CLOCK / ( 16.0 * psg_N.ValPeriod) );
		printf("E  %fhz\n", PSG_CLOCK / (256.0 * psg_E.ValPeriod) );

	}

}

//-----------------------------------------------------------------------------
void SysSound::playRialtime( int num )
//-----------------------------------------------------------------------------
{

//		printf("sound playRialtime %d\n", num);

}

//-----------------------------------------------------------------------------
void SysSound::play( int num )
//-----------------------------------------------------------------------------
{

}
//-----------------------------------------------------------------------------
void SysSound::stop( int num )
//-----------------------------------------------------------------------------
{
//			waveOutReset(g_wave.hWaveB);
}
//-----------------------------------------------------------------------------
int SysSound::entry( const char* fname )
//-----------------------------------------------------------------------------
{
	return 1;
}
