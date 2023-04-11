#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "handler.h"
#include "utils.h"

/// Reads a string char-by-char to [buffer] from [stdin] and simultaneously outputs its contents.
/// Saves string's length to [size] pointer
static int read_line(char *buffer, size_t *size) {
    char c;
    size_t index = 0;

    while (true) {
        read(STDIN_FILENO, &c, 1);
        if (c == '\n')
            break;

        buffer[index++] = c;
        write(STDOUT_FILENO, &c, 1);
    }
    buffer[index] = '\0';
    *size = index;
    WRITE(STDOUT_FILENO, " -> ");

    return 0;
}

int invert_case() {
    WRITE(STDOUT_FILENO, "1) Invert letters' case: ");
    static char line[BUFSIZ];
    size_t size;

    if (read_line(line, &size) != 0)
        return 0;

    for (size_t i = 0; i < size; i++) {
        char c = line[i];

        if ('a' <= c && c <= 'z') {
            c = c - 'a' + 'A';
        } else if ('A' <= c && c <= 'Z') {
            c = c - 'A' + 'a';
        }

        write(STDOUT_FILENO, &c, 1);
    }

    return 0;
}

int reverse_str() {
    WRITE(STDOUT_FILENO, "2) Reverse string: ");
    static char line[BUFSIZ];
    size_t size;

    if (read_line(line, &size) != 0)
        return 0;

    for (size_t i = 0; i < size; i++)
        write(STDOUT_FILENO, &line[size - i - 1], 1);

    return 0;
}

int pairwise_swap() {
    WRITE(STDOUT_FILENO, "3) Swap pairs: ");
    static char line[BUFSIZ];
    size_t size;

    if (read_line(line, &size) != 0)
        return 0;

    for (size_t i = 0; i < size - 1; i += 2) {
        char pair[] = {line[i + 1], line[i]};
        write(STDOUT_FILENO, pair, 2);
    }

    if (size % 2 == 1)
        write(STDOUT_FILENO, &line[size - 1], 1);

    return 0;
}

int convert_koi8() {
    WRITE(STDOUT_FILENO, "4) Convert to KOI-8: ");
    static char line[BUFSIZ];
    size_t size;

    if (read_line(line, &size) != 0)
        return 0;

    for (size_t i = 0; i < size; i++) {
        char c = line[i];
        if (('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z'))
            c |= 1 << 7;
        write(STDOUT_FILENO, &c, 1);
    }

    return 0;
}
