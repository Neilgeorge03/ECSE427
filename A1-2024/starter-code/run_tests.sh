#!/bin/bash

# Function to clean up the directory, keeping only specified files
cleanup() {
    echo "Cleaning up the directory..."
    find . -type f ! -name 'helpers.c' \
                 ! -name 'helpers.h' \
                 ! -name 'helpers.o' \
                 ! -name 'interpreter.c' \
                 ! -name 'interpreter.h' \
                 ! -name 'interpreter.o' \
                 ! -name 'Makefile' \
                 ! -name 'mysh' \
                 ! -name 'README.md' \
                 ! -name 'run_tests.sh' \
                 ! -name 'shell.c' \
                 ! -name 'shell.h' \
                 ! -name 'shellmemory.c' \
                 ! -name 'shellmemory.h' \
                 ! -name 'shellmemory.o' \
                 ! -name 'shell.o' \
                 -exec rm -f {} +

	     find . -type d -empty -exec rmdir {} +
	 }

# Clean and build the project
make clean
make mysh

# Array of test files and their expected result files without the .txt extension
declare -A tests=(
    ["badcommand"]="badcommand_result"
    ["blankline"]="blankline_result"
    ["echo"]="echo_result"
    ["ls"]="ls_result"
    ["mkdir"]="mkdir_result"
    ["prompt"]="prompt_result"
    ["oneline"]="oneline_result"
    ["oneline2"]="oneline2_result"
    ["run"]="run_result"
    ["set"]="set_result"
)

# Run tests and compare outputs
for test_file in "${!tests[@]}"; do
    result_file=${tests[$test_file]}
    echo "Testing: $test_file.txt"
    diff -b <(./mysh < "../test-cases/$test_file.txt") "../test-cases/$result_file.txt"
    
    # Clean up after each test
    cleanup
done

