#pragma once

#include <stdbool.h>

typedef struct {
    bool generateImage;
    double height;
    double width;
    double time_layers;
    double external_weight;
    double a;
    double step;
} Config;

Config readConfig(int argc, char **argv);