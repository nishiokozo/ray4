# 2019/07/04   msys64 / GDI
# $@ ルールのターゲットのファイル名。
# $< 最初の依存関係 (ターゲットの右に書かれるファイル名)
# $^ 全ての依存関係の名前のそれぞれの間にスペースを挟んで並べたもの。
# 
TAR = main.exe

SRCS = \
	main.cpp \
	skeleton.cpp \
	raytrace.cpp \
	pers.cpp \
	func.cpp \
	geom.cpp \
	gui.cpp \
	lab.cpp \
	lab0.cpp \
	lab1.cpp \
	lab2.cpp \
	lab3.cpp \
	lab4.cpp \
	lab5.cpp \
	lab6.cpp \
	lab7.cpp \
	lab8.cpp \
	lab9.cpp \
	lab10.cpp \
	lab11.cpp \
	lab12.cpp \
	lab13.cpp \
	lab14.cpp \
	lab15.cpp \
	lab16.cpp \
	lab17.cpp \
	lab18.cpp \
	lab19.cpp \
	lab20.cpp \
	lab21.cpp \
	lab22.cpp \
	lab23.cpp \
	lab24.cpp \
	lab25.cpp \
	lab26.cpp \
	lab27.cpp \
	lab28.cpp \
	Sys.cpp \
	SysWin.cpp \
	SysGra.cpp \
	SysKeys.cpp \
	SysMouse.cpp \
	SysSound.cpp \

LIBS = \
	-lgdi32 \
	-lopengl32 \
	-lwinmm

FLGS = \
	-c \
	-g \
	-std=c++17 \
	-Werror \
	-Wall -Wconversion -Wno-float-conversion -Wno-sign-conversion -Wno-implicit-float-conversion \
	-Wno-unused\

FILE=main.o

CC	= clang++
#CC	= g++

$(TAR)	:	$(SRCS:.cpp=.o)
	$(CC) -o $(TAR) $^ $(LIBS)

%.o:%.cpp
	$(CC) -o $@ $< $(FLGS) 

cleanall:clean
	rm mm.mk
	touch mm.mk
	make mm

clean:
	rm -f *.exe
	rm -f *.o
	rm -f *.*.stackdump

mm:	$(SRCS)
	echo $^
	$(CC)   -MM $^ >mm.mk

include mm.mk

