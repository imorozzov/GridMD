#!/bin/sh

# Show help if no arguments
if [ -z "$3" ]; then
  echo 'Usage:	make_release.sh output_directory os_type component'
	echo "	os_type = win | unix"
	echo "	component = workflow | job_manager"
  exit
fi

# Create output directory
OUTDIR=$1
if mkdir -p $OUTDIR; then
  echo "Making the release in $OUTDIR"
else
  exit
fi

# Check OS parameter
OSTYPE=$2
if [ $OSTYPE != win -a $OSTYPE != unix ]; then
  echo "The second parameter must be 'win' or 'unix'!"
	exit
fi

# Specify common files
DIRS="
	gridmd/doc
	gridmd/doc/html
	gridmd/include
	gridmd/src
  ivutils/include
	ivutils/src
"
FILES_TXT="
	changes.txt
	README.txt
	gridmd/doc/gridmd_license.txt
	gridmd/doc/wx_license.txt
"
# Specify component specific files
COMPONENT=$3
if [ $COMPONENT = workflow ]; then
	DIRS="$DIRS
    gridmd/include/jobmngr
    gridmd/src/jobmngr
		examples/simple_workflow/graphs
  "
	DIRS_WIN="$DIRS_WIN
	  examples/simple_workflow/vs8
	"
	DIRS_UNIX="$DIRS_UNIX
	  examples/simple_workflow/unix
	"
  FILES_TXT="$FILES_TXT
		gridmd/include/gridmd.h
		gridmd/include/workflow.h
		gridmd/include/wxinc.h
		gridmd/include/gmfork.h
		gridmd/include/jobmngr/gmshell.h
		gridmd/include/jobmngr/jobmngr.h
		gridmd/include/jobmngr/pbsmngr.h
		gridmd/include/jobmngr/plinkshell.h
    gridmd/include/jobmngr/unixshell.h
		ivutils/include/logexc.h
		ivutils/include/seqpack.h
    ivutils/include/refobj.h
		ivutils/include/utiltl.h
    gridmd/src/gridmd.cpp
		gridmd/src/workflow.cpp
		gridmd/src/gmfork.cpp
    gridmd/src/jobmngr/gmshell.cpp
		gridmd/src/jobmngr/jobmngr.cpp
		gridmd/src/jobmngr/pbsmngr.cpp
		gridmd/src/jobmngr/plinkshell.cpp
    gridmd/src/jobmngr/unixshell.cpp
		ivutils/src/logexc.cpp
		ivutils/src/seqpack.cpp
    examples/simple_workflow/*.cpp
		examples/simple_workflow/resources.xml
	"
  FILES_BIN="$FILES_BIN
	  examples/simple_workflow/graphs/*.png
	  gridmd/doc/html/*
	"
  FILES_TXT_WIN="$FILES_TXT_WIN
    examples/simple_workflow/vs8/*.sln
    examples/simple_workflow/vs8/*.vcproj
  "
  FILES_TXT_UNIX="$FILES_TXT_UNIX
    examples/simple_workflow/unix/Makefile*
  "	
	
elif [ $COMPONENT = job_manager ]; then
	DIRS="$DIRS 
    gridmd/include/jobmngr
    gridmd/src/jobmngr
	  examples/pbs_job_manager/input/indir
	"
	DIRS_WIN="$DIRS_WIN
	  examples/pbs_job_manager/vs8
	"
	DIRS_UNIX="$DIRS_UNIX
	  examples/pbs_job_manager/unix
	"
  FILES_BIN="$FILES_BIN
	  gridmd/doc/jobmngr_en.pdf
    examples/pbs_job_manager/input/indir/somefile.bin
	"
  FILES_TXT="$FILES_TXT
    gridmd/include/jobmngr/gmshell.h
    gridmd/include/jobmngr/jobmngr.h
    gridmd/include/jobmngr/pbsmngr.h
    gridmd/include/jobmngr/plinkshell.h
    gridmd/include/jobmngr/unixshell.h
    ivutils/include/logexc.h
    gridmd/src/jobmngr/gmshell.cpp
    gridmd/src/jobmngr/jobmngr.cpp
    gridmd/src/jobmngr/pbsmngr.cpp
    gridmd/src/jobmngr/plinkshell.cpp
    gridmd/src/jobmngr/unixshell.cpp
    ivutils/src/logexc.cpp
    examples/pbs_job_manager/README.txt
    examples/pbs_job_manager/input/prog.sh
    examples/pbs_job_manager/input/infile1.txt
    examples/pbs_job_manager/input/infile2.txt
    examples/pbs_job_manager/input/permfile.txt
    examples/pbs_job_manager/pbs_unixshell1.cpp
	"
  FILES_TXT_WIN="$FILES_TXT_WIN
    examples/pbs_job_manager/*.cpp
    examples/pbs_job_manager/vs8/*.sln
    examples/pbs_job_manager/vs8/*.vcproj
	"
  FILES_TXT_UNIX="$FILES_TXT_UNIX
    examples/pbs_job_manager/unix/Makefile*
	"
else
  echo "Wrong component!"
	exit
fi

# Copy files
if [ $OSTYPE = win ]; then
  for DIR in $DIRS $DIRS_WIN; do
	  mkdir -p $OUTDIR/$DIR
	done
	
	for FILE in $FILES_BIN $FILES_BIN_WIN; do
		cp -Rf $FILE $OUTDIR/$FILE
	done
	
	for FILE in $FILES_TXT $FILES_TXT_WIN; do
		unix2dos <$FILE >$OUTDIR/$FILE
	done
	
else
  for DIR in $DIRS $DIRS_UNIX; do
	  mkdir -p $OUTDIR/$DIR
	done
	
	for FILE in $FILES_BIN $FILES_BIN_UNIX; do
		cp -Rf $FILE $OUTDIR/$FILE
	done
	
	for FILE in $FILES_TXT $FILES_TXT_UNIX; do
		dos2unix <$FILE >$OUTDIR/$FILE
	done

fi
