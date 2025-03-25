// gcc test_terminal_game.c -lncurses
#include <ncurses.h>

// Define colors
#define PLAYER_COLOR 1
#define BG_COLOR 3

// Define player starting position
int player_x = 10, player_y = 5;
int start_x = 10, start_y = 5;

void draw_block(int y, int x, chtype ch, int color_pair) {
    attron(COLOR_PAIR(color_pair));
    mvaddch(y, x, ch);
    attroff(COLOR_PAIR(color_pair));
}

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
