#include "config.h"

Config readConfig(int argc, char **argv) {
    Config empty = {.width = 100,
                    .height = 100,
                    .a = 20,
                    .external_weight = 0.1,
                    .time_layers = 3000,
                    .generateImage = true,
                    .step = 0.5};
    return empty;
}