#!/usr/bin/env python3

import argparse
import difflib
import os
import sys

___doc___ = '''
Compare my own test results, produced by pptok.exe, to the reference test
results, produced by pptoken-ref, provided in CPPGM assignment 1.

The differences are:

- pptok does not print EXIT_SUCCESS or EXIT_FAILURE. Instead, it returns 0 for
  success and 1 for failure.

- Whitespace characters, i.e, space, horizontal tab, vertical tab, form feed, a
  backslash \ followed by a new-line \\n, are ignored in pptok but emitted as
  whitespace-sequence in pptoken.

- Comments (single line comment starting with // and multiline comments enclosed
  in /* ... */) are issued as whitespace-sequence in pptok but ignored in
  pptoken. Though, for testing purpose, whitespace-sequences are ignored.

- The new-line character \\n issues "new-line\\n" in pptok, instead of the
  "new-line 0\\n" in pptoken.

For testing purpose, whitespace-characters, whitespace-sequences, and exit
status are ignored. The new-line token are normalized to "new-line\\n".
'''

def normalize(file):
    out = []
    for line in file:
        type = line.split(' ')[0]
        if type == 'new-line':
            out.append('new-line\n')
        elif type == 'whitespace-sequence':
            pass
        else:
            out.append(line)
    return out

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description = ___doc___,
                                     formatter_class = argparse.RawTextHelpFormatter)

    parser.add_argument('file1', metavar = 'FROM_FILE',
                        type = argparse.FileType('r', encoding = 'utf8'))
    parser.add_argument('file2', metavar = 'TO_FILE',
                        type = argparse.FileType('r', encoding = 'utf8'))

    args = parser.parse_args()

    tmp1 = normalize(args.file1)
    tmp2 = normalize(args.file2)

    retval = 0
    difflines = difflib.unified_diff(tmp1, tmp2, fromfile=args.file1.name,
                                     tofile=args.file2.name)
    for line in difflines:
        if line.startswith('+') or line.startswith('-'):
            retval = 1
            print(line.rstrip('\n'))

    exit(retval)
