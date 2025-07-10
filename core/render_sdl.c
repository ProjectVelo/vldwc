// File: core/render_sdl.c
#include <stdio.h>
#include <SDL2/SDL.h>
#include "compositor.h"
#include "background.h"
static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;
static SDL_Texture *wallpaper_texture = NULL;
static int win_width = 0, win_height = 0;

extern const unsigned char bg_bmp[];
extern const unsigned int bg_bmp_len;

void renderer_init() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("SDL_Init Error: %s\n", SDL_GetError());
        return;
    }

    // สร้าง window แบบ fullscreen desktop (ขนาดเต็มจอ เท่ากับ resolution ของจอ)
    window = SDL_CreateWindow("vldwc Compositor",
                              SDL_WINDOWPOS_UNDEFINED,
                              SDL_WINDOWPOS_UNDEFINED,
                              0, 0,
                              SDL_WINDOW_FULLSCREEN_DESKTOP);
    if (!window) {
        printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
        return;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        printf("SDL_CreateRenderer Error: %s\n", SDL_GetError());
        return;
    }

    SDL_GetWindowSize(window, &win_width, &win_height);

    // โหลด wallpaper จาก memory (ถ้ามี)
    SDL_RWops *rw = SDL_RWFromMem((void *)bg_bmp, bg_bmp_len);
    if (!rw) {
        printf("SDL_RWFromMem failed: %s\n", SDL_GetError());
        return;
    }

    SDL_Surface *surface = SDL_LoadBMP_RW(rw, 1);
    if (!surface) {
        printf("SDL_LoadBMP_RW failed: %s\n", SDL_GetError());
        return;
    }

    wallpaper_texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    if (!wallpaper_texture) {
        printf("SDL_CreateTextureFromSurface failed: %s\n", SDL_GetError());
    }
}




void render_window(VLDWindow *win) {
    if (!renderer || !win) return;

    SDL_Rect rect = {win->x, win->y, win->width, win->height};

    // สร้าง texture จาก buffer ของ window
    SDL_Texture *texture = SDL_CreateTexture(renderer,
                                             SDL_PIXELFORMAT_RGBA32,
                                             SDL_TEXTUREACCESS_STATIC,
                                             win->width, win->height);
    if (!texture) {
        printf("SDL_CreateTexture failed: %s\n", SDL_GetError());
        return;
    }

    SDL_UpdateTexture(texture, NULL, win->buffer, win->width * 4);

    // วาดหน้าต่าง
    SDL_RenderCopy(renderer, texture, NULL, &rect);

    // วาดกรอบถ้า focused
    if (win->focused) {
        SDL_SetRenderDrawColor(renderer, 0, 128, 255, 255); // สีน้ำเงิน
    } else {
        SDL_SetRenderDrawColor(renderer, 128, 128, 128, 255); // สีเทา
    }
    SDL_RenderDrawRect(renderer, &rect);

    SDL_DestroyTexture(texture);
}

// void vldwc_render_all(VLDCompositor *compositor) {
//     // เรียก render_window สำหรับแต่ละ window ใน compositor
//     for (int i = 0; i < compositor->window_count; i++) {
//         render_window(&compositor->windows[i]);
//     }
// }

void renderer_present() {
    if (!renderer) return;

    // ล้างหน้าจอด้วยสีดำ
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // วาด wallpaper ให้เต็มหน้าจอ
    if (wallpaper_texture) {
        SDL_RenderCopy(renderer, wallpaper_texture, NULL, NULL);
    }

    // วาดหน้าต่างอื่น ๆ ต่อที่นี่ (ถ้ามี)

    SDL_RenderPresent(renderer);
}

void renderer_cleanup() {
    if (wallpaper_texture) {
        SDL_DestroyTexture(wallpaper_texture);
        wallpaper_texture = NULL;
    }
    if (renderer) {
        SDL_DestroyRenderer(renderer);
        renderer = NULL;
    }
    if (window) {
        SDL_DestroyWindow(window);
        window = NULL;
    }
    SDL_Quit();
}
