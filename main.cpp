#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "class_shader.h"

#include <iostream>
#include <chrono>
#include <thread>
#include <time.h>
#include <math.h>
#include <assert.h>

void processInput(GLFWwindow *window);

#define SCREEN_WIDTH 1600
#define SCREEN_HEIGHT 900
#define NUMBER_OF_SEEDS 20
const int SEED_RADIUS = 15;


typedef struct {
    float x, y;
} Vector;

typedef struct {
    float r, g, b;
} Color;

Vector SEEDS[NUMBER_OF_SEEDS];
Color COLORS[NUMBER_OF_SEEDS];
Vector DIRECTIONS[NUMBER_OF_SEEDS];
Vector SPEEDS[NUMBER_OF_SEEDS];

float random_float();
void generate_random_seeds(Vector *SEEDS);
void generate_random_colors(Color *COLORS);
void generate_random_directions(Vector *DIRECTIONS);
void generate_random_velocities(Vector *SPEEDS, float lower, float upper);
void update_positions(Vector *SEEDS, Vector *DIRECTIONS, Vector *SPEEDS);
int squared_distance(int cx, int cy, int x, int y);
float inner_product(Vector a, Vector b);
Vector substract_vectors(Vector a, Vector b);
Vector add_vectors(Vector a, Vector b);
float vector_norm(Vector a);
Vector scale_vector(Vector a, float scalar);
void print_vector(Vector a);
Vector normalize_vector(Vector a);


int main()
{
    //srand(time(0));
    srand(0);

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "VoronoiDiagram", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    Shader shader("vertex_shader.glsl", "fragment_shader.glsl");

    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glEnable(GL_DEPTH_TEST);

    const int target_fps = 60;
    const std::chrono::milliseconds frame_duration(1000 / target_fps);

    shader.use();

    GLint location_resolution = glGetUniformLocation(shader.ID, "resolution");
    GLint location_seed = glGetUniformLocation(shader.ID, "seed");
    GLint location_color = glGetUniformLocation(shader.ID, "color");
    GLint location_radius = glGetUniformLocation(shader.ID, "radius");

    generate_random_seeds(SEEDS);
    //SEEDS[0].x += 1;
    generate_random_colors(COLORS);
    //COLORS[0] = Color{1.0f, 1.0f, 1.0f};
    generate_random_directions(DIRECTIONS);
    generate_random_velocities(SPEEDS, 1, 5);

    int CHANGE_DIRECTIONS_AND_SPEEDS = 0;
    while (!glfwWindowShouldClose(window))
    {
        auto start_time = std::chrono::high_resolution_clock::now();
        processInput(window);

        //if (CHANGE_DIRECTIONS_AND_SPEEDS == 150) {
        //    generate_random_directions(DIRECTIONS);
        //    generate_random_velocities(SPEEDS, 1, 20);
        //    CHANGE_DIRECTIONS_AND_SPEEDS = 0;
        //}

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        update_positions(SEEDS, DIRECTIONS, SPEEDS);

        for (int i = 0; i < NUMBER_OF_SEEDS; i++) {
            glUniform1i(location_radius, SEED_RADIUS);
            glUniform2f(location_resolution, SCREEN_WIDTH, SCREEN_HEIGHT);
            glUniform2f(location_seed, SEEDS[i].x, SEEDS[i].y);
            glUniform4f(location_color, COLORS[i].r, COLORS[i].g, COLORS[i].b, 1.0f);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();

        auto end_time = std::chrono::high_resolution_clock::now();
        auto elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        if (elapsed_time < frame_duration) {
            std::this_thread::sleep_for(frame_duration - elapsed_time);

        }
        CHANGE_DIRECTIONS_AND_SPEEDS++;
    }
    glDeleteVertexArrays(1, &VAO);

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

float random_float()
{
    return (float)rand() / RAND_MAX;
}

void generate_random_seeds(Vector *SEEDS)
{
    for (int i = 0; i < NUMBER_OF_SEEDS; i++) {
        SEEDS[i].x = (rand() % (SCREEN_WIDTH-SEED_RADIUS-SEED_RADIUS) + SEED_RADIUS);
        SEEDS[i].y = (rand() % (SCREEN_HEIGHT-SEED_RADIUS-SEED_RADIUS) + SEED_RADIUS);
    }
}

void generate_random_colors(Color *COLORS)
{
    for (int i = 0; i < NUMBER_OF_SEEDS; i++) {
        COLORS[i].r = random_float();
        COLORS[i].g = random_float();
        COLORS[i].b = random_float();
    }

}

void generate_random_directions(Vector *DIRECTIONS)
{
    for (int i = 0; i < NUMBER_OF_SEEDS; i++) {
        //float random = random_float();
        DIRECTIONS[i].x = cosf(random_float() * 2 * M_PI);
        DIRECTIONS[i].y = sinf(random_float() * 2 * M_PI);
    }
}

void generate_random_velocities(Vector *SPEEDS, float lower, float upper)
{
    assert(upper >= lower);
    for (int i = 0; i < NUMBER_OF_SEEDS; i++) {
       SPEEDS[i].x = random_float() * (upper - lower) + lower;
       SPEEDS[i].y = random_float() * (upper - lower) + lower;
    }
}

int squared_distance(int cx, int cy, int x, int y)
{
    int dx = cx - x;
    int dy = cy - y;
    return dx*dx + dy*dy;
}

void update_positions(Vector *SEEDS, Vector *DIRECTIONS, Vector *SPEEDS)
{
    int TWO_RADIUS_SQUARED = (SEED_RADIUS+SEED_RADIUS) * (SEED_RADIUS+SEED_RADIUS);
    for (int i = 0; i < NUMBER_OF_SEEDS; i++) {
        if (SEEDS[i].x < SCREEN_WIDTH-SEED_RADIUS && SEEDS[i].x > SEED_RADIUS) {
            SEEDS[i].x += DIRECTIONS[i].x * SPEEDS[i].x;
        } else {
            DIRECTIONS[i].x = -DIRECTIONS[i].x;
            SEEDS[i].x += DIRECTIONS[i].x * SPEEDS[i].x;
        }

        if (SEEDS[i].y < SCREEN_HEIGHT-SEED_RADIUS && SEEDS[i].y > SEED_RADIUS) {
            SEEDS[i].y += DIRECTIONS[i].y * SPEEDS[i].y;
        } else {
            DIRECTIONS[i].y = -DIRECTIONS[i].y;
            SEEDS[i].y += DIRECTIONS[i].y * SPEEDS[i].y;
        }

        for (int i = 0; i < NUMBER_OF_SEEDS - 1; i++) {
            for (int j = i + 1; j < NUMBER_OF_SEEDS; j++) {
                if (squared_distance(SEEDS[i].x, SEEDS[i].y,
                                     SEEDS[j].x, SEEDS[j].y) <= TWO_RADIUS_SQUARED) {
                    float distance_between = sqrt(squared_distance(SEEDS[i].x, SEEDS[i].y, SEEDS[j].x, SEEDS[j].y));
                    float overlap = SEED_RADIUS + SEED_RADIUS - distance_between;

                    Vector direction_of_displacement = substract_vectors(SEEDS[i], SEEDS[j]);
                    direction_of_displacement = normalize_vector(direction_of_displacement);

                    SEEDS[i] = add_vectors(SEEDS[i], scale_vector(direction_of_displacement, overlap));
                    SEEDS[j] = substract_vectors(SEEDS[j], scale_vector(direction_of_displacement, overlap));

                    Vector tangent = {-direction_of_displacement.y, direction_of_displacement.x};

                    float inner_tangent_1 = inner_product(DIRECTIONS[i], tangent);
                    float inner_tangent_2 = inner_product(DIRECTIONS[j], tangent);

                    float inner_norm_1 = inner_product(DIRECTIONS[i], direction_of_displacement);
                    float inner_norm_2 = inner_product(DIRECTIONS[j], direction_of_displacement);

                    DIRECTIONS[i] = add_vectors(scale_vector(tangent, inner_tangent_1), scale_vector(direction_of_displacement, inner_norm_2));
                    DIRECTIONS[j] = add_vectors(scale_vector(tangent, inner_tangent_2), scale_vector(direction_of_displacement, inner_norm_1));

             }
          }
        }
    }
}

float inner_product(Vector a, Vector b)
{
    return a.x*b.x + a.y*b.y;
}

Vector substract_vectors(Vector a, Vector b)
{
    Vector c = {a.x - b.x, a.y - b.y};
    return c;
}

Vector add_vectors(Vector a, Vector b)
{
    Vector c = {a.x + b.x, a.y + b.y};
    return c;
}

float vector_norm(Vector a)
{
    return sqrt(a.x*a.x + a.y*a.y);
}

Vector scale_vector(Vector a, float scalar)
{
    Vector c = {a.x*scalar, a.y*scalar};
    return c;
}

void print_vector(Vector a)
{
    printf("x = %f\t y = %f\n", a.x, a.y);
}

Vector normalize_vector(Vector a)
{
    float norm = vector_norm(a);
    Vector b = {a.x/norm, a.y/norm};
    return b;
}
