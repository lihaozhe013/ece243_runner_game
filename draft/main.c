#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#define OBSTACLE_1_X_POS 53 // 106 / 2 = 53
#define OBSTACLE_2_X_POS 160// 106 + 108 / 2 = 106 + 54 = 160
#define OBSTACLE_3_X_POS 267 // 320 - 53 = 267
#define HALF_OBSTACLE_1_WIDTH 53
#define HALF_OBSTACLE_2_WIDTH 54
#define HALF_OBSTACLE_3_WIDTH 53
#define HALF_OBSTACLES_HEIGHT 40 

#define PLAYER_SPEED 5
#define player_pos_y 220
#define SCREEN_HEIGHT 240
#define SCREEN_WIDTH 320
#define PLYAER_X_OFFSET 3
#define PLAYER_Y_OFFSET 5
#define LANES 3

int pixel_buffer_start; // global variable
short int Buffer1[240][512]; // 240 rows, 512 (320 + padding) columns
short int Buffer2[240][512];

int player_pos_x = SCREEN_WIDTH / 2;
int old_player_pos_x;
int obstacle_height[5];
int obstacle_height_old[5];
bool obstacle_pos[5][3] = {0};
bool obstacle_pos_old[5][3] = {0};
int current_speed = 5;
int arrow_input = 0;

void swap(int *a, int *b);
void plot_pixel(int x, int y, short int line_color);
void draw_ranctangle(int x, int y, int half_witdh, int half_height, short int line_color);
void clear_screen();
void wait_for_vsync();
void get_button_input();
void get_keyboard_input();
bool game(); // if return true, means game over, if return false, it means game ended accidentally
bool collideObstacle(int x, int y, int half_width);

int main(void)
{
    arrow_input = 0;
	srand(time(NULL));
    volatile int * pixel_ctrl_ptr = (int *)0xFF203020;
    // declare other variables(not shown)
    // initialize location and direction of rectangles(not shown)

    /* set front pixel buffer to Buffer 1 */
    *(pixel_ctrl_ptr + 1) = (int) &Buffer1; // first store the address in the  back buffer
    /* now, swap the front/back buffers, to set the front buffer location */
    wait_for_vsync();
    /* initialize a pointer to the pixel buffer, used by drawing functions */
    pixel_buffer_start = *pixel_ctrl_ptr;
    clear_screen(); // pixel_buffer_start points to the pixel buffer

    /* set back pixel buffer to Buffer 2 */
    *(pixel_ctrl_ptr + 1) = (int) &Buffer2;
    pixel_buffer_start = *(pixel_ctrl_ptr + 1); // we draw on the back buffer
    clear_screen(); // pixel_buffer_start points to the pixel buffer

    // game start
    while (1) {
        if (game()) {
            while (1) {
                get_button_input();
                if (arrow_input == 1) {
                    /* set front pixel buffer to Buffer 1 */
                    *(pixel_ctrl_ptr + 1) = (int) &Buffer1; // first store the address in the  back buffer
                    /* now, swap the front/back buffers, to set the front buffer location */
                    wait_for_vsync();
                    /* initialize a pointer to the pixel buffer, used by drawing functions */
                    pixel_buffer_start = *pixel_ctrl_ptr;
                    clear_screen(); // pixel_buffer_start points to the pixel buffer

                    /* set back pixel buffer to Buffer 2 */
                    *(pixel_ctrl_ptr + 1) = (int) &Buffer2;
                    pixel_buffer_start = *(pixel_ctrl_ptr + 1); // we draw on the back buffer
                    clear_screen(); // pixel_buffer_start points to the pixel buffer
                    break;
                }
            }
        }
    }
}

void plot_pixel(int x, int y, short int line_color)
{
    volatile short int *one_pixel_address;
    one_pixel_address = pixel_buffer_start + (y << 10) + (x << 1);
    *one_pixel_address = line_color;
}

void swap(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

void draw_ranctangle(int x, int y, int half_witdh, int half_height, short int line_color) {
    for (int i = ((x - half_witdh >= 0) ? (x - half_witdh) : 0); i < ((x + half_witdh) <= SCREEN_WIDTH ? (x + half_witdh) : SCREEN_WIDTH); ++i) {
        for (int j = ((y - half_height) >= 0 ? (y - half_height) : 0); j < ((y + half_height) <= SCREEN_HEIGHT ? (y + half_height) : SCREEN_HEIGHT); ++j) {
            plot_pixel(i, j, line_color);
        }
    }
}

void wait_for_vsync() {
    volatile int * pixel_ctrl_ptr = (int *)0xFF203020;
    * pixel_ctrl_ptr = 1;
    while(*(pixel_ctrl_ptr + 3) & 0b1);
}

void clear_screen() {
    for (int i = 0; i < SCREEN_WIDTH; ++i) {
        for (int j = 0; j < SCREEN_HEIGHT; ++j) {
            plot_pixel(i, j, 0);
        }
    }
}

void get_button_input() {
    volatile int* buttons_ptr = (int *)0xFF200050;
    int button_data = *(buttons_ptr);
    
    if (button_data & 0b0001) { // key 0
        arrow_input = 1;
    } else if (button_data & 0b0010) { // key 1 pressed
        arrow_input = 2;
    }
}

void get_keyboard_input() {
    volatile int * keyboard_ptr = (int * )0xFF200100;
    int input[3] = {0};
    int inputIndex = 2;
   
    if (*(keyboard_ptr) & 0xF) {
        for (int i = 0; i < (*(keyboard_ptr) & 0xFFFF0000); i++) {
            input[inputIndex] = *(keyboard_ptr) & 0x7; // save the input
            inputIndex--;
            if (inputIndex == -1) { // reset index
                inputIndex = 2;
            }
            if (input[0] == 0xE0 && input[1] == 0xF0 && input[2] == 0x6B) { // left arrow key release
                arrow_input = 1;
                break;
            } else if (input[0] == 0xE0 && input[1] == 0xF0 && input[2] == 0x74) { // right arrow key release
                arrow_input = 2;
                break;
            } else if (input[0] == 0xE0 && input[1] == 0xF0 && input[2] == 0x75) { // up arrow key release
                arrow_input = 3;
                break;
            } else if (input[0] == 0xE0 && input[1] == 0xF0 && input[2] == 0x72) { // down arrow key release
                arrow_input = 4;
                break;
            }
        }
    }
}

bool game() {
    volatile int * pixel_ctrl_ptr = (int *)0xFF203020;
    // draw player for the first time
    old_player_pos_x = player_pos_x;
    draw_ranctangle(player_pos_x, player_pos_y, PLYAER_X_OFFSET, PLAYER_Y_OFFSET, 0xFFFF);

    // draw the obstacles for the first time
    for (int i = 0; i < 5; ++i) {
        obstacle_height[i] = -SCREEN_HEIGHT - (120 * i);
        int numObstacles = rand() % LANES;
        int placed = 0;
        while (placed < numObstacles) {
            int lane = rand() % LANES;
            if (obstacle_pos[i][lane] == 0) {
                obstacle_pos[i][lane] = 1;
                ++placed;
            }
        }
    }

    for (;;) {
        // ===================Clear Screen====================
        // clear old player
        draw_ranctangle(old_player_pos_x, player_pos_y, PLYAER_X_OFFSET + PLAYER_SPEED, PLAYER_Y_OFFSET, 0);
       
        // clear old obstacles
        for (int i = 0; i < 5; ++i) {
            if (obstacle_pos_old[i][0] == true) {
                draw_ranctangle(OBSTACLE_1_X_POS, obstacle_height_old[i], HALF_OBSTACLE_1_WIDTH + current_speed, HALF_OBSTACLES_HEIGHT + current_speed, 0);
            }
            if (obstacle_pos_old[i][1] == true) {
                draw_ranctangle(OBSTACLE_2_X_POS, obstacle_height_old[i], HALF_OBSTACLE_2_WIDTH + current_speed, HALF_OBSTACLES_HEIGHT + current_speed, 0);
            }
            if (obstacle_pos_old[i][2] == true) {
                draw_ranctangle(OBSTACLE_3_X_POS, obstacle_height_old[i], HALF_OBSTACLE_3_WIDTH + current_speed, HALF_OBSTACLES_HEIGHT + current_speed, 0);
            }
        }
        // ================End of Clear Screen================



        // ================Draw New Elements==================
        // draw new player
        draw_ranctangle(player_pos_x, player_pos_y, PLYAER_X_OFFSET, PLAYER_Y_OFFSET, 0xFFFF);

        // draw the new obstacles
        for (int i = 0; i < 5; ++i) {
            if (obstacle_pos[i][0] == true) {
                draw_ranctangle(OBSTACLE_1_X_POS, obstacle_height[i], HALF_OBSTACLE_1_WIDTH, HALF_OBSTACLES_HEIGHT, 0xFFC0CB);
            }
            if (obstacle_pos[i][1] == true) {
                draw_ranctangle(OBSTACLE_2_X_POS, obstacle_height[i], HALF_OBSTACLE_2_WIDTH, HALF_OBSTACLES_HEIGHT, 0xFFC0CB);
            }
            if (obstacle_pos[i][2] == true) {
                draw_ranctangle(OBSTACLE_3_X_POS, obstacle_height[i], HALF_OBSTACLE_3_WIDTH, HALF_OBSTACLES_HEIGHT, 0xFFC0CB);
            }
        }
        // Wait for vertical sync to swap buffers
        // =============End of Draw New Elements==============

        wait_for_vsync();
       
        // Update the pixel_buffer_start pointer to the new back buffer
        pixel_buffer_start = *(pixel_ctrl_ptr + 1);



        // ==========Update the Previous Position===========
        old_player_pos_x = player_pos_x;
        for (int i = 0; i < 5; ++i) {
            obstacle_height_old[i] = obstacle_height[i];
            for (int j = 0; j < 3; ++j) {
                obstacle_pos_old[i][j] = obstacle_pos[i][j];
            }
        }
        // ======End of Update the Previous Position=======



        // ================Check Game Over=================
        for (int i = 0; i < 5; ++i) {
            if (obstacle_pos[i][0] == true)
                if (collideObstacle(OBSTACLE_1_X_POS, obstacle_height[i], HALF_OBSTACLE_1_WIDTH)) {
                    return true;
                }
            if (obstacle_pos[i][1] == true)
                if (collideObstacle(OBSTACLE_2_X_POS, obstacle_height[i], HALF_OBSTACLE_2_WIDTH)) {
                    return true;
                }
            if (obstacle_pos[i][0] == true)
                if (collideObstacle(OBSTACLE_3_X_POS, obstacle_height[i], HALF_OBSTACLE_3_WIDTH)) {
                    return true;
                }
        }
        // ============End of Check Game Over==============




        // ===========Update Elements Position=============
        // update player position
        get_button_input();
        switch (arrow_input)
        {
        case 1:
            if (player_pos_x <= SCREEN_WIDTH - 6)
                player_pos_x += PLAYER_SPEED;
            arrow_input = 0;
            break;
        case 2:
            if (player_pos_x >= 6)
                player_pos_x -= PLAYER_SPEED;
            arrow_input = 0;
            break;
        }

        // update obstacle postion
        for (int i = 0; i < 5; ++i) {
            if (obstacle_height[i] - HALF_OBSTACLES_HEIGHT - 1 >= SCREEN_HEIGHT) {
                obstacle_height[i] = obstacle_height[(i + 4) % 5] - 120;
                // clear pos info
                for (int j = 0; j < 3; ++j) {
                    obstacle_pos[i][j] = false;
                }
                int numObstacles = rand() % LANES;
                int placed = 0;
                while (placed < numObstacles) {
                    int lane = rand() % LANES;
                    if (obstacle_pos[i][lane] == 0) {
                        obstacle_pos[i][lane] = 1;
                        ++placed;
                    }
                }
            }
            else {
                obstacle_height[i] += current_speed;
            }
        }
        // ========End of Update Elements Position==========
    }
    return false;
}

bool collideObstacle(int x, int y, int half_width) {
    return !(player_pos_x + PLYAER_X_OFFSET < x - half_width || 
             player_pos_x - PLYAER_X_OFFSET > x + half_width ||
             player_pos_y + PLAYER_Y_OFFSET < y - HALF_OBSTACLES_HEIGHT||
             player_pos_y - PLAYER_Y_OFFSET > y + HALF_OBSTACLES_HEIGHT);
}
