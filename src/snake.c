#include "snake.h"

#define WINDOW_HEIGHT 540
#define WINDOW_WIDTH 960

#define GRID_DIM 400
#define GRID_SIZE 20
#define CELL_SIZE (GRID_DIM / GRID_SIZE)
#define GRID_X (WINDOW_WIDTH / 2) - (GRID_DIM / 2)
#define GRID_Y (WINDOW_HEIGHT / 2) - (GRID_DIM / 2)

enum {
  UP,
  DOWN,
  LEFT,
  RIGHT,
};

typedef struct Snake {
  unsigned int x, y;
  int dir;

  struct Snake *next;
} Snake;

typedef struct GameSetup {
  SDL_Window *window;
  SDL_Renderer *renderer;
  TTF_Font *font;
} GameSetup;
GameSetup game = {0};

typedef struct GameState {
  bool running;
  int score;
  Snake *snake_head;
  Snake *snake_tail;
  int fruit[2];
} GameState;
GameState state = {0};

void init_snake(void) {
  Snake *segment = malloc(sizeof(Snake));
  segment->x = rand() % (GRID_SIZE / 2) + (GRID_SIZE / 4);
  segment->y = rand() % (GRID_SIZE / 2) + (GRID_SIZE / 4);
  segment->dir = UP;
  segment->next = NULL;

  state.snake_head = segment;
  state.snake_tail = segment;
}

void increase_snake(void) {
  Snake *segment = malloc(sizeof(Snake));
  segment->x = state.snake_tail->x;
  segment->y = state.snake_tail->y - 1;
  segment->dir = state.snake_tail->dir;
  segment->next = NULL;

  state.snake_tail->next = segment;
  state.snake_tail = segment;
}

void move_snake(void) {
  int prev_x = state.snake_head->x;
  int prev_y = state.snake_head->y;

  switch (state.snake_head->dir) {
  case UP:
    state.snake_head->y--;
    break;
  case DOWN:
    state.snake_head->y++;
    break;
  case LEFT:
    state.snake_head->x--;
    break;
  case RIGHT:
    state.snake_head->x++;
    break;
  default:
    break;
  }

  Snake *track = state.snake_head;
  if (track->next != NULL) {
    track = track->next;
  }

  while (track != NULL) {
    int x = track->x;
    int y = track->y;

    track->x = prev_x;
    track->y = prev_y;

    track = track->next;

    prev_x = x;
    prev_y = y;
  }
}

static const SDL_Color WHITE = {255, 255, 255, 255};
// static const SDL_Color BLACK = {0, 0, 0, 255};
// static const SDL_Color RED = {255, 0, 0, 255};
// static const SDL_Color GREEN = {0, 255, 0, 255};
// static const SDL_Color BLUE = {0, 0, 255, 255};

void init_window() {
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    fprintf(stderr, "Error: SDL failed to initialise: %s\n", SDL_GetError());
    exit(1);
  }

  if (TTF_Init() < 0) {
    fprintf(stderr, "Error: error initialising TTF: %s\n", SDL_GetError());
    exit(1);
  }

  game.window = SDL_CreateWindow("Snake", 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 0);

  if (!game.window) {
    fprintf(stderr, "Error: SDL Window initialization failed: %s\n",
            SDL_GetError());
  }

  game.renderer = SDL_CreateRenderer(game.window, -1, SDL_RENDERER_ACCELERATED);

  state.running = true;
}

void handle_events() {
  SDL_Event event;

  while (SDL_PollEvent(&event)) {
    switch (event.type) {
    case SDL_QUIT:
      state.running = false;
      break;
    case SDL_KEYDOWN:
      switch (event.key.keysym.sym) {
      case SDLK_ESCAPE:
        state.running = false;
        break;

      case SDLK_w:
        if (state.snake_head->dir != DOWN)
          state.snake_head->dir = UP;
        break;
      case SDLK_s:
        if (state.snake_head->dir != UP)
          state.snake_head->dir = DOWN;
        break;
      case SDLK_a:
        if (state.snake_head->dir != RIGHT)
          state.snake_head->dir = LEFT;
        break;
      case SDLK_d:
        if (state.snake_head->dir != LEFT)
          state.snake_head->dir = RIGHT;
        break;

      default:
        break;
      }
    default:
      break;
    }
  }
}

void close_window() {
  SDL_DestroyRenderer(game.renderer);
  SDL_DestroyWindow(game.window);
  SDL_Quit();
}

void render_score() {
  if (!game.font) {

    game.font = TTF_OpenFont("./resources/fonts/square-deal.ttf", 22);
    if (!game.font) {
      fprintf(stderr, "Error: Loading font failed: %s\n", SDL_GetError());
    }
  }

  char score_text[12];
  snprintf(score_text, 12, "Score %d", state.score);
  SDL_Surface *score_text_surface =
      TTF_RenderText_Solid(game.font, score_text, WHITE);

  SDL_Texture *score_texture =
      SDL_CreateTextureFromSurface(game.renderer, score_text_surface);

  SDL_Rect score_rect = {.x = WINDOW_WIDTH - score_text_surface->w - 10,
                         .y = 10,
                         .w = score_text_surface->w,
                         .h = score_text_surface->h};
  SDL_RenderCopy(game.renderer, score_texture, NULL, &score_rect);

  SDL_DestroyTexture(score_texture);
  SDL_FreeSurface(score_text_surface);
}

void render_grid(int grid_x, int grid_y) {
  SDL_SetRenderDrawColor(game.renderer, 40, 40, 40, 255);

  SDL_Rect cell = {.w = CELL_SIZE, .h = CELL_SIZE};

  for (int i = 0; i <= GRID_SIZE; i++) {
    for (int j = 0; j <= GRID_SIZE; j++) {
      cell.x = grid_x + (i * CELL_SIZE);
      cell.y = grid_y + (j * CELL_SIZE);
      SDL_RenderDrawRect(game.renderer, &cell);
    }
  }
}

void render_snake(int grid_x, int grid_y) {
  SDL_SetRenderDrawColor(game.renderer, 0x00, 0xff, 0x00, 255);

  SDL_Rect segment = {
      .w = CELL_SIZE,
      .h = CELL_SIZE,
  };

  Snake *track = state.snake_head;
  while (track != NULL) {
    segment.x = grid_x + track->x * CELL_SIZE;
    segment.y = grid_y + track->y * CELL_SIZE;
    // TODO: Prevent snake from going off-grid
    track = track->next;
    SDL_RenderFillRect(game.renderer, &segment);
  }
}

void generate_random_fruit() {
  state.fruit[0] = rand() % (GRID_SIZE);
  state.fruit[1] = rand() % (GRID_SIZE);
}

void render_fruit() {
  SDL_SetRenderDrawColor(game.renderer, 0xff, 0x00, 0x00, 255);

  SDL_Rect segment = {
      .x = GRID_X + state.fruit[0] * CELL_SIZE,
      .y = GRID_Y + state.fruit[1] * CELL_SIZE,
      .w = CELL_SIZE,
      .h = CELL_SIZE,
  };
  SDL_RenderFillRect(game.renderer, &segment);
}

void render(void) {
  SDL_SetRenderDrawColor(game.renderer, 0, 0, 0, 255);
  SDL_RenderClear(game.renderer);

  render_grid(GRID_X, GRID_Y);
  render_snake(GRID_X, GRID_Y);
  render_fruit();
  render_score();

  SDL_RenderPresent(game.renderer);
}

int main(void) {
  srand(time(0));
  init_snake();
  increase_snake();
  init_window();
  generate_random_fruit();
  while (state.running) {
    handle_events();
    move_snake();
    render();
    SDL_Delay(100);
  }
  close_window();
  return 0;
}
