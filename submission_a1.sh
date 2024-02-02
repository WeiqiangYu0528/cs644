#!/bin/bash

# Define the name of the output zip file
OUTPUT_ZIP="a1_submission.zip"

# Define the directories and files to be zipped
INCLUDE_DIR="include"
SRC_DIR="src"
TESTS_DIR="tests/syntax-supported"
MAKEFILE="Makefile"

# Zip the directories and file
zip -r $OUTPUT_ZIP $INCLUDE_DIR $SRC_DIR $TESTS_DIR $MAKEFILE

echo "Zipped files into $OUTPUT_ZIP"
