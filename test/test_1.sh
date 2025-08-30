#!/usr/bin/env bash

insert_test_input="insert 1 user1 person1@example.com\nselect\n.exit"
# One way
expected_output=$(cat <<EOF
db > Executed.
db > (1, user1, person1@example.com)
Executed.
db > 
EOF
)

# Another way
#oka="db > Executed.\ndb > (1, user1, person1@example.com)\nExecuted.\ndb > \n"

run_test() {
  local input="$1"
  local expected="$2"
  
  cd ..
  output=$(echo -e "$input" | ./mini-db)
  
  # for another way
  #expected=$(echo -e "$expected")
  if [[ $output == $expected ]]; then
    echo "Test1: 0. Good"
  else
    echo "Test1: 1. Bad"
  fi
} 

run_test "$insert_test_input" "$expected_output"
