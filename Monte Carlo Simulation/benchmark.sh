#!/bin/csh

# number of threads:
foreach t (1 2 4 8)
        #echo NUMT = $t
        echo ""
        # array size
        foreach s ( 1 10 100 1000 10000 100000 500000 1000000 )
                #echo NUMTRIALS = $s
                g++ -DNUMTRIALS=$s -DNUMT=$t Project1.cpp -o out -lm -fopenmp
                ./out
        end
end