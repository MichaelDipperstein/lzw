#!/bin/bash

if [ -f foo ]
then
    echo this script requies use of a file foo
    exit 1
fi

if [ -f bar ]
then
    echo this script requies use of a file bar
    exit 1
fi

for X in *
do
    if [ -f "$X" ]
    then
        echo checking $X
        filesize=$(stat -c '%s' $X)
        printf "\tuncompressed size:\t%d\n" $filesize
        ./sample -c -i $X -o foo
        filesize=$(stat -c '%s' foo)
        printf "\tcompressed size:\t%d\n" $filesize
        ./sample -d -i foo -o bar
        diff $X bar
        rm foo
        rm bar
    fi
done

exit 0
