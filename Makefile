#server : demo.o server.o
#	g++ demo.o server.o -o server

#client: client.o
#	g++ client.o -o client

#client.o : client.cpp
#	g++ -c client.cpp

#server.o : server.cpp
#	g++ -c server.cpp

#demo.o : demo.cpp
#	g++ -c demo.cpp


#clean:
#	rm -f *.o
#	rm -f *~

# Makefile for Irrlicht Examples
# It's usually sufficient to change just the target name and source file list
# and be sure that CXX is set to a valid compiler
Target = client
Sources = client.cpp

# general compiler settings
CPPFLAGS = -I../../include -I/usr/X11R6/include -I../../sources/Irrlicht -fpermissive
CXXFLAGS = -O3 -ffast-math
#CXXFLAGS = -g -Wall

#default target is Linux
all: all_linux

ifeq ($(HOSTTYPE), x86_64)
LIBSELECT=64
endif

# target specific settings
all_linux: LDFLAGS = -L/usr/X11R6/lib$(LIBSELECT) -L../../lib/Linux -lIrrlicht -lGL -lXxf86vm -lXext -lX11 -lXcursor -lpthread
all_linux clean_linux: SYSTEM=Linux
all_win32: LDFLAGS = -L../../lib/Win32-gcc -lIrrlicht -lopengl32 -lm
all_win32: CPPFLAGS += -D__GNUWIN32__ -D_WIN32 -DWIN32 -D_WINDOWS -D_MBCS -D_USRDLL
all_win32 clean_win32: SYSTEM=Win32-gcc
all_win32 clean_win32: SUF=.exe
# name of the binary - only valid for targets which set SYSTEM
DESTPATH = ./$(Target)$(SUF)

all_linux all_win32:
	$(warning Building...)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(Sources) -o $(DESTPATH) $(LDFLAGS)

clean: clean_linux clean_win32
	$(warning Cleaning...)

clean_linux clean_win32:
	@$(RM) $(DESTPATH)

.PHONY: all all_win32 clean clean_linux clean_win32
