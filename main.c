#include <stdlib.h>
#include <stdbool.h>

// Global Variables
#define SCREEN_HEIGHT 240
#define SCREEN_WIDTH 320
int pixel_buffer_start;
bool reset = false;
bool in_start_page = false;
bool start = false;

// Functions Declear
void plot_pixel(int x, int y, short int line_color);
void swap(int *a, int *b);
void showStartPage();
void showGameOver();
void startGame();

// Main Function
int main(void) {
    volatile int * pixel_ctrl_ptr = (int *)0xFF203020;
    
    while (1) {
        if (in_start_page && start) {
            startGame();
        }
    }
    return 0;
}



// Functions
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

void showStartPage() {

}

void startGame() {
    while (1) {
        // if reset is pressed, reset the game to start screen
        if (reset) {
            reset = false;
            in_start_page = true;
        }
        // game over condition
        if (1) {
            showGameOver();
        }
        // 
    }
}

