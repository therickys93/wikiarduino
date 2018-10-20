#!/bin/bash

DESCRIBE=`git describe --tags`
MAJOR=`git describe --tags | tr -s '.' | tr '.' '-' | awk -F '-' '{print $1}'`
MINOR=`git describe --tags | tr -s '.' | tr '.' '-' | awk -F '-' '{print $2}'`
PATCH=`git describe --tags | tr -s '.' | tr '.' '-' | awk -F '-' '{print $3}'`
COMMIT=$(git rev-parse --short HEAD)

echo $DESCRIBE

rm src/version.h 2> /dev/null

echo "#define MAJOR ( "$MAJOR" )"     >> src/version.h
echo "#define MINOR ( "$MINOR" )"     >> src/version.h
echo "#define PATCH ( "$PATCH" )"     >> src/version.h
echo "#define COMMIT ( 0x"$COMMIT" )" >> src/version.h