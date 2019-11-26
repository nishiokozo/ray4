extern float func_intersect_Line_Point( vect3 P0, vect3 I0, vect3 P1 );
extern std::tuple<bool,float,vect3,vect3,float,float>func_intersect_HarfLine_SegLine( vect3 s0, vect3 I0, vect3 s1, vect3 e1 );
extern std::tuple<bool,float,vect3,vect3,float,float>func_intersect_SegLine_SegLine( vect3 s0, vect3 e0, vect3 s1, vect3 e1 );
extern std::tuple<bool,float,vect3,vect3,float,float>func_intersect_Line_Line( vect3 P0, vect3 I0, vect3 P1, vect3 I1 );
extern std::tuple<bool,float,vect3,vect3,float,float>func_intersect_Line_SegLine( vect3 P0, vect3 I0, vect3 s1, vect3 e1 );
extern vect3 func_bezier3( float t, vect3 P0, vect3 P1, vect3 P2, vect3 P3 );
extern vect3 func_bezier3_delta( float t, vect3 P0, vect3 P1, vect3 P2, vect3 P3 );
extern vect3 func_catmull3( float t, const vect3 P0, const vect3 P1, const vect3 P2, const vect3 P3 );
extern std::tuple<bool,vect3,float> func_intersect_Plate_SegLine( vect3 plate_P, vect3 plate_N, vect3 line_P, vect3 line_Q );
extern std::tuple<bool,vect3,float> func_intersect_Plate_HarfLine( vect3 plate_P, vect3 plate_N, vect3 P, vect3 I);
extern bool func_IsIntersectSphereLine( vect3 sphere_P, float sphere_r, vect3 line_P , vect3 line_I );
extern std::tuple<bool,vect3,float> func_intersect_Plate_Curve( vect3 P, vect3 N, vect3 P0, vect3 A, vect3 V );
extern std::tuple<bool,vect3,float> func_intersect_Plate_SegCurve( vect3 plate_P, vect3 plate_N, vect3 curve_P0, float curve_t0, float curve_t1, vect3 curve_A, vect3 curve_V );
extern std::tuple<bool,vect3,float>func_intersect_Plate_SegCurve_ball( vect3 plate_P, vect3 plate_N, vect3 curve_P0, float r, float curve_t0, float curve_t1, vect3 curve_A, vect3 curve_V );

extern float func_accelerationGetTime_DVv( float vg, float d, float v0 );
extern vect3 func_accelerationGetDistance_TVv( vect3 vg, float t, vect3 v0 );
