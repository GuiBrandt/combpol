#!/usr/bin/env bash

for log in resultados/**/*.log
do
    best=$(grep $log -Poe '(?<=Best objective )([^,]+)')
    heuristic=$(grep $log -Poe '(?<=Set parameter Cutoff to value )(\d+)')
    nodes=$(grep $log -Poe '(?<=Explored )(\d+)')
    bound=$(grep $log -Poe '(?<=best bound )([^,]+)')
    t=$(grep $log -Poe '(?<=simplex iterations\) in )(\d+\.\d+)')
    callback=$(grep $log -Poe '(?<=user-callback )(\d+\.\d+)')
    echo "$log & $best & $heuristic & $nodes & $bound & ${t}s & ${callback}s"
done
