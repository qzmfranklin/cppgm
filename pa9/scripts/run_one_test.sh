#!/bin/bash

app=$1
suffix=$2
testbase=$3

./$app -o $testbase.$suffix.program $testbase.t.* 1> $testbase.$suffix.impl.stdout 2> $testbase.$suffix.impl.stderr
impl_exit_status=$?

echo $impl_exit_status > $testbase.$suffix.impl.exit_status

if [ $impl_exit_status -eq 0 ]
then timeout 600 ./$testbase.$suffix.program < $testbase.stdin 1> $testbase.$suffix.program.stdout 2> $testbase.$suffix.program.stderr; echo $? > $testbase.$suffix.program.exit_status
fi

