#define SDL_MAIN_HANDLED

#include <stdio.h>
#include <stdbool.h>
#include <math.h>

#include "display.h"
#include "vec2.h"

bool is_running = false;

vec2_t vertices[4] = {
  {.x = 40, .y = 40},
  {.x = 80, .y = 40},
  {.x = 40, .y = 80},
  {.x = 90, .y = 90},
};

typedef struct {
  uint8_t r;
  uint8_t g;
  uint8_t b;
} color_t;

color_t colors[3] = {
  {.r = 0xff, .g = 0x00, .b = 0x00 },
  {.r = 0x00, .g = 0xff, .b = 0x00 },
  {.r = 0x00, .g = 0x00, .b = 0xff }
};

typedef struct {
  vec2_t pos;
  vec2_t dir;
} player_t;

player_t player = {
  {20, 20},
  {0, -2},
};

bool fill_rule = true;
bool draw_1 = true;
bool draw_2 = true;
bool draw_3 = true;

const float PI = 3.1415926f;

vec2_t origin = { 0, 0 };
//vec2_t origin = {(float)SCREEN_WIDTH / 2.0f, (float)SCREEN_HEIGHT / 2.0f};

void print_debug() {
  printf("fill_rule: %d  draw_1: %d  draw_2: %d  draw_3: %d\n", fill_rule, draw_1, draw_2, draw_3);
  printf("player: x: %f  y: %f\n", player.pos.x, player.pos.y);
}

void process_input() {
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    switch (event.type) {
      case SDL_QUIT:
        is_running = false;
        break;
      case SDL_KEYDOWN:
        if (event.key.keysym.sym == SDLK_ESCAPE) {
          is_running = false;
        }
        if (event.key.keysym.sym == SDLK_RIGHT) {
          vertices[0].x += 1;
          vertices[1].x += 1;
          vertices[2].x += 1;
        }
        if (event.key.keysym.sym == SDLK_LEFT) {
          vertices[0].x -= 1;
          vertices[1].x -= 1;
          vertices[2].x -= 1;
        }
        if (event.key.keysym.scancode == SDL_SCANCODE_Q) {
          //float rot = -PI / 32.0f;
          // vertices[0] = vec2_rotate(vertices[0], origin, rot);
          // vertices[1] = vec2_rotate(vertices[1], origin, rot);
          // vertices[2] = vec2_rotate(vertices[2], origin, rot);
          printf("%f %f\n", vertices[0].x, vertices[0].y);
        }
        if (event.key.keysym.scancode == SDL_SCANCODE_E) {
          //float rot = PI / 32.0f;
          // vertices[0] = vec2_rotate(vertices[0], origin, rot);
          // vertices[1] = vec2_rotate(vertices[1], origin, rot);
          // vertices[2] = vec2_rotate(vertices[2], origin, rot);
        }
        if (event.key.keysym.scancode == SDL_SCANCODE_F) {
          fill_rule = !fill_rule;
        }
        if (event.key.keysym.scancode == SDL_SCANCODE_1) {
          draw_1 = !draw_1;
        }
        if (event.key.keysym.scancode == SDL_SCANCODE_2) {
          draw_2 = !draw_2;
        }
        if (event.key.keysym.scancode == SDL_SCANCODE_3) {
          draw_3 = !draw_3;
        }

        if (event.key.keysym.scancode == SDL_SCANCODE_A) {
          vec2_t b = vec2_rotate(&player.dir, PI / -2.0f);
          player.pos = vec2_add(&player.pos, &b);
        }
        if (event.key.keysym.scancode == SDL_SCANCODE_D) {
          vec2_t b = vec2_rotate(&player.dir, PI / 2.0f);
          player.pos = vec2_add(&player.pos, &b);
        }
        if (event.key.keysym.scancode == SDL_SCANCODE_W) {
          //vec2_t b = { 0.0f, -1.0f };
          //player.pos = vec2_add(&player.pos, &b);
          player.pos = vec2_add(&player.pos, &player.dir);
        }
        if (event.key.keysym.scancode == SDL_SCANCODE_S) {
          vec2_t b = vec2_mul(&player.dir, -1.0f);
          player.pos = vec2_add(&player.pos, &b);
        }
        if (event.key.keysym.scancode == SDL_SCANCODE_K) {
          player.dir = vec2_rotate(&player.dir, -PI / 4.0f);
        }
        if (event.key.keysym.scancode == SDL_SCANCODE_L) {
          player.dir = vec2_rotate(&player.dir, PI / 4.0f);
        }
        print_debug();
        break;
    }
  }
}

float edge_cross(vec2_t *a, vec2_t *b, vec2_t *p) {
  vec2_t ab = { b->x - a->x, b->y - a->y };
  vec2_t ap = { p->x - a->x, p->y - a->y };
  return ab.x * ap.y - ab.y * ap.x;
}

bool is_top_left(vec2_t *start, vec2_t *end) {
  vec2_t edge = { end->x - start->x, end->y - start->y };
  bool is_top_edge = edge.y == 0 && edge.x > 0;
  bool is_left_edge = edge.y < 0;
  return is_top_edge || is_left_edge;
}

void triangle_fill(vec2_t v0, vec2_t v1, vec2_t v2/*, uint32_t color*/) {
  // TODO: clamp these to the screen size?
  int x_min = fmin(fmin(v0.x, v1.x), v2.x);
  int y_min = fmin(fmin(v0.y, v1.y), v2.y);
  int x_max = fmax(fmax(v0.x, v1.x), v2.x);
  int y_max = fmax(fmax(v0.y, v1.y), v2.y);

  int bias0 = 0;
  int bias1 = 0;
  int bias2 = 0;
  if (fill_rule) {
    bias0 = is_top_left(&v1, &v2) ? 0 : -1;
    bias1 = is_top_left(&v2, &v0) ? 0 : -1;
    bias2 = is_top_left(&v0, &v1) ? 0 : -1;
  }

  float area = edge_cross(&v0, &v1, &v2);

  for (int y = y_min; y < y_max; ++y) {
    for (int x = x_min; x < x_max; ++x) {
      vec2_t p = { x, y };

      float w0 = edge_cross(&v1, &v2, &p) + bias0;
      float w1 = edge_cross(&v2, &v0, &p) + bias1;
      float w2 = edge_cross(&v0, &v1, &p) + bias2;

      bool is_inside = w0 >= 0 && w1 >= 0 && w2 >= 0;
      if (is_inside) {
        float alpha = w0 / area;
        float beta = w1 / area;
        float gamma = w2 / area;

        int a = 0xff;
        int r = alpha * colors[0].r + beta * colors[1].r + gamma * colors[2].r;
        int g = alpha * colors[0].g + beta * colors[1].g + gamma * colors[2].g;
        int b = alpha * colors[0].b + beta * colors[1].b + gamma * colors[2].b;

        uint32_t interp_color = 0x00000000;
        interp_color = (interp_color | a) << 8;
        interp_color = (interp_color | b) << 8;
        interp_color = (interp_color | g) << 8;
        interp_color = (interp_color | r);

        draw_pixel(x, y, interp_color);
      }
    }
  }
}

// void
// plot_line(int x0, int y0, int x1, int y1) {
//   int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
//   int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
//   int err = dx + dy, e2; /* error value e_xy */
// 
//   for (;;) {  /* loop */
//     setPixel(x0, y0);
//     if (x0 == x1 && y0 == y1) break;
//     e2 = 2 * err;
//     if (e2 >= dy) {
//       err += dy; x0 += sx;
//     } /* e_xy+e_x > 0 */
//     if (e2 <= dx) {
//       err += dx; y0 += sy;
//     } /* e_xy+e_y < 0 */
//   }
// }

void draw_line(int x0, int y0, int x1, int y1, uint32_t color) {
  int dx = abs(x1 - x0);
  int sx = x0 < x1 ? 1 : -1;
  int dy = -abs(y1 - y0);
  int sy = y0 < y1 ? 1 : -1;
  int error = dx + dy;
  int e2;

  for (;;) {
    draw_pixel(x0, y0, color);
    if (x0 == x1 && y0 == y1) {
      break;
    }

    e2 = 2 * error;

    if (e2 >= dy) {
      if (x0 == x1) {
        break;
      }
      error += dy;
      x0 += sx;
    }
    if (e2 <= dx) {
      if (y0 == y1) {
        break;
      }
      error += dx;
      y0 += sy;
    }
  }
}

void draw_circle(int cx, int cy, int r, uint32_t color) {
  int x = -r;
  int y = 0;
  int err = 2 - 2 * r;
  do {
    draw_pixel(cx - x, cy + y, color);
    draw_pixel(cx - y, cy - x, color);
    draw_pixel(cx + x, cy - y, color);
    draw_pixel(cx + y, cy + x, color);
    r = err;
    if (r > x) {
      err += ++x * 2 + 1;
    }
    if (r <= y) {
      err += ++y * 2 + 1;
    }
  } while (x < 0);
}

void render() {
  clear_framebuffer(0xFF000000);

  vec2_t v0 = vertices[0];
  vec2_t v1 = vertices[1];
  vec2_t v2 = vertices[2];
  vec2_t v3 = vertices[3];
  //vec2_t v4 = vertices[4];

  if (draw_1) {
    triangle_fill(v0, v1, v2);
  }
  if (draw_2) {
    triangle_fill(v3, v2, v1);
  }
  //if (draw_3) {
  //  triangle_fill(v4, v1, v0, 0xFF0000FF);
  //}


  // draw_line(v0.x, v0.y, v1.x, v1.y, 0xffffffff);
  // draw_line(v1.x, v1.y, v2.x, v2.y, 0xffffffff);
  // draw_line(v2.x, v2.y, v0.x, v0.y, 0xffffffff);

  vec2_t player_dir = vec2_mul(&player.dir, 4.0f);
  player_dir = vec2_add(&player.pos, &player_dir);
  //vec2_normalize(&player_dir);
  //player_dir = vec2_mul(&player_dir, 2.0f);
  draw_line(player.pos.x, player.pos.y, player_dir.x, player_dir.y, 0xffffffff);
  //draw_pixel(player.pos.x, player.pos.y, 0xff0000ff);
  draw_circle(player.pos.x, player.pos.y, 4, 0xff0000ff);

  render_framebuffer();
}

// int main(int argc, char *argv[]) {
//   is_running = create_window();
// 
//   while (is_running) {
//     fix_framerate();
//     process_input();
//     render();
//   }
// 
//   destroy_window();
// 
//   return 0;
// }
