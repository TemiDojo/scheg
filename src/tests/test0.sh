#!/bin/bash

echo -e "-------TEST: 1--------------"
echo " " |./compile
sleep 5

echo -e "-------TEST: 2--------------\n"
printf '' | ./compile
sleep 5

echo -e "-------TEST: 3--------------\n"
printf '(a\x00)' | ./compile
sleep 5

echo -e "-------TEST: 4--------------\n"
printf '(let (a\x00bc 1) a)' | ./compile
sleep 5

echo -e "-------TEST: 5--------------\n"
python3 -c 'print("a" * 2**33)' | ./compile
sleep 5

echo -e "-------TEST: 6--------------\n"
python3 -c 'print("01", end="")' | ./compile
sleep 5

echo -e "-------TEST: 7--------------\n"
python3 -c 'print("0" * 1000 + "1", end="")' | ./compile
sleep 5

echo -e "-------TEST: 8--------------\n"
printf '(le\x00' | ./compile
sleep 5

echo -e "-------TEST: 9--------------\n"
printf '\x00' | ./compile
sleep 5

echo -e "-------TEST: 10-------------\n"
printf '(\x00' | ./compile
sleep 5

echo -e "-------TEST: 11-------------\n"
printf '(\x00)' | ./compile
sleep 5

echo -e "-------TEST: 12-------------\n"
printf 'a' | ./compile

echo -e "-------END OF TEST----------\n"
