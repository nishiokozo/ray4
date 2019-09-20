extern tuple<bool,float,vect3,vect3>lengthLineLine_func( vect3 P0, vect3 I0, vect3 P1, vect3 I1 );
extern vect3 bezier_func( float t, vect3 P0, vect3 P1, vect3 P2, vect3 P3 );
extern vect3 catmull3d_func( float t, const vect3 P0, const vect3 P1, const vect3 P2, const vect3 P3 );
extern void g_line3d( SysGra& gra, Pers& pers, vect3 p0, vect3 p1, rgb col, bool bScissor = false );
extern bool IsIntersectPlate( vect3 plate_P, vect3 plate_N, vect3 P , vect3 I, vect3& Q);
extern bool IsIntersectSphereLine( vect3 sphere_P, float sphere_r, vect3 line_P , vect3 line_I );

