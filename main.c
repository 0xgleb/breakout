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

  struct object *platform = (struct object *) malloc(sizeof(struct object));
  platform -> texture = SDL_CreateTextureFromSurface(rend, surface);
  SDL_FreeSurface(surface);

  if(!platform -> texture) {
    printf("Error occured while creating sprite texture: %s\n", SDL_GetError());
    SDL_DestroyTexture(bcg -> texture);
    SDL_DestroyRenderer(rend);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 1;
  }

  platform -> selection = (struct SDL_Rect *) malloc(sizeof(struct SDL_Rect));
  platform -> selection -> x = PLATFORM_X;
  platform -> selection -> y = PLATFORM_Y;
  platform -> selection -> w = PLATFORM_W;
  platform -> selection -> h = PLATFORM_H;
  platform -> destination = (struct SDL_Rect*) malloc(sizeof(struct SDL_Rect));
  platform -> destination -> x = (WINDOW_WIDTH - platform -> selection -> w) / 2;
  platform -> destination -> y = WINDOW_HEIGHT - platform -> selection -> h;
  platform -> destination -> w = platform -> selection -> w;
  platform -> destination -> h = platform -> selection -> h;
  platform -> coord = (struct vector *) malloc(sizeof(struct vector));
  platform -> coord -> x = platform -> destination -> x;
  platform -> coord -> y = platform -> destination -> y;
  platform -> velocity = (struct vector *) malloc(sizeof(struct vector));
  platform -> velocity -> x = 0;
  platform -> velocity -> y = 0; // use y velocity for storing acceleration
  
  struct object *ball = (struct object *) malloc(sizeof(struct object));
  ball -> texture = platform -> texture;
  ball -> selection = (struct SDL_Rect *) malloc(sizeof(struct SDL_Rect));
  ball -> selection -> x = BALL_X;
  ball -> selection -> y = BALL_Y;
  ball -> selection -> w = (int) (BALL_R * 2);
  ball -> selection -> h = (int) (BALL_R * 2);
  ball -> destination = (struct SDL_Rect *) malloc(sizeof(struct SDL_Rect));
  ball -> destination -> x = (WINDOW_WIDTH - ball -> selection -> w) / 2;
  ball -> destination -> y = platform -> destination -> y - platform -> destination -> h;
  ball -> destination -> h = ball -> selection -> h;
  ball -> destination -> w = ball -> selection -> w;
  ball -> coord = (struct vector *) malloc(sizeof(struct vector));
  ball -> coord -> x = ball -> destination -> x;
  ball -> coord -> y = ball -> destination -> y;
  ball -> velocity = (struct vector *) malloc(sizeof(struct vector));
  ball -> velocity -> x = SPEED;
  ball -> velocity -> y = - SPEED;

  struct object *brick = (struct object *) malloc(sizeof(struct object));
  brick -> texture = platform -> texture;
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

    if(ball -> coord -> y >= WINDOW_HEIGHT - PLATFORM_H / 2) {
      printf("Game over!\n");
      break;
    }
    if(close_requested) 
      break;

    if(ball -> coord -> y <= 0) {
      ball -> coord -> y = 0;
      ball -> velocity -> y = - ball -> velocity -> y;
    }
    else if(ball -> coord -> y >= platform -> coord -> y - 2 * BALL_R && platform -> coord -> x <= ball -> coord -> x 
    && ball -> coord -> x <= platform -> coord -> x + PLATFORM_W) {
      if(platform -> coord -> x - 2 * BALL_R <= ball -> coord -> x 
      && ball -> coord -> x <= platform -> coord -> x - 2 * BALL_R + PLATFORM_EDGE_WIDTH) {
        printf("opa\n");
      }
      else {
        ball -> velocity -> y = - ball -> velocity -> y;
        ball -> velocity -> x += platform -> velocity -> x / 4;
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

    platform -> velocity -> y = right? left? 0 : 8 * SPEED : left? - 8 * SPEED : 0;

    if(platform -> velocity -> x < SPEED * 2 && right)
      platform -> velocity -> x += platform -> velocity -> y / FPS;
    else if(platform -> velocity -> x > - SPEED * 2 && left)
      platform -> velocity -> x += platform -> velocity -> y / FPS;

    if(!(right ^ left))
      platform -> velocity -> x /= DECELERATION;

    platform -> coord -> x += platform -> velocity -> x / FPS;

    if(platform -> coord -> x < 0) {
      platform -> coord -> x = 0;
      platform -> velocity -> x = - platform -> velocity -> x;
    }
    else if(platform -> coord -> x + PLATFORM_W > WINDOW_WIDTH) {
      platform -> coord -> x = WINDOW_WIDTH - PLATFORM_W;
      platform -> velocity -> x = - platform -> velocity -> x;
   }

    platform -> destination -> x = platform -> coord -> x;

    SDL_RenderClear(rend);

    SDL_RenderCopy(rend, bcg -> texture, bcg -> selection, bcg -> destination);
    SDL_RenderCopy(rend, platform -> texture, platform -> selection, platform -> destination);
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

  SDL_DestroyTexture(platform -> texture);
  SDL_DestroyTexture(bcg -> texture);
  SDL_DestroyRenderer(rend);
  SDL_DestroyWindow(win);
  SDL_Quit();

  return 0;
}
