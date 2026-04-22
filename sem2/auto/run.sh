#!/usr/bin/env bash

DIR="bin"
BIN="UPP_SP2"
HOSTFILE=".hostfile"
NP=10 # total number of processes
N=3 # worker A
M=2 # worker B

mpirun --oversubscribe -np "${NP}" --hostfile "${HOSTFILE}" \
  "./${DIR}/${BIN}" -n "${N}" -m "${M}"
