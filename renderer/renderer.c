// renderer/renderer.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "renderer.h"

void renderer_init() {
    printf("[renderer] Initialized (dummy)\n");
}

void render_window(int x, int y, int w, int h, void *buffer, int focused) {
    printf("[renderer] Render window at (%d,%d) size %dx%d %s\n",
        x, y, w, h, focused ? "[FOCUSED]" : "");
}

void renderer_present() {
    printf("[renderer] Present frame\n");
}
