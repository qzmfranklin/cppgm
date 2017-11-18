#!/usr/bin/env python3
''' Generate commands for reformatting the lines modified by a diff patch.

Typical usage:
    git diff --cached | <this-script>
'''

import argparse
import os
import subprocess
import sys
import unidiff

CLANG_FORMAT_EXTS = {'cc', 'cpp', 'C', 'c', 'h', 'hpp', 'hh', 'ipp', 'java'}


def cmds_from_diff(diff_string, *, modified_only=True):
    ''' Construct a list of commands to issue.

    Args:
        diff_string: A string containing the git unified diff.
        modified_only: The generated commands will only reformat the hunks as
            modifed in the diff_string.  When set to False, the generated
            commands will reformat the entire file if any part of the file is
            touched by the diff_string.

    Returns:
        Each command is a list suitable for running with subprocess.call().
    '''
    patch_set = unidiff.PatchSet.from_string(diff_string)
    for patched_file in patch_set:
        cmd = []

        # In git, all the target files are prefixed by path 'b/'.  Strip that to
        # recover the actual path for the target file.
        fname = os.path.relpath(patched_file.target_file, 'b')

        base_name, ext_name = os.path.splitext(fname)
        if not ext_name.startswith('.'):
            continue

        ext_name = ext_name[1:]
        if ext_name in CLANG_FORMAT_EXTS:
            # Reformat C++ and Java in-place using `clang-format`.
            cmd = ['clang-format', '-i', fname]
            if modified_only:
                for hunk in patched_file:
                    start_lineno = hunk.target_start
                    end_lineno = hunk.target_start + hunk.target_length
                    option = '--lines=%s:%s' % (start_lineno, end_lineno)
                    cmd.append(option)
        elif ext_name == 'py':
            # Reformat python in-place using `yapf`.
            cmd = ['yapf', '-i', fname]
            if modified_only:
                for hunk in patched_file:
                    start_lineno = hunk.target_start
                    end_lineno = hunk.target_start + hunk.target_length
                    option = '--lines=%s-%s' % (start_lineno, end_lineno)
                    cmd.append(option)
        else:
            # We cannot handle other types
            pass

        yield cmd


def update_parser(parser):
    parser.add_argument(
        '-i',
        dest='input',
        metavar='IFILE',
        type=lambda x: sys.stdin if x == '-' else open(x, 'r'),
        default='-',
        help='''input filename, '-' means stdin''')
    parser.add_argument(
        '-o',
        dest='output',
        metavar='OFILE',
        type=lambda x: sys.stdout if x == '-' else open(x, 'r'),
        default='-',
        help='''output filename, '-' means stdout''')
    parser.add_argument(
        '-m',
        '--modified-only',
        action='store_true',
        help='''when set, the generated commands only reformats the hunks
        modified by the input diff''')
    parser.add_argument(
        '-x',
        '--execute',
        action='store_true',
        help='''when set, greedily issue the generated commands as well''')


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    update_parser(parser)

    args = parser.parse_args()
    for cmd in cmds_from_diff(
            args.input.read(), modified_only=args.modified_only):
        if args.execute:
            subprocess.call(cmd)
        else:
            print(subprocess.list2cmdline(cmd))


if __name__ == '__main__':
    main()
