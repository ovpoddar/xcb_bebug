#include <xcb/xcb.h>
#include <xcb/xproto.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    xcb_connection_t *conn;
    int screen_num;
    xcb_screen_t *screen;
    xcb_window_t wid;
    xcb_visualtype_t *visual = NULL;
    xcb_colormap_t cmid;
    xcb_gcontext_t gc;
    uint32_t white_pixel, black_pixel;
    int depth = 32;

    printf("xcb_connect \n");
    printf("****************************************************************\n");
    conn = xcb_connect(NULL, &screen_num);
    if (xcb_connection_has_error(conn)) {
        fprintf(stderr, "Cannot open display\n");
        return 1;
    }
    xcb_flush(conn);
    printf("****************************************************************\n");

    const xcb_setup_t *setup = xcb_get_setup(conn);
    xcb_screen_iterator_t screen_iter = xcb_setup_roots_iterator(setup);
    for (int i = 0; i < screen_num; ++i) {
        xcb_screen_next(&screen_iter);
    }
    screen = screen_iter.data;
    white_pixel = screen->white_pixel;
    black_pixel = screen->black_pixel;

    xcb_depth_iterator_t depth_iter = xcb_screen_allowed_depths_iterator(screen);
    for (; depth_iter.rem; xcb_depth_next(&depth_iter)) {
        if (depth_iter.data->depth == depth) {
            xcb_visualtype_iterator_t visual_iter = xcb_depth_visuals_iterator(depth_iter.data);
            for (; visual_iter.rem; xcb_visualtype_next(&visual_iter)) {
                if (visual_iter.data->_class == XCB_VISUAL_CLASS_TRUE_COLOR) {
                    visual = visual_iter.data;
                    break;
                }
            }
            if (visual) break;
        }
    }
    if (!visual) {
        fprintf(stderr, "No RGBA visual found\n");
        xcb_disconnect(conn);
        return 1;
    }

    cmid = xcb_generate_id(conn);
    printf("xcb_create_colormap \n");
    printf("****************************************************************\n");
    xcb_create_colormap(conn, XCB_COLORMAP_ALLOC_NONE, cmid, screen->root, visual->visual_id);
    xcb_flush(conn);
    printf("****************************************************************\n");

    wid = xcb_generate_id(conn);
    uint32_t mask = XCB_CW_BACK_PIXEL | XCB_CW_BORDER_PIXEL | XCB_CW_EVENT_MASK | XCB_CW_COLORMAP;
    uint32_t values[] = {0, 0, XCB_EVENT_MASK_EXPOSURE, cmid};
    printf("xcb_create_window \n");
    printf("****************************************************************\n");
    xcb_create_window(conn, depth, wid, screen->root, 10, 10, 168, 195, 1,
                     XCB_WINDOW_CLASS_INPUT_OUTPUT, visual->visual_id, mask, values);
    xcb_flush(conn);
    printf("****************************************************************\n");

    printf("xcb_map_window \n");
    printf("****************************************************************\n");
    xcb_map_window(conn, wid);
    xcb_flush(conn);
    printf("****************************************************************\n");

    gc = xcb_generate_id(conn);
    uint32_t gc_mask = XCB_GC_FOREGROUND | XCB_GC_BACKGROUND;
    uint32_t gc_values[] = {black_pixel, white_pixel};
    printf("xcb_create_gc \n");
    printf("****************************************************************\n");
    xcb_create_gc(conn, gc, wid, gc_mask, gc_values);
    xcb_flush(conn);
    printf("****************************************************************\n");
    
    xcb_flush(conn);
    xcb_point_t points[] = {
        {10, 10}, {1430, 10}, {1430, 868}, {10, 868}, {10, 10}
    };
    while (1) {
        xcb_poly_line(conn, XCB_COORD_MODE_ORIGIN, wid, gc, 5, points);
        xcb_flush(conn);
        sleep(10); // Sleep for 10 seconds
    }

    xcb_free_gc(conn, gc);
    xcb_free_colormap(conn, cmid);
    xcb_disconnect(conn);
    return 0;
}
