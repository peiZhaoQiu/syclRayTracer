#! /bin/sh


cmake -S . -B debugBuild -DENABLE_DEBUG=ON -DENABLE_SYCL=ON;
make -C debugBuild