#!/bin/bash

#for f in $(find . -name '*output*'); do ./dataplot.bash $f; done


sed 's/\r//' $1 | sed 's/x(//g' | sed 's/x_(//g' | sed 's/y_(//g' | sed 's/y(//g' | sed 's/)//g' | sed 's/ -> /\n/g' | egrep -o '^[^=]*=' | sed 's/ =//g' | awk '!seen[$0]++' | sed 's/,/ /g'> "$1"plot.txt;
