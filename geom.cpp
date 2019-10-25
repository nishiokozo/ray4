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

#define	M00	0
#define	M01	1
#define	M02	2
#define	M03	3
#define	M10	4
#define	M11	5
#define	M12	6
#define	M13	7
#define	M20	8
#define	M21	9
#define	M22	10
#define	M23	11
#define	M30	12
#define	M31	13
#define	M32	14
#define	M33	15

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
