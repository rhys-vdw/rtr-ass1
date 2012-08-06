#Makefile: Sun Jul 22 00:24:15 EST 2012 pknowles */

DEBUG = -g -Wall
#OPTIMISE = -O3

CFLAGS = `sdl-config --cflags` $(DEBUG) $(OPTIMISE)
LDFLAGS = `sdl-config --libs` -lGL -lGLU -lglut -lm -lGLEW
EXE = tute-vbo
OBJECTS = tute-vbo.o sdlbase.o vec3f.o mesh_generation.o

#default target
all: $(EXE)

#executable
$(EXE) : $(OBJECTS)
	gcc -o $@ $(LDFLAGS) $(OBJECTS)

#general object file rule
%.o : %.c
	gcc -c -o $@ $(CFLAGS) $<

#additional dependencies
sdlbase.o : sdlbase.h
tute-vbo.o : sdlbase.h vec3f.h vertex.h mesh_generation.h
mesh_generation.o: mesh_generation.h vec3f.h vertex.h

#clean (-f stops error if files don't exist)
clean:
	rm -f $(EXE) \
	      $(OBJECTS)
