#!/usr/bin/python

set -Eeuox

CURR_BRANCH=`echo $1 | sed 's/-/\//g'`

if [[ ${CURR_BRANCH} != *"task"* ]];then
	echo "not hw branch -> skip"
	exit 0
fi

cd $CURR_BRANCH
mkdir build && cd build
cmake ..
make
