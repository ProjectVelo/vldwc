// File: core/compositor.c
#include <SDL2/SDL.h>
#include <string.h>
#include <stdio.h>
#include "compositor.h"
#include "../renderer/renderer.h" // <-- later!

#include <stdlib.h>


void vldwc_compositor_init(VLDCompositor *comp) {
    comp->window_count = 0;
    comp->focused_index = -1;
}

uint32_t vldwc_create_window(VLDCompositor *comp, int width, int height, const char *title) {
    if (comp->window_count >= MAX_WINDOWS) return -1;

    uint32_t id = comp->window_count;
    VLDWindow *win = &comp->windows[id];

    win->x = 100 + id * 20;
    win->y = 100 + id * 20;
    win->width = width;
    win->height = height;
    win->id = id;
    strncpy(win->title, title, sizeof(win->title) - 1);
    win->visible = true;
    win->focused = (id == 0); // first window gets focus
    win->buffer = calloc(width * height, 4); // 32-bit RGBA

    comp->window_count++;
    if (win->focused)
        comp->focused_index = id;

    printf("[vldwc] Created window %u (%s)\n", id, title);
    return id;
}

void vldwc_update_window_buffer(VLDCompositor *comp, uint32_t id, void *buffer) {
    if (id >= comp->window_count) return;

    VLDWindow *win = &comp->windows[id];
    memcpy(win->buffer, buffer, win->width * win->height * 4);
}

void vldwc_focus_next(VLDCompositor *comp) {
    if (comp->window_count == 0) return;

    comp->windows[comp->focused_index].focused = false;
    comp->focused_index = (comp->focused_index + 1) % comp->window_count;
    comp->windows[comp->focused_index].focused = true;
    printf("[vldwc] Focused window %u\n", comp->focused_index);
}

void vldwc_render_all(VLDCompositor *comp) {
    for (int i = 0; i < comp->window_count; i++) {
        VLDWindow *win = &comp->windows[i];
        if (!win->visible) continue;

        render_window(win->x, win->y, win->width, win->height, win->buffer, win->focused);
    }
}

void vldwc_cursor_init(VLDCompositor *comp) {
    comp->cursor.x = 0;
    comp->cursor.y = 0;
    comp->cursor.width = 16;
    comp->cursor.height = 16;
    comp->cursor.pixels = malloc(sizeof(uint32_t) * 16 * 16);

    // ตัวอย่าง cursor สีขาวล้วน (หรือจะโหลด bitmap มา)
    for (int i = 0; i < 16*16; i++) {
        comp->cursor.pixels[i] = 0xFFFFFFFF; // สีขาวเต็ม
    }
}

void vldwc_cursor_move(VLDCompositor *comp, int dx, int dy) {
    comp->cursor.x += dx;
    comp->cursor.y += dy;
    if (comp->cursor.x < 0) comp->cursor.x = 0;
    if (comp->cursor.y < 0) comp->cursor.y = 0;
    // limit max ตามหน้าจอ (ใส่ได้ถ้ามีขนาดหน้าจอใน compositor)
}

// render cursor ลงบน framebuffer หรือ SDL renderer
void render_cursor(SDL_Renderer *renderer, VLDCursor *cursor) {
    // ทำเป็น texture หรือใช้ SDL_RenderDrawPoint วาดแบบง่ายๆ ก็ได้
    SDL_Rect rect = {cursor->x, cursor->y, cursor->width, cursor->height};
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &rect);
}