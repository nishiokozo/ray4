#include <iostream>
#include <cmath>

#include "geom.h"

using namespace std;


//-----------------------------------------------------------------------------
mat33 mrotateByAxis( vect3 axis, float th )
//-----------------------------------------------------------------------------
{
	// axis軸回りにth°回転する行列を返す。
	//
	// axisは正規化されてなくても良い。

	// z軸をaixsに合わせたマトリクスを作り
	float ry	= atan2( axis.x , axis.z);
	float lxz	= sqrt( axis.z * axis.z + axis.x * axis.x );
	float rz	= atan2( axis.y, lxz );
	mat33 mr = midentity();
	mr *= mrotx(-rz);
	mr *= mroty(ry);

	mat33 m = midentity();
	// 作成した行列のz軸で回転
	m *= mr.invers();
	m *= mrotz(th);
	m *= mr;

	return m;
};

//-----------------------------------------------------------------------------
void vect3::rotateByAxis( vect3 axis, float th )
//-----------------------------------------------------------------------------
{
	(*this) = mrotateByAxis( axis, th ) * (*this);  
};

//-----------------------------------------------------------------------------
float	mod( float a, float b )
//-----------------------------------------------------------------------------
{
	return fmod(a,b);
}

//-----------------------------------------------------------------------------
vect2	ave( vect2 a, vect2 b )
//-----------------------------------------------------------------------------
{
	return	(a+b)/2;
}

//-----------------------------------------------------------------------------
vect3	ave( vect3 a, vect3 b )
//-----------------------------------------------------------------------------
{
	return	(a+b)/2;
}

//-----------------------------------------------------------------------------
float	min( float a, float b )
//-----------------------------------------------------------------------------
{
	if ( a < b ) return a;
	return	b;
}

//-----------------------------------------------------------------------------
float	max( float a, float b )
//-----------------------------------------------------------------------------
{
	if ( a > b ) return a;
	return	b;
}
//-----------------------------------------------------------------------------
vect2	max( vect2 a, vect2 b )
//-----------------------------------------------------------------------------
{
	vect2	ret=b;
	if ( a.x > b.x ) ret.x = a.x;
	if ( a.y > b.y ) ret.y = a.y;
	return	ret;
}
//-----------------------------------------------------------------------------
vect2	min( vect2 a, vect2 b )
//-----------------------------------------------------------------------------
{
	vect2	ret=b;
	if ( a.x < b.x ) ret.x = a.x;
	if ( a.y < b.y ) ret.y = a.y;
	return	ret;
}
//-----------------------------------------------------------------------------
vect3	max( vect3 a, vect3 b )
//-----------------------------------------------------------------------------
{
	vect3	ret=b;
	if ( a.x > b.x ) ret.x = a.x;
	if ( a.y > b.y ) ret.y = a.y;
	if ( a.z > b.z ) ret.z = a.z;
	return	ret;
}
//-----------------------------------------------------------------------------
vect3	min( vect3 a, vect3 b )
//-----------------------------------------------------------------------------
{
	vect3	ret=b;
	if ( a.x < b.x ) ret.x = a.x;
	if ( a.y < b.y ) ret.y = a.y;
	if ( a.z < b.z ) ret.z = a.z;
	return	ret;
}


//-----------------------------------------------------------------------------
vect3	max( float a, vect3 b )
//-----------------------------------------------------------------------------
{
	vect3	ret;
	ret = b;
	if ( a > b.x ) ret.x = a;
	if ( a > b.y ) ret.y = a;
	if ( a > b.z ) ret.z = a;
	return	ret;
}

//-----------------------------------------------------------------------------
vect3	max( vect3 b, float a )
//-----------------------------------------------------------------------------
{
	vect3	ret;
	ret = b;
	if ( a > b.x ) ret.x = a;
	if ( a > b.y ) ret.y = a;
	if ( a > b.z ) ret.z = a;
	return	ret;
}

//-----------------------------------------------------------------------------
vect3	min( float a, vect3 b )
//-----------------------------------------------------------------------------
{
	vect3	ret;
	ret = b;
	if ( a < b.x ) ret.x = a;
	if ( a < b.y ) ret.y = a;
	if ( a < b.z ) ret.z = a;
	return	ret;
}

//-----------------------------------------------------------------------------
vect3	min( vect3 b, float a )
//-----------------------------------------------------------------------------
{
	vect3	ret;
	ret = b;
	if ( a < b.x ) ret.x = a;
	if ( a < b.y ) ret.y = a;
	if ( a < b.z ) ret.z = a;
	return	ret;
}

//-----------------------------------------------------------------------------
vect3	mix( float f, vect3 a, vect3 b )
//-----------------------------------------------------------------------------
{
	vect3	ret;

	ret = a*f + b*(1.0f-f);

	return ret;
}

//-----------------------------------------------------------------------------
vect3 reflect( vect3 I, vect3 N )
//-----------------------------------------------------------------------------
{
	vect3	ret;

 	ret = I - 2* dot(I,N)*N;

	return	ret;
}

//-----------------------------------------------------------------------------
vect3 refract( vect3 I, vect3 N, float nm )
//-----------------------------------------------------------------------------
{
	vect3	V;

#if 1
	float	n1 = 1.0;
	float	n2 = nm;
	float	d = dot(-I,N);

	V = (n1/n2)*(I-N*(sqrt((n2/n1)*(n2/n1)-1+d*d)-d));
#else

 #if 0
//V = (n*I*cosθ + n*N - m*N)/(m*cosφ)
	float	n = 1.0;
	float	m = nm;
	float	d = dot(I,-N); // cosθ


	V = (n*I*d + n*N - m*N)/(m*d);
 #else
//V = (n*N*cosθ + n*I - m*N*cosφ)/m
	float	n = 1.0;
	float	m = nm;
	float	d = dot(I,-N); // cosθ

	V = (n*N*d + n*I - m*N*d)/m


 #endif


	
#endif

	return V;
}

//-----------------------------------------------------------------------------
float	dot( vect3 a, vect3 b )
//-----------------------------------------------------------------------------
{
	return a.x * b.x + a.y * b.y +a.z * b.z ;
}
//-----------------------------------------------------------------------------
vect3	cross( vect3 a, vect3 b )
//-----------------------------------------------------------------------------
{
	// z = cross( x, y )

#if 0
	vect3 v; 
	v.x = a.y*b.z-a.z*b.y;
	v.y = a.z*b.x-a.x*b.z;
	v.z = a.x*b.y-a.y*b.x;
	return v;
#else
	return vect3(
		a.y*b.z-a.z*b.y,
		a.z*b.x-a.x*b.z,
		a.x*b.y-a.y*b.x
	);
#endif

}





//-----------------------------------------------------------------------------
float	length( vect3 a )
//-----------------------------------------------------------------------------
{
//	return sqrt( a.x*a.x + a.y*a.y + a.z*a.z );
	return sqrt(dot(a,a));
}
/*
//-----------------------------------------------------------------------------
float	length( vect3 a )
//-----------------------------------------------------------------------------
{
//	float	l = sqrt( a.x*a.x + a.y*a.y + a.z*a.z );
	float	l = sqrt( dot(a,a) );

	return	l;
}
*/

//-----------------------------------------------------------------------------
vect3	normalize( vect3 a )
//-----------------------------------------------------------------------------
{
	vect3 v; 
	float	l = 1.0 / sqrt( a.x*a.x + a.y*a.y + a.z*a.z );
	
	v.x = a.x * l;
	v.y = a.y * l;
	v.z = a.z * l;

	return	v;
}
//-----------------------------------------------------------------------------
void	vec4_multiplyMatrix( float v[4], float m[16], float a[4] )
//-----------------------------------------------------------------------------
{

	float	m00 = m[ 0];
	float	m01 = m[ 1];
	float	m02 = m[ 2];
	float	m03 = m[ 3];
	float	m10 = m[ 4];
	float	m11 = m[ 5];
	float	m12 = m[ 6];
	float	m13 = m[ 7];
	float	m20 = m[ 8];
	float	m21 = m[ 9];
	float	m22 = m[10];
	float	m23 = m[11];
	float	m30 = m[12];
	float	m31 = m[13];
	float	m32 = m[14];
	float	m33 = m[15];

	float	a0 = a[0];
	float	a1 = a[1];
	float	a2 = a[2];
	float	a3 = a[3];

	v[0] = m00 * a0 +  m01 * a1 +  m02 * a2 +  m03 * a3;
	v[1] = m10 * a0 +  m11 * a1 +  m12 * a2 +  m13 * a3;
	v[2] = m20 * a0 +  m21 * a1 +  m22 * a2 +  m23 * a3;
	v[3] = m30 * a0 +  m31 * a1 +  m32 * a2 +  m33 * a3;

}



//-----------------------------------------------------------------------------
int	mat4_print( float* m )
//-----------------------------------------------------------------------------
{
	printf("-----\n" );
	printf("%9f %9f %9f %9f\n", m[M00], m[M01], m[M02], m[M03] );
	printf("%9f %9f %9f %9f\n", m[M10], m[M11], m[M12], m[M13] );
	printf("%9f %9f %9f %9f\n", m[M20], m[M21], m[M22], m[M23] );
	printf("%9f %9f %9f %9f\n", m[M30], m[M31], m[M32], m[M33] );
	return true;
}
//-----------------------------------------------------------------------------
int	mat4_print( mat44& m )
//-----------------------------------------------------------------------------
{
	printf("-----\n" );
	printf("%9f %9f %9f %9f\n", m.m_array[M00], m.m_array[M01], m.m_array[M02], m.m_array[M03] );
	printf("%9f %9f %9f %9f\n", m.m_array[M10], m.m_array[M11], m.m_array[M12], m.m_array[M13] );
	printf("%9f %9f %9f %9f\n", m.m_array[M20], m.m_array[M21], m.m_array[M22], m.m_array[M23] );
	printf("%9f %9f %9f %9f\n", m.m_array[M30], m.m_array[M31], m.m_array[M32], m.m_array[M33] );
	return true;
}


//-----------------------------------------------------------------------------
void mat4_invers( mat44& a, const mat44& y1 )
//-----------------------------------------------------------------------------
{
	mat4_invers( a.GetArray(), a.GetArray() );
}
//-----------------------------------------------------------------------------
void mat4_invers( float* m, const float* in )
//-----------------------------------------------------------------------------
{
//	float a[4][4]={{1,2,0,-1},{-1,1,2,0},{2,0,1,1},{1,-2,-1,1}}; //入力用の配列
	float a[4][4];
//memcpy( a, in, sizeof(float[16] ) );
	a[0][0] = in[0];
	a[0][1] = in[1]; 
	a[0][2] = in[2]; 
	a[0][3] = in[3]; 
	a[1][0] = in[4]; 
	a[1][1] = in[5]; 
	a[1][2] = in[6]; 
	a[1][3] = in[7]; 
	a[2][0] = in[8]; 
	a[2][1] = in[9]; 
	a[2][2] = in[10]; 
	a[2][3] = in[11]; 
	a[3][0] = in[12]; 
	a[3][1] = in[13]; 
	a[3][2] = in[14]; 
	a[3][3] = in[15]; 



	float inv_a[4][4]; //ここに逆行列が入る
	float buf; //一時的なデータを蓄える
	int i,j,k; //カウンタ
	int z1=4;  //配列の次数

	//単位行列を作る
	for(i=0;i<z1;i++)
	{
		for(j=0;j<z1;j++)
		{
			inv_a[i][j]=(i==j)?1.0:0.0;
		}
	}
	//掃き出し法
	for(i=0;i<z1;i++)
	{
		buf=1/a[i][i];
		for(j=0;j<z1;j++)
		{
			a[i][j]*=buf;
			inv_a[i][j]*=buf;
		}
		for(j=0;j<z1;j++)
		{
			if(i!=j)
			{
				buf=a[j][i];
				for(k=0;k<z1;k++)
				{
					a[j][k]-=a[i][k]*buf;
					inv_a[j][k]-=inv_a[i][k]*buf;
				}
			}
		}
	}
	//逆行列を出力
	int	s = 0;
	for(i=0;i<z1;i++)
	{
		for(j=0;j<z1;j++)
		{
			//	    printf(" %z2",inv_a[i][j]);
			m[s] = inv_a[i][j];
			s++;
		}
		//	  printf("\n");
	}

}

//-----------------------------------------------------------------------------
void	mat4_ortho( float* m, float x1, float x2, float y1, float y2, float z1, float z2 )
//-----------------------------------------------------------------------------
{
	float sx =  (2.0     )/( x2 - x1 );
	float sy =  (2.0     )/( y2 - y1 );
	float sz = -(2.0     )/( z2 - z1 );
	float tx =  -(x2+x1)/(x2-x1);
	float ty =  -(y2+y1)/(y2-y1);
	float tz =  -(z2+z1)/(z2-z1);


	m[ 0] =   sx;	m[ 1] = 0.0f;	m[ 2] = 0.0f;	m[ 3] = 0.0f;
	m[ 4] = 0.0f;	m[ 5] =   sy;	m[ 6] = 0.0f;	m[ 7] = 0.0f;
	m[ 8] = 0.0f;	m[ 9] = 0.0f;	m[10] =   sz;	m[11] = 0.0f;
	m[12] =   tx;	m[13] =   ty;	m[14] =   tz;	m[15] = 1.0f;
}
//-----------------------------------------------------------------------------
void mat4_perspective ( mat44& m, float fovy, float aspect, float zNear, float zFar)
//-----------------------------------------------------------------------------
{
	mat4_perspective ( m.GetArray(), fovy, aspect, zNear, zFar );
}
//-----------------------------------------------------------------------------
void mat4_perspective (float* m, float fovy, float aspect, float zNear, float zFar)
//-----------------------------------------------------------------------------
{
        float p = 1.0 / tan(fovy * (pi/360.0));

/*
        m[0] = z2 / aspect;
        m[1] = 0.0;
        m[2] = 0.0;
        m[3] = 0.0;

        m[4] = 0.0;
        m[5] = z2;
        m[6] = 0.0;
        m[7] = 0.0;

        m[8] = 0.0;
        m[9] = 0.0;
        m[10] = (zFar + zNear) / (zNear - zFar);
        m[11] = -1.0;

        m[12] = 0.0;
        m[13] = 0.0;
        m[14] = 2.0 * zFar * zNear / (zNear - zFar);
        m[15] = 0.0;
*/

/*
	float	p = {
        z2 / aspect,	0.0,	0.0,									 0.0,
        0.0,		z2,		0.0,									 0.0,
        0.0,        0.0,    (zFar + zNear) / (zNear - zFar),        -1.0,
 		0.0,        0.0,	2.0 * zFar * zNear / (zNear - zFar),	 0.0
	};

	mat4_copy( m, p );
*/
	float	z2 = zFar;
	float	z1 = zNear;


        m[ 0] = p / aspect; m[ 1] = 0.0;	m[ 2] = 0.0;        			m[ 3] =  0.0;
        m[ 4] = 0.0;        m[ 5] = p;    	m[ 6] = 0.0;        			m[ 7] =  0.0;
        m[ 8] = 0.0;        m[ 9] = 0.0;  	m[10] = (z2+z1)         / (z1-z2); 	m[11] = -1.0;
        m[12] = 0.0;    	m[13] = 0.0;	m[14] = (2.0 * z2 * z1) / (z1-z2);  m[15] =  0.0;


/*
float w = z2 / aspect;
float h = z2;
float c = (zFar + zNear) / (zNear - zFar);
float d = 2.0 * zFar * zNear / (zNear - zFar);
          w,	0.0,	 0.0,	 0.0,
        0.0,	  h,	 0.0,	 0.0,
        0.0,    0.0,       c,      d,
 		0.0,    0.0,	-1.0,	 0.0
	};

	mat4_copy( m, p );
*/

}
//-----------------------------------------------------------------------------
void mat4_ray_perspective (float* m, float fovy, float aspect )
//-----------------------------------------------------------------------------
{
        float	p = 1.0f / tan(fovy * (pi/360.0));
		float	ofs_x = 0.0;	//x	offset-x
		float	ofs_y = 0.0;	//y	offset-y
		float	ofs_z = -p;		//z	offset-z


        m[ 0] = 1.0* aspect;m[ 1] = 0.0;    m[ 2] = 0.0;    m[ 3] = 0.0;
        m[ 4] = 0.0;  		m[ 5] = 1.0;    m[ 6] = 0.0;    m[ 7] = 0.0;
        m[ 8] = 0.0;        m[ 9] = 0.0;    m[10] = 1.0;    m[11] = 0.0;
        m[12] = ofs_x;      m[13] = ofs_y;	m[14] = ofs_z;	m[15] = 0.0;

}

//-----------------------------------------------------------------------------
void mat4_glFrustumf ( float* m,  float x1, float x2, float y1, float y2, float z1, float z2)
//-----------------------------------------------------------------------------
{
    m[ 0] = (2*z1) / (x2-x1);	m[ 1] = 0.0;         	m[ 2] = 0.0;        		m[ 3] = 0.0;
    m[ 4] = 0.0;       	 	m[ 5] = (2*z1) / (y2-y1);	m[ 6] = 0.0;        		m[ 7] = 0.0;
    m[ 8] = (x2+x1) / (x2-x1);	m[ 9] = (y2+y1) / (y2-y1); 	m[10] = -(z2+z1) / (z2-z1);     m[11] =-1.0;
    m[12] = 0.0;         	m[13] = 0.0;         	m[14] = -(2*z1*z2) / (z2-z1);   m[15] = 0.0;

}
//-----------------------------------------------------------------------------
void	mat4_frustum( float* m, float x1, float x2, float y1, float y2, float z1, float z2 )
//-----------------------------------------------------------------------------
{
	float sx =       (2.0f * z1) / (x2-x1);
	float sy =       (2.0f * z1) / (y2-y1);
	float mx =           (x2+x1) / (x2-x1);
	float my =           (y2+y1) / (y2-y1);
	float sz =          -(z2+z1) / (z2-z1);
	float w  = -(2.0 * z2 * z1 ) / (z2-z1);
	m[M00] =   sx;	m[M01] = 0.0f;	m[M02] =  mx;	m[M03] = 0.0f;
	m[M10] = 0.0f;	m[M11] =   sy;	m[M12] =  my;	m[M13] = 0.0f;
	m[M20] = 0.0f;	m[M21] = 0.0f;	m[M22] =  sz;	m[M23] =    w;
	m[M30] = 0.0f;	m[M31] = 0.0f;	m[M32] =-1.0f;	m[M33] = 0.0f;
}
//-----------------------------------------------------------------------------
void	mat4_Frustum( mat44& m, float x1, float x2, float y1, float y2, float z1, float z2 )
//-----------------------------------------------------------------------------
{
	float* a = m.m_array;


	float sx =    2.0f / (x2-x1);
	float sy =    2.0f / (y2-y1);
	float sz =    2.0f / (z2-z1);
//	float tx = (x2+x1) / (x2-x1);
//	float ty = (y2+y1) / (y2-y1);
//	float tz = (z2+z1) / (z2-z1);
	
	float w = 1.0f/z1;
	
sx = 1.0f;
sy = 1.0f;
sz = 1.0f;
w = 1.0f/2.0f;


	a[M00] =   sx;	a[M01] = 0.0f;	a[M02] = 0.0f;	a[M03] = 0.0f;
	a[M10] = 0.0f;	a[M11] =   sy;	a[M12] = 0.0f;	a[M13] = 0.0f;
	a[M20] = 0.0f;	a[M21] = 0.0f;	a[M22] =   sz;	a[M23] =    w;
	a[M30] = 0.0f;	a[M31] = 0.0f;	a[M32] = 0.0f;	a[M33] = 0.0f;

}
//-----------------------------------------------------------------------------
void	mat4_Ortho( mat44& m, float x1, float x2, float y1, float y2, float z1, float z2 )
//-----------------------------------------------------------------------------
{
	float* a = m.m_array;

	float sx =    2.0f / (x2-x1);
	float sy =    2.0f / (y2-y1);
	float sz =    2.0f / (z2-z1);
	float tx = (x2+x1) / (x2-x1);
	float ty = (y2+y1) / (y2-y1);
	float tz = (z2+z1) / (z2-z1);
	a[M00] =   sx;	a[M01] = 0.0f;	a[M02] = 0.0f;	a[M03] = 0.0f;
	a[M10] = 0.0f;	a[M11] =   sy;	a[M12] = 0.0f;	a[M13] = 0.0f;
	a[M20] = 0.0f;	a[M21] = 0.0f;	a[M22] =   sz;	a[M23] = 0.0f;
	a[M30] =   tx;	a[M31] =   ty;	a[M32] =   tz;	a[M33] = 1.0f;
}
//-----------------------------------------------------------------------------
void	mat4_Ortho( float* m, float x1, float x2, float y1, float y2, float z1, float z2 )
//-----------------------------------------------------------------------------
{

	float sx =    2.0f / (x2-x1);
	float sy =    2.0f / (y2-y1);
	float sz =    2.0f / (z2-z1);
	float tx = (x2+x1) / (x2-x1);
	float ty = (y2+y1) / (y2-y1);
	float tz = (z2+z1) / (z2-z1);
	m[M00] =   sx;	m[M01] = 0.0f;	m[M02] = 0.0f;	m[M03] = 0.0f;
	m[M10] = 0.0f;	m[M11] =   sy;	m[M12] = 0.0f;	m[M13] = 0.0f;
	m[M20] = 0.0f;	m[M21] = 0.0f;	m[M22] =   sz;	m[M23] = 0.0f;
	m[M30] =   tx;	m[M31] =   ty;	m[M32] =   tz;	m[M33] = 1.0f;
}

//-----------------------------------------------------------------------------
void	mat4_multiply( float* m, const float* a, const float* y1 )
//-----------------------------------------------------------------------------
{

	float	a00 = a[M00];
	float	a01 = a[M01];
	float	a02 = a[M02];
	float	a03 = a[M03];
	float	a10 = a[M10];
	float	a11 = a[M11];
	float	a12 = a[M12];
	float	a13 = a[M13];
	float	a20 = a[M20];
	float	a21 = a[M21];
	float	a22 = a[M22];
	float	a23 = a[M23];
	float	a30 = a[M30];
	float	a31 = a[M31];
	float	a32 = a[M32];
	float	a33 = a[M33];

	float	b00 = y1[M00];
	float	b01 = y1[M01];
	float	b02 = y1[M02];
	float	b03 = y1[M03];
	float	b10 = y1[M10];
	float	b11 = y1[M11];
	float	b12 = y1[M12];
	float	b13 = y1[M13];
	float	b20 = y1[M20];
	float	b21 = y1[M21];
	float	b22 = y1[M22];
	float	b23 = y1[M23];
	float	b30 = y1[M30];
	float	b31 = y1[M31];
	float	b32 = y1[M32];
	float	b33 = y1[M33];

	m[M00] = a00 * b00 +  a01 * b10 +  a02 * b20 +  a03 * b30;
	m[M01] = a00 * b01 +  a01 * b11 +  a02 * b21 +  a03 * b31;
	m[M02] = a00 * b02 +  a01 * b12 +  a02 * b22 +  a03 * b32;
	m[M03] = a00 * b03 +  a01 * b13 +  a02 * b23 +  a03 * b33;

	m[M10] = a10 * b00 +  a11 * b10 +  a12 * b20 +  a13 * b30;
	m[M11] = a10 * b01 +  a11 * b11 +  a12 * b21 +  a13 * b31;
	m[M12] = a10 * b02 +  a11 * b12 +  a12 * b22 +  a13 * b32;
	m[M13] = a10 * b03 +  a11 * b13 +  a12 * b23 +  a13 * b33;

	m[M20] = a20 * b00 +  a21 * b10 +  a22 * b20 +  a23 * b30;
	m[M21] = a20 * b01 +  a21 * b11 +  a22 * b21 +  a23 * b31;
	m[M22] = a20 * b02 +  a21 * b12 +  a22 * b22 +  a23 * b32;
	m[M23] = a20 * b03 +  a21 * b13 +  a22 * b23 +  a23 * b33;

	m[M30] = a30 * b00 +  a31 * b10 +  a32 * b20 +  a33 * b30;
	m[M31] = a30 * b01 +  a31 * b11 +  a32 * b21 +  a33 * b31;
	m[M32] = a30 * b02 +  a31 * b12 +  a32 * b22 +  a33 * b32;
	m[M33] = a30 * b03 +  a31 * b13 +  a32 * b23 +  a33 * b33;
}
//-----------------------------------------------------------------------------
void	mat4_sub( float* m, float* a, float* y1 )
//-----------------------------------------------------------------------------
{
	m[M00] = a[M00]-y1[M00];
	m[M01] = a[M01]-y1[M01];
	m[M02] = a[M02]-y1[M02];
	m[M03] = a[M03]-y1[M03];

	m[M10] = a[M10]-y1[M10];
	m[M11] = a[M11]-y1[M11];
	m[M12] = a[M12]-y1[M12];
	m[M13] = a[M13]-y1[M13];

	m[M20] = a[M20]-y1[M20];
	m[M21] = a[M21]-y1[M21];
	m[M22] = a[M22]-y1[M22];
	m[M23] = a[M23]-y1[M23];

	m[M30] = a[M30]-y1[M30];
	m[M31] = a[M31]-y1[M31];
	m[M32] = a[M32]-y1[M32];
	m[M33] = a[M33]-y1[M33];
}
//-----------------------------------------------------------------------------
void	mat4_add( float* m, float* a, float* y1 )
//-----------------------------------------------------------------------------
{
	m[M00] = a[M00]+y1[M00];
	m[M01] = a[M01]+y1[M01];
	m[M02] = a[M02]+y1[M02];
	m[M03] = a[M03]+y1[M03];

	m[M10] = a[M10]+y1[M10];
	m[M11] = a[M11]+y1[M11];
	m[M12] = a[M12]+y1[M12];
	m[M13] = a[M13]+y1[M13];

	m[M20] = a[M20]+y1[M20];
	m[M21] = a[M21]+y1[M21];
	m[M22] = a[M22]+y1[M22];
	m[M23] = a[M23]+y1[M23];

	m[M30] = a[M30]+y1[M30];
	m[M31] = a[M31]+y1[M31];
	m[M32] = a[M32]+y1[M32];
	m[M33] = a[M33]+y1[M33];
}
//-----------------------------------------------------------------------------
void	mat4_div( float* m, float z2 )
//-----------------------------------------------------------------------------
{
	m[M00] /= z2;
	m[M01] /= z2;
	m[M02] /= z2;
	m[M03] /= z2;

	m[M10] /= z2;
	m[M11] /= z2;
	m[M12] /= z2;
	m[M13] /= z2;

	m[M20] /= z2;
	m[M21] /= z2;
	m[M22] /= z2;
	m[M23] /= z2;

	m[M30] /= z2;
	m[M31] /= z2;
	m[M32] /= z2;
	m[M33] /= z2;
}
//-----------------------------------------------------------------------------
void	mat4_copy( float* m, float a[16] )
//-----------------------------------------------------------------------------
{
//	memcpy( m, a, sizeof(float[16]) );
	m[0] = a[0];
	m[1] = a[1]; 
	m[2] = a[2]; 
	m[3] = a[3]; 
	m[4] = a[4]; 
	m[5] = a[5]; 
	m[6] = a[6]; 
	m[7] = a[7]; 
	m[8] = a[8]; 
	m[9] = a[9]; 
	m[10] = a[10]; 
	m[11] = a[11]; 
	m[12] = a[12]; 
	m[13] = a[13]; 
	m[14] = a[14]; 
	m[15] = a[15]; 

}
//-----------------------------------------------------------------------------
void	mat4_identity( mat44& m )
//-----------------------------------------------------------------------------
{
	mat4_identity( m.GetArray() );
}
//-----------------------------------------------------------------------------
void	mat4_identity( float* m )
//-----------------------------------------------------------------------------
{
	m[M00] = 1.0f;	m[M01] = 0.0f;	m[M02] = 0.0f;	m[M03] = 0.0f;
	m[M10] = 0.0f;	m[M11] = 1.0f;	m[M12] = 0.0f;	m[M13] = 0.0f;
	m[M20] = 0.0f;	m[M21] = 0.0f;	m[M22] = 1.0f;	m[M23] = 0.0f;
	m[M30] = 0.0f;	m[M31] = 0.0f;	m[M32] = 0.0f;	m[M33] = 1.0f;
}
//-----------------------------------------------------------------------------
void	mat4_transpose( float* m, float a[16] )
//-----------------------------------------------------------------------------
{
	float	a00 = a[M00];
	float	a01 = a[M01];
	float	a02 = a[M02];
	float	a03 = a[M03];
	float	a10 = a[M10];
	float	a11 = a[M11];
	float	a12 = a[M12];
	float	a13 = a[M13];
	float	a20 = a[M20];
	float	a21 = a[M21];
	float	a22 = a[M22];
	float	a23 = a[M23];
	float	a30 = a[M30];
	float	a31 = a[M31];
	float	a32 = a[M32];
	float	a33 = a[M33];

	m[M00] = a00;
	m[M01] = a10;
	m[M02] = a20;
	m[M03] = a30;

	m[M10] = a01;
	m[M11] = a11;
	m[M12] = a21;
	m[M13] = a31;

	m[M20] = a02;
	m[M21] = a12;
	m[M22] = a22;
	m[M23] = a32;

	m[M30] = a03;
	m[M31] = a13;
	m[M32] = a23;
	m[M33] = a33;
}
//-----------------------------------------------------------------------------
void	mat4_getScale( float* m, float sx, float sy, float sz  )
//-----------------------------------------------------------------------------
{
	m[M00] =   sx;	m[M01] = 0.0f;	m[M02] = 0.0f;	m[M03] = 0.0f;
	m[M10] = 0.0f;	m[M11] =   sy;	m[M12] = 0.0f;	m[M13] = 0.0f;
	m[M20] = 0.0f;	m[M21] = 0.0f;	m[M22] =   sz;	m[M23] = 0.0f;
	m[M30] = 0.0f;	m[M31] = 0.0f;	m[M32] = 0.0f;	m[M33] = 1.0f;
}
//-----------------------------------------------------------------------------
void	mat4_getTranslate( float* m, float x, float y, float z )
//-----------------------------------------------------------------------------
{
	m[M00] = 1.0f;	m[M01] = 0.0f;	m[M02] = 0.0f;	m[M03] = 0.0f;
	m[M10] = 0.0f;	m[M11] = 1.0f;	m[M12] = 0.0f;	m[M13] = 0.0f;
	m[M20] = 0.0f;	m[M21] = 0.0f;	m[M22] = 1.0f;	m[M23] = 0.0f;
	m[M30] =   x;	m[M31] =   y;	m[M32] =   z;	m[M33] = 1.0f;
}
//-----------------------------------------------------------------------------
mat44	mat4_GetTranslate( float x, float y, float z )
//-----------------------------------------------------------------------------
{
	return mat44(
		1.0f,	0.0f,	0.0f,	0.0f,
		0.0f,	1.0f,	0.0f,	0.0f,
		0.0f,	0.0f,	1.0f,	0.0f,
		  x,	  y,	  z,	1.0f
	);
}
//-----------------------------------------------------------------------------
void	mat4_scaling( float* m, float sx, float sy, float sz  )
//-----------------------------------------------------------------------------
{
	float a[16];
	mat4_getScale( a, sx, sy, sz  );
	mat4_multiply( m, m, a );
}
//-----------------------------------------------------------------------------
void	mat4_scaling( float* m, vect3 vecScale  )
//-----------------------------------------------------------------------------
{
	float a[16];
	mat4_getScale( a, vecScale.x, vecScale.y, vecScale.z  );
	mat4_multiply( m, m, a );
}
//-----------------------------------------------------------------------------
void	mat4_scaling( mat44& m, vect3 vecScale  )
//-----------------------------------------------------------------------------
{
	mat4_scaling( m.GetArray(), vecScale.x, vecScale.y, vecScale.z  );
}
//-----------------------------------------------------------------------------
void	mat4_scaling( mat44& m, float sx, float sy, float sz   )
//-----------------------------------------------------------------------------
{
	mat4_scaling( m.GetArray(), sx, sy, sz  );
}
//-----------------------------------------------------------------------------
void	mat4_translate( mat44& m, float sx, float sy, float sz   )
//-----------------------------------------------------------------------------
{
	mat4_translate( m.GetArray(), sx, sy, sz  );
}
//-----------------------------------------------------------------------------
void	mat4_translate( float* m, float x, float y, float z )
//-----------------------------------------------------------------------------
{
	m[M30] += x;
	m[M31] += y;
	m[M32] += z;
}

//-----------------------------------------------------------------------------
void	mat4_translate( mat44& m, vect3 v )
//-----------------------------------------------------------------------------
{
	mat4_translate( m.GetArray(), v );
}
//-----------------------------------------------------------------------------
void	mat4_translate( float* m, vect3 v )
//-----------------------------------------------------------------------------
{
	m[M30] += v.x;
	m[M31] += v.y;
	m[M32] += v.z;
}
//-----------------------------------------------------------------------------
void	mat4_rotateX( mat44& m, float th )
//-----------------------------------------------------------------------------
{
	mat4_rotateX( m.GetArray(), th );
}
//-----------------------------------------------------------------------------
void	mat4_rotateY( mat44& m, float th )
//-----------------------------------------------------------------------------
{
	mat4_rotateY( m.GetArray(), th );
}
//-----------------------------------------------------------------------------
void	mat4_rotateZ( mat44& m, float th )
//-----------------------------------------------------------------------------
{
	mat4_rotateZ( m.GetArray(), th );
}
//-----------------------------------------------------------------------------
void	mat4_rotateX( float* m, float th )
//-----------------------------------------------------------------------------
{
	float a[16];
	mat4_getRotateX(a,th);

	mat4_multiply( m, m, a );
}
//-----------------------------------------------------------------------------
void	mat4_rotateY( float* m, float th )
//-----------------------------------------------------------------------------
{
	float a[16];
	mat4_getRotateY(a,th);

	mat4_multiply( m, m, a );
}
//-----------------------------------------------------------------------------
void	mat4_rotateZ( float* m, float th )
//-----------------------------------------------------------------------------
{
	float a[16];
	mat4_getRotateZ(a,th);

	mat4_multiply( m, m, a );
}
//-----------------------------------------------------------------------------
void	mat4_rotateZ( float* m, float a[16], float th )
//-----------------------------------------------------------------------------
{
	float	c = cos(th);
	float	s = sin(th);

	m[M00] =   c;	m[M01] =   -s;	m[M02] = 0.0f;	m[M03] = 0.0f;
	m[M10] =   s;	m[M11] =    c;	m[M12] = 0.0f;	m[M13] = 0.0f;
	m[M20] = 0.0f;	m[M21] = 0.0f;	m[M22] = 1.0f;	m[M23] = 0.0f;
	m[M30] = 0.0f;	m[M31] = 0.0f;	m[M32] = 0.0f;	m[M33] = 1.0f;

	mat4_multiply( m, a, m );
}

//-----------------------------------------------------------------------------
void	mat4_getRotateX( float* m, float th )
//-----------------------------------------------------------------------------
{
	float	c = cos(th);
	float	s = sin(th);

	m[M00] = 1.0f;	m[M01] = 0.0f;	m[M02] = 0.0f;	m[M03] = 0.0f;
	m[M10] = 0.0f;	m[M11] =   c;	m[M12] =  -s;	m[M13] = 0.0f;
	m[M20] = 0.0f;	m[M21] =   s;	m[M22] =   c;	m[M23] = 0.0f;
	m[M30] = 0.0f;	m[M31] = 0.0f;	m[M32] = 0.0f;	m[M33] = 1.0f;
}
//-----------------------------------------------------------------------------
void	mat4_getRotateY( float* m, float th )
//-----------------------------------------------------------------------------
{
	float	c = cos(th);
	float	s = sin(th);

	m[M00] =   c;	m[M01] = 0.0f;	m[M02] =  -s;	m[M03] = 0.0f;
	m[M10] = 0.0f;	m[M11] = 1.0f;	m[M12] = 0.0f;	m[M13] = 0.0f;
	m[M20] =   s;	m[M21] = 0.0f;	m[M22] =   c;	m[M23] = 0.0f;
	m[M30] = 0.0f;	m[M31] = 0.0f;	m[M32] = 0.0f;	m[M33] = 1.0f;
}
//-----------------------------------------------------------------------------
void	mat4_getRotateZ( float* m, float th )
//-----------------------------------------------------------------------------
{
	float	c = cos(th);
	float	s = sin(th);

	m[M00] =   c;	m[M01] =   -s;	m[M02] = 0.0f;	m[M03] = 0.0f;
	m[M10] =   s;	m[M11] =    c;	m[M12] = 0.0f;	m[M13] = 0.0f;
	m[M20] = 0.0f;	m[M21] = 0.0f;	m[M22] = 1.0f;	m[M23] = 0.0f;
	m[M30] = 0.0f;	m[M31] = 0.0f;	m[M32] = 0.0f;	m[M33] = 1.0f;
}
//-----------------------------------------------------------------------------
mat44	mat4_GetRotateX( float th )
//-----------------------------------------------------------------------------
{
	float	c = cos(th);
	float	s = sin(th);

	return mat44(
		1.0f,	0.0f,	0.0f,	0.0f,
		0.0f,	   c,	  -s,	0.0f,
		0.0f,	   s,	   c,	0.0f,
		0.0f,	0.0f,	0.0f,	1.0f
	);
}
//-----------------------------------------------------------------------------
mat44	mat4_GetRotateY( float th )
//-----------------------------------------------------------------------------
{
	float	c = cos(th);
	float	s = sin(th);

	return mat44(
		   c,	0.0f,	  -s,	0.0f,
		0.0f,	1.0f,	0.0f,	0.0f,
		   s,	0.0f,	   c,	0.0f,
		0.0f,	0.0f,	0.0f,	1.0f
	);
}
//-----------------------------------------------------------------------------
mat44	mat4_GetRotateZ( float th )
//-----------------------------------------------------------------------------
{
	float	c = cos(th);
	float	s = sin(th);

	return mat44(
		   c,	  -s,	0.0f,	0.0f,
		   s,	   c,	0.0f,	0.0f,
		0.0f,	0.0f,	1.0f,	0.0f,
		0.0f,	0.0f,	0.0f,	1.0f
	);
}

//-----------------------------------------------------------------------------
void	mat4_copyMatrix( float* m, float* a )
//-----------------------------------------------------------------------------
{
	m[M00] = a[M00];
	m[M01] = a[M01];
	m[M02] = a[M02];
	m[M03] = a[M03];
	m[M10] = a[M10];
	m[M11] = a[M11];
	m[M12] = a[M12];
	m[M13] = a[M13];
	m[M20] = a[M20];
	m[M21] = a[M21];
	m[M22] = a[M22];
	m[M23] = a[M23];
	m[M30] = a[M30];
	m[M31] = a[M31];
	m[M32] = a[M32];
	m[M33] = a[M33];

}

//-----------------------------------------------------------------------------
float atan2_360( float y, float x )
//-----------------------------------------------------------------------------
{
//	return	((n)*pi/180.0f);
	return 0;
}
//-----------------------------------------------------------------------------
float deg2rad( float n )
//-----------------------------------------------------------------------------
{
	return	((n)*pi/180.0f);
}

//-----------------------------------------------------------------------------
float	rad2deg( float n)
//-----------------------------------------------------------------------------
{
	return ((n)*180.0f/pi);
}

//-----------------------------------------------------------------------------
mat33 midentity()
//-----------------------------------------------------------------------------
{
	return mat33(
		1,  0,  0,
		0,  1,  0,
		0,  0,  1
	);
}

///////////////////////////////////////
//	                                 //
//      マトリクスの回転の関係性     //
//	 	             	             //
//                y ->               //
//                .		             //
//                .		             //
//                .		             //
//                .		             //
//          ^   .   .   		     //
//          | .       . 	         //
//          x        <- z 	         //
//	                                 //
//	         ※左ねじの法則          //
//	                                 //
///////////////////////////////////////

//-----------------------------------------------------------------------------
mat33 mrotx( float f )
//-----------------------------------------------------------------------------
{
	// f : atan2(Z,Y)=atan2(0,1)=0	Y->Zへ回転

	float	c = cos(f);
	float	s = sin(f);
	return mat33(
		1,  0,  0,
		0,  c,  s,
		0, -s,  c
	);
}

//-----------------------------------------------------------------------------
mat33 mroty( float f )
//-----------------------------------------------------------------------------
{
	// f : atan2(X,Z)=atan2(0,1)=0	Z->Xへ回転

	float	c = cos(f);
	float	s = sin(f);
	return mat33(
		c,  0, -s,
		0,  1,  0,
	    s,  0,  c
	);
}

//-----------------------------------------------------------------------------
mat33 mrotz( float f )
//-----------------------------------------------------------------------------
{
	// f : atan2(Y,X)=atan2(0,1)=0	X->Yへ回転

	float	c = cos(f);
	float	s = sin(f);
	return mat33(
		c,  s,  0,
	   -s,	c,  0,
		0,  0,  1
	);
}
