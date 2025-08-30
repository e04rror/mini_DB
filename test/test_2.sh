#!/usr/bin/env bash

NUMBER_OF_OBJECTS=1401
EXPECTED_RESULT="db > Error: Table full."

commands=""
for i in $(seq 1 $NUMBER_OF_OBJECTS); do 
  commands+="insert $i user$1 person$1@example.com"$'\n'
done
commands+=".exit"

cd ..
output=$(echo "$commands" | ./mini-db)

last_line=$(echo "$output" | tail -n 2 | head -n 1)

if [[ $last_line == $EXPECTED_RESULT ]]; then
  echo "Test2: 0. Good."
else 
  echo "Test2: 1. Bad. GOT: $last_line"
fi
