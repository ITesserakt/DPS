#include "config.h"

Config readConfig(int argc, char **argv) {
    Config empty = {.width = 500,
                    .height = 500,
                    .a = 1,
                    .external_weight = 1,
                    .time_layers = 100,
                    .generateImage = true,
                    .step = 1};
    return empty;
}