bone.o: bone.cpp geom.h SysGra.h obj.h bone.h
raytrace.o: raytrace.cpp geom.h SysGra.h raytrace.h
geom.o: geom.cpp geom.h
Sys.o: Sys.cpp SysKeys.h SysMouse.h geom.h SysGra.h Sys.h SysWin.h
Syswin.o: Syswin.cpp SysWin.h
SysGra.o: SysGra.cpp SysGra.h geom.h SysWin.h
SysKeys.o: SysKeys.cpp SysKeys.h
SysMouse.o: SysMouse.cpp SysMouse.h geom.h SysWin.h
