#pragma once

#include <stdint.h>
#include <stdbool.h>

#define MAX_WINDOWS 128

typedef struct {
    int x, y;
    int width, height;
    uint32_t id;
    char title[64];
    bool focused;
    bool visible;
    void *buffer;  // raw RGBA or shared memory
} VLDWindow;

typedef struct {
    int x, y;
    int width, height;
    uint32_t *pixels; // bitmap cursor RGBA
} VLDCursor;

typedef struct {
    VLDWindow windows[MAX_WINDOWS];
    int window_count;
    int focused_index;
    VLDCursor cursor;
    // You can add more like background, wallpaper, cursor state, etc.
} VLDCompositor;




// Init the compositor system
void vldwc_compositor_init(VLDCompositor *comp);

// Handle new window request
uint32_t vldwc_create_window(VLDCompositor *comp, int width, int height, const char *title);

// Handle buffer updates
void vldwc_update_window_buffer(VLDCompositor *comp, uint32_t id, void *buffer);

// Handle input focus
void vldwc_focus_next(VLDCompositor *comp);

// Composite all windows to screen
void vldwc_render_all(VLDCompositor *comp);

// #endif
