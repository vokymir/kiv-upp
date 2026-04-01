#!/bin/bash

# Usage: ./average_times.sh logfile.txt
# useful to process/aggreagate data created via test.sh

if [ $# -ne 1 ]; then
    echo "Usage: $0 <logfile>"
    exit 1
fi

file="$1"

awk '
{
    # Match lines with [NUMBERms] Task
    if (match($0, /^\[([0-9]+)ms\] (.+)/, arr)) {
        time = arr[1]
        task = arr[2]
        sum[task] += time
        count[task] += 1
    }
}
END {
    for (t in sum) {
        printf "%s: %.2f ms\n", t, sum[t]/count[t]
    }
}' "$file"
