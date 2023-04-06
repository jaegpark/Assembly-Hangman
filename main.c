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

/* WORDS */
#define EASY1 "apple"
#define EASY2 "beach"
#define EASY3 "chair"
#define EASY4 "dance"
#define EASY5 "earth"
#define MEDIUM1 "slate"
#define MEDIUM2 "sugar"
#define MEDIUM3 "tiger"
#define MEDIUM4 "towel"
#define MEDIUM5 "water"
#define HARD1 "ducat"
#define HARD2 "ouija"
#define HARD3 "enoki"
#define HARD4 "azure"
#define HARD5 "soare"

#define FALSE 0
#define TRUE 1

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h> 


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

void draw_letter_helper(int x, int y, short int color, const uint32_t letter_matrix[8]);

void draw_letter(char letter, int x, int y, short int color){
    /**
     * @brief Draws a letter on the VGA screen at specified coordinates with specified color.
     * 
     * Each letter will be written in uppercase for simplicity
     * Each letter is 8x8 pixels.
     */
    letter = toupper(letter);
    static const uint32_t letter_matrices[][8] = {
        // Define 8x8 binary matrices for each uppercase letter here, with 1s indicating where to draw the letter pixels.
        // Each matrix should have dimensions [8] and represent a single uppercase letter.
        0x1C, 0x36, 0x63, 0x63, 0x7F, 0x63, 0x63, 0x00, // A
        0x3E, 0x33, 0x33, 0x3E, 0x33, 0x33, 0x3E, 0x00, // B
        0x1E, 0x33, 0x30, 0x30, 0x30, 0x33, 0x1E, 0x00, // C
        0x3E, 0x33, 0x33, 0x33, 0x33, 0x33, 0x3E, 0x00, // D
        0x3F, 0x30, 0x30, 0x3C, 0x30, 0x30, 0x3F, 0x00, // E
        0x3F, 0x30, 0x30, 0x3E, 0x30, 0x30, 0x30, 0x00, // F
        0x1F, 0x30, 0x30, 0x33, 0x33, 0x33, 0x1F, 0x00, // G
        0x33, 0x33, 0x33, 0x3F, 0x33, 0x33, 0x33, 0x00, // H
        0x1C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x1C, 0x00, // I
        0x0E, 0x06, 0x06, 0x06, 0x06, 0x36, 0x1C, 0x00, // J
        0x33, 0x36, 0x3C, 0x38, 0x3C, 0x36, 0x33, 0x00, // K
        0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x3F, 0x00, // L
        0x63, 0x77, 0x7F, 0x7F, 0x6B, 0x63, 0x63, 0x00, // M
        0x33, 0x33, 0x3B, 0x3F, 0x37, 0x33, 0x33, 0x00, // N
        0x1E, 0x33, 0x33, 0x33, 0x33, 0x33, 0x1E, 0x00, // O
        // P
        0x3E, 0x33, 0x33, 0x3E, 0x30, 0x30, 0x30, 0x00,
        // Q
        0x1E, 0x33, 0x33, 0x33, 0x37, 0x36, 0x1D, 0x00,
        // R
        0x3E, 0x33, 0x33, 0x3E, 0x3C, 0x36, 0x33, 0x00,
        // S
        0x1F, 0x30, 0x30, 0x1E, 0x03, 0x03, 0x3E, 0x00,
        // T
        0x7F, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x00,
        // U
        0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x1E, 0x00,
        // V
        0x33, 0x33, 0x33, 0x33, 0x33, 0x1E, 0x0C, 0x00,
        // W
        0x63, 0x63, 0x63, 0x6B, 0x7F, 0x77, 0x63, 0x00,
        // X
        0x63, 0x36, 0x1C, 0x08, 0x1C, 0x36, 0x63, 0x00,
        // Y
        0x33, 0x33, 0x33, 0x1E, 0x0C, 0x0C, 0x0C, 0x00,
        // Z
        0x3F, 0x06, 0x0C, 0x18, 0x30, 0x60, 0x7F, 0x00 
        // Define 16x16 binary matrices for each uppercase letter here, with 1s indicating where to draw the letter pixels.
        // Each matrix should have dimensions [16] and represent a single uppercase letter.
       /*  0x1C1C, 0x3636, 0x6363, 0x6363, 0x7F7F, 0x6363, 0x6363, 0x0000, // A
        0x3E3E, 0x3333, 0x3333, 0x3E3E, 0x3333, 0x3333, 0x3E3E, 0x0000, // B
        0x1E1E, 0x3333, 0x3030, 0x3030, 0x3030, 0x3333, 0x1E1E, 0x0000, // C
        0x3E3E, 0x3333, 0x3333, 0x3333, 0x3333, 0x3333, 0x3E3E, 0x0000, // D
        0x3F3F, 0x3030, 0x3030, 0x3C3C, 0x3030, 0x3030, 0x3F3F, 0x0000, // E
        0x3F3F, 0x3030, 0x3030, 0x3E3E, 0x3030, 0x3030, 0x3030, 0x0000, // F
        0x1F1F, 0x3030, 0x3030, 0x3333, 0x3333, 0x3333, 0x1F1F, 0x0000, // G
        0x3333, 0x3333, 0x3333, 0x3F3F, 0x3333, 0x3333, 0x3333, 0x0000, // H
        0x1C1C, 0x0C0C, 0x0C0C, 0x0C0C, 0x0C0C, 0x0C0C, 0x1C1C, 0x0000, // I
        0x0E0E, 0x0606, 0x0606, 0x0606, 0x0606, 0x3636, 0x1C1C, 0x0000, // J
        0x3333, 0x3636, 0x3C3C, 0x3838, 0x3C3C, 0x3636, 0x3333, 0x0000, // K
        0x3030, 0x3030, 0x3030, 0x3030, 0x3030, 0x3030, 0x3F3F, 0x0000, // L
        0x6363, 0x7777, 0x7F7F, 0x7F7F, 0x6B6B, 0x6363, 0x6363, 0x0000, // M
        0x3333, 0x3333, 0x3B3B, 0x3F3F, 0x3737, 0x3333, 0x3333, 0x0000, // N
        0x1E1E, 0x3333, 0x3333, 0x3333, 0x3333, 0x3333, 0x1E1E, 0x0000, // O
        0x3E3E, 0x3333, 0x3333, 0x3E3E, 0x3030, 0x3030, 0x3030, 0x0000, // P
        0x1E1E, 0x3333, 0x3333, 0x3333, 0x3737, 0x3636, 0x1D1D, 0x0000, // Q
        0x3E3E, 0x3333, 0x3333, 0x3E3E, 0x3C3C, 0x3636, 0x3333, 0x0000, // R
        0x1F1F, 0x3030, 0x3030, 0x1E1E, 0x0303, 0x0303, 0x3E3E, 0x0000, // S
        0x7F7F, 0x0C0C, 0x0C0C, 0x0C0C, 0x0C0C, 0x0C0C, 0x0C0C, 0x0000, // T
        0x3333, 0x3333, 0x3333, 0x3333, 0x3333, 0x3333, 0x1E1E, 0x0000, // U
        0x3333, 0x3333, 0x3333, 0x3333, 0x3333, 0x1E1E, 0x0C0C, 0x0000, // V
        0x6363, 0x6363, 0x6363, 0x6B6B, 0x7F7F, 0x7777, 0x6363, 0x0000, // W
        0x6363, 0x3636, 0x1C1C, 0x0808, 0x1C1C, 0x3636, 0x6363, 0x0000, // X
        0x3333, 0x3333, 0x3333, 0x1E1E, 0x0C0C, 0x0C0C, 0x0C0C, 0x0000, // Y
        0x3F3F, 0x0606, 0x0C0C, 0x1818, 0x3030, 0x6060, 0x7F7F, 0x0000  // Z */
    };
    
    if (letter >= 'A' && letter <= 'Z') {
        int letter_index = letter - 'A';
        draw_letter_helper(x, y, color, letter_matrices[letter_index]);
    }
}

void draw_letter_helper(int x, int y, short int color, const uint32_t letter_matrix[8]) {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (letter_matrix[i] & (1 << j)) {
                plot_pixel(x + j, y + 2* i, color);
                
                plot_pixel(x + j, y + 2* i + 1, color);
            }
        }
    }
    /* for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            if (letter_matrix[i] & (1 << (15 - j))) {
                plot_pixel(x + j, y + (2 * i), color);
                plot_pixel(x + j, y + (2 * i) + 1, color);
            }
        }
         for (int j = 0; j < 16; ++j) {
            if (letter_matrix[i] & (1 << (31 - j))) {
                plot_pixel(x + j, y + (2 * i) + 16, color);
                plot_pixel(x + j, y + (2 * i) + 17, color);
            }
        } 
    } */
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

int convert_to_ascii(int num) {
    // Convert number to ASCII
    if (num == 0x1C) {
        return 'a';
    }
    else if (num == 0x32) {
        return 'b';
    }
    else if (num == 0x21) {
        return 'c';
    }
    else if (num == 0x23) {
        return 'd';
    }
    else if (num == 0x24) {
        return 'e';
    }
    else if (num == 0x2B) {
        return 'f';
    }
    else if (num == 0x34) {
        return 'g';
    }
    else if (num == 0x33) {
        return 'h';
    }
    else if (num == 0x43) {
        return 'i';
    }
    else if (num == 0x3B) {
        return 'j';
    }
    else if (num == 0x42) {
        return 'k';
    }
    else if (num == 0x4B) {
        return 'l';
    }
    else if (num == 0x3A) {
        return 'm';
    }
    else if (num == 0x31) {
        return 'n';
    }
    else if (num == 0x44) {
        return 'o';
    }
    else if (num == 0x4D) {
        return 'p';
    }
    else if (num == 0x15) {
        return 'q';
    }
    else if (num == 0x2D) {
        return 'r';
    }
    else if (num == 0x1B) {
        return 's';
    }
    else if (num == 0x2C) {
        return 't';
    }
    else if (num == 0x3C) {
        return 'u';
    }
    else if (num == 0x2A) {
        return 'v';
    }
    else if (num == 0x1D) {
        return 'w';
    }
    else if (num == 0x22) {
        return 'x';
    }
    else if (num == 0x35) {
        return 'y';
    }
    else if (num == 0x1A) {
        return 'z';
    }    
}

char* generate_word(int difficulty, char* word_array[]) {
    // Generate random index between 0 and 4
    int i = rand() % 5;
    if (difficulty == 2) {
        // Easy
        return word_array[i];
    }
    else if (difficulty == 4) {
        // Medium
        return word_array[i+5];
    }
    else{
        // Hard
        return word_array[i+10];
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

    char* wordArray[] = {EASY1,EASY2,EASY3,EASY4,EASY5,MEDIUM1,MEDIUM2,MEDIUM3,MEDIUM4,MEDIUM5,HARD1,HARD2,HARD3,HARD4,HARD5};

    int SnowmanHealth = 6;
    volatile int* PS2_ADDRESS = 0xFF200100;
    volatile int* LED_ADDRESS = LEDR_BASE;
    volatile int* KEY_ADDRESS = KEY_BASE;
    int PS2_data, RVALID;
    unsigned char key_val;

    //Game State Integer
    int game_state = 0;
    int difficulty = 0;

    //temporary word
    char* word;

    while (1)
    {
        int key_value = *(KEY_ADDRESS);
        if (key_value == 1) {
            game_state = 0;
            SnowmanHealth = 6;
        }
        if (game_state == 0) {
            //Draw starting screen, wait for button press to determine difficulty
            if (key_value > 1) {
                difficulty = key_value; // Switch to edgecaptures if needed
                game_state = 1;
                //Generate random word based on difficulty
                //word = "hello";
                word = generate_word(difficulty, wordArray);
                
            }
        }
        else if (game_state == 1) {
            //Draw game screen, wait for key input to determine if snowman is hit or character is guessed
            draw_current_snowman(SnowmanHealth);
            draw_letter('Q', 0, 0, WHITE);
            draw_letter('R', 10, 0, WHITE);
            draw_letter('S', 20, 0, WHITE);
            draw_letter('T', 30, 0, WHITE);
            draw_letter('U', 40, 0, WHITE);
            draw_letter('V', 50, 0, WHITE);
            draw_letter('W', 60, 0, WHITE);
            draw_letter('X', 70, 0, WHITE);
            draw_letter('Y', 80, 0, WHITE);
            draw_letter('Z', 80, 0, WHITE);
            
            // Read from PS2
            PS2_data = *(PS2_ADDRESS);
            RVALID = (PS2_data & 0x8000);
            if (RVALID != 0)
            {
                key_val = PS2_data & 0xFF;
                if (key_val != 0xF0) {
                    key_val = convert_to_ascii(key_val);

                    //check if key_val is inside the word
                    //if not, decrement health
                    int key_in_word = 0;
                    for (int i = 0; i < 5; i++) {
                        if (key_val == word[i]) {
                            key_in_word = 1;
                            //reveal_letter(i);
                        }
                    }

                    if (!key_in_word) {
                        SnowmanHealth--;
                        if (SnowmanHealth == 0) {
                            draw_current_snowman(SnowmanHealth);
                            game_state = 2;
                        }
                    }
                }

            }
            else {
                //if no key is pressed set key_val to 0
                key_val = 0;
            }
            

            wait_for_vsync();
            pixel_buffer_start = *(pixel_ctrl_ptr + 1); // new back buffer

        }
        else if (game_state == 2) {
            //Draw game over screen, prompt restart option
        }
        *(LED_ADDRESS) = game_state; 
        
        
        
    }
}
