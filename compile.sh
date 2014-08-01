#!/bin/bash
#
# Just saves me typing the same thing over and over again ;-)
# @author Daniel J. Finnegan
# @date July 2014
#

if [ "$1" == "-b" ]
then
  mkdir build && cd build && cmake ..
elif [ "$1" == "-c" ]
then
  rm -rf bin/ build/
else
  echo "usage: compile.sh -b|-c"
fi

