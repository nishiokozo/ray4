extern std::tuple<bool,float,vect3,vect3,float,float>distanceLineLine0( vect3 P0, vect3 I0, vect3 P1, vect3 I1 );
extern std::tuple<bool,float,vect3,vect3,float,float>distanceLineSegline_func( vect3 P0, vect3 I0, vect3 s1, vect3 e1 );
extern std::tuple<bool,float,vect3,vect3>distanceLineLine_func( vect3 P0, vect3 I0, vect3 P1, vect3 I1 );
extern vect3 bezier3_func( float t, vect3 P0, vect3 P1, vect3 P2, vect3 P3 );
extern vect3 bezier3_delta_func( float t, vect3 P0, vect3 P1, vect3 P2, vect3 P3 );
extern vect3 catmull3_func( float t, const vect3 P0, const vect3 P1, const vect3 P2, const vect3 P3 );
extern bool IsIntersectPlate( vect3 plate_P, vect3 plate_N, vect3 P , vect3 I, vect3& Q);
extern bool IsIntersectSphereLine( vect3 sphere_P, float sphere_r, vect3 line_P , vect3 line_I );
//
//extern void g_pset3d( SysGra& gra, Pers& pers, vect3 p0, rgb col, float wide = 1.0f );
//extern void g_line3d( SysGra& gra, Pers& pers, vect3 p0, vect3 p1, rgb col, float wide = 1.0f );
//extern void g_line3d_scissor( SysGra& gra, Pers& pers, vect3 p0, vect3 p1, rgb col, float wide = 1.0f );

