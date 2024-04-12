#!/bin/bash

SCRIPT_DIR="$(pwd)"

TEST_DIR="${SCRIPT_DIR}/tests/a5-tests"

for file in "${TEST_DIR}"/*
do
    filename=$(basename "$file")

    python "${SCRIPT_DIR}/a2_test.py" -s "$file" > /dev/null 2>&1
    output=$(bash "${SCRIPT_DIR}/compile.sh")
    echo -e "File: $filename\033[80GCompile output: $output"
done