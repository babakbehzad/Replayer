Replayer
========
**Automatic Generation of I/O Trace Generators for HPC Applications**



Publication
-----------

For a full description of the Replayer library please refer to the following
paper:

[Automatic Generation of I/O Kernels for HPC
Applications](http://web.engr.illinois.edu/~bbehza2/files/Babak_Behzad_PDSW2014_paper.pdf)

Babak Behzad, Hoang-Vu Dang, Farah Hariri, Weizhe Zhang, and Marc Snir

Full Paper, Workshop on Parallel Data Storage Workshop
(PDSW 2014), in conjuction with ACM/IEEE Supercomputing 2014.


Description
-----------

![alt
text](http://web.engr.illinois.edu/~bbehza2/files/Replayer-Flow.png "Flow of the Replayer Framework")


Installation
------------
	cd  merger
    make

	cd  codegen
    ./config.sh /opt/hdf5-1.8.9 /opt/boost_1_51_0 /opt/mpich2-1.4.1p1
    make
