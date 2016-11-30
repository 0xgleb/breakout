#The Essay#

The first difference between this to solutions is that I developed the game using *C* and not *C#*.

##*C* vs *C#*##
*C* is a very low level language that requires understanding of how computer works. It's harder to write and debug *C* code comparing to *C#*. But *C* is faster.

###Why *C* is faster than *C#*?###
- *C* compiler generates binary code, when C# compiler generates bytecode which requires a virtual machine for running it. It makes it more portable, but slower.
- *C* doen't have any garbage collection, which makes life of a developer harder and code faster. <br/>
  *"C# programmers after a party don't collect garbage."*
- *C* compiler can embed assembler language to improve speed
- Also *C* doesn't support OOP, so for example runtime polymorphism doesn't take any time.
*C* is much closer to hardware, that makes it much faster than C#.

##Target platform##
SDL for *C#* works fine only with Windows when SDL for *C* works fine only with Linux.

##Code##
###In general###

I used structures for storing different info. I've created a structure called vector that holds 2 integers - x and y value of a vector.

```c
struct vector {
  int x;
  int y;
};
```

Also I have a structure for objects (not like OOP objects). It has links to vectors *velocity* and *coord*, selection and destination rectangles and a texture.
```c
struct object {
  struct vector *velocity;
  struct vector *coord;

  struct SDL_Rect *selection;
  struct SDL_Rect *destination;
  struct SDL_Texture *texture;
};
```

Author of C# code didn't use any custom data types for storing data.

####Arrays vs Linked lists####

Also he uses arrays for storing data about bricks and I use double linked list. Linked lists are better if you want to delete values. <br/>

```c
struct list {
  struct vector *value;
  struct list *prev;
  struct list *next;
};
```
<br/> <br/> <br/>

Let's say we have an array and a linked list of *n* variables of type **T**.
Array will use *n*\*sizeof(**T**) and double linked list will take *n*\*(sizeof(**T**) + (size of link (4 bytes in 32bit systems or 8 bytes in 64bit systems))).<br/>
Elements of a linked list can be anyware in the memory (that's why we need links to next and prev element) and elements of an array should be in a row
(they have adresses from *array* to *array + n\*sizeof(T)*).

###Setup###
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
  SDL_Window *win = SDL_CreateWindow(
                          title,
                          SDL_WINDOWPOS_CENTERED,
                          SDL_WINDOWPOS_CENTERED,
                          WINDOW_WIDTH,
                          WINDOW_HEIGHT, 0);

  if(!win) {
    printf("Error occured while creating window: %s\n", SDL_GetError());
    SDL_Quit();
    return NULL;
  }

  return win;
}

SDL_Renderer* createRenderer(SDL_Window* win)
{
  SDL_Renderer *rend = SDL_CreateRenderer(win, -1,
                        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  if(!rend) {
    printf("Error occured while creating renderer: %s\n", SDL_GetError());
    SDL_DestroyWindow(win);
    SDL_Quit();
    return NULL;
  }

  return rend;
}
```

<br/> <br/> <br/> <br/> <br/> <br/>
<br/> <br/> <br/> <br/> <br/> <br/>
<br/> <br/> <br/> <br/> <br/> <br/> <br/>

```c
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
    printf("Error occured while creating background texture: %s\n",
                                                  SDL_GetError());
    SDL_DestroyRenderer(rend);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 1;
  }

  ...
  return 0;
}
```

###Main game loop###
