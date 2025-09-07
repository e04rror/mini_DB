#!/usr/bin/env bash

FILENAME="test9.db"
INPUT="insert 1 user1 person1@example.com\ninsert 1 user1 person1@example.com\nselect\n.exit"

EXPECTED=$(cat <<EOF
db > Executed.
db > Error: Duplicate key.
db > (1, user1, person1@example.com)
Executed.
db > 
EOF
)
cd ..
output=$(echo -e $INPUT | ./mini-db $FILENAME )

if [[ $output == $EXPECTED ]]; then
  echo "Test9: 0. Good."
else 
  echo "Test9: 1. Bad."
  echo "Got: $output"
fi

rm $FILENAME
