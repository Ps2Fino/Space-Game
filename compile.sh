#!/bin/bash

if [ "$1" == "-b" ]
then
  mkdir build && cd build && cmake .. && make install
elif [ "$1" == "-c" ]
then
  rm -rf bin/ build/
elif [ "$1" == "-m" ]
then
  cd build && make install
else
  echo "usage: compile.sh -b|-c|-m"
fi

