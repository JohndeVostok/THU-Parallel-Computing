#include <cstdint>
#include <cstdio>
#include <vector>
#include <mpi.h>

uint32_t min(uint32_t x, uint32_t y) {
    return x < y ? x : y;
}

int main(int argc, char** argv) {
    MPI_Init(0, 0);
    int mpi_size, mpi_rank;
    MPI_Comm_size(MPI_COMM_WORLD, &mpi_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);

    uint32_t n = 100;
    std::vector <uint32_t> base(n), resa(n), resb(n);
    std::vector <int> recvcnt(mpi_size), displs(mpi_size);
    for (int i = 0; i < n; i++) base[i] = i;
    for (int i = 0; i < mpi_size; i++) recvcnt[i] = n * (i+1) / mpi_size - n * i / mpi_size;
    for (int i = 0; i < mpi_size; i++) displs[i] = n * i / mpi_size;
    uint32_t head, tail;
    head = n * mpi_rank / mpi_size;
    tail = n * (mpi_rank+1) / mpi_size;
    // printf("%d %d %d\n", mpi_rank, head, tail);

    MPI_File fh;
    MPI_Status status;
    MPI_File_open(MPI_COMM_WORLD, "data", MPI_MODE_CREATE|MPI_MODE_WRONLY, MPI_INFO_NULL, &fh);
    MPI_File_write_at(fh, head * sizeof(int), &base[head], tail-head, MPI_INT, &status);
    MPI_File_close(&fh);
    MPI_Barrier(MPI_COMM_WORLD);

    MPI_Gatherv(&base[head], tail-head, MPI_INT, &resb[0], &recvcnt[0], &displs[0], MPI_INT, 0, MPI_COMM_WORLD);

    if (mpi_rank == 0) {
        FILE *pf = fopen("data", "rb");
        fread(&resa[0], sizeof(int), n, pf);

        // Validate res.
        bool flag = 1;
        for (int i = 0; i < n; i++) if (resa[i] != resb[i]) flag = 0;
        if (flag) {
            printf("OK!\n");
        } else {
            printf("Not OK!\n");
        }
    }

    MPI_Finalize();
}