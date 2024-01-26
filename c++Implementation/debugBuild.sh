#! /bin/sh
. /opt/intel/oneapi/setvars.sh --include-intel-llvm > /dev/null; 
cmake -S . -B debugBuild -DENABLE_DEBUG=ON;
make -C debugBuild