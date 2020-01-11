#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <vector>
#include <chrono>

#include <omp.h>

const double eps = 0.00001;
const double thresh = 0.1;

int nthread;

class smatrix {
public:
    struct node {
        int x, y;
        double v;
    };
    std::vector<node> nodes;
    int n;

    smatrix (int nn) {
        n = nn;
    }
    ~smatrix () {}

    int ins(int x, int y, double v) {
        node t;
        t.x = x;
        t.y = y;
        t.v = v;
        nodes.emplace_back(t);
        return 0;
    }

    int lmul(std::vector<double> &res, std::vector<double> &v) {
        if (v.size() != n) {
            return -1;
        }
        // int nthread = omp_get_num_procs();
        std::vector<std::vector<double>> tmp(nthread, std::vector<double>(n));
#pragma omp parallel for num_threads(nthread)
        for (int i = 0; i < nodes.size(); i++)
        {
            auto &node = nodes[i];
            tmp[omp_get_thread_num()][node.y] += v[node.x] * node.v;
        }
        res.clear();
        res.resize(n, 0);
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < nthread; j++) {
                res[i] += tmp[j][i];
            }
        }
        return 0;
    }

    int rmul(std::vector<double> &res, std::vector<double> &v) {
        if (v.size() != n) {
            return -1;
        }
        // int nthread = omp_get_num_procs();
        std::vector<std::vector<double>> tmp(nthread, std::vector<double>(n));
#pragma omp parallel for num_threads(nthread)
        for (int i = 0; i < nodes.size(); i++)
        {
            auto &node = nodes[i];
            // printf("%d %d %d\n", node.x, node.y, omp_get_thread_num());
            tmp[omp_get_thread_num()][node.x] += v[node.y] * node.v;
        }
        res.clear();
        res.resize(n, 0);
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < nthread; j++) {
                res[i] += tmp[j][i];
            }
        }
        return 0;
    }
};

double abs(double v) {
    return (v > 0) ? v : -v;
}

int main(int argc, char **argv) {
    FILE *pf = fopen("mat.txt", "r");
    int n;
    nthread = omp_get_num_procs();
    fscanf(pf, "%d", &n);
    if (argc > 1) {
        nthread = atoi(argv[1]);
    }
    printf("%d\n", nthread);
    smatrix mat(n);
    std::vector<double> b(n);
    int cnt = 0;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            double v;
            fscanf(pf, "%lf", &v);
            if (abs(v) > eps) {
                mat.ins(i, j, v);
                cnt++;
            }
        }
    }
    for (int i = 0; i < n; i++) {
        fscanf(pf, "%lf", &b[i]);
    }
    printf("cnt: %d\n", cnt);
    
    std::vector<std::vector<double>> x(2, std::vector<double>(n, 1)), r(2, std::vector<double>(n)), p(2, std::vector<double>(n));

    auto st = std::chrono::system_clock::now();
    mat.rmul(r[0], x[0]);
    for (int i = 0; i < n; i++) {
        r[0][i] = b[i] - r[0][i];
        p[0][i] = r[0][i];
    }

    int niter = 0;
    for (;;) {
        niter++;
        std::vector<double> tmp(n);
        double a = 0;
        for (int i = 0; i < n; i++) {
            a += r[0][i] * r[0][i];
        }
        double ta = 0;
        mat.lmul(tmp, p[0]);
        for (int i = 0; i < n; i++) {
            ta += tmp[i] * p[0][i];
        }
        a = a / ta;

        mat.rmul(tmp, p[0]);

        for (int i = 0; i < n; i++) {
            x[1][i] = x[0][i] + a * p[0][i];
            r[1][i] = r[0][i] - a * tmp[i];
        }

        ta = 0;
        for (int i = 0; i < n; i++) {
            ta += r[1][i] * r[1][i];
        }

        if (ta < thresh) {
            break;
        }

        double b = 0;
        for (int i = 0; i < n; i++) {
            b += r[0][i] * r[0][i];
        }
        b = ta / b;
        for (int i = 0; i < n; i++) {
            p[1][i] = r[1][i] + b * p[0][i];
        }
        x[0] = x[1];
        r[0] = r[1];
        p[0] = p[1];
    }
    auto ed = std::chrono::system_clock::now();
    printf("time: %.6lfs\n", 1e-6*(uint64_t)std::chrono::duration_cast<std::chrono::microseconds>(ed - st).count());
    printf("Iter: %d\n", niter);
    // for (int i = 0; i < n; i++) printf("%.2lf ", x[1][i]); printf("\n");

    return 0;
}