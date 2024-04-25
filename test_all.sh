#!/bin/bash

SCRIPT_DIR="$(pwd)"

TEST_DIR="${SCRIPT_DIR}/tests/a6-tests"

for file in "${TEST_DIR}"/*
do
    filename=$(basename "$file")

    python3 "${SCRIPT_DIR}/a2_test.py" $1 -s "$file" > /dev/null 2>&1
    output=$(bash "${SCRIPT_DIR}/compile.sh")
    echo -e "File: $filename\033[80GCompile output: $output"
done