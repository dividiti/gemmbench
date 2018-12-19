Benchmark for generic matrix-matrix multiplication (GEMM)
=========================================================

[![compatibility](https://github.com/ctuning/ck-guide-images/blob/master/ck-compatible.svg)](https://github.com/ctuning/ck)
[![automation](https://github.com/ctuning/ck-guide-images/blob/master/ck-artifact-automated-and-reusable.svg)](http://cTuning.org/ae)
[![workflow](https://github.com/ctuning/ck-guide-images/blob/master/ck-workflow.svg)](http://cKnowledge.org)
[![License](https://img.shields.io/badge/License-BSD%203--Clause-blue.svg)](https://opensource.org/licenses/BSD-3-Clause)

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
