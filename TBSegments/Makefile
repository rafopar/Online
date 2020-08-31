## -*- Makefile -*-
##
## User: rafopar
## Time: Aug 19, 2020 9:20:35 PM


#### Compiler and tool definitions shared by all build targets #####

CC              = g++ -std=c++11
CC_OBJ_FLAGS    = -c -fPIC
CC_Shared_FLAGS = -shared -Wl,-soname,libTBSegments.so
ROOT_CFLAGS     = $(shell ${ROOTSYS}/bin/root-config --cflags --glibs)
ROOT_LIBS       = $(shell ${ROOTSYS}/bin/root-config --libs)
HIPO_FLAGS	= -L${HIPODIR}/lib -lhipo4 -llz4 -I${HIPODIR}/hipo4
libTBSEGMENTS   = libTBSegments


all:		DCObjects.o SegDisplay.o dict FitDCSegments.cc
		mkdir -p lib ; rm -f lib/*.so
		$(CC) $(CC_Shared_FLAGS) -o lib/${libTBSEGMENTS}.so.1.0.1 DCObjects.o SegDisplay.o
		cd lib;\
		ln -sf ${libTBSEGMENTS}.so.1.0.1 ${libTBSEGMENTS}.so.1; ln -sf ${libTBSEGMENTS}.so.1.0.1 ${libTBSEGMENTS}.so
		cd ../;
		mv src/*.pcm lib
		$(CC) -o FitDCSegments.exe FitDCSegments.cc src/SegDisplayDict.cxx -I ./include -I./ $(ROOT_CFLAGS) $(ROOT_LIBS) $(HIPO_FLAGS) -L./lib -lTBSegments
		
DCObjects.o:	src/DCObjects.cc include/DCObjects.h
		$(CC) $(CC_OBJ_FLAGS) src/DCObjects.cc -o $@ -I./include $(ROOT_CFLAGS) $(ROOT_LIBS)
	
SegDisplay.o:	src/SegDisplay.cc include/SegDisplay.h include/DCObjects.h
		$(CC) $(CC_OBJ_FLAGS) src/SegDisplay.cc -o $@ -I./include $(ROOT_CFLAGS) $(HIPO_FLAGS)
	
dict:		include/SegDisplay.h include/SegDisplayLinkDef.h include/DCObjects.h
		rootcint -f src/SegDisplayDict.cxx -c include/SegDisplay.h $(HIPO_FLAGS) -I./include include/SegDisplayLinkDef.h 
	
clean:		
		rm -f *.o lib/*.so.* lib/*.so src/*.pcm lib/*.pcm