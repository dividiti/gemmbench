Benchmark for generic matrix-matrix multiplication (GEMM)
=========================================================

Prerequisites
=============
* Collective Knowledge Framework: http://github.com/ctuning/ck
* OpenCL library

Installation
============

> ck pull repo --url=https://github.com/dividiti/gemmbench

or

> ck pull repo --url=git@github.com:dividiti/gemmbench.git

TBD: register OpenCL with CK

Usage
=====

* Compile benchmark

> ck compile program:gemmbench-cl-launcher-1.0

* Run benchmark (select "default" CMD when asked)

> ck run program:gemmbench-cl-launcher-1.0

* Run benchmark with custom CMD (for example, change matrix order, platform ID and device ID):

> ck run program:gemmbench-cl-launcher-1.0 --extra_run_cmd="-n 512 -p 0 -d 1"

Publications
============
* http://arxiv.org/abs/1511.03742
