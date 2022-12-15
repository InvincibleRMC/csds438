#!/bin/bash
#SBATCH --nodes=32 -p classc -A sxg125_csds438 --mem=32G

cp c_version/main_openmp.c $PFSDIR
cd $PFSDIR

#compile
gcc main_openmp.c -fopenmp -O3 -o main_openmp

./main_openmp
cp * $SLURM_SUBMIT_DIR