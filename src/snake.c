#include "snake.h"

#define WINDOW_HEIGHT 400
#define WINDOW_WIDTH 400

#define CELL_SIZE 20
#define ROWS (WINDOW_HEIGHT / CELL_SIZE)
#define COLS (WINDOW_WIDTH / CELL_SIZE)

typedef struct Snake {
  int x, y;
  struct Snake *next;
} Snake;

typedef struct GameState {
  // Game Setup things
  SDL_Window *window;
  SDL_Renderer *renderer;
  TTF_Font *font;
  SDL_Event event;

  // FIXME: [Refactor?] Game State
  bool running;
  int score;
  Snake snake;
} GameState;

static GameState game = {0};
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

  game.running = true;
}

void handle_events() {
  while (SDL_PollEvent(&game.event)) {
    switch (game.event.type) {
    case SDL_QUIT:
      game.running = false;
      break;
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
  snprintf(score_text, 12, "Score %d", game.score);
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

void render_grid() {
  SDL_SetRenderDrawColor(game.renderer, 60, 60, 60, 255);
  for (int row = 0; row <= ROWS; row++) {
    SDL_RenderDrawLine(game.renderer, 0, row * CELL_SIZE, WINDOW_WIDTH,
                       row * CELL_SIZE);
  }
  for (int col = 0; col <= ROWS; col++) {
    SDL_RenderDrawLine(game.renderer, col * CELL_SIZE, 0, col * CELL_SIZE,
                       WINDOW_HEIGHT);
  }
}

void render() {
  SDL_SetRenderDrawColor(game.renderer, 0, 0, 0, 255);
  SDL_RenderClear(game.renderer);

  render_grid();
  render_score();

  SDL_RenderPresent(game.renderer);
}

int main(void) {

  init_window();
  while (game.running) {
    handle_events();
    render();
  }
  close_window();
  return 0;
}
