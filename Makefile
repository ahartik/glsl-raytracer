default: test
CFLAGS=-I lib  -Wall -Os -g
LDFLAGS=-lGL -lGLU -lglut -lGLEW -lSDL ./lib.so

lib.so: 
	g++ lib/*.cpp -shared -fPIC -o lib.so
	

test: lib.so test.cpp
	g++ ${CFLAGS} test.cpp -o test $(LDFLAGS)

