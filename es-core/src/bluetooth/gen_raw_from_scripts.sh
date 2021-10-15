#!/bin/bash
# baarreth, october 2021

# https://www.stroustrup.com/C++11FAQ.html#raw-strings
# https://stackoverflow.com/questions/410980/include-a-text-file-in-a-c-program-as-a-char#410988

for i in scripts/*.sh
  do
    SCRIPT=${i#*/}
    CVAR=${SCRIPT//./_}
    echo -n "R\"===("  > ${CVAR}.raw
    cat $i  >> ${CVAR}.raw
    echo ")===\"" >> ${CVAR}.raw
done
