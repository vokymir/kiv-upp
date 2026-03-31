#!/usr/bin/env bash

# check if stations are ordered and IDs are incremented by one
# useful assumption in program - is much faster

file="$1"

if [[ -z "$file" ]]; then
  echo "Usage: $0 <file.csv>"
  exit 1
fi

prev_id=""
line_num=0

while IFS=';' read -r id rest; do
  ((line_num++))

  # Skip header (first line)
  if [[ $line_num -eq 1 ]]; then
    continue
  fi

  # Skip empty lines
  [[ -z "$id" ]] && continue

  # Ensure it's an integer
  if ! [[ "$id" =~ ^[0-9]+$ ]]; then
    echo "Line $line_num: invalid ID '$id'"
    exit 1
  fi

  if [[ -n "$prev_id" ]]; then
    expected=$((prev_id + 1))
    if [[ "$id" -ne "$expected" ]]; then
      echo "Mismatch at line $line_num: got $id, expected $expected"
      exit 1
    fi
  fi

  prev_id="$id"
done < "$file"

echo "OK: IDs increment correctly"
