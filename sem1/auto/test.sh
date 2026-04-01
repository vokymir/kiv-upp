#!/usr/bin/env bash
set -euo pipefail

RUNS=10

bench_commit() {
local commit=$1
local outfile=$2

echo "=== Benchmarking $commit ==="

git checkout "$commit"
./auto/build.sh

# clear previous results

: > "$outfile"

for ((i=1; i<=RUNS; i++)); do
echo "Run $i" | tee -a "$outfile"
./auto/run.sh --large --parallel >> "$outfile"
done
}

bench_commit ebe568c8 ~/school/results_ebe568c8.txt
bench_commit 5ca306aa ~/school/results_5ca306aa.txt

# go back to previous branch

git checkout master
