#!/usr/bin/env bash
 
long_username=$(printf "%*s" 33)
long_username="${long_username// /a}"

long_email=$(printf "%*s" 256)
long_email="${long_email// /a}"

EXPECTED=$(cat << EOF
db > String is too long.
db > Executed.
db > 
EOF
)

command=$(printf "insert 1 %s %s\nselect\n.exit" "$long_username" "$long_email")

cd ..
output=$(echo "$command" | ./mini-db)

#if diff <(echo "$output") <(echo "$EXPECTED"); then
if [[ "$output" == "$EXPECTED" ]]; then 
  echo "Test3: 0. Good."
else
  echo -e "Test3: 1. Bad.\n Got: $output"
fi
