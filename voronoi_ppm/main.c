#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>

#define WIDTH 1600
#define HEIGHT 900
#define NUMBER_OF_SEEDS 14

typedef struct {
    int x, y;
} Point;

uint32_t PIXELS[WIDTH*HEIGHT];
Point SEEDS[NUMBER_OF_SEEDS];
int INDEX_BUFFER[WIDTH*HEIGHT];

#define BRIGHT_RED      0xFF4349FB
#define BRIGHT_GREEN    0xFFB26BB8
#define BRIGHT_YELLOW   0xFFB2FDFA
#define BRIGHT_BLUE     0xFFA98583
#define BRIGHT_PURPLE   0xFF89B6D3
#define BRIGHT_AQUA     0xFFC7C08E
#define BRIGHT_ORANGE   0xFF8190FE
#define NEUTRAL_RED     0xFF21D4CC
#define NEUTRAL_GREEN   0xFF91A798
#define NEUTRAL_YELLOW  0xFF9219D7
#define NEUTRAL_BLUE    0xFF888545
#define NEUTRAL_PURPLE  0xFF6862B1
#define NEUTRAL_AQUA    0xFF96AD68
#define NEUTRAL_ORANGE  0xFF50EDD6

uint32_t COLORS[NUMBER_OF_SEEDS] = {
BRIGHT_RED,
BRIGHT_GREEN,
BRIGHT_YELLOW,
BRIGHT_BLUE,
BRIGHT_PURPLE,
BRIGHT_AQUA,
BRIGHT_ORANGE,
NEUTRAL_RED,
NEUTRAL_GREEN,
NEUTRAL_YELLOW,
NEUTRAL_BLUE,
NEUTRAL_PURPLE,
NEUTRAL_AQUA,
NEUTRAL_ORANGE,
};


void fill_everything(uint32_t *PIXELS, uint32_t color);
void save_to_ppm_file(uint32_t *PIXELS, const char *FILE_PATH);
void draw_filled_circle(uint32_t *PIXELS, int cx, int cy, int radius, uint32_t color);
int squared_distance(int cx, int cy, int x, int y);
int manhattan_distance(int cx, int cy, int x, int y);
int chebyshev_distance(int cx, int cy, int x, int y);
int absolute(int x);
int maximum(int x, int y);
void generate_random_seeds(Point *SEEDS);
void render_seeds(uint32_t *PIXELS, Point *SEEDS, int radius, uint32_t color);
void render_voronoi(uint32_t *PIXELS, Point *SEEDS, uint32_t *COLORS);
void render_voronoi_upside_down(uint32_t *PIXELS, Point *SEEDS, uint32_t *COLORS, int *INDEX_BUFFER);

int main(void)
{
    printf("Hello Debug!\n");
    //srand(time(NULL));
    const char * FILE_PATH = "chebyshev.ppm";
    fill_everything(PIXELS, 0xFF000000);
    generate_random_seeds(SEEDS);
    render_voronoi(PIXELS, SEEDS, COLORS);
    //render_voronoi_upside_down(PIXELS, SEEDS, COLORS, INDEX_BUFFER);
    render_seeds(PIXELS, SEEDS, 5, 0xFF181818);
    save_to_ppm_file(PIXELS, FILE_PATH);
    return 0;
}

void fill_everything(uint32_t *PIXELS, uint32_t color)
{
    for (int i = 0; i < WIDTH*HEIGHT; i++) {
            PIXELS[i] = color;
        }
}

void save_to_ppm_file(uint32_t *PIXELS, const char *FILE_PATH)
{
    FILE *f = fopen(FILE_PATH, "wb");
    if (f == NULL) {
        printf("Could not open a file!\n");
        exit(EXIT_FAILURE);
    }

    fprintf(f, "P6\n%d %d\n255\n", WIDTH, HEIGHT);
    if (ferror(f)) {
        fclose(f);
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < WIDTH*HEIGHT; i++) {
            uint32_t color = PIXELS[i];
            uint8_t buffer[3] = {
            ( color >> (8*2) )&0xFF,
            ( color >> (8*1) )&0xFF,
            ( color >> (8*0) )&0xFF,
        };
            fwrite(buffer, sizeof(buffer), 1, f);
            if (ferror(f)) {
                fclose(f);
                exit(EXIT_FAILURE);
            }
        }
    fclose(f);
}

void draw_filled_circle(uint32_t *PIXELS, int cx, int cy, int radius, uint32_t color)
{
    int max_x = cx+radius;
    int max_y = cy+radius;
    int min_x = cx-radius;
    int min_y = cy-radius;

    for (int x = min_x; x <= max_x; x++) {
        if ( (x >= 0) && (x < WIDTH) ) {
            for (int y = min_y; y <= max_y; y++) {
                if ( (y >= 0) && (y < HEIGHT) ) {
                    if ( squared_distance(cx, cy, x, y) <= radius*radius) {
                        PIXELS[y * WIDTH + x] = color;
                    }
                }
            }
        }
    }
}

int squared_distance(int cx, int cy, int x, int y)
{
    int dx = x - cx;
    int dy = y - cy;
    return dx*dx + dy*dy;
}

void generate_random_seeds(Point *SEEDS)
{
    for (int i = 0; i < NUMBER_OF_SEEDS; i++) {
        SEEDS[i].x = rand()%WIDTH;
        SEEDS[i].y = rand()%HEIGHT;
    }
}

void render_seeds(uint32_t *PIXELS, Point *SEEDS, int radius, uint32_t color)
{
    for (int i = 0; i < NUMBER_OF_SEEDS; i++) {
        draw_filled_circle(PIXELS, SEEDS[i].x,SEEDS[i].y, radius, color);
    }
}

void render_voronoi(uint32_t *PIXELS, Point *SEEDS, uint32_t *COLORS)
{
    for (int x = 0; x < WIDTH; x ++) {
        for (int y = 0; y < HEIGHT; y++) {
            int INDEX = 0;
            for (int i = 1; i < NUMBER_OF_SEEDS; i++) {
                if (chebyshev_distance(SEEDS[i].x, SEEDS[i].y, x, y) <
                    chebyshev_distance(SEEDS[INDEX].x, SEEDS[INDEX].y, x, y)) {
                    INDEX = i;
                }
            }
            PIXELS[y * WIDTH + x] = COLORS[INDEX];
        }
    }
}



void render_voronoi_upside_down(uint32_t *PIXELS, Point *SEEDS, uint32_t *COLORS, int *INDEX_BUFFER)
{
    for (int i = 0; i < WIDTH*HEIGHT; i++) {
        PIXELS[i] = COLORS[0];
        INDEX_BUFFER[i] = 0;
    }

    for (int index = 1; index < NUMBER_OF_SEEDS; index++) {
        for (int x = 0; x < WIDTH; x++) {
            for (int y = 0; y < HEIGHT; y++) {
                int seed_in_buffer = INDEX_BUFFER[y * WIDTH + x];
                if (squared_distance(SEEDS[index].x, SEEDS[index].y, x, y) <
                    squared_distance(SEEDS[seed_in_buffer].x, SEEDS[seed_in_buffer].y, x, y)) {
                   INDEX_BUFFER[y * WIDTH + x]  = index;
                   PIXELS[y * WIDTH + x]  = COLORS[index];
                }
            }
        }
    }
}

int absolute(int x)
{
    if (x < 0) {
        return -x;
    }
    return x;
}

int maximum(int x, int y)
{
    if (x < y) {
       x = y;
    }
    return x;
}

int manhattan_distance(int cx, int cy, int x, int y)
{
    return absolute(cx - x) + absolute(cy - y);
}

int chebyshev_distance(int cx, int cy, int x, int y)
{
    return maximum(absolute(cx - x), absolute(cy - y));
}
