import os
import subprocess

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
    result = subprocess.run(command, capture_output=True)
    return result.returncode

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
        return_code = execute_joosc(files)
        expected_return_code = 0 if item.startswith(('J1_', 'J2_')) else 42
        if return_code != expected_return_code:
            type_linking, hierarchy = check_error_type(files)
            item_string = os.path.join(start_dir, item)
            if type_linking:
                error_type = "TYPE_LINKING"
            if hierarchy:
                error_type = "HIERARCHY"
            incorrect_items.append(f"{item_string}\033[80G{error_type}")
        else:
            correct += 1

    print("Incorrect items:")
    for item in incorrect_items:
        print(item)
    print("Correct:", correct)
    print("Total:", len(items_to_process))

jsl_dir = './JSL_2.0'
test_dir = './tests/a2-tests'
main(test_dir)