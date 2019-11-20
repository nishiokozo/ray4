extern float func_distanceLinePoint( vect3 P0, vect3 I0, vect3 P1 );
extern std::tuple<bool,float,vect3,vect3,float,float>func_distance_Harfline_Segline( vect3 s0, vect3 I0, vect3 s1, vect3 e1 );
extern std::tuple<bool,float,vect3,vect3,float,float>func_distance_Segline_Segline( vect3 s0, vect3 e0, vect3 s1, vect3 e1 );
extern std::tuple<bool,float,vect3,vect3,float,float>func_distance_Line_Line( vect3 P0, vect3 I0, vect3 P1, vect3 I1 );
extern std::tuple<bool,float,vect3,vect3,float,float>func_distance_Line_Segline( vect3 P0, vect3 I0, vect3 s1, vect3 e1 );
extern vect3 func_bezier3( float t, vect3 P0, vect3 P1, vect3 P2, vect3 P3 );
extern vect3 func_bezier3_delta( float t, vect3 P0, vect3 P1, vect3 P2, vect3 P3 );
extern vect3 func_catmull3( float t, const vect3 P0, const vect3 P1, const vect3 P2, const vect3 P3 );
extern std::tuple<bool,vect3,float> func_distance_Plate_Segline( vect3 plate_P, vect3 plate_N, vect3 line_P, vect3 line_Q );
extern std::tuple<bool,vect3,float> func_distance_Plate_Harfline( vect3 plate_P, vect3 plate_N, vect3 P, vect3 I);
extern bool func_IsIntersectSphereLine( vect3 sphere_P, float sphere_r, vect3 line_P , vect3 line_I );

extern float func_accelerationGetTime_DVv( float vg, float d, float v0 );
extern vect3 func_accelerationGetDistance_TVv( vect3 vg, float t, vect3 v0 );
