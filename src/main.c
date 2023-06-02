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
        if (2 < t && t < 50)
            return config.external_weight * exp(sin(t));
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

    *get(current, x, y) = (-dt * dt * dzxy + Ap + An) / 2;
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
                *get(current, x, y) = 0;
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
        for (int i = 0; i < time_steps; i++) {
            unsigned char *data = heatmap_render_default_to(frames[i], NULL);
            GifWriteFrame(&w, data, width, height, 10, 8, false);
            heatmap_free(frames[i]);
        }
        GifEnd(&w);
    }

    freeMatrix(previous);
    freeMatrix(current);
    freeMatrix(next);

    MPI_Finalize();

    // const double num = (-(width / height + 1) +
    //                     sqrt(pow(width / height + 1, 2) -
    //                          4 * (width / height) * (1 - num_points))) /
    //                    (2 * width / height);

    // const int num_intervals = (int)num;

    // const int N = num_intervals + 1; // vertical dimention
    // const int M =
    //     num_intervals * (width / height) + 1; //// horizontal dimention
    // const double dx = height / num_intervals;
    // const double dy = dx;
    // const double dt = dx * dx / (2 * a);

    // int num_threads, myrank;
    // MPI_Init(&argc, &argv);
    // MPI_Comm_size(MPI_COMM_WORLD, &num_threads);
    // MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
    // double *curr_iterG = NULL;
    // if (!myrank) {
    //     if (/*num_intervals != num || */ (
    //             (int)(num_intervals * width / height) + 1) %
    //             num_threads !=
    //         0) {
    //         printf("LOL\n");
    //         exit(1);
    //     }
    // }
    // curr_iterG = (double *)calloc(num_points, sizeof(double));

    // // Matrix current = newMatrix(num_points);
    // // Matrix next = newMatrix(num_points);
    // // Matrix previous = newMatrix(num_points);

    // double *next_iter = NULL;
    // double *prev_iter = NULL;
    // double *curr_iter = NULL;
    // next_iter = (double *)calloc(num_points, sizeof(double));
    // curr_iter = (double *)calloc(num_points, sizeof(double));
    // prev_iter = (double *)calloc(num_points, sizeof(double));

    // const int offset = (num_intervals * width / height + 1) / num_threads;
    // Record record;
    // record.first = offset * myrank; // Индекс нач. строки А для потока
    // record.last =
    //     (offset * (myrank + 1)) - 1; // Индекс конечн. строки А для потока
    // FILE *input_file = NULL;
    // if (!myrank) {
    //     input_file = fopen("input.txt", "w");
    // }
    // fprintf(stderr, "-- %d %d %d %d\n", record.first, record.last, N,
    // offset);

    // double t = 0;
    // for (int i = 0; i < time_steps; i++) {
    //     for (int j = record.first * N; j != (record.last + 1) * N; j++) {

    //         double z_11, z_21, z_01, z_12, z_10, z_11_0, f;
    //         if (j < N || j % N == 0 || j % N == N - 1 || j > N * (M - 1)) {
    //             next_iter[j] = 0; // граничное условие первого рода
    //         } else {
    //             z_11 = curr_iter[j];         // z_i_j   t = 0
    //             z_21 = curr_iter[j + 1];     // z_i+1_j   t = 0
    //             z_01 = curr_iter[j - 1];     // z_i-1_j   t = 0
    //             z_12 = curr_iter[j + N + 1]; // z_i+1_j   t = 0
    //             z_10 = curr_iter[j - N - 1]; // z_i-1_j   t = 0
    //             z_11_0 = prev_iter[j];       // z_i_j   t = -1
    //             if (t == dt && j == 205) {

    //                 f = 100 * 11 * 5 *
    //                     5 /*0.1*100 / dt * sin((i % N) * (i / N) * t)*/
    //                     ; // f(x,y,t)
    //             } else
    //                 f = 0;
    //             next_iter[j] = (a * a * dt * dt / (dx * dx)) *
    //                                (z_21 + z_01 + z_12 + z_10 - 4 * z_11) +
    //                            dt * dt * f - z_11_0 + 2 * z_11;
    //         }
    //     }
    //     // ВЫЧИСЛЕНИЯ КОНЕЦ

    //     memcpy(prev_iter, curr_iter, num_points * sizeof(double));
    //     memcpy(curr_iter, next_iter, num_points * sizeof(double));
    // if (myrank != 0) {
    //     // MPI_Sendrecv(curr_iter[(i - 1) * m], 1, MPI_DOUBLE, myrank -
    //     1,
    //     // 0,
    //     //              &left, 1, MPI_DOUBLE, myrank - 1, MPI_ANY_TAG,
    //     //              MPI_COMM_WORLD, &s1);
    // } // сосед слева
    // if (myrank != num_threads - 1) {
    //     // MPI_Sendrecv(curr_iter[(i - 1) * m + (m - 1)], 1, MPI_DOUBLE,
    //     //              myrank + 1, 1, &right, 1, MPI_DOUBLE, myrank + 1,
    //     //              MPI_ANY_TAG, MPI_COMM_WORLD, &s2);
    // } // сосед справа
    //     memcpy(curr_iter, curr_iterG, num_points * sizeof(double));
    //     t += dt;

    //     if (!myrank) {
    //         MPI_Gather((void *)(curr_iter + (record.first * N)), (offset)*N,
    //                    MPI_DOUBLE, (void *)(curr_iterG), (offset)*N,
    //                    MPI_DOUBLE, 0, MPI_COMM_WORLD);

    //         if (i % 10 == 0 || i == time_steps - 1) {
    //             num_time++;
    //             int k_1 = 0;

    //             for (int j = 0; j < num_points; j++) {
    //                 int k = j % N;

    //                 fprintf(input_file, "%lf ", curr_iter[k * N + k_1]);

    //                 if (k == N - 1) {
    //                     fprintf(input_file, "\n");
    //                     k_1++;
    //                 }
    //             }
    //             fprintf(input_file, "\n\n");
    //         }
    //     }
    // }
    // if (!myrank) {
    //     visualize(input_file);
    // }
    // free(prev_iter);
    // free(curr_iter);
    // free(next_iter);
    // free(curr_iterG);

    // MPI_Finalize();
}

// void visualize(FILE *input_file) {
//     int i;
//     FILE *gnu = popen("gnuplot -persist", "w");
//     fprintf(gnu, "set zrange [-1:1]\n");

//     for (i = 1; i < num_time; i++) {
//         fprintf(gnu, "splot \"input.txt\" index %d matrix w l\n", i);
//         fprintf(gnu, "pause 0.25\n");
//     }
//     fflush(gnu);
// }
