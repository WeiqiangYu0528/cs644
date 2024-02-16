import os
import subprocess

def collect_files(directory):
    file_paths = []
    for root, dirs, files in os.walk(directory):
        for file in files:
            file_path = os.path.join(root, file)
            file_paths.append(file_path)
    return file_paths

def execute_joosc(files):
    command = ['./joosc'] + files
    result = subprocess.run(command, capture_output=True)
    return result.returncode

def main(start_dir):
    items_to_process = [] 
    for item in os.listdir(start_dir):
        item_path = os.path.join(start_dir, item)
        if os.path.isdir(item_path):
            files = collect_files(item_path)
            items_to_process.append((item, files))
        elif os.path.isfile(item_path):
            items_to_process.append((item, [item_path]))

    incorrect_items = []
    correct = 0
    for item, files in items_to_process:
        return_code = execute_joosc(files)
        expected_return_code = 0 if item.startswith(('J1_', 'J2_')) else 42
        if return_code != expected_return_code:
            incorrect_items.append(os.path.join(start_dir, item))
        else:
            correct += 1

    print("Incorrect items:")
    for item in incorrect_items:
        print(item)
    print("Correct:", correct)
    print("Total:", len(items_to_process))

test_dir = './tests/a2-tests'
main(test_dir)