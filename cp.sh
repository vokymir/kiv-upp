#!/usr/bin/env bash

# Ensure we're inside a git repo
git rev-parse --is-inside-work-tree >/dev/null 2>&1 || {
    echo "Not inside a git repository."
    exit 1
}

git ls-files -z | while IFS= read -r -d '' file; do
    echo "===== $file ====="
    cat "$file"
    echo
done | tee /dev/tty | xclip -selection clipboard

echo "All tracked file contents copied to clipboard."
