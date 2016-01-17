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
