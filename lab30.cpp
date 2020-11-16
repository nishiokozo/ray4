//2019/11/22

#include <iostream>
#include <memory>
#include <vector>
#include <map>
#include <thread>
#include <chrono>
#include <cmath>
#include <functional>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iomanip>
#include <tuple>
#include <random>

#include "geom.h"

#include "SysSound.h"
#include "SysKeys.h"
#include "SysMouse.h"
#include "SysGra.h"

#include "func.h"
#include "pers.h"

#include "lab.h"
#include "lab30.h"

#include <GL/gl.h>

using namespace std;

struct Lab30::Impl
{
	bool	bResetAll = true;
	bool	bResetParam = true;
	bool	bPause = false;
	bool	bStep = false;

	vector<shared_ptr<Obj>>	tbl_pObj;

	//
	
	struct	Texture
	{
		GLuint	texname;
		int	width;
		int height;

		Texture( int _width, int _height ) : width(_width), height(_height)
		{
			glGenTextures( 1, &texname );
			glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
			glBindTexture( GL_TEXTURE_2D, texname );
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0 );
			glBindTexture( GL_TEXTURE_2D, 0 );	// Bindを開放
		}

		~Texture()
		{
			glDeleteTextures( 1, &texname );
		}

		void loadRGBA( void* pBuf )
		{
			glBindTexture( GL_TEXTURE_2D, texname );
			glTexSubImage2D( GL_TEXTURE_2D, 0, 0,0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pBuf );
			glBindTexture( GL_TEXTURE_2D, 0 );	// Bindを外す
		}

		void Enable()
		{
			glEnable(GL_TEXTURE_2D);
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
			glBindTexture(GL_TEXTURE_2D, texname);
		}
		void Disable()
		{
			//glBindTexture( GL_TEXTURE_2D, 0 );	// Bindを外す
			glDisable(GL_TEXTURE_2D);
		}
	};

	unique_ptr<Texture> pTexture;
};
Lab30::Lab30() : pImpl( new Lab30::Impl ){}





//------------------------------------------------------------------------------
void Lab30::Update( SysKeys& keys, SysMouse& mouse, SysSound& sound, SysGra& gra, Pers& pers, float delta, int& text_y, Cp& cp )
//------------------------------------------------------------------------------
{
	// 画面クリア
	gra.Clr(rgb(0.3,0.3,0.3));
	pers.grid.DrawGrid3d( gra, pers, vect3(0,0,0), mat33::mrotx(rad(90)), 10, 10, 1, rgb(0.2,0.2,0.2) );
	gra.Print(1,(float)text_y++,"30 : GL Texture" ); 

	static GLuint texName = 0;

	static GLubyte* pImage = 0;


	//初期化
	if ( pImpl->bResetAll )
	{
		pImpl->bResetAll = false;

		// カメラ
		pers.cam.pos = vect3( 0.0, 2.0, -5.0 );
		pers.cam.at = vect3( 0,  1.0, 0 );
		pers.cam.Update();

		unique_ptr<Impl::Texture> pTmp(new Impl::Texture(gra.GetWidth(),gra.GetHeight()));
		pImpl->pTexture = move(pTmp);

		int adr =  gra.GetWidth()*gra.GetHeight()*4;
		pImage = new GLubyte[adr];

   	}

	static int cnt = 0;


	// リアルタイムにテクスチャ生成
//	GLubyte image[height][width][4];
	{
	   int i, j, c;

		int step = 1.0;
		{
			int width		= gra.GetWidth(); 
			int height	= gra.GetHeight(); 

	    
			for( int py = 0 ; py < height ; py += step )
			{
				for( int px = 0 ; px < width ; px += step )
				{

						int tx = px;
						int ty = py;
						int idx= (ty*width+tx)*4;
					
						unsigned char* adr = &pImage[idx];
						adr[0] = (unsigned char)(tx % 256);
						adr[1] = (unsigned char)(ty % 256);
						adr[2] = (unsigned char)(128);
						adr[3] = 255;


		      }
		   }
	   }
	}
	cnt++;

	// リアルタイムにテクスチャ転送
	pImpl->pTexture->loadRGBA( pImage );

	rgb	col(1,0,0);
	vect2 v0(-0.5,-0.5);
	vect2 v1( 0.5, 0.5);
	{
		glDepthFunc( GL_ALWAYS );	// ENABLEのときだけ 必ず書く
	
		pImpl->pTexture->Enable();

	    glBegin(GL_QUADS);
	    glTexCoord2f(0.0, 0.0); glVertex2f(v0.x, v0.y);
	    glTexCoord2f(0.0, 1.0); glVertex2f(v1.x, v0.y);
	    glTexCoord2f(1.0, 1.0); glVertex2f(v1.x, v1.y);
	    glTexCoord2f(1.0, 0.0); glVertex2f(v0.x, v1.y);
	    glEnd();

		pImpl->pTexture->Disable();

	}
	
	gra.Wait();
}
