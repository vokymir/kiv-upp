#!/usr/bin/env bash

awk -F';' '
NR == 2 { min = $3 }      # initialize with first data row
NR > 1 {
    if ($3 < min) {
        min = $3
    }
}
END {
    print min
}
' $1
