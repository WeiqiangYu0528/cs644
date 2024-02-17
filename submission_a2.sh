#!/bin/bash
# Do clean fisrt
make clean
rm a2.log
git log --pretty=format:"%h - %an, %ar : %s" > a2.log
# Define the name of the output zip file
OUTPUT_ZIP="a2_submission.zip"

# Define the directories and files to be zipped
INCLUDE_DIR="include"
SRC_DIR="src"
MAKEFILE="Makefile"
LOG="a2.log"

# Zip the directories and file
zip -r $OUTPUT_ZIP $INCLUDE_DIR $SRC_DIR $TESTS_DIR $MAKEFILE $LOG

echo "Zipped files into $OUTPUT_ZIP"