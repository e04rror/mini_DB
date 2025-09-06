#!/usr/bin/env bash

cd ..
command=$(./mini-db mydb.db <<EOF
insert 3 user3 use3@go.com
insert 1 user1 use1@go.com
insert 2 user2 use2@go.com
.btree
.exit
EOF
)

EXPECTED=$(cat <<EOF
db > Executed.
db > Executed.
db > Executed.
db > Tree:
leaf (size 3)
  - 0 : 3
  - 1 : 1
  - 2 : 2
db > 
EOF
)

if [[ $command == $EXPECTED ]]; then
  echo "Test_8_1: 0. Good."
else 
  echo "Test_8_1: 1. Bad."
  echo "Got: $command"
fi
