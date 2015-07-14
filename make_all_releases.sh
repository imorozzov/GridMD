#!/bin/sh

# Show help if no arguments
if [ -z "$2" ]; then
  echo 'Usage:	make_release.sh output_directory release_name'
  exit
fi

TMPDIR=$1
RNAME=$2
SRCDIR=$PWD

FILENAME=$RNAME-job_manager.tgz
echo "Making $FILENAME"
rm -Rf $TMPDIR/$RNAME
./make_release.sh $TMPDIR/$RNAME unix job_manager
cd $TMPDIR
tar -czf $FILENAME $RNAME
cd $SRCDIR

FILENAME=$RNAME.tgz
echo "Making $FILENAME"
./make_release.sh $TMPDIR/$RNAME unix workflow
cd $TMPDIR
tar -czf $FILENAME $RNAME
cd $SRCDIR

FILENAME=$RNAME-job_manager-win.zip
echo "Making $FILENAME"
rm -Rf $TMPDIR/$RNAME
./make_release.sh $TMPDIR/$RNAME win job_manager
cd $TMPDIR
zip -rq $FILENAME $RNAME
cd $SRCDIR

FILENAME=$RNAME-win.zip
echo "Making $FILENAME"
./make_release.sh $TMPDIR/$RNAME win workflow
cd $TMPDIR
zip -rq $FILENAME $RNAME
cd $SRCDIR

rm -Rf $TMPDIR/$RNAME
