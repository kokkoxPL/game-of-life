#include "nuklear_config.h"
#include <glad/glad.h>

#include "game_logic.h"
#include "gui_utils.h"

#include "nuklear.h"
#include "nuklear_glfw_gl3.h"
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CELL_SIZE 15

static GLFWwindow *win;
struct nk_context *ctx;
struct nk_colorf bg;
struct nk_glfw glfw = {0};
struct nk_scroll grid_scroll = {0, 0};
static nk_bool use_random = nk_true;
static int w, h;

static double last_sim_time = 0.0;
static _Bool running = 0;
static unsigned int gens = 100;
static unsigned int rows = 1024;
static unsigned int cols = 1024;
static unsigned int use_simd = 0;
static unsigned int interval = 100; // czas w ms

static char rows_buf[16];
static int rows_len = 0;
static char cols_buf[16];
static int cols_len = 0;
static char interval_buf[16];
static int interval_len = 0;

typedef enum { SCREEN_SETTINGS, SCREEN_SIMULATION } AppState;
static AppState current_state = SCREEN_SETTINGS;

int gui_init(int width, int height) {
  if (!glfwInit())
    return 0;
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  win = glfwCreateWindow(width, height, "Game of Life", NULL, NULL);
  if (!win)
    return 0;

  glfwMakeContextCurrent(win);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    return 0;
  }

  ctx = nk_glfw3_init(&glfw, win, NK_GLFW3_INSTALL_CALLBACKS);
  w = width;
  h = height;

  struct nk_font_atlas *atlas;
  nk_glfw3_font_stash_begin(&glfw, &atlas);
  nk_glfw3_font_stash_end(&glfw);

  return 1;
}

int gui_update(uint8_t **game_grid, uint8_t **next_game_grid) {
  if (glfwWindowShouldClose(win))
    return 0;

  glfwGetWindowSize(win, &w, &h);

  glfwPollEvents();
  nk_glfw3_new_frame(&glfw);

  switch (current_state) {
  case SCREEN_SETTINGS:

    if (nk_begin(ctx, "Settings", nk_rect(0, 0, w, h),
                 NK_WINDOW_NO_SCROLLBAR)) {

      nk_layout_row_dynamic(ctx, h * 0.1, 1);
      nk_label(ctx, "Wielkosc planszy", NK_TEXT_CENTERED);

      input_row(ctx, &rows, rows_buf, &rows_len, 1, "Rzedy");
      input_row(ctx, &cols, cols_buf, &cols_len, 1, "Kolumny");
      input_row(ctx, &interval, interval_buf, &interval_len, 100, "Czas");
    }

    nk_layout_row_dynamic(ctx, 30, 3);
    nk_spacing(ctx, 1);
    nk_checkbox_label(ctx, "Uzyj losowosci", &use_random);
    nk_spacing(ctx, 1);

    nk_layout_row_dynamic(ctx, 30, 3);
    nk_spacing(ctx, 1);
    nk_checkbox_label(ctx, "Uzyj simd", &use_simd);
    nk_spacing(ctx, 1);

    nk_layout_row_dynamic(ctx, h * 0.1, 1);

    nk_layout_row_begin(ctx, NK_DYNAMIC, 40, 3);
    {
      nk_layout_row_push(ctx, 0.30f);
      nk_spacing(ctx, 1);

      nk_layout_row_push(ctx, 0.40f);
      if (nk_button_label(ctx, "Zacznij")) {
        printf("%d, %d", rows, cols);

        if (*game_grid)
          free(*game_grid);
        if (*next_game_grid)
          free(*next_game_grid);

        *game_grid = (use_random == nk_true)
                         ? set_random_game_grid(rows, cols, 0.4f)
                         : set_game_grid(rows, cols);
        *next_game_grid = set_game_grid(rows, cols);
        current_state = SCREEN_SIMULATION;
      }

      nk_layout_row_push(ctx, 0.3f);
      nk_spacing(ctx, 1);
    }
    nk_layout_row_end(ctx);

    break;
  case SCREEN_SIMULATION:
    if (nk_begin(ctx, "Game", nk_rect(0, 0, w, h), NK_WINDOW_BORDER)) {
      int pitch = cols + 2;

      nk_layout_row_begin(ctx, NK_DYNAMIC, 0, 2);
      {
        nk_layout_row_push(ctx, 0.20f);
        if (nk_button_label(ctx, "powrót")) {
          current_state = SCREEN_SETTINGS;
          running = 0;
        }

        nk_layout_row_push(ctx, 0.40f);
        if (nk_button_label(ctx, "+1")) {
          next_step(*game_grid, *next_game_grid, rows, cols, use_simd);

          uint8_t *temp = *game_grid;
          *game_grid = *next_game_grid;
          *next_game_grid = temp;
        }

        double current_time = glfwGetTime();
        if (running &&
            current_time - last_sim_time >= (double)interval / 1000.0) {
          if (gens-- > 0) {
            next_step(*game_grid, *next_game_grid, rows, cols, use_simd);

            uint8_t *temp = *game_grid;
            *game_grid = *next_game_grid;
            *next_game_grid = temp;

            last_sim_time = current_time;
          } else {
            running = 0;
            gens = 100;
          }
        }

        nk_layout_row_push(ctx, 0.40f);
        if (nk_button_label(ctx, "start")) {
          running = 1;
        }
      }
      nk_layout_row_end(ctx);

      const float cell = 10.0f;

      float grid_w = ((float)cols * CELL_SIZE);
      float grid_h = ((float)rows * CELL_SIZE);

      nk_layout_row_dynamic(ctx, 250, 1);

      if (nk_group_scrolled_begin(ctx, &grid_scroll, "grid",
                                  NK_WINDOW_BORDER |
                                      NK_WINDOW_SCROLL_AUTO_HIDE)) {
        nk_layout_space_begin(ctx, NK_STATIC, grid_h, 2);

        // Bez tego nie działa poziomy scrollbar
        nk_layout_space_push(ctx, nk_rect(0, 0, grid_w, 1));
        nk_spacing(ctx, 1);

        nk_layout_space_push(ctx, nk_rect(0, 0, grid_w, grid_h));

        struct nk_command_buffer *canvas = nk_window_get_canvas(ctx);
        struct nk_rect bounds = nk_window_get_content_region(ctx);

        if (nk_input_is_mouse_pressed(&ctx->input, NK_BUTTON_LEFT)) {
          struct nk_vec2 mouse = ctx->input.mouse.pos;

          float mx = mouse.x + grid_scroll.x - bounds.x;
          float my = mouse.y + grid_scroll.y - bounds.y;

          if (mx >= 0 && my >= 0 && mx < cols * CELL_SIZE &&
              my < rows * CELL_SIZE) {
            int c = (int)(mx / CELL_SIZE);
            int r = (int)(my / CELL_SIZE);

            int idx = r * cols + c;
            int padded_idx = (r + 1) * pitch + (c + 1);
            (*game_grid)[padded_idx] ^= 1;
          }
        }

        for (int r = 0; r < rows; r++) {
          for (int c = 0; c < cols; c++) {
            int padded_idx = (r + 1) * pitch + (c + 1);
            float x = bounds.x + c * CELL_SIZE - grid_scroll.x;
            float y = bounds.y + r * CELL_SIZE - grid_scroll.y;

            struct nk_rect rect = nk_rect(x, y, CELL_SIZE, CELL_SIZE);

            nk_fill_rect(canvas, rect, 0,
                         (*game_grid)[padded_idx] ? nk_rgb(0, 0, 0)
                                                  : nk_rgb(255, 255, 255));
          }
        }

        for (int r = 0; r <= rows; r++) {
          float y = bounds.y + r * CELL_SIZE - grid_scroll.y;
          nk_stroke_line(canvas, bounds.x - grid_scroll.x, y,
                         bounds.x + cols * CELL_SIZE - grid_scroll.x, y, 1.0f,
                         nk_rgb(100, 100, 100));
        }

        for (int c = 0; c <= cols; c++) {
          float x = bounds.x + c * CELL_SIZE - grid_scroll.x;
          nk_stroke_line(canvas, x, bounds.y - grid_scroll.y, x,
                         bounds.y + rows * CELL_SIZE - grid_scroll.y, 1.0f,
                         nk_rgb(100, 100, 100));
        }

        nk_layout_space_end(ctx);
        nk_group_scrolled_end(ctx);
      }
    }
    break;
  }

  nk_end(ctx);

  glViewport(0, 0, w, h);
  glClear(GL_COLOR_BUFFER_BIT);
  glClearColor(bg.r, bg.g, bg.b, bg.a);

  nk_glfw3_render(&glfw, NK_ANTI_ALIASING_ON, 512 * 1024, 128 * 1024);
  glfwSwapBuffers(win);

  return 1;
}

void gui_cleanup(void) {
  nk_glfw3_shutdown(&glfw);
  glfwTerminate();
}