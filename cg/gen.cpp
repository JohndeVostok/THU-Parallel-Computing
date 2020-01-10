#include <cstdio>
#include <cstdlib>
#include <vector>

int main(int argc, char** argv) {
    int n = 512;
    float r = 1.0;
    if (argc > 1) {
        n = atof(argv[1]);
    }
    if (argc > 2) {
        r = atof(argv[2]);
    }
    FILE *pf = fopen("mat.txt", "w");
    std::vector<std::vector<double>> a(n, std::vector<double>(n));
    fprintf(pf, "%d\n", n);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < i; j++) {
            a[i][j] = a[j][i];
        }
        for (int j = i; j < n; j++) {
            if (rand() % 100 < 100 * r) {
                a[i][j] = rand() % 100 + 1;
            } else {
                a[i][j] = 0;
            }
        }
    }
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            double tmp = a[i][j];
            fprintf(pf, "%.2lf ", tmp);
        }
        fprintf(pf, "\n");
    }
    for (int i = 0; i < n; i++) {
        double tmp = rand() % 100 + 1;
        fprintf(pf, "%.2lf ", tmp);
    }
    fprintf(pf, "\n");
}