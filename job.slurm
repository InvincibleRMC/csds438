#!/bin/bash
#SBATCH --nodes=8 --cpus-per-task=4 -p classc -A sxg125_csds438 --mem=32G

cp main_openmp.c $PFSDIR
cd $PFSDIR

module load gcc

#compile
gcc main_openmp.c -fopenmp -lm -O3 -o main_openmp

mkdir data

./main_openmp

cp -r * $SLURM_SUBMIT_DIR