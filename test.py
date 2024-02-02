import os
import subprocess

def run_joosc_on_java_files(directory):
    # Iterate through all files in the directory and its subdirectories
    for root, dirs, files in os.walk(directory):
        for file in files:
            if file.endswith(".java"):
                # Construct the full file path
                full_path = os.path.join(root, file)
                
                # Run the joosc command on the file
                result = subprocess.run(["./joosc", full_path], capture_output=True, text=True)

                # Print the output of the command
                if result is not None:
                    if result.stderr:  # Check if there is any error message
                        print(f"Running joosc on {full_path}:")
                        print(result.stdout)
                        print(result.stderr)


# Replace with your directory path
directory_path1 = 'tests/syntax-supported/class-structure'
directory_path2 = 'tests/syntax-supported/method-and-field-access'
directory_path3 = 'tests/syntax-supported/modifiers'
directory_path4 = 'tests/syntax-supported/constructors'
directory_path5 = 'tests/syntax-supported/method-structure'
directory_path6 = 'tests/syntax-supported/types'
directory_path7 = 'tests/syntax-supported/literals-and-names'
directory_path8 = 'tests/syntax-supported/operations'

run_joosc_on_java_files(directory_path1)
run_joosc_on_java_files(directory_path2)
run_joosc_on_java_files(directory_path3)
run_joosc_on_java_files(directory_path4)
run_joosc_on_java_files(directory_path5)
run_joosc_on_java_files(directory_path6)
run_joosc_on_java_files(directory_path7)
run_joosc_on_java_files(directory_path8)
