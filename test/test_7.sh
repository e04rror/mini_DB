#!/usr/bin/env bash

FILENAME="mydb.db"

INPUT=".constants\n.exit"

EXPECTED=$(cat << EOF
db > Constants:
ROW_SIZE: 108
COMMON_NODE_HEADER_SIZE: 6
LEAF_NODE_HEADER_SIZE: 10
LEAF_NODE_CELL_SIZE: 112
LEAF_NODE_SPACE_FOR_CELLS: 4086
LEAF_NODE_MAX_CELLS: 36
db > 
EOF
)

cd ..
output=$(echo -e "$INPUT" | ./mini-db $FILENAME)
if [[ $output == $EXPECTED ]]; then
  echo "Test_7: 0. Good."
else
  echo "Test_7: 1. Bad."
  echo "Got: $output"
fi
