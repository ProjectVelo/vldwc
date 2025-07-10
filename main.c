// File: main.c
#include <string.h>
#include <SDL2/SDL.h>
#include "core/compositor.h"
#include "core/render_sdl.h"  // เปลี่ยนตามโครงสร้างไฟล์จริง
#include "core/background.h"

int main() {
    VLDCompositor compositor;
    vldwc_compositor_init(&compositor);
    renderer_init();

    // ไม่ต้องสร้าง window ย่อยแล้ว

    SDL_Event event;
    int running = 1;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            }
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
                running = 0;
            }
            if (event.type == SDL_MOUSEMOTION) {
                int dx = event.motion.xrel;
                int dy = event.motion.yrel;
                vldwc_cursor_move(&compositor, dx, dy);
            }
        }

        // วาด wallpaper เต็มจอเลย (ใน render_sdl.c)
        renderer_present();

        SDL_Delay(16); // ~60fps
    }

    renderer_cleanup();
    return 0;
}
