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


TESTNAME1 = simple_workflow
TESTNAME2 = fork_simple_workflow
TESTNAME3 = fork_skeleton
TESTNAME4 = fork_skeleton_extra

all:: $(TESTNAME1).exe $(TESTNAME2).exe $(TESTNAME3).exe $(TESTNAME4).exe
	


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
	gridmd.cpp \
	gmshell.cpp \
	jobmngr.cpp \
	plinkshell.cpp \
	pbsmngr.cpp \
	unixshell.cpp \
	workflow.cpp \
	gmfork.cpp
SRC_UTILS = \
	logexc.cpp \
	seqpack.cpp
SRC_MAIN1 = \
	$(TESTNAME1).cpp 
SRC_MAIN2 = \
	$(TESTNAME2).cpp 
SRC_MAIN3 = \
	$(TESTNAME3).cpp 
SRC_MAIN4 = \
	$(TESTNAME4).cpp 

    
OBJ_MAIN =  $(SRC_MAIN:.cpp=.o)
	     

SRC_LIB = $(SRC_GMD) $(SRC_UTILS)
SRC1 = $(SRC_LIB) $(SRC_MAIN1)
SRC2 = $(SRC_LIB) $(SRC_MAIN2)
SRC3 = $(SRC_LIB) $(SRC_MAIN3)
SRC4 = $(SRC_LIB) $(SRC_MAIN4)
OBJ_CPP = $(SRC_LIB:.cpp=.o)
OBJ_LIB = $(OBJ_CPP:.c=.o)
OBJ1 = $(OBJ_LIB) $(SRC_MAIN1:.cpp=.o)
OBJ2 = $(OBJ_LIB) $(SRC_MAIN2:.cpp=.o)
OBJ3 = $(OBJ_LIB) $(SRC_MAIN3:.cpp=.o)
OBJ4 = $(OBJ_LIB) $(SRC_MAIN4:.cpp=.o)
        

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

$(TESTNAME1).exe: $(OBJ1)
	$(CPP) $(OFLAG) $(OBJ1) \
	$(USRLIB)  -o $(TESTNAME1).exe; \
	$(POSTLINK);

$(TESTNAME2).exe: $(OBJ2)
	$(CPP) $(OFLAG) $(OBJ2) \
	$(USRLIB)  -o $(TESTNAME2).exe; \
	$(POSTLINK);

$(TESTNAME3).exe: $(OBJ3)
	$(CPP) $(OFLAG) $(OBJ3) \
	$(USRLIB)  -o $(TESTNAME3).exe; \
	$(POSTLINK);

$(TESTNAME4).exe: $(OBJ4)
	$(CPP) $(OFLAG) $(OBJ4) \
	$(USRLIB)  -o $(TESTNAME4).exe; \
	$(POSTLINK);

$(TESTNAME1).make:  ../Makefile ../Makefile.arch
	echo GMDINC = $(PREPROC_FLAGS) $(INC) $(WXFLAGS) > $(TESTNAME1).make
	echo GMDLIB = $(shell pwd)/lib$(TESTNAME1).a $(USRLIB) >> $(TESTNAME1).make

$(TESTNAME2).make:  ../Makefile ../Makefile.arch
	echo GMDINC = $(PREPROC_FLAGS) $(INC) $(WXFLAGS) > $(TESTNAME2).make
	echo GMDLIB = $(shell pwd)/lib$(TESTNAME2).a $(USRLIB) >> $(TESTNAME2).make

$(TESTNAME31).make:  ../Makefile ../Makefile.arch
	echo GMDINC = $(PREPROC_FLAGS) $(INC) $(WXFLAGS) > $(TESTNAME3).make
	echo GMDLIB = $(shell pwd)/lib$(TESTNAME3).a $(USRLIB) >> $(TESTNAME3).make

$(TESTNAME4).make:  ../Makefile ../Makefile.arch
	echo GMDINC = $(PREPROC_FLAGS) $(INC) $(WXFLAGS) > $(TESTNAME4).make
	echo GMDLIB = $(shell pwd)/lib$(TESTNAME4).a $(USRLIB) >> $(TESTNAME4).make


# Individual dependencies

DEPENDS = $(OBJ1:.o=.d)
   -include $(DEPENDS)
DEPENDS = $(OBJ2:.o=.d)
   -include $(DEPENDS)
DEPENDS = $(OBJ3:.o=.d)
   -include $(DEPENDS)
DEPENDS = $(OBJ4:.o=.d)
   -include $(DEPENDS)
   
#----- Begin Boilerplate
endif
