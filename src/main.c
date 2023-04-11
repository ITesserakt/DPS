#include <pthread.h>
#include <signal.h>
#include <termios.h>
#include <unistd.h>

#include "handler.h"
#include "lift.h"
#include "utils.h"

/// Used in function exit code
typedef enum {
    ThreadCreateFailed = -1,
    TerminalProps = -2,
    MutexCreateFailed = -3,
    WrongStdin = -4,

    Ok = 0,
} Error;

typedef void *(*Handler)(void *);

struct termios save;
pthread_t threads[3];
Lift lifts[2];

/// Disables lifts and joins threads while they're exiting
/// Also reenables canon input
void graceful_shutdown(int _sig) {
    lifts[0].state = lifts[1].state = Disabled;

    pthread_join(threads[0], NULL);
    pthread_join(threads[1], NULL);
    pthread_join(threads[2], NULL);

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &save);
    printf("\n");

    exit(0);
}

/// Checks if current session is in terminal, saves it state and sets non-canon type of input
/// ERRORS: [WrongStdin] if current session is not in terminal
///         [TerminalProps] if failed to save or to set terminal properties
Error disable_canon_input(struct termios *save) {
    if (!isatty(STDIN_FILENO)) {
        perror("stdin isn't a terminal");
        return WrongStdin;
    }

    struct termios tty;

    TRY(tcgetattr(STDIN_FILENO, save), TerminalProps);
    tty = *save;
    tty.c_lflag &= ~(ICANON | ECHO);
    tty.c_cc[VMIN] = 1;
    TRY(tcsetattr(STDIN_FILENO, TCSAFLUSH, &tty), TerminalProps);

    return Ok;
}

/// Runs a new thread with default attributes calling [handler] with [arg]
/// ERRORS: [ThreadCreateFailed] - if failed to create a thread with default attrs
Error open_thread(pthread_t *thread, Handler handler, void *arg) {
    pthread_attr_t attr;
    TRY(pthread_attr_init(&attr), ThreadCreateFailed);
    TRY(pthread_create(thread, &attr, handler, arg), ThreadCreateFailed);

    return Ok;
}

/// Initialises [mutex] with default attributes
/// ERRORS: [MutexCreateFailed] - if failed to create a mutex with default attrs
Error get_fresh_mutex(pthread_mutex_t *mutex) {
    pthread_mutexattr_t attr;
    TRY(pthread_mutexattr_init(&attr));
    TRY(pthread_mutex_init(mutex, &attr));

    return Ok;
}

/// Searches for [ch] in array of chars [alphabet] with size [size]
/// Returns index of the first char of [alphabet] when it's equals to [ch], otherwise returns -1
size_t find_index(const char alphabet[], size_t size, char ch) {
    for (size_t i = 0; i < size; i++)
        if (alphabet[i] == ch)
            return i;
    return -1;
}

/// Reacts on user's input by moving lifts to certain floors
/// In particular, if input in 0..9, moves the nearest lift to this floor
/// if input in [lp_control], moves the first lift
/// if input in [lc_control], moves the second lift
void manage_lifts(unsigned char c, pthread_mutex_t *busy_mutex) {
    static const char floor_control[] = "1234567890";
    static const char lp_control[] = "qwertyuiop";
    static const char lc_control[] = "asdfghjkl;";
    size_t index;

    if ((index = find_index(floor_control, sizeof(floor_control), c)) != -1) {
        Lift *nearest = get_nearest_free(lifts, 2, index + 1);
        if (!nearest)
            return;

        nearest->target_floor = index + 1;
        nearest->load_change = 1;
    } else if ((index = find_index(lp_control, sizeof(lp_control), c)) != -1 && lifts[0].load > 0) {
        lifts[0].target_floor = index + 1;
        lifts[0].load_change = -1;
    } else if ((index = find_index(lc_control, sizeof(lc_control), c)) != -1 && lifts[1].load > 0) {
        lifts[1].target_floor = index + 1;
        lifts[1].load_change = -1;
    }

    return;
}

/// Program's entry point. Initializes lifts, mutex and displaying system, opens 3 threads for handling user input and
/// reads user input in infinite loop
int main() {
    TRY(disable_canon_input(&save));

    pthread_mutex_t busy_mutex;
    TRY(get_fresh_mutex(&busy_mutex));

    init_lift(&lifts[0]);
    init_lift(&lifts[1]);
    init_handlers_system(&busy_mutex);

    TRY(open_thread(&threads[0], update_pass_lift, &lifts[0]));
    TRY(open_thread(&threads[1], update_cargo_lift, &lifts[1]));
    TRY(open_thread(&threads[2], display_lifts, lifts));

    signal(SIGINT, graceful_shutdown);

    while (true) {
        unsigned char next;
        read(STDIN_FILENO, &next, 1);
        size_t index;

        pthread_mutex_lock(&busy_mutex);

        manage_lifts(next, &busy_mutex);

        pthread_mutex_unlock(&busy_mutex);
    }
}