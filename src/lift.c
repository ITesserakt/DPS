#include "lift.h"

/// Gets a distance between current floor of lift and specified floor [b] with respect of lift's state
static unsigned int dist(Lift a, Floor b) {
    switch (a.state) {
    case Idle:
        if (a.current_floor > b)
            return a.current_floor - b;
        return b - a.current_floor;
    case Downward:
        if (a.current_floor < b)
            return a.current_floor + b;
        return a.current_floor - b;
    case Upward:
        if (a.current_floor > b)
            return 2 * MAX_FLOOR - a.current_floor - b - 1;
        return b - a.current_floor;
    case Disabled:
        return -1;
    }
}

/// Compares two unsigned int values and returns [Ordering]
static Ordering compare_uint(unsigned int a, unsigned int b) {
    if (a < b)
        return Less;
    if (a == b)
        return Equal;
    return Greater;
}

Ordering compare(Lift a, Lift b, Floor floor) { return compare_uint(dist(a, floor), dist(b, floor)); }

void call_lift(Lift *l, Floor floor) {
    if (l->state == Idle) {
        if (l->current_floor > floor)
            l->state = Downward;
        else if (l->current_floor < floor)
            l->state = Upward;
    }
}

bool is_busy(Lift l) { return l.state != Idle && l.state != Disabled; }

void init_lift(Lift *l) {
    l->current_floor = 1;
    l->load = 0;
    l->state = Idle;
    l->target_floor = 1;
    l->load_change = 0;
}

Lift *get_nearest_free(Lift *lifts, size_t size, Floor floor) {
    if (size == 0)
        return NULL;

    Lift *current = lifts;
    for (size_t i = 1; i < size; i++) {
        if (is_busy(lifts[i]))
            continue;

        Ordering cmp = compare(*current, lifts[i], floor);

        if (cmp == Greater || is_busy(*current))
            current = &lifts[i];
    }

    return is_busy(*current) ? NULL : current;
}