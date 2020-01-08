#include <cstdio>
#include <cstdint>
#include <cstdlib>
#include <omp.h>

#include <chrono>

const float CENTERX = -0.5, CENTERY = 0.5, HEIGHT = 0.5, WIDTH=0.5;
const uint32_t MAX_ITER = 100, M = 512, N = 512;
uint8_t image[N][M];

class complex {
public:
    float u, v;

    complex () {
        u = 0;
        v = 0;
    }

    complex (float uu, float vv) {
        u = uu;
        v = vv;
    }

    ~complex() {}

    complex operator + (complex &x) {
        float uy = u + x.u;
        float vy = v + x.v;
        return complex(uy, vy);
    }

    complex operator * (complex &x) {
        float uy = u * x.u - v * x.v;
        float vy = u * x.v + v * x.u;
        return complex(uy, vy);
    }

    float mag2() {
        return u * u + v * v;
    }
};

int main(int argc, char *argv[]) {
    float x, y, v;
    uint32_t max_iter = MAX_ITER, nthread = omp_get_num_procs();

    float cx = CENTERX, cy = CENTERY, w = WIDTH, h = HEIGHT;
    if (argc > 1) {
        cx = atof(argv[1]);
    }
    if (argc > 2) {
        cy = atof(argv[2]);
    }
    if (argc > 3) {
        w = h = atof(argv[3]);
    }
    if (argc > 4) {
        max_iter = atoi(argv[4]);
    }
    if (argc > 5) {
        nthread = atoi(argv[5]);
    }

    auto st = std::chrono::system_clock::now();
#pragma omp parallel for num_threads(nthread)
    for (int i = 0; i < N * M; i++)
    {
        float x = ((i / M) + 0.5) * w / N + cx - w / 2;
        float y = ((i % M) + 0.5) * h / M + cy - h / 2;
        complex c, c0(x, y);
        float v = 0;

        for (int k = 0; k < max_iter; k++) {
            c = c * c + c0;
            if (c.mag2() > 4.0) break;
        }
        v = c.mag2();
        if (v > 1.0) {
            v = 1.0;
        }
        image[i / M][i % M] = 255 * v;
    }
    auto ed = std::chrono::system_clock::now();
    printf("time: %.6lfs\n", 1e-6*(uint64_t)std::chrono::duration_cast<std::chrono::microseconds>(ed - st).count());
    return 0;
}
