#!/usr/bin/env bash

for INSTANCE in B C X PUC SP I080
do
    ARCHIVE="$INSTANCE.tgz"
    if [ ! -f $ARCHIVE ]
    then
        wget https://steinlib.zib.de/download/$ARCHIVE
    fi
    tar -xf $ARCHIVE -C instancias/
done
