#!/bin/bash

# Initialize counters
total_count=0
passed_count=0
declare -a failed_tests

# Path to the tests directories relative to the script location
test_dir="tests/public-tests"
result_file="result.txt"

# Clear the result file at the start
> "$result_file"

# Function to run joosc and check the return value
run_test() {
    local file=$1
    local file_basename=$(basename "$file")
    
    # Determine the expected exit code based on filename prefix
    local expected_exit_code=0
    if [[ $file_basename == Je* ]]; then
        expected_exit_code=42
    fi

    # Run joosc on the file and capture the exit code
    ./joosc "$file" > /dev/null 2>&1
    local exit_code=$?

    # Check if the exit code matches the expected value
    if [[ $exit_code -eq $expected_exit_code ]]; then
        ((passed_count++))
    else
        # Store failed test cases
        failed_tests+=("$file")
    fi
    ((total_count++))
}

# Function to iterate over directories and run tests
run_tests_in_directory() {
    local directory=$1

    # Use process substitution to avoid creating a subshell
    while IFS= read -r -d $'\0' file; do
        run_test "$file"
    done < <(find "$directory" -type f -name "*.java" -print0)
}

# Run tests in syntax-supported directory
run_tests_in_directory "$test_dir"


# Calculate pass rate
pass_rate=$(awk "BEGIN {print ($passed_count / $total_count) * 100}")

# Write summary to result file
{
    echo "Total tests run: $total_count"
    echo "Total passed: $passed_count"
    echo "Pass rate: $pass_rate%"
} >> "$result_file"


echo "Total tests run: $total_count"
echo "Total passed: $passed_count"
echo "Pass rate: $pass_rate%"


# Write failed test cases to result file and print them to console
{
    echo "Failed test cases:"
    for failed_test in "${failed_tests[@]}"; do
        echo "$failed_test"
    done
} | tee -a "$result_file" >&2


# Output the final message regarding the result file
if [[ -s "$result_file" ]]; then
    echo "The test outcomes have been written to $result_file"
else
    echo "There was an error writing the test outcomes."
fi
