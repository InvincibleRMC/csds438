#!/bin/bash
#SBATCH --time=00:10:00
#SBATCH -N 2 -n 4 -p classc -A sxg125_csds438

cp ~/assignment/csds438/c_version/bitonic_mpi_2.c $PFSDIR 
cd $PFSDIR
which mpicc
echo "MPI"
mpicc bitonic_mpi_2.c -o bitonic
mpirun -n 4 ./bitonic 16
cp * $SLURM_SUBMIT_DIR


