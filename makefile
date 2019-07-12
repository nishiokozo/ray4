# 2019/07/04   msys64 / GDI

TAR = main.exe

SRCS = \
	main.cpp \
	geom.cpp \
	plat.cpp \
	syswin.cpp \
	sysgdi.cpp \
	sysgra.cpp \
	syskeys.cpp \
	sysmouse.cpp \

LIBS = \
	 -lgdi32

FLGS = \
	-g \
	-c \
	-m64 \
	-std=c++17 \
	-Wall \
	-Wno-unused-function \
	-Wno-unused-variable \
	-Wno-unused-value \
	-Wno-unused-but-set-variable \

#	-O3 \
#	-Wno-unknown-pragmas \
#	-Werror \

#CC	= clang++
CC	= g++

$(TAR)	:	$(SRCS:.cpp=.o)
	$(CC) -o $(TAR) $(SRCS:.cpp=.o) $(LIBS)

geom.o:geom.cpp
	$(CC)   -O1 $(FLGS) $< -o $@ 

%.o:%.cpp
	$(CC)   $(FLGS) $< -o $@ 

clean:
	rm -f *.exe
	rm -f *.o
	rm -f *.*.stackdump
	rm mm.txt
	touch mm.txt
	make mm

mm:	$(SRCS)
	$(CC)   -MM $? >mm.txt

include mm.txt

