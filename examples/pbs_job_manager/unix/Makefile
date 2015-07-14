##----------------------------------------------------------------------
# Job Manager test application
#----------------------------------------------------------------------
# This file jumps into the architecture-specific directory and 
# builds a program.
#
# This is taken from Paul Smith, GNU
#
SHELL = /bin/tcsh
# The variable DEFAULT_ARCH contains the default value.
# To choose a different value, users can edit this, 
# or invoke make using
#     make _ARCH = <user's architecture>
#
# To build parallel version use:
#     make _MPI=true _ARCH = <user's architecture>

DEFAULT_ARCH = gnu

ifeq (,$(filter Obj_%,$(notdir $(CURDIR))))
  include Makefile.target
else

#----- End Boilerplate

# Make the first (i.e. default) target all, 
# then use special target to declare it phony to 
# avoid searching for non-existent file
all::
.PHONY: all


# This section is for the architecture-specific target directory

# This sub-section is common to all architectures


TESTNAME  = pbs_unixshell1

all:: $(TESTNAME).exe
	


ROOTDIR =../../../..#
GMDDIR  =$(ROOTDIR)/gridmd#
UTILDIR =$(ROOTDIR)/ivutils#
TSTDIR  =../..#

# VPATH tells make where to look for source files.

VPATH   = $(TSTDIR):$(UTILDIR)/src:$(GMDDIR)/src:$(GMDDIR)/src/jobmngr


PREF = $(strip $(shell pwd))# 
INCPATH = $(UTILDIR)/include $(GMDDIR)/include $(GMDDIR)/jobmngr/include
INC    =  $(addprefix -I$(PREF)/,$(INCPATH)) 

SRC_GMD = \
	gmshell.cpp \
	jobmngr.cpp \
	pbsmngr.cpp \
	unixshell.cpp
SRC_UTILS = \
	logexc.cpp
SRC_MAIN = \
	$(TESTNAME).cpp 

    
OBJ_MAIN =  $(SRC_MAIN:.cpp=.o)
	     

SRC_LIB = $(SRC_GMD) $(SRC_UTILS)
SRC = $(SRC_LIB) $(SRC_MAIN)
OBJ1 = $(SRC_LIB:.cpp=.o)
OBJ_LIB = $(OBJ1:.c=.o)
OBJ = $(OBJ_LIB) $(SRC_MAIN:.cpp=.o)
        

_MPI = false
ifeq (true,$(_MPI))
MESSAGE = 'Using MPI'
MPI_PREPROC_FLAG = -DUSE_MPI
endif

  

#
# Note: here are the preprocessor flags
# These need to be applied manually, using e.g. 
#
# touch a.cpp; make
#
#alg_parser.o: alg_parser.cpp
#	$(CPP) $(CPPFLAGS) -O0 -c $<


# This subsection contains branches to specific architecture definitions
#
# Initialize with values that are guaranteed to fail
# This is used to catch invalid _ARCH values
  C = "Invalid_ARCH_value"
  CPP = "Invalid_ARCH_value"
  F77 = "Invalid_ARCH_value"
# Add null defaults for rarely used variables
  RUNDIR = .
  SLEEP = 
  POSTLINK = 

include $(SRCDIR)/Makefile.arch

ifndef WXCONFIG
  $(error Please set the value of WXCONFIG in Makefile.arch!)
endif

# This sub-section is common to all architectures
.SUFFIXES : .o .cpp .c .F
# inference rule to get .o targets from .cpp prerequisites
.cpp.o :
	$(CPP) $(CPPFLAGS) $(INC) -c $<
# inference rule to get .o targets from .c prerequisites
.c.o :
	$(C) $(CFLAGS) $(INC) -c $<

# Target rules for executables specify prerequisite objects
# and linking command.

$(TESTNAME).exe: $(OBJ)
	$(CPP) $(OFLAG) $(OBJ) \
	$(USRLIB)  -o $(TESTNAME).exe; \
	$(POSTLINK);

$(TESTNAME).make:  ../Makefile ../Makefile.arch
	echo GMDINC = $(PREPROC_FLAGS) $(INC) $(WXFLAGS) > $(TESTNAME).make
	echo GMDLIB = $(shell pwd)/lib$(TESTNAME).a $(USRLIB) >> $(TESTNAME).make



lib: $(OBJ_LIB) $(TESTNAME).make 
	ar -r lib$(TESTNAME).a $(OBJ_LIB)   

libclean:
	rm -rf lib$(TESTNAME).a $(TESTNAME).make


#  include $(SRCDIR)/Makefile.testing

# Individual dependencies

DEPENDS = $(OBJ:.o=.d)
   -include $(DEPENDS)
   
#----- Begin Boilerplate
endif
