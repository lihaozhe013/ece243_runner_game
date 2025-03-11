#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <time.h>
#include <stdbool.h>
// Global Variables
#define player_pos_y 40
#define SCREEN_HEIGHT 240
#define SCREEN_WIDTH 320
#define PLYAER_X_OFFSET 10 
#define PLAYER_Y_OFFSET 30
int pixel_buffer_start;
bool reset = false;
bool in_start_page = false;
bool start = false;
char draw_screen[SCREEN_WIDTH][SCREEN_HEIGHT] = {' '};

void setup_terminal() {
    struct termios t;
    tcgetattr(STDIN_FILENO, &t);
    t.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &t);
}

int kbhit() {
    struct termios oldt, newt;
    int ch;
    int oldf;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if (ch != EOF) {
        ungetc(ch, stdin);
        return 1;
    }
    return 0;
}

void draw() {
    system("clear");
    for (int i = 0; i <= SCREEN_HEIGHT; i++) {
        for (int j = 0; j <= SCREEN_WIDTH; j++) {
            printf("%c", draw_screen[i][j]);
        }
        printf("\n");
    }
}

int main(void) {
    return 0;
}