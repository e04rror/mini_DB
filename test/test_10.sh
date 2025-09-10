#!/usr/bin/env bash

FILENAME="test10.db"
cd ..
output=$({
  for i in {1..14}; do 
    echo "insert $i user$i person$i@example.com"
  done
  echo ".btree"
  echo "insert 15 user15 peron15@example.com"
  echo ".exit"
} | ./mini-db $FILENAME)

expected=""
for i in {1..14}; do 
  expected+=$'db > Executed.\n'
done

expected+=$'db > Tree:\n'
expected+=$'- internal (size 1)\n'
expected+=$'  - leaf (size 7)\n'
for i in {1..7}; do 
  expected+=$"    - $i"$'\n'
done

expected+=$'  - key 7\n'
expected+=$'  - leaf (size 7)\n'
for i in {8..14}; do 
  expected+=$"    - $i"$'\n'
done
expected+=$'db > Need to implement searching an internal node'

diff -u <(printf "%s" "$output") <(printf "%s" "$expected")

if [[ "$output" == "$expected" ]]; then
  echo "Test10: 0. Good."
else 
  echo "Test10: 1. Bad."
  echo "Got:"
  echo "$output"
  echo -e "\nExpected:\n"
  echo -e "$expected"
fi

rm $FILENAME
