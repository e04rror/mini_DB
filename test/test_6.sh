#!/usr/bin/env bash

FILENAME="mydb.db"

INPUT_1="insert 1 user1 person1@example.com\n.exit"
INPUT_2="select\n.exit"

EXPECTED_1=$(cat << EOF
db > Executed.
db > 
EOF
)
EXPECTED_2="(1, user1, person1@example.com)"

cd ..
command_1=$(echo -e "$INPUT_1" | ./mini-db $FILENAME)
if [[ $command_1 == $EXPECTED_1 ]]; then
  echo "Test_6_1: 0. Good."
else
  echo "Test_6_1: 1. Bad."
  echo "Got: $command_1"
fi 

command_2=$(echo -e "$INPUT_2" | ./mini-db $FILENAME)

output=$(echo $command_2 | grep -o '([^)]*)' | tail -n 1)

if [[ $output == $EXPECTED_2 ]]; then
  echo "Test_6_2: 0. Good."
else 
  echo "Test_6_2: 1. Bad."
  echo "Got: $output"
fi 
