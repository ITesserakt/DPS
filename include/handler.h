#pragma once

#include "lift.h"

/// Receives a passenger lift as a parameter [p] and updates it in infinite loop
/// Returns [NULL]
void *update_pass_lift(void *p);

/// Receives a cargo lift as a parameter [p] and updates it in infinite loop
/// Returns [NULL]
void *update_cargo_lift(void *p);

/// Receives an array of lifts of size 2 and displays it in infinite loop
/// Returns [NULL]
void *display_lifts(void *p);

/// Saves pointer to [mutex] for internal purposes
void init_handlers_system(pthread_mutex_t *mutex);