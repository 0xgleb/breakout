#Commentry on Coding Essay by ***Gleb Dianov***#

The first difference between these two solutions is that I developed the game using  *C* and not *C#*.

##*C* vs *C#*##
*C* is a very low-level language that requires understanding of how computers work.
It's harder to write and debug  *C* code comparing to *C#*. But *C* works faster.

###Why *C* is faster than *C#*?###
- *C* compiler generates binary code, when C# compiler generates bytecode which requires a virtual machine for running it.
  It makes it more portable, but slower.
- *C* doesn't have any garbage collection, which makes life of a developer harder and code faster. <br/>
  *"C# programmers don't collect garbage after a party."*
- *C* compiler can embed assembler language to improve speed
- Also, *C* doesn't support **OOP**, so for example runtime polymorphism doesn't take any time.
  *C* is much closer to hardware, that makes it much faster than C#.

##Target platform##
SDL for *C#* works fine only with Windows when SDL for *C* works fine only with Linux.

##Code##
###Code structure###
*C#* version of the project is definitely more readable and more structured. Fancy event handlers allow to easily split code to several methods 
without doing it all manually.
My main function looks messy and contains almost everything. I split my code into 3 files: "list.c", "methods.c" and "main.c". 

"list.c" contains a definition of a structure "list", one global variable and several methods for using it.

####"methods.c" contains####
- initSDL:void -> int - initializes sdl and returns status
- createWindow:string -> SDL\_Window\* - creates a new window and returns a pointer to it
- createRenderer: SDL\_Window\* -> SDL\_Renderer\* - creates a renderer using a window and returns pointer to it
- onCollision: (struct list\*, struct object\*) -> void - handles bouncing off a brick

###Storing data###

I used structures for storing different info. I've created a structure called vector that holds 2 integers - x and y value
of a vector.

```c
struct vector {
  int x;
  int y;
};
```

Also, I have a structure for objects (not like **OOP** objects). It has links to vectors *velocity* and *coord*, *selection*
and *destination* rectangles and a *texture*.
```c
struct object {
  struct vector *velocity;
  struct vector *coord;

  struct SDL_Rect *selection;
  struct SDL_Rect *destination;
  struct SDL_Texture *texture;
};
```

The author of C# code didn't use any custom data types for storing data.
Using custom data types is better cause it makes code more readable and it makes it easier to maintain.

<br/>
####Arrays vs Linked lists####

Also, he uses arrays for storing data about bricks and I use double linked list. Linked lists are better if you want to
delete values. <br/>

```c
struct list {
  struct vector *value;
  struct list *prev;
  struct list *next;
};
```
<br/> <br/> <br/>

Let's say we have an array and a linked list with  *n* variables of type **T**.
Array will use *n*\*sizeof(**T**) bytes and double linked list will require 
*n*\*(sizeof(**T**) + (size of link (4 bytes in 32bit systems or 8 bytes in 64bit systems))) bytes.<br/>
Elements of a linked list can be anywhere in the memory (that's why we need links to next and previous element) and elements of an array should be in a row
(they have addresses from *array* to *array + n\*sizeof(T)*).

The requirement for arrays to hold data in a row makes it worse for situations when you need to dynamically add or delete elements from an array. 
Sometimes it can be impossible to add an element to an array without doing memory reallocation for entire array.

All you need to do to add element to a linked list is allocate memory for the next element and store a pointer in the first or the last element of the list.

So deleting elements of an array isn't a nice thing to do. That is why author of  *C#* code used another 2-dimensional array for storing booleans so he knows whether
a brick is deleted or not. Size of a boolean is 1 byte. But all you need to store the data is 1 bit. But alligned memory works faster so people prefer to spend 7 
bits for nothing and get much better perfomance.
So in this case it would be more efficient to use one-dimensional array of integers, using bits of this integers to store data and using bitwise operators to manipulate 
this data.


###Numbers###

It's a bad practice to use a lot of numbers in your code. It's much better to use constants or  `#define ...` for that purposes (even if you use it once).
It makes code much more readable and in case you want to change something you'll have to change it only in one place.

The author of *C#* code used a lot of numbers and has only 2 constants. He stores some constant values in variables, and it's a bad idea cause you can 
accidentally change it and compiler won't complain.
The best thing to do is to create a config file and parse it to get all the settings you need. But none of us used this method.

*C*:
```c
#define WINDOW_WIDTH (640)
#define WINDOW_HEIGHT (480)
#define PLATFORM_X (0)
#define PLATFORM_Y (200)
#define PLATFORM_W (100)
#define PLATFORM_H (30)
#define PLATFORM_EDGE_WIDTH (10)
#define DECELERATION (1.05)
#define BALL_X (160)
#define BALL_Y (200)
#define BALL_R (7.5)
#define BRICK_X (0)
#define BRICK_Y (80)
#define BRICK_W (35)
#define MARGIN (10)
#define NUM_OF_BRICKS (35)
#define SPEED (300)
#define FPS (60)
...
```

*C#* (inside static class Program):
```cpp
const int BRICKS_COLUMNS = 10;
const int BRICKS_ROWS = 5;
...
```
###Flexibility###

The author of *C#* code used 2 constants for storing data about bricks: number of rows and number of columns. This way it's more flexible and it's easier to draw them.
I used only one 'define' for storing number of bricks. I automatically calculate coordinates of bricks when I fill the list.

*C#* (inside 'onInit' function):
```cpp
for(int i = 0; i < BRICKS_COLUMNS; ++i)
  for(int j = 0; j < BRICKS_ROWS; ++j) {
    brick_sprite[i, j] = (Sprite) rnd.Next(0, 3);
    brick_x[i, j] = 20 + (i * (brick_width + brick_gap_width));
    brick_x[i, j] = 80 + (j * (brick_height + brick_gap_height));
    brick_visible[i, j] = true;
  }
```

*C* (inside 'main' function):
```c
for(bricksLeft = 0; bricksLeft < NUM_OF_BRICKS - 1; bricksLeft++) {
  int y = current -> value -> y;
  int x = current -> value -> x + MARGIN + BRICK_W;

  if(x + 2 * MARGIN + BRICK_W > WINDOW_WIDTH) {
    x = MARGIN;
    y = y + MARGIN + BRICK_W;
  }

  current = add(current, x, y);
}
```

But I have one-dimensional list and *C#* code uses 2 2-dimensional arrays for storing coordinates and another 2-dimensional array for checking whether brick exists. <br/>
Even with 3 2-dimensional arrays the author of *C#* code used less memory than I. <br/> <br/>
*C#*: 2 \* n \* sizeof(int) + n \* sizeof(bool) <br/>
< <br/>
*C*: 3 \* n \* sizeof(void *) + n \* sizeof(struct vector) <br/>

*C#*:
```cpp
static void drawBricks() {
  for(int i = 0; i < BRICKS_COLUMNS; ++i) {
    for(int j = 0; i < BRICKS_ROWS; ++j) {
      if(brick_visible[i, j]) {
        drawSprite(brick_sprite[i, j], brick_x[i, j], brick_y[i, j]);
      }
    }
  }
}
```

<br/> <br/><br/> <br/>
*C*:
```c
void renderBricks(struct SDL_Renderer *rend, 
                    struct object *brick, struct list *current)
{
  while(current) {
    brick -> destination -> x = current -> value -> x;
    brick -> destination -> y = current -> value -> y;

    SDL_RenderCopy(rend, brick -> texture, 
                    brick -> selection, brick -> destination);
      
    current = current -> next;
  }
}
```

###Bat behaviour###

I use keyboard for controlling bat, instead of mouse. A player can use 'a' or '<-' to move left or 'd' or '->' to move right.

```c
while(SDL_PollEvent(event)) {
  switch(event -> type) {
    case SDL_QUIT:
      close_requested = 1;
      break;
    case SDL_KEYDOWN:
      switch(event -> key.keysym.scancode) {
        case SDL_SCANCODE_D:
        case SDL_SCANCODE_RIGHT:
          right = 1;
          break;
        case SDL_SCANCODE_A:
        case SDL_SCANCODE_LEFT:
          left = 1;
          break;
        default:
          break;
      }
      break;
    case SDL_KEYUP:
      switch(event -> key.keysym.scancode) {
        case SDL_SCANCODE_D:
        case SDL_SCANCODE_RIGHT:
          right = 0;
          break;
        case SDL_SCANCODE_A:
        case SDL_SCANCODE_LEFT:
          left = 0;
          break;
        default:
          break;
      }
      break;
    default:
      break;
  }
}
```

In my version of breakout when user a clicks a button I don't change the velocity of the bat, I change acceleration. 
You can't go faster than '2*SPEED' and if you release the button bat will get a nice deceleration.

<br/>

```c
bat -> velocity -> y = right? left? 0 : 8 * SPEED : left? - 8 * SPEED : 0;

if(bat -> velocity -> x < SPEED * 2 && right)
  bat -> velocity -> x += bat -> velocity -> y / FPS;
else if(bat -> velocity -> x > - SPEED * 2 && left)
  bat -> velocity -> x += bat -> velocity -> y / FPS;

if(!(right ^ left))
  bat -> velocity -> x /= DECELERATION;

bat -> coord -> x += bat -> velocity -> x / FPS;

if(bat -> coord -> x < 0) {
  bat -> coord -> x = 0;
  bat -> velocity -> x = - bat -> velocity -> x;
}
else if(bat -> coord -> x + BAT_W > WINDOW_WIDTH) {
  bat -> coord -> x = WINDOW_WIDTH - BAT_W;
  bat -> velocity -> x = - bat -> velocity -> x;
}

bat -> destination -> x = bat -> coord -> x;
```

###Ball behaviour###

When the ball bounces off the bat I change it's x-velocity by adding fraction of the bat's velocity.
This way you can change the velocity vector of the ball and get more control over the ball.

```c
if(ball -> coord -> y <= 0) {
  ball -> coord -> y = 0;
  ball -> velocity -> y = - ball -> velocity -> y;
}
else if(ball -> coord -> y >= bat -> coord -> y - 2 * BALL_R && bat -> coord -> x <= ball -> coord -> x
  && ball -> coord -> x <= bat -> coord -> x + BAT_W) {

  ball -> velocity -> y = - ball -> velocity -> y;
  ball -> velocity -> x += bat -> velocity -> x / 4;
}
```

###Collisions###

Detecting collisions is a simple task. All you need to do is check whether x-coordinate of the ball is between x-coordinate of a brick - width of the ball and
x-coordinate of + width of a brick and similar thing for the y-coordinate.

Function `findIntersections` takes the coordinates of a ball and returns NULL if ball doesn't intersect any bricks or returns pointer to list element with a brick otherwise.

```c
struct list* findIntersections(struct vector *ball) 
{
  struct list *current = bricks_coords;
  while(current) {
    if(current -> value -> x - 2 * BALL_R <= ball -> x && ball -> x <= current -> value -> x + BRICK_W)
      if(current -> value -> y - 2 * BALL_R <= ball -> y && ball -> y <= current -> value -> y + BRICK_W)
        return current;
    current = current -> next;
  }
  return NULL;
}
```

Programming behaviour that updates the ball's velocity and deletes bricks is much harder. I did some maths and wrote a function `onCollision` that looks like that:

```c
void onCollision(struct list *current, struct object *ball) 
{
  int dx, dy;

  if(current -> value -> x + BRICK_W / 2 < ball -> coord -> x && ball -> coord -> x <= current -> value -> x + BRICK_W)
    dx = current -> value -> x + BRICK_W - ball -> coord -> x;
  else 
    dx = current -> value -> x - (ball -> coord -> x + 2 * BALL_R);

  if(current -> value -> y + BRICK_W / 2 < ball -> coord -> y && ball -> coord -> y <= current -> value -> y + BRICK_W)
    dy = current -> value -> y + BRICK_W - ball -> coord -> y;
  else
    dy = current -> value -> y - (ball -> coord -> y + 2 * BALL_R);

  if(abs(dx) > abs(dy)) {
    if(dx > 0)
      ball -> coord -> x = current -> value -> x + BRICK_W;
    else
      ball -> coord -> x = current -> value -> x;

    ball -> velocity -> y = - ball -> velocity -> y;
  }
  else if(abs(dx) < abs(dy)) {
    if(dy > 0)
      ball -> coord -> y = current -> value -> y + BRICK_W;
    else
      ball -> coord -> y = current -> value -> y;

    ball -> velocity -> x = - ball -> velocity -> x;
  }
  else {
    if(dx > 0)
      ball -> coord -> x = current -> value -> y + BRICK_W;
    else
      ball -> coord -> x = current -> value -> x;

    if(dy > 0)
      ball -> coord -> y = current -> value -> y + BRICK_W;
    else
      ball -> coord -> y = current -> value -> y;

    ball -> velocity -> x = - ball -> velocity -> x;
    ball -> velocity -> y = - ball -> velocity -> y;
  }

  del(current -> value);
}
```

This function has a bug, but unfortunately I can't find it. But in 90% of situations it works fine.

This behaviour works badly in *C#* version of the project so I don't see any reasons to do comparisons.


##$ wc -l $(ls -a | grep "\\.c$")##
Main file in *C#* project ('Program.cs') has 421 lines of code. I have 486 lines of code in total.
I had to write more code cause I chose *C* for developing and it has less built in functions, so result is pretty much the same.
Both  *C#* and *C* version of code are compact and readable.

But sometimes the author of *C#* code uses unnecessary lines of code. 
For example function 'drawBricks'
```cpp
static void drawBricks() {
  for(int i = 0; i < BRICKS_COLUMNS; ++i) {
    for(int j = 0; i < BRICKS_ROWS; ++j) {
      if(brick_visible[i, j]) {
        drawSprite(brick_sprite[i, j], brick_x[i, j], brick_y[i, j]);
      }
    }
  }
}
```

It could be shorter and a little more readable
```cpp
static void drawBricks() {
  for(int i = 0; i < BRICKS_COLUMNS; ++i)
    for(int j = 0; i < BRICKS_ROWS; ++j)
      if(brick_visible[i, j])
        drawSprite(brick_sprite[i, j], brick_x[i, j], brick_y[i, j]);
}
```

##Game play experience##

My code has several bugs and sometimes wierd behaviour can be seen when the ball hits a brick. <br/>

####*C#* code has several problems####
- You can't win or loose. The ball bounces off the bottom and after there are no bricks left the game doesn't stop.
- Sometimes the ball bounces off the bricks incorrectly.
- The ball sometimes goes throw the bricks.

####In my version ####
- You can win or loose
- Bouncing behaviour works better
- There is nice behaviour when the ball bounces off the bat
- Controls are better (in my opinion)
- Has several small bugs

<br/>

##SRC##

**list.c**: 
```c
struct list {
  struct vector *value;
  struct list *prev;
  struct list *next;
};

struct list *bricks_coords = NULL;

struct list* add(struct list *start, int x, int y)
{
  struct vector *value = (struct vector *) malloc(sizeof(struct vector));
  value -> x = x;
  value -> y = y;

  if(!start -> value) {
    start -> value = value;
    return start;
  }

  struct list *current = start;
  while(current -> next)
    current = current -> next;

  current -> next = (struct list *) malloc(sizeof(struct list));
  current -> next -> value = value;
  current -> next -> prev = current;
  current -> next -> next = NULL;

  return current -> next;
}

struct list* findIntersections(struct vector *ball) 
{
  struct list *current = bricks_coords;

  while(current) {
    if(current -> value -> x - 2 * BALL_R <= ball -> x && ball -> x <= current -> value -> x + BRICK_W)
      if(current -> value -> y - 2 * BALL_R <= ball -> y && ball -> y <= current -> value -> y + BRICK_W)
        return current;

    current = current -> next;
  }

  return NULL;
}

void del(struct vector *value)
{
  if(bricks_coords -> value == value) {
    struct list *next = bricks_coords -> next;
    free(bricks_coords -> value);
    free(bricks_coords);

    bricks_coords = next;

    return;
  }

  struct list *current = bricks_coords;

  while(current) {
    if(current -> value == value) {
      if(current -> prev)
        current -> prev -> next = current -> next;

      if(current -> next)
        current -> next -> prev = current -> prev;

      free(current -> value);
      free(current);

      return;
    }

    current = current -> next;
  }

  printf("ERROR: value not found!\n");
}
```

**methods.c**:
```c
int initSDL()
{
  if(SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER)) {
    printf("Error occured while initializing SDL: %s\n", SDL_GetError());
    return 1;
  }

  return 0;
}

SDL_Window* createWindow(char *title)
{
  SDL_Window *win = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
  if(!win) {
    printf("Error occured while creating window: %s\n", SDL_GetError());
    SDL_Quit();
    return NULL;
  }

  return win;
}

SDL_Renderer* createRenderer(SDL_Window* win)
{
  SDL_Renderer *rend = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  if(!rend) {
    printf("Error occured while creating renderer: %s\n", SDL_GetError());
    SDL_DestroyWindow(win);
    SDL_Quit();
    return NULL;
  }

  return rend;
}


void onCollision(struct list *current, struct object *ball) 
{
  int dx, dy;

  if(current -> value -> x + BRICK_W / 2 < ball -> coord -> x && ball -> coord -> x <= current -> value -> x + BRICK_W)
    dx = current -> value -> x + BRICK_W - ball -> coord -> x;
  else 
    dx = current -> value -> x - (ball -> coord -> x + 2 * BALL_R);

  if(current -> value -> y + BRICK_W / 2 < ball -> coord -> y && ball -> coord -> y <= current -> value -> y + BRICK_W)
    dy = current -> value -> y + BRICK_W - ball -> coord -> y;
  else
    dy = current -> value -> y - (ball -> coord -> y + 2 * BALL_R);

  if(abs(dx) > abs(dy)) {
    if(dx > 0)
      ball -> coord -> x = current -> value -> x + BRICK_W;
    else
      ball -> coord -> x = current -> value -> x;

    ball -> velocity -> y = - ball -> velocity -> y;
  }
  else if(abs(dx) < abs(dy)) {
    if(dy > 0)
      ball -> coord -> y = current -> value -> y + BRICK_W;
    else
      ball -> coord -> y = current -> value -> y;

    ball -> velocity -> x = - ball -> velocity -> x;
  }
  else {
    if(dx > 0)
      ball -> coord -> x = current -> value -> y + BRICK_W;
    else
      ball -> coord -> x = current -> value -> x;

    if(dy > 0)
      ball -> coord -> y = current -> value -> y + BRICK_W;
    else
      ball -> coord -> y = current -> value -> y;

    ball -> velocity -> x = - ball -> velocity -> x;
    ball -> velocity -> y = - ball -> velocity -> y;
  }

  del(current -> value);
}

void renderBricks(struct SDL_Renderer *rend, struct object *brick, struct list *current)
{
  while(current) {
    brick -> destination -> x = current -> value -> x;
    brick -> destination -> y = current -> value -> y;

    SDL_RenderCopy(rend, brick -> texture, brick -> selection, brick -> destination);
      
    current = current -> next;
  }
}
```

**main.c**:
```c
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>

#include <time.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_image.h>

// picture width is 640px
#define WINDOW_WIDTH (640)
// picture height is 640px
#define WINDOW_HEIGHT (480)

#define BAT_X (0)
#define BAT_Y (200)
#define BAT_W (100)
#define BAT_H (30)
#define BAT_EDGE_WIDTH (10)
#define DECELERATION (1.05)

#define BALL_X (160)
#define BALL_Y (200)
#define BALL_R (7.5)

#define BRICK_X (0)
#define BRICK_Y (80)
#define BRICK_W (35)

#define MARGIN (10)

#define NUM_OF_BRICKS (35)

// pixels per second
#define SPEED (300)

#define FPS (60)

struct vector {
  int x;
  int y;
};

struct object {
  struct vector *velocity;
  struct vector *coord;

  struct SDL_Rect *selection;
  struct SDL_Rect *destination;
  struct SDL_Texture *texture;
};

#include "list.c"
#include "methods.c"

int main()
{
  if(initSDL()) return 1;

  SDL_Window *win = createWindow("Breakout");
  if(!win) return 1;

  SDL_Renderer *rend = createRenderer(win);
  if(!rend) return 1;

  SDL_Surface *surface = IMG_Load("images/breakout_bg.png");
  if(!surface) {
    printf("Error occured while creating surface: %s\n", SDL_GetError());
    SDL_DestroyRenderer(rend);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 1;
  }

  struct object *bcg = (struct object *) malloc(sizeof(struct object));
  bcg -> selection = NULL;
  bcg -> destination = NULL;

  bcg -> texture = SDL_CreateTextureFromSurface(rend, surface);
  SDL_FreeSurface(surface);
  if(!bcg -> texture) {
    printf("Error occured while creating background texture: %s\n", SDL_GetError());
    SDL_DestroyRenderer(rend);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 1;
  }

  surface = IMG_Load("images/breakout_sprites.png");
  if(!surface) {
    printf("Error occured while creating second sprite: %s\n", SDL_GetError());
    SDL_DestroyTexture(bcg -> texture);
    SDL_DestroyRenderer(rend);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 1;
  }

  struct object *bat = (struct object *) malloc(sizeof(struct object));
  bat -> texture = SDL_CreateTextureFromSurface(rend, surface);
  SDL_FreeSurface(surface);

  if(!bat -> texture) {
    printf("Error occured while creating sprite texture: %s\n", SDL_GetError());
    SDL_DestroyTexture(bcg -> texture);
    SDL_DestroyRenderer(rend);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 1;
  }

  bat -> selection = (struct SDL_Rect *) malloc(sizeof(struct SDL_Rect));
  bat -> selection -> x = BAT_X;
  bat -> selection -> y = BAT_Y;
  bat -> selection -> w = BAT_W;
  bat -> selection -> h = BAT_H;
  bat -> destination = (struct SDL_Rect*) malloc(sizeof(struct SDL_Rect));
  bat -> destination -> x = (WINDOW_WIDTH - bat -> selection -> w) / 2;
  bat -> destination -> y = WINDOW_HEIGHT - bat -> selection -> h;
  bat -> destination -> w = bat -> selection -> w;
  bat -> destination -> h = bat -> selection -> h;
  bat -> coord = (struct vector *) malloc(sizeof(struct vector));
  bat -> coord -> x = bat -> destination -> x;
  bat -> coord -> y = bat -> destination -> y;
  bat -> velocity = (struct vector *) malloc(sizeof(struct vector));
  bat -> velocity -> x = 0;
  bat -> velocity -> y = 0; // use y velocity for storing acceleration

  struct object *ball = (struct object *) malloc(sizeof(struct object));
  ball -> texture = bat -> texture;
  ball -> selection = (struct SDL_Rect *) malloc(sizeof(struct SDL_Rect));
  ball -> selection -> x = BALL_X;
  ball -> selection -> y = BALL_Y;
  ball -> selection -> w = (int) (BALL_R * 2);
  ball -> selection -> h = (int) (BALL_R * 2);
  ball -> destination = (struct SDL_Rect *) malloc(sizeof(struct SDL_Rect));
  ball -> destination -> x = (WINDOW_WIDTH - ball -> selection -> w) / 2;
  ball -> destination -> y = bat -> destination -> y - bat -> destination -> h;
  ball -> destination -> h = ball -> selection -> h;
  ball -> destination -> w = ball -> selection -> w;
  ball -> coord = (struct vector *) malloc(sizeof(struct vector));
  ball -> coord -> x = ball -> destination -> x;
  ball -> coord -> y = ball -> destination -> y;
  ball -> velocity = (struct vector *) malloc(sizeof(struct vector));
  ball -> velocity -> x = SPEED;
  ball -> velocity -> y = - SPEED;

  struct object *brick = (struct object *) malloc(sizeof(struct object));
  brick -> texture = bat -> texture;
  brick -> selection = (struct SDL_Rect *) malloc(sizeof(struct SDL_Rect));
  brick -> selection -> x = BRICK_X;
  brick -> selection -> y = BRICK_Y;
  brick -> selection -> w = BRICK_W;
  brick -> selection -> h = BRICK_W;
  brick -> destination = (struct SDL_Rect *) malloc(sizeof(struct SDL_Rect));
  brick -> destination -> x = 0;
  brick -> destination -> y = 0;
  brick -> destination -> w = BRICK_W;
  brick -> destination -> h = BRICK_W;

  int bricksLeft;
  bricks_coords = (struct list *) malloc(sizeof(struct list));
  bricks_coords -> value = bricks_coords -> next = bricks_coords -> prev = NULL;
  struct list *current = add(bricks_coords, MARGIN, MARGIN);

  for(bricksLeft = 0; bricksLeft < NUM_OF_BRICKS - 1; bricksLeft++) {
    int y = current -> value -> y;
    int x = current -> value -> x + MARGIN + BRICK_W;

    if(x + 2 * MARGIN + BRICK_W > WINDOW_WIDTH) {
      x = MARGIN;
      y = y + MARGIN + BRICK_W;
    }

    current = add(current, x, y);
  }

  SDL_Event *event = (union SDL_Event *) malloc(sizeof(union SDL_Event));
  bool close_requested = 0;
  bool left = 0, right = 0;
  clock_t begin;

  while(1) {
    begin = clock();
    while(SDL_PollEvent(event)) {
      switch(event -> type) {
        case SDL_QUIT:
          close_requested = 1;
          break;
        case SDL_KEYDOWN:
          switch(event -> key.keysym.scancode) {
            case SDL_SCANCODE_D:
            case SDL_SCANCODE_RIGHT:
              right = 1;
              break;
            case SDL_SCANCODE_A:
            case SDL_SCANCODE_LEFT:
              left = 1;
              break;
            default:
              break;
          }
          break;
        case SDL_KEYUP:
          switch(event -> key.keysym.scancode) {
            case SDL_SCANCODE_D:
            case SDL_SCANCODE_RIGHT:
              right = 0;
              break;
            case SDL_SCANCODE_A:
            case SDL_SCANCODE_LEFT:
              left = 0;
              break;
            default:
              break;
          }
          break;
        default:
          break;
      }
    }

    if(ball -> coord -> y >= WINDOW_HEIGHT - BAT_H / 2) {
      printf("Game over!\n");
      break;
    }
    if(close_requested)
      break;

    if(ball -> coord -> y <= 0) {
      ball -> coord -> y = 0;
      ball -> velocity -> y = - ball -> velocity -> y;
    }
    else if(ball -> coord -> y >= bat -> coord -> y - 2 * BALL_R && bat -> coord -> x <= ball -> coord -> x
    && ball -> coord -> x <= bat -> coord -> x + BAT_W) {
      if(bat -> coord -> x - 2 * BALL_R <= ball -> coord -> x
      && ball -> coord -> x <= bat -> coord -> x - 2 * BALL_R + BAT_EDGE_WIDTH) {
        printf("opa\n");
      }
      else {
        ball -> velocity -> y = - ball -> velocity -> y;
        ball -> velocity -> x += bat -> velocity -> x / 4;
      }
    }

    if(ball -> coord -> x <= 0) {
      ball -> coord -> x = 0;
      ball -> velocity -> x = - ball -> velocity -> x;
    }
    else if(ball -> coord -> x >= WINDOW_WIDTH - 2 * BALL_R) {
      ball -> coord -> x = WINDOW_WIDTH - 2 * BALL_R;
      ball -> velocity -> x = - ball -> velocity -> x;
    }

    current = findIntersections(ball -> coord);
    if(current) {
      onCollision(current, ball);
      bricksLeft--;
    }

    ball -> coord -> x += ball -> velocity -> x / FPS;
    ball -> coord -> y += ball -> velocity -> y / FPS;

    ball -> destination -> x = ball -> coord -> x;
    ball -> destination -> y = ball -> coord -> y;

    bat -> velocity -> y = right? left? 0 : 8 * SPEED : left? - 8 * SPEED : 0;

    if(bat -> velocity -> x < SPEED * 2 && right)
      bat -> velocity -> x += bat -> velocity -> y / FPS;
    else if(bat -> velocity -> x > - SPEED * 2 && left)
      bat -> velocity -> x += bat -> velocity -> y / FPS;

    if(!(right ^ left))
      bat -> velocity -> x /= DECELERATION;

    bat -> coord -> x += bat -> velocity -> x / FPS;

    if(bat -> coord -> x < 0) {
      bat -> coord -> x = 0;
      bat -> velocity -> x = - bat -> velocity -> x;
    }
    else if(bat -> coord -> x + BAT_W > WINDOW_WIDTH) {
      bat -> coord -> x = WINDOW_WIDTH - BAT_W;
      bat -> velocity -> x = - bat -> velocity -> x;
   }

    bat -> destination -> x = bat -> coord -> x;

    SDL_RenderClear(rend);

    SDL_RenderCopy(rend, bcg -> texture, bcg -> selection, bcg -> destination);
    SDL_RenderCopy(rend, bat -> texture, bat -> selection, bat -> destination);
    SDL_RenderCopy(rend, ball -> texture, ball -> selection, ball -> destination);

    if(!bricksLeft) {
      printf("You won!\n");
      break;
    }

    current = bricks_coords;
    renderBricks(rend, brick, current);

    SDL_RenderPresent(rend);

    SDL_Delay(1000 / FPS + (int) ((begin - clock()) / CLOCKS_PER_SEC) * 1000);
  }

  SDL_DestroyTexture(bat -> texture);
  SDL_DestroyTexture(bcg -> texture);
  SDL_DestroyRenderer(rend);
  SDL_DestroyWindow(win);
  SDL_Quit();

  return 0;
}
```
