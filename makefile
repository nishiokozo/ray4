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
	lab12.cpp \
	Sys.cpp \
	Syswin.cpp \
	SysGra.cpp \
	SysKeys.cpp \
	SysMouse.cpp \

LIBS = \
	-lgdi32 \
	-lopengl32 \

FLGS = \
	-c -g \
	-std=c++17 \
	-Werror \
	-Wall -Wno-unused -Wconversion -Wno-sign-conversion -Wno-shorten-64-to-32 -Wno-implicit-float-conversion

#	-Wall -Wno-unused -Wconversion -Wno-sign-conversion -Wno-float-conversion -Wno-shorten-64-to-32 -Wno-implicit-float-conversion
#	-Wall -Wno-unused -Wconversion -Wno-sign-conversion -Wno-shorten-64-to-32 -Wno-implicit-float-conversion -Wno-float-conversion

FILE=main.o

#	-Wno-unused-variable \
#	-Wno-unused-function \
#	-Wno-unused-value \
#	-Wno-unused-but-set-variable \

#	-g \
#	-m64 \

#	-O3 \
#	-Wno-unknown-pragmas \
#	-Werror \

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

