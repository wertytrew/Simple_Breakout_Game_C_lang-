//
// breakout.c
//
// Computer Science 50
// Problem Set 4
//

// standard libraries
#define _XOPEN_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Stanford Portable Library
#include "gevents.h"
#include "gobjects.h"
#include "gwindow.h"

// height and width of game's window in pixels
#define HEIGHT 600
#define WIDTH 400

// height and width of game's paddle in pixels
#define PADDLE_HEIGHT 10
#define PADDLE_WIDTH 60
#define PADDLE_BOTTOM (HEIGHT - 100)

// number of rows of bricks
#define ROWS 5

// number of columns of bricks
#define COLS 7

// size of bricks
#define BRICK_WIDTH ((WIDTH / COLS) - 4)
#define BRICK_HEIGHT 15
#define SPACING ((WIDTH - (BRICK_WIDTH * COLS)) / (COLS + 1))

// radius of ball in pixels
#define RADIUS 10

// lives
#define LIVES 3

// prototypes
void initBricks(GWindow window);
GOval initBall(GWindow window);
GRect initPaddle(GWindow window);
GLabel initScoreboard(GWindow window);
void updateScoreboard(GWindow window, GLabel label, int points);
GObject detectCollision(GWindow window, GOval ball);

int main(void)
{
    // seed pseudorandom number generator
    srand48(time(NULL));

    // instantiate window
    GWindow window = newGWindow(WIDTH, HEIGHT);

    // instantiate bricks
    initBricks(window);

    // instantiate ball, centered in middle of window
    GOval ball = initBall(window);

    // instantiate paddle, centered at bottom of window
    GRect paddle = initPaddle(window);

    // instantiate scoreboard, bottom right of window
    GLabel label = initScoreboard(window);

    // number of bricks initially
    int bricks = COLS * ROWS;

    // number of lives initially
    int lives = LIVES;

    // number of points initially
    int points = 0;
    
    // initial velocity for horizontal and vertical
    double velocityX = drand48() + 2;
    double velocityY = drand48() + 3;
    
    waitForClick();

    // keep playing until game over
    while (lives > 0)
    {
        // listen for mouse movement
        GEvent event = getNextEvent(MOUSE_EVENT);
        
        // paddle movment and bounds
        if (event != NULL)
        {   
            if (getEventType(event) == MOUSE_MOVED)
            {
                double x = getX(event);
                
                // stop at right window edge
                if ((x > WIDTH - PADDLE_WIDTH / 2))
                {
                    setLocation(paddle, WIDTH - PADDLE_WIDTH, PADDLE_BOTTOM);
                }
                
                // stop at left window edge
                else if (x > 0 && x < PADDLE_WIDTH / 2)
                {
                    setLocation(paddle, 0, PADDLE_BOTTOM);
                }
                
                // just move normally
                else
                {
                    setLocation(paddle, x - (PADDLE_WIDTH / 2), PADDLE_BOTTOM);
                }
            }
        }
        
        move(ball, velocityX, velocityY);
        
        // throttle ball movement a bit
        pause(10);
        
        // bounce off horizontal edge of window
        if ((getX(ball) + RADIUS * 2 >= WIDTH) || getX(ball) <= 0)
        {
            velocityX = -velocityX;
        }

        // bounce off top edge of window
        else if (getY(ball) <= 0)
        {
            velocityY = -velocityY;
        }
        
        // if ball makes it below the paddle and touches bottom edge of window
        else if (getY(ball) + RADIUS * 2 >= HEIGHT)
        {
            lives--;
            removeGWindow(window, ball);

            // recenter the paddle
            addAt(window, paddle, (WIDTH / 2) - (PADDLE_WIDTH / 2), PADDLE_BOTTOM);
            
            if (lives > 0)
            {
                ball = initBall(window);
                waitForClick();
            }
        }
        
        // detect collision with ball in the window
        GObject object = detectCollision(window, ball);
        
        if (object == paddle)
        {
            velocityY = -velocityY;
        }
        
        if (object == label)
        {
            draw(window, label);
        }
        
        if ((object != NULL) && (object != paddle) && (object != label))
        {
            removeGWindow(window, object);
            velocityY = -velocityY;
            bricks--;
            
            // points per brick equal lives left
            points = points + (1 * lives);
            updateScoreboard(window, label, points);
            
            if (bricks == 0)
            {
                GLabel win_label = newGLabel("You Won - Goodbye :)");
                
                // easthetic appearance
                setFont(win_label, "SansSerif-24");
                setFilled(win_label, true);
                setColor(win_label, "RED");
                
                // center label
                double x = (getWidth(window) - getWidth(win_label)) / 2;
                double y = (getHeight(window) - getHeight(win_label)) / 2;
                setLocation(win_label, x, y);
                add(window, win_label);
                
                // wait before closing window to quit game
                pause(2000);
                closeGWindow(window);
            }
        }
    }

    // wait for click before exiting
    waitForClick();

    // game over
    closeGWindow(window);
    return 0;
}

/**
 * Initializes window with a grid of bricks.
 */
void initBricks(GWindow window)
{
    string colors[5] =  {"CYAN", "ORANGE", "GREEN", "ORANGE", "CYAN"};
    for (int i = 0; i < COLS; i++)
    {
        for (int j = 0; j < ROWS; j++)
        {
            double x = (BRICK_WIDTH * i) + SPACING + (SPACING * i);
            double y = (BRICK_HEIGHT * j) + SPACING + (SPACING * j);
            GRect brick = newGRect(x, y + 50, BRICK_WIDTH, BRICK_HEIGHT);
            setFilled(brick, true);
            setColor(brick, colors[j]);
            add(window, brick);
        }
    }
}

/**
 * Instantiates ball in center of window.  Returns ball.
 */
GOval initBall(GWindow window)
{
    GOval ball = newGOval(WIDTH / 2 - RADIUS, HEIGHT / 2 - RADIUS, RADIUS * 2, RADIUS * 2);
    setFilled(ball, true);
    setColor(ball, "GRAY");
    add(window, ball);
    return ball;
    free(ball);
}

/**
 * Instantiates paddle in bottom-middle of window.
 */
GRect initPaddle(GWindow window)
{
    GRect paddle = newGRect((WIDTH / 2) - (PADDLE_WIDTH / 2), PADDLE_BOTTOM, PADDLE_WIDTH, PADDLE_HEIGHT);
    setFilled(paddle, true);
    setColor(paddle, "LIGHT_GRAY");
    add(window, paddle);
    return paddle;
    free(paddle);
}

/**
 * Instantiates, configures, and returns label for scoreboard.
 */
GLabel initScoreboard(GWindow window)
{
    GLabel label = newGLabel("");
    
    // easthetic appearance of label
    setFont(label, "SansSerif-30");
    setFilled(label, true);
    setColor(label, "RED");
    
    // position label in lower right of window
    double x = WIDTH - 50;
    double y = HEIGHT - 20;
    setLocation(label, x, y);
    
    add(window, label);
    return label;
    free(label);
}

/**
 * Updates scoreboard's label, keeping it centered in window.
 */
void updateScoreboard(GWindow window, GLabel label, int points)
{
    // update label
    char s[12];
    sprintf(s, "%i", points);
    setLabel(label, s);
}

/**
 * Detects whether ball has collided with some object in window
 * by checking the four corners of its bounding box (which are
 * outside the ball's GOval, and so the ball can't collide with
 * itself).  Returns object if so, else NULL.
 */
GObject detectCollision(GWindow window, GOval ball)
{
    // ball's location
    double x = getX(ball);
    double y = getY(ball);

    // for checking for collisions
    GObject object;

    // check for collision at ball's top-left corner
    object = getGObjectAt(window, x, y);
    if (object != NULL)
    {
        return object;
        free(object);
    }

    // check for collision at ball's top-right corner
    object = getGObjectAt(window, x + 2 * RADIUS, y);
    if (object != NULL)
    {
        return object;
        free(object);
    }

    // check for collision at ball's bottom-left corner
    object = getGObjectAt(window, x, y + 2 * RADIUS);
    if (object != NULL)
    {
        return object;
        free(object);
    }

    // check for collision at ball's bottom-right corner
    object = getGObjectAt(window, x + 2 * RADIUS, y + 2 * RADIUS);
    if (object != NULL)
    {
        return object;
        free(object);
    }

    // no collision
    return NULL;
    free(object);
}
