#!/bin/bash

cd "`dirname "$0"`"
if [ ! -f "cmake-build-debug/bf" ];then
    echo "Run build before tests"
    exit 1
fi

rm -rf cmake-build-debug/tests tests.log
mkdir -p cmake-build-debug/tests

for n in tests/*.bf; do
    i="`basename "$n"`"
    echo "---- Compiling '$i'..." >>tests.log
    ./cmake-build-debug/bf bfpp -i "tests/$i" -o "cmake-build-debug/tests/$i.bfi" -O 2 >>tests.log 2>&1
    echo "---- Done, error=$?" >>tests.log
    echo "---- Running '$i'..." >>tests.log
    ./cmake-build-debug/bf bfrun -i "cmake-build-debug/tests/$i.bfi" $@ >>tests.log 2>&1
    echo "---- Done, error=$?" >>tests.log
done
