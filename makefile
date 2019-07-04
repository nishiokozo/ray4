# 2019/07/04   msys64 / GDI

TAR = main.exe

SRCS = \
	main.cpp \
	win.cpp \
	geom.cpp \
	keyboard.cpp \

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
	-Wno-unused-but-set-variable \

#	-O3 \
#	-Wno-unknown-pragmas \
#	-Werror \

#CC	= clang++
CC	= g++

$(TAR)	:	$(SRCS:.cpp=.o)
	$(CC) -o $(TAR) $(SRCS:.cpp=.o) $(LIBS)

%.o:%.cpp
	$(CC)   $(FLGS) $< -o $@ 

clean:
	rm -f *.exe
	rm -f *.o
	rm -f *.*.stackdump

mm:	$(SRCS)
	$(CC)   -MM $? >mm.txt

include mm.txt

