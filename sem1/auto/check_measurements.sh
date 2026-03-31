#!/usr/bin/env bash

# check if measurements are:
# 1. ordered by station ID
# 2. ordinal is increasing by one
# 3. every measurement is done later than the previous

file="$1"

if [[ -z "$file" ]]; then
  echo "Usage: $0 <file.csv>"
  exit 1
fi

prev_id=""
prev_ordinal=""
prev_date=""
line_num=0

while IFS=';' read -r station_id ordinal year month day value; do
  ((line_num++))

  # Skip header
  if [[ $line_num -eq 1 ]]; then
    continue
  fi

  # Skip empty lines
  [[ -z "$station_id" ]] && continue

  # Validate integers
  if ! [[ "$station_id" =~ ^[0-9]+$ && "$ordinal" =~ ^[0-9]+$ ]]; then
    echo "Line $line_num: invalid station_id or ordinal"
    exit 1
  fi

  current_date=$(printf "%04d-%02d-%02d" "$year" "$month" "$day")
  current_ts=$(date -d "$current_date" +%s 2>/dev/null)

  if [[ -z "$current_ts" ]]; then
    echo "Line $line_num: invalid date $current_date"
    exit 1
  fi

  # --- station_id check ---
  if [[ -n "$prev_id" ]]; then
    if [[ "$station_id" -lt "$prev_id" ]]; then
      echo "Line $line_num: station_id $station_id < previous $prev_id"
      exit 1
    fi

    if [[ "$station_id" -gt "$prev_id" ]]; then
      # NEW STATION → ordinal must be 0
      if [[ "$ordinal" -ne 0 ]]; then
        echo "Line $line_num: new station_id $station_id must start with ordinal 0 (got $ordinal)"
        exit 1
      fi

      # Reset tracking
      prev_ordinal="$ordinal"
      prev_date="$current_date"
      prev_id="$station_id"
      continue
    fi
  fi

  # --- ordinal check (+1 exactly within same station) ---
  if [[ -n "$prev_ordinal" ]]; then
    expected=$((prev_ordinal + 1))
    if [[ "$ordinal" -ne "$expected" ]]; then
      echo "Line $line_num: ordinal $ordinal, expected $expected"
      exit 1
    fi
  fi

  # --- date check (strictly increasing within same station) ---
  if [[ -n "$prev_date" ]]; then
    prev_ts=$(date -d "$prev_date" +%s)
    if [[ "$current_ts" -le "$prev_ts" ]]; then
      echo "Line $line_num: date $current_date not after $prev_date"
      exit 1
    fi
  fi

  prev_id="$station_id"
  prev_ordinal="$ordinal"
  prev_date="$current_date"

done < "$file"

echo "OK: all checks passed"
