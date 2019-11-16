main.o: main.cpp geom.h SysKeys.h SysMouse.h SysGra.h Sys.h func.h pers.h \
  gui.h raytrace.h skeleton.h lab.h
skeleton.o: skeleton.cpp geom.h SysGra.h func.h pers.h gui.h skeleton.h
raytrace.o: raytrace.cpp geom.h SysGra.h raytrace.h
pers.o: pers.cpp geom.h SysGra.h func.h pers.h
func.o: func.cpp geom.h func.h
geom.o: geom.cpp geom.h
gui.o: gui.cpp geom.h SysKeys.h SysMouse.h SysGra.h Sys.h func.h pers.h \
  gui.h
lab.o: lab.cpp geom.h SysKeys.h SysMouse.h SysGra.h func.h pers.h gui.h \
  lab.h
lab12.o: lab12.cpp geom.h SysKeys.h SysMouse.h SysGra.h func.h pers.h \
  gui.h lab.h
Sys.o: Sys.cpp SysKeys.h SysMouse.h geom.h SysGra.h Sys.h SysWin.h
Syswin.o: Syswin.cpp SysWin.h
SysGra.o: SysGra.cpp SysGra.h geom.h SysWin.h
SysKeys.o: SysKeys.cpp SysKeys.h
SysMouse.o: SysMouse.cpp SysMouse.h geom.h SysWin.h
