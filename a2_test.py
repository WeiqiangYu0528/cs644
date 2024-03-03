import os
import subprocess
import argparse

def parse_arguments():
    parser = argparse.ArgumentParser()
    parser.add_argument('-s', '--single', metavar='dir', type=str)
    parser.add_argument('-a', '--all', action='store_true')
    return parser.parse_args()


def collect_files(directory):
    file_paths = []
    for root, dirs, files in os.walk(directory):
        for file in files:
            if file.endswith('.java'):
                file_path = os.path.join(root, file)
                file_paths.append(file_path)
    return file_paths

def execute_joosc(files):
    command = ['./joosc'] + files
    result = subprocess.run(command, capture_output=True, text=True)
    return result

def add_jsl_files(files, jsl_dir):
    jsl_files = collect_files(jsl_dir)
    files.extend(jsl_files)

def check_error_type(files):
    type_linking_found = False
    hierarchy_found = False
    for file_path in files:
        with open(file_path, 'r', encoding='utf-8') as file:
            content = file.read().upper() 
            if "TYPE_LINKING" in content:
                type_linking_found = True
            if "HIERARCHY" in content:
                hierarchy_found = True
            if type_linking_found and hierarchy_found:
                break
    return type_linking_found, hierarchy_found


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
        add_jsl_files(files, jsl_dir)
        return_code = execute_joosc(files).returncode
        expected_return_code = 0 if item.startswith(('J1_', 'J2_')) else 42
        if return_code != expected_return_code:
            type_linking, hierarchy = check_error_type(files)
            item_string = os.path.join(start_dir, item)
            if type_linking:
                error_type = "TYPE_LINKING"
            elif hierarchy:
                error_type = "HIERARCHY"
            else:
                error_type = "OTHER"
            incorrect_items.append(f"{item_string}\033[80G{error_type}")
        else:
            correct += 1

    print("Incorrect items:")
    for item in incorrect_items:
        print(item)
    print("Correct:", correct)
    print("Total:", len(items_to_process))

def single(dir_or_file):
    if os.path.isdir(dir_or_file):
        files = collect_files(dir_or_file)
    elif os.path.isfile(dir_or_file):
        files = [dir_or_file]
        
    add_jsl_files(files, jsl_dir)
    result = execute_joosc(files)
    print("[", ", ".join(f'"{s}"' for s in files), "]", sep="")
    expected_return_code = 0 if 'J1_' in dir_or_file or 'J2_' in dir_or_file else 42
    print("return:", result.returncode, "expected_return:", expected_return_code)
    if result.stdout:
        print(result.stdout)
    if result.stderr:
        print(result.stderr)


if __name__ == "__main__":
    args = parse_arguments()
    jsl_dir = './JSL_3.0'
    #test_dir = './tests/a3-tests'
    test_dir = './tests/a2-tests'

    if args.single:
        single(args.single)
    elif args.all:
        main(test_dir)
