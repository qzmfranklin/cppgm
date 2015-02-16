#!/bin/sh

rsync -avr --exclude-from=EXCLUSIONS --delete . ../cppgm-pa1/

