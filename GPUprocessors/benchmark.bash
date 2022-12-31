#!/bin/bash
#SBATCH -J project7
#SBATCH -A cs475-575
#SBATCH -p class
#SBATCH -N 8 # number of nodes
#SBATCH -n 8 # number of tasks
#SBATCH -o project7.out
#SBATCH -e project7.err
#SBATCH --mail-type=END,FAIL
#SBATCH --mail-user=chenxia2@oregonstate.edu

for n in 1 2 4 8
  do
    for m in 1 2 4 8 16 32
      do
        module load slurm
        module load openmpi/3.1
        mpic++ -DNMB=$n project7.cpp -o out -lm
        mpiexec -mca btl self,tcp -np $m out
      done
  done