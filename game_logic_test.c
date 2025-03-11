// gcc game_logic_test.c -lncurses
#include <stdlib.h>
#include <stdbool.h>
#include <ncurses.h>

// Define colors
#define PLAYER_COLOR 1
#define BG_COLOR 3

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

void startGame();
void draw_block(int y, int x, chtype ch, int color_pair);
void draw_block(int y, int x, chtype ch, int color_pair) {
    attron(COLOR_PAIR(color_pair));
    mvaddch(y, x, ch);
    attroff(COLOR_PAIR(color_pair));
}
int main() {
    initscr();            // Initialize ncurses mode
    start_color();        // Enable color support
    keypad(stdscr, TRUE); // Enable arrow keys
    noecho();             // Don't echo keypresses
    curs_set(0);          // Hide cursor

    // Define color pairs
    init_pair(PLAYER_COLOR, COLOR_WHITE, COLOR_BLACK);
    init_pair(BG_COLOR, COLOR_WHITE, COLOR_WHITE);

    game_loop(); // Start the game loop

    endwin(); // Restore normal terminal mode
    return 0;
}

void startGame() {
    bool game_over = false;

    // in game variables
    int player_pos_x = SCREEN_WIDTH / 2;
    int obstacles_pos[10][2], obstacle_height[10], obstacle_width[10];

    // init obstacles
    for (int i = 0; i < 10; ++i) {
        resetObstacle(obstacles_pos[i], obstacle_height[i], obstacle_width[i]);
    }


    while (1) {
        // if reset is pressed, reset the game to start screen
        if (reset) {
            reset = false;
            in_start_page = true;
        }
        // game over condition
        if (game_over) {
            showGameOver();
            break;
        }
        
        /* Game Logic Begin */
        // check if player collide with obstacle
        for (int i = 0; i < 10; ++i) {
            if (collideObstacle(obstacles_pos[i], obstacle_height[i], obstacle_width[i])) {
                game_over = true;
                break;
            }
        }

        // update obstacles position
        for (int i = 0; i < 10; ++i) {
            if (1/*obstacle completely outside screen*/) {
                resetObstacle(obstacles_pos[i], obstacle_height[i], obstacle_width[i]);
            }
            else { // else update obstacle position
                obstacles_pos[i][1] += 1;
                
            }
        }
        // move player if key is pressed

        // clean screen
        
        // draw obstacle

        // draw player

        /* Game Logic End */
    }
    showGameOver();
}



/*
void game_loop() {
    int ch;
    while (1) {
        clear();
        
        // Draw background blocks
        for (int i = 0; i < LINES; i++) {
            for (int j = 0; j < COLS; j++) {
                draw_block(i, j, ' ', BG_COLOR);
            }
        }

        // Draw player block
        draw_block(player_y, player_x, '@', PLAYER_COLOR);

        refresh();
        
        ch = getch(); // Get user input
        
        switch (ch) {
            case KEY_UP:
                if (player_y > 0) player_y--;
                break;
            case KEY_DOWN:
                if (player_y < LINES - 1) player_y++;
                break;
            case KEY_LEFT:
                if (player_x > 0) player_x--;
                break;
            case KEY_RIGHT:
                if (player_x < COLS - 1) player_x++;
                break;
            case 10:  // Enter key (Reset position)
                player_x = start_x;
                player_y = start_y;
                break;
            case 'q': // Quit game
                return;
        }
    }
}
*/