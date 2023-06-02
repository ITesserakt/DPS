#pragma once

#include <stdbool.h>

typedef struct {
    bool generateImage;
    int height;
    int width;
    double time_layers;
    double external_weight;
    double a;
    double step;
} Config;

Config readConfig(int argc, char **argv);