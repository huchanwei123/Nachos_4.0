#!/bin/bash
make clean
make
make consoleIO_test1
make consoleIO_test2
../build.linux/nachos -d t -e consoleIO_test1 -e consoleIO_test2
