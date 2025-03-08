

############################################################################
all: build mcboardbot
############################################################################



############################################################################
CC = c++
CFLAGS		= -c -x c++
CINCDIR		= 

LOAD		= g++
LDFLAGS		= -lm
CLIBS		=

ALL_INCDIRS	=$(CINCDIR)
ALL_LIBDIRS	= -L/usr/local/lib
ALL_LIBS	=$(CLIBS)
############################################################################



############################################################################
# Define what source files to use and how to derive object file names, etc.
HEADERS		= md5c.h md5class.h md5global.h SFIni.h SFIniNews.h stdafx.h
SOURCE		= md5c.cpp md5class.cpp newsboard.cpp SFIni.cpp SFIniNews.cpp stdafx.cpp
OBJECTS		= md5c.o md5class.o newsboard.o SFIni.o SFIniNews.o stdafx.o
############################################################################

############################################################################
# My linux depend sometimes is not as good as gcc at finding the include
#  files and complains.  Doesn't effect compilation.
# This is an extra include directory for depend to look in and be happy.
# You may have to add stuff here if your include files are not being found.
CMAKEINCDIR	= 
############################################################################




############################################################################
# Build object files from .C files
%.o: %.cpp
	@echo Compiling $*
	${CC} ${CFLAGS} ${OPTION_FLAGS} ${ALL_INCDIRS} $*.cpp -o $*.o
############################################################################


############################################################################
# build the default executable program from all the objects
mcboardbot: build FORCE
	@echo Linking files
	$(LOAD) $(OBJECTS) ${LDFLAGS} -o ../mcboardbot.exe ${ALL_LIBDIRS} ${ALL_LIBS}
############################################################################


############################################################################
# what is needed to make the executables (dynamic and static)
build: $(OBJECTS)
############################################################################

############################################################################
# Technique used to force rebuilding of rules
FORCE:
############################################################################




############################################################################
# Can be run to erase any old object files and junk
clean: undepend
	rm -f *.o *.a Makefile.bak
	rm -f *~ core
############################################################################

############################################################################
# Can be run to erase any old object files and junk
fileclean: FORCE
	rm -f *.o *.a Makefile.bak
	rm -f *~ core
############################################################################

############################################################################
# automatic dependency building
undepend: FORCE
	make depend
depend: FORCE
	make depend $(ALL_INCDIRS) $(CMAKEINCDIR) $(SOURCE) $(HEADERS)
############################################################################











