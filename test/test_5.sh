#!/usr/bin/env bash

INPUT="insert -1 cstack foo@bar.com\nselect\n.exit"
EXPECTED=$(cat <<EOF
db > ID must be positive.
db > Executed.
db > 
EOF
)

cd ..
output=$(echo -e $INPUT | ./mini-db)
if [[ $output == $EXPECTED ]]; then
  echo "Test5: 0. Good."
else
  echo "Test5: 1. Bad."
  echo "Got: $output"
fi
