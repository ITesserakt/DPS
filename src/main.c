#include <fcntl.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>

#include "handler.h"
#include "utils.h"

typedef int (*Handler)(void);

static pid_t children[4];
static struct termios save;

/// Kills all children processes, restores session state and exits
void graceful_shutdown(int _sgn_code) {
    for (unsigned int i = 0; i < ARRAY_LEN(children); i++) {
        if (kill(children[i], SIGTERM) != 0)
            exit(-1);
    }

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &save);
    exit(0);
}

/// Used to accept [SIGUSR1] signal to unpause process
void accept_awake(int _sgn_code) {}

/// Used in function exit code
typedef enum {
    ForkFailed = -1,
    StdinReadFailed = -2,
    SignalFailed = -3,
    WrongStdin = -4,
    TerminalProps = -5,

    Ok = 0,
} Error;

/// Creates new child process via [fork], registers [SIGUSR1] signal handler and calls [f] in infinite loop.
/// If any error happened in that loop, then the process exits with [-1] exit code
/// ERRORS: [ForkFailed] if failed to create child process
///			[SignalFailed] if parent process cannot accept [SIGUSR1] signal invokation
Error fork_process(Handler f, pid_t *pid) {
    pid_t parent_pid = getpid();
    *pid = fork();
    if (*pid == -1) {
        perror("Cannot fork process");
        return ForkFailed;
    }
    if (*pid != 0)
        return Ok;

    signal(SIGUSR1, accept_awake);

    while (true) {
        pause();
        TRY(f());
        TRY(kill(parent_pid, SIGUSR1), SignalFailed);
        WRITE(STDOUT_FILENO, "\n");
    }

    exit(-1);
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

/// Program's entry point. Opens 4 processes for handling user input, which awakens in infinite loop here
/// ERRORS: [StdinReadFailed] if failed to read next control char 
///         [SignalFailed] if failed to send a signal to one of a children
int main() {
    TRY(disable_canon_input(&save));

    TRY(fork_process(invert_case, &children[0]));
    TRY(fork_process(reverse_str, &children[1]));
    TRY(fork_process(pairwise_swap, &children[2]));
    TRY(fork_process(convert_koi8, &children[3]));

    signal(SIGUSR1, accept_awake);
    signal(SIGINT, graceful_shutdown);

    WRITE(STDOUT_FILENO, "Enter a value between [0; 3] to choose operation\n");
    while (true) {
        unsigned char next;
        int code = read(STDIN_FILENO, &next, 1);
		if (code < 0)
            return StdinReadFailed;

        if ('0' <= next && next <= '3') {
            TRY(kill(children[next - '0'], SIGUSR1), SignalFailed);
            pause();
        }
    }
}
