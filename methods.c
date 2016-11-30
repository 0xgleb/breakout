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
