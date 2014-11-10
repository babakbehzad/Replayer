#!/bin/sh

usage() {
  echo "Usage: $0 <hdf5_dir> <boost_dir> <mpi_dir>"
}

if [ "$#" -le 2 ]
then
    usage
else
  echo HDF5root=$1 >> config.inc
  echo BOOST=$2 >> config.inc
  echo MPIroot=$3 >> config.inc
fi
