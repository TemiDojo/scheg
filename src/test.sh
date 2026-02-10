#!/bin/bash


make clean && make

# TEST 0
./tests/test0.sh
./interpret put.out
echo -e "Test 0 completed\n"
sleep 5

# TEST 1
./compile -c ./tests/test1 -o test1.out
./interpret test1.out
echo -e "Test 1 completed\n"
sleep 5

# TEST 2
./compile -c ./tests/test2 -o test2.out
./interpret test2.out
echo -e "Test 2 completed\n"
sleep 5

# TEST 3
./compile -c ./tests/test3 -o test3.out
./interpret test3.out
echo -e "Test 3 completed\n"


make clean
