#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#define HALF_WIDTH_BOX 2
#define SCREEN_HEIGHT 240
#define SCREEN_WIDTH 320
int pixel_buffer_start; // global variable
short int Buffer1[240][512]; // 240 rows, 512 (320 + padding) columns
short int Buffer2[240][512];

void swap(int *a, int *b);
void draw_line(int x0, int y0, int x1, int y1, short int line_color);
void plot_pixel(int x, int y, short int line_color);
void draw_ranctangle(int x, int y, int half_witdh, int half_height, short int line_color);
void clear_screen();
void wait_for_vsync();

int arr_dirrection[8][2];
int arr_pos[8][2];
int old_arr_pos[8][2];
bool first_time = true;

int main(void)
{
    srand(time(NULL));
    for (int i = 0; i < 8; ++i) {
        arr_dirrection[i][0] = rand() % 2 ? 1 : -1;
        arr_dirrection[i][1] = rand() % 2 ? 1 : -1;
        arr_pos[i][0] = rand() % SCREEN_WIDTH;
        arr_pos[i][1] = rand() % SCREEN_HEIGHT;
    }
    
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

    while (1)
    {
		clear_screen();
        // code for drawing the boxes and lines (not shown)
        // draw boxes
        draw_ranctangle(arr_pos[0][0], arr_pos[0][1], 0xFFFF);
        
        for (int i = 1; i < 8; ++i) {
            draw_ranctangle(arr_pos[i][0], arr_pos[i][1], 0xFFFF);
            draw_line(arr_pos[i-1][0], arr_pos[i-1][1], arr_pos[i][0], arr_pos[i][1], 0xF1FF);
            old_arr_pos[i][0] = arr_pos[i][0];
            old_arr_pos[i][1] = arr_pos[i][1];
        }
        // update position of boxes
        for (int i = 0; i < 8; ++i) { // 0 is up, 1 is down, 2 is left, 3 is right
            arr_pos[i][0] += arr_dirrection[i][0];
            arr_pos[i][1] += arr_dirrection[i][1];
			if (arr_pos[i][0] == SCREEN_WIDTH || arr_pos[i][0] == 0) {arr_dirrection[i][0] = -arr_dirrection[i][0];}
            if (arr_pos[i][1] == SCREEN_HEIGHT || arr_pos[i][1] == 0) {arr_dirrection[i][1] = -arr_dirrection[i][1];}
        }

        // code for updating the locations of boxes (not shown)

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

void draw_line(int x0, int y0, int x1, int y1, short int line_color) {
    bool is_steep = abs(y1 - y0) > abs(x1 - x0);
    if (is_steep) {
        swap(&x0, &y0);
        swap(&x1, &y1);
    }
    if (x0 > x1) {
        swap(&x0, &x1);
        swap(&y0, &y1);
    }
    int delta_x = x1 - x0;
    int delta_y = abs(y1 - y0);
    int error = -(delta_x / 2);
    int y = y0;

    int y_step;
    if (y0 < y1)
        y_step = 1;
    else
        y_step = -1;
    for (int x = x0; x < x1; ++x) {
        if (is_steep)
            plot_pixel(y, x, line_color);
        else
            plot_pixel(x, y, line_color);
        error += delta_y;
        if (error > 0) {
            y = y + y_step;
            error -= delta_x;
        }
    }
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