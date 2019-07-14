main.o: main.cpp geom.h SysKeys.h SysMouse.h SysGra.h Sys.h
geom.o: geom.cpp geom.h
Sys.o: Sys.cpp SysKeys.h SysMouse.h SysGra.h Sys.h SysWin.h
Syswin.o: Syswin.cpp SysWin.h
SysGra.o: SysGra.cpp SysGra.h SysWin.h
SysKeys.o: SysKeys.cpp SysKeys.h
SysMouse.o: SysMouse.cpp SysMouse.h
