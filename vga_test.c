#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#define HALF_WIDTH_BOX 2
#define SCREEN_HEIGHT 240
#define SCREEN_WIDTH 320
#define player_pos_y 40
#define PLYAER_X_OFFSET 4
#define PLAYER_Y_OFFSET 7

int pixel_buffer_start; // global variable
short int Buffer1[240][512]; // 240 rows, 512 (320 + padding) columns
short int Buffer2[240][512];
int player_pos_x = SCREEN_WIDTH / 2;
int old_player_pos_x;

void swap(int *a, int *b);
void plot_pixel(int x, int y, short int line_color);
void draw_ranctangle(int x, int y, int half_witdh, int half_height, short int line_color);
void clear_screen();
void wait_for_vsync();

int main(void)
{
    srand(time(NULL));
    
    volatile int * pixel_ctrl_ptr = (int *)0xFF203020;
    volatile int * keyboard_ptr = (int * )0xFF200100;
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
    old_player_pos_x = player_pos_x;
    draw_ranctangle(player_pos_x, player_pos_y, PLYAER_X_OFFSET, PLAYER_Y_OFFSET, 0xFFFF);
    while (1)
    {
		// clear_screen();
        draw_ranctangle(old_player_pos_x, player_pos_y, PLYAER_X_OFFSET, PLAYER_Y_OFFSET, 0);
        wait_for_vsync();
        // code for drawing the boxes and lines (not shown)
        // draw boxes
        draw_ranctangle(player_pos_x, player_pos_y, PLYAER_X_OFFSET, PLAYER_Y_OFFSET, 0xFFFF);
        old_player_pos_x = player_pos_x;

        player_pos_x += 1;
        wait_for_vsync(); // swap front and back buffers on VGA vertical sync
        pixel_buffer_start = *(pixel_ctrl_ptr + 1); // new back buffer
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
    for (int i = x - half_witdh; i < x + half_witdh; ++i) {
        for (int j = y - half_height; j < y + half_height; ++j) {
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

void get_keyboard_input() {
    int input[3]; = {0};
    int inputIndex = 2;
    
    if (*(keyboard_ptr) & 0xF) {
        while (*(keyboard_ptr) & 0xF) {
            input[inputIndex] = *(keyboard_ptr) & 0x7; // save the input
            inputIndex--;
            if (inputIndex == -1) { // reset index
                inputIndex = 2;
            }
            if (input[0] == 0xE0 && input[1] == 0xF0 && input[2] == 0x6B) { // left arrow key release
                break;
            } else if (input[0] == 0xE0 && input[1] == 0xF0 && input[2] == 0x74) { // right arrow key release
                break;
            } else if (input[0] == 0xE0 && input[1] == 0xF0 && input[2] == 0x75) { // up arrow key release
                break;
            } else if (input[0] == 0xE0 && input[1] == 0xF0 && input[2] == 0x72) { // down arrow key release
                break;
            }
        }
    }
}