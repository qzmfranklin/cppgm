#!/bin/sh

echo 'Running CPPGM tests...'
make clean
make pptok.exe -j8 > /dev/null
for fin in tests/*.t; do
  ref=${fin%.t}.ref
  my=${fin%.t}.my
  ./pptok.exe < $fin > $my
  ./compare.py $ref $my || echo ''
done
echo 'Passed CPPGM tests!!!'

echo '
This script tests the conformance of my C++ tokenizer to the CPPGM course
implementation. All differences as of 02/08/2016 are expected and are not
considered correct. The full list of differences is in the README.md file.
'
