extern vect3 bezier_func( float t, vect3 P0, vect3 P1, vect3 P2, vect3 P3 );
extern vect3 catmull3d_func( float t, const vect3 P0, const vect3 P1, const vect3 P2, const vect3 P3 );
extern void g_line3d( SysGra& gra, Pers& pers, vect3 p0, vect3 p1, rgb col, bool bScissor = false );

