// clang game_logic_test.c -lncurses
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <ncurses.h>

// Define colors
#define PLAYER_COLOR 1
#define BG_COLOR 2
#define BLOCK_COLOR 3

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
int draw_screen[SCREEN_WIDTH][SCREEN_HEIGHT] = {0};

// Define player starting position
int player_x = 10, player_y = 5;
int start_x = 10, start_y = 5;
int player_pos_x = SCREEN_WIDTH / 2;

void startGame();
void resetObstacle(int pos[2], int *height, int *width);
bool collideObstacle(int pos[2], int height, int width);
void draw_block(int y, int x, chtype ch, int color_pair);


int main() {
    srand(time(NULL));
    initscr();            // Initialize ncurses mode
    start_color();        // Enable color support
    keypad(stdscr, TRUE); // Enable arrow keys
    noecho();             // Don't echo keypresses
    curs_set(0);          // Hide cursor

    // Define color pairs
    init_pair(PLAYER_COLOR, COLOR_RED, COLOR_RED);
    init_pair(BG_COLOR, COLOR_WHITE, COLOR_WHITE);
    init_pair(BLOCK_COLOR, COLOR_BLUE, COLOR_BLUE);

    startGame();

    endwin(); // Restore normal terminal mode
    return 0;
}

void startGame() {
    bool game_over = false;

    // in game variables
    int obstacles_pos[10][2], obstacle_height[10], obstacle_width[10];

    // init obstacles
    for (int i = 0; i < 10; ++i) {
        resetObstacle(obstacles_pos[i], &obstacle_height[i], &obstacle_width[i]);
    }

    // Make getch() non-blocking
    nodelay(stdscr, TRUE);

    while (!game_over) {
        clear();

        // Check for collision
        for (int i = 0; i < 10; ++i) {
            if (collideObstacle(obstacles_pos[i], obstacle_height[i], obstacle_width[i])) {
                game_over = true;
                break;
            }
        }

        // Update obstacles position
        for (int i = 0; i < 10; ++i) {
            if (obstacles_pos[i][1] < 0) {
                resetObstacle(obstacles_pos[i], &obstacle_height[i], &obstacle_width[i]);
            } else {
                obstacles_pos[i][1] -= 1;
            }
        }

        // Get user input
        int ch = getch();
        switch (ch) {
            case KEY_LEFT:
                if (player_pos_x > 0) player_pos_x -= 10;
                break;
            case KEY_RIGHT:
                if (player_pos_x < SCREEN_WIDTH - 1) player_pos_x += 10;
                break;
            case 'q': // Quit game
                return;
        }

        // Draw obstacles
        for (int i = 0; i < 10; ++i) {
            for (int j = obstacles_pos[i][0]; j < obstacles_pos[i][0] + obstacle_width[i]; ++j) {
                for (int k = obstacles_pos[i][1]; k < obstacles_pos[i][1] + obstacle_height[i]; ++k) {
                    draw_block(k, j, ' ', BLOCK_COLOR);
                }
            }
        }

        // Draw player
        for (int i = player_pos_x - PLYAER_X_OFFSET; i < player_pos_x + PLYAER_X_OFFSET; ++i) {
            for (int j = player_pos_y - PLAYER_Y_OFFSET; j < player_pos_y + PLAYER_Y_OFFSET; ++j) {
                draw_block(j, i, ' ', PLAYER_COLOR);
            }
        }

        // Refresh screen to show updates
        refresh();
        napms(50); // Slow down loop
    }

    // Display Game Over message
    mvprintw(SCREEN_HEIGHT / 2, SCREEN_WIDTH / 2 - 5, "GAME OVER!");
    refresh();
    napms(2000); // Pause before exiting
}

void resetObstacle(int pos[2], int *height, int *width) {
    pos[0] = rand() % SCREEN_WIDTH;
    pos[1] = rand() % SCREEN_HEIGHT;
    *height = rand() % 30 + 1; // Make sure it's not zero
    *width = rand() % 30 + 1;
}


bool collideObstacle(int pos[2], int height, int width) {
    return !(player_pos_x + PLYAER_X_OFFSET < pos[0] || 
             player_pos_x - PLYAER_X_OFFSET > pos[0] + width ||
             player_pos_y + PLAYER_Y_OFFSET < pos[1] ||
             player_pos_y - PLAYER_Y_OFFSET > pos[1] + height);
}


void draw_block(int y, int x, chtype ch, int color_pair) {
    attron(COLOR_PAIR(color_pair));
    mvaddch(y, x, ch);
    attroff(COLOR_PAIR(color_pair));
}