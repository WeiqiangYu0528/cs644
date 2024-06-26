#!/bin/bash
# Do clean fisrt
make clean
rm a3.log
git log --pretty=format:"%h - %an, %ar : %s" > a3.log
# Define the name of the output zip file
OUTPUT_ZIP="a3_submission.zip"

# Define the directories and files to be zipped
INCLUDE_DIR="include"
SRC_DIR="src"
MAKEFILE="Makefile"
TESTS_DIR="type-checking-tests"
LOG="a3.log"

# Zip the directories and file
zip -r $OUTPUT_ZIP $INCLUDE_DIR $SRC_DIR $TESTS_DIR $MAKEFILE $LOG

echo "Zipped files into $OUTPUT_ZIP"