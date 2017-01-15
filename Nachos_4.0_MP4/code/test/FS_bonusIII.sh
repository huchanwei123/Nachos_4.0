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
echo "=============recursive list all the items in root================"
../build.linux/nachos -lr /
echo "=============remove /t0/bb/f1=============="
echo "=============recursively remove /t0/aa=============="
echo "=============recursive list all the items under /t0=============="
../build.linux/nachos -r /t0/bb/f1
../build.linux/nachos -rr /t0/aa
../build.linux/nachos -lr /t0
echo "=============recursive remove /t0/bb============"
echo "=============recursive list all the items in root============"
../build.linux/nachos -rr /t0/bb
valgrind ../build.linux/nachos -lr /
echo "========================================="
