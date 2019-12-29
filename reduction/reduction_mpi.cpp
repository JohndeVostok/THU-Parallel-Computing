#include <iostream>
#include <cmath>
#include <algorithm>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <sys/time.h>
#include <vector>
#include <mpi.h>

#define epsilon 1.e-8

using namespace std;

int main (int argc, char* argv[]) {
    MPI_Init(0, 0);
    int mpi_size, mpi_rank;
    MPI_Comm_size(MPI_COMM_WORLD, &mpi_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);

    int M,N;
    string T,P,Db;
    M = atoi(argv[1]);
    N = atoi(argv[2]);

    double elapsedTime,elapsedTime2;
    timeval start,end,end2;

    if(argc > 3){
        T = argv[3];
        if(argc > 4) {
            P = argv[4];
            if(argc > 5){
                Db = argv[5];
            }
        }
    }
 // cout<<T<<P<<endl;
  
    double **U_t;
    double alpha, beta, gamma, *Alphas, *Betas, *Gammas;

    int acum = 0;
    int temp1, temp2;

    U_t = new double*[N];
    Alphas = new double[M * M];
    Betas = new double[M * M];
    Gammas = new double[M * M];

    for(int i =0; i<N; i++){
        U_t[i] = new double[N];
        // Alphas[i] = new double[N];
        // Betas[i] = new double[N];
        // Gammas[i] = new double[N];
    }


  //Read from file matrix, if not available, app quit
  //Already transposed

    ifstream matrixfile("matrix");
    if(!(matrixfile.is_open())){
        cout<<"Error: file not found"<<endl;
        return 0;
    }
    for (int i = 0; i < M; i++) {
        for (int j =0; j < N; j++) {
           matrixfile >> U_t[i][j];
        }
    }
    matrixfile.close();

  /* Reductions */

    int head = M * M * mpi_rank / mpi_size, tail = M * M * (mpi_rank+1) / mpi_size;
    std::vector <int> recvcnt(mpi_size), displs(mpi_size);
    for (int i = 0; i < mpi_size; i++) recvcnt[i] = M * M * (i+1) / mpi_size - M * M * i / mpi_size;
    for (int i = 0; i < mpi_size; i++) displs[i] = M * M * i / mpi_size;
    // for (int x : recvcnt) printf("%d ", x); printf("\n");
    // for (int x : displs) printf("%d ", x); printf("\n");

    gettimeofday(&start, NULL);
    for (int t = head; t < tail; t++) {
        int i = t / M, j = t % M;
        alpha =0.0;
        beta = 0.0;
        gamma = 0.0;
        for(int k = 0; k<N; k++){
            alpha = alpha + (U_t[i][k] * U_t[i][k]);
            beta = beta + (U_t[j][k] * U_t[j][k]);
            gamma = gamma + (U_t[i][k] * U_t[j][k]);
        }
        Alphas[t] = alpha;
        Betas[t] = beta;
        Gammas[t] = gamma;
    }

    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Gatherv(&Alphas[head], tail-head, MPI_DOUBLE, Alphas, &recvcnt[0], &displs[0], MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Gatherv(&Betas[head], tail-head, MPI_DOUBLE, Betas, &recvcnt[0], &displs[0], MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Gatherv(&Gammas[head], tail-head, MPI_DOUBLE, Gammas, &recvcnt[0], &displs[0], MPI_DOUBLE, 0, MPI_COMM_WORLD);
    gettimeofday(&end, NULL);

// fix final result


  //Output time and iterations
    if (mpi_rank == 0) {
        ofstream Af;
        Af.open("AlphasMPI.mat"); 
        for (int i = 0; i < M; i++) {
            for (int j = 0; j < M; j++) {
                Af << " " << Alphas[i * M + j];
            }
            Af<<"\n";
        }
        Af.close();

        ofstream Uf;
        Uf.open("BetasMPI.mat");
        for (int i = 0; i < M; i++) {
            for (int j = 0; j < M; j++) {
                Uf << " " << Betas[i * M + j];
            }
            Uf<<"\n";
        }
        Uf.close();

        ofstream Vf;
        Vf.open("GammasMPI.mat");
        for (int i = 0; i < M; i++) {
            for (int j = 0; j < M; j++) {
                Vf << " " << Gammas[i * M + j];
            }
            Vf<<"\n";
        }
        Vf.close();
    }
   
    for (int i = 0; i<N;i++) {
        // delete [] Alphas[i];
        delete [] U_t[i];
        // delete [] Betas[i];
        // delete [] Gammas[i];
    }
    delete [] Alphas;
    delete [] Betas;
    delete [] Gammas;
    delete [] U_t;

    MPI_Finalize();
    return 0;
}
