#!/bin/bash
#SBATCH --job-name=Binning
#SBATCH --chdir=.
#SBATCH --output=%j.out
#SBATCH --error=%j.err
#SBATCH --time=00:05:00
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=40
#SBATCH --gres=gpu:1
#SBATCH --reservation=PUMPS+AI22-day3

./binning -a
