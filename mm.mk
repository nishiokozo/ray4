main.o: main.cpp geom.h SysKeys.h SysMouse.h SysGra.h Sys.h func.h pers.h \
  gui.h raytrace.h lab.h
skeleton.o: skeleton.cpp geom.h SysGra.h func.h pers.h skeleton.h
raytrace.o: raytrace.cpp geom.h SysGra.h raytrace.h
pers.o: pers.cpp geom.h SysGra.h func.h pers.h
func.o: func.cpp geom.h func.h
geom.o: geom.cpp geom.h
gui.o: gui.cpp geom.h SysKeys.h SysMouse.h SysGra.h Sys.h func.h pers.h \
  gui.h
lab.o: lab.cpp geom.h SysKeys.h SysMouse.h SysGra.h func.h pers.h lab.h \
  lab0.h lab1.h lab2.h lab3.h lab4.h lab5.h lab6.h lab7.h lab8.h lab9.h \
  lab10.h lab11.h lab12.h lab13.h lab14.h lab15.h lab16.h lab17.h \
  lab18.h
lab0.o: lab0.cpp geom.h SysKeys.h SysMouse.h SysGra.h func.h pers.h lab.h \
  lab0.h
lab1.o: lab1.cpp geom.h SysKeys.h SysMouse.h SysGra.h func.h pers.h lab.h \
  lab1.h
lab2.o: lab2.cpp geom.h SysKeys.h SysMouse.h SysGra.h func.h pers.h lab.h \
  lab2.h
lab3.o: lab3.cpp geom.h SysKeys.h SysMouse.h SysGra.h func.h pers.h lab.h \
  lab3.h
lab4.o: lab4.cpp geom.h SysKeys.h SysMouse.h SysGra.h func.h pers.h lab.h \
  lab4.h
lab5.o: lab5.cpp geom.h SysKeys.h SysMouse.h SysGra.h func.h pers.h lab.h \
  lab5.h
lab6.o: lab6.cpp geom.h SysKeys.h SysMouse.h SysGra.h func.h pers.h lab.h \
  lab6.h
lab7.o: lab7.cpp geom.h SysKeys.h SysMouse.h SysGra.h func.h pers.h lab.h \
  lab7.h
lab8.o: lab8.cpp geom.h SysKeys.h SysMouse.h SysGra.h func.h pers.h lab.h \
  lab8.h
lab9.o: lab9.cpp geom.h SysKeys.h SysMouse.h SysGra.h func.h pers.h lab.h \
  lab9.h
lab10.o: lab10.cpp geom.h SysKeys.h SysMouse.h SysGra.h func.h pers.h \
  lab.h lab10.h
lab11.o: lab11.cpp geom.h SysKeys.h SysMouse.h SysGra.h func.h pers.h \
  lab.h lab11.h
lab12.o: lab12.cpp geom.h SysKeys.h SysMouse.h SysGra.h func.h pers.h \
  lab.h lab12.h
lab13.o: lab13.cpp geom.h SysKeys.h SysMouse.h SysGra.h func.h pers.h \
  lab.h lab13.h
lab14.o: lab14.cpp geom.h SysKeys.h SysMouse.h SysGra.h func.h pers.h \
  lab.h lab14.h
lab15.o: lab15.cpp geom.h SysKeys.h SysMouse.h SysGra.h func.h pers.h \
  lab.h lab15.h
lab16.o: lab16.cpp geom.h SysKeys.h SysMouse.h SysGra.h func.h pers.h \
  lab.h lab16.h skeleton.h
lab17.o: lab17.cpp geom.h SysKeys.h SysMouse.h SysGra.h func.h pers.h \
  lab.h lab17.h
lab18.o: lab18.cpp geom.h SysKeys.h SysMouse.h SysGra.h func.h pers.h \
  lab.h lab18.h
Sys.o: Sys.cpp SysKeys.h SysMouse.h geom.h SysGra.h Sys.h SysWin.h
Syswin.o: Syswin.cpp SysWin.h
SysGra.o: SysGra.cpp SysGra.h geom.h SysWin.h
SysKeys.o: SysKeys.cpp SysKeys.h
SysMouse.o: SysMouse.cpp SysMouse.h geom.h SysWin.h
