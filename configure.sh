#! /bin/sh
. /opt/intel/oneapi/setvars.sh --include-intel-llvm; 
cmake -S . -B build -DENABLE_SYCL=OFF -DENABLE_GPGPU=OFF;
