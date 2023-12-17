#!/usr/bin/env bash

for instance in instancias/**/*.stp
do
    instance_name=${instance/.stp/}
    output=${instance_name/instancias/resultados}
    if [ ! -f "${output}.sol" ]
    then
        echo "[${instance}]"
        time python3 main.py $instance
    fi
done
