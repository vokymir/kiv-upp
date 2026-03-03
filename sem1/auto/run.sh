#!/bin/bash

# Base path for datasets
BASE="./data"

# Default size is small
SIZE="small"

# Parse short flags for dataset size
while [[ $# -gt 0 ]]; do
  case $1 in
    -s|--small) SIZE="small"; shift ;;
    -m|--medium) SIZE="medium"; shift ;;
    -l|--large) SIZE="large"; shift ;;
    *) break ;; # stop parsing, rest are passed to program
  esac
done

# Build file paths
STANICE="$BASE/$SIZE/stanice.csv"
MERENI="$BASE/$SIZE/mereni.csv"

# Pass all remaining args as-is
./bin/upp_sp1 "$STANICE" "$MERENI" "$@"
