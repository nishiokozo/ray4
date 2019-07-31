# 2019/07/04   msys64 / GDI

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

#	-Wno-unused-variable \
#	-Wno-unused-function \
#	-Wno-unused-value \
#	-Wno-unused-but-set-variable \

#	-g \
#	-m64 \

#	-O3 \
#	-Wno-unknown-pragmas \
#	-Werror \

#CC	= clang++
CC	= g++

$(TAR)	:	$(SRCS:.cpp=.o)
	$(CC) -o $(TAR) $(SRCS:.cpp=.o) $(LIBS)

geom.o:geom.cpp
	$(CC) -o $@ $< $(FLGS) -O1

%.o:%.cpp
	$(CC) -o $@ $< $(FLGS) 

clean:
	rm -f *.exe
	rm -f *.o
	rm -f *.*.stackdump
	rm mm.mk
	touch mm.mk
	make mm

mm:	$(SRCS)
	$(CC)   -MM $? >mm.mk

include mm.mk

