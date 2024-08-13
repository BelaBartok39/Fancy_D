#ifndef COLOR_UTILS_H
#define COLOR_UTILS_H

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

#define print_color(color, fmt, ...) \
    printf(color fmt ANSI_COLOR_RESET, ##__VA_ARGS__)

#define print_red(fmt, ...) print_color(ANSI_COLOR_RED, fmt, ##__VA_ARGS__)
#define print_green(fmt, ...) print_color(ANSI_COLOR_GREEN, fmt, ##__VA_ARGS__)
#define print_yellow(fmt, ...) print_color(ANSI_COLOR_YELLOW, fmt, ##__VA_ARGS__)
#define print_blue(fmt, ...) print_color(ANSI_COLOR_BLUE, fmt, ##__VA_ARGS__)
#define print_magenta(fmt, ...) print_color(ANSI_COLOR_MAGENTA, fmt, ##__VA_ARGS__)
#define print_cyan(fmt, ...) print_color(ANSI_COLOR_CYAN, fmt, ##__VA_ARGS__)

#endif // COLOR_UTILS_H
