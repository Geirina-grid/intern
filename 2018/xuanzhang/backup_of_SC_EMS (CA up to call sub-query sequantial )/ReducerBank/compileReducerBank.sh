#!/bin/sh

SHARED_LIB=../../lib
export "LD_LIBRARY_PATH=$SHARED_LIB"

g++ -fPIC -D__STDC_LIMIT_MACROS  -I../ReducerLib -shared "./ReducerBank.cpp" "../ReducerLib/ReducerLib.cpp" -o "$SHARED_LIB/ReducerBank.so"


