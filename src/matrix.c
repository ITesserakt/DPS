#include <heatmap.h>
#include <math.h>
#include <stdlib.h>

#include "matrix.h"

Matrix newMatrix(int x, int y) {
    Matrix m;
    m.width = y;
    m.height = x;
    m.buffer = calloc(x * y, sizeof(double));
    return m;
}

double *get(Matrix m, int x, int y) { return &m.buffer[x * m.width + y]; }

double *buffer(Matrix m) { return m.buffer; }

heatmap_t *intoHeatmap(Matrix m) {
    heatmap_t *frame = heatmap_new(m.width, m.height);

    for (int i = 0; i < m.width; i++)
        for (int j = 0; j < m.height; j++)
            heatmap_add_weighted_point(frame, i, j, fmax(*get(m, i, j), 0));

    return frame;
}

void freeMatrix(Matrix m) { free(m.buffer); }