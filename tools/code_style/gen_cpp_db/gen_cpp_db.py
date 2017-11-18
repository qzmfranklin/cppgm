#!/usr/bin/python3

# This reads the _compile_command files :generate_compile_commands_action
# generates a outputs a compile_commands.json file at the top of the source
# tree for things like clang-tidy to read.

# Overall usage directions: run bazel with
# --experimental_action_listener=//tools/actions:generate_compile_commands_listener
# for all the files you want to use clang-tidy with and then run this script.
# Afer that, `clang-tidy build_tests/gflags.cc` should work.

import glob
import json
import os
import pathlib
import shlex
import subprocess
import sys


def get_bazel_info(key):
    cmd = ['bazel', 'info', key]
    return subprocess.check_output(cmd).decode().strip()


def main(argv):
    data = []
    execution_root = get_bazel_info('execution_root')
    compile_command_fname_pattern = os.path.normpath(
        os.path.join(
            get_bazel_info('bazel-bin'), '../extra_actions',
            'tools/code_style/gen_cpp_db', '**/*_cpp_compile_command'))
    for fname in glob.iglob(compile_command_fname_pattern, recursive=True):
        with open(fname, 'r') as f:
            datum = json.load(f)
            # Empty json object means that this file is blacklisted, e.g., under
            # third_party.
            if not datum:
                continue
            datum['directory'] = execution_root
            data.append(datum)
    data.sort(key=lambda x: x['file'])
    fname = os.path.join(get_bazel_info('workspace'), 'compile_commands.json')
    with open(fname, 'w') as f:
        json.dump(data, f, indent=4, sort_keys=True)


if __name__ == '__main__':
    sys.exit(main(sys.argv))
