#include <math.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

#include <mpi.h>
#include <pthread.h>
#include <sched.h>
#include <string.h>

#include <gif.h>
#include <heatmap.h>
#include <lodepng.h>

#include "config.h"
#include "matrix.h"

Config config;

typedef struct {
    int size;
    int start;
    int end;
    int position;
    int x;
    int y;
    double *top;
    double *down
} Stripes;

double external(double x, double y, double t) {
    if (x == config.width / 2 && y == config.height / 2) {
        return config.external_weight * sin(x * y + t);
    }
    return 0;
}

double *getS(Stripes s, Matrix m, int x, int y) {
    int newPos = x * config.width + y;
    if (newPos < s.start)
        return &s.top[s.size - newPos + s.position];
    if (newPos > s.end)
        return &s.down[newPos - s.size];
    return get(m, x, y);
}

void explicit_solver(Stripes stripes, double t, Matrix current, Matrix prev,
                     Matrix next) {
    int x = stripes.x, y = stripes.y;
    double dx = 1;
    double dy = 1;
    double dt = dx * dy / 2 / config.a;
    double weight = external(stripes.x, stripes.y, t);

    /**
     *      C
     *      |
     * B -- A -- D
     *      |
     *      E
     */

    double A = *get(current, x, y), B = *getS(stripes, current, x - 1, y);
    double C = *getS(stripes, current, x, y + 1),
           D = *getS(stripes, current, x + 1, y);
    double E = *getS(stripes, current, x, y - 1);

    double dzx = (D - 2 * A + B) / dx / dx;
    double dzy = (E - 2 * A + C) / dy / dy;

    double dzxy = config.a * config.a * (dzx + dzy) + weight;
    double Ap = *get(prev, x, y);
    double An = *get(next, x, y);

    *get(next, x, y) = (-dt * dt * dzxy + Ap + A) / 2;
}

int main(int argc, char *argv[]) {
    int threads, currentRank;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &threads);
    MPI_Comm_rank(MPI_COMM_WORLD, &currentRank);

    config = readConfig(argc, argv);

    const int height = config.height;
    const int width = config.width;
    const int num_points = height * width;
    const int time_steps = config.time_layers;
    const double a = config.a;

    Matrix previous = newMatrix(width, height);
    Matrix current = newMatrix(width, height);
    Matrix next = newMatrix(width, height);

    Matrix global = newMatrix(width, height);

    heatmap_t *frames[time_steps];
    float last_max = 0;

    int offset = num_points / threads;
    int start = offset * currentRank;
    int end = offset * (currentRank + 1);
    int size = offset;

    double topStripe[size];
    double downStripe[size];

    for (int t = 0; t < time_steps; t++) {
        for (int i = start; i < end; i++) {
            int x = i % width;
            int y = i / width;

            if (x == 0 || y == 0 || x == width - 1 || y == height - 1)
                *get(next, x, y) = 1;
            else {
                Stripes s = {.x = x,
                             .y = y,
                             .start = start,
                             .end = end,
                             .position = i,
                             .top = topStripe,
                             .down = downStripe};
                explicit_solver(s, t, current, previous, next);
            }
        }

        memmove(buffer(previous), buffer(current), num_points * sizeof(double));
        memmove(buffer(current), buffer(next), num_points * sizeof(double));

        if (currentRank != 0)
            MPI_Sendrecv(buffer(current) + start - size, size, MPI_DOUBLE,
                         currentRank - 1, 0, downStripe, size, MPI_DOUBLE,
                         currentRank - 1, MPI_ANY_TAG, MPI_COMM_WORLD,
                         MPI_STATUS_IGNORE);

        if (currentRank != threads - 1)
            MPI_Sendrecv(buffer(current) + end, size, MPI_DOUBLE,
                         currentRank + 1, 0, topStripe, size, MPI_DOUBLE,
                         currentRank + 1, MPI_ANY_TAG, MPI_COMM_WORLD,
                         MPI_STATUS_IGNORE);

        if (currentRank != 0)
            MPI_Gather(buffer(previous) + start, size, MPI_DOUBLE,
                       buffer(global) + start, size, MPI_DOUBLE, 0,
                       MPI_COMM_WORLD);
        else
            memcpy(buffer(global) + start, buffer(previous) + start,
                   size * sizeof(double));

        if (config.generateImage && currentRank == 0) {
            frames[t] = intoHeatmap(global);
            frames[t]->max = last_max;
            if (last_max < frames[t]->max)
                last_max = frames[t]->max;
        }
    }

    MPI_Barrier(MPI_COMM_WORLD);

    if (config.generateImage && currentRank == 0) {
        GifWriter w;
        GifBegin(&w, "output.gif", width, height, 10, 8, false);
        for (int i = 0; i < time_steps - 1; i++) {
            unsigned char *data = heatmap_render_default_to(frames[i], NULL);
            GifWriteFrame(&w, data, width, height, 10, 8, false);
            heatmap_free(frames[i]);
        }
        GifEnd(&w);

        unsigned char *data = heatmap_render_default_to(frames[time_steps - 1], NULL);
        lodepng_encode32_file("output.png", data, width, height);
    }

    freeMatrix(previous);
    freeMatrix(current);
    freeMatrix(next);

    MPI_Finalize();
}
