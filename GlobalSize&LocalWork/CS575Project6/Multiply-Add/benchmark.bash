#!/bin/bash
#SBATCH -J opencl
#SBATCH -A cs475-575
#SBATCH -p class
#SBATCH --gres=gpu:1
#SBATCH -o opencl.out
#SBATCH -e opencl.err
#SBATCH --mail-type=BEGIN,END,FAIL
#SBATCH --mail-user=chenxia2@oregonstate.edu
for g in 1 128 512 1024 2048 3072 4096 5120 6144 8192 102400
do
for l in 8 16 32 64 128 256 512
do
g++ -o second second.cpp /usr/local/apps/cuda/cuda-10.1/lib64/libOpenCL.so.1.1 -DNMB=$g -DLOCAL_SIZE=$l -lm -fopenmp
./second
done
done
