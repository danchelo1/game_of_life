#include <ncurses.h>
#include <stdio.h>

#define WIDTH 80
#define HEIGHT 25
#define MIN_DELAY 10000
#define MAX_DELAY 500000

void init_curses(void);
void finish_curses(void);
int read_initial_field(int field[HEIGHT][WIDTH]);
void draw_field(const int field[HEIGHT][WIDTH]);
int count_neighbors(int field[HEIGHT][WIDTH], int y, int x);
void compute_next(int current[HEIGHT][WIDTH], int next[HEIGHT][WIDTH]);
void copy_field(int current[HEIGHT][WIDTH], int next[HEIGHT][WIDTH]);
int process_input(int *delay);
void game_loop(int field[HEIGHT][WIDTH]);

void init_curses(void) {
    initscr();
    noecho();
    curs_set(FALSE);
    nodelay(stdscr, TRUE);
}

void finish_curses(void) { endwin(); }

int read_initial_field(int field[HEIGHT][WIDTH]) {
    int ch = 0;
    int x = 0;
    int y = 0;
    for (y = 0; y < HEIGHT && ch != EOF; y++) {
        for (x = 0; x < WIDTH && (ch = getchar()) != EOF;) {
            if (ch == '0' || ch == '1' || ch == '*') {
                field[y][x] = (ch == '1' || ch == '*');
                x++;
            }
        }
    }
    return (ch == EOF && y < HEIGHT) ? 1 : 0;
}

void draw_field(const int field[HEIGHT][WIDTH]) {
    clear();

    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            if (field[y][x] == 1) {
                mvaddch(y, x, '#');
            } else {
                mvaddch(y, x, ' ');
            }
        }
    }

    refresh();
}

int count_neighbors(int field[HEIGHT][WIDTH], int y, int x) {
    int count = 0;

    for (int dy = -1; dy <= 1; dy++) {
        for (int dx = -1; dx <= 1; dx++) {
            int ny = (y + dy + HEIGHT) % HEIGHT;
            int nx = (x + dx + WIDTH) % WIDTH;

            if (!(dy == 0 && dx == 0)) {
                count += field[ny][nx];
            }
        }
    }

    return count;
}

void compute_next(int current[HEIGHT][WIDTH], int next[HEIGHT][WIDTH]) {
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            int neighbors = count_neighbors(current, y, x);

            if (current[y][x] == 1) {
                next[y][x] = (neighbors == 2 || neighbors == 3);
            } else {
                next[y][x] = (neighbors == 3);
            }
        }
    }
}

void copy_field(int current[HEIGHT][WIDTH], int next[HEIGHT][WIDTH]) {
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            current[y][x] = next[y][x];
        }
    }
}

int process_input(int *delay) {
    int quit = 0;
    int ch = getch();

    if (ch == 'a' || ch == 'A') {
        if (*delay > MIN_DELAY) *delay -= 10000;
    }

    if (ch == 'z' || ch == 'Z') {
        if (*delay < MAX_DELAY) *delay += 10000;
    }

    if (ch == ' ') {
        quit = 1;
    }

    return quit;
}

void game_loop(int field[HEIGHT][WIDTH]) {
    int next[HEIGHT][WIDTH];
    int delay = 100000;
    int quit = 0;

    while (!quit) {
        draw_field(field);
        compute_next(field, next);
        copy_field(field, next);
        quit = process_input(&delay);
        napms(delay / 1000);
    }
}

int main(void) {
    int field[HEIGHT][WIDTH];
    int status = read_initial_field(field);
    int exit = 0;
    if (freopen("/dev/tty", "r", stdin) == NULL) {
        exit = 1;
    } else if (status == 0) {
        init_curses();
        game_loop(field);
        finish_curses();
        exit = 0;
    } else {
        exit = status;
    }

    return exit;
}