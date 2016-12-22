#!/bin/bash
make clean
make
make test_SJF1
make test_SJF2
make test_SJF3
make test_prior1
make test_prior2
make test_prior3
make test_RR1
make test_RR2
make test_RR3
../build.linux/nachos -ep test_RR1 19 -ep test_RR2 21 -ep test_RR3 22
#../build.linux/nachos -ep test_prior1 59 -ep test_prior2 51 -ep test_prior3 52
#../build.linux/nachos -ep test_SJF1 123 -ep test_SJF2 122 -ep test_SJF3 128
