#!/bin/sh

# shell script invoked with the following arguments
# $(CXX) $(CXX_DEFINES) $(CXX_FLAGS) -o OBJECT_FILE -c SOURCE_FILE

# invoke compiler
exec "$@" 2>&1 | "/home/marius/cg/myfilter.sh"
