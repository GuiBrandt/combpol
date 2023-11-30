#!/usr/bin/env bash

for INSTANCE in B C D E MC X SP PUC I080 LIN
do
    ARCHIVE="$INSTANCE.tgz"
    if [ ! -f $ARCHIVE ]
    then
        wget https://steinlib.zib.de/download/$ARCHIVE
    fi
    tar -xf $ARCHIVE -C instancias/
done
