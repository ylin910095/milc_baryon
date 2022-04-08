#!/bin/bash
#SBATCH --account=m3926
#SBATCH --nodes=256
#SBATCH --ntasks-per-node=4
#SBATCH --cpus-per-task=32
#SBATCH --time=32:00:00
#SBATCH --job-name=example
#SBATCH --constraint=knl
#SBATCH --qos=regular
#SBATCH --output=./example_slurmout_%j.out

mpi_rank=$(( $SLURM_JOB_NUM_NODES * $SLURM_NTASKS_PER_NODE ))

export OMP_PROC_BIND=true
export OMP_PLACES=threads
export OMP_NUM_THREADS=${SLURM_CPUS_PER_TASK}
export MPICH_MAX_THREAD_SAFETY=multiple

# Load the appropriate conda
conda activate py27

echo "Start time:" $(date)
echo "SLURM_JOB_ID:" $SLURM_JOB_ID
echo "SLURM_JOB_NUM_NODES:" $SLURM_JOB_NUM_NODES

MILC_bin=./ks_spectrum_hisq_gb_baryon_blind_no_sink_links
milcoutfile=./example_milc_$SLURM_JOB_ID.out
inputfile=./example.inp

srun --cpu-bind=cores -n ${mpi_rank} -N $SLURM_JOB_NUM_NODES ${MILC_bin} ${inputfile} >> ${milcoutfile}
echo "End time:" $(date)
