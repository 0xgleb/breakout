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
