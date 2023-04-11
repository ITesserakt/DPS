#include "handler.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static const __useconds_t BASE_DELAY = 500 * 1000;

static pthread_mutex_t *mutex;

void init_handlers_system(pthread_mutex_t *mutex_) {
    mutex = mutex_;
}

/// Moves lift upwards or downwards or applies load change in idle state
static void do_update(Lift *lift) {
    pthread_mutex_lock(mutex);

    if (lift->current_floor == lift->target_floor) {
        lift->state = Idle;
        lift->load += lift->load_change;
        lift->load_change = 0;
    }
    call_lift(lift, lift->target_floor);

    if (lift->state == Downward)
        lift->current_floor--;
    if (lift->state == Upward)
        lift->current_floor++;

    pthread_mutex_unlock(mutex);
}

/// Displays current lift position to [stdout] with [prefix]
static void show(const Lift *l, const char *prefix) {
    pthread_mutex_lock(mutex);

    printf("%s:", prefix);

    for (Floor i = 1; i <= MAX_FLOOR; i++) {
        if (l->current_floor == i)
            printf(" %u ", l->load);
        else
            printf(" - ");
    }

    pthread_mutex_unlock(mutex);
}

void *update_pass_lift(void *p) {
    Lift *lift = p;
    while (lift->state != Disabled) {
        do_update(lift);

        usleep(BASE_DELAY);
    }

    return NULL;
}

void *update_cargo_lift(void *p) {
    Lift *lift = p;
    while (lift->state != Disabled) {
        do_update(lift);

        usleep(2 * BASE_DELAY);
    }

    return NULL;
}

/// Returns string representation of lift's state
static const char *display_state(LiftState state) {
    if (state == Disabled)
        return "dsbd";
    else if (state == Idle)
        return "idle";
    else if (state == Downward)
        return "down";

    return "upwd";
}

void *display_lifts(void *p) {
    Lift *lifts = p;

    while (lifts[0].state != Disabled || lifts[1].state != Disabled) {
        printf("\r");

        show(&lifts[0], "LP");
        printf(" |%s|\t", display_state(lifts[0].state));
        show(&lifts[1], "LC");
        printf(" |%s|", display_state(lifts[1].state));

        fflush(stdout);

        usleep(BASE_DELAY / 2);
    }

    return NULL;
}