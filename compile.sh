#!/bin/bash

if [ "$1" == "-b" ]
then
  mkdir build && cd build && cmake ..
elif [ "$1" == "-c" ]
then
  rm -rf bin/ build/
else
  echo "usage: compile.sh -b|-c"
fi

