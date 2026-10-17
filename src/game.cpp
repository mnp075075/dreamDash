#include <kos.h>
#include <dc/pvr.h>
#include <kos/img.h>
#include <kmg/kmg.h>
#include <png/png.h>
#include <zlib/zlib.h>
#include <unistd.h>
#include <cstdio>
#include <cmath>
#include <vector>
#include <string>

pvr_ptr_t image = nullptr;
pvr_poly_cxt_t context;
pvr_poly_hdr_t texture;

void init_image(std::string path, int mode, int width, int height) {
    image = pvr_mem_malloc(512*512*2);
    int result = png_to_texture(path.c_str(), image, mode);
    dbglog(DBG_INFO, "PNG load result: %d\n", result);
}

void render_texture(pvr_poly_cxt_t &context, pvr_poly_hdr_t &texture, pvr_list_t mode, int format, int flags, pvr_filter_mode_t filter, float x, float y, std::vector<float> z, std::vector<float> u, std::vector<float> v, float width, float height, uint32_t color) {
    pvr_poly_cxt_txr(&context, mode, format, width, height, image, filter);
    pvr_poly_compile(&texture, &context);

    pvr_prim(&texture, sizeof(texture));

    pvr_vertex_t vert;
    
    vert.flags = flags;
    vert.argb = color;
    vert.oargb = 0;

    for (size_t i = 0; i < 4; i++) {
        if (i == 3) vert.flags = PVR_CMD_VERTEX_EOL;
        vert.x = (i % 2 == 0) ? x : x+width;
        vert.y = ((i / 2) % 2 == 0) ? y : y+height;
        vert.z = z[i];
        vert.u = u[i];
        vert.v = v[i];
        pvr_prim(&vert, sizeof(vert));
    }
}

int main() {
    pvr_init_defaults();

    std::vector<float> z_axis = {1.0f, 1.0f, 1.0f, 1.0f};
    std::vector<float> u_axis, v_axis;
   
    for (size_t i = 0; i < 4; i++) {
        u_axis.push_back((i % 2 == 0) ? 0.0f : 1.0f);
        v_axis.push_back(((i / 2) % 2 == 0) ? 0.0f : 1.0f);
    }

    init_image("/rd/background.png", PNG_NO_ALPHA, 512, 512);

    if (!image) {
        dbglog(DBG_ERROR, "[ERROR] Image not loaded, exiting...");
        return -1;
    }

    while (true) {
        pvr_wait_ready();
        pvr_scene_begin();
        pvr_list_begin(PVR_LIST_OP_POLY);

        render_texture(context, texture, PVR_LIST_OP_POLY, PVR_TXRFMT_RGB565, PVR_CMD_VERTEX, PVR_FILTER_NONE, 0.0f, 0.0f, z_axis, u_axis, v_axis, 512.0f, 512.0f, 0xFFFFFFFF);

        pvr_list_finish();
        pvr_scene_finish();
    }
    return 0;
}