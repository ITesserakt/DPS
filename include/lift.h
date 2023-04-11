#pragma once

#include <pthread.h>
#include <stdbool.h>
#include <stdlib.h>

#include "compare.h"

#define MAX_FLOOR 10

typedef unsigned int Floor;

/// Idle - lift waits for commands - default state
/// Disabled - lift does not handle any commands
/// Upward - lift goes up
/// Downward - lift goes down
typedef enum { Downward, Upward, Idle, Disabled } LiftState;

typedef struct {
    Floor current_floor;
    Floor target_floor;
    /// Change of load when lift becomes idle again
    int load_change;
    unsigned int load;
    LiftState state;
} Lift;

/// Fills fields of [l] with default values
void init_lift(Lift *l);

/// Compares distances between two lifts against the [floor] and returns [Ordering]
Ordering compare(Lift a, Lift b, Floor floor);

/// Changes direction of the lift [l] when it's idle with respect to [floor]
void call_lift(Lift *l, Floor floor);

/// Checks if lift [l] is enabled and is not in idle state
bool is_busy(Lift l);

/// Returns nearest lift to [floor] from [lifts] using [compare] function, filtering all busy lifts
/// If all lifts are busy returns [NULL]
Lift *get_nearest_free(Lift *lifts, size_t size, Floor floor);