#!/bin/csh

# number of threads:
foreach t (1 2 4 8 )
        #echo NUMT = $t
        echo ""
        # array size
        foreach s ( 5 10 20 40 80 160 320 640 1280 )
                #echo NUMNODES = $s
                g++ -DNUMNODES=$s -DNUMT=$t project2_1.cpp -o out -lm -fopenmp
                ./out
        end
end
