# 2019/07/04   msys64 / GDI
# $@ ルールのターゲットのファイル名。
# $< 最初の依存関係 (ターゲットの右に書かれるファイル名)
# $^ 全ての依存関係の名前のそれぞれの間にスペースを挟んで並べたもの。
# 
TAR = main.exe

SRCS = \
	main.cpp \
	raytrace.cpp \
	geom.cpp \
	Sys.cpp \
	Syswin.cpp \
	SysGra.cpp \
	SysKeys.cpp \
	SysMouse.cpp \

LIBS = \
	 -lgdi32

FLGS = \
	-c -g \
	-std=c++14 \
	-Wall -Werror\
	-Wno-unused \

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

