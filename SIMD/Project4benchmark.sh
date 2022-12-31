#!/bin/csh

foreach s ( 1024 2024 4096 8192 16384 32769 65536 131072 262144 524288 1048576 2097152 4194304 8388608 )
    #echo NUMNODES = $s
    g++ -DARRAYLENGTH=$s Project4.cpp -o out -lm -fopenmp
    ./out
end
