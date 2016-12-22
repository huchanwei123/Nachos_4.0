#!/bin/bash
make clean
make
make consoleIO_test1
make consoleIO_test2
make fileIO_test1
make fileIO_test2
../build.linux/nachos -e fileIO_test1
../build.linux/nachos -e fileIO_test2
