/* This files provides address values that exist in the system */

#define SDRAM_BASE            0xC0000000
#define FPGA_ONCHIP_BASE      0xC8000000
#define FPGA_CHAR_BASE        0xC9000000

/* Cyclone V FPGA devices */
#define LEDR_BASE             0xFF200000
#define HEX3_HEX0_BASE        0xFF200020
#define HEX5_HEX4_BASE        0xFF200030
#define SW_BASE               0xFF200040
#define KEY_BASE              0xFF200050
#define TIMER_BASE            0xFF202000
#define PIXEL_BUF_CTRL_BASE   0xFF203020
#define CHAR_BUF_CTRL_BASE    0xFF203030

/* VGA colors */
#define WHITE 0xFFFF
#define YELLOW 0xFFE0
#define RED 0xF800
#define GREEN 0x07E0
#define BLUE 0x001F
#define CYAN 0x07FF
#define MAGENTA 0xF81F
#define GREY 0xC618
#define PINK 0xFC18
#define ORANGE 0xFC00

#define ABS(x) (((x) > 0) ? (x) : -(x))

/* Screen size. */
#define RESOLUTION_X 320
#define RESOLUTION_Y 240

/* Constants for snowman drawing */
#define HEAD_RADIUS 25
#define BODY_RADIUS 35
#define FEET_RADIUS 45

#define FALSE 0
#define TRUE 1

#include <stdlib.h>
#include <stdio.h>

// Begin part3.c code for Lab 7


volatile int pixel_buffer_start; // global variable

// code for subroutines (not shown)

void plot_pixel(int x, int y, short int line_color){
    *(short int *)(pixel_buffer_start + (y << 10) + (x << 1)) = line_color;
}

void clear_screen(){
    for (int i = 0; i < RESOLUTION_X; i++){
        for (int j = 0; j < RESOLUTION_Y; j++){
            plot_pixel(i, j, 0x0000);
        }
    }
}


void wait_for_vsync(){
    volatile int * pixel_ctrl_ptr = (int *)0xFF203020;
    register int status;
    *pixel_ctrl_ptr = 1;
    status = *(pixel_ctrl_ptr + 3);
    while ((status & 0x01) != 0){
        status = *(pixel_ctrl_ptr + 3);
    }
}


void swap(int *x0, int *y0)
{
    int temp = *x0;
    *x0 = *y0;
    *y0 = temp;
}


void draw_line(int x0, int y0, int x1, int y1, short int color)
{
    int is_steep = ABS(y1 - y0) > ABS(x1 - x0);
    if (is_steep)
    {
        swap(&x0, &y0);
        swap(&x1, &y1);
    }
    if (x0 > x1)
    {
        swap(&x0, &x1);
        swap(&y0, &y1);
    }

    int deltax = x1 - x0;
    int deltay = ABS(y1 - y0);
    int error = -(deltax / 2);
    int y = y0;
    int y_step;
    if (y0 < y1)
    {
        y_step = 1;
    }
    else
    {
        y_step = -1;
    }

    int x;
    for (x = x0; x < x1; ++x)
    {
        if (is_steep)
        {
            plot_pixel(y, x, color);
        }
        else
        {
            plot_pixel(x, y, color);
        }

        error = error + deltay;
        if (error >= 0)
        {
            y = y + y_step;
            error = error - deltax;
        }
    }
}

void draw_sphere(int x, int y, int radius, short int color)
{
    int x0 = x - radius;
    int x1 = x + radius;
    int y0 = y - radius;
    int y1 = y + radius;
    int i, j;
    for (i = x0; i <= x1; i++)
    {
        for (j = y0; j <= y1; j++)
        {
            if ((i - x) * (i - x) + (j - y) * (j - y) <= radius * radius)
            {
                plot_pixel(i, j, color);
            }
        }
    }
}

void draw_current_snowman(int health) {
    // Draw snowman based on health
    if (health == 6) {
        // Draw full snowman
        clear_screen();
        draw_sphere(RESOLUTION_X/2, 0+HEAD_RADIUS + 5, HEAD_RADIUS, WHITE);
        draw_sphere(RESOLUTION_X/2, 0+HEAD_RADIUS + 5 + BODY_RADIUS + 5, BODY_RADIUS, WHITE);
        draw_sphere(RESOLUTION_X/2, 0+HEAD_RADIUS + 5 + BODY_RADIUS + 5 + FEET_RADIUS + 5, FEET_RADIUS, WHITE);
    }
    else if (health == 5) {
        clear_screen();
        draw_sphere(RESOLUTION_X/2, 0+HEAD_RADIUS + 5, HEAD_RADIUS, WHITE);
        draw_sphere(RESOLUTION_X/2, 0+HEAD_RADIUS + 5 + BODY_RADIUS + 5, BODY_RADIUS, WHITE);
        draw_sphere(RESOLUTION_X/2, 0+HEAD_RADIUS + 5 + BODY_RADIUS + 5 + FEET_RADIUS + 5, FEET_RADIUS, WHITE);
    }
    else if (health == 4) {
        clear_screen();
        draw_sphere(RESOLUTION_X/2, 0+HEAD_RADIUS + 5, HEAD_RADIUS, WHITE);
        draw_sphere(RESOLUTION_X/2, 0+HEAD_RADIUS + 5 + BODY_RADIUS + 5, BODY_RADIUS, WHITE);
        draw_sphere(RESOLUTION_X/2, 0+HEAD_RADIUS + 5 + BODY_RADIUS + 5 + FEET_RADIUS + 5, FEET_RADIUS, WHITE);
    }
    else if (health == 3) {
        clear_screen();
        draw_sphere(RESOLUTION_X/2, 0+HEAD_RADIUS + 5, HEAD_RADIUS, WHITE);
        draw_sphere(RESOLUTION_X/2, 0+HEAD_RADIUS + 5 + BODY_RADIUS + 5, BODY_RADIUS, WHITE);
        draw_sphere(RESOLUTION_X/2, 0+HEAD_RADIUS + 5 + BODY_RADIUS + 5 + FEET_RADIUS + 5, FEET_RADIUS, WHITE);
    }
    else if (health == 2) {
        // Draw snowman head and feet
        clear_screen();
        draw_sphere(RESOLUTION_X/2, 0+HEAD_RADIUS + 5, HEAD_RADIUS, WHITE);
        draw_sphere(RESOLUTION_X/2, 0+HEAD_RADIUS + 5 + BODY_RADIUS + 5 + FEET_RADIUS + 5, FEET_RADIUS, WHITE);
    }
    else if (health == 1) {
        // Draw snowman head only
        clear_screen();
        draw_sphere(RESOLUTION_X/2, 0+HEAD_RADIUS + 5, HEAD_RADIUS, WHITE);
    }
    else {
        // Draw red X
        clear_screen();
        draw_line(RESOLUTION_X/2 - HEAD_RADIUS, 0+HEAD_RADIUS + 5 - HEAD_RADIUS, RESOLUTION_X/2 + HEAD_RADIUS, 0+HEAD_RADIUS + 5 + HEAD_RADIUS, RED);
        draw_line(RESOLUTION_X/2 - HEAD_RADIUS, 0+HEAD_RADIUS + 5 + HEAD_RADIUS, RESOLUTION_X/2 + HEAD_RADIUS, 0+HEAD_RADIUS + 5 - HEAD_RADIUS, RED);
    }
}

int main(void)
{
    volatile int * pixel_ctrl_ptr = (int *)0xFF203020;
    // declare other variables(not shown)
    // initialize location and direction of rectangles(not shown)

    /* set front pixel buffer to start of FPGA On-chip memory */
    *(pixel_ctrl_ptr + 1) = 0xC8000000; // first store the address in the 
                                        // back buffer
    /* now, swap the front/back buffers, to set the front buffer location */
    wait_for_vsync();
    /* initialize a pointer to the pixel buffer, used by drawing functions */
    pixel_buffer_start = *pixel_ctrl_ptr;
    clear_screen(); // pixel_buffer_start points to the pixel buffer
    /* set back pixel buffer to start of SDRAM memory */
    *(pixel_ctrl_ptr + 1) = 0xC0000000;
    pixel_buffer_start = *(pixel_ctrl_ptr + 1); // we draw on the back buffer
    clear_screen(); // pixel_buffer_start points to the pixel buffer

    // Snowman health states (list of points to draw for each health value):
    //6 health --> filled circle for head, filled circle for body, filled circle for feet
    //int health_6[];

    int SnowmanHealth = 6;
    volatile int * PS2_ADDRESS = 0xFF200100;
    int PS2_data, RVALID;
    unsigned char val;

    while (1)
    {
        // Erase old snowman
        draw_current_snowman(SnowmanHealth);

        // Read from PS2
        PS2_data = *(PS2_ADDRESS);
        RVALID = (PS2_data & 0x8000);
        if (RVALID != 0)
		{
			/* always save the last three bytes received */
			val = PS2_data & 0xFF;
		}
        else {
            val = 0;
        }

        if (val >= 1) {
            SnowmanHealth--;
        }

        wait_for_vsync();
        pixel_buffer_start = *(pixel_ctrl_ptr + 1); // new back buffer

        
    }
}
