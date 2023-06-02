#include "config.h"

Config readConfig(int argc, char **argv) {
    Config empty = {.width = 100,
                    .height = 100,
                    .a = 1,
                    .external_weight = 1,
                    .time_layers = 1000,
                    .generateImage = true,
                    .step = 1};
    return empty;
}