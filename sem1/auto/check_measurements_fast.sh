#!/usr/bin/env bash
# check_measurements_fast.sh
# Fast validation of measurements CSV with progress output

file="$1"
if [[ -z "$file" ]]; then
  echo "Usage: $0 <file.csv>"
  exit 1
fi

prev_id=""
prev_ordinal=""
prev_year=""
prev_month=""
prev_day=""
line_num=0
progress_interval=10000  # print every N lines

while IFS=';' read -r station_id ordinal year month day value; do
  ((line_num++))
  [[ $line_num -eq 1 ]] && continue  # skip header
  [[ -z "$station_id" ]] && continue # skip empty lines

  # validate integers
  if ! [[ "$station_id" =~ ^[0-9]+$ && "$ordinal" =~ ^[0-9]+$ && "$year" =~ ^[0-9]+$ && "$month" =~ ^[0-9]+$ && "$day" =~ ^[0-9]+$ ]]; then
    echo "Line $line_num: invalid numeric field"
    exit 1
  fi

  # --- first measurement of a new station ---
  if [[ -z "$prev_id" || "$station_id" -ne "$prev_id" ]]; then
    if [[ "$ordinal" -ne 0 ]]; then
      echo "Line $line_num: first measurement of station $station_id must have ordinal 0 (got $ordinal)"
      exit 1
    fi

    # progress output for new station
    if [[ -n "$prev_id" ]]; then
      echo "All measurements from station $prev_id checked, now on line $line_num"
    fi

    prev_id="$station_id"
    prev_ordinal="$ordinal"
    prev_year="$year"
    prev_month="$month"
    prev_day="$day"
    continue
  fi

  # --- station_id must not decrease ---
  if [[ "$station_id" -lt "$prev_id" ]]; then
    echo "Line $line_num: station_id $station_id < previous $prev_id"
    exit 1
  fi

  # --- ordinal must increment by 1 within same station ---
  expected=$((prev_ordinal + 1))
  if [[ "$ordinal" -ne "$expected" ]]; then
    echo "Line $line_num: ordinal $ordinal, expected $expected"
    exit 1
  fi

  # --- date must strictly increase ---
  prev_ymd=$((10#$prev_year*10000 + 10#$prev_month*100 + 10#$prev_day))
  current_ymd=$((10#$year*10000 + 10#$month*100 + 10#$day))
  if [[ $current_ymd -le $prev_ymd ]]; then
    echo "Line $line_num: date $year-$month-$day not after previous $prev_year-$prev_month-$prev_day"
    exit 1
  fi

  # --- update trackers ---
  prev_ordinal="$ordinal"
  prev_year="$year"
  prev_month="$month"
  prev_day="$day"

  # --- periodic progress output ---
  if (( line_num % progress_interval == 0 )); then
    echo "Checked $line_num lines, current station $station_id"
  fi

done < "$file"

# final station output
if [[ -n "$prev_id" ]]; then
  echo "All measurements from station $prev_id checked, total lines $line_num"
fi

echo "OK: all checks passed"
