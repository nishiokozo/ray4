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



#define SAMPLING_RATE 	44100		// 44.1Khz  byte / sec
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


#define	MML_DIV	64
//									   全
//						0	1	2	3	4	5	6	7	8	9
static int	mml_tblLim[10] = 
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

static HANDLE g_mml_hdlThread = 0;
static int	mml_tempo = 120;

static int	g_mml_flg = true;
static int	g_sound_flg = true;
static int	g_fin = false;
static HANDLE g_sound_hdlThread = 0;

class Wave
{
public:
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

static double	g_tblDA[16] = 
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


static MML	tblMML[MAX_CH]=
{
	{0,false,false,0,0,{}},
	{1,false,false,0,0,{}},
	{2,false,false,0,0,{}}
};

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

	{
		unsigned int v = int(vol*16)&0xf;
		vol = g_tblDA[v];

		vol = v/16.0;


	}
	vol = (vol + (f*1.0));
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
	}
	tone.Limit--;


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

		psg_Set2(7,0b111000);
		psg_Set2( 0+mml.ch*2,l);
		psg_Set2( 1+mml.ch*2,h);
		psg_Set2( 8+mml.ch, vol);
	}

}

//----------------------------------------------------------------------
void	mml_playch( MML& mml )
//----------------------------------------------------------------------
{
	if ( mml.cnt < mml.len )
	{
		MML_INF&	inf = mml.inf[mml.cnt];

		if ( mml.flgRequest == true )
		{
			mml.flgRequest = false;

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
				psg_Set2( 8+mml.ch,0);
				mml.flgPlaying = false;
			}
		}
	}
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
		time -= t1;

		for ( int ch = 0 ; ch < MAX_CH ; ch++ )
		{
			MML&		mml = tblMML[ch];

			mml_playch( mml );
			
		}

	}
}

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

	if ( uMsg == WOM_DONE )
	{

		g_wave.numBack = (g_wave.numBack==0);
		{
			for ( int i = 0 ; i < Wave::LEN_BUFFER ; i++ )
			{
				mml_Play1Step( 0.5/Wave::LEN_BUFFER/15 );
				double	vol = psg_generate1Step( SAMPLING_RATE/PSG_CLOCK*16 );
				vol *= (SAMPLING_MAX-SAMPLING_MIN)/2;
				g_wave.bufWaveB[g_wave.numBack][i] = (short)(vol * g_volume);

			}

		}
		waveOutWrite(g_wave.hWaveB , &g_wave.whdrB[g_wave.numBack] , sizeof(g_wave.whdrB[g_wave.numBack]));
	}
}

//-----------------------------------------------------------------------------
SysSound::~SysSound()
//-----------------------------------------------------------------------------
{
	g_sound_flg = false;
	

	if ( g_mml_hdlThread )	 CloseHandle(g_mml_hdlThread)   ;
	if ( g_sound_hdlThread )	 CloseHandle(g_sound_hdlThread)   ;

	if ( g_wave.hWaveB )
	{
		waveOutUnprepareHeader(g_wave.hWaveB , &g_wave.whdrB[0] , sizeof(WAVEHDR));
		waveOutUnprepareHeader(g_wave.hWaveB , &g_wave.whdrB[1] , sizeof(WAVEHDR));
		waveOutClose(g_wave.hWaveB);
		g_wave.hWaveB = 0;
	}

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

	char	strToken[100];
	char	strNum[100];
	int		lenToken = 0;
	int		lenNum = 0;
	bool	flgFade = false;
	bool	flgOto = false;


	{
		for ( int i = 0 ; i < MAX_CH ; i++ )
		{
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

	int	vol = 12;
	int	lim = 3;
	int	oct = 2;
	int ch = 0;

	for ( int i = 0 ; i < (signed)strlen(str) ; i++ )
	{
		char	c = (char)toupper(str[i]);
		{
			if ( c==':' )
			{
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

				MML&		mml = tblMML[ch];

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
				}

				lenToken = 0;
				lenNum = 0;
				flgFade = false;

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


}

//-----------------------------------------------------------------------------
SysSound::SysSound()
//-----------------------------------------------------------------------------
{
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
			f *= fq;
		}
	}

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

	memset( (void*)&psg_A, 0, sizeof(psg_A) );
	memset( (void*)&psg_B, 0, sizeof(psg_B) );
	memset( (void*)&psg_C, 0, sizeof(psg_C) );
	memset( (void*)&psg_N, 0, sizeof(psg_N) );
	memset( (void*)&psg_E, 0, sizeof(psg_E) );

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
