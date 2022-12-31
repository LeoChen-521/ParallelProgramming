#!/bin/bash

#SBATCH -J kernel
#SBATCH -A cs475-575
#SBATCH -p class
#SBATCH --gres=gpu:1
#SBATCH -o kernel.out
#SBATCH -e kernel.err
#SBATCH --mail-type=BEGIN,END,FAIL
#SBATCH --mail-user=chenxia2@oregonstate.edu

for t in 16 32 64 128
do  
    # for 16KiB, 32KiB, 64KiB, 128KiB, 256KiB, 512KiB, 1MiB
    for s in 2048 4096 8192 16384 32769 65536 131072 262144 524288 1048576 2097152
    do
        /usr/local/apps/cuda/cuda-10.1/bin/nvcc -DBLOCKSIZE=$t -DNUMTRIALS=$s -o out project5.cu
        ./out
    done
done