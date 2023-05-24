#pragma once

typedef struct {
    int width, height;
    double *buffer;
} Matrix;

Matrix newMatrix(int x, int y);

double *get(Matrix m, int x, int y);

double *buffer(Matrix m);

heatmap_t *intoHeatmap(Matrix m);

void freeMatrix(Matrix m);