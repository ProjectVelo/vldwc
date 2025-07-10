#pragma once

void renderer_init();
void render_window(int x, int y, int w, int h, void *buffer, int focused);
void renderer_present();
void renderer_cleanup();
