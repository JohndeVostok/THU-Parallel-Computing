python random_matrix.py 1000 1000
g++ reduction_seq.cpp -o seq
mpicxx reduction_mpi.cpp -o mpi
./seq 1000 1000 -d -d -d
mpirun -np 24 ./mpi 1000 1000
g++ validation_mpi.cpp
./a.out
