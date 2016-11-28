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
