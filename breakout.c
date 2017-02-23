// standard libraries
#define _XOPEN_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Stanford Portable Library
#include <spl/gevents.h>
#include <spl/gobjects.h>
#include <spl/gwindow.h>

// height and width of game's window in pixels
#define HEIGHT 600
#define WIDTH 400

// number of rows of bricks
#define ROWS 5

// number of columns of bricks
#define COLS 10

// radius of ball in pixels
#define RADIUS 10

// height and width of the paddle in pixels
#define PAD_H 6
#define PAD_W 90

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

    // instantiate scoreboard, centered in middle of window, just above ball
    GLabel label = initScoreboard(window);

    // number of bricks initially
    int bricks = COLS * ROWS;

    // number of lives initially
    int lives = LIVES;

    // number of points initially
    int points = 0;
    
    // display points on scoreboard
    updateScoreboard(window, label, points);
    
    // select x and y components of velocity randomly
    double velocityX = drand48() * 2 + 1;
    double velocityY = drand48() * 2 + 1;
    
    // click to start game
    waitForClick();
    
    // keep playing until game over
    while (lives > 0 && bricks > 0)
    {
        // check for a mouse event
        GEvent mouseMove = getNextEvent(MOUSE_EVENT);
        
        // check if the mouse moved
        if (mouseMove != NULL && getEventType(mouseMove) == MOUSE_MOVED)
        {
            /**
             * move paddle so that the X-coordinate of 
             * it's centre and the mouse is the same
             */
            double dx = getX(mouseMove) - getX(paddle) - PAD_W / 2;
            move(paddle, dx, 0);
        }
        
        // move the ball with randomly chosen velocity components
        move(ball, velocityX, velocityY);
        
        // check for a collision with ball
        GObject object = detectCollision(window, ball);
        
        if (object != NULL)
        {
            // bounce back upon collision with paddle
            if (object == paddle) 
            {
                velocityY *= -1;
            }
            
            // do nothing upon collision with label
            else if (object == label) 
            {
                pause(5);
                continue;
            }
            
            // upon collision with a brick
            else
            {
                // increment points
                points += 1;
                
                // update score on label
                updateScoreboard(window, label, points);
                
                // bounce back vertically
                velocityY *= -1;
                
                // decrement bricks
                bricks -= 1;
                
                // remove the brick
                removeGWindow(window, object);
            }
        }

        // bounce back when ball hits a side of the window
        if (getX(ball) + 2 * RADIUS >= WIDTH || getX(ball) <= 0)
        {
            velocityX *= -1;
        }
        
        // bounce back when ball hits top of the window
        if (getY(ball) <= 0) 
        {
            velocityY *= -1;
        }
        
        // when ball hits the bottom of window
        if (getY(ball) + 2 * RADIUS >= HEIGHT)
        {
            // decrement lives
            lives -= 1;
            if (lives > 0) 
            {
                // click to continue if lives are left
                waitForClick();
                setLocation(ball, WIDTH / 2 - RADIUS, HEIGHT / 2 - RADIUS);
            }
        }
        
        // wait for 5 milliseconds
        pause(5);
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
    // declare a 2-D array of GRect objects
    GRect grid[ROWS][COLS];
    
    // colours of bricks in different rows
    char* colours[5] = {"RED", "BLUE", "ORANGE", "GREEN", "YELLOW"};
    
    // position of top left brick
    double x = 2, y = 25;
    for (int i = 0; i < ROWS; i++)
    {
        for (int j = 0; j < COLS; j++)
        {
            // instantiate new brick
            grid[i][j] = newGRect(x, y, 36, 12);
            
            // set colour according to the row
            setColor(grid[i][j], colours[i]);
            setFilled(grid[i][j], true);
            
            // add brick to the window
            add(window, grid[i][j]);
            
            // increment x-coordinate for the next brick in the row
            x += 40;
        }
        // reset x-coordinate for the next row
        x = 2;
        
        // increment y-coordinate for next row
        y += 16;
    }
}

/**
 * Instantiates ball in center of window.  Returns ball.
 */
GOval initBall(GWindow window)
{
    double wid = WIDTH, hgt = HEIGHT, rad = RADIUS;
    // instantiate ball
    GOval ball = newGOval(wid / 2 - rad, hgt / 2 - rad, 2 * rad, 2 * rad);
    
    // colour set to black
    setColor(ball, "BLACK");
    setFilled(ball, true);
    
    // add object to the window
    add(window, ball);
    
    return ball;
}

/**
 * Instantiates paddle in bottom-middle of window.
 */
GRect initPaddle(GWindow window)
{
    // instantiate paddle, initially positioned at the bottom-middle
    GRect paddle; 
    paddle = newGRect((WIDTH - PAD_W) / 2, HEIGHT - PAD_H - 30, PAD_W, PAD_H);
    
    // colour set to black
    setColor(paddle, "BLACK");
    setFilled(paddle, true);
    
    // add object to the window
    add(window, paddle);
    return paddle;
}

/**
 * Instantiates, configures, and returns label for scoreboard.
 */
GLabel initScoreboard(GWindow window)
{
    // instantiate label
    GLabel label = newGLabel("0");
    
    // font
    setFont(label, "SansSerif-36");
    
    // colour set to gray
    setColor(label,"GRAY");
    
    // add the object to the window
    add(window, label);
    return label;
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

    // center label in window
    double x = (getWidth(window) - getWidth(label)) / 2;
    double y = (getHeight(window) - getHeight(label)) / 2;
    setLocation(label, x, y);
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
    }

    // check for collision at ball's top-right corner
    object = getGObjectAt(window, x + 2 * RADIUS, y);
    if (object != NULL)
    {
        return object;
    }

    // check for collision at ball's bottom-left corner
    object = getGObjectAt(window, x, y + 2 * RADIUS);
    if (object != NULL)
    {
        return object;
    }

    // check for collision at ball's bottom-right corner
    object = getGObjectAt(window, x + 2 * RADIUS, y + 2 * RADIUS);
    if (object != NULL)
    {
        return object;
    }

    // no collision
    return NULL;
}
