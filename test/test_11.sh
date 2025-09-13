#!/usr/bin/env bash
set -euo pipefail

cd ..
DB_EXEC="./mini-db"
DB_FILE="test.db"

script=()
for i in $(seq 1 1400); do 
  script+=("insert $i user$i person$i@example.com")
done
script+=(".exit")

output_file=$(mktemp)
(
  for cmd in "${script[@]}"; do 
    if ! echo "$cmd"; then
      break 
    fi
  done
) | "$DB_EXEC" "$DB_FILE" >"$output_file" 2>&1 || true

mapfile -t result <"$output_file"

last_two=("${result[@]: -2}")

expected1="db > Executed."
expected2="db > Need to implement updating parent after split"

if [[ " ${last_two[*]} " == *"$expected1"* && " ${last_two[*]} " == *"$expected2"* ]]; then
  echo "Test11. 0. Good."
else 
  echo "Test11. 1. Bad."
  echo "Got:"
  echo "$result"
fi

rm -r $DB_FILE
