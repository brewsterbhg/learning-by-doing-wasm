#include <emscripten.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>

#define NUM_CIRCLES 500      // The number of circles to render on the canvas
#define MAX_RADIUS 50        // The max radius of a circle in pixels
#define MAX_COORDINATES 1000 // The max starting x,y coordinates to initally render a circle
#define MAX_RGB 255          // RGB max value for randomizing circle colour
#define MAX_VELOCITY 10      // The max velocity value for a circle (pixels moved per frame)
#define DIRECTION_FORWARD 1  // If the circle is moving forwards
#define DIRECTION_BACKWARD 0 // If the circle is moving backwards

// Correlates to number of circle struct properties. This is needed
// so JavaScript can properly parse the necessary data from the
// array buffer it receives from memory.
#define CIRCLE_STRUCT_LENGTH 6

// The struct defining the properties of a single circle
struct Circle
{
    int x;  // Circle x-position
    int y;  // Circle y-position
    int r;  // Circle radius
    int cr; // Circle red value (RGB)
    int cg; // Circle green value (RGB)
    int cb; // Circle blue valie (RGB)
};

// The struct defining the animation properties of a single circle
struct CircleAnimationData
{
    int x;  // Circle x-position
    int y;  // Circle y-position
    int r;  // Circle radius
    int xv; // Velocity moving on x-axis
    int yv; // Velocity moving on y-axis
    int xd; // Current moving direction on x-axis
    int yd; // Current moving direction on y-axis
};

struct Circle circles[NUM_CIRCLES];                    // Array struct for holding circle instance data
struct CircleAnimationData animationData[NUM_CIRCLES]; // Array struct for holding circle animation data

// Returns a random number between 1 and the supplied max value
int getRand(max)
{
    return ((rand() % max) + 1);
}

// Main function to execute
int main()
{
    // Set a seed for the randomizer based on timestamp (not the best seed)
    srand(time(NULL));

    for (int i = 0; i < NUM_CIRCLES; i++)
    {
        // Generate a random radius for the circle
        int radius = getRand(MAX_RADIUS);

        // Generate random starting x,y coordinates (taking radius in to account to stay within canvas)
        int x = getRand(MAX_COORDINATES) + radius;
        int y = getRand(MAX_COORDINATES) + radius;

        // Set starting data for animation
        animationData[i].x = x;
        animationData[i].y = y;
        animationData[i].r = radius;

        // Generate random velocities for x and y axis
        animationData[i].xv = getRand(MAX_VELOCITY);
        animationData[i].yv = getRand(MAX_VELOCITY);

        // Generate random starting direction
        animationData[i].xd = getRand(1);
        animationData[i].yd = getRand(1);

        // Set starting data for circle
        circles[i].x = x;
        circles[i].y = y;
        circles[i].r = radius;

        // Generate random RGB values for the circle
        circles[i].cr = getRand(MAX_RGB);
        circles[i].cg = getRand(MAX_RGB);
        circles[i].cb = getRand(MAX_RGB);
    }

    // Helper function to call a JavaScript function with arguments
    EM_ASM({ render($0, $1); }, NUM_CIRCLES * CIRCLE_STRUCT_LENGTH, CIRCLE_STRUCT_LENGTH);
}

// Each frame in JavaScript canvas will call this function. This function handles collision
// detection, as well as updating the current x,y coordinates based on the velocity of each
// circle. This function receives the canvas bounds to perform collision detection.
struct Circle *getCircles(int canvasWidth, int canvasHeight)
{
    for (int i = 0; i < NUM_CIRCLES; i++)
    {
        // If the circle x position + radius is at the right edge of the canvas, flip the direction
        if (animationData[i].x + animationData[i].r >= canvasWidth)
        {
            animationData[i].xd = DIRECTION_BACKWARD;
        }
        // If the circle x position - radius is at the left edge of the canvas, flip the direction
        if (animationData[i].x - animationData[i].r <= 0)
        {
            animationData[i].xd = DIRECTION_FORWARD;
        }
        // If the circle y position + radius is at the top edge of the canvas, flip the direction
        if (animationData[i].y + animationData[i].r >= canvasHeight)
        {
            animationData[i].yd = DIRECTION_BACKWARD;
        }
        // If the circle y position - radius is at the bottom edge of the canvas, flip the direction
        if (animationData[i].y - animationData[i].r <= 0)
        {
            animationData[i].yd = DIRECTION_FORWARD;
        }
        // Update the x position based on the circle velocity and direction
        if (animationData[i].xd == DIRECTION_FORWARD)
        {
            animationData[i].x += animationData[i].xv;
        }
        else
        {
            animationData[i].x -= animationData[i].xv;
        }
        // Update the y position based on the circle velocity and direction
        if (animationData[i].yd == DIRECTION_FORWARD)
        {
            animationData[i].y += animationData[i].yv;
        }
        else
        {
            animationData[i].y -= animationData[i].yv;
        }

        // Set the circle coordinates equal to the animation coordinates
        circles[i].x = animationData[i].x;
        circles[i].y = animationData[i].y;
    }

    return circles;
}