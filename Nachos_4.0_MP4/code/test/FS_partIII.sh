../build.linux/nachos -f
../build.linux/nachos -mkdir /t0
../build.linux/nachos -mkdir /t1
../build.linux/nachos -mkdir /t2
../build.linux/nachos -cp num_100.txt /t0/f1
../build.linux/nachos -mkdir /t0/aa
../build.linux/nachos -mkdir /t0/bb
../build.linux/nachos -mkdir /t0/cc
../build.linux/nachos -cp num_100.txt /t0/bb/f1
../build.linux/nachos -cp num_100.txt /t0/bb/f2
../build.linux/nachos -cp num_100.txt /t0/bb/f3
../build.linux/nachos -cp num_100.txt /t0/bb/f4
../build.linux/nachos -l /
echo "============List all items in /t0================="
../build.linux/nachos -l /t0
echo "============Remove /t0/bb/f1========"
../build.linux/nachos -r /t0/bb/f1
../build.linux/nachos -lr /
echo "============print out the content in /t0/f1============="
../build.linux/nachos -p /t0/f1
echo "============print out the content in /t0/bb/f3==========="
valgrind ../build.linux/nachos -p /t0/bb/f3
