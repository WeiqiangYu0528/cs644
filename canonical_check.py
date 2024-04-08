import os
import subprocess
import re

directory = 'tests/a5-tests/'

for filename in os.listdir(directory):
    if os.path.isfile(os.path.join(directory, filename)):

        command = f'python3 a2_test.py -s {os.path.join(directory, filename)}'
        result = subprocess.run(command, shell=True, capture_output=True, text=True)        
        canonical_status = "Canonical? Yes" if "Canonical? Yes" in result.stdout else "Error"
        
        match = re.search(r'Before: program evaluates to (\d+)', result.stdout)
        before_eval_status = ("Before: evaluates to: " + match.group(1)) if match else "Error"

        match = re.search(r'After CanonicalVisitor: program evaluates to (\d+)', result.stdout)
        canonicalvisitor_eval_status = ("CanonicalVisitor: evaluates to: " + match.group(1)) if match else "Error"

        match = re.search(r'After CFG: program evaluates to (\d+)', result.stdout)
        cfg_eval_status = ("CFG: evaluates to: " + match.group(1)) if match else "Error"


        print(f'{filename}\033[40G{canonical_status}\033[80G{before_eval_status}\033[120G{canonicalvisitor_eval_status}\033[160G{cfg_eval_status}')