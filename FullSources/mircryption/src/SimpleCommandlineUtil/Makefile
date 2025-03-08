#############################################################################
# SEE BELOW FOR DIFFERENT CHANGES TO MAKE FOR MACOSX OR UNIX/LINUX/PC
# updated 07/11/03 to support macosx options (thanks darkfiber)
# updated 09/17/03 to offer some alternative file link locations
# updated 11/10/03 to better explain link options, and modify freebsd to use altlink option
# updated 1/26/04 with mirc color code source file added
# updated 3/24/04 added include directory ./ (from SUSE linux text)
# updated 6/9/05 updated to build with new cbc source code files
#############################################################################


#############################################################################
CC = gcc
LOAD = gcc
#############################################################################


#############################################################################
# PLATFORM DEPENDENT FLAGS
CFLAGS_MAC = "-c -x c++ -DMACOSX"
LDFLAGS_MAC = " -Wl"
CFLAGS_GENERIC = "-c -x c++"
LDFLAGS_GENERIC = "-Wl"
CFLAGS2_BIGENDIAN = "-DBIG_ENDIAN"
CFLAGS2_LITTLENDIAN = "-DLITTLE_ENDIAN"
#############################################################################


#############################################################################
# PLATFORM TARGETS

all: .DEFAULT

.DEFAULT:
	@echo "To Build mcps, type 'make TARGET' (generic | linux | freebsd | macosx | windows | altlink | altlink2)"
	@echo " or type: 'make clean' to clean .o and .so"
	@echo "try altlink1 or altlink2 if your linker complains about not being able to find the .o files"
	@echo "    altlink1 may be good for freebsd"	
macosx:
	$(MAKE) _mircryption $(MACFLAGS) CFLAGS=$(CFLAGS_MAC) LDFLAGS=$(LDFLAGS_MAC) CFLAGS2=$(CFLAGS2_BIGENDIAN)
unix linux windows generic:
	$(MAKE) _mircryption $(GENERICFLAGS) CFLAGS=$(CFLAGS_GENERIC) LDFLAGS=$(LDFLAGS_GENERIC)
altlink freebsd:
	$(MAKE) _mircryption_altlink $(GENERICFLAGS) CFLAGS=$(CFLAGS_GENERIC) LDFLAGS=$(LDFLAGS_GENERIC)
altlink2:
	$(MAKE) _mircryption_altlink2 $(GENERICFLAGS) CFLAGS=$(CFLAGS_GENERIC) LDFLAGS=$(LDFLAGS_GENERIC)
#############################################################################




#############################################################################
SOURCE = ../mc_blowfish.cpp mcps.cpp ../newblowfish.cpp ../oldblowfish.cpp ../BlowfishCbc.cpp ../b64stuff.cpp
OBJECTS = ../mc_blowfish.o mcps.o ../newblowfish.o ../oldblowfish.o ../BlowfishCbc.o ../b64stuff.o
INCDIRS = -I../ -I./

%.o : %.cpp
	@echo Compiling $*
	$(CC) $(CFLAGS) $(CFLAGS2) $(INCDIRS) $*.cpp -o $*.o

_mircryption: $(OBJECTS)
	@echo "Linking mcps simple commandline util..."
	$(LOAD) $(LDFLAGS) -o mcps *.o ../*.o -lstdc++

_mircryption_altlink: $(OBJECTS)
	@echo "Linking mcps simple commandline util..."
# this is for linkers (freebsd?) which put all the .o in the current directory
	$(LOAD) $(LDFLAGS) -o mcps *.o -lstdc++

_mircryption_altlink2: $(OBJECTS)
	@echo "Linking mcps simple commandline util..."
	$(LOAD) $(LDFLAGS) -Wl -o mcps $(OBJECTS) -lstdc++

clean:
	@echo Deleting intermediate files
	rm -f *.o
	rm -f ../*.o
#############################################################################
